#include "ChoppingBlock.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_Flower.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_Mushroom.hpp" // Included Mushroom
#include "GameAssembly/WH/Tools/Knife.hpp"
#include <Termina/World/World.hpp>
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Core/Logger.hpp>
#include <ImGui/imgui.h>

namespace TerminaScript {

    void ChoppingBlock::OnUpdate(float deltaTime) {
        auto* world = m_Owner->GetParentWorld();

        Termina::Actor* itemPos = world->GetActorByName("chopping_itemPos");
        Termina::Actor* intakeBox = world->GetActorByName("chopping_IntakeBox");
        if (!itemPos || !intakeBox) return;

        // --- PART A: SLOT MAINTENANCE ---
        if (m_SlottedItemID != 0) {
            Termina::Actor* currentItem = GetSlottedItem();
            if (currentItem == nullptr) {
                m_SlottedItemID = 0;
            }
        }

        glm::vec3 hitboxPos = intakeBox->GetComponent<Termina::Transform>().GetPosition();

        // --- PART B: THE SENSORS (Impact Check) ---
        for (const auto& actorPtr : world->GetActors()) {
            Termina::Actor* actor = actorPtr.get();

            if (!actor || actor == m_Owner) continue;

            if (IsPlayerHolding(actor)) continue;

            glm::vec3 actorPos = actor->GetComponent<Termina::Transform>().GetPosition();
            float distToHitbox = glm::length(actorPos - hitboxPos);

            // 1. KNIFE SENSOR (Reduced range from 1.0f to 0.4f for tighter, realistic cutting)
            if (distToHitbox < 0.4f && actor->HasComponent<Knife>() && m_SlottedItemID != 0) {
                PerformChop(actor);
                break;
            }

            // 2. INGREDIENT SENSOR
            if (m_SlottedItemID == 0 && distToHitbox < 0.8f) {
                if (IsRawIngredient(actor)) {
                    m_SlottedItemID = actor->GetID();

                    actor->DetachFromParent();
                    itemPos->AttachChild(actor);

                    auto& tr = actor->GetComponent<Termina::Transform>();
                    tr.SetPosition(itemPos->GetComponent<Termina::Transform>().GetPosition());
                    tr.SetRotation(itemPos->GetComponent<Termina::Transform>().GetRotation());

                    if (actor->HasComponent<Termina::Rigidbody>()) {
                        auto& rb = actor->GetComponent<Termina::Rigidbody>();
                        rb.Type = Termina::Rigidbody::BodyType::Static;
                        rb.SetLinearVelocity(glm::vec3(0));
                    }
                    TN_INFO("Board: Item hit the block and docked!");
                    break;
                }
            }
        }
    }

    void ChoppingBlock::PerformChop(Termina::Actor* knifeActor) {
        Termina::Actor* rawItem = GetSlottedItem();
        if (!rawItem) return;

        std::string prefabPath = GetCutPrefabPath(rawItem);
        if (prefabPath.empty()) return;

        auto* world = m_Owner->GetParentWorld();

        // 1. Spawn the new Cut Item
        Termina::Actor* cutItem = world->SpawnActorFromJSON(prefabPath);

        if (cutItem) {
            auto& cutTr = cutItem->GetComponent<Termina::Transform>();
            Termina::Actor* resultPos = world->GetActorByName("chopping_resultPos");

            // Teleport to the result area if it exists
            if (resultPos) {
                cutTr.SetPosition(resultPos->GetComponent<Termina::Transform>().GetPosition());
                cutTr.SetRotation(resultPos->GetComponent<Termina::Transform>().GetRotation());
            }
            else {
                // Fallback just in case
                cutTr.SetPosition(rawItem->GetComponent<Termina::Transform>().GetPosition() + glm::vec3(0, 0.3f, 0));
            }

            if (cutItem->HasComponent<Termina::Rigidbody>()) {
                auto& rb = cutItem->GetComponent<Termina::Rigidbody>();
                rb.Type = Termina::Rigidbody::BodyType::Dynamic;
                rb.SetLinearVelocity(glm::vec3(0, 2.0f, 1.0f)); // Slightly reduced pop-out velocity
            }
        }

        // --- THE "GRAVEYARD" DOCKING HACK ---
        m_SlottedItemID = 0;

        Termina::Actor* hidePos = world->GetActorByName("hide_Pos");
        if (hidePos) {
            rawItem->DetachFromParent();
            hidePos->AttachChild(rawItem);

            auto& tr = rawItem->GetComponent<Termina::Transform>();
            tr.SetPosition(hidePos->GetComponent<Termina::Transform>().GetPosition());
            tr.SetRotation(hidePos->GetComponent<Termina::Transform>().GetRotation());

            if (rawItem->HasComponent<Termina::Rigidbody>()) {
                auto& rb = rawItem->GetComponent<Termina::Rigidbody>();
                rb.SetLinearVelocity(glm::vec3(0.0f));
            }

            auto children = rawItem->GetChildren();
            for (const auto& child : children) {
                if (child && child->HasComponent<Termina::Rigidbody>()) {
                    auto& childRb = child->GetComponent<Termina::Rigidbody>();
                    childRb.SetLinearVelocity(glm::vec3(0.0f));
                }
            }

            TN_INFO("Board: Item Chopped! Old item successfully docked to hide_Pos.");
        }
        else {
            TN_ERROR("Board: hide_Pos not found! Make sure it is named exactly 'hide_Pos' in the world.");
        }
    }

    Termina::Actor* ChoppingBlock::GetSlottedItem() const {
        if (m_SlottedItemID == 0) return nullptr;
        return m_Owner->GetParentWorld()->GetActorById(m_SlottedItemID);
    }

    void ChoppingBlock::ClearSlot() {
        m_SlottedItemID = 0;
    }

    void ChoppingBlock::Inspect() {
        ImGui::Text("Slot ID: %llu", m_SlottedItemID);
        if (m_SlottedItemID != 0) ImGui::Text("STATUS: OCCUPIED");
        else ImGui::Text("STATUS: EMPTY");
    }

    bool ChoppingBlock::IsRawIngredient(Termina::Actor* actor) {
        if (!actor) return false;

        // We completely removed the Enum states! 
        // Now, just having the base Ingre_Flower or Ingre_Mushroom component 
        // means it IS the raw, uncut version.
        if (actor->HasComponent<Ingre_Flower>()) return true;
        if (actor->HasComponent<Ingre_Mushroom>()) return true;

        return false;
    }

    bool ChoppingBlock::IsPlayerHolding(Termina::Actor* actor) {
        if (!actor) return false;
        auto* parent = actor->GetParent();
        return (parent != nullptr && parent->GetName() == "GrabbedPos");
    }

    std::string ChoppingBlock::GetCutPrefabPath(Termina::Actor* actor) {
        if (!actor) return "";

        if (actor->HasComponent<Ingre_Flower>()) {
            return actor->GetComponent<Ingre_Flower>().CutPrefabName;
        }

        if (actor->HasComponent<Ingre_Mushroom>()) {
            return actor->GetComponent<Ingre_Mushroom>().CutPrefabName;
        }

        return "";
    }
}
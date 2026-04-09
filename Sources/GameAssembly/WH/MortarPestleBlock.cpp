#include "MortarPestleBlock.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_Flower.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_CutFlower.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_Mushroom.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_CutMushroom.hpp"
#include "GameAssembly/WH/Tools/Pestle.hpp"
#include <Termina/World/World.hpp>
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Core/Logger.hpp>
#include <ImGui/imgui.h>

namespace TerminaScript {

    void MortarPestleBlock::OnUpdate(float deltaTime) {
        auto* world = m_Owner->GetParentWorld();

        Termina::Actor* itemPos = world->GetActorByName("mortarpestle_itemPos");
        Termina::Actor* intakeBox = world->GetActorByName("mortarpestle_IntakeBox");
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

            // EVERYTHING requires the player to let go first! Throwing only!
            if (IsPlayerHolding(actor)) continue;

            glm::vec3 actorPos = actor->GetComponent<Termina::Transform>().GetPosition();
            float distToHitbox = glm::length(actorPos - hitboxPos);

            // 1. PESTLE SENSOR (Generous 1.2f range so throwing is fun, not frustrating)
            if (distToHitbox < 1.2f && actor->HasComponent<Pestle>() && m_SlottedItemID != 0) {

                // The Pestle MUST be moving (thrown/falling) to crush.
                // This stops the "resting auto-crush" bug if the pestle is just laying in the bowl.
                bool isStriking = true;
                if (actor->HasComponent<Termina::Rigidbody>()) {
                    auto& rb = actor->GetComponent<Termina::Rigidbody>();
                    float speed = glm::length(rb.GetLinearVelocity());

                    if (speed < 0.5f) {
                        isStriking = false;
                    }
                }

                if (isStriking) {
                    PerformCrush(actor);
                    break;
                }
            }

            // 2. INGREDIENT SENSOR (Generous 1.0f range for throwing ingredients)
            if (m_SlottedItemID == 0 && distToHitbox < 1.0f) {
                if (IsCrushableIngredient(actor)) {
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
                    TN_INFO("Mortar: Item hit the block and docked!");
                    break;
                }
            }
        }
    }

    void MortarPestleBlock::PerformCrush(Termina::Actor* pestleActor) {
        Termina::Actor* rawItem = GetSlottedItem();
        if (!rawItem) return;

        std::string prefabPath = GetCrushPrefabPath(rawItem);
        if (prefabPath.empty()) return;

        auto* world = m_Owner->GetParentWorld();

        // 1. Spawn the new Crushed Item
        Termina::Actor* crushedItem = world->SpawnActorFromJSON(prefabPath);

        if (crushedItem) {
            auto& crushTr = crushedItem->GetComponent<Termina::Transform>();
            // Updated to match your exact naming convention
            Termina::Actor* resultPos = world->GetActorByName("mortarpestle_resultPos");

            if (resultPos) {
                crushTr.SetPosition(resultPos->GetComponent<Termina::Transform>().GetPosition());
                crushTr.SetRotation(resultPos->GetComponent<Termina::Transform>().GetRotation());
            }
            else {
                crushTr.SetPosition(rawItem->GetComponent<Termina::Transform>().GetPosition() + glm::vec3(0, 0.3f, 0));
            }

            if (crushedItem->HasComponent<Termina::Rigidbody>()) {
                auto& rb = crushedItem->GetComponent<Termina::Rigidbody>();
                rb.Type = Termina::Rigidbody::BodyType::Dynamic;
                rb.SetLinearVelocity(glm::vec3(0, 2.0f, 1.0f));
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

            TN_INFO("Mortar: Item Crushed! Old item successfully docked to hide_Pos.");
        }
        else {
            TN_ERROR("Mortar: hide_Pos not found! Make sure it is named exactly 'hide_Pos' in the world.");
        }
    }

    Termina::Actor* MortarPestleBlock::GetSlottedItem() const {
        if (m_SlottedItemID == 0) return nullptr;
        return m_Owner->GetParentWorld()->GetActorById(m_SlottedItemID);
    }

    void MortarPestleBlock::ClearSlot() {
        m_SlottedItemID = 0;
    }

    void MortarPestleBlock::Inspect() {
        ImGui::Text("Slot ID: %llu", m_SlottedItemID);
        if (m_SlottedItemID != 0) ImGui::Text("STATUS: OCCUPIED");
        else ImGui::Text("STATUS: EMPTY");
    }

    bool MortarPestleBlock::IsCrushableIngredient(Termina::Actor* actor) {
        if (!actor) return false;

        // ONLY accept RAW ingredients. If it's already cut or crushed, it will be ignored.
        if (actor->HasComponent<Ingre_Flower>()) return true;
        if (actor->HasComponent<Ingre_Mushroom>()) return true;

        return false;
    }

    bool MortarPestleBlock::IsPlayerHolding(Termina::Actor* actor) {
        if (!actor) return false;
        auto* parent = actor->GetParent();
        return (parent != nullptr && parent->GetName() == "GrabbedPos");
    }

    std::string MortarPestleBlock::GetCrushPrefabPath(Termina::Actor* actor) {
        if (!actor) return "";

        if (actor->HasComponent<Ingre_Flower>()) return actor->GetComponent<Ingre_Flower>().CrushPrefabName;
        if (actor->HasComponent<Ingre_Mushroom>()) return actor->GetComponent<Ingre_Mushroom>().CrushPrefabName;

        return "";
    }
}
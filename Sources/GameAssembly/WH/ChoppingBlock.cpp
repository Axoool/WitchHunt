#include "ChoppingBlock.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_Flower.hpp"
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
        if (!itemPos) return;

        glm::vec3 targetPos = itemPos->GetComponent<Termina::Transform>().GetPosition();

        // --- PART A: SLOT MAINTENANCE ---
        if (m_SlottedItemID != 0) {
            Termina::Actor* currentItem = GetSlottedItem();
            if (currentItem == nullptr) {
                m_SlottedItemID = 0;
            }
        }

        // --- PART B: THE SENSORS (Proximity Check) ---
        // FIX: world->GetActors() returns shared_ptr. Use .get() to get the raw pointer.
        for (const auto& actorPtr : world->GetActors()) {
            Termina::Actor* actor = actorPtr.get(); // Access the actual Actor pointer

            if (!actor || actor == m_Owner) continue;

            float dist = glm::distance(actor->GetComponent<Termina::Transform>().GetPosition(), targetPos);

            // 1. KNIFE SENSOR: If a knife gets close and we have an item, CHOP!
            if (dist < 0.7f && actor->HasComponent<Knife>() && m_SlottedItemID != 0) {
                PerformChop(actor);
                break;
            }

            // 2. INGREDIENT SENSOR: Catching the throw
            if (m_SlottedItemID == 0 && dist < 0.5f) {
                if (IsRawIngredient(actor) && !IsPlayerHolding(actor)) {
                    m_SlottedItemID = actor->GetID();

                    actor->DetachFromParent();
                    itemPos->AttachChild(actor);

                    auto& tr = actor->GetComponent<Termina::Transform>();
                    tr.SetPosition(targetPos);
                    tr.SetRotation(itemPos->GetComponent<Termina::Transform>().GetRotation());

                    if (actor->HasComponent<Termina::Rigidbody>()) {
                        auto& rb = actor->GetComponent<Termina::Rigidbody>();
                        rb.Type = Termina::Rigidbody::BodyType::Static;
                        rb.SetLinearVelocity(glm::vec3(0));
                    }
                    TN_INFO("Board: Caught flying ingredient!");
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
        Termina::Actor* cutItem = world->SpawnActorFromJSON(prefabPath);

        if (cutItem) {
            auto& cutTr = cutItem->GetComponent<Termina::Transform>();
            cutTr.SetPosition(rawItem->GetComponent<Termina::Transform>().GetPosition() + glm::vec3(0, 0.3f, 0));

            if (cutItem->HasComponent<Termina::Rigidbody>()) {
                auto& rb = cutItem->GetComponent<Termina::Rigidbody>();
                rb.Type = Termina::Rigidbody::BodyType::Dynamic;
                rb.SetLinearVelocity(glm::vec3(0, 4.0f, 2.0f));
            }
        }

        m_SlottedItemID = 0;
        rawItem->DetachFromParent();
        rawItem->GetComponent<Termina::Transform>().SetPosition(glm::vec3(0, -9999, 0));
        world->DestroyActor(rawItem);
        TN_INFO("Board: Item Chopped!");
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
        if (actor->HasComponent<Ingre_Flower>()) {
            return actor->GetComponent<Ingre_Flower>().CurrentState == Ingre_Flower::State::Unprocessed;
        }
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
        return "";
    }
}
#include "CauldronBlock.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_Flower.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_CutFlower.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_Mushroom.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_CutMushroom.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_CrushedFlower.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_CrushedMushroom.hpp"
#include <Termina/World/World.hpp>
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Core/Logger.hpp>
#include <ImGui/imgui.h>

namespace TerminaScript {

    void CauldronBlock::OnUpdate(float deltaTime) {
        auto* world = m_Owner->GetParentWorld();
        Termina::Actor* intakeBox = world->GetActorByName("cauldron_IntakeBox");
        if (!intakeBox) return;

        // Clean up the list in case an item was destroyed or removed
        m_SlottedItems.erase(std::remove_if(m_SlottedItems.begin(), m_SlottedItems.end(),
            [world](uint64_t id) { return world->GetActorById(id) == nullptr; }), m_SlottedItems.end());

        // If cauldron is full (3 items max), stop scanning for new ones
        if (m_SlottedItems.size() >= 3) return;

        glm::vec3 hitboxPos = intakeBox->GetComponent<Termina::Transform>().GetPosition();

        for (const auto& actorPtr : world->GetActors()) {
            Termina::Actor* actor = actorPtr.get();
            if (!actor || actor == m_Owner) continue;

            // Must be thrown, and cannot already be in the cauldron!
            if (IsPlayerHolding(actor)) continue;
            if (std::find(m_SlottedItems.begin(), m_SlottedItems.end(), actor->GetID()) != m_SlottedItems.end()) continue;

            glm::vec3 actorPos = actor->GetComponent<Termina::Transform>().GetPosition();
            float distToHitbox = glm::length(actorPos - hitboxPos);

            // Generous 1.2f range for throwing
            if (distToHitbox < 1.2f && IsAnyIngredient(actor)) {

                // 1. Add it to our list
                m_SlottedItems.push_back(actor->GetID());
                size_t currentSlotIndex = m_SlottedItems.size(); // 1, 2, or 3

                // 2. Find the correct pos based on how many items are in
                std::string posName = "cauldron_itemPos" + std::to_string(currentSlotIndex);
                Termina::Actor* targetPos = world->GetActorByName(posName);

                if (targetPos) {
                    actor->DetachFromParent();
                    targetPos->AttachChild(actor);

                    auto& tr = actor->GetComponent<Termina::Transform>();
                    tr.SetPosition(targetPos->GetComponent<Termina::Transform>().GetPosition());
                    tr.SetRotation(targetPos->GetComponent<Termina::Transform>().GetRotation());

                    // ADD THIS LINE: Shrink to 0.5 scale
                    tr.SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

                    if (actor->HasComponent<Termina::Rigidbody>()) {
                        auto& rb = actor->GetComponent<Termina::Rigidbody>();
                        rb.Type = Termina::Rigidbody::BodyType::Static;
                        rb.SetLinearVelocity(glm::vec3(0));
                    }
                    TN_INFO("Cauldron: Item docked at %s and shrunk!", posName.c_str());
                }
                else {
                    TN_ERROR("Cauldron: Could not find %s", posName.c_str());
                }

                // Break to only process one ingredient per frame
                break;
            }
        }
    }

    Termina::Actor* CauldronBlock::PopLastIngredient() {
        if (m_SlottedItems.empty()) return nullptr;

        auto* world = m_Owner->GetParentWorld();
        uint64_t lastItemID = m_SlottedItems.back();
        Termina::Actor* itemToReturn = world->GetActorById(lastItemID);

        // Remove it from our list
        m_SlottedItems.pop_back();

        if (itemToReturn) {
            itemToReturn->DetachFromParent();

            // ADD THIS: Restore normal size
            auto& tr = itemToReturn->GetComponent<Termina::Transform>();
            tr.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

            // Wake physics back up
            if (itemToReturn->HasComponent<Termina::Rigidbody>()) {
                auto& rb = itemToReturn->GetComponent<Termina::Rigidbody>();
                rb.Type = Termina::Rigidbody::BodyType::Dynamic;
            }
            TN_INFO("Cauldron: Item popped and restored to normal size.");
        }

        return itemToReturn;
    }

    void CauldronBlock::Inspect() {
        ImGui::Text("Items in Cauldron: %zu / 3", m_SlottedItems.size());

        for (size_t i = 0; i < m_SlottedItems.size(); i++) {
            auto* item = m_Owner->GetParentWorld()->GetActorById(m_SlottedItems[i]);
            if (item) {
                ImGui::Text("Slot %zu: %s", i + 1, item->GetName().c_str());
            }
        }
    }

    bool CauldronBlock::IsPlayerHolding(Termina::Actor* actor) {
        if (!actor) return false;
        auto* parent = actor->GetParent();
        return (parent != nullptr && parent->GetName() == "GrabbedPos");
    }

    // Accepts ALL forms of ingredients (Raw, Cut, and Crushed!)
    bool CauldronBlock::IsAnyIngredient(Termina::Actor* actor) {
        if (!actor) return false;

        if (actor->HasComponent<Ingre_Flower>()) return true;
        if (actor->HasComponent<Ingre_CutFlower>()) return true;
        if (actor->HasComponent<Ingre_CrushedFlower>()) return true; // ADDED

        if (actor->HasComponent<Ingre_Mushroom>()) return true;
        if (actor->HasComponent<Ingre_CutMushroom>()) return true;
        if (actor->HasComponent<Ingre_CrushedMushroom>()) return true; // ADDED

        return false;
    }
}
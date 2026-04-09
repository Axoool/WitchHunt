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

#include <algorithm> // Required for std::sort

namespace TerminaScript {

    // ==========================================
    // 🧪 RECIPE BOOK
    // Add all your new potions right here!
    // ==========================================
    const std::vector<Recipe> CAULDRON_RECIPES = {
        {
            // Damage Potion: 1 Cut Mushroom + 1 Crushed Mushroom
            { IngredientType::CutMushroom, IngredientType::CrushedMushroom },
            "Assets\\Prefabs\\Grabbable\\Potions\\potion_dmg.trp"
        }
        // Example of a 3-item recipe you could add later:
        // { { IngredientType::CutFlower, IngredientType::CrushedFlower, IngredientType::RawMushroom }, "Assets\\Prefabs\\Grabbable\\Potions\\potion_heal.trp" }
    };

    void CauldronBlock::OnUpdate(float deltaTime) {
        auto* world = m_Owner->GetParentWorld();
        Termina::Actor* intakeBox = world->GetActorByName("cauldron_IntakeBox");
        if (!intakeBox) return;

        m_SlottedItems.erase(std::remove_if(m_SlottedItems.begin(), m_SlottedItems.end(),
            [world](uint64_t id) { return world->GetActorById(id) == nullptr; }), m_SlottedItems.end());

        if (m_SlottedItems.size() >= 3) return;

        glm::vec3 hitboxPos = intakeBox->GetComponent<Termina::Transform>().GetPosition();

        for (const auto& actorPtr : world->GetActors()) {
            Termina::Actor* actor = actorPtr.get();
            if (!actor || actor == m_Owner) continue;

            if (IsPlayerHolding(actor)) continue;
            if (std::find(m_SlottedItems.begin(), m_SlottedItems.end(), actor->GetID()) != m_SlottedItems.end()) continue;

            glm::vec3 actorPos = actor->GetComponent<Termina::Transform>().GetPosition();
            float distToHitbox = glm::length(actorPos - hitboxPos);

            if (distToHitbox < 1.2f && IsAnyIngredient(actor)) {
                m_SlottedItems.push_back(actor->GetID());
                size_t currentSlotIndex = m_SlottedItems.size();

                std::string posName = "cauldron_itemPos" + std::to_string(currentSlotIndex);
                Termina::Actor* targetPos = world->GetActorByName(posName);

                if (targetPos) {
                    actor->DetachFromParent();
                    targetPos->AttachChild(actor);

                    auto& tr = actor->GetComponent<Termina::Transform>();
                    tr.SetPosition(targetPos->GetComponent<Termina::Transform>().GetPosition());
                    tr.SetRotation(targetPos->GetComponent<Termina::Transform>().GetRotation());
                    tr.SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

                    if (actor->HasComponent<Termina::Rigidbody>()) {
                        auto& rb = actor->GetComponent<Termina::Rigidbody>();
                        rb.Type = Termina::Rigidbody::BodyType::Static;
                        rb.SetLinearVelocity(glm::vec3(0));
                    }
                    TN_INFO("Cauldron: Item docked at %s!", posName.c_str());

                    // --- NEW: Check if a potion can be made after adding this item! ---
                    CheckRecipes();
                }
                break;
            }
        }
    }

    Termina::Actor* CauldronBlock::PopLastIngredient() {
        if (m_SlottedItems.empty()) return nullptr;
        auto* world = m_Owner->GetParentWorld();
        uint64_t lastItemID = m_SlottedItems.back();
        Termina::Actor* itemToReturn = world->GetActorById(lastItemID);

        m_SlottedItems.pop_back();

        if (itemToReturn) {
            itemToReturn->DetachFromParent();
            auto& tr = itemToReturn->GetComponent<Termina::Transform>();
            tr.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

            if (itemToReturn->HasComponent<Termina::Rigidbody>()) {
                auto& rb = itemToReturn->GetComponent<Termina::Rigidbody>();
                rb.Type = Termina::Rigidbody::BodyType::Dynamic;
            }
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

    bool CauldronBlock::IsAnyIngredient(Termina::Actor* actor) {
        if (!actor) return false;
        if (actor->HasComponent<Ingre_Flower>()) return true;
        if (actor->HasComponent<Ingre_CutFlower>()) return true;
        if (actor->HasComponent<Ingre_CrushedFlower>()) return true;
        if (actor->HasComponent<Ingre_Mushroom>()) return true;
        if (actor->HasComponent<Ingre_CutMushroom>()) return true;
        if (actor->HasComponent<Ingre_CrushedMushroom>()) return true;
        return false;
    }

    // --- NEW CRAFTING LOGIC ---

    IngredientType CauldronBlock::GetIngredientTypeFromActor(Termina::Actor* actor) {
        if (!actor) return IngredientType::None;
        if (actor->HasComponent<Ingre_Flower>()) return IngredientType::RawFlower;
        if (actor->HasComponent<Ingre_CutFlower>()) return IngredientType::CutFlower;
        if (actor->HasComponent<Ingre_CrushedFlower>()) return IngredientType::CrushedFlower;
        if (actor->HasComponent<Ingre_Mushroom>()) return IngredientType::RawMushroom;
        if (actor->HasComponent<Ingre_CutMushroom>()) return IngredientType::CutMushroom;
        if (actor->HasComponent<Ingre_CrushedMushroom>()) return IngredientType::CrushedMushroom;
        return IngredientType::None;
    }

    void CauldronBlock::CheckRecipes() {
        if (m_SlottedItems.empty()) return;

        auto* world = m_Owner->GetParentWorld();
        std::vector<IngredientType> currentContents;

        // 1. Convert slotted Actor IDs into an Enum list
        for (uint64_t id : m_SlottedItems) {
            Termina::Actor* item = world->GetActorById(id);
            if (item) {
                currentContents.push_back(GetIngredientTypeFromActor(item));
            }
        }

        // 2. Sort the current contents alphabetically (by Enum value) to ignore throw order
        std::sort(currentContents.begin(), currentContents.end());

        // 3. Compare against our Recipe Book
        for (const auto& recipe : CAULDRON_RECIPES) {
            std::vector<IngredientType> recipeRequirements = recipe.Inputs;

            // Sort the recipe requirements too, just to be perfectly safe
            std::sort(recipeRequirements.begin(), recipeRequirements.end());

            // 4. If vectors match exactly, we have a potion!
            if (currentContents == recipeRequirements) {
                TN_INFO("Cauldron: Match found for prefab %s!", recipe.ResultPrefab.c_str());
                CraftPotion(recipe.ResultPrefab);
                return;
            }
        }
    }

    void CauldronBlock::CraftPotion(const std::string& prefabPath) {
        auto* world = m_Owner->GetParentWorld();
        Termina::Actor* hidePos = world->GetActorByName("hide_Pos");
        Termina::Actor* resultPos = world->GetActorByName("cauldron_resultPos");

        // 1. Send all used ingredients to the graveyard
        for (uint64_t id : m_SlottedItems) {
            Termina::Actor* item = world->GetActorById(id);
            if (item && hidePos) {
                item->DetachFromParent();
                hidePos->AttachChild(item);
                item->GetComponent<Termina::Transform>().SetPosition(hidePos->GetComponent<Termina::Transform>().GetPosition());

                if (item->HasComponent<Termina::Rigidbody>()) {
                    auto& rb = item->GetComponent<Termina::Rigidbody>();
                    rb.SetLinearVelocity(glm::vec3(0));
                    rb.Type = Termina::Rigidbody::BodyType::Static;
                }
            }
        }

        // 2. Empty the Cauldron
        m_SlottedItems.clear();

        // 3. Spawn the Potion
        if (resultPos) {
            Termina::Actor* potion = world->SpawnActorFromJSON(prefabPath);
            if (potion) {
                auto& potTr = potion->GetComponent<Termina::Transform>();
                potTr.SetPosition(resultPos->GetComponent<Termina::Transform>().GetPosition());

                // Add a satisfying little "pop" up out of the cauldron
                if (potion->HasComponent<Termina::Rigidbody>()) {
                    auto& rb = potion->GetComponent<Termina::Rigidbody>();
                    rb.Type = Termina::Rigidbody::BodyType::Dynamic;
                    rb.SetLinearVelocity(glm::vec3(0.0f, 3.0f, 0.0f));
                }

                TN_INFO("Cauldron: Potion successfully crafted!");
            }
        }
        else {
            TN_ERROR("Cauldron: Missing 'cauldron_resultPos' in world!");
        }
    }
}
#include "ChoppingBlock.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_Flower.hpp"
#include "GameAssembly/WH/Ingredients/Ingre_Mushroom.hpp"
#include "GameAssembly/WH/Tools/Knife.hpp"
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Core/Logger.hpp>
#include <Termina/World/World.hpp>

using namespace TerminaScript;

void ChoppingBlock::OnCollisionEnter(Termina::Actor* other)
{
    if (other == nullptr) return;

    Termina::World* world = m_Owner->GetParentWorld();
    if (world == nullptr) return;

    // Check if the colliding object is an ingredient
    bool isIngredient = false;
    bool isProcessed = false;

    if (other->HasComponent<Ingre_Flower>())
    {
        isIngredient = true;
        isProcessed = (other->GetComponent<Ingre_Flower>().CurrentState == Ingre_Flower::State::Processed);
    }
    else if (other->HasComponent<Ingre_Mushroom>())
    {
        isIngredient = true;
        isProcessed = (other->GetComponent<Ingre_Mushroom>().CurrentState == Ingre_Mushroom::State::Processed);
    }

    // ==========================================
    // SCENARIO 1: AN INGREDIENT LANDS ON THE BOARD
    // ==========================================
    if (isIngredient)
    {
        if (isProcessed)
        {
            TN_INFO(">>> Item is already processed! Ignoring.");
            return;
        }

        Termina::Actor* snapPos = world->GetActorByName("chopping_itemPos");
        if (snapPos != nullptr)
        {
            auto& itemTr = other->GetComponent<Termina::Transform>();
            auto& snapTr = snapPos->GetComponent<Termina::Transform>();

            // Use Kinematic instead of Static to stop sinking into the collision mesh
            if (other->HasComponent<Termina::Rigidbody>())
            {
                auto& rb = other->GetComponent<Termina::Rigidbody>();
                rb.Type = Termina::Rigidbody::BodyType::Kinematic;
                rb.SetLinearVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
            }

            itemTr.SetPosition(snapTr.GetPosition());
            itemTr.SetRotation(snapTr.GetRotation());

            // Assign the ID instead of the pointer for memory safety!
            m_CurrentIngredientID = other->GetID();
            TN_INFO((">>> Ingredient placed on board: " + other->GetName()).c_str());
        }
    }
    // ==========================================
    // SCENARIO 2: A TOOL (KNIFE) HITS THE BOARD
    // ==========================================
    else if (other->HasComponent<Knife>())
    {
        // Resolve the ID safely at the exact moment of collision
        Termina::Actor* currentIngredient = GetCurrentIngredient();

        if (currentIngredient != nullptr)
        {
            std::string prefabToSpawn = "";
            bool isReadyToCut = false;

            // Verify the prefab name AND that it is truly Unprocessed
            if (currentIngredient->HasComponent<Ingre_Flower>())
            {
                auto& flower = currentIngredient->GetComponent<Ingre_Flower>();
                if (flower.CurrentState == Ingre_Flower::State::Unprocessed)
                {
                    prefabToSpawn = flower.CutPrefabName;
                    isReadyToCut = true;
                }
            }
            else if (currentIngredient->HasComponent<Ingre_Mushroom>())
            {
                auto& mushroom = currentIngredient->GetComponent<Ingre_Mushroom>();
                if (mushroom.CurrentState == Ingre_Mushroom::State::Unprocessed)
                {
                    prefabToSpawn = mushroom.CutPrefabName;
                    isReadyToCut = true;
                }
            }

            // Only proceed if we have a valid, UNPROCESSED ingredient
            if (isReadyToCut && !prefabToSpawn.empty())
            {
                TN_INFO((">>> Chopping into " + prefabToSpawn).c_str());

                Termina::Actor* snapPos = world->GetActorByName("chopping_itemPos");

                // 2. Clear the ID immediately so nothing else (like the Grabber) can access it!
                m_CurrentIngredientID = 0;

                // 3. DESTROY CHILDREN FIRST (Memory Leak Fix)
                auto children = currentIngredient->GetChildren();
                for (Termina::Actor* child : children)
                {
                    if (child != nullptr)
                    {
                        child->DetachFromParent(); // <--- THE MAGIC BULLET
                        world->DestroyActor(child);
                    }
                }

                // 4. Destroy the raw ingredient safely
                world->DestroyActor(currentIngredient);

                // 5. Spawn the new cut prefab
                Termina::Actor* newIngredient = world->SpawnActorFromJSON(prefabToSpawn);

                if (newIngredient != nullptr && snapPos != nullptr)
                {
                    auto& newTr = newIngredient->GetComponent<Termina::Transform>();
                    auto& snapTr = snapPos->GetComponent<Termina::Transform>();

                    // Apply Kinematic to the newly spawned item as well
                    if (newIngredient->HasComponent<Termina::Rigidbody>())
                    {
                        auto& rb = newIngredient->GetComponent<Termina::Rigidbody>();
                        rb.Type = Termina::Rigidbody::BodyType::Kinematic;
                        rb.SetLinearVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
                    }

                    newTr.SetPosition(snapTr.GetPosition());
                    newTr.SetRotation(snapTr.GetRotation());

                    // 6. Officially assign the newly spawned ID
                    m_CurrentIngredientID = newIngredient->GetID();
                    TN_INFO(">>> Chopping successful!");
                }
            }
        }
    }
}

void ChoppingBlock::OnCollisionExit(Termina::Actor* other)
{
    if (other == nullptr) return;

    // If the player picks up the current item, we cleanly forget about it
    if (m_CurrentIngredientID != 0 && other->GetID() == m_CurrentIngredientID)
    {
        m_CurrentIngredientID = 0;
        TN_INFO(">>> Ingredient removed from chopping block.");
    }
}
#include "ChoppingBlock.hpp"
#include "Ingredients.hpp"
#include "Tools.hpp"
#include <Termina/World/Actor.hpp>
#include <Termina/World/World.hpp>
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Core/Logger.hpp>

using namespace TerminaScript;

void ChoppingBlock::OnTriggerEnter(Termina::Actor* other)
{
    // CASE 1: AN INGREDIENT ENTERS THE INTAKE BOX
    if (other->HasComponent<Ingredient>())
    {
        auto& ingredient = other->GetComponent<Ingredient>();

        // Only accept uncut ingredients, and only if the block is empty
        if (ingredient.CurrentState == Ingredient::State::Uncut && m_CurrentIngredient == nullptr)
        {
            m_CurrentIngredient = other; // Claim the item!

            // 1. Make it static so it stays put and doesn't roll away
            auto& rb = other->GetComponent<Termina::Rigidbody>();
            rb.Type = Termina::Rigidbody::BodyType::Static;

            // 2. Teleport it to the visual snapping position
            Termina::Actor* snapPos = m_Owner->GetParentWorld()->GetActorByName("chopping_itemPos");
            if (snapPos != nullptr)
            {
                auto& itemTr = other->GetComponent<Termina::Transform>();
                auto& snapTr = snapPos->GetComponent<Termina::Transform>();

                itemTr.SetPosition(snapTr.GetPosition());
                itemTr.SetRotation(snapTr.GetRotation());
            }
            TN_INFO("Ingredient placed on the chopping block!");
        }
    }

    // CASE 2: A TOOL ENTERS THE INTAKE BOX
    else if (other->HasComponent<Tool>())
    {
        auto& tool = other->GetComponent<Tool>();

        // If it's a knife, and we have an uncut ingredient waiting... CHOP IT!
        if (tool.ToolType == Tool::Type::Knife && m_CurrentIngredient != nullptr)
        {
            TN_INFO("CHOPPING INGREDIENT!");

            // TODO: Destroy m_CurrentIngredient
            // TODO: Spawn the "Cut" prefab in its place

            m_CurrentIngredient = nullptr; // Free up the block!
        }
    }
}

void ChoppingBlock::OnTriggerExit(Termina::Actor* other)
{
    // If the player simply picks the item back up before cutting it, 
    // we need to free the block so they can put something else down!
    if (other == m_CurrentIngredient)
    {
        m_CurrentIngredient = nullptr;
        TN_INFO("Ingredient removed from the block.");
    }
}
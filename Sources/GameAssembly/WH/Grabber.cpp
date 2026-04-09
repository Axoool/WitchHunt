#include "Grabber.hpp"
#include "Grabbable.hpp"
#include "ChoppingBlock.hpp" 
#include "CauldronBlock.hpp" // <-- ADDED THIS INCLUDE
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Renderer/Components/CameraComponent.hpp>
#include <Termina/Core/Logger.hpp>
#include <Termina/Core/Application.hpp>
#include <Termina/Physics/PhysicsSystem.hpp>
#include "GameAssembly/WH/Potions/Potion_dmg.hpp"

using namespace TerminaScript;

Termina::Actor* Grabber::GetHoveredItem() const {
    if (m_HoveredItemID == 0) return nullptr;
    return m_Owner->GetParentWorld()->GetActorById(m_HoveredItemID);
}

Termina::Actor* Grabber::GetHeldItem() const {
    if (m_HeldItemID == 0) return nullptr;
    return m_Owner->GetParentWorld()->GetActorById(m_HeldItemID);
}

void Grabber::Update(float deltaTime)
{
    auto* physics = Termina::Application::GetSystem<Termina::PhysicsSystem>();
    Termina::Transform cameraTr;
    Termina::World* world = m_Owner->GetParentWorld();
    auto children = m_Owner->GetChildren();
    for (Termina::Actor* child : children)
    {
        if (child->HasComponent<Termina::CameraComponent>())
        {
			cameraTr = child->GetComponent<Termina::Transform>();
            break;
        }
    }
    Termina::Ray reachRay;
    reachRay.Origin = cameraTr.GetPosition();
    glm::vec3 trueForward = cameraTr.GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f);
    reachRay.Direction = glm::normalize(trueForward);
    reachRay.MaxDistance = reachDistance;

    Termina::RayResult hit = physics->Raycast(reachRay);

    // ==========================================
    // 1. Determine what we are looking at
    // ==========================================
    uint64 currentHitID = 0;
    if (hit.Hit && hit.HitActor != nullptr) {
        if (m_HeldItemID != 0 && hit.HitActor->GetID() == m_HeldItemID) {
            currentHitID = m_HoveredItemID;
        }
        else if (hit.HitActor->HasComponent<Grabbable>()) {
            currentHitID = hit.HitActor->GetID();
        }
        else if (hit.HitActor->HasComponent<ChoppingBlock>()) {
            auto& board = hit.HitActor->GetComponent<ChoppingBlock>();
            Termina::Actor* itemOnBoard = board.GetSlottedItem();
            if (itemOnBoard) {
                currentHitID = itemOnBoard->GetID();
            }
        }
    }

    // ==========================================
    // 2. Handle Highlighting & Memory Safety
    // ==========================================
    if (m_HoveredItemID != currentHitID)
    {
        Termina::Actor* oldHover = GetHoveredItem();

        if (oldHover != nullptr && oldHover->GetID() == m_HoveredItemID && m_HoveredItemID != m_HeldItemID)
        {
            if (oldHover->HasComponent<Termina::Transform>())
            {
                oldHover->GetComponent<Termina::Transform>().SetScale(m_OriginalScale);
            }
        }

        m_HoveredItemID = currentHitID;

        Termina::Actor* newHover = GetHoveredItem();

        if (newHover != nullptr && m_HoveredItemID != m_HeldItemID)
        {
            if (newHover->HasComponent<Termina::Transform>())
            {
                auto& itemTr = newHover->GetComponent<Termina::Transform>();
                m_OriginalScale = itemTr.GetScale();
                itemTr.SetScale(m_OriginalScale * hoverSize);
            }
        }
    }

    // ==========================================
    // 3. INPUT: Grab or Throw
    // ==========================================
    if (Input::IsMouseButtonPressed(Termina::MouseButton::Left))
    {
        Termina::Actor* heldItem = GetHeldItem();
        Termina::Actor* hoveredItem = GetHoveredItem();

        // ==========================================================
        // NEW CAULDRON EXTRACTION LOGIC
        // If we click the intake box with an empty hand, pull an item out!
        // ==========================================================
        if (heldItem == nullptr && hit.Hit && hit.HitActor != nullptr && hit.HitActor->GetName() == "cauldron_IntakeBox")
        {
            Termina::Actor* cauldronActor = nullptr;

            // Find the actor that actually has the CauldronBlock component
            for (const auto& actorPtr : world->GetActors()) {
                if (actorPtr->HasComponent<CauldronBlock>()) {
                    cauldronActor = actorPtr.get();
                    break;
                }
            }

            if (cauldronActor) {
                auto& cauldron = cauldronActor->GetComponent<CauldronBlock>();
                Termina::Actor* poppedItem = cauldron.PopLastIngredient();

                if (poppedItem) {
                    // Force the script to hold the item we just popped
                    m_HeldItemID = poppedItem->GetID();
                    heldItem = poppedItem;

                    // Clear the hover state so it doesn't get confused
                    m_HoveredItemID = 0;
                    m_OriginalScale = glm::vec3(1.0f); // Cauldron PopLastIngredient resets scale to 1.0f

                    // Make it Kinematic so it doesn't fall through the floor
                    if (heldItem->HasComponent<Termina::Rigidbody>()) {
                        auto& rb = heldItem->GetComponent<Termina::Rigidbody>();
                        rb.Type = Termina::Rigidbody::BodyType::Kinematic;
                        rb.SetLinearVelocity(glm::vec3(0.0f));
                    }

                    // Attach to player's hand
                    Termina::Actor* handTarget = world->GetActorByName("GrabbedPos");
                    if (handTarget != nullptr) {
                        if (heldItem->GetParent() != nullptr) {
                            heldItem->DetachFromParent();
                        }
                        handTarget->AttachChild(heldItem);

                        auto& itemTr = heldItem->GetComponent<Termina::Transform>();
                        auto& handTr = handTarget->GetComponent<Termina::Transform>();
                        itemTr.SetPosition(handTr.GetPosition());
                        itemTr.SetRotation(handTr.GetRotation());

                        TN_INFO("Item GRABBED from Cauldron!");
                    }

                    // Return early so we don't accidentally trigger the normal grab logic
                    return;
                }
            }
        }
        // ==========================================================
        // END OF CAULDRON EXTRACTION LOGIC
        // ==========================================================

        // Original Grab Logic
        if (heldItem == nullptr && hoveredItem != nullptr)
        {
            m_HeldItemID = m_HoveredItemID;
            heldItem = hoveredItem;

            auto& itemTr = heldItem->GetComponent<Termina::Transform>();
            itemTr.SetScale(m_OriginalScale);

            if (heldItem->HasComponent<Termina::Rigidbody>())
            {
                auto& rb = heldItem->GetComponent<Termina::Rigidbody>();
                rb.Type = Termina::Rigidbody::BodyType::Kinematic;
                rb.SetLinearVelocity(glm::vec3(0.0f));
            }

            if (heldItem->GetParent() && heldItem->GetParent()->GetParent()) {
                auto* boardActor = heldItem->GetParent()->GetParent();
                if (boardActor->HasComponent<ChoppingBlock>()) {
                    boardActor->GetComponent<ChoppingBlock>().ClearSlot();
                }
            }

            Termina::Actor* handTarget = world->GetActorByName("GrabbedPos");
            if (handTarget != nullptr)
            {
                if (heldItem->GetParent() != nullptr)
                {
                    heldItem->DetachFromParent();
                }

                handTarget->AttachChild(heldItem);

                auto& handTr = handTarget->GetComponent<Termina::Transform>();
                itemTr.SetPosition(handTr.GetPosition());
                itemTr.SetRotation(handTr.GetRotation());

                TN_INFO("Item GRABBED!");
            }
        }
        // Original Throw Logic
        else if (heldItem != nullptr)
        {
            // --- NEW: ARM THE POTION ---
            if (heldItem->HasComponent<Potion_dmg>()) {
                heldItem->GetComponent<Potion_dmg>().SetThrown(true);
            }

            if (heldItem->HasComponent<Termina::Rigidbody>())
            {
                auto& rb = heldItem->GetComponent<Termina::Rigidbody>();
                rb.Type = Termina::Rigidbody::BodyType::Dynamic;

                glm::vec3 throwForce = reachRay.Direction * 5.0f;
                rb.SetLinearVelocity(throwForce);
            }

            heldItem->DetachFromParent();
            m_HeldItemID = 0;
            TN_INFO("Item THROWN!");
        }
    }
}
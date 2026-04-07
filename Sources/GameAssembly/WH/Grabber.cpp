#include "Grabber.hpp"
#include "Grabbable.hpp"
#include "ChoppingBlock.hpp" 
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Core/Logger.hpp>
#include <Termina/Core/Application.hpp>
#include <Termina/Physics/PhysicsSystem.hpp>

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
    auto& cameraTr = m_Owner->GetComponent<Termina::Transform>();
    Termina::World* world = m_Owner->GetParentWorld();

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
    // Part 1: Determine what we are looking at
    if (hit.Hit && hit.HitActor != nullptr) {
        if (m_HeldItemID != 0 && hit.HitActor->GetID() == m_HeldItemID) {
            currentHitID = m_HoveredItemID;
        }
        else if (hit.HitActor->HasComponent<Grabbable>()) {
            currentHitID = hit.HitActor->GetID();
        }
        // NEW LOGIC: Ask the board for its item
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

        // FIX FOR THE DELAYED CRASH: 
        // Ensure oldHover isn't corrupted, dangling memory before touching its scale.
        // We do this by checking if the ID matches and if it actually has a transform.
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

        if (heldItem == nullptr && hoveredItem != nullptr)
        {
            m_HeldItemID = m_HoveredItemID;
            heldItem = hoveredItem;

            // Define the transform for the item we just picked up
            auto& itemTr = heldItem->GetComponent<Termina::Transform>();
            itemTr.SetScale(m_OriginalScale); // Reset scale from the hover effect

            // Handle Physics: Change to Kinematic so it follows the hand perfectly
            if (heldItem->HasComponent<Termina::Rigidbody>())
            {
                auto& rb = heldItem->GetComponent<Termina::Rigidbody>();
                rb.Type = Termina::Rigidbody::BodyType::Kinematic;
                rb.SetLinearVelocity(glm::vec3(0.0f));
            }

            // BOARD SAFETY: If we took it from a chopping block, tell the block it's empty!
            // Path: Item -> itemPos -> ChoppingBlock
            if (heldItem->GetParent() && heldItem->GetParent()->GetParent()) {
                auto* boardActor = heldItem->GetParent()->GetParent();
                if (boardActor->HasComponent<ChoppingBlock>()) {
                    boardActor->GetComponent<ChoppingBlock>().ClearSlot();
                }
            }

            // ATTACH TO HAND
            Termina::Actor* handTarget = world->GetActorByName("GrabbedPos");
            if (handTarget != nullptr)
            {
                // THE ORPHAN GUARD: Only detach if it actually has a parent
                if (heldItem->GetParent() != nullptr)
                {
                    heldItem->DetachFromParent();
                }

                handTarget->AttachChild(heldItem);

                // Snap to the hand's position and rotation
                auto& handTr = handTarget->GetComponent<Termina::Transform>();
                itemTr.SetPosition(handTr.GetPosition());
                itemTr.SetRotation(handTr.GetRotation());

                TN_INFO("Item GRABBED!");
            }
        }
        else if (heldItem != nullptr)
        {
            // ... (Your existing THROW logic remains the same)
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
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

    // 1. Determine what we are looking at
    uint64 currentHitID = 0;
    if (hit.Hit && hit.HitActor != nullptr)
    {
        if (hit.HitActor->HasComponent<Grabbable>())
        {
            currentHitID = hit.HitActor->GetID();
        }
        else if (hit.HitActor->HasComponent<ChoppingBlock>())
        {
            Termina::Actor* boardIngredient = hit.HitActor->GetComponent<ChoppingBlock>().GetCurrentIngredient();
            if (boardIngredient != nullptr)
                currentHitID = boardIngredient->GetID();
        }
    }

    // ==========================================
    // 2. Handle Highlighting (Fixed Hover-while-holding Logic)
    // ==========================================
    if (m_HoveredItemID != currentHitID)
    {
        // Reset the OLD item scale BEFORE losing the ID
        Termina::Actor* oldHover = GetHoveredItem();

        // Only reset if it's NOT the item we are currently holding in our hand
        if (oldHover != nullptr && m_HoveredItemID != m_HeldItemID)
        {
            oldHover->GetComponent<Termina::Transform>().SetScale(m_OriginalScale);
        }

        m_HoveredItemID = currentHitID;

        // Setup the NEW item highlight
        Termina::Actor* newHover = GetHoveredItem();

        // Only highlight if it's a valid item AND it's not the one we are holding
        if (newHover != nullptr && m_HoveredItemID != m_HeldItemID)
        {
            auto& itemTr = newHover->GetComponent<Termina::Transform>();

            // CRITICAL: Always capture the scale so we know how to "un-shrink" it later
            m_OriginalScale = itemTr.GetScale();

            itemTr.SetScale(m_OriginalScale * hoverSize);
        }
    }

    // 3. INPUT: Grab or Throw
    if (Input::IsMouseButtonPressed(Termina::MouseButton::Left))
    {
        Termina::Actor* heldItem = GetHeldItem();
        Termina::Actor* hoveredItem = GetHoveredItem();

        if (heldItem == nullptr && hoveredItem != nullptr)
        {
            m_HeldItemID = m_HoveredItemID;
            heldItem = hoveredItem; // Resolved for this frame

            auto& itemTr = heldItem->GetComponent<Termina::Transform>();
            itemTr.SetScale(m_OriginalScale); // Reset scale while held

            auto& rb = heldItem->GetComponent<Termina::Rigidbody>();
            rb.Type = Termina::Rigidbody::BodyType::Static;

            Termina::Actor* handTarget = world->GetActorByName("GrabbedPos");
            if (handTarget != nullptr)
            {
                handTarget->AttachChild(heldItem);
                itemTr.SetPosition(handTarget->GetComponent<Termina::Transform>().GetPosition());
                itemTr.SetRotation(handTarget->GetComponent<Termina::Transform>().GetRotation());
                TN_INFO("Item GRABBED!");
            }
        }
        else if (heldItem != nullptr)
        {
            auto& rb = heldItem->GetComponent<Termina::Rigidbody>();
            rb.Type = Termina::Rigidbody::BodyType::Dynamic;

            heldItem->DetachFromParent(); // <--- CRITICAL FIX FOR CRASH

            glm::vec3 throwForce = reachRay.Direction * 5.0f;
            rb.SetLinearVelocity(throwForce);

            m_HeldItemID = 0;
            TN_INFO("Item THROWN!");
        }
    }
}
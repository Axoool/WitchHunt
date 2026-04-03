#include "Grabber.hpp"
#include "Grabbable.hpp"
#include "ChoppingBlock.hpp"
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Core/Logger.hpp>

#include <Termina/Core/Application.hpp>
#include <Termina/Physics/PhysicsSystem.hpp>

using namespace TerminaScript;

void Grabber::Update(float deltaTime)
{
    // ==========================================
    // THE RAYCAST (Runs every frame to act as your "Eyes")
    // ==========================================
    auto* physics = Termina::Application::GetSystem<Termina::PhysicsSystem>();
    auto& cameraTr = m_Owner->GetComponent<Termina::Transform>();

    Termina::Ray reachRay;
    reachRay.Origin = cameraTr.GetPosition();

    glm::vec3 trueForward = cameraTr.GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f);
    reachRay.Direction = glm::normalize(trueForward);
    reachRay.MaxDistance = reachDistance;

    Termina::RayResult hit = physics->Raycast(reachRay);

    // ==========================================
    // RAYCAST INTERCEPTION LOGIC
    // ==========================================
    Termina::Actor* currentHitItem = nullptr;
    if (hit.Hit && hit.HitActor != nullptr)
    {
        // 1. Did we directly hit a Grabbable item?
        if (hit.HitActor->HasComponent<Grabbable>())
        {
            currentHitItem = hit.HitActor;
        }
        // 2. NEW: Did we hit the Chopping Block? 
        else if (hit.HitActor->HasComponent<ChoppingBlock>())
        {
            // Ask the block what ingredient it is currently holding
            Termina::Actor* boardIngredient = hit.HitActor->GetComponent<ChoppingBlock>().GetCurrentIngredient();

            if (boardIngredient != nullptr && boardIngredient->HasComponent<Grabbable>())
            {
                // Reroute! Pretend we looked directly at the ingredient.
                currentHitItem = boardIngredient;
            }
        }
    }

    // Handle Highlighting (Scaling up and down)
    if (m_HoveredItem != currentHitItem)
    {
        // 1. Un-highlight the old item if we look away from it
        if (m_HoveredItem != nullptr && m_HoveredItem != m_HeldItem)
        {
            auto& itemTr = m_HoveredItem->GetComponent<Termina::Transform>();
            itemTr.SetScale(m_OriginalScale);
        }

        m_HoveredItem = currentHitItem;

        // 2. Highlight the new item (only if we aren't already holding something!)
        if (m_HoveredItem != nullptr && m_HeldItem == nullptr)
        {
            auto& itemTr = m_HoveredItem->GetComponent<Termina::Transform>();
            m_OriginalScale = itemTr.GetScale();
            itemTr.SetScale(m_OriginalScale * hoverSize);
        }
    }

    // ==========================================
    // INPUT: Grab or Throw
    // ==========================================
    if (Input::IsMouseButtonPressed(Termina::MouseButton::Left))
    {
        if (m_HeldItem == nullptr && m_HoveredItem != nullptr)
        {
            m_HeldItem = m_HoveredItem;

            auto& itemTr = m_HeldItem->GetComponent<Termina::Transform>();
            itemTr.SetScale(m_OriginalScale);

            auto& rb = m_HeldItem->GetComponent<Termina::Rigidbody>();
            rb.Type = Termina::Rigidbody::BodyType::Static;

            Termina::Actor* handTarget = m_Owner->GetParentWorld()->GetActorByName("GrabbedPos");

            if (handTarget != nullptr)
            {
                handTarget->AttachChild(m_HeldItem);

                auto& handTr = handTarget->GetComponent<Termina::Transform>();

                itemTr.SetPosition(handTr.GetPosition());
                itemTr.SetRotation(handTr.GetRotation());

                TN_INFO("Item GRABBED!");
            }
        }
        else if (m_HeldItem != nullptr)
        {
            auto& rb = m_HeldItem->GetComponent<Termina::Rigidbody>();
            rb.Type = Termina::Rigidbody::BodyType::Dynamic;

            m_HeldItem->DetachFromParent();

            float throwStrength = 5.0f;
            glm::vec3 throwForce = reachRay.Direction * throwStrength;

            rb.SetLinearVelocity(throwForce);

            TN_INFO("Item THROWN! Velocity -> X: %f, Y: %f, Z: %f", throwForce.x, throwForce.y, throwForce.z);

            m_HeldItem = nullptr;
        }
    }
}
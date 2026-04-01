#include "Grabber.hpp"
#include "Grabbable.hpp"
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Core/Logger.hpp>

using namespace TerminaScript;

void Grabber::Update(float deltaTime)
{
    // Changed to "Pressed" or "Down" to prevent grabbing and throwing in the same millisecond!
    if (Input::IsMouseButtonPressed(Termina::MouseButton::Left)) 
    {
        // ==========================================
        // STATE 1: WE ARE EMPTY HANDED -> GRAB ITEM
        // ==========================================
        if (m_HeldItem == nullptr && m_HoveredItem != nullptr)
        {
            m_HeldItem = m_HoveredItem;

            auto& rb = m_HeldItem->GetComponent<Termina::Rigidbody>();
            rb.Type = Termina::Rigidbody::BodyType::Static;

            Termina::Actor* handTarget = m_Owner->GetParentWorld()->GetActorByName("GrabbedPos");

            if (handTarget != nullptr)
            {
                handTarget->AttachChild(m_HeldItem);

                auto& itemTr = m_HeldItem->GetComponent<Termina::Transform>();
                auto& handTr = handTarget->GetComponent<Termina::Transform>();

                itemTr.SetPosition(handTr.GetPosition());
                itemTr.SetRotation(handTr.GetRotation());

                TN_INFO("Item GRABBED!");
            }
        }
        // ==========================================
        // STATE 2: WE ARE HOLDING SOMETHING -> THROW!
        // ==========================================
        else if (m_HeldItem != nullptr)
        {
            // 1. Make it a physics object again
            auto& rb = m_HeldItem->GetComponent<Termina::Rigidbody>();
            rb.Type = Termina::Rigidbody::BodyType::Dynamic;


            m_HeldItem->AttachChildSilent(nullptr); 

            auto& reachBoxTr = m_Owner->GetComponent<Termina::Transform>();

            // Get the actual 3D rotation of your ReachBox/Camera
            glm::quat currentRotation = reachBoxTr.GetRotation();

            // Multiply the rotation by the default forward vector (0, 0, -1)
            // This calculates exactly where your crosshair is pointing in the world!
            glm::vec3 trueForward = currentRotation * glm::vec3(0.0f, 0.0f, -1.0f);
            trueForward = glm::normalize(trueForward);

            // Let's bump the strength to 5.0f so it clears your feet
            float throwStrength = 5.0f;
            glm::vec3 throwForce = trueForward * throwStrength;

            // Apply the velocity
            rb.SetLinearVelocity(throwForce);

            TN_INFO("Item THROWN! Velocity -> X: %f, Y: %f, Z: %f", throwForce.x, throwForce.y, throwForce.z);

            m_HeldItem = nullptr;
            TN_INFO("Item THROWN!");
        }
    }
}

void Grabber::OnCollisionEnter(Termina::Actor* other)
{
    // 1. THE DEBUG PRINT: This will spam the console if it touches literally anything!
    TN_INFO("BONK! The ReachBox just touched an entity!");

    // 2. The actual logic
    if (other->HasComponent<Grabbable>() && m_HeldItem == nullptr)
    {
        m_HoveredItem = other;

        auto& itemTr = m_HoveredItem->GetComponent<Termina::Transform>();
        m_OriginalScale = itemTr.GetScale();
        itemTr.SetScale(m_OriginalScale * 1.2f);

        TN_INFO("Hovering over grabbable item!");
    }
}

void Grabber::OnCollisionExit(Termina::Actor* other)
{
    // ENLEVER LE HIGHLIGHT
    if (other == m_HoveredItem)
    {
        auto& itemTr = m_HoveredItem->GetComponent<Termina::Transform>();
        itemTr.SetScale(m_OriginalScale);

        m_HoveredItem = nullptr;
    }
}
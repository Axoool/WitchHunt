#include "Potion_dmg.hpp"
#include <Termina/World/World.hpp>
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Core/Logger.hpp>
#include <Termina/Core/Application.hpp>
#include <Termina/Physics/PhysicsSystem.hpp>

namespace TerminaScript {
    void Potion_dmg::OnUpdate(float deltaTime) {
        // 1. Only run if the potion is in flight
        if (!m_WasThrown) return;

        // 2. DISARM if picked up. 
        // If the player catches it, it becomes a child of GrabbedPos.
        if (m_Owner->GetParent() && m_Owner->GetParent()->GetName() == "GrabbedPos") {
            m_WasThrown = false;
            m_ArmTimer = 0.0f;
            return;
        }

        m_ArmTimer += deltaTime;
        if (m_ArmTimer < 0.1f) return;

        if (m_Owner->HasComponent<Termina::Rigidbody>()) {
            auto& rb = m_Owner->GetComponent<Termina::Rigidbody>();
            glm::vec3 velocity = rb.GetLinearVelocity();
            float speed = glm::length(velocity);

            // Only check for impact if it's moving
            if (speed > 0.1f) {
                auto* physics = Termina::Application::GetSystem<Termina::PhysicsSystem>();
                auto& myTr = m_Owner->GetComponent<Termina::Transform>();

                // 3. PROXIMITY DETECTION (The "Look-Ahead" Ray)
                Termina::Ray impactRay;
                impactRay.Origin = myTr.GetPosition();
                impactRay.Direction = glm::normalize(velocity);
                impactRay.MaxDistance = 0.4f; // 40cm look-ahead

                Termina::RayResult hit = physics->Raycast(impactRay);

                // 4. SHATTER LOGIC
                // If we hit something that isn't the potion itself...
                if (hit.Hit && hit.HitActor != m_Owner) {
                    TN_INFO("Damage Potion Shattered on Impact!");

                    auto* world = m_Owner->GetParentWorld();

                    // Spawn the Field at the potion's current location
                    Termina::Actor* damageField = world->SpawnActorFromJSON("Assets/Prefabs/Fields/field_dmg.trp");
                    if (damageField) {
                        auto& fieldTr = damageField->GetComponent<Termina::Transform>();
                        fieldTr.SetPosition(myTr.GetPosition());
                    }

                    // Move to hide_Pos (Graveyard)
                    Termina::Actor* hidePos = world->GetActorByName("hide_Pos");
                    if (hidePos) {
                        m_Owner->DetachFromParent();
                        hidePos->AttachChild(m_Owner);
                        myTr.SetPosition(hidePos->GetComponent<Termina::Transform>().GetPosition());

                        rb.SetLinearVelocity(glm::vec3(0.0f));
                        rb.Type = Termina::Rigidbody::BodyType::Static;
                    }

                    m_WasThrown = false;
                    m_ArmTimer = 0.0f;
                }
            }
        }
    }
}
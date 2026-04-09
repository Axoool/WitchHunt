#include "Potion_dmg.hpp"
#include "GameAssembly/WH/Environment/Ground.hpp"
#include <Termina/World/World.hpp>
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Core/Logger.hpp>
#include <Termina/Core/Application.hpp>
#include <Termina/Physics/PhysicsSystem.hpp>

namespace TerminaScript {
    void Potion_dmg::OnUpdate(float deltaTime) {
        if (!m_WasThrown) return;

        // Disarm if caught
        if (m_Owner->GetParent() && m_Owner->GetParent()->GetName() == "GrabbedPos") {
            m_WasThrown = false;
            return;
        }

        m_ArmTimer += deltaTime;
        if (m_ArmTimer < 0.1f) return;

        auto* world = m_Owner->GetParentWorld();
        auto& potionTr = m_Owner->GetComponent<Termina::Transform>();

        // SENSOR CHECK: Loop through all actors and check for the Ground tag
        for (const auto& actorPtr : world->GetActors()) {
            Termina::Actor* actor = actorPtr.get();
            if (!actor || actor == m_Owner) continue;

            // Here is the Component tag check, just like your ChoppingBlock
            if (actor->HasComponent<Ground>()) {
                auto& groundTr = actor->GetComponent<Termina::Transform>();

                // Is the potion's height at or below this ground's height?
                // (0.3f is the offset so it breaks when the bottom of the bottle hits)
                // Change the 0.3f to something much larger, like 1.2f or 1.5f
                if (potionTr.GetPosition().y <= groundTr.GetPosition().y + 1.2f) {
                    TN_INFO("Potion hit a Ground surface! Shattering...");
                    Shatter();
                    return;
                }
            }
        }
    }

    void Potion_dmg::Shatter() {
        auto* world = m_Owner->GetParentWorld();
        auto& myTr = m_Owner->GetComponent<Termina::Transform>();

        // 1. Spawn Damage Field
        Termina::Actor* damageField = world->SpawnActorFromJSON("Assets/Prefabs/Fields/field_dmg.trp");
        if (damageField) {
            // Get the potion's exact position (which is 1.2f units in the air)
            glm::vec3 spawnPos = myTr.GetPosition();

            spawnPos.y -= 0.29f;

            // Apply the new grounded position
            damageField->GetComponent<Termina::Transform>().SetPosition(spawnPos);
        }

        // 2. Send the empty potion to hide_Pos
        Termina::Actor* hidePos = world->GetActorByName("hide_Pos");
        if (hidePos) {
            m_Owner->DetachFromParent();
            hidePos->AttachChild(m_Owner);
            m_Owner->GetComponent<Termina::Transform>().SetPosition(hidePos->GetComponent<Termina::Transform>().GetPosition());

            auto& rb = m_Owner->GetComponent<Termina::Rigidbody>();
            rb.SetLinearVelocity(glm::vec3(0.0f));
            rb.Type = Termina::Rigidbody::BodyType::Static;
        }

        // Reset the states so this potion instance goes back to sleep
        m_WasThrown = false;
        m_ArmTimer = 0.0f;
    }
}
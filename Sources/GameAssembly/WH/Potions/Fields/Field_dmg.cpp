#include "Field_dmg.hpp"
#include <Termina/World/World.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Physics/Components/Rigidbody.hpp>
#include <Termina/Core/Logger.hpp>

namespace TerminaScript {
    void Field_dmg::OnUpdate(float deltaTime) {
        m_Lifetime += deltaTime;

        // Damage logic for enemies would happen here

        // 5 second duration
        if (m_Lifetime >= 5.0f) {
            auto* world = m_Owner->GetParentWorld();
            Termina::Actor* hidePos = world->GetActorByName("hide_Pos");

            if (hidePos) {
                m_Owner->DetachFromParent();
                hidePos->AttachChild(m_Owner);

                auto& tr = m_Owner->GetComponent<Termina::Transform>();
                tr.SetPosition(hidePos->GetComponent<Termina::Transform>().GetPosition());

                if (m_Owner->HasComponent<Termina::Rigidbody>()) {
                    auto& rb = m_Owner->GetComponent<Termina::Rigidbody>();
                    rb.Type = Termina::Rigidbody::BodyType::Static;
                    rb.SetLinearVelocity(glm::vec3(0.0f));
                }
                TN_INFO("Field: Expired and moved to hide_Pos.");
            }

            m_Lifetime = -1.0f; // Stop update logic
        }
    }
}
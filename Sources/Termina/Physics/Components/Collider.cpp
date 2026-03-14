#include "Collider.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/World/WorldSystem.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/World/Actor.hpp>

#include <GLM/gtc/matrix_transform.hpp>

namespace Termina {

    void Collider::OnPreRender(float /*dt*/)
    {
        // Draw collider shape only in editor mode; in play mode Jolt's debug
        // renderer handles visualization when enabled.
        auto* ws = Application::GetSystem<WorldSystem>();
        if (!ws || ws->IsPlaying()) return;

        if (!m_Owner || !m_Owner->HasComponent<Transform>()) return;

        glm::mat4 world = m_Owner->GetComponent<Transform>().GetWorldMatrix();

        // Bake the collider offset and rotation into the draw matrix.
        glm::mat4 offsetMatrix = glm::translate(glm::mat4(1.0f), Offset) *
                                 glm::mat4_cast(RotationOffset);
        DrawShape(world * offsetMatrix);
    }

    void Collider::Serialize(nlohmann::json& out) const
    {
        out["offset"]   = { Offset.x, Offset.y, Offset.z };
        out["rot_off"]  = { RotationOffset.x, RotationOffset.y,
                            RotationOffset.z, RotationOffset.w };
    }

    void Collider::Deserialize(const nlohmann::json& in)
    {
        if (in.contains("offset"))
        {
            Offset = glm::vec3(in["offset"][0], in["offset"][1], in["offset"][2]);
        }
        if (in.contains("rot_off"))
        {
            RotationOffset = glm::quat(in["rot_off"][3], in["rot_off"][0],
                                       in["rot_off"][1], in["rot_off"][2]);
        }
    }

} // namespace Termina

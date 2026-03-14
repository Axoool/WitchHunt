#include "CapsuleCollider.hpp"

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

#include <Termina/Renderer/Passes/DebugPass.hpp>
#include <ImGui/imgui.h>

namespace Termina {

    JPH::ShapeRefC CapsuleCollider::CreateShape() const
    {
        JPH::CapsuleShapeSettings settings(HalfHeight, Radius);
        auto result = settings.Create();
        return result.IsValid() ? result.Get() : nullptr;
    }

    void CapsuleCollider::DrawShape(const glm::mat4& worldMatrix) const
    {
        glm::vec3 from(0.0f, -HalfHeight, 0.0f);
        glm::vec3 to(0.0f,   HalfHeight, 0.0f);
        DebugPass::DrawCapsule(from, to, Radius, 8, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &worldMatrix);
    }

    void CapsuleCollider::Inspect()
    {
        ImGui::DragFloat("Half Height", &HalfHeight, 0.01f, 0.001f, 1000.0f);
        ImGui::DragFloat("Radius",      &Radius,     0.01f, 0.001f, 1000.0f);
    }

    void CapsuleCollider::Serialize(nlohmann::json& out) const
    {
        Collider::Serialize(out);
        out["half_height"] = HalfHeight;
        out["radius"]      = Radius;
    }

    void CapsuleCollider::Deserialize(const nlohmann::json& in)
    {
        Collider::Deserialize(in);
        if (in.contains("half_height")) HalfHeight = in["half_height"];
        if (in.contains("radius"))      Radius     = in["radius"];
    }

} // namespace Termina

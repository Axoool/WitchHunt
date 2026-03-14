#include "CylinderCollider.hpp"

#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

#include <Termina/Renderer/Passes/DebugPass.hpp>
#include <ImGui/imgui.h>

namespace Termina {

    JPH::ShapeRefC CylinderCollider::CreateShape() const
    {
        JPH::CylinderShapeSettings settings(HalfHeight, Radius);
        auto result = settings.Create();
        return result.IsValid() ? result.Get() : nullptr;
    }

    void CylinderCollider::DrawShape(const glm::mat4& worldMatrix) const
    {
        glm::vec3 from(0.0f, -HalfHeight, 0.0f);
        glm::vec3 to(0.0f,   HalfHeight, 0.0f);
        DebugPass::DrawCylinder(from, to, Radius, 16, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &worldMatrix);
    }

    void CylinderCollider::Inspect()
    {
        ImGui::DragFloat("Half Height", &HalfHeight, 0.01f, 0.001f, 1000.0f);
        ImGui::DragFloat("Radius",      &Radius,     0.01f, 0.001f, 1000.0f);
    }

    void CylinderCollider::Serialize(nlohmann::json& out) const
    {
        Collider::Serialize(out);
        out["half_height"] = HalfHeight;
        out["radius"]      = Radius;
    }

    void CylinderCollider::Deserialize(const nlohmann::json& in)
    {
        Collider::Deserialize(in);
        if (in.contains("half_height")) HalfHeight = in["half_height"];
        if (in.contains("radius"))      Radius     = in["radius"];
    }

} // namespace Termina

#include "SphereCollider.hpp"

#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <Termina/Renderer/Passes/DebugPass.hpp>
#include <ImGui/imgui.h>

namespace Termina {

    JPH::ShapeRefC SphereCollider::CreateShape() const
    {
        JPH::SphereShapeSettings settings(Radius);
        auto result = settings.Create();
        return result.IsValid() ? result.Get() : nullptr;
    }

    void SphereCollider::DrawShape(const glm::mat4& worldMatrix) const
    {
        DebugPass::DrawSphere(glm::vec3(0.0f), Radius, 16, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &worldMatrix);
    }

    void SphereCollider::Inspect()
    {
        ImGui::DragFloat("Radius", &Radius, 0.01f, 0.001f, 1000.0f);
    }

    void SphereCollider::Serialize(nlohmann::json& out) const
    {
        Collider::Serialize(out);
        out["radius"] = Radius;
    }

    void SphereCollider::Deserialize(const nlohmann::json& in)
    {
        Collider::Deserialize(in);
        if (in.contains("radius")) Radius = in["radius"];
    }

} // namespace Termina

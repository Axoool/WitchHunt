#include "BoxCollider.hpp"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>

#include <Termina/Renderer/Passes/DebugPass.hpp>
#include <ImGui/imgui.h>

namespace Termina {

    JPH::ShapeRefC BoxCollider::CreateShape() const
    {
        JPH::BoxShapeSettings settings(JPH::Vec3(HalfExtents.x, HalfExtents.y, HalfExtents.z));
        auto result = settings.Create();
        return result.IsValid() ? result.Get() : nullptr;
    }

    void BoxCollider::DrawShape(const glm::mat4& worldMatrix) const
    {
        DebugPass::DrawBox(glm::vec3(0.0f), HalfExtents, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &worldMatrix);
    }

    void BoxCollider::Inspect()
    {
        ImGui::DragFloat3("Half Extents", &HalfExtents.x, 0.01f, 0.001f, 1000.0f);
    }

    void BoxCollider::Serialize(nlohmann::json& out) const
    {
        Collider::Serialize(out);
        out["half_extents"] = { HalfExtents.x, HalfExtents.y, HalfExtents.z };
    }

    void BoxCollider::Deserialize(const nlohmann::json& in)
    {
        Collider::Deserialize(in);
        if (in.contains("half_extents"))
            HalfExtents = glm::vec3(in["half_extents"][0], in["half_extents"][1], in["half_extents"][2]);
    }

} // namespace Termina

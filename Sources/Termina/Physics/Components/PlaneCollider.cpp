#include "PlaneCollider.hpp"

#include <Jolt/Physics/Collision/Shape/PlaneShape.h>

#include <Termina/Renderer/Passes/DebugPass.hpp>
#include <ImGui/imgui.h>

#include <GLM/gtc/type_ptr.hpp>

namespace Termina {

    JPH::ShapeRefC PlaneCollider::CreateShape() const
    {
        glm::vec3 n = glm::normalize(Normal);
        JPH::PlaneShapeSettings settings(JPH::Plane(JPH::Vec3(n.x, n.y, n.z), 0.0f));
        auto result = settings.Create();
        return result.IsValid() ? result.Get() : nullptr;
    }

    void PlaneCollider::DrawShape(const glm::mat4& worldMatrix) const
    {
        glm::vec3 n = glm::normalize(Normal);
        DebugPass::DrawPlane(glm::vec3(0.0f), n, DebugSize, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &worldMatrix);
    }

    void PlaneCollider::Inspect()
    {
        ImGui::DragFloat3("Normal",     glm::value_ptr(Normal),    0.01f, -1.0f, 1.0f);
        ImGui::DragFloat("Debug Size", &DebugSize, 0.1f, 0.1f, 100.0f);
    }

    void PlaneCollider::Serialize(nlohmann::json& out) const
    {
        Collider::Serialize(out);
        out["normal"]     = { Normal.x, Normal.y, Normal.z };
        out["debug_size"] = DebugSize;
    }

    void PlaneCollider::Deserialize(const nlohmann::json& in)
    {
        Collider::Deserialize(in);
        if (in.contains("normal"))     Normal     = glm::vec3(in["normal"][0], in["normal"][1], in["normal"][2]);
        if (in.contains("debug_size")) DebugSize  = in["debug_size"];
    }

} // namespace Termina

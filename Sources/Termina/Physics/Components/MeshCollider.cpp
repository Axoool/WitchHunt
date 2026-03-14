#include "MeshCollider.hpp"

#include <Jolt/Physics/Collision/Shape/MeshShape.h>

#include <Termina/Core/Application.hpp>
#include <Termina/Core/Logger.hpp>
#include <Termina/Asset/AssetSystem.hpp>
#include <Termina/Renderer/Passes/DebugPass.hpp>
#include <Termina/Renderer/UIUtils.hpp>

#include <ImGui/imgui.h>

namespace Termina {

    JPH::ShapeRefC MeshCollider::CreateShape() const
    {
        if (!Mesh)
        {
            TN_WARN("MeshCollider: no mesh asset assigned");
            return nullptr;
        }

        ModelAsset* model = Mesh.Get();
        if (!model || model->CpuPositions.empty() || model->CpuIndices.empty())
        {
            TN_WARN("MeshCollider: mesh has no CPU geometry (CpuPositions/CpuIndices empty)");
            return nullptr;
        }

        JPH::TriangleList triangles;
        triangles.reserve(model->CpuIndices.size() / 3);

        for (size_t i = 0; i + 2 < model->CpuIndices.size(); i += 3)
        {
            const glm::vec3& v0 = model->CpuPositions[model->CpuIndices[i]];
            const glm::vec3& v1 = model->CpuPositions[model->CpuIndices[i + 1]];
            const glm::vec3& v2 = model->CpuPositions[model->CpuIndices[i + 2]];

            triangles.push_back(JPH::Triangle(
                JPH::Float3(v0.x, v0.y, v0.z),
                JPH::Float3(v1.x, v1.y, v1.z),
                JPH::Float3(v2.x, v2.y, v2.z)
            ));
        }

        if (triangles.empty())
        {
            TN_WARN("MeshCollider: no triangles extracted");
            return nullptr;
        }

        JPH::MeshShapeSettings settings(std::move(triangles));
        auto result = settings.Create();
        if (!result.IsValid())
        {
            TN_ERROR("MeshCollider: Jolt failed to create mesh shape");
            return nullptr;
        }
        return result.Get();
    }

    void MeshCollider::DrawShape(const glm::mat4& worldMatrix) const
    {
        if (!Mesh) return;
        ModelAsset* model = Mesh.Get();
        if (!model || model->CpuPositions.empty() || model->CpuIndices.empty()) return;

        const glm::vec4 color(0.0f, 1.0f, 0.5f, 1.0f);
        for (size_t i = 0; i + 2 < model->CpuIndices.size(); i += 3)
        {
            const glm::vec3& v0 = model->CpuPositions[model->CpuIndices[i]];
            const glm::vec3& v1 = model->CpuPositions[model->CpuIndices[i + 1]];
            const glm::vec3& v2 = model->CpuPositions[model->CpuIndices[i + 2]];
            DebugPass::DrawTriangle(v0, v1, v2, color, &worldMatrix);
        }
    }

    void MeshCollider::Inspect()
    {
        const char* label = Mesh.IsValid() ? Mesh.GetPath().c_str() : "None";
        ImGui::Text("Mesh: %s", label);

        UIUtils::TryReceiveAsset<ModelAsset>(Mesh, [&](const std::string& path) {
            auto* assets = Application::GetSystem<AssetSystem>();
            Mesh = assets->Load<ModelAsset>(path);
        });
    }

    void MeshCollider::Serialize(nlohmann::json& out) const
    {
        Collider::Serialize(out);
        out["mesh"] = Mesh.GetPath();
    }

    void MeshCollider::Deserialize(const nlohmann::json& in)
    {
        Collider::Deserialize(in);
        std::string path = in.value("mesh", "");
        if (!path.empty())
        {
            auto* assets = Application::GetSystem<AssetSystem>();
            Mesh = assets->Load<ModelAsset>(path);
        }
    }

} // namespace Termina

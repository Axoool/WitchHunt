#include "MeshComponent.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/Asset/AssetSystem.hpp>
#include <Termina/Renderer/UIUtils.hpp>

#include <ImGui/imgui.h>

namespace Termina {

    void MeshComponent::Inspect()
    {
        const char* label = Model.IsValid() ? Model.GetPath().c_str() : "None";
        ImGui::Text("Model: %s", label);

        UIUtils::TryReceiveAsset<ModelAsset>(Model, [&](const std::string& path) {
            auto* assets = Application::GetSystem<AssetSystem>();
            Model = assets->Load<ModelAsset>(path);
        });

        if (Model.IsValid() && Model.Get())
        {
            const ModelAsset* m = Model.Get();
            ImGui::Text("Instances: %zu  Materials: %zu", m->Instances.size(), m->Materials.size());
        }
    }

    void MeshComponent::Serialize(nlohmann::json& out) const
    {
        out["model"] = Model.GetPath();
    }

    void MeshComponent::Deserialize(const nlohmann::json& in)
    {
        std::string modelPath = in.value("model", "");
        if (!modelPath.empty())
        {
            auto* assets = Application::GetSystem<AssetSystem>();
            Model = assets->Load<ModelAsset>(modelPath);
        }
    }

} // namespace Termina

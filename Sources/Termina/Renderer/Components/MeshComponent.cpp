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

            if (!m->Materials.empty() && ImGui::CollapsingHeader("Material Overrides"))
            {
                auto* assets = Application::GetSystem<AssetSystem>();
                for (uint32 i = 0; i < (uint32)m->Materials.size(); ++i)
                {
                    ImGui::PushID((int)i);

                    auto it = MaterialOverrides.find(i);
                    bool hasOverride = (it != MaterialOverrides.end() && it->second.IsValid());

                    const char* slotLabel = hasOverride
                        ? it->second.GetPath().c_str()
                        : "(model default)";

                    ImGui::Text("Slot %u:", i);
                    ImGui::SameLine();
                    ImGui::TextDisabled("%s", slotLabel);

                    // Drop target — accepts a .mat asset dragged onto this slot
                    AssetHandle<MaterialAsset> dropTarget = hasOverride ? it->second : AssetHandle<MaterialAsset>{};
                    UIUtils::TryReceiveAsset<MaterialAsset>(dropTarget, [&, i](const std::string& path) {
                        MaterialOverrides[i] = assets->Load<MaterialAsset>(path);
                    });

                    if (hasOverride)
                    {
                        ImGui::SameLine();
                        if (ImGui::SmallButton("Clear"))
                            MaterialOverrides.erase(it);
                    }

                    ImGui::PopID();
                }
            }
        }
    }

    void MeshComponent::Serialize(nlohmann::json& out) const
    {
        out["model"] = Model.GetPath();

        if (!MaterialOverrides.empty())
        {
            nlohmann::json overrides = nlohmann::json::object();
            for (const auto& [slot, handle] : MaterialOverrides)
            {
                if (handle.IsValid())
                    overrides[std::to_string(slot)] = handle.GetPath();
            }
            out["material_overrides"] = overrides;
        }
    }

    void MeshComponent::Deserialize(const nlohmann::json& in)
    {
        std::string modelPath = in.value("model", "");
        if (!modelPath.empty())
        {
            auto* assets = Application::GetSystem<AssetSystem>();
            Model = assets->Load<ModelAsset>(modelPath);
        }

        if (in.contains("material_overrides"))
        {
            auto* assets = Application::GetSystem<AssetSystem>();
            for (const auto& [key, val] : in["material_overrides"].items())
            {
                uint32 slot = (uint32)std::stoul(key);
                std::string matPath = val.get<std::string>();
                if (!matPath.empty())
                    MaterialOverrides[slot] = assets->Load<MaterialAsset>(matPath);
            }
        }
    }

} // namespace Termina

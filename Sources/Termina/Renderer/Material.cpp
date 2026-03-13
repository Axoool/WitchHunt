#include "Material.hpp"
#include "Renderer.hpp"
#include "UIUtils.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/Asset/AssetSystem.hpp>
#include <Termina/RHI/TextureView.hpp>

#include <ImGui/imgui.h>

namespace Termina {

    // -------------------------------------------------------------------------
    // Serialization
    // -------------------------------------------------------------------------

    void Material::Save(nlohmann::json& out)
    {
        out["albedo_texture"]   = AlbedoTexture.GetPath();
        out["normal_texture"]   = NormalTexture.GetPath();
        out["orm_texture"]      = ORMTexture.GetPath();
        out["emissive_texture"] = EmissiveTexture.GetPath();

        out["alpha_test"]          = AlphaTest;
        out["color"]               = { Color.r, Color.g, Color.b };
        out["override_metallic"]   = OverrideMetallic;
        out["override_roughness"]  = OverrideRoughness;
        out["metallic_factor"]     = MetallicFactor;
        out["roughness_factor"]    = RoughnessFactor;
    }

    void Material::Load(const nlohmann::json& in)
    {
        // Texture paths are loaded by MaterialLoader — skip them here.

        if (in.contains("alpha_test"))
            AlphaTest = in["alpha_test"].get<bool>();

        if (in.contains("color") && in["color"].is_array() && in["color"].size() == 3)
        {
            Color.r = in["color"][0].get<float>();
            Color.g = in["color"][1].get<float>();
            Color.b = in["color"][2].get<float>();
        }

        if (in.contains("override_metallic"))
            OverrideMetallic = in["override_metallic"].get<bool>();

        if (in.contains("override_roughness"))
            OverrideRoughness = in["override_roughness"].get<bool>();

        if (in.contains("metallic_factor"))
            MetallicFactor = in["metallic_factor"].get<float>();

        if (in.contains("roughness_factor"))
            RoughnessFactor = in["roughness_factor"].get<float>();
    }

    // -------------------------------------------------------------------------
    // Inspector UI
    // -------------------------------------------------------------------------

    static void DrawTextureSlot(const char* label, AssetHandle<TextureAsset>& handle,
                                RendererSystem* renderer)
    {
        ImGui::Text("%s", label);
        ImGui::SameLine();

        // Show texture preview thumbnail if loaded
        if (handle.IsValid() && handle->GetTexture())
        {
            TextureViewDesc viewDesc = TextureViewDesc().CreateDefault(
                handle->GetTexture(), TextureViewType::SHADER_READ,
                TextureViewDimension::TEXTURE_2D);
            TextureView* view = renderer->GetResourceViewCache()->GetTextureView(viewDesc);
            ImGui::Image(view->GetBindlessIndex(), ImVec2(48, 48));
        }
        else
        {
            ImGui::Button("None##tex", ImVec2(48, 48));
        }

        // Accept texture drag-drop
        UIUtils::TryReceiveAsset<TextureAsset>(handle, [&](const std::string& path) {
            auto* assets = Application::GetSystem<AssetSystem>();
            handle = assets->Load<TextureAsset>(path);
        });

        if (handle.IsValid())
        {
            ImGui::SameLine();
            ImGui::TextUnformatted(handle.GetPath().c_str());
        }
    }

    void Material::Inspect()
    {
        RendererSystem* renderer = Application::GetSystem<RendererSystem>();

        ImGui::PushID(0); DrawTextureSlot("Albedo  ", AlbedoTexture,   renderer); ImGui::PopID();
        ImGui::PushID(1); DrawTextureSlot("Normal  ", NormalTexture,   renderer); ImGui::PopID();
        ImGui::PushID(2); DrawTextureSlot("ORM     ", ORMTexture,       renderer); ImGui::PopID();
        ImGui::PushID(3); DrawTextureSlot("Emissive", EmissiveTexture, renderer); ImGui::PopID();

        ImGui::Separator();

        ImGui::ColorEdit3("Color", &Color.r);
        ImGui::Checkbox("Alpha Test", &AlphaTest);

        ImGui::Separator();

        ImGui::Checkbox("Override Metallic",   &OverrideMetallic);
        if (OverrideMetallic)
            ImGui::SliderFloat("Metallic Factor", &MetallicFactor, 0.0f, 1.0f);

        ImGui::Checkbox("Override Roughness",  &OverrideRoughness);
        if (OverrideRoughness)
            ImGui::SliderFloat("Roughness Factor", &RoughnessFactor, 0.0f, 1.0f);
    }

} // namespace Termina

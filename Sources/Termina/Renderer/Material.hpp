#pragma once

#include <JSON/json.hpp>
#include <GLM/glm.hpp>
#include <Termina/Asset/AssetHandle.hpp>
#include <Termina/Asset/Texture/TextureAsset.hpp>

namespace Termina {
    struct Material : public IInspectable
    {
        AssetHandle<TextureAsset> AlbedoTexture;
        AssetHandle<TextureAsset> NormalTexture;
        AssetHandle<TextureAsset> ORMTexture;
        AssetHandle<TextureAsset> EmissiveTexture;

        bool AlphaTest = false;
        glm::vec3 Color;

        bool OverrideMetallic = false;
        bool OverrideRoughness = false;
        float MetallicFactor = 0.0f;
        float RoughnessFactor = 0.0f;

        void Save(nlohmann::json& out);
        void Load(const nlohmann::json& in);
        void Inspect() override;
    };

    /// GPU-side material entry stored in the global bindless material buffer (32 bytes).
    /// Matches the HLSL GPUMaterial struct in Common/Scene.hlsli.
    struct GPUMaterial
    {
        int AlbedoIndex;    // Bindless SRV index (-1 = white)
        int NormalIndex;    // Bindless SRV index (-1 = vertex normal)
        int ORMIndex;       // Bindless SRV index (-1 = defaults)
        int EmissiveIndex;  // Bindless SRV index (-1 = black)
        int SamplerIndex;   // Bindless sampler index
        int _pad[3];
    };
    static_assert(sizeof(GPUMaterial) == 32, "GPUMaterial must be 32 bytes");
}

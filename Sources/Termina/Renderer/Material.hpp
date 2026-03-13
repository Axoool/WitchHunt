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

    /// GPU-side material representation.
    struct GPUMaterial
    {
        uint AlbedoTexture;
        uint NormalTexture;
        uint ORMTexture;
        uint EmissiveTexture;

        bool AlphaTest;
        bool OverrideMetallic;
        bool OverrideRoughness;
        float MetallicFactor;

        float RoughnessFactor;
        glm::vec3 Color;
    };
}

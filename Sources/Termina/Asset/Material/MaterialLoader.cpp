#include "MaterialLoader.hpp"
#include "Core/Logger.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/Asset/AssetSystem.hpp>

#include <JSON/json.hpp>
#include <fstream>

namespace Termina {

    MaterialAsset* MaterialLoader::LoadFromDisk(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            TN_ERROR("Failed to open material file: %s", path.c_str());
            return nullptr;
        }

        nlohmann::json json;
        try
        {
            file >> json;
        }
        catch (const nlohmann::json::exception& e)
        {
            TN_ERROR("Failed to parse material file %s: %s", path.c_str(), e.what());
            return nullptr;
        }

        Material material;
        material.Load(json);

        // Load texture handles via AssetSystem
        auto* assets = Application::GetSystem<AssetSystem>();

        auto loadTexture = [&](const char* key) -> AssetHandle<TextureAsset>
        {
            if (json.contains(key) && json[key].is_string())
            {
                std::string texPath = json[key].get<std::string>();
                if (!texPath.empty())
                    return assets->Load<TextureAsset>(texPath);
            }
            return {};
        };

        material.AlbedoTexture   = loadTexture("albedo_texture");
        material.NormalTexture   = loadTexture("normal_texture");
        material.ORMTexture      = loadTexture("orm_texture");
        material.EmissiveTexture = loadTexture("emissive_texture");

        return new MaterialAsset(std::move(material), path);
    }

    MaterialAsset* MaterialLoader::LoadFromPackage(const uint8* /*data*/, size_t /*size*/)
    {
        return nullptr;
    }

    std::vector<uint8> MaterialLoader::ExportToPackage(const MaterialAsset* /*asset*/)
    {
        return {};
    }

} // namespace Termina

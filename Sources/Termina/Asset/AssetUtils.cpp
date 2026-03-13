#include "AssetUtils.hpp"
#include "Core/FileSystem.hpp"
#include <unordered_map>
#include <vector>

namespace Termina {
    std::unordered_map<AssetType, std::vector<std::string>> s_AssetMap = {
        { AssetType::Audio, { ".wav", ".mp3", ".ogg" } },
        { AssetType::Model, { ".gltf" } },
        { AssetType::Texture, { ".png", ".jpg", ".jpeg" } },
        { AssetType::Material, { ".mat" } },
        { AssetType::Scene, { ".trw" } },
        { AssetType::Prefab, { ".trp" } },
        { AssetType::Skybox, { ".sky" } },
        { AssetType::Font, { ".ttf" } }
    };

    bool AssetUtils::IsAssetType(const std::string& path, AssetType type)
    {
        const auto& extensions = s_AssetMap[type];
        for (const auto& ext : extensions) {
            if (FileSystem::HasExtension(path, ext)) {
                return true;
            }
        }
        return false;
    }
}

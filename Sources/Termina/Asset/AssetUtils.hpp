#pragma once

#include <string>

namespace Termina {
    enum class AssetType
    {
        Audio,
        Model,
        Texture,
        Material,
        Scene,
        Prefab,
        Skybox,
        Font
    };

    class AssetUtils
    {
    public:
        static bool IsAssetType(const std::string& path, AssetType type);
    };
}

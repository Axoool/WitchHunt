#pragma once

#include <Termina/Asset/IAssetLoader.hpp>
#include "ModelAsset.hpp"

namespace Termina {

    /// Loads GLTF model files into a ModelAsset with a global vertex/index buffer.
    /// Automatically generates sidecar .mat files for each GLTF material if they
    /// don't already exist.
    class ModelLoader : public IAssetLoader<ModelAsset>
    {
    public:
        ModelAsset* LoadFromDisk(const std::string& path) override;

        /// Not yet implemented.
        ModelAsset* LoadFromPackage(const uint8* data, size_t size) override;

        /// Not yet implemented.
        std::vector<uint8> ExportToPackage(const ModelAsset* asset) override;
    };

} // namespace Termina

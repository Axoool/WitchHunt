#pragma once

#include <Termina/Asset/IAssetLoader.hpp>
#include "MaterialAsset.hpp"

namespace Termina {

    /// Loads material assets from .mat JSON files on disk.
    class MaterialLoader : public IAssetLoader<MaterialAsset>
    {
    public:
        MaterialAsset* LoadFromDisk(const std::string& path) override;

        /// Not yet implemented — package loading will be added with the asset package system.
        MaterialAsset* LoadFromPackage(const uint8* data, size_t size) override;

        /// Not yet implemented — package export will be added with the asset package system.
        std::vector<uint8> ExportToPackage(const MaterialAsset* asset) override;
    };

} // namespace Termina

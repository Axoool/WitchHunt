#pragma once

#include <Termina/Core/IInspectable.hpp>
#include <Termina/Renderer/Material.hpp>
#include <string>

namespace Termina {

    /// A material asset loaded from a .mat JSON file.
    class MaterialAsset : public IInspectable
    {
    public:
        explicit MaterialAsset(Material material, std::string path = {})
            : m_Material(std::move(material)), m_Path(std::move(path))
        {
        }

        Material&       GetMaterial()       { return m_Material; }
        const Material& GetMaterial() const { return m_Material; }

        void Save();
        void Inspect() override;

    private:
        Material    m_Material;
        std::string m_Path;
    };

} // namespace Termina

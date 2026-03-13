#include "MaterialAsset.hpp"

#include <ImGui/imgui.h>
#include <JSON/json.hpp>
#include <fstream>

namespace Termina {

    void MaterialAsset::Save()
    {
        if (m_Path.empty())
            return;

        nlohmann::json json;
        m_Material.Save(json);

        std::ofstream f(m_Path);
        f << json.dump(2);
    }

    void MaterialAsset::Inspect()
    {
        m_Material.Inspect();

        ImGui::Separator();

        if (ImGui::Button("Save"))
            Save();
    }

} // namespace Termina

#include "AssetSystem.hpp"
#include "Texture/TextureLoader.hpp"
#include "Audio/AudioLoader.hpp"
#include "Material/MaterialLoader.hpp"
#include "Model/ModelLoader.hpp"

#include <ImGui/imgui.h>
#include <map>
#include <vector>

namespace Termina {
    AssetSystem::AssetSystem()
    {
        m_TextureLoader = std::make_unique<TextureLoader>();
        RegisterLoader<TextureAsset>(m_TextureLoader.get());

        m_AudioLoader = std::make_unique<AudioLoader>();
        RegisterLoader<AudioAsset>(m_AudioLoader.get());

        m_MaterialLoader = std::make_unique<MaterialLoader>();
        RegisterLoader<MaterialAsset>(m_MaterialLoader.get());

        m_ModelLoader = std::make_unique<ModelLoader>();
        RegisterLoader<ModelAsset>(m_ModelLoader.get());
    }

    AssetSystem::~AssetSystem()
    {
        ProcessPendingDeletions();
        for (auto& [id, record] : m_Records)
            record.Deleter(record.Asset);
        for (auto& [type, defaultEntry] : m_Defaults)
            defaultEntry.second(defaultEntry.first);
    }

    void AssetSystem::AddRef(uint32 id)
    {
        auto it = m_Records.find(id);
        if (it != m_Records.end())
            it->second.RefCount++;
    }

    void AssetSystem::Release(uint32 id)
    {
        auto it = m_Records.find(id);
        if (it != m_Records.end() && it->second.RefCount > 0)
            it->second.RefCount--;
    }

    void AssetSystem::Clean(int32 minRefCount)
    {
        std::vector<uint32> toEvict;
        for (auto& [id, record] : m_Records)
        {
            if (record.RefCount <= minRefCount)
                toEvict.push_back(id);
        }

        for (uint32 id : toEvict)
        {
            auto& record = m_Records[id];
            m_PendingDeletion.push_back({ record.Asset, record.Deleter });
            m_PathToID.erase(record.Path);
            m_Records.erase(id);
        }
    }

    void AssetSystem::PostRender(float /*deltaTime*/)
    {
        if (!m_DefaultsRegistered)
        {
            RegisterDefaults();
            m_DefaultsRegistered = true;
        }
        ProcessPendingDeletions();
    }

    void AssetSystem::RegisterDefaults()
    {
        m_TextureLoader->RegisterDefault(*this);
    }

    void AssetSystem::ProcessPendingDeletions()
    {
        for (auto& entry : m_PendingDeletion)
            entry.Deleter(entry.Asset);
        m_PendingDeletion.clear();
    }

    void AssetSystem::ShowDebugWindow(bool* open)
    {
        if (!ImGui::Begin("Asset System", open))
        {
            ImGui::End();
            return;
        }

        ImGui::Text("Total loaded: %zu", m_Records.size());
        ImGui::Text("Pending deletion: %zu", m_PendingDeletion.size());
        ImGui::Separator();

        // Group records by TypeName
        std::map<std::string, std::vector<std::pair<uint32, const AssetRecord*>>> byType;
        for (auto& [id, record] : m_Records)
            byType[record.TypeName].emplace_back(id, &record);

        for (auto& [typeName, entries] : byType)
        {
            std::string header = typeName + " (" + std::to_string(entries.size()) + ")";
            if (ImGui::CollapsingHeader(header.c_str()))
            {
                if (ImGui::BeginTable(typeName.c_str(), 3,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
                {
                    ImGui::TableSetupColumn("ID",       ImGuiTableColumnFlags_WidthFixed, 40.f);
                    ImGui::TableSetupColumn("Refs",     ImGuiTableColumnFlags_WidthFixed, 40.f);
                    ImGui::TableSetupColumn("Path",     ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();

                    for (auto& [id, record] : entries)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0); ImGui::Text("%u", id);
                        ImGui::TableSetColumnIndex(1); ImGui::Text("%d", record->RefCount);
                        ImGui::TableSetColumnIndex(2); ImGui::TextUnformatted(record->Path.c_str());
                    }

                    ImGui::EndTable();
                }
            }
        }

        ImGui::End();
    }

} // namespace Termina

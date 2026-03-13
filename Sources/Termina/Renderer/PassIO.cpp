#include "PassIO.hpp"

namespace Termina {
    void PassIO::RegisterTexture(const std::string& name, RendererTexture* texture)
    {
        m_Textures[name] = texture;
    }

    void PassIO::RegisterBuffer(const std::string& name, RendererBuffer* buffer)
    {
        m_Buffers[name] = buffer;
    }

    RendererTexture* PassIO::GetTexture(const std::string& name) const
    {
        auto it = m_Textures.find(name);
        return it != m_Textures.end() ? it->second : nullptr;
    }

    RendererBuffer* PassIO::GetBuffer(const std::string& name) const
    {
        auto it = m_Buffers.find(name);
        return it != m_Buffers.end() ? it->second : nullptr;
    }

    bool PassIO::HasTexture(const std::string& name) const
    {
        return m_Textures.count(name) > 0;
    }

    bool PassIO::HasBuffer(const std::string& name) const
    {
        return m_Buffers.count(name) > 0;
    }

    std::vector<std::string> PassIO::GetTextureNames() const
    {
        std::vector<std::string> names;
        names.reserve(m_Textures.size());
        for (auto& [name, _] : m_Textures)
            names.push_back(name);
        return names;
    }

    std::vector<std::string> PassIO::GetBufferNames() const
    {
        std::vector<std::string> names;
        names.reserve(m_Buffers.size());
        for (auto& [name, _] : m_Buffers)
            names.push_back(name);
        return names;
    }

    void PassIO::Clear()
    {
        m_Textures.clear();
        m_Buffers.clear();
    }
}

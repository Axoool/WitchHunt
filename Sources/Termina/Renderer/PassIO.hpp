#pragma once

#include <Termina/RHI/Texture.hpp>
#include <Termina/RHI/Buffer.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace Termina {
    /// Represents the input/output resources for a render pass, shared across multiple passes.
    class PassIO
    {
    public:
        void RegisterTexture(const std::string& name, RendererTexture* texture);
        void RegisterBuffer(const std::string& name, RendererBuffer* buffer);

        RendererTexture* GetTexture(const std::string& name) const;
        RendererBuffer* GetBuffer(const std::string& name) const;

        bool HasTexture(const std::string& name) const;
        bool HasBuffer(const std::string& name) const;

        std::vector<std::string> GetTextureNames() const;
        std::vector<std::string> GetBufferNames() const;

        void Clear();

    private:
        std::unordered_map<std::string, RendererTexture*> m_Textures;
        std::unordered_map<std::string, RendererBuffer*> m_Buffers;
    };
}

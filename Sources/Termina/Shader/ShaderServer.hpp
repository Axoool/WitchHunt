#pragma once

#include <Termina/Core/FileSystem.hpp>

#include <Termina/RHI/Device.hpp>
#include <Termina/RHI/Pipeline.hpp>
#include <Termina/RHI/RenderPipeline.hpp>
#include <Termina/RHI/ComputePipeline.hpp>

#include <unordered_map>

namespace Termina {
    class ShaderServer
    {
    public:
        ShaderServer();
        ~ShaderServer() = default;

        void Clear();

        void ConnectDevice(RendererDevice* device);
        void WatchPipeline(const std::string& path, const RenderPipelineDesc& desc, PipelineType type);
        void WatchPipeline(const std::string& path, PipelineType type);

        RenderPipeline* GetPipeline(const std::string& path, const std::vector<std::string>& defines = {"DEFAULT"});
        ComputePipeline* GetComputePipeline(const std::string& path, const std::vector<std::string>& defines = {"DEFAULT"});

        void ReloadModifiedPipelines();
        void ProcessPendingDeletions();

        void ShowDebugWindow(bool* open = nullptr);
    private:
        struct ShaderEntry
        {
            PipelineType Type;

            std::unordered_map<std::string, RenderPipeline*> GraphicsPipelines;
            RenderPipelineDesc GraphicsDesc;

            std::unordered_map<std::string, ComputePipeline*> ComputePipelines;

            FileSystem::Watch FileHandle;
        };

        RendererDevice* m_Device;
        std::unordered_map<std::string, ShaderEntry> m_Shaders;
        std::vector<Pipeline*> m_PendingDeletion; // Pipelines to delete next frame
    };
}

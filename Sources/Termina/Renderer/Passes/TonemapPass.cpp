#include "TonemapPass.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/Renderer/Renderer.hpp>
#include <Termina/Shader/ShaderManager.hpp>
#include <Termina/Shader/ShaderServer.hpp>
#include "RHI/TextureView.hpp"

namespace Termina {

    TonemapPass::TonemapPass()
    {
        RendererDevice* device = Application::GetSystem<RendererSystem>()->GetDevice();
        uint32 width  = Application::Get().GetWindow()->GetPixelWidth();
        uint32 height = Application::Get().GetWindow()->GetPixelHeight();

        m_LDRTexture = device->CreateTexture(TextureDesc()
            .SetSize(width, height)
            .SetFormat(TextureFormat::RGBA8_UNORM)
            .SetUsage(TextureUsage::SHADER_READ | TextureUsage::SHADER_WRITE | TextureUsage::RENDER_TARGET));
        m_LDRTexture->SetName("LDR Output");

        ShaderServer& server = Application::GetSystem<ShaderManager>()->GetShaderServer();
        server.WatchPipeline("__TERMINA__/CORE_SHADERS/Tonemap.hlsl", PipelineType::Compute);
    }

    TonemapPass::~TonemapPass()
    {
        delete m_LDRTexture;
    }

    void TonemapPass::Resize(int32 width, int32 height)
    {
        m_LDRTexture->Resize(width, height);
    }

    void TonemapPass::Execute(RenderPassExecuteInfo& info)
    {
        ShaderServer& server = Application::GetSystem<ShaderManager>()->GetShaderServer();

        RendererTexture* hdrTex = info.IO->GetTexture("HDRColor");

        TextureView* hdrSRV = info.ViewCache->GetTextureView(
            TextureViewDesc::CreateDefault(hdrTex, TextureViewType::SHADER_READ, TextureViewDimension::TEXTURE_2D));
        TextureView* ldrUAV = info.ViewCache->GetTextureView(
            TextureViewDesc::CreateDefault(m_LDRTexture, TextureViewType::SHADER_WRITE, TextureViewDimension::TEXTURE_2D));

        struct TonemapPushConstants
        {
            int32 InputIndex;
            int32 OutputIndex;
            int32 Width;
            int32 Height;
        };

        TonemapPushConstants pc;
        pc.InputIndex  = hdrSRV->GetBindlessIndex();
        pc.OutputIndex = ldrUAV->GetBindlessIndex();
        pc.Width       = info.Width;
        pc.Height      = info.Height;

        ComputeEncoder* ce = info.Ctx->CreateComputeEncoder("Tonemap Pass");
        ce->SetPipeline(server.GetComputePipeline("__TERMINA__/CORE_SHADERS/Tonemap.hlsl"));
        ce->SetConstants(sizeof(pc), &pc);
        ce->Dispatch(info.Width, info.Height, 1, 8, 8, 1);
        ce->End();

        info.IO->RegisterTexture("RendererOutput", m_LDRTexture);

        info.Ctx->Barrier(TextureBarrier()
            .SetTargetTexture(m_LDRTexture)
            .SetNewLayout(TextureLayout::READ_ONLY)
            .SetDstAccess(ResourceAccess::SHADER_READ)
            .SetDstStage(PipelineStage::PIXEL_SHADER));
    }

} // namespace Termina

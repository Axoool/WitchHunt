#include "MeshPass.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/Asset/AssetSystem.hpp>
#include <Termina/Asset/Model/ModelAsset.hpp>
#include <Termina/Asset/Material/MaterialAsset.hpp>
#include <Termina/Renderer/Components/MeshComponent.hpp>
#include <Termina/Renderer/Renderer.hpp>
#include <Termina/Shader/ShaderManager.hpp>
#include <Termina/Shader/ShaderServer.hpp>
#include "World/Components/Transform.hpp"

#include "RHI/TextureView.hpp"

#include <GLM/glm.hpp>

namespace Termina {

    MeshPass::MeshPass()
    {
        RendererDevice* device = Application::GetSystem<RendererSystem>()->GetDevice();
        uint32 width  = Application::Get().GetWindow()->GetPixelWidth();
        uint32 height = Application::Get().GetWindow()->GetPixelHeight();

        TextureDesc colorDesc = TextureDesc()
            .SetSize(width, height)
            .SetFormat(TextureFormat::RGBA8_UNORM)
            .SetUsage(TextureUsage::RENDER_TARGET | TextureUsage::SHADER_READ | TextureUsage::SHADER_WRITE);
        m_ColorTexture = device->CreateTexture(colorDesc);
        m_ColorTexture->SetName("Mesh Color Texture");

        TextureDesc depthDesc = TextureDesc()
            .SetSize(width, height)
            .SetFormat(TextureFormat::D32_FLOAT)
            .SetUsage(TextureUsage::DEPTH_TARGET | TextureUsage::SHADER_READ);
        m_DepthTexture = device->CreateTexture(depthDesc);
        m_DepthTexture->SetName("Mesh Depth Texture");

        m_Sampler = device->CreateSampler(SamplerDesc()
            .SetFilter(SamplerFilter::LINEAR)
            .SetAddress(SamplerAddressMode::WRAP)
            .EnableMips(true));

        RenderPipelineDesc rpDesc = RenderPipelineDesc()
            .AddColorAttachmentFormat(TextureFormat::RGBA8_UNORM)
            .SetName("Mesh Pipeline")
            .SetCullMode(PipelineCullMode::FRONT)
            .SetDepthAttachmentFormat(TextureFormat::D32_FLOAT)
            .SetDepthCompareOp(PipelineCompareOp::LESS)
            .SetDepthReadEnabled(true)
            .SetDepthWriteEnabled(true);

        ShaderServer& server = Application::GetSystem<ShaderManager>()->GetShaderServer();
        server.WatchPipeline("Assets/Shaders/Mesh.hlsl", rpDesc, PipelineType::Graphics);
    }

    MeshPass::~MeshPass()
    {
        delete m_Sampler;
        delete m_ColorTexture;
        delete m_DepthTexture;
    }

    void MeshPass::Resize(int32 width, int32 height)
    {
        m_ColorTexture->Resize(width, height);
        m_DepthTexture->Resize(width, height);
    }

    void MeshPass::Execute(RenderPassExecuteInfo& info)
    {
        ShaderServer& server = Application::GetSystem<ShaderManager>()->GetShaderServer();

        TextureView* colorView = info.ViewCache->GetTextureView(
            TextureViewDesc().SetTexture(m_ColorTexture).SetType(TextureViewType::RENDER_TARGET));
        TextureView* depthView = info.ViewCache->GetTextureView(
            TextureViewDesc().SetTexture(m_DepthTexture).SetType(TextureViewType::DEPTH_TARGET));

        RenderEncoderInfo rei = RenderEncoderInfo()
            .AddColorAttachment(colorView, true, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))
            .SetDepthAttachment(depthView)
            .SetName("Mesh Pass")
            .SetDimensions(info.Width, info.Height);

        RenderEncoder* re = info.Ctx->CreateRenderEncoder(rei);
        re->SetPipeline(server.GetPipeline("Assets/Shaders/Mesh.hlsl"));
        re->SetViewport(0.0f, 0.0f, static_cast<float>(info.Width), static_cast<float>(info.Height));
        re->SetScissorRect(0, 0, info.Width, info.Height);

        // Must match HLSL PushConstants exactly.
        struct MeshPushConstants
        {
            glm::mat4 MVP;
            int32 VertexBufferIndex;
            int32 BaseVertex;   // added to SV_VertexID in shader
                                // (Metal: DrawIndexed baseVertex doesn't reach SV_VertexID for bindless fetches)
            int32 AlbedoIndex;  // -1 = no texture, shader uses white
            int32 SamplerIndex;
        };

        const int32 samplerIndex = m_Sampler->GetBindlessHandle();

        for (auto& actor : info.CurrentWorld->GetActors())
        {
            if (!actor->HasComponent<MeshComponent>()) continue;

            auto& meshComp = actor->GetComponent<MeshComponent>();
            ModelAsset* model = meshComp.Model.Get();
            if (!model || !model->VertexBuffer || !model->IndexBuffer || !model->VertexView)
                continue;

            re->SetIndexBuffer(model->IndexBuffer);

            const int32 vbIndex    = model->VertexView->GetBindlessHandle();
            const glm::mat4 actorWorld = actor->GetComponent<Transform>().GetWorldMatrix();

            for (const MeshInstance& inst : model->Instances)
            {
                if (inst.LODs.empty()) continue;
                const MeshLOD& lod = inst.LODs[0];

                // Resolve albedo texture bindless index (-1 = absent, shader uses white)
                int32 albedoIndex = -1;
                if (inst.MaterialIndex < model->Materials.size())
                {
                    MaterialAsset* mat = model->Materials[inst.MaterialIndex].Get();
                    if (mat && mat->GetMaterial().AlbedoTexture.IsValid())
                    {
                        RendererTexture* tex = mat->GetMaterial().AlbedoTexture->GetTexture();
                        if (tex)
                        {
                            TextureView* tv = info.ViewCache->GetTextureView(
                                TextureViewDesc::CreateDefault(tex,
                                    TextureViewType::SHADER_READ,
                                    TextureViewDimension::TEXTURE_2D));
                            albedoIndex = tv->GetBindlessIndex();
                        }
                    }
                }

                MeshPushConstants pc;
                pc.MVP               = info.CurrentCamera.ViewProjection * actorWorld * inst.LocalTransform;
                pc.VertexBufferIndex = vbIndex;
                pc.BaseVertex        = static_cast<int32>(inst.BaseVertex);
                pc.AlbedoIndex       = albedoIndex;
                pc.SamplerIndex      = samplerIndex;

                re->SetConstants(sizeof(pc), &pc);

                // baseVertex=0: Metal's DrawIndexed baseVertex doesn't propagate into SV_VertexID
                // for bindless ResourceDescriptorHeap fetches; shader adds BaseVertex manually.
                re->DrawIndexed(lod.IndexCount, 1, lod.IndexOffset, 0, 0);
            }
        }

        re->End();

        info.IO->RegisterTexture("RendererOutput", m_ColorTexture);

        TextureBarrier toSRV = TextureBarrier()
            .SetTargetTexture(m_ColorTexture)
            .SetNewLayout(TextureLayout::READ_ONLY)
            .SetDstAccess(ResourceAccess::SHADER_READ)
            .SetDstStage(PipelineStage::PIXEL_SHADER);
        info.Ctx->Barrier(toSRV);
    }

} // namespace Termina

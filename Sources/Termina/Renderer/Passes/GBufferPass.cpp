#include "GBufferPass.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/Asset/AssetSystem.hpp>
#include <Termina/Asset/Model/ModelAsset.hpp>
#include <Termina/Asset/Material/MaterialAsset.hpp>
#include <Termina/Renderer/Components/MeshComponent.hpp>
#include <Termina/Renderer/Renderer.hpp>
#include <Termina/Renderer/GPUInstance.hpp>
#include <Termina/Renderer/Material.hpp>
#include <Termina/Shader/ShaderManager.hpp>
#include <Termina/Shader/ShaderServer.hpp>
#include "World/Components/Transform.hpp"
#include "RHI/TextureView.hpp"

#include <GLM/glm.hpp>

#include <unordered_map>
#include <cstring>

namespace Termina {

    GBufferPass::GBufferPass()
    {
        RendererDevice* device = Application::GetSystem<RendererSystem>()->GetDevice();
        uint32 width  = Application::Get().GetWindow()->GetPixelWidth();
        uint32 height = Application::Get().GetWindow()->GetPixelHeight();

        auto colorRT = TextureUsage::RENDER_TARGET | TextureUsage::SHADER_READ;
        auto floatRT = TextureUsage::RENDER_TARGET | TextureUsage::SHADER_READ;

        m_AlbedoTexture = device->CreateTexture(TextureDesc()
            .SetSize(width, height)
            .SetFormat(TextureFormat::RGBA8_UNORM)
            .SetUsage(colorRT));
        m_AlbedoTexture->SetName("GBuffer Albedo");

        m_NormalsTexture = device->CreateTexture(TextureDesc()
            .SetSize(width, height)
            .SetFormat(TextureFormat::RGBA16_FLOAT)
            .SetUsage(floatRT));
        m_NormalsTexture->SetName("GBuffer Normals");

        m_ORMTexture = device->CreateTexture(TextureDesc()
            .SetSize(width, height)
            .SetFormat(TextureFormat::RGBA8_UNORM)
            .SetUsage(colorRT));
        m_ORMTexture->SetName("GBuffer ORM");

        m_EmissiveTexture = device->CreateTexture(TextureDesc()
            .SetSize(width, height)
            .SetFormat(TextureFormat::RGBA16_FLOAT)
            .SetUsage(floatRT));
        m_EmissiveTexture->SetName("GBuffer Emissive");

        m_MotionVecTexture = device->CreateTexture(TextureDesc()
            .SetSize(width, height)
            .SetFormat(TextureFormat::RGBA16_FLOAT)
            .SetUsage(floatRT));
        m_MotionVecTexture->SetName("GBuffer MotionVectors");

        m_DepthTexture = device->CreateTexture(TextureDesc()
            .SetSize(width, height)
            .SetFormat(TextureFormat::D32_FLOAT)
            .SetUsage(TextureUsage::DEPTH_TARGET | TextureUsage::SHADER_READ));
        m_DepthTexture->SetName("GBuffer Depth");

        m_Sampler = device->CreateSampler(SamplerDesc()
            .SetFilter(SamplerFilter::LINEAR)
            .SetAddress(SamplerAddressMode::WRAP)
            .EnableMips(true));

        // --- Global scene buffers ---
        // SHADER_READ | TRANSFER → eCpuToGpu: host-visible + GPU-readable as storage buffer.
        // Persistently mapped; written each frame without staging.
        m_InstanceBuffer = device->CreateBuffer(BufferDesc()
            .SetSize(sizeof(GPUInstance) * MAX_INSTANCES)
            .SetStride(sizeof(GPUInstance))
            .SetUsage(BufferUsage::SHADER_READ | BufferUsage::TRANSFER));
        m_InstanceBuffer->SetName("Instance Buffer");
        m_InstanceMapped = m_InstanceBuffer->Map();

        m_InstanceBufView = device->CreateBufferView(BufferViewDesc()
            .SetBuffer(m_InstanceBuffer)
            .SetType(BufferViewType::SHADER_READ));

        m_MaterialBuffer = device->CreateBuffer(BufferDesc()
            .SetSize(sizeof(GPUMaterial) * MAX_MATERIALS)
            .SetStride(sizeof(GPUMaterial))
            .SetUsage(BufferUsage::SHADER_READ | BufferUsage::TRANSFER));
        m_MaterialBuffer->SetName("Material Buffer");
        m_MaterialMapped = m_MaterialBuffer->Map();

        m_MaterialBufView = device->CreateBufferView(BufferViewDesc()
            .SetBuffer(m_MaterialBuffer)
            .SetType(BufferViewType::SHADER_READ));

        // --- Pipeline ---
        RenderPipelineDesc rpDesc = RenderPipelineDesc()
            .SetName("GBuffer Pipeline")
            .AddColorAttachmentFormat(TextureFormat::RGBA8_UNORM)   // albedo
            .AddColorAttachmentFormat(TextureFormat::RGBA16_FLOAT)  // normals
            .AddColorAttachmentFormat(TextureFormat::RGBA8_UNORM)   // ORM
            .AddColorAttachmentFormat(TextureFormat::RGBA16_FLOAT)  // emissive
            .AddColorAttachmentFormat(TextureFormat::RGBA16_FLOAT)  // motion vectors
            .SetDepthAttachmentFormat(TextureFormat::D32_FLOAT)
#if defined(TRMN_MACOS)
            .SetCullMode(PipelineCullMode::FRONT)
#else
            .SetCullMode(PipelineCullMode::BACK)
#endif
            .SetDepthCompareOp(PipelineCompareOp::LESS)
            .SetDepthReadEnabled(true)
            .SetDepthWriteEnabled(true)
            .SetPushConstantSize(128);

        ShaderServer& server = Application::GetSystem<ShaderManager>()->GetShaderServer();
        server.WatchPipeline("__TERMINA__/CORE_SHADERS/GBuffer.hlsl", rpDesc, PipelineType::Graphics);
    }

    GBufferPass::~GBufferPass()
    {
        m_InstanceBuffer->Unmap();
        m_MaterialBuffer->Unmap();

        delete m_InstanceBufView;
        delete m_MaterialBufView;
        delete m_InstanceBuffer;
        delete m_MaterialBuffer;

        delete m_Sampler;
        delete m_AlbedoTexture;
        delete m_NormalsTexture;
        delete m_ORMTexture;
        delete m_EmissiveTexture;
        delete m_MotionVecTexture;
        delete m_DepthTexture;
    }

    void GBufferPass::Resize(int32 width, int32 height)
    {
        m_AlbedoTexture->Resize(width, height);
        m_NormalsTexture->Resize(width, height);
        m_ORMTexture->Resize(width, height);
        m_EmissiveTexture->Resize(width, height);
        m_MotionVecTexture->Resize(width, height);
        m_DepthTexture->Resize(width, height);
    }

    void GBufferPass::Execute(RenderPassExecuteInfo& info)
    {
        ShaderServer& server = Application::GetSystem<ShaderManager>()->GetShaderServer();

        // --- Build instance and material lists, writing directly into mapped buffers ---
        GPUInstance* instanceDst = static_cast<GPUInstance*>(m_InstanceMapped);
        GPUMaterial* materialDst = static_cast<GPUMaterial*>(m_MaterialMapped);

        int32 instanceCount = 0;
        int32 materialCount = 0;

        std::unordered_map<MaterialAsset*, int32> materialMap;

        const int32 samplerIndex = m_Sampler->GetBindlessHandle();

        auto resolveTextureIndex = [&](const AssetHandle<TextureAsset>& handle) -> int32 {
            if (!handle.IsValid()) return -1;
            RendererTexture* tex = handle->GetTexture();
            if (!tex) return -1;
            TextureView* tv = info.ViewCache->GetTextureView(
                TextureViewDesc::CreateDefault(tex, TextureViewType::SHADER_READ, TextureViewDimension::TEXTURE_2D));
            return tv->GetBindlessIndex();
        };

        // Collect draw calls while filling scene buffers
        struct DrawCall { int32 instanceID; RendererBuffer* indexBuffer; uint32 indexCount; uint32 indexOffset; };
        std::vector<DrawCall> draws;

        for (auto& actor : info.CurrentWorld->GetActors())
        {
            if (!actor->HasComponent<MeshComponent>()) continue;

            auto& meshComp = actor->GetComponent<MeshComponent>();
            ModelAsset* model = meshComp.Model.Get();
            if (!model || !model->VertexBuffer || !model->IndexBuffer || !model->VertexView)
                continue;

            const int32     vbIndex    = model->VertexView->GetBindlessHandle();
            const glm::mat4 actorWorld = actor->GetComponent<Transform>().GetWorldMatrix();

            for (const MeshInstance& inst : model->Instances)
            {
                if (inst.LODs.empty()) continue;
                if (instanceCount >= MAX_INSTANCES) break;

                // Material deduplication
                MaterialAsset* matAsset = (inst.MaterialIndex < model->Materials.size())
                    ? model->Materials[inst.MaterialIndex].Get() : nullptr;

                int32 matIdx = 0;
                if (matAsset)
                {
                    auto it = materialMap.find(matAsset);
                    if (it != materialMap.end())
                    {
                        matIdx = it->second;
                    }
                    else if (materialCount < MAX_MATERIALS)
                    {
                        matIdx = materialCount;
                        materialMap[matAsset] = matIdx;

                        const Material& m = matAsset->GetMaterial();
                        GPUMaterial& gpuMat  = materialDst[materialCount++];
                        gpuMat.AlbedoIndex   = resolveTextureIndex(m.AlbedoTexture);
                        gpuMat.NormalIndex   = resolveTextureIndex(m.NormalTexture);
                        gpuMat.ORMIndex      = resolveTextureIndex(m.ORMTexture);
                        gpuMat.EmissiveIndex = resolveTextureIndex(m.EmissiveTexture);
                        gpuMat.SamplerIndex  = samplerIndex;
                        gpuMat._pad[0] = gpuMat._pad[1] = gpuMat._pad[2] = 0;
                    }
                }

                // Write instance
                const int32 instID      = instanceCount;
                GPUInstance& gpuInst    = instanceDst[instanceCount++];
                gpuInst.WorldMatrix       = actorWorld * inst.LocalTransform;
                gpuInst.MaterialIndex     = matIdx;
                gpuInst.VertexBufferIndex = vbIndex;
                gpuInst.BaseVertex        = static_cast<int32>(inst.BaseVertex);
                gpuInst._pad              = 0;

                const MeshLOD& lod = inst.LODs[0];
                draws.push_back({ instID, model->IndexBuffer, lod.IndexCount, lod.IndexOffset });
            }
        }

        // --- Render encoder ---
        auto getRTV = [&](RendererTexture* tex) {
            return info.ViewCache->GetTextureView(
                TextureViewDesc().SetTexture(tex).SetType(TextureViewType::RENDER_TARGET));
        };
        TextureView* depthDTV = info.ViewCache->GetTextureView(
            TextureViewDesc().SetTexture(m_DepthTexture).SetType(TextureViewType::DEPTH_TARGET));

        RenderEncoderInfo rei = RenderEncoderInfo()
            .SetName("GBuffer Pass")
            .SetDimensions(info.Width, info.Height)
            .AddColorAttachment(getRTV(m_AlbedoTexture),    true, glm::vec4(0.0f))
            .AddColorAttachment(getRTV(m_NormalsTexture),   true, glm::vec4(0.0f))
            .AddColorAttachment(getRTV(m_ORMTexture),       true, glm::vec4(0.0f))
            .AddColorAttachment(getRTV(m_EmissiveTexture),  true, glm::vec4(0.0f))
            .AddColorAttachment(getRTV(m_MotionVecTexture), true, glm::vec4(0.0f))
            .SetDepthAttachment(depthDTV);

        RenderEncoder* re = info.Ctx->CreateRenderEncoder(rei);
        re->SetPipeline(server.GetPipeline("__TERMINA__/CORE_SHADERS/GBuffer.hlsl"));
        re->SetViewport(0.0f, 0.0f, static_cast<float>(info.Width), static_cast<float>(info.Height));
        re->SetScissorRect(0, 0, info.Width, info.Height);

        struct GBufferPushConstants
        {
            glm::mat4 ViewProjection;       // 64 bytes
            int32     InstanceID;           //  4 bytes
            int32     InstanceBufferIndex;  //  4 bytes
            int32     MaterialBufferIndex;  //  4 bytes
            int32     _pad;                 //  4 bytes
        };                                  // Total: 80 bytes

        const int32 instBufIdx = m_InstanceBufView->GetBindlessHandle();
        const int32 matBufIdx  = m_MaterialBufView->GetBindlessHandle();

        for (const DrawCall& dc : draws)
        {
            re->SetIndexBuffer(dc.indexBuffer);

            GBufferPushConstants pc;
            pc.ViewProjection      = info.CurrentCamera.ViewProjection;
            pc.InstanceID          = dc.instanceID;
            pc.InstanceBufferIndex = instBufIdx;
            pc.MaterialBufferIndex = matBufIdx;
            pc._pad                = 0;

            re->SetConstants(sizeof(pc), &pc);
            re->DrawIndexed(dc.indexCount, 1, dc.indexOffset, 0, 0);
        }

        re->End();

        // Register GBuffer outputs and scene buffers via PassIO
        info.IO->RegisterTexture("GBuffer_Albedo",        m_AlbedoTexture);
        info.IO->RegisterTexture("GBuffer_Normals",       m_NormalsTexture);
        info.IO->RegisterTexture("GBuffer_ORM",           m_ORMTexture);
        info.IO->RegisterTexture("GBuffer_Emissive",      m_EmissiveTexture);
        info.IO->RegisterTexture("GBuffer_MotionVectors", m_MotionVecTexture);
        info.IO->RegisterTexture("GBuffer_Depth",         m_DepthTexture);
        info.IO->RegisterBuffer("Scene_InstanceBuffer",   m_InstanceBuffer);
        info.IO->RegisterBuffer("Scene_MaterialBuffer",   m_MaterialBuffer);

        // Transition color targets to shader-readable
        auto colorBarrier = [&](RendererTexture* tex) {
            info.Ctx->Barrier(TextureBarrier()
                .SetTargetTexture(tex)
                .SetNewLayout(TextureLayout::READ_ONLY)
                .SetDstAccess(ResourceAccess::SHADER_READ)
                .SetDstStage(PipelineStage::PIXEL_SHADER));
        };
        colorBarrier(m_AlbedoTexture);
        colorBarrier(m_NormalsTexture);
        colorBarrier(m_ORMTexture);
        colorBarrier(m_EmissiveTexture);
        colorBarrier(m_MotionVecTexture);
    }

} // namespace Termina

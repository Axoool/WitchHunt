#pragma once

#include <Termina/Renderer/RenderPass.hpp>
#include <Termina/RHI/Sampler.hpp>

namespace Termina {

    /// Renders all actors that have a MeshComponent using a bindless vertex buffer.
    class MeshPass : public RenderPass
    {
    public:
        MeshPass();
        ~MeshPass() override;

        void Resize(int32 width, int32 height) override;
        void Execute(RenderPassExecuteInfo& info) override;

    private:
        RendererTexture* m_ColorTexture = nullptr;
        RendererTexture* m_DepthTexture = nullptr;
        Sampler*         m_Sampler      = nullptr;
    };

} // namespace Termina

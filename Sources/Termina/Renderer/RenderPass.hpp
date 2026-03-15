#pragma once

#include <Termina/RHI/Device.hpp>
#include <Termina/RHI/RenderContext.hpp>
#include <Termina/World/World.hpp>
#include <Termina/Core/Common.hpp>

#include "GPUBumpAllocator.h"
#include "GPUUploader.h"
#include "Camera.hpp"
#include "ResourceViewCache.h"
#include "SamplerCache.h"
#include "PassIO.hpp"
#include "GPULight.hpp"

#include <vector>

namespace Termina {
    /// Contains the information needed to execute a render pass.
    struct RenderPassExecuteInfo
    {
        RendererDevice* Device;
        RendererSurface* Surface;
        RenderContext* Ctx;

        GPUUploader* Uploader;
        GPUBumpAllocator* Allocator;
        ResourceViewCache* ViewCache;
        SamplerCache* SampCache;
        PassIO* IO;

        World* CurrentWorld;
        Camera CurrentCamera;
        const std::vector<GPULight>* LightList = nullptr;

        uint32 FrameIndex;
        int32 Width;
        int32 Height;
    };

    /// Represents a render pass that can be executed.
    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;

        virtual void Resize(int32 width, int32 height) {};
        virtual void Execute(RenderPassExecuteInfo& Info) = 0;
    };
}

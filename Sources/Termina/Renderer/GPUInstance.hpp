#pragma once

#include <GLM/glm.hpp>
#include <Termina/Core/Common.hpp>

namespace Termina {
    /// Per-draw instance entry stored in the global instance buffer (80 bytes, 16-byte aligned).
    /// Matches the HLSL GPUInstance struct in Common/Scene.hlsli.
    struct GPUInstance
    {
        glm::mat4 WorldMatrix;      // Combined actor world × mesh local transform
        int32     MaterialIndex;    // Index into the material buffer
        int32     VertexBufferIndex;// Bindless handle to the model's vertex buffer
        int32     BaseVertex;       // Base vertex offset within the vertex buffer
        int32     _pad;
    };
    static_assert(sizeof(GPUInstance) == 80, "GPUInstance must be 80 bytes");
}

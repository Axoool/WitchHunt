#pragma once

#include <GLM/glm.hpp>
#include <Termina/Core/Common.hpp>

namespace Termina {
    enum class LightType : int32 { Directional = 0, Point = 1, Spot = 2 };

    /// POD struct matching the HLSL GPULight layout (64 bytes, 16-byte aligned).
    struct GPULight
    {
        glm::vec3 Position;   // World-space position (unused for directional)
        int32     Type;       // LightType enum value
        glm::vec3 Color;      // Linear RGB × intensity
        float     Range;      // Effective falloff range (unused for directional)
        glm::vec3 Direction;  // Normalized world-space direction (directional/spot)
        float     InnerCos;   // Spot: cos(inner half-angle)
        float     OuterCos;   // Spot: cos(outer half-angle)
        float     _pad[3];
    };
    static_assert(sizeof(GPULight) == 64, "GPULight must be 64 bytes");
}

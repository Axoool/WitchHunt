#pragma once

// JoltDebugRenderer bridges Jolt's debug renderer interface with the engine's DebugPass.
// When JPH_DEBUG_RENDERER is defined this inherits from JPH::DebugRendererSimple,
// otherwise it is an empty stub so the physics system compiles in all configurations.

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRendererSimple.h>

namespace Termina {
    class JoltDebugRenderer : public JPH::DebugRendererSimple
    {
    public:
        JoltDebugRenderer();

        void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;

        // DebugRendererSimple already provides DrawTriangle as 3 line calls,
        // and DrawText3D is a no-op below.
        void DrawText3D(JPH::RVec3Arg, const std::string_view&, JPH::ColorArg, float) override {}
    };
}

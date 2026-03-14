#include "JoltDebugRenderer.hpp"

#ifdef JPH_DEBUG_RENDERER

#include <Termina/Renderer/Passes/DebugPass.hpp>
#include <GLM/glm.hpp>

namespace Termina {

    JoltDebugRenderer::JoltDebugRenderer()
    {
        Initialize();
    }

    void JoltDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
    {
        glm::vec3 from(inFrom.GetX(), inFrom.GetY(), inFrom.GetZ());
        glm::vec3 to(inTo.GetX(),   inTo.GetY(),   inTo.GetZ());
        glm::vec4 col(inColor.r / 255.0f, inColor.g / 255.0f,
                      inColor.b / 255.0f, inColor.a / 255.0f);
        DebugPass::DrawLine(from, to, col);
    }

} // namespace Termina

#endif // JPH_DEBUG_RENDERER

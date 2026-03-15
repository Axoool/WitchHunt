#pragma once

#include <Termina/World/Component.hpp>
#include <GLM/glm.hpp>

namespace Termina {

    /// Spot light with inner/outer cone angles. Position and direction from the actor's Transform.
    class SpotLightComponent : public Component
    {
    public:
        SpotLightComponent() = default;
        SpotLightComponent(Actor* owner) : Component(owner) {}

        glm::vec3 Color          = glm::vec3(1.0f, 1.0f, 1.0f);
        float     Intensity      = 1.0f;
        float     Range          = 10.0f;
        float     InnerAngleDeg  = 15.0f;  // Inner cone half-angle in degrees
        float     OuterAngleDeg  = 30.0f;  // Outer cone half-angle in degrees

        void OnRender(float dt) override;
        void OnPreRender(float dt) override;

        void Inspect() override;
        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;

        UpdateFlags GetUpdateFlags() const override
        {
            return UpdateFlags::UpdateDuringEditor | UpdateFlags::RenderUpdateDuringEditor;
        }
    };

} // namespace Termina

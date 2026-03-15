#pragma once

#include <Termina/World/Component.hpp>
#include <GLM/glm.hpp>

namespace Termina {

    /// Omnidirectional point light. Position is taken from the actor's Transform.
    class PointLightComponent : public Component
    {
    public:
        PointLightComponent() = default;
        PointLightComponent(Actor* owner) : Component(owner) {}

        glm::vec3 Color     = glm::vec3(1.0f, 1.0f, 1.0f);
        float     Intensity = 1.0f;
        float     Range     = 10.0f;

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

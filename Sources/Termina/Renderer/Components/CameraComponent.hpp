#pragma once

#include <Termina/Renderer/Camera.hpp>
#include <Termina/World/Component.hpp>

namespace Termina {
    class CameraComponent : public Component
    {
    public:
        CameraComponent() = default;
        CameraComponent(Actor* owner) : Component(owner) {}

        Camera& GetCamera() { return m_Camera; }
        const Camera& GetCamera() const { return m_Camera; }

        void OnUpdate(float deltaTime) override;
        void OnPostRender(float deltaTime) override;
        void Inspect() override;

        UpdateFlags GetUpdateFlags() const override { return UpdateFlags::UpdateDuringEditor; }
    private:
        Camera m_Camera;

        float m_FOV = 75.0f;
        bool m_Primary = false;
    };
}

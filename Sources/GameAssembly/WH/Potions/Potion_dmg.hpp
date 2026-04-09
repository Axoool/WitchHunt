#pragma once
#include <Termina/World/Component.hpp>
#include <Termina/World/Actor.hpp>

namespace TerminaScript {
    class Potion_dmg : public Termina::Component {
    public:
        Potion_dmg(Termina::Actor* owner) : Termina::Component(owner) {}

        Termina::UpdateFlags GetUpdateFlags() const override { return static_cast<Termina::UpdateFlags>(1); }

        void OnUpdate(float deltaTime) override;

        // Call this from Grabber.cpp when throwing
        void SetThrown(bool thrown) {
            m_WasThrown = thrown;
            m_ArmTimer = 0.0f;
        }

    private:
        void Shatter();
        bool m_WasThrown = false;
        float m_ArmTimer = 0.0f; // Prevents it from breaking the exact millisecond it leaves your hand
    };
}
#pragma once
#include <Termina/World/Component.hpp>
#include <Termina/World/Actor.hpp>

namespace TerminaScript {
    class Field_dmg : public Termina::Component {
    public:
        Field_dmg(Termina::Actor* owner) : Termina::Component(owner) {}

        Termina::UpdateFlags GetUpdateFlags() const override { return static_cast<Termina::UpdateFlags>(1); }

        void OnUpdate(float deltaTime) override;

    private:
        float m_Lifetime = 0.0f;
    };
}
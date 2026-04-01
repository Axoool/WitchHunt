#pragma once
#include <Termina/World/Component.hpp>
#include <Termina/Physics/IPhysicsCallbacks.hpp>

namespace TerminaScript {

    class ChoppingBlock : public Termina::Component, public Termina::IPhysicsCallbacks {
    public:
        // 1. Add the Constructor that takes the Actor owner
        ChoppingBlock(Termina::Actor* owner) : Termina::Component(owner) {}

    Termina::UpdateFlags GetUpdateFlags() const override { 
        return static_cast<Termina::UpdateFlags>(0); 
    }

        // Physics Callbacks
        void OnTriggerEnter(Termina::Actor* other) override;
        void OnTriggerExit(Termina::Actor* other) override;

    private:
        Termina::Actor* m_CurrentIngredient = nullptr;
    };

}
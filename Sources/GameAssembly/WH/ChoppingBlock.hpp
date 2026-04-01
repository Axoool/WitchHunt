#pragma once
#include <Termina/World/Component.hpp>
#include <Termina/Physics/IPhysicsCallbacks.hpp>

namespace TerminaScript {

    class ChoppingBlock : public Termina::Component, public Termina::IPhysicsCallbacks {
    public:
        // Physics Callbacks
        void OnTriggerEnter(Termina::Actor* other) override;
        void OnTriggerExit(Termina::Actor* other) override;

    private:
        // This remembers which ingredient is currently sitting on the block
        Termina::Actor* m_CurrentIngredient = nullptr;
    };

}
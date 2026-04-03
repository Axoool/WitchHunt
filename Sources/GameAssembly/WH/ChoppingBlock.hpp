#pragma once
#include <Termina/World/Component.hpp>
#include <Termina/Physics/IPhysicsCallbacks.hpp>
#include <Termina/World/World.hpp> // Need this to access GetActorById
#include <Termina/World/Actor.hpp> 

namespace TerminaScript {

    class ChoppingBlock : public Termina::Component, public Termina::IPhysicsCallbacks {
    public:
        ChoppingBlock(Termina::Actor* owner) : Termina::Component(owner) {}

        // Safely resolves the ID to the actor. Returns nullptr if destroyed or restarted!
        Termina::Actor* GetCurrentIngredient() const
        {
            if (m_CurrentIngredientID == 0) return nullptr;
            return m_Owner->GetParentWorld()->GetActorById(m_CurrentIngredientID);
        }

        Termina::UpdateFlags GetUpdateFlags() const override {
            return static_cast<Termina::UpdateFlags>(0);
        }

        void OnCollisionEnter(Termina::Actor* other) override;
        void OnCollisionExit(Termina::Actor* other);

    private:
        // Store the ID instead of the raw pointer! (0 means no item)
        uint64 m_CurrentIngredientID = 0;
    };

}
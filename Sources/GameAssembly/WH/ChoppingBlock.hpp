#pragma once
#include <Termina/World/Component.hpp>
#include <Termina/World/Actor.hpp>
#include <string>

namespace TerminaScript {

    class ChoppingBlock : public Termina::Component {
    public:
        ChoppingBlock(Termina::Actor* owner) : Termina::Component(owner) {}

        // 1 = Update is active
        Termina::UpdateFlags GetUpdateFlags() const override {
            return static_cast<Termina::UpdateFlags>(1);
        }

        void OnUpdate(float deltaTime);
        void Inspect() override;

        // Public Logic
        Termina::Actor* GetSlottedItem() const;
        void ClearSlot();
        void PerformChop(Termina::Actor* knifeActor);

    private:
        uint64 m_SlottedItemID = 0;

        // Internal Helpers
        bool IsRawIngredient(Termina::Actor* actor);
        bool IsPlayerHolding(Termina::Actor* actor);
        std::string GetCutPrefabPath(Termina::Actor* actor);
    };
}
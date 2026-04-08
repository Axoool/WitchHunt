#pragma once
#include <Termina/World/Component.hpp>
#include <string>

namespace TerminaScript {
    class MortarPestleBlock : public Termina::Component {
    public:
        MortarPestleBlock(Termina::Actor* owner) : Termina::Component(owner) {}

        void OnUpdate(float deltaTime) override;
        void Inspect() override;

        Termina::UpdateFlags GetUpdateFlags() const override {
            return static_cast<Termina::UpdateFlags>(1);
        }

    private:
        uint64_t m_SlottedItemID = 0;

        Termina::Actor* GetSlottedItem() const;
        void ClearSlot();
        void PerformCrush(Termina::Actor* pestleActor);

        bool IsCrushableIngredient(Termina::Actor* actor);
        bool IsPlayerHolding(Termina::Actor* actor);
        std::string GetCrushPrefabPath(Termina::Actor* actor);
    };
}
#pragma once
#include <Termina/World/Component.hpp>
#include <vector>
#include <cstdint>

namespace TerminaScript {
    class CauldronBlock : public Termina::Component {
    public:
        CauldronBlock(Termina::Actor* owner) : Termina::Component(owner) {}

        void OnUpdate(float deltaTime) override;
        void Inspect() override;

        // Call this from your Player raycast script to take an item out!
        Termina::Actor* PopLastIngredient();

    private:
        // We use a vector to store up to 3 items in order (First In, Last Out)
        std::vector<uint64_t> m_SlottedItems;

        bool IsPlayerHolding(Termina::Actor* actor);
        bool IsAnyIngredient(Termina::Actor* actor);
    };
}
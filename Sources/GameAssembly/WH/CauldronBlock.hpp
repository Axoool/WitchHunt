#pragma once
#include <Termina/World/Component.hpp>
#include <vector>
#include <cstdint>
#include <string> // Added for the Recipe prefab strings

namespace TerminaScript {

    // --- NEW: Ingredient Enum & Recipe Struct ---
    enum class IngredientType {
        None,
        RawFlower, CutFlower, CrushedFlower,
        RawMushroom, CutMushroom, CrushedMushroom
    };

    struct Recipe {
        std::vector<IngredientType> Inputs;
        std::string ResultPrefab;
    };

    class CauldronBlock : public Termina::Component {
    public:
        CauldronBlock(Termina::Actor* owner) : Termina::Component(owner) {}

        // ADD THIS LINE TO FIX THE ERROR:
        Termina::UpdateFlags GetUpdateFlags() const override { return static_cast<Termina::UpdateFlags>(1); }

        void OnUpdate(float deltaTime) override;
        void Inspect() override;

        // Call this from your Player raycast script to take an item out!
        Termina::Actor* PopLastIngredient();

    private:
        // We use a vector to store up to 3 items in order (First In, Last Out)
        std::vector<uint64_t> m_SlottedItems;

        bool IsPlayerHolding(Termina::Actor* actor);
        bool IsAnyIngredient(Termina::Actor* actor);

        // --- NEW: Crafting logic functions ---
        IngredientType GetIngredientTypeFromActor(Termina::Actor* actor);
        void CheckRecipes();
        void CraftPotion(const std::string& prefabPath);
    };
}
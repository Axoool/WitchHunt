#pragma once
#include <Termina/World/Component.hpp>
#include <ImGui/imgui.h>
#include <string>

namespace TerminaScript {
    class Ingredient : public Termina::Component {
    public:
        Ingredient(Termina::Actor* owner) : Termina::Component(owner) {}

        Termina::UpdateFlags GetUpdateFlags() const override {
            return static_cast<Termina::UpdateFlags>(0);
        }

        void Inspect() override {
            // Ingredient Identity
            const char* types[] = { "Mushroom", "Flower", "Flower Cut", "Flower Crush", "Mushroom Cut", "Mushroom Crush" };
            int currentType = (int)IngredientType;
            if (ImGui::Combo("Ingredient Type", &currentType, types, IM_ARRAYSIZE(types))) {
                IngredientType = (Type)currentType;
            }

            // --- NEW: State Dropdown ---
            const char* states[] = { "Uncut", "Processed" };
            int currentStateInt = (int)CurrentState;
            if (ImGui::Combo("Processing State", &currentStateInt, states, IM_ARRAYSIZE(states))) {
                CurrentState = (State)currentStateInt;
            }

            // Results for different tools
            auto InputString = [](const char* label, std::string& target) {
                char buf[128];
                strcpy_s(buf, target.c_str());
                if (ImGui::InputText(label, buf, sizeof(buf))) {
                    target = buf;
                }
                };

            InputString("Prefab when CUT (Knife)", CutPrefabName);
            InputString("Prefab when CRUSHED (Mortar)", CrushedPrefabName);
        }

        // --- FIX: Expanded the enum to match the 6 elements in the types array ---
        enum class Type {
            Mushroom,
            Flower,
            FlowerCut,
            FlowerCrush,
            MushroomCut,
            MushroomCrush
        };

        enum class State { Uncut, Processed };

        Type IngredientType = Type::Mushroom;
        State CurrentState = State::Uncut;

        std::string CutPrefabName = "";
        std::string CrushedPrefabName = "";
    };
}
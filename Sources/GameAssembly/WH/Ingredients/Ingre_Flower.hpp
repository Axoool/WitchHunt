#pragma once
#include <Termina/World/Component.hpp>
#include <ImGui/imgui.h>
#include <string>

namespace TerminaScript {
    class Ingre_Flower : public Termina::Component {
    public:
        Ingre_Flower(Termina::Actor* owner) : Termina::Component(owner) {}

        enum class State { Unprocessed, Processed };
        enum class Method { None, Cut, Crushed };

        State CurrentState = State::Unprocessed;
        Method ProcessMethod = Method::None;

        // --- FIX: Hardcoded default path! ---
        std::string CutPrefabName = "Assets\\Prefabs\\Grabbable\\Ingredients\\Flowers\\flower_cut.trp";

        Termina::UpdateFlags GetUpdateFlags() const override {
            return static_cast<Termina::UpdateFlags>(0);
        }

        void Inspect() override {
            // 1. Is it Processed or Unprocessed?
            const char* states[] = { "Unprocessed", "Processed" };
            int currentStateInt = (int)CurrentState;
            if (ImGui::Combo("Processing State", &currentStateInt, states, IM_ARRAYSIZE(states))) {
                CurrentState = (State)currentStateInt;
            }

            // 2. How was it processed? (Cauldron uses this!)
            const char* methods[] = { "None", "Cut", "Crushed" };
            int currentMethodInt = (int)ProcessMethod;
            if (ImGui::Combo("Processing Method", &currentMethodInt, methods, IM_ARRAYSIZE(methods))) {
                ProcessMethod = (Method)currentMethodInt;
            }

            // 3. Prefab target
            char buf[128];
            strcpy_s(buf, CutPrefabName.c_str());
            if (ImGui::InputText("Prefab when CUT (Knife)", buf, sizeof(buf))) {
                CutPrefabName = buf;
            }
        }
    };
}
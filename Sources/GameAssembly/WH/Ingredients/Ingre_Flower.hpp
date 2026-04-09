#pragma once
#include <Termina/World/Component.hpp>
#include <ImGui/imgui.h>
#include <string>

namespace TerminaScript {
    class Ingre_Flower : public Termina::Component {
    public:
        Ingre_Flower(Termina::Actor* owner) : Termina::Component(owner) {}

        std::string CutPrefabName = "Assets\\Prefabs\\Grabbable\\Ingredients\\Flowers\\flower_cut.trp";
        // ADD THIS LINE:
        std::string CrushPrefabName = "Assets\\Prefabs\\Grabbable\\Ingredients\\Flowers\\flower_crush.trp";

        Termina::UpdateFlags GetUpdateFlags() const override { return static_cast<Termina::UpdateFlags>(0); }

        void Inspect() override {
            ImGui::Text("State: RAW Ingredient");

            char cutBuf[128];
            strcpy_s(cutBuf, CutPrefabName.c_str());
            if (ImGui::InputText("Prefab when CUT", cutBuf, sizeof(cutBuf))) {
                CutPrefabName = cutBuf;
            }

            char crushBuf[128];
            strcpy_s(crushBuf, CrushPrefabName.c_str());
            if (ImGui::InputText("Prefab when CRUSHED", crushBuf, sizeof(crushBuf))) {
                CrushPrefabName = crushBuf;
            }
        }
    };
}
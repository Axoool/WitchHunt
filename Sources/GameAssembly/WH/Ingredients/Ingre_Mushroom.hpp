#pragma once
#include <Termina/World/Component.hpp>
#include <ImGui/imgui.h>
#include <string>

namespace TerminaScript {
    class Ingre_Mushroom : public Termina::Component {
    public:
        Ingre_Mushroom(Termina::Actor* owner) : Termina::Component(owner) {}

        // Set these paths to wherever your mushroom prefabs actually live!
        std::string CutPrefabName = "Assets\\Prefabs\\Grabbable\\Ingredients\\Mushrooms\\mushroom_cut.trp";
        std::string CrushPrefabName = "Assets\\Prefabs\\Grabbable\\Ingredients\\Mushrooms\\mushroom_crush.trp";

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
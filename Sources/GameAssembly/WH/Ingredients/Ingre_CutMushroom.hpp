#pragma once
#include <Termina/World/Component.hpp>
#include <ImGui/imgui.h>

namespace TerminaScript {
    // FIX: Renamed from Ingre_CutFlower to Ingre_CutMushroom
    class Ingre_CutMushroom : public Termina::Component {
    public:
        // FIX: Constructor renamed to match the class
        Ingre_CutMushroom(Termina::Actor* owner) : Termina::Component(owner) {}

        Termina::UpdateFlags GetUpdateFlags() const override { return static_cast<Termina::UpdateFlags>(0); }
        void Inspect() override { ImGui::Text("State: CUT Ingredient (Ready for Cauldron)"); }
    };
}
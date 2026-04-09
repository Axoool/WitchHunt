#pragma once
#include <Termina/World/Component.hpp>
#include <ImGui/imgui.h>

namespace TerminaScript {
    class Ingre_CutFlower : public Termina::Component {
    public:
        Ingre_CutFlower(Termina::Actor* owner) : Termina::Component(owner) {}
        Termina::UpdateFlags GetUpdateFlags() const override { return static_cast<Termina::UpdateFlags>(0); }
        void Inspect() override { ImGui::Text("State: CUT Ingredient (Ready for Cauldron)"); }
    };
}
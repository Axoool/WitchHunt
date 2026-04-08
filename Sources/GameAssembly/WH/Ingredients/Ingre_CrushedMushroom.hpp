#pragma once
#include <Termina/World/Component.hpp>
#include <ImGui/imgui.h>

namespace TerminaScript {
    class Ingre_CrushedMushroom : public Termina::Component {
    public:
        Ingre_CrushedMushroom(Termina::Actor* owner) : Termina::Component(owner) {}
        Termina::UpdateFlags GetUpdateFlags() const override { return static_cast<Termina::UpdateFlags>(0); }
        void Inspect() override { ImGui::Text("State: CRUSHED Ingredient (Ready for Cauldron)"); }
    };
}
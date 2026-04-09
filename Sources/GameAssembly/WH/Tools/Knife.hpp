#pragma once
#include <Termina/World/Component.hpp>
#include <ImGui/imgui.h>

namespace TerminaScript {
    class Knife : public Termina::Component {
    public:
        Knife(Termina::Actor* owner) : Termina::Component(owner) {}

        Termina::UpdateFlags GetUpdateFlags() const override {
            return static_cast<Termina::UpdateFlags>(0);
        }

        void Inspect() override {
            // Just a simple text label in the inspector to confirm it's attached
            ImGui::Text("Identity: Knife Tool");
            ImGui::Text("Use this to chop ingredients on the Chopping Block.");
        }
    };
}
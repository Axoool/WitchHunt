#pragma once
#include <Termina/World/Component.hpp>
#include <ImGui/imgui.h>

namespace TerminaScript {
    class Pestle : public Termina::Component {
    public:
        Pestle(Termina::Actor* owner) : Termina::Component(owner) {}

        Termina::UpdateFlags GetUpdateFlags() const override {
            return static_cast<Termina::UpdateFlags>(0);
        }

        void Inspect() override {
            ImGui::Text("Identity: Pestle Tool");
            ImGui::Text("Use this to crush ingredients in the Mortar.");
        }
    };
}
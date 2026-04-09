#pragma once
#include <Termina/World/Component.hpp>
#include <ImGui/imgui.h>

namespace TerminaScript {
    class Tool : public Termina::Component {
    public:
        Tool(Termina::Actor* owner) : Termina::Component(owner) {}

        Termina::UpdateFlags GetUpdateFlags() const override {
            return static_cast<Termina::UpdateFlags>(0);
        }

        void Inspect() override {
            const char* toolNames[] = { "Knife", "Mortar and Pestle" };
            int currentTool = (int)ToolType;
            if (ImGui::Combo("Tool Type", &currentTool, toolNames, IM_ARRAYSIZE(toolNames))) {
                ToolType = (Type)currentTool;
            }
        }

        enum class Type { Knife, MortarAndPestle };
        Type ToolType = Type::Knife;
    };
}
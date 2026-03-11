#pragma once

#include <ImGui/imgui.h>

namespace Termina {
    class UIUtils
    {
    public:
        static void Setup();
        static void DrawStylizedText(const char* text, ImVec2 offset = ImVec2(8, 4));

        // Window wrapper
        static bool BeginEditorWindow(const char* title, bool* open = nullptr, ImGuiWindowFlags flags = 0);
        static void EndEditorWindow();
    
    private:
        static void SetTheme();

        static struct Data {
            ImFont* RegularFont;
            ImFont* CapitalFont;
        } sData;
    };
}

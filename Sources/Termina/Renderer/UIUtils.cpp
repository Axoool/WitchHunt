#include "UIUtils.hpp"
#include "Core/Logger.hpp"
#include "ImGui/imgui.h"

namespace Termina {
    UIUtils::Data UIUtils::sData;

    void UIUtils::Setup()
    {
        ImGuiIO& io = ImGui::GetIO();
        sData.RegularFont = io.Fonts->AddFontFromFileTTF("Assets/Fonts/PlayfairDisplay-Regular.ttf", 20.0f);
        sData.CapitalFont = io.Fonts->AddFontFromFileTTF("Assets/Fonts/UnifrakturMaguntia-Regular.ttf", 24.0f);

        SetTheme();
    }

    void UIUtils::DrawStylizedText(const char* text, ImVec2 offset)
    {
        if (!text || text[0] == '\0') return;

        ImGui::PushFont(sData.CapitalFont);
        ImGui::Text("%c", text[0]);
        ImGui::PopFont();

        if (text[1] != '\0')
        {
            ImGui::SameLine(0, 0); // no spacing
            ImGui::SetCursorPosY(ImGui::GetCursorPosY());
            ImGui::PushFont(sData.RegularFont);
            ImGui::Text("%s", text + 1);
            ImGui::PopFont();
        }
    }

    bool UIUtils::BeginEditorWindow(const char* title, bool* open, ImGuiWindowFlags flags)
    {
        ImGui::PushFont(sData.RegularFont);

        bool visible = ImGui::Begin(title, open, flags); // keep the real title bar
        float regularFontSize = 18.0f * ImGui::GetIO().FontGlobalScale;
        float capitalFontSize = 20.0f * ImGui::GetIO().FontGlobalScale;

        if (visible)
        {
            float titleBarHeight = ImGui::GetFrameHeight();
            ImVec2 windowPos = ImGui::GetWindowPos();
            float windowWidth = ImGui::GetWindowWidth();
        
            ImDrawList* dl = ImGui::GetWindowDrawList();

            dl->PushClipRect(
                windowPos,
                ImVec2(windowPos.x + windowWidth, windowPos.y + ImGui::GetWindowHeight()),
                false // false = don't intersect with current cliprect, replace it
            );
        
            dl->AddRectFilled(
                windowPos,
                ImVec2(windowPos.x + windowWidth, windowPos.y + titleBarHeight),
                IM_COL32(255, 255, 255, 255)
            );
        
            dl->AddLine(
                ImVec2(windowPos.x, windowPos.y + titleBarHeight),
                ImVec2(windowPos.x + windowWidth, windowPos.y + titleBarHeight),
                IM_COL32(90, 75, 60, 200), 1.0f
            );
        
            // Draw text directly via drawlist, bypassing cursor/clip issues
            ImVec2 textPos = ImVec2(windowPos.x + 8, windowPos.y + ImGui::GetStyle().FramePadding.y);
        
            // Capital letter with Unifraktur
            int cmdCountBefore = dl->CmdBuffer.Size;
            dl->AddText(sData.CapitalFont, capitalFontSize,
                textPos,
                IM_COL32(40, 35, 30, 255),
                title, title + 1
            );
        
            // Rest with IM Fell — advance x by the capital glyph width
            float capWidth = sData.CapitalFont->CalcTextSizeA(capitalFontSize, FLT_MAX, 0, title, title + 1).x;
            dl->AddText(sData.RegularFont, regularFontSize,
                ImVec2(textPos.x + (capWidth + 2), textPos.y),
                IM_COL32(40, 35, 30, 255),
                title + 1
            );


            dl->PopClipRect();
        
            ImGui::SetCursorPosY(titleBarHeight + ImGui::GetStyle().ItemSpacing.y);
        }

        return visible;
    }

    void UIUtils::EndEditorWindow()
    {
        ImGui::End();
        ImGui::PopFont(); // matches PushFont in BeginEditorWindow
    }

    void UIUtils::SetTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        // Rounding — sharp, clinical
        style.WindowRounding    = 0.0f;
        style.ChildRounding     = 0.0f;
        style.FrameRounding     = 0.0f;
        style.PopupRounding     = 0.0f;
        style.ScrollbarRounding = 0.0f;
        style.GrabRounding      = 0.0f;
        style.TabRounding       = 0.0f;

        // Sizing
        style.WindowBorderSize  = 1.0f;
        style.FrameBorderSize   = 1.0f;
        style.PopupBorderSize   = 1.0f;
        style.FramePadding      = ImVec2(6, 4);
        style.ItemSpacing       = ImVec2(8, 4);
        style.IndentSpacing     = 14.0f;
        style.ScrollbarSize     = 10.0f;
        style.GrabMinSize       = 8.0f;

        ImVec4* c = style.Colors;

        // Newsprint whites — cold, not warm
        c[ImGuiCol_WindowBg]             = ImVec4(0.92f, 0.92f, 0.93f, 1.00f);
        c[ImGuiCol_ChildBg]              = ImVec4(0.89f, 0.89f, 0.90f, 1.00f);
        c[ImGuiCol_PopupBg]              = ImVec4(0.94f, 0.94f, 0.95f, 1.00f);

        // Borders — dark, sharp
        c[ImGuiCol_Border]               = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
        c[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

        // Title bars — slightly darker cold grey
        c[ImGuiCol_TitleBg]              = ImVec4(0.78f, 0.78f, 0.80f, 1.00f);
        c[ImGuiCol_TitleBgActive]        = ImVec4(0.78f, 0.78f, 0.80f, 1.00f);
        c[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.78f, 0.78f, 0.80f, 1.00f);

        // Frames — input fields, checkboxes etc
        c[ImGuiCol_FrameBg]              = ImVec4(0.85f, 0.85f, 0.87f, 1.00f);
        c[ImGuiCol_FrameBgHovered]       = ImVec4(0.80f, 0.80f, 0.82f, 1.00f);
        c[ImGuiCol_FrameBgActive]        = ImVec4(0.72f, 0.72f, 0.75f, 1.00f);

        // Scrollbar
        c[ImGuiCol_ScrollbarBg]          = ImVec4(0.88f, 0.88f, 0.89f, 1.00f);
        c[ImGuiCol_ScrollbarGrab]        = ImVec4(0.50f, 0.50f, 0.52f, 1.00f);
        c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.37f, 1.00f);
        c[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);

        // Headers (TreeNode, CollapsingHeader, Selectable)
        c[ImGuiCol_Header]               = ImVec4(0.72f, 0.72f, 0.74f, 1.00f);
        c[ImGuiCol_HeaderHovered]        = ImVec4(0.62f, 0.62f, 0.65f, 1.00f);
        c[ImGuiCol_HeaderActive]         = ImVec4(0.50f, 0.50f, 0.53f, 1.00f);

        // Buttons
        c[ImGuiCol_Button]               = ImVec4(0.75f, 0.75f, 0.77f, 1.00f);
        c[ImGuiCol_ButtonHovered]        = ImVec4(0.60f, 0.60f, 0.63f, 1.00f);
        c[ImGuiCol_ButtonActive]         = ImVec4(0.40f, 0.40f, 0.43f, 1.00f);

        // Tabs
        c[ImGuiCol_Tab]                  = ImVec4(0.78f, 0.78f, 0.80f, 1.00f);
        c[ImGuiCol_TabHovered]           = ImVec4(0.60f, 0.60f, 0.63f, 1.00f);
        c[ImGuiCol_TabActive]            = ImVec4(0.92f, 0.92f, 0.93f, 1.00f); // matches WindowBg — feels "open"
        c[ImGuiCol_TabUnfocused]         = ImVec4(0.82f, 0.82f, 0.84f, 1.00f);
        c[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.88f, 0.88f, 0.90f, 1.00f);

        // Docking
        c[ImGuiCol_DockingPreview]       = ImVec4(0.20f, 0.20f, 0.22f, 0.70f);
        c[ImGuiCol_DockingEmptyBg]       = ImVec4(0.85f, 0.85f, 0.87f, 1.00f);

        // Selection / highlight — single cold accent, like a stamped mark
        c[ImGuiCol_CheckMark]            = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
        c[ImGuiCol_SliderGrab]           = ImVec4(0.40f, 0.40f, 0.43f, 1.00f);
        c[ImGuiCol_SliderGrabActive]     = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);

        // Text
        c[ImGuiCol_Text]                 = ImVec4(0.08f, 0.08f, 0.10f, 1.00f); // cold near-black
        c[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.50f, 0.53f, 1.00f);
        c[ImGuiCol_TextSelectedBg]       = ImVec4(0.40f, 0.40f, 0.43f, 0.40f);

        // Separator
        c[ImGuiCol_Separator]            = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
        c[ImGuiCol_SeparatorHovered]     = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
        c[ImGuiCol_SeparatorActive]      = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);

        // Resize grip
        c[ImGuiCol_ResizeGrip]           = ImVec4(0.40f, 0.40f, 0.43f, 0.40f);
        c[ImGuiCol_ResizeGripHovered]    = ImVec4(0.20f, 0.20f, 0.22f, 0.70f);
        c[ImGuiCol_ResizeGripActive]     = ImVec4(0.08f, 0.08f, 0.10f, 0.90f);

        // Menubar
        c[ImGuiCol_MenuBarBg]            = ImVec4(0.82f, 0.82f, 0.84f, 1.00f);

        // Modal overlay
        c[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.08f, 0.08f, 0.10f, 0.45f);

        // Nav highlight — keyboard/gamepad focus
        c[ImGuiCol_NavHighlight]         = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
        c[ImGuiCol_NavWindowingHighlight]= ImVec4(0.10f, 0.10f, 0.12f, 0.70f);
        c[ImGuiCol_NavWindowingDimBg]    = ImVec4(0.08f, 0.08f, 0.10f, 0.20f);
    }
}

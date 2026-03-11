#include "ContentViewerPanel.hpp"

#include "ImGui/imgui.h"
#include "Termina/Renderer/UIUtils.hpp"

void ContentViewerPanel::OnImGuiRender()
{
    Termina::UIUtils::BeginEditorWindow(m_Name.c_str(), &m_Open);
    ImGui::Text("Content Viewer placeholder");
    Termina::UIUtils::EndEditorWindow();
}

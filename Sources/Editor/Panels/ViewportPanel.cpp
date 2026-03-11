#include "ViewportPanel.hpp"
#include "ImGui/imgui.h"
#include "Termina/Renderer/UIUtils.hpp"

void ViewportPanel::OnImGuiRender()
{
    Termina::UIUtils::BeginEditorWindow(m_Name.c_str(), &m_Open);
    ImGui::Text("Viewport placeholder");
    Termina::UIUtils::EndEditorWindow();
}

#include "WorldHierarchyPanel.hpp"
#include "ImGui/imgui.h"
#include "Termina/Renderer/UIUtils.hpp"

void WorldHierarchyPanel::OnImGuiRender()
{
    Termina::UIUtils::BeginEditorWindow(m_Name.c_str(), &m_Open);
    ImGui::Text("World Hierarchy placeholder");
    Termina::UIUtils::EndEditorWindow();
}

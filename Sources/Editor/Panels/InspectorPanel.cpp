#include "InspectorPanel.hpp"
#include "ImGui/imgui.h"
#include "Termina/Renderer/UIUtils.hpp"

void InspectorPanel::OnImGuiRender()
{
    Termina::UIUtils::BeginEditorWindow(m_Name.c_str(), &m_Open);
    ImGui::Text("Content Viewer placeholder");

    if (m_Context.SelectedEntity != -1)
        ImGui::Text("Inspecting entity: %d", m_Context.SelectedEntity);
    else
        ImGui::Text("No entity selected");

    Termina::UIUtils::EndEditorWindow();
}

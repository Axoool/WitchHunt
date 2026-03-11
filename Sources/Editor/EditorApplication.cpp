#include "EditorApplication.hpp"

#include "Panels/ViewportPanel.hpp"
#include "Panels/WorldHierarchyPanel.hpp"
#include "Panels/InspectorPanel.hpp"
#include "Panels/ContentViewerPanel.hpp"

#include <Termina/Renderer/Renderer.hpp>
#include <Termina/Shader/ShaderManager.hpp>

#include "ImGui/imgui.h"

EditorApplication::EditorApplication()
    : Application("Editor")
{
    m_SystemManager.AddSystem<Termina::RendererSystem>(m_Window);
    m_SystemManager.AddSystem<Termina::ShaderManager>();

    RegisterPanel<ViewportPanel>();
    RegisterPanel<WorldHierarchyPanel>();
    RegisterPanel<InspectorPanel>();
    RegisterPanel<ContentViewerPanel>();
}

void EditorApplication::OnUpdate(float dt)
{
    RenderDockspace();

    for (auto& panel : m_Panels)
    {
        if (panel->IsOpen())
            panel->OnImGuiRender();
    }
}

void EditorApplication::RenderDockspace()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("##DockspaceRoot", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Termina"))
        {
            if (ImGui::MenuItem("Quit"))
                m_Running = false;
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Layout"))
        {
            for (auto& panel : m_Panels)
            {
                bool open = panel->IsOpen();
                if (ImGui::MenuItem(panel->GetName().c_str(), nullptr, open))
                    panel->SetOpen(!open);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGuiID dockspaceId = ImGui::GetID("DockspaceRoot");
    ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();
}

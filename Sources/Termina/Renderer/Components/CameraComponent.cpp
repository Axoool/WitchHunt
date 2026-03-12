#include "CameraComponent.hpp"
#include "GLM/ext/matrix_clip_space.hpp"
#include "GLM/ext/matrix_transform.hpp"
#include "ImGui/imgui.h"
#include "World/Components/Transform.hpp"
#include <Termina/World/Actor.hpp>

namespace Termina {
    void CameraComponent::OnUpdate(float deltaTime)
    {
        Transform& transform = GetOwner()->GetComponent<Transform>();

        m_Camera.Position = transform.GetPosition();
        m_Camera.Direction = transform.GetForward();

        m_Camera.Projection = glm::perspective(glm::radians(m_FOV), 16.0f / 9.0f, m_Camera.Near, m_Camera.Far);
        m_Camera.View = glm::lookAt(transform.GetPosition(), transform.GetPosition() + transform.GetForward(), transform.GetUp());
    }

    void CameraComponent::OnPostRender(float deltaTime)
    {
        m_Camera.UpdatePrevious();
    }

    void CameraComponent::Inspect()
    {
        ImGui::Checkbox("Primary", &m_Primary);
        ImGui::DragFloat("Near", &m_Camera.Near, 0.01f, 0.01f, 100.0f);
        ImGui::DragFloat("Far", &m_Camera.Far, 0.01f, 0.01f, 100.0f);
        ImGui::DragFloat("FOV", &m_FOV, 1.0f, 1.0f, 179.0f);
    }
}

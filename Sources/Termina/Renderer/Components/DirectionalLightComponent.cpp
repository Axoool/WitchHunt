#include "DirectionalLightComponent.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/World/WorldSystem.hpp>
#include <Termina/World/Actor.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Renderer/Renderer.hpp>
#include <Termina/Renderer/GPULight.hpp>
#include <Termina/Renderer/Passes/DebugPass.hpp>

#include <ImGui/imgui.h>

#include <GLM/gtc/matrix_transform.hpp>

namespace Termina {

    void DirectionalLightComponent::OnRender(float /*dt*/)
    {
        if (!m_Owner || !m_Owner->HasComponent<Transform>()) return;

        const Transform& t = m_Owner->GetComponent<Transform>();

        GPULight light{};
        light.Type      = static_cast<int32>(LightType::Directional);
        light.Color     = Color * Intensity;
        light.Direction = glm::normalize(t.GetForward());
        light.Direction.y = -light.Direction.y;
        light.Position  = glm::vec3(0.0f);
        light.Range     = 0.0f;
        light.InnerCos  = 0.0f;
        light.OuterCos  = 0.0f;

        Application::GetSystem<RendererSystem>()->SubmitLight(light);
    }

    void DirectionalLightComponent::OnPreRender(float /*dt*/)
    {
        auto* ws = Application::GetSystem<WorldSystem>();
        if (!ws || ws->IsPlaying()) return;
        if (!m_Owner || !m_Owner->HasComponent<Transform>()) return;

        const Transform& t = m_Owner->GetComponent<Transform>();
        glm::vec3 pos = t.GetPosition();
        glm::vec3 dir = glm::normalize(t.GetForward());

        // Draw an arrow showing the light direction
        DebugPass::DrawArrow(pos, pos + dir * 2.0f, 0.15f,
                             glm::vec4(Color, 1.0f));
    }

    void DirectionalLightComponent::Inspect()
    {
        ImGui::ColorEdit3("Color", &Color.x);
        ImGui::DragFloat("Intensity", &Intensity, 0.01f, 0.0f, 100.0f);
    }

    void DirectionalLightComponent::Serialize(nlohmann::json& out) const
    {
        out["color"]     = { Color.x, Color.y, Color.z };
        out["intensity"] = Intensity;
    }

    void DirectionalLightComponent::Deserialize(const nlohmann::json& in)
    {
        if (in.contains("color"))
            Color = glm::vec3(in["color"][0], in["color"][1], in["color"][2]);
        Intensity = in.value("intensity", 1.0f);
    }

} // namespace Termina

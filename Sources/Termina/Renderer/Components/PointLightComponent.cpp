#include "PointLightComponent.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/World/WorldSystem.hpp>
#include <Termina/World/Actor.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Renderer/Renderer.hpp>
#include <Termina/Renderer/GPULight.hpp>
#include <Termina/Renderer/Passes/DebugPass.hpp>

#include <ImGui/imgui.h>

namespace Termina {

    void PointLightComponent::OnRender(float /*dt*/)
    {
        if (!m_Owner || !m_Owner->HasComponent<Transform>()) return;

        const Transform& t = m_Owner->GetComponent<Transform>();

        GPULight light{};
        light.Type      = static_cast<int32>(LightType::Point);
        light.Color     = Color * Intensity;
        light.Position  = t.GetPosition();
        light.Range     = Range;
        light.Direction = glm::vec3(0.0f);
        light.InnerCos  = 0.0f;
        light.OuterCos  = 0.0f;

        Application::GetSystem<RendererSystem>()->SubmitLight(light);
    }

    void PointLightComponent::OnPreRender(float /*dt*/)
    {
        auto* ws = Application::GetSystem<WorldSystem>();
        if (!ws || ws->IsPlaying()) return;
        if (!m_Owner || !m_Owner->HasComponent<Transform>()) return;

        const Transform& t = m_Owner->GetComponent<Transform>();
        glm::vec3 pos = t.GetPosition();

        DebugPass::DrawSphere(pos, Range, 16, glm::vec4(Color, 1.0f));
    }

    void PointLightComponent::Inspect()
    {
        ImGui::ColorEdit3("Color", &Color.x);
        ImGui::DragFloat("Intensity", &Intensity, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Range", &Range, 0.1f, 0.01f, 10000.0f);
    }

    void PointLightComponent::Serialize(nlohmann::json& out) const
    {
        out["color"]     = { Color.x, Color.y, Color.z };
        out["intensity"] = Intensity;
        out["range"]     = Range;
    }

    void PointLightComponent::Deserialize(const nlohmann::json& in)
    {
        if (in.contains("color"))
            Color = glm::vec3(in["color"][0], in["color"][1], in["color"][2]);
        Intensity = in.value("intensity", 1.0f);
        Range     = in.value("range", 10.0f);
    }

} // namespace Termina

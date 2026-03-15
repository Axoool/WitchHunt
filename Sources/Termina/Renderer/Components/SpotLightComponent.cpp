#include "SpotLightComponent.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/World/WorldSystem.hpp>
#include <Termina/World/Actor.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Renderer/Renderer.hpp>
#include <Termina/Renderer/GPULight.hpp>
#include <Termina/Renderer/Passes/DebugPass.hpp>

#include <ImGui/imgui.h>

#include <GLM/gtc/constants.hpp>
#include <cmath>

namespace Termina {

    void SpotLightComponent::OnRender(float /*dt*/)
    {
        if (!m_Owner || !m_Owner->HasComponent<Transform>()) return;

        const Transform& t = m_Owner->GetComponent<Transform>();

        const float innerRad = glm::radians(InnerAngleDeg);
        const float outerRad = glm::radians(OuterAngleDeg);

        GPULight light{};
        light.Type      = static_cast<int32>(LightType::Spot);
        light.Color     = Color * Intensity;
        light.Position  = t.GetPosition();
        light.Range     = Range;
        light.Direction = glm::normalize(t.GetForward());
        light.InnerCos  = std::cos(innerRad);
        light.OuterCos  = std::cos(outerRad);

        Application::GetSystem<RendererSystem>()->SubmitLight(light);
    }

    void SpotLightComponent::OnPreRender(float /*dt*/)
    {
        auto* ws = Application::GetSystem<WorldSystem>();
        if (!ws || ws->IsPlaying()) return;
        if (!m_Owner || !m_Owner->HasComponent<Transform>()) return;

        const Transform& t = m_Owner->GetComponent<Transform>();
        glm::vec3 pos = t.GetPosition();
        glm::vec3 dir = glm::normalize(t.GetForward());
        glm::vec4 col = glm::vec4(Color, 1.0f);

        // Draw cone at the outer angle
        DebugPass::DrawCone(pos, dir, Range, glm::radians(OuterAngleDeg), col);
        // Draw smaller inner cone
        DebugPass::DrawCone(pos, dir, Range, glm::radians(InnerAngleDeg),
                            glm::vec4(Color * 0.5f, 0.6f));
    }

    void SpotLightComponent::Inspect()
    {
        ImGui::ColorEdit3("Color", &Color.x);
        ImGui::DragFloat("Intensity",       &Intensity,      0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Range",           &Range,          0.1f,  0.01f, 10000.0f);
        ImGui::DragFloat("Inner Angle",     &InnerAngleDeg,  0.5f,  0.0f, 89.0f);
        ImGui::DragFloat("Outer Angle",     &OuterAngleDeg,  0.5f,  0.0f, 89.0f);

        // Keep inner <= outer
        if (InnerAngleDeg > OuterAngleDeg)
            InnerAngleDeg = OuterAngleDeg;
    }

    void SpotLightComponent::Serialize(nlohmann::json& out) const
    {
        out["color"]       = { Color.x, Color.y, Color.z };
        out["intensity"]   = Intensity;
        out["range"]       = Range;
        out["inner_angle"] = InnerAngleDeg;
        out["outer_angle"] = OuterAngleDeg;
    }

    void SpotLightComponent::Deserialize(const nlohmann::json& in)
    {
        if (in.contains("color"))
            Color = glm::vec3(in["color"][0], in["color"][1], in["color"][2]);
        Intensity     = in.value("intensity",   1.0f);
        Range         = in.value("range",       10.0f);
        InnerAngleDeg = in.value("inner_angle", 15.0f);
        OuterAngleDeg = in.value("outer_angle", 30.0f);
    }

} // namespace Termina

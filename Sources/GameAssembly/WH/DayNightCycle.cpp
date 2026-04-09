#include "DayNightCycle.hpp"

#include "ImGui/imgui.h"
#include "Termina/Renderer/Components/DirectionalLightComponent.hpp"

void DayNightCycle::Serialize(nlohmann::json& out) const
{
	out["halfCycleTime"] = m_halfCycleTime;
}

void DayNightCycle::Deserialize(const nlohmann::json& in)
{
	if (in.contains("halfCycleTime")) m_halfCycleTime = in["halfCycleTime"];
}

void DayNightCycle::Inspect()
{
	ImGui::DragFloat("halfCycleTime", &m_halfCycleTime, 1.f);
}

void DayNightCycle::Update(float deltaTime)
{
	if (!m_isPaused)
		m_timer += deltaTime;

	if (m_timer > m_halfCycleTime)
	{
		switchDayNight();
		m_timer = 0.f;
	}
}

void DayNightCycle::switchDayNight()
{
	auto& sunlight = m_Owner->GetComponent<Termina::DirectionalLightComponent>();

	m_isDay = !m_isDay;
	if (m_isDay)
	{
		sunlight.Color = glm::vec3(1.f, 1.f, 1.f);
		sunlight.Intensity = 5.f;
	}
	else
	{
		sunlight.Color = glm::vec3(0.3f, 1.f, 1.f);
		sunlight.Intensity = 2.f;
	}
}
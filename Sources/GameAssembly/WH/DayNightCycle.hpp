#pragma once
#include <Termina/Scripting/API/ScriptingAPI.hpp>

class DayNightCycle : public TerminaScript::ScriptableComponent
{
public:
	DayNightCycle() = default;
	DayNightCycle(Termina::Actor* owner) : TerminaScript::ScriptableComponent(owner) {}

	void Update(float deltaTime) override;

	void Serialize(nlohmann::json& out) const override;
	void Deserialize(const nlohmann::json& in) override;

	void Inspect() override;

	bool m_isDay = true;
	bool m_isPaused = false;
	float m_halfCycleTime = 150.f;
private:
	float m_timer = 0.f;

	void switchDayNight();
};
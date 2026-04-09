#pragma once
#include <Termina/Scripting/API/ScriptingAPI.hpp>

class PlayerHealth : public TerminaScript::ScriptableComponent
{
	public:
	PlayerHealth() = default;
	PlayerHealth(Termina::Actor* owner) : TerminaScript::ScriptableComponent(owner) {}

	void Start() override;

	void TakeDamage(float amount);

	void Serialize(nlohmann::json& out) const override;
	void Deserialize(const nlohmann::json& in) override;
	void Inspect() override;

private:
	float m_Health = 100.0f;
	float m_MaxHealth = 100.0f;
	bool isAlive = true;
};
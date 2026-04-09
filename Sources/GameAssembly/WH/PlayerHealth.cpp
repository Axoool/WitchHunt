#include "PlayerHealth.hpp"

void PlayerHealth::Start()
{
	m_Health = m_MaxHealth;
	isAlive = true;
}

void PlayerHealth::TakeDamage(float amount)
{
	m_Health -= amount;
	if (m_Health <= 0.0f)
	{
		m_Health = 0.0f;
	}
}

void PlayerHealth::Serialize(nlohmann::json& out) const
{
	out["MaxHealth"] = m_MaxHealth; 
}

void PlayerHealth::Deserialize(const nlohmann::json& in)
{
	if (in.contains("MaxHealth")) m_MaxHealth = in["MaxHealth"];
}

void PlayerHealth::Inspect()
{
	ImGui::DragFloat("Max Health", &m_MaxHealth, 0.1f, 1.0f, 1000.0f);
}

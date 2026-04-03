#pragma once

#include <Termina/Scripting/API/ScriptingAPI.hpp>

using namespace TerminaScript;

class Controller : public ScriptableComponent
{
public:
	Controller() = default;
	Controller(Termina::Actor* owner) : ScriptableComponent(owner) {}

	void Start() override;
	void Update(float deltaTime) override;

	void Serialize(nlohmann::json& out) const override;
	void Deserialize(const nlohmann::json& in) override;

	void Inspect() override;
	
	float m_moveSpeed = 7.f;
	float m_sprintSpeed = 12.f;
	float m_sensitivity = 0.15f;

private:
	bool isGrounded();
	void setVelocity(glm::vec3 velocity);
};

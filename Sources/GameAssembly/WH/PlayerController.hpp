#pragma once

#include <Termina/Scripting/API/ScriptingAPI.hpp>

using namespace TerminaScript;

class PlayerController : public TerminaScript::ScriptableComponent
{
public:
    PlayerController(Termina::Actor* owner) : ScriptableComponent(owner) {}

    void Update(float deltaTime) override;
    void Start() override;

    void Serialize(nlohmann::json& out) const override;
	void Deserialize(const nlohmann::json& in) override;

	void Inspect() override;
    

private:
    float m_WalkSpeed = 5.0f;
    float m_SprintSpeed = 10.0f;
    float m_CurrentSpeed = 5.0f;

    float m_Stamina = 100.0f;
    float m_MaxStamina = 100.0f;
    float m_StaminaDrain = 30.0f;
    float m_StaminaRegen = 15.0f;

    float m_JumpPower = 5.0f;
    float m_VerticalVelocity = 0.0f;

    float m_Yaw = 0.0f;
    float m_Pitch = 0.0f;
    float m_Sensitivity = 0.15f;
};
#include "Controller.hpp"

void Controller::Start()
{

}

void Controller::Update(float deltaTime)
{

}

void Controller::Serialize(nlohmann::json& out) const
{
	out["moveSpeed"] = m_moveSpeed;
	out["sprintSpeed"] = m_sprintSpeed;
	out["sensitivity"] = m_sensitivity;
}

void Controller::Deserialize(const nlohmann::json& in)
{
	if (in.contains("moveSpeed")) m_moveSpeed = in["moveSpeed"];
	if (in.contains("sprintSpeed")) m_sprintSpeed = in["sprintSpeed"];
	if (in.contains("sensitivity")) m_sensitivity = in["sensitivity"];
}

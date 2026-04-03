#include "Controller.hpp"

#include "ImGui/imgui.h"
#include <Termina/Core/Application.hpp>
#include <Termina/Physics/PhysicsSystem.hpp>
#include <Termina/Physics/Components/Rigidbody.hpp>

void Controller::Start()
{

}

void Controller::Update(float deltaTime)
{
	// Input
	glm::vec3 forward = m_Transform->GetForward();
	glm::vec3 right = m_Transform->GetRight();
	
	glm::vec3 targetDir = glm::vec3(0.f);
	if (Input::IsKeyHeld(Termina::Key::W)) targetDir += forward;
	if (Input::IsKeyHeld(Termina::Key::S)) targetDir -= forward;
	if (Input::IsKeyHeld(Termina::Key::D)) targetDir += right;
	if (Input::IsKeyHeld(Termina::Key::A)) targetDir -= right;

	if (glm::length(targetDir) > 0.0f)
		targetDir = glm::normalize(targetDir);

	glm::vec3 targetVelocity = glm::vec3(0.f);
	if (Input::IsKeyHeld(Termina::Key::LeftShift))
		targetVelocity = targetDir * m_sprintSpeed;
	else
		targetVelocity = targetDir * m_moveSpeed;
	if (isGrounded)
		setVelocity(targetVelocity);

	// 
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

void Controller::Inspect()
{
	ImGui::DragFloat("Walk speed", &m_moveSpeed, 0.1f);
	ImGui::DragFloat("Sprint speed", &m_sprintSpeed, 0.1f);
	ImGui::DragFloat("Sensitivity", &m_sensitivity, 0.01f);
}

bool Controller::isGrounded()
{
	auto* jolt = Termina::Application::GetSystem<Termina::PhysicsSystem>();

	auto& transform = m_Owner->GetComponent<Termina::Transform>();
	Termina::Ray rayToGround;

	rayToGround.Origin = transform.GetPosition();
	rayToGround.Direction = glm::vec3(0.f, 1.f, 0.f);

	rayToGround.MaxDistance = 0.91;
	auto result = jolt->Raycast(rayToGround);

	return result.Hit;
}

void Controller::setVelocity(glm::vec3 velocity)
{
	auto& rigidBody = m_Owner->GetComponent<Termina::Rigidbody>();

	auto currentVelocity = rigidBody.GetLinearVelocity();

	rigidBody.SetLinearVelocity(glm::vec3(velocity.x, currentVelocity.y, velocity.z));
}

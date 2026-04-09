#include "GameAssembly/WH/PlayerController.hpp"

#include <Termina/Physics/Components/CharacterController.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/World/Actor.hpp>
#include <Termina/Renderer/Components/CameraComponent.hpp>
#include <ImGui/imgui.h>
#include <algorithm>

void PlayerController::Update(float deltaTime)
{
    if (Input::IsKeyPressed(Termina::Key::Escape))
    {
        TerminaScript::Input::SetCursorVisible(true);
        TerminaScript::Input::SetCursorLocked(false);
    }

    glm::vec2 mouseDelta = Input::GetMouseDelta();

    m_Yaw -= mouseDelta.x * m_Sensitivity;
    m_Pitch -= mouseDelta.y * m_Sensitivity;
    m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);

    glm::quat yawQuat = glm::angleAxis(glm::radians(m_Yaw), glm::vec3(0, 1, 0));
    glm::quat pitchQuat = glm::angleAxis(glm::radians(m_Pitch), glm::vec3(1, 0, 0));

    auto children = m_Owner->GetChildren();
    for (Termina::Actor* child : children)
    {
        if (child->HasComponent<Termina::CameraComponent>())
        {
			child->AddComponent<Termina::Transform>().SetRotation(yawQuat * pitchQuat);
            break;
        }
    }

    auto& controller = m_Owner->GetComponent<Termina::CharacterController>();

    Termina::Camera camera = m_Owner->GetParentWorld()->GetMainCamera();

    glm::vec3 camForward = camera.Direction;
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 camRight = glm::normalize(glm::cross(camForward, worldUp));

    glm::vec3 moveForward = glm::normalize(glm::vec3(camForward.x, 0.0f, camForward.z));
    glm::vec3 moveRight = glm::normalize(glm::vec3(camRight.x, 0.0f, camRight.z));

    bool isMoving = (Input::IsKeyHeld(Termina::Key::W) || Input::IsKeyHeld(Termina::Key::S) ||
        Input::IsKeyHeld(Termina::Key::A) || Input::IsKeyHeld(Termina::Key::D));

    bool wantsToSprint = Input::IsKeyHeld(Termina::Key::LeftShift) && isMoving && m_Stamina > 0.0f;

    if (wantsToSprint)
    {
        m_CurrentSpeed = m_SprintSpeed;
        m_Stamina -= m_StaminaDrain * deltaTime;
    }
    else
    {
        m_CurrentSpeed = m_WalkSpeed;
        m_Stamina += m_StaminaRegen * deltaTime;
    }
    m_Stamina = std::clamp(m_Stamina, 0.0f, m_MaxStamina);

    glm::vec3 wishDir(0.0f);
    if (Input::IsKeyHeld(Termina::Key::W)) wishDir += moveForward;
    if (Input::IsKeyHeld(Termina::Key::S)) wishDir -= moveForward;
    if (Input::IsKeyHeld(Termina::Key::D)) wishDir += moveRight;
    if (Input::IsKeyHeld(Termina::Key::A)) wishDir -= moveRight;

    if (glm::length(wishDir) > 1.0f)
        wishDir = glm::normalize(wishDir);

    glm::vec3 finalVelocity = wishDir * m_CurrentSpeed;

    if (controller.IsGrounded())
    {
        m_VerticalVelocity = 0.0f;
        if (Input::IsKeyPressed(Termina::Key::Space))
            m_VerticalVelocity = m_JumpPower;
    }
    else
    {
        m_VerticalVelocity -= 9.81f * deltaTime;
    }

    finalVelocity.y = m_VerticalVelocity;

    controller.Move(finalVelocity);
}

void PlayerController::Start()
{
    TerminaScript::Input::SetCursorVisible(false);
    TerminaScript::Input::SetCursorLocked(true);
}

void PlayerController::Serialize(nlohmann::json& out) const
{
    out["walkSpeed"] = m_WalkSpeed;
    out["sprintSpeed"] = m_SprintSpeed;
    out["stamina"] = m_Stamina;
    out["maxStamina"] = m_MaxStamina;
    out["staminaDrain"] = m_StaminaDrain;
    out["staminaRegen"] = m_StaminaRegen;
    out["jumpPower"] = m_JumpPower;
	out["sensitivity"] = m_Sensitivity; 
}

void PlayerController::Deserialize(const nlohmann::json& in)
{
    if (in.contains("walkSpeed")) m_WalkSpeed = in["walkSpeed"];
    if (in.contains("sprintSpeed")) m_SprintSpeed = in["sprintSpeed"];
    if (in.contains("jumpPower")) m_JumpPower = in["jumpPower"];
    if (in.contains("maxStamina")) m_MaxStamina = in["maxStamina"];
    if (in.contains("staminaDrain")) m_StaminaDrain = in["staminaDrain"];
    if (in.contains("staminaRegen")) m_StaminaRegen = in["staminaRegen"];
	if (in.contains("sensitivity")) m_Sensitivity = in["sensitivity"];

    m_Stamina = m_MaxStamina;
}

void PlayerController::Inspect()
{
    ImGui::DragFloat("Walk Speed", &m_WalkSpeed, 0.1f, 0.0f, 20.0f);
    ImGui::DragFloat("Sprint Speed", &m_SprintSpeed, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Jump Power", &m_JumpPower, 0.1f, 0.0f, 20.0f);
    ImGui::DragFloat("Max Stamina", &m_MaxStamina, 1.0f, 0.0f, 200.0f);
    ImGui::DragFloat("Stamina Drain", &m_StaminaDrain, 1.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Stamina Regen", &m_StaminaRegen, 1.0f, 0.0f, 100.0f);
    ImGui::DragFloat("Sensitivity", &m_Sensitivity, 0.01f, 0.01f, 1.0f);
}
#include "MobComponent.hpp"

#include <Termina/Renderer/UIUtils.hpp>
#include <ImGui/imgui.h>
#include <GLM/gtx/quaternion.hpp>
#include <cmath>
void MobComponent::FindPlayerActor()
{
    m_PlayerActor = m_Owner->GetParentWorld()->GetActorByName("Player");

    if (!m_PlayerActor)
    {
        TN_INFO("ERROR: Player not found. Check name!");
    }
}
void MobComponent::Start()
{
    FindPlayerActor();
}



void MobComponent::Update(float deltaTime)
{
    if (!m_PlayerActor)
        return;

    glm::vec3 myPos = m_Transform->GetPosition();

    auto& playerTransform = m_PlayerActor->GetComponent<Termina::Transform>();
    glm::vec3 playerPos = playerTransform.GetPosition();

    glm::vec3 direction = playerPos - myPos;
    float distance = glm::length(direction);

    if (distance < 0.001f)
        return;

    direction = glm::normalize(direction);

    if (distance > m_StopDistance)
    {
        myPos += direction * m_Speed * deltaTime;
        m_Transform->SetPosition(myPos);
    }
    glm::quat rotation = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
    m_Transform->SetRotation(rotation);
}


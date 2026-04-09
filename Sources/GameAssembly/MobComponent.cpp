#include "MobComponent.hpp"

#include <Termina/Core/Logger.hpp>
#include <GLM/gtx/quaternion.hpp>

void MobComponent::Start()
{
    FindPlayerActor();
}

void MobComponent::FindPlayerActor()
{
    m_PlayerActor = m_Owner->GetParentWorld()->GetActorByName("Player");

    if (!m_PlayerActor)
    {
        TN_INFO("MobComponent: Player not found!");
    }
}

void MobComponent::OnCollisionEnter(Termina::Actor* other)
{
    if (!other)
        return;

    // Si collision avec un autre mob (slime)
    if (other->HasComponent<MobComponent>())
    {
        m_IsBlocked = true;
        m_BlockTimer = 0.2f;
    }
}

void MobComponent::Update(float deltaTime)
{
    // Gestion du blocage
    if (m_IsBlocked)
    {
        m_BlockTimer -= deltaTime;

        if (m_BlockTimer <= 0.0f)
        {
            m_IsBlocked = false;
        }
        else
        {
            return; // stop temporairement
        }
    }

    if (!m_PlayerActor)
        return;

    if (!m_PlayerActor->HasComponent<Termina::Transform>())
        return;

    // Position du mob
    glm::vec3 myPos = m_Transform->GetPosition();

    // Position du joueur
    auto& playerTransform = m_PlayerActor->GetComponent<Termina::Transform>();
    glm::vec3 playerPos = playerTransform.GetPosition();

    // Direction vers le joueur
    glm::vec3 direction = playerPos - myPos;

    // Bloquer l’axe Y
    direction.y = 0.0f;

    float distance = glm::length(direction);

    if (distance < 0.001f)
        return;

    direction = glm::normalize(direction);

    // Déplacement vers le joueur
    if (distance > m_StopDistance)
    {
        myPos += direction * m_Speed * deltaTime;
        m_Transform->SetPosition(myPos);
    }

    // Rotation vers le joueur (avec correction de 90°)
    glm::quat look = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
    glm::quat offset = glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 1, 0));

    m_Transform->SetRotation(look * offset);
}
#include "SpawnerComponent.hpp"
#include <Termina/Core/Logger.hpp>

void SpawnerComponent::Start()
{
    // We leave this empty since we only want to spawn when you press the button!
}

void SpawnerComponent::Update(float deltaTime)
{
    // 1. Decrease the cooldown timer using the time since the last frame
    if (m_SpawnCooldown > 0.0f)
    {
        m_SpawnCooldown -= deltaTime;
    }

    // 2. Check if Spacebar is held AND our cooldown timer has reached 0
    if (Input::IsKeyHeld(Termina::Key::Space) && m_SpawnCooldown <= 0.0f)
    {
        // 3. Spawn the object!
        Termina::Actor* newEntity = Instantiate();
        TN_INFO("Witch Hunt: Spawned a new entity via Spacebar!");

        // Move it slightly so it doesn't spawn stuck in the floor
        auto& tr = newEntity->GetComponent<Termina::Transform>();
        tr.SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));

        // Add a physics collider so it can interact with the world
        auto& col = newEntity->AddComponent<Termina::BoxCollider>();
        col.HalfExtents = glm::vec3(0.5f);

        // 4. Reset the cooldown timer to half a second
        m_SpawnCooldown = 0.5f;
    }
}
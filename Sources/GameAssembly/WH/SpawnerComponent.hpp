#pragma once
#include <Termina/Scripting/API/ScriptingAPI.hpp>
#include <Termina/Physics/Components/BoxCollider.hpp>
#include <Termina/Physics/Components/SphereCollider.hpp>
#include <Termina/Physics/Components/CapsuleCollider.hpp>
#include <Termina/Physics/Components/CylinderCollider.hpp>
#include <Termina/World/Components/Transform.hpp>

using namespace TerminaScript;

class SpawnerComponent : public TerminaScript::ScriptableComponent
{
public:
    SpawnerComponent() = default;
    SpawnerComponent(Termina::Actor* owner) : TerminaScript::ScriptableComponent(owner) {}

    void Start() override;
    void Update(float deltaTime) override;

private:
    // Our cooldown timer to prevent spawning 60 objects per second!
    float m_SpawnCooldown = 0.0f;
};
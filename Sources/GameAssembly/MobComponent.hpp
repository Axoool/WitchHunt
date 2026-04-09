#pragma once

#include <Termina/Scripting/API/ScriptingAPI.hpp>

using namespace TerminaScript;

class MobComponent : public TerminaScript::ScriptableComponent
{
public:
    MobComponent() = default;
    MobComponent(Termina::Actor* owner) : TerminaScript::ScriptableComponent(owner) {}

    void Start() override;
    void Update(float deltaTime) override;
    void OnCollisionEnter(Termina::Actor* other) override;

private:
    void FindPlayerActor();

private:
    Termina::Actor* m_PlayerActor = nullptr;

    float m_Speed = 3.0f;
    float m_StopDistance = 1.5f;

    // Gestion du blocage entre slimes
    bool m_IsBlocked = false;
    float m_BlockTimer = 0.0f;
};
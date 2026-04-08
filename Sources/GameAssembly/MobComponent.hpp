#pragma once

#include <Termina/Scripting/API/ScriptingAPI.hpp>

using namespace TerminaScript;

class MobComponent : public TerminaScript::ScriptableComponent
{
public:
    MobComponent() = default;
    MobComponent(Termina::Actor* owner) : TerminaScript::ScriptableComponent(owner) {}
    void Start()  override;
    void Update(float deltaTime) override;
    void FindPlayerActor();
private:
    Termina::Actor* m_PlayerActor = nullptr;

    float m_Speed = 3.0f;
    float m_StopDistance = 1.5f;
};

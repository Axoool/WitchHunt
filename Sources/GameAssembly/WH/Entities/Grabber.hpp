#pragma once
#include <Termina/Scripting/API/ScriptingAPI.hpp>

class Grabber : public TerminaScript::ScriptableComponent
{
public:
    Grabber() = default;
    Grabber(Termina::Actor* owner) : TerminaScript::ScriptableComponent(owner) {}

    void Update(float deltaTime) override;
    void OnCollisionEnter(Termina::Actor* other) override;
    void OnCollisionExit(Termina::Actor* other) override;

private:
    Termina::Actor* m_HoveredItem = nullptr;
    Termina::Actor* m_HeldItem = nullptr;

    // To remember the item's original size for the "Highlight" trick
    glm::vec3 m_OriginalScale = glm::vec3(1.0f);
};
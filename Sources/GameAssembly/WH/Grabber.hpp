#pragma once
#include <Termina/Scripting/API/ScriptingAPI.hpp>

class Grabber : public TerminaScript::ScriptableComponent
{
public:
    Grabber() = default;
    Grabber(Termina::Actor* owner) : TerminaScript::ScriptableComponent(owner) {}

    void Update(float deltaTime) override;

private:
    Termina::Actor* m_HoveredItem = nullptr;
    Termina::Actor* m_HeldItem = nullptr;
    float reachDistance = 3.0f;
    float hoverSize = 1.2f;

    // To remember the item's original size for the "Highlight" trick
    glm::vec3 m_OriginalScale = glm::vec3(1.0f);
};
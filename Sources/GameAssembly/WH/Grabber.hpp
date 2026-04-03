#pragma once
#include <Termina/Scripting/API/ScriptingAPI.hpp>
#include <Termina/World/World.hpp>
#include <Termina/World/Actor.hpp>

class Grabber : public TerminaScript::ScriptableComponent
{
public:
    Grabber() = default;
    Grabber(Termina::Actor* owner) : TerminaScript::ScriptableComponent(owner) {}

    void Update(float deltaTime) override;

    // Helper functions to safely find the actors by ID
    Termina::Actor* GetHoveredItem() const;
    Termina::Actor* GetHeldItem() const;

private:
    // Store IDs for memory safety
    uint64 m_HoveredItemID = 0;
    uint64 m_HeldItemID = 0;

    float reachDistance = 4.0f;
    float hoverSize = 1.1f;
    glm::vec3 m_OriginalScale = glm::vec3(1.0f);
};
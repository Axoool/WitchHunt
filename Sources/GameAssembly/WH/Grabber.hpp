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

    // Helper functions to safely fetch the actual actors
    Termina::Actor* GetHoveredItem() const
    {
        if (m_HoveredItemID == 0) return nullptr;
        return m_Owner->GetParentWorld()->GetActorById(m_HoveredItemID);
    }

    Termina::Actor* GetHeldItem() const
    {
        if (m_HeldItemID == 0) return nullptr;
        return m_Owner->GetParentWorld()->GetActorById(m_HeldItemID);
    }

private:
    // Store IDs instead of raw pointers!
    uint64 m_HoveredItemID = 0;
    uint64 m_HeldItemID = 0;

    float reachDistance = 3.0f;
    float hoverSize = 1.2f;

    glm::vec3 m_OriginalScale = glm::vec3(1.0f);
};
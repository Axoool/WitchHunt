#pragma once

#include <Termina/Scripting/API/ScriptingAPI.hpp>

using namespace TerminaScript;

/// Spawns a static funnel made of 4 angled box panels and drops 100 dynamic
/// rigid bodies (cycling through Box, Sphere, Capsule, Cylinder shapes) into
/// it when the scene starts playing.
class PhysicsTestComponent : public TerminaScript::ScriptableComponent
{
public:
    PhysicsTestComponent() = default;
    PhysicsTestComponent(Termina::Actor* owner) : TerminaScript::ScriptableComponent(owner) {}

    void Start() override;
    void OnCollisionEnter(Termina::Actor* other) override;

    void Update(float dt) override;

    void Inspect() override;

    void Serialize(nlohmann::json& out) const override;
    void Deserialize(const nlohmann::json& in) override;

    int   SpawnCount  = 100;
    float SpawnHeight = 5.0f;

private:
    void SpawnFunnel();
    void SpawnObjects();
    Termina::Actor* MakeFunnelPanel(const glm::vec3& pos, const glm::vec3& eulerDeg, const glm::vec3& halfExtents);
};

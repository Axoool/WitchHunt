#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>

#include <Termina/World/Component.hpp>
#include <GLM/glm.hpp>

namespace Termina {

    /// Character controller using JPH::CharacterVirtual.
    ///
    /// Does not add a rigid body to the simulation — movement is solved via
    /// continuous collision detection and applied each physics step.
    /// Use Move() to set the desired velocity before each physics step.
    class CharacterController : public Component
    {
    public:
        CharacterController() = default;
        CharacterController(Actor* owner) : Component(owner) {}
        ~CharacterController() override = default;

        // --- Properties ---
        float Height        = 1.8f;
        float Radius        = 0.35f;
        float MaxSlopeAngle = 50.0f;  // degrees
        float MaxStrength   = 100.0f;
        float Mass          = 70.0f;

        // --- Runtime API ---

        /// Set the desired linear velocity for the next physics step.
        void      Move(const glm::vec3& velocity);
        bool      IsGrounded() const;
        glm::vec3 GetLinearVelocity() const;

        // --- Lifecycle ---
        void OnPlay()  override;
        void OnStop()  override;
        void OnPhysics(float dt) override;

        void Inspect() override;
        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;

        UpdateFlags GetUpdateFlags() const override { return (UpdateFlags)0; }

    private:
        JPH::Ref<JPH::CharacterVirtual> m_Character;
    };

} // namespace Termina

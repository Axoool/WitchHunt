#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

#include <Termina/World/Component.hpp>
#include <GLM/glm.hpp>

namespace Termina {

    /// Rigidbody component. Must be on the same actor as a Collider subclass.
    ///
    /// Bodies are created when the world enters play mode (OnPlay) and destroyed
    /// when play stops (OnStop). Transform is synced to/from Jolt every physics step.
    class Rigidbody : public Component
    {
    public:
        Rigidbody() = default;
        Rigidbody(Actor* owner) : Component(owner) {}
        ~Rigidbody() override = default;

        // --- Properties (serialized, editable in inspector) ---

        enum class BodyType : uint8_t { Dynamic, Static, Kinematic };
        BodyType Type = BodyType::Dynamic;

        float Mass           = 1.0f;
        float LinearDamping  = 0.05f;
        float AngularDamping = 0.05f;
        float GravityFactor  = 1.0f;
        bool  IsSensor       = false;
        bool  AllowSleep     = true;

        bool FreezePositionX = false;
        bool FreezePositionY = false;
        bool FreezePositionZ = false;
        bool FreezeRotationX = false;
        bool FreezeRotationY = false;
        bool FreezeRotationZ = false;

        // --- Runtime API ---

        JPH::BodyID GetBodyID() const { return m_BodyID; }
        bool IsBodyValid() const;

        void AddForce(const glm::vec3& force);
        void AddImpulse(const glm::vec3& impulse);
        void AddTorque(const glm::vec3& torque);
        void SetLinearVelocity(const glm::vec3& velocity);
        glm::vec3 GetLinearVelocity() const;
        glm::vec3 GetAngularVelocity() const;

        // --- Lifecycle ---
        void OnPlay()  override;
        void OnStop()  override;

        // Sync transform ↔ Jolt:
        //   PrePhysics:  Transform → Jolt  (kinematic/static)
        //   PostPhysics: Jolt → Transform  (dynamic)
        void OnPrePhysics(float dt)  override;
        void OnPostPhysics(float dt) override;

        void Inspect() override;
        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;

        UpdateFlags GetUpdateFlags() const override { return (UpdateFlags)0; }

    private:
        JPH::BodyID m_BodyID = JPH::BodyID();
    };

} // namespace Termina

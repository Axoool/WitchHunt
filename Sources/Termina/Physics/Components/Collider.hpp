#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

#include <Termina/World/Component.hpp>

#include <GLM/glm.hpp>
#include <GLM/gtc/quaternion.hpp>

namespace Termina {

    /// Abstract base for all collider components.
    /// Subclasses create the appropriate Jolt shape; the Rigidbody component
    /// calls CreateShape() when entering play mode to build the physics body.
    ///
    /// In editor mode (no Jolt body), each collider draws its own wireframe
    /// via the DebugPass so the designer can see and tweak its dimensions.
    class Collider : public Component
    {
    public:
        Collider() = default;
        Collider(Actor* owner) : Component(owner) {}

        /// Local position offset relative to the actor's transform.
        glm::vec3 Offset       = glm::vec3(0.0f);
        /// Local rotation offset relative to the actor's transform.
        glm::quat RotationOffset = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

        /// Build and return a Jolt shape representing this collider.
        /// Called by Rigidbody::OnPlay(); must not be called before that.
        virtual JPH::ShapeRefC CreateShape() const = 0;

        /// Draw the collider wireframe using DebugPass (editor mode only).
        virtual void DrawShape(const glm::mat4& worldMatrix) const = 0;

        // Draw in editor (no Jolt body yet).
        void OnPreRender(float dt) override;

        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;

        /// Colliders update during editor for debug drawing and during gameplay
        /// for Rigidbody to call OnPreRender (editor wireframes).
        UpdateFlags GetUpdateFlags() const override
        {
            return UpdateFlags::UpdateDuringEditor | UpdateFlags::RenderUpdateDuringEditor;
        }
    };

} // namespace Termina

#pragma once

#include "Collider.hpp"

namespace Termina {

    /// An infinite plane collider, useful for floors and walls.
    /// Implemented as JPH::PlaneShape.
    class PlaneCollider : public Collider
    {
    public:
        PlaneCollider() = default;
        PlaneCollider(Actor* owner) : Collider(owner) {}

        /// Normal direction of the plane surface (local space, default = Y-up).
        glm::vec3 Normal    = glm::vec3(0.0f, 1.0f, 0.0f);
        /// Half-size of the debug visualization quad.
        float     DebugSize = 5.0f;

        JPH::ShapeRefC CreateShape() const override;
        void DrawShape(const glm::mat4& worldMatrix) const override;
        void Inspect() override;
        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;
    };

} // namespace Termina

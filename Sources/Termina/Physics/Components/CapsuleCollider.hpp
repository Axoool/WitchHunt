#pragma once

#include "Collider.hpp"

namespace Termina {

    class CapsuleCollider : public Collider
    {
    public:
        CapsuleCollider() = default;
        CapsuleCollider(Actor* owner) : Collider(owner) {}

        float HalfHeight = 0.5f;
        float Radius     = 0.25f;

        JPH::ShapeRefC CreateShape() const override;
        void DrawShape(const glm::mat4& worldMatrix) const override;
        void Inspect() override;
        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;
    };

} // namespace Termina

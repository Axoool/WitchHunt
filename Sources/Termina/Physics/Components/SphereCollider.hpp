#pragma once

#include "Collider.hpp"

namespace Termina {

    class SphereCollider : public Collider
    {
    public:
        SphereCollider() = default;
        SphereCollider(Actor* owner) : Collider(owner) {}

        float Radius = 0.5f;

        JPH::ShapeRefC CreateShape() const override;
        void DrawShape(const glm::mat4& worldMatrix) const override;
        void Inspect() override;
        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;
    };

} // namespace Termina

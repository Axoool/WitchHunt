#pragma once

#include "Collider.hpp"
#include <GLM/glm.hpp>

namespace Termina {

    class BoxCollider : public Collider
    {
    public:
        BoxCollider() = default;
        BoxCollider(Actor* owner) : Collider(owner) {}

        glm::vec3 HalfExtents = glm::vec3(0.5f);

        JPH::ShapeRefC CreateShape() const override;
        void DrawShape(const glm::mat4& worldMatrix) const override;
        void Inspect() override;
        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;
    };

} // namespace Termina

#pragma once

#include "Collider.hpp"

#include <Termina/Asset/AssetHandle.hpp>
#include <Termina/Asset/Model/ModelAsset.hpp>

namespace Termina {

    /// Mesh collider that builds a JPH::MeshShape from a ModelAsset's CPU geometry.
    /// NOTE: Only suitable for Static / Kinematic bodies (non-moving layer).
    /// For dynamic convex objects use CapsuleCollider or BoxCollider instead.
    class MeshCollider : public Collider
    {
    public:
        MeshCollider() = default;
        MeshCollider(Actor* owner) : Collider(owner) {}

        AssetHandle<ModelAsset> Mesh;

        JPH::ShapeRefC CreateShape() const override;
        void DrawShape(const glm::mat4& worldMatrix) const override;
        void Inspect() override;
        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;
    };

} // namespace Termina

#pragma once

#include <Termina/World/Component.hpp>
#include <Termina/Asset/AssetHandle.hpp>
#include <Termina/Asset/Model/ModelAsset.hpp>

namespace Termina {

    /// Attaches a static GLTF model to an actor. The MeshPass will render
    /// every MeshInstance in the model at the actor's world transform.
    class MeshComponent : public Component
    {
    public:
        MeshComponent() = default;
        MeshComponent(Actor* owner) : Component(owner) {}

        AssetHandle<ModelAsset> Model;

        void Inspect() override;
        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;

        UpdateFlags GetUpdateFlags() const override { return (UpdateFlags)0; }
    };

} // namespace Termina

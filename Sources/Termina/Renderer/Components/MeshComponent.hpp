#pragma once

#include <Termina/World/Component.hpp>
#include <Termina/Asset/AssetHandle.hpp>
#include <Termina/Asset/Model/ModelAsset.hpp>
#include <Termina/Asset/Material/MaterialAsset.hpp>

#include <unordered_map>

namespace Termina {

    /// Attaches a static GLTF model to an actor. The MeshPass will render
    /// every MeshInstance in the model at the actor's world transform.
    ///
    /// MaterialOverrides lets you replace individual material slots without
    /// re-exporting the model. Key = slot index from ModelAsset::Materials.
    class MeshComponent : public Component
    {
    public:
        MeshComponent() = default;
        MeshComponent(Actor* owner) : Component(owner) {}

        AssetHandle<ModelAsset> Model;

        /// Per-slot material overrides. Entries absent from this map fall back
        /// to the model's own material for that slot.
        std::unordered_map<uint32, AssetHandle<MaterialAsset>> MaterialOverrides;

        void Inspect() override;
        void Serialize(nlohmann::json& out) const override;
        void Deserialize(const nlohmann::json& in) override;

        UpdateFlags GetUpdateFlags() const override { return (UpdateFlags)0; }
    };

} // namespace Termina

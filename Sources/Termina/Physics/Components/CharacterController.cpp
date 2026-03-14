#include "CharacterController.hpp"

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Body/BodyFilter.h>
#include <Jolt/Physics/Collision/ShapeFilter.h>
#include <Jolt/Core/TempAllocator.h>

#include <Termina/Core/Application.hpp>
#include <Termina/Core/Logger.hpp>
#include <Termina/World/Actor.hpp>
#include <Termina/World/Components/Transform.hpp>
#include <Termina/Physics/PhysicsSystem.hpp>

#include <ImGui/imgui.h>

#include <GLM/gtc/constants.hpp>

namespace Termina {

    void CharacterController::OnPlay()
    {
        if (!m_Owner) return;

        auto& transform = m_Owner->GetComponent<Transform>();
        glm::vec3 pos = transform.GetPosition();
        glm::quat rot = transform.GetRotation();

        // Build a standing capsule: rotated so Y is up.
        JPH::RefConst<JPH::Shape> capsuleShape;
        {
            JPH::CapsuleShapeSettings cs(Height * 0.5f, Radius);
            auto result = cs.Create();
            if (!result.IsValid())
            {
                TN_ERROR("CharacterController: failed to create capsule shape");
                return;
            }
            // Rotate capsule from Z-up to Y-up (Jolt capsules are Y-axis by default in newer builds).
            capsuleShape = result.Get();
        }

        JPH::CharacterVirtualSettings settings;
        settings.mShape          = capsuleShape;
        settings.mMaxSlopeAngle  = glm::radians(MaxSlopeAngle);
        settings.mMaxStrength    = MaxStrength;
        settings.mMass           = Mass;

        auto* physics = Application::GetSystem<PhysicsSystem>();

        m_Character = new JPH::CharacterVirtual(&settings,
                                                JPH::RVec3(pos.x, pos.y, pos.z),
                                                JPH::Quat(rot.x, rot.y, rot.z, rot.w),
                                                0,
                                                &physics->GetJoltSystem());
    }

    void CharacterController::OnStop()
    {
        m_Character = nullptr;
    }

    void CharacterController::OnPhysics(float dt)
    {
        if (!m_Character || !m_Owner) return;

        auto* physics = Application::GetSystem<PhysicsSystem>();
        JPH::PhysicsSystem& jolt = physics->GetJoltSystem();

        JPH::Vec3 gravity = jolt.GetGravity();

        JPH::TempAllocatorMalloc tempAlloc;
        m_Character->Update(
            dt,
            gravity,
            jolt.GetDefaultBroadPhaseLayerFilter(PhysicsLayers::CHARACTER),
            jolt.GetDefaultLayerFilter(PhysicsLayers::CHARACTER),
            JPH::BodyFilter{},
            JPH::ShapeFilter{},
            tempAlloc
        );

        // Sync position → Transform.
        JPH::RVec3 jPos = m_Character->GetPosition();
        auto& transform = m_Owner->GetComponent<Transform>();
        transform.SetPosition(glm::vec3(jPos.GetX(), jPos.GetY(), jPos.GetZ()));
    }

    void CharacterController::Move(const glm::vec3& velocity)
    {
        if (!m_Character) return;
        m_Character->SetLinearVelocity(JPH::Vec3(velocity.x, velocity.y, velocity.z));
    }

    bool CharacterController::IsGrounded() const
    {
        if (!m_Character) return false;
        return m_Character->IsSupported();
    }

    glm::vec3 CharacterController::GetLinearVelocity() const
    {
        if (!m_Character) return glm::vec3(0.0f);
        JPH::Vec3 v = m_Character->GetLinearVelocity();
        return glm::vec3(v.GetX(), v.GetY(), v.GetZ());
    }

    void CharacterController::Inspect()
    {
        ImGui::DragFloat("Height",         &Height,       0.01f, 0.1f, 10.0f);
        ImGui::DragFloat("Radius",         &Radius,       0.01f, 0.01f, 5.0f);
        ImGui::DragFloat("Max Slope (deg)",&MaxSlopeAngle,1.0f,  0.0f, 90.0f);
        ImGui::DragFloat("Max Strength",   &MaxStrength,  1.0f,  0.0f, 1000.0f);
        ImGui::DragFloat("Mass",           &Mass,         1.0f,  0.1f, 500.0f);
    }

    void CharacterController::Serialize(nlohmann::json& out) const
    {
        out["height"]          = Height;
        out["radius"]          = Radius;
        out["max_slope_angle"] = MaxSlopeAngle;
        out["max_strength"]    = MaxStrength;
        out["mass"]            = Mass;
    }

    void CharacterController::Deserialize(const nlohmann::json& in)
    {
        if (in.contains("height"))          Height        = in["height"];
        if (in.contains("radius"))          Radius        = in["radius"];
        if (in.contains("max_slope_angle")) MaxSlopeAngle = in["max_slope_angle"];
        if (in.contains("max_strength"))    MaxStrength   = in["max_strength"];
        if (in.contains("mass"))            Mass          = in["mass"];
    }

} // namespace Termina

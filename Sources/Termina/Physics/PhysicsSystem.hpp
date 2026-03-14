#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>

#include <GLM/glm.hpp>

#include <Termina/Core/System.hpp>

#include <mutex>
#include <unordered_map>
#include <vector>

namespace Termina {
    constexpr float PhysicsUpdateRate = 1.0f / 90.0f;

    class Actor;
    class JoltDebugRenderer;

    namespace PhysicsLayers
    {
        static constexpr uint8 NON_MOVING = 0;
        static constexpr uint8 MOVING = 1;
        static constexpr uint8 CHARACTER = 2;
        static constexpr uint8 CHARACTER_GHOST = 3;
        static constexpr uint8 TRIGGER = 4;
        static constexpr uint8 NUM_LAYERS = 5;
    };

    struct Ray
    {
        glm::vec3 Origin;
        glm::vec3 Direction;
        float MaxDistance;
    };

    struct RayResult
    {
        bool Hit = false;
        glm::vec3 Position;
        float T = 0.0f;
        Actor* HitActor = nullptr;
    };

    class PhysicsSystem : public ISystem
    {
    public:
        PhysicsSystem();
        ~PhysicsSystem();

        void Physics(float deltaTime) override;
        void PostPhysics(float deltaTime) override;
        void PreRender(float deltaTime) override;

        void RegisterComponents() override;
        void UnregisterComponents() override;

        void SetGravity(const glm::vec3& gravity);
        RayResult Raycast(const Ray& ray) const;

        // Body ↔ Actor registry used by Rigidbody components.
        void   RegisterBody(JPH::BodyID id, Actor* actor);
        void   UnregisterBody(JPH::BodyID id);
        Actor* GetActorForBody(JPH::BodyID id) const;

        // Accessors for components that need to communicate directly with Jolt.
        JPH::PhysicsSystem& GetJoltSystem()    { return *m_System; }
        JPH::BodyInterface& GetBodyInterface() { return *m_BodyInterface; }

        // Toggle the Jolt debug overlay (draws body shapes each frame via DebugPass).
        void SetDebugDrawEnabled(bool enabled) { m_DebugDrawEnabled = enabled; }
        bool IsDebugDrawEnabled() const        { return m_DebugDrawEnabled; }

        void ShowDebugWindow(bool* open = nullptr);

        UpdateFlags GetUpdateFlags() const override { return (UpdateFlags)0; }
        std::string GetName()        const override { return "Physics System"; }
        int         GetPriority()    const override { return 10; }

    private:
        JPH::PhysicsSystem*       m_System       = nullptr;
        JPH::JobSystemThreadPool* m_ThreadPool    = nullptr;
        JPH::BodyInterface*       m_BodyInterface = nullptr;
        JPH::TempAllocatorMalloc* m_TempAllocator = nullptr;

        // Body → Actor mapping (BodyID index used as key for fast lookup).
        std::unordered_map<uint32_t, Actor*> m_BodyActorMap;

        // Contact events queued on Jolt worker threads, drained in PostPhysics.
        struct ContactEvent
        {
            JPH::BodyID A, B;
            enum class Type : uint8_t { Enter, Stay, Exit } type;
            bool isTrigger = false;
        };
        std::vector<ContactEvent> m_PendingContacts;
        mutable std::mutex        m_ContactMutex;

        // Contact listener implementation (defined in PhysicsSystem.cpp).
        class ContactListenerImpl;
        ContactListenerImpl* m_ContactListener = nullptr;

        JoltDebugRenderer* m_DebugRenderer    = nullptr;
        bool               m_DebugDrawEnabled = false;
    };
}

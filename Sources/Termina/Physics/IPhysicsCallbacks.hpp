#pragma once

namespace Termina {
    class Actor;

    /// Interface implemented by components that want to receive physics collision/trigger callbacks.
    /// Kept in the engine core so PhysicsSystem can dispatch events without depending on the scripting DLL.
    class IPhysicsCallbacks
    {
    public:
        virtual ~IPhysicsCallbacks() = default;

        virtual void OnCollisionEnter(Actor* other) {}
        virtual void OnCollisionStay(Actor* other)  {}
        virtual void OnCollisionExit(Actor* other)  {}
        virtual void OnTriggerEnter(Actor* other)   {}
        virtual void OnTriggerStay(Actor* other)    {}
        virtual void OnTriggerExit(Actor* other)    {}
    };
}

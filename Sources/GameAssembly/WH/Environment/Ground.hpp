#pragma once
#include <Termina/World/Component.hpp>

namespace TerminaScript {
    class Ground : public Termina::Component {
    public:
        // 1. Fulfill the constructor requirement so the Registry can pass the Actor pointer
        Ground(Termina::Actor* owner) : Termina::Component(owner) {}

        // 2. Fulfill the pure virtual function requirement
        Termina::UpdateFlags GetUpdateFlags() const override {
            // Since this is just a tag, we don't need it to update. 
            // Casting 0 is a safe C++ trick for returning an empty/None enum flag.
            return static_cast<Termina::UpdateFlags>(0);
        }
    };
}
#pragma once
#include <Termina/Scripting/API/ScriptingAPI.hpp>

class Grabbable : public TerminaScript::ScriptableComponent
{
public:
    Grabbable() = default;
    Grabbable(Termina::Actor* owner) : TerminaScript::ScriptableComponent(owner) {}
};
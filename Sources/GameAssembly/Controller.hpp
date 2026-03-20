#pragma once
#include <Termina/Scripting/API/ScriptingAPI.hpp>

using namespace TerminaScript;

class Controller : public ScriptableComponent
{
public:
	Controller() = default;
	Controller(Termina::Actor* owner) : ScriptableComponent(owner) {}

	void Start() override;
	void Update(float deltaTime) override;
};

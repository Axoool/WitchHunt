#include <Termina/Scripting/API/ScriptingAPI.hpp>
#include <Termina/World/ComponentRegistry.hpp>
#include <ImGui/imgui.h>

#include "FlyCamComponent.hpp"
#include "ParticleSystem.hpp"
#include "PhysicsTestComponent.hpp"

//
#include "WH/ChoppingBlock.hpp"
#include "WH/Tools.hpp"
#include "WH/Ingredients.hpp"

#include "WH/SpawnerComponent.hpp"

#include "WH/Grabber.hpp"
#include "WH/Grabbable.hpp"

// 1. Move the ImGui context setup to the global scope (outside the macro)
TERMINA_DLL_EXPORT void SetImGuiContext(void* ctx, void* allocFunc, void* freeFunc, void* userData)
{
    ImGui::SetCurrentContext(static_cast<ImGuiContext*>(ctx));
    ImGui::SetAllocatorFunctions(
        reinterpret_cast<ImGuiMemAllocFunc>(allocFunc),
        reinterpret_cast<ImGuiMemFreeFunc>(freeFunc),
        userData);
}

// 2. Keep the component registration tightly inside the macros
COMPONENT_MODULE_BEGIN()
REGISTER_COMPONENT(FlyCamComponent, "Fly Cam Component")
REGISTER_COMPONENT(ParticleSystemComponent, "Particle System")
REGISTER_COMPONENT(PhysicsTestComponent, "Physics Test")
REGISTER_COMPONENT(SpawnerComponent, "Spawner Component")

REGISTER_COMPONENT(Grabber, "Grabber")
REGISTER_COMPONENT(Grabbable, "Grabbable")
REGISTER_COMPONENT(ChoppingBlock, "ChoppingBlock Component")
REGISTER_COMPONENT(Ingredient, "Ingredient")
REGISTER_COMPONENT(Tool, "Tools")
COMPONENT_MODULE_END()
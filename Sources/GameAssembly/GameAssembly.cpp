#include <Termina/Scripting/API/ScriptingAPI.hpp>
#include <Termina/World/ComponentRegistry.hpp>
#include <ImGui/imgui.h>

#include "FlyCamComponent.hpp"
#include "ParticleSystem.hpp"
#include "PhysicsTestComponent.hpp"

//
#include "WH/Entities/SpawnerComponent.hpp"

#include "WH/Entities/Grabber.hpp"
#include "WH/Entities/Grabbable.hpp"

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
COMPONENT_MODULE_END()
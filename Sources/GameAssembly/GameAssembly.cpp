#include <Termina/Scripting/API/ScriptingAPI.hpp>
#include <Termina/World/ComponentRegistry.hpp>
#include <ImGui/imgui.h>

#include "FlyCamComponent.hpp"
#include "ParticleSystem.hpp"
#include "PhysicsTestComponent.hpp"

// --- STATIONS ---
#include "WH/ChoppingBlock.hpp"
#include "WH/MortarPestleBlock.hpp"

// --- INGREDIENTS ---
#include "WH/Ingredients/Ingre_Flower.hpp"
#include "WH/Ingredients/Ingre_CutFlower.hpp"
#include "WH/Ingredients/Ingre_CrushedFlower.hpp"

#include "WH/Ingredients/Ingre_Mushroom.hpp"
#include "WH/Ingredients/Ingre_CutMushroom.hpp"
#include "WH/Ingredients/Ingre_CrushedMushroom.hpp"

// --- CORE / UTILS ---
#include "WH/SpawnerComponent.hpp"
#include "WH/Grabber.hpp"
#include "WH/Grabbable.hpp"

// --- TOOLS ---
#include "WH/Tools/Knife.hpp"
#include "WH/Tools/Pestle.hpp"

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

// --- STATIONS REGISTRATION ---
REGISTER_COMPONENT(ChoppingBlock, "ChoppingBlock Component")
REGISTER_COMPONENT(MortarPestleBlock, "MortarPestleBlock Component")

// --- TOOLS REGISTRATION ---
REGISTER_COMPONENT(Knife, "Tool: Knife")
REGISTER_COMPONENT(Pestle, "Tool: Pestle")

// --- INGREDIENTS REGISTRATION ---
REGISTER_COMPONENT(Ingre_Flower, "Ingredient: Flower (RAW)")
REGISTER_COMPONENT(Ingre_CutFlower, "Ingredient: Flower (CUT)")
REGISTER_COMPONENT(Ingre_CrushedFlower, "Ingredient: Flower (CRUSHED)")

REGISTER_COMPONENT(Ingre_Mushroom, "Ingredient: Mushroom (RAW)")
REGISTER_COMPONENT(Ingre_CutMushroom, "Ingredient: Mushroom (CUT)")
REGISTER_COMPONENT(Ingre_CrushedMushroom, "Ingredient: Mushroom (CRUSHED)")

COMPONENT_MODULE_END()
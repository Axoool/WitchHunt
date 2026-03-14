#include "World.hpp"
#include "WorldSystem.hpp"
#include "ComponentRegistry.hpp"
#include "Renderer/Components/CameraComponent.hpp"
#include "Renderer/Renderer.hpp"

#include <Termina/Core/Application.hpp>
#include <Termina/Core/ID.hpp>
#include <Termina/Core/Logger.hpp>
#include <Termina/Scripting/ScriptModuleManager.hpp>
#include <ThirdParty/JSON/json.hpp>

#include <fstream>

namespace Termina {
    World::~World()
    {
        Clear();
    }

    Actor* World::SpawnActor()
    {
        auto actor = std::make_shared<Actor>(this);
        Actor* ptr = actor.get();
        m_Actors.push_back(std::move(actor));
        ptr->AddComponent<Transform>();
        ptr->OnInit();
        if (auto* ws = Application::GetSystem<WorldSystem>()) {
            if (ws->IsPlaying()) {
                ptr->OnPlay();
            }
        }
        return ptr;
    }

    Actor* World::SpawnActorFrom(Actor* actor)
    {
        Actor* newActor = SpawnActor();

        nlohmann::json temp;
        for (Component* comp : actor->GetAllComponents()) {
            std::string typeName = ComponentRegistry::Get().GetNameForType(typeid(*comp));
            if (typeName.empty()) continue; // unregistered — skip

            nlohmann::json compJson;
            compJson["type"]   = typeName;
            compJson["active"] = comp->IsActive();
            nlohmann::json data = nlohmann::json::object();
            comp->Serialize(data);
            compJson["data"] = std::move(data);

            temp.push_back(std::move(compJson));
        }
        for (const auto& compJson : temp) {
            std::string type = compJson.value("type", "");
            bool compActive  = compJson.value("active", true);
            const auto& data = compJson.contains("data") ? compJson["data"] : nlohmann::json::object();

            if (type == "Transform") {
                // Already added by SpawnActor — just deserialize its data.
                newActor->GetComponent<Transform>().Deserialize(data);
                newActor->GetComponent<Transform>().SetActive(compActive);
                continue;
            }

            Component* comp = ComponentRegistry::Get().CreateByName(type, newActor);
            if (!comp) {
                TN_WARN("Unknown component type '%s' — skipping.", type.c_str());
                continue;
            }
            comp->SetActive(compActive);
            comp->Deserialize(data);
            newActor->AddComponentRaw(comp);
        }
        return newActor;
    }

    Actor* World::SpawnActorFromJSON(const std::string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            TN_ERROR("Could not open prefab '%s' for reading.", filename.c_str());
            return nullptr;
        }

        nlohmann::json root;
        try {
            file >> root;
        } catch (const nlohmann::json::exception& e) {
            TN_ERROR("JSON parse error in prefab '%s': %s", filename.c_str(), e.what());
            return nullptr;
        }

        if (!root.contains("actors") || !root["actors"].is_array() || root["actors"].empty()) {
            TN_ERROR("Prefab '%s' does not contain any actors.", filename.c_str());
            return nullptr;
        }

        // Prefabs are stored as a flat list where the first actor is the root of the prefab.
        // We need to remap IDs because the IDs in the file might collide with existing ones.
        struct ActorEntry {
            Actor* actor;
            uint64 oldId;
            uint64 oldParentId;
        };
        std::vector<ActorEntry> entries;
        std::unordered_map<uint64, Actor*> idMap;

        for (const auto& actorJson : root["actors"]) {
            uint64 oldId = std::stoull(actorJson.value("id", "0"));
            std::string actorName = actorJson.value("name", "Actor");
            bool actorActive = actorJson.value("active", true);

            Actor* actor = SpawnActor();
            actor->SetName(actorName);
            actor->SetActive(actorActive);

            // Deserialize components.
            if (actorJson.contains("components")) {
                for (const auto& compJson : actorJson["components"]) {
                    std::string type = compJson.value("type", "");
                    bool compActive  = compJson.value("active", true);
                    const auto& data = compJson.contains("data") ? compJson["data"] : nlohmann::json::object();

                    if (type == "Transform") {
                        actor->GetComponent<Transform>().Deserialize(data);
                        actor->GetComponent<Transform>().SetActive(compActive);
                        continue;
                    }

                    Component* comp = ComponentRegistry::Get().CreateByName(type, actor);
                    if (comp) {
                        comp->SetActive(compActive);
                        comp->Deserialize(data);
                        actor->AddComponentRaw(comp);
                    }
                }
            }

            uint64 oldParentId = 0;
            if (actorJson.contains("parentId") && !actorJson["parentId"].is_null())
                oldParentId = std::stoull(actorJson["parentId"].get<std::string>());

            entries.push_back({ actor, oldId, oldParentId });
            idMap[oldId] = actor;
        }

        // Wire hierarchy within the prefab
        for (const auto& entry : entries) {
            if (entry.oldParentId != 0 && idMap.count(entry.oldParentId)) {
                idMap[entry.oldParentId]->AttachChild(entry.actor);
            }
        }

        return entries.empty() ? nullptr : entries[0].actor;
    }

    void World::SaveActorToJSON(Actor* actor, const std::string& filename)
    {
        if (!actor) return;

        nlohmann::json root;
        root["version"] = 1;
        
        nlohmann::json actorsJson = nlohmann::json::array();

        std::function<void(Actor*, bool)> serializeActorRecursive = [&](Actor* current, bool isRoot) {
            nlohmann::json actorJson;
            actorJson["id"]   = std::to_string(current->GetID());
            actorJson["name"] = current->GetName();
            actorJson["active"] = current->IsActive();

            if (!isRoot && current->GetParent())
                actorJson["parentId"] = std::to_string(current->GetParent()->GetID());
            else
                actorJson["parentId"] = nullptr;

            nlohmann::json compsJson = nlohmann::json::array();
            for (Component* comp : current->GetAllComponents()) {
                std::string typeName = ComponentRegistry::Get().GetNameForType(typeid(*comp));
                if (typeName.empty()) continue;

                nlohmann::json compJson;
                compJson["type"]   = typeName;
                compJson["active"] = comp->IsActive();
                nlohmann::json data = nlohmann::json::object();
                comp->Serialize(data);
                compJson["data"] = std::move(data);
                compsJson.push_back(std::move(compJson));
            }
            actorJson["components"] = std::move(compsJson);
            actorsJson.push_back(std::move(actorJson));

            for (Actor* child : current->GetChildren())
                serializeActorRecursive(child, false);
        };

        serializeActorRecursive(actor, true);
        root["actors"] = std::move(actorsJson);

        std::ofstream file(filename);
        if (!file.is_open()) {
            TN_ERROR("Could not open '%s' for writing prefab.", filename.c_str());
            return;
        }
        file << root.dump(4);
    }

    void World::DestroyActor(Actor* actor)
    {
        if (!actor) return;

        // Defer the erase if a lifecycle loop is currently iterating m_Actors.
        // FlushPendingDestroy() is called after every lifecycle dispatch.
        if (m_Iterating)
        {
            m_PendingDestroy.push_back(actor);
            return;
        }

        if (m_MainCamera    == actor) m_MainCamera    = nullptr;
        if (m_AudioListener == actor) m_AudioListener = nullptr;

        m_Actors.erase(
            std::remove_if(m_Actors.begin(), m_Actors.end(),
                [actor](const auto& a) { return a.get() == actor; }),
            m_Actors.end());
    }

    void World::FlushPendingDestroy()
    {
        for (Actor* actor : m_PendingDestroy)
        {
            if (m_MainCamera    == actor) m_MainCamera    = nullptr;
            if (m_AudioListener == actor) m_AudioListener = nullptr;

            m_Actors.erase(
                std::remove_if(m_Actors.begin(), m_Actors.end(),
                    [actor](const auto& a) { return a.get() == actor; }),
                m_Actors.end());
        }
        m_PendingDestroy.clear();
    }

    Actor* World::GetActorById(uint64 id)
    {
        for (uint64 i = 0; i < m_Actors.size(); ++i) {
            if (m_Actors[i]->GetID() == id) {
                return m_Actors[i].get();
            }
        }
        return nullptr;
    }

    Actor* World::GetActorByName(const std::string& name)
    {
        for (uint64 i = 0; i < m_Actors.size(); ++i) {
            if (m_Actors[i]->GetName() == name) {
                return m_Actors[i].get();
            }
        }
        return nullptr;
    }

    std::vector<Actor*> World::GetRootActors() const
    {
        std::vector<Actor*> rootActors;
        for (const auto& actorPtr : m_Actors) {
            if (actorPtr->GetParent() == nullptr) {
                rootActors.push_back(actorPtr.get());
            }
        }
        return rootActors;
    }

// Macro for lifecycle dispatches that must tolerate Instantiate() (push_back)
// and Destroy() (erase) called by component callbacks.
//
// - m_Iterating flag causes DestroyActor() to defer erases to m_PendingDestroy.
// - The count is captured *before* the loop so newly spawned actors (appended
//   via push_back) are not iterated this frame — they were already initialised
//   by SpawnActor()->OnInit().
// - Index-based access (m_Actors[i]) is reallocation-safe: operator[] always
//   recomputes the base pointer, unlike a cached iterator or range-for.
#define TN_WORLD_DISPATCH(call)                          \
    do {                                                 \
        m_Iterating = true;                              \
        const size_t _n = m_Actors.size();               \
        for (size_t _i = 0; _i < _n; ++_i)              \
            m_Actors[_i]->call;                          \
        m_Iterating = false;                             \
        FlushPendingDestroy();                           \
    } while (0)

    void World::OnInit()
    {
        TN_WORLD_DISPATCH(OnInit());
    }

    void World::OnShutdown()
    {
        TN_WORLD_DISPATCH(OnShutdown());
    }

    void World::OnPlay()
    {
        RendererSystem* renderer = Application::GetSystem<RendererSystem>();
        if (renderer) {
            renderer->SetCurrentCamera(GetMainCamera());
        }
        TN_WORLD_DISPATCH(OnPlay());
    }

    void World::OnStop()
    {
        TN_WORLD_DISPATCH(OnStop());
    }

    void World::OnPreUpdate(float deltaTime)
    {
        TN_WORLD_DISPATCH(OnPreUpdate(deltaTime));
    }

    void World::OnUpdate(float deltaTime)
    {
        TN_WORLD_DISPATCH(OnUpdate(deltaTime));
    }

    void World::OnPostUpdate(float deltaTime)
    {
        TN_WORLD_DISPATCH(OnPostUpdate(deltaTime));
    }

    void World::OnPrePhysics(float deltaTime)
    {
        TN_WORLD_DISPATCH(OnPrePhysics(deltaTime));
    }

    void World::OnPhysics(float deltaTime)
    {
        TN_WORLD_DISPATCH(OnPhysics(deltaTime));
    }

    void World::OnPostPhysics(float deltaTime)
    {
        TN_WORLD_DISPATCH(OnPostPhysics(deltaTime));
    }

    void World::OnPreRender(float deltaTime)
    {
        TN_WORLD_DISPATCH(OnPreRender(deltaTime));
    }

    void World::OnRender(float deltaTime)
    {
        TN_WORLD_DISPATCH(OnRender(deltaTime));
    }

    void World::OnPostRender(float deltaTime)
    {
        TN_WORLD_DISPATCH(OnPostRender(deltaTime));
    }

    void World::Clear()
    {
        m_Actors.clear();
        m_MainCamera = nullptr;
        m_AudioListener = nullptr;
    }

    void World::LoadFromFile(const std::string& filename)
    {
        m_CurrentPath = filename;

        std::ifstream file(filename);
        if (!file.is_open()) {
            TN_ERROR("Could not open '%s' for reading.", filename.c_str());
            return;
        }

        nlohmann::json root;
        try {
            file >> root;
        } catch (const nlohmann::json::exception& e) {
            TN_ERROR("JSON parse error in '%s': %s", filename.c_str(), e.what());
            return;
        }

        int version = root.value("version", 0);
        if (version != 1)
            TN_WARN("Unknown world version %d, attempting load anyway.", version);

        m_Name = root.value("name", "World");

        // Load DLL modules before deserializing actors.
        if (root.contains("modules")) {
            for (const auto& mod : root["modules"]) {
                std::string modName = mod.value("name", "");
                std::string modPath = mod.value("path", "");
                if (!modName.empty() && !modPath.empty() && !ScriptModuleManager::Get().IsLoaded(modName))
                    ScriptModuleManager::Get().Load(modName, modPath);
            }
        }

        Clear();
        IDGenerator::Get().Clear();

        if (!root.contains("actors")) return;

        // Pass 1: Spawn actors flat, deserialize components, inject saved IDs.
        struct ActorEntry {
            Actor* actor;
            uint64 parentId;
        };
        std::vector<ActorEntry> entries;

        for (const auto& actorJson : root["actors"]) {
            uint64 savedId = std::stoull(actorJson.value("id", "0"));
            std::string actorName = actorJson.value("name", "Actor");
            bool actorActive     = actorJson.value("active", true);

            // Construct actor directly to avoid SpawnActor's auto-OnInit.
            auto actorPtr = std::make_shared<Actor>(this, actorName);
            Actor* actor  = actorPtr.get();

            // Replace the auto-generated ID with the saved one.
            IDGenerator::Get().Release(actor->GetID());
            IDGenerator::Get().Reserve(savedId);
            actor->SetID(savedId);

            actor->SetActive(actorActive);

            // Auto-add Transform (mirrors what SpawnActor does).
            actor->AddComponent<Transform>();

            // Deserialize components.
            if (actorJson.contains("components")) {
                for (const auto& compJson : actorJson["components"]) {
                    std::string type = compJson.value("type", "");
                    bool compActive  = compJson.value("active", true);
                    const auto& data = compJson.contains("data") ? compJson["data"] : nlohmann::json::object();

                    if (type == "Transform") {
                        // Already added above — just deserialize its data.
                        actor->GetComponent<Transform>().Deserialize(data);
                        actor->GetComponent<Transform>().SetActive(compActive);
                        continue;
                    }

                    Component* comp = ComponentRegistry::Get().CreateByName(type, actor);
                    if (!comp) {
                        TN_WARN("Unknown component type '%s' — skipping.", type.c_str());
                        continue;
                    }
                    comp->SetActive(compActive);
                    comp->Deserialize(data);
                    actor->AddComponentRaw(comp);
                }
            }

            uint64 parentId = 0;
            if (actorJson.contains("parentId") && !actorJson["parentId"].is_null())
                parentId = std::stoull(actorJson["parentId"].get<std::string>());

            m_Actors.push_back(std::move(actorPtr));
            entries.push_back({ actor, parentId });
        }

        // Pass 2: Wire parent-child hierarchy.
        for (const auto& entry : entries) {
            if (entry.parentId == 0) continue;
            Actor* parent = GetActorById(entry.parentId);
            if (parent)
                parent->AttachChild(entry.actor);
            else
                TN_WARN("Parent ID %llu not found for actor '%s'.",
                    static_cast<unsigned long long>(entry.parentId), entry.actor->GetName().c_str());
        }

        // Pass 3: Initialize all actors.
        // Use index-based iteration so Instantiate() calls inside Awake()
        // (which push_back to m_Actors) cannot invalidate our loop state.
        // Only initialize the actors that existed before this pass — newly
        // spawned pool actors are already initialized by SpawnActor()->OnInit().
        {
            m_Iterating = true;
            const size_t initCount = m_Actors.size();
            for (size_t i = 0; i < initCount; ++i)
                m_Actors[i]->OnInit();
            m_Iterating = false;
            FlushPendingDestroy();
        }
    }

    void World::SaveToFile(const std::string& filename)
    {
        if (!filename.empty())
            m_CurrentPath = filename;

        if (m_CurrentPath.empty()) {
            TN_ERROR("SaveToFile called with no path.");
            return;
        }

        nlohmann::json root;
        root["version"] = 1;
        root["name"]    = m_Name;

        // Emit loaded script modules.
        nlohmann::json modulesJson = nlohmann::json::array();
        for (const auto& [modName, modPath] : ScriptModuleManager::Get().GetLoadedModules()) {
            modulesJson.push_back({ {"name", modName}, {"path", modPath} });
        }
        root["modules"] = modulesJson;

        // DFS traversal: parents before children.
        nlohmann::json actorsJson = nlohmann::json::array();

        std::function<void(Actor*)> serializeActor = [&](Actor* actor) {
            nlohmann::json actorJson;
            actorJson["id"]   = std::to_string(actor->GetID());
            actorJson["name"] = actor->GetName();
            actorJson["active"] = actor->IsActive();

            if (actor->GetParent())
                actorJson["parentId"] = std::to_string(actor->GetParent()->GetID());
            else
                actorJson["parentId"] = nullptr;

            nlohmann::json compsJson = nlohmann::json::array();
            for (Component* comp : actor->GetAllComponents()) {
                std::string typeName = ComponentRegistry::Get().GetNameForType(typeid(*comp));
                if (typeName.empty()) continue; // unregistered — skip

                nlohmann::json compJson;
                compJson["type"]   = typeName;
                compJson["active"] = comp->IsActive();
                nlohmann::json data = nlohmann::json::object();
                comp->Serialize(data);
                compJson["data"] = std::move(data);
                compsJson.push_back(std::move(compJson));
            }
            actorJson["components"] = std::move(compsJson);
            actorsJson.push_back(std::move(actorJson));

            for (Actor* child : actor->GetChildren())
                serializeActor(child);
        };

        for (Actor* root_actor : GetRootActors())
            serializeActor(root_actor);

        root["actors"] = std::move(actorsJson);

        std::ofstream file(m_CurrentPath);
        if (!file.is_open()) {
            TN_ERROR("Could not open '%s' for writing.", m_CurrentPath.c_str());
            return;
        }
        file << root.dump();
    }

    Camera World::GetMainCamera() const
    {
        if (m_MainCamera)
            return m_MainCamera->GetComponent<CameraComponent>().GetCamera();
        return Camera();
    }
}

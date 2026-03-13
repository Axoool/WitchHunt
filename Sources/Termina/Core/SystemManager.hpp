#pragma once

#include "System.hpp"

#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Termina {
    /// Manages the registration and update of systems.
    class SystemManager
    {
    public:
        SystemManager() = default;
        ~SystemManager();

        template<typename T, typename... Args>
        T* AddSystem(Args&&... args)
        {
            T* system = new T(std::forward<Args>(args)...);
            m_Subsystems[typeid(T)] = system;
            system->RegisterComponents();
            return system;
        }

        template<typename T>
        T* GetSystem()
        {
            return reinterpret_cast<T*>(m_Subsystems[typeid(T)]);
        }

        /// Begins the update cycle, sorting systems by priority.
        void Begin();

        /// Cleans up all registered systems.
        void Clean();

        void PreUpdate(float deltaTime);
        void Update(float deltaTime);
        void PostUpdate(float deltaTime);
        void PrePhysics(float deltaTime);
        void Physics(float deltaTime);
        void PostPhysics(float deltaTime);
        void PreRender(float deltaTime);
        void Render(float deltaTime);
        void PostRender(float deltaTime);

        /// Sets whether the editor is active, affecting system update behavior.
        void SetIsInEditor(bool isInEditor);

        /// Renders an ImGui window listing all registered systems sorted by priority.
        void ShowDebugWindow(bool* open = nullptr);
    private:
        std::unordered_map<std::type_index, ISystem*> m_Subsystems;
        std::vector<ISystem*> m_UpdateList;

        bool m_IsInEditor = false;
    };
}

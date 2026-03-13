#pragma once

#include "ScriptModuleManager.hpp"

#include <Termina/Core/System.hpp>
#include <Termina/Core/FileSystem.hpp>

namespace Termina {
    /// Represents the script system that manages script modules and their execution.
    class ScriptSystem : public ISystem
    {
    public:
        ScriptSystem();
        ~ScriptSystem();

        /// Compiles the script modules.
        bool Compile();
        /// Recompiles the script modules.
        bool Recompile();
        /// Checks for pending reloads and recompiles if necessary.
        void Update(float deltaTime) override;

        void ShowDebugWindow(bool* open = nullptr);

        UpdateFlags GetUpdateFlags() const override { return UpdateFlags::UpdateDuringEditor; }
        std::string GetName() const override { return "Script System"; }
        int GetPriority() const override { return -1; }

    private:
        void RebuildWatches();

        std::vector<FileSystem::Watch> m_Watches;
        bool m_PendingReload = false;
    };
}

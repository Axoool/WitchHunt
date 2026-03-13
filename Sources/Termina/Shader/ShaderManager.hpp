#pragma once

#include <Termina/Core/System.hpp>
#include <Termina/Platform/DLL.hpp>

#include "ShaderServer.hpp"

#include <string>

namespace Termina {
    class ShaderManager : public ISystem
    {
    public:
        ShaderManager();
    	~ShaderManager();

        void PostRender(float dt) override;

        UpdateFlags GetUpdateFlags() const override { return UpdateFlags::RenderUpdateDuringEditor; }
    	std::string GetName() const override { return "Shader Manager"; }
        int GetPriority() const override { return 100; }

        ShaderServer& GetShaderServer() { return m_ShaderServer; }
        DLL& GetLibrary() { return m_Library; }

        void ShowDebugWindow(bool* open = nullptr) { m_ShaderServer.ShowDebugWindow(open); }
    private:
        DLL m_Library;
        ShaderServer m_ShaderServer;
    };
}

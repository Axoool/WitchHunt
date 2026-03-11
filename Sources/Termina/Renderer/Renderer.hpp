#pragma once

#include <Termina/Core/System.hpp>
#include <Termina/Core/Window.hpp>
#include <Termina/RHI/Device.hpp>

namespace Termina {
    using RenderCallback = std::function<void(RendererDevice*, RendererSurface*, float)>;

    class RendererSystem : public ISystem
    {
    public:
        RendererSystem(Window* window);
        ~RendererSystem();

        void Render(float deltaTime) override;

        void RegisterRenderCallback(const RenderCallback& callback) { m_RenderCallbacks.push_back(callback); }

        UpdateFlags GetUpdateFlags() const override {
            return UpdateFlags::RenderUpdateDuringEditor | UpdateFlags::UpdateDuringEditor;
        }
        std::string GetName() const override { return "Renderer System"; }
        int GetPriority() const override { return 0; }

        RendererDevice* GetDevice() const { return m_Device; }
        RendererSurface* GetSurface() const { return m_Surface; }
    private:
        Window* m_Window;
	    RendererDevice* m_Device = nullptr;
	    RendererSurface* m_Surface = nullptr;

        int m_CurrentWidth = 0;
        int m_CurrentHeight = 0;

        std::vector<RenderCallback> m_RenderCallbacks;
    };
}

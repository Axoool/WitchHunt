#include "Renderer.hpp"
#include "RHI/RenderContext.hpp"

namespace Termina {
    RendererSystem::RendererSystem(Window* window)
        : m_Window(window)
    {
        m_Device = RendererDevice::Create();
        m_Surface = m_Device->CreateSurface(window);

        m_CurrentWidth = window->GetWidth();
        m_CurrentHeight = window->GetHeight();
    }

    RendererSystem::~RendererSystem()
    {
        m_Device->WaitIdle();

        delete m_Surface;
        delete m_Device;
    }

    void RendererSystem::Render(float deltaTime)
    {
        if (m_Window->GetWidth() != m_CurrentWidth || m_Window->GetHeight() != m_CurrentHeight) {
            m_Surface->Resize(m_Window->GetWidth(), m_Window->GetHeight());
            m_CurrentWidth = m_Window->GetWidth();
            m_CurrentHeight = m_Window->GetHeight();
        }

        uint32 frameIndex = m_Surface->GetFrameIndex();

        RenderContext* context = m_Surface->BeginFrame();
        for (const auto& callback : m_RenderCallbacks) {
            callback(m_Device, m_Surface, deltaTime);
        }
        m_Surface->EndFrame();
    }
}

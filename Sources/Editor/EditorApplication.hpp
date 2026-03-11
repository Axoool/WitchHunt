#pragma once

#include <Termina/Core/Application.hpp>

#include "EditorContext.hpp"
#include "Panel.hpp"

#include <memory>
#include <vector>

class EditorApplication : public Termina::Application
{
public:
    EditorApplication();
    ~EditorApplication() = default;

    void OnUpdate(float dt) override;

    template<typename T>
    void RegisterPanel()
    {
        m_Panels.push_back(std::make_unique<T>(m_Context));
    }

private:
    void RenderDockspace();

    EditorContext m_Context;
    std::vector<std::unique_ptr<Panel>> m_Panels;
};

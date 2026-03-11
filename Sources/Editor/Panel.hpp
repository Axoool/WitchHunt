#pragma once

#include "EditorContext.hpp"
#include <string>

class Panel
{
public:
    Panel(std::string name, EditorContext& context)
        : m_Name(std::move(name)), m_Context(context) {}

    virtual ~Panel() = default;

    virtual void OnImGuiRender() = 0;

    void SetOpen(bool open) { m_Open = open; }
    bool IsOpen() const { return m_Open; }
    const std::string& GetName() const { return m_Name; }

protected:
    std::string m_Name;
    EditorContext& m_Context;
    bool m_Open = true;
};

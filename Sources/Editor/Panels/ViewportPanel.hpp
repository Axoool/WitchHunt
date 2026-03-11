#pragma once

#include "Editor/Panel.hpp"

class ViewportPanel : public Panel
{
public:
    ViewportPanel(EditorContext& context)
        : Panel("Viewport", context) {}

    void OnImGuiRender() override;
};

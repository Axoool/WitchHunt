#pragma once

#include "Editor/Panel.hpp"

class ContentViewerPanel : public Panel
{
public:
    ContentViewerPanel(EditorContext& context)
        : Panel("Content Viewer", context) {}

    void OnImGuiRender() override;
};

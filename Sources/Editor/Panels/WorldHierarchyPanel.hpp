#pragma once

#include "Editor/Panel.hpp"

class WorldHierarchyPanel : public Panel
{
public:
    WorldHierarchyPanel(EditorContext& context)
        : Panel("World Hierarchy", context) {}

    void OnImGuiRender() override;
};

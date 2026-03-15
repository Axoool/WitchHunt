#pragma once

#include <Termina/Core/IInspectable.hpp>
#include <Termina/Core/Project.hpp>

class ContentViewerPanel;

struct EditorContext
{
    Termina::IInspectable* ItemToInspect = nullptr;
    ContentViewerPanel* ContentViewer = nullptr;
    Termina::Project CurrentProject;
    float ViewportWidth = 0.0f;
    float ViewportHeight = 0.0f;
};

#pragma once

#include <string>

namespace Termina {
    class FileDialog
    {
    public:
        static std::string OpenFile();
        static std::string SaveFile();
    };
}

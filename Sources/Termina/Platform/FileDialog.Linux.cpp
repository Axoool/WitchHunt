#include "FileDialog.hpp"

#if defined(TRMN_LINUX)
#include <cstdio>
#include <memory>
#include <array>
#include <string>
#include <algorithm>

namespace Termina {
    static std::string RunCommandCapture(const char* cmd)
    {
        std::array<char, 256> buffer;
        std::string result;

        // Open the command for reading
        FILE* pipe = popen(cmd, "r");
        if (!pipe) return {};

        // Read until EOF
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
            result += buffer.data();
        }

        int rc = pclose(pipe);
        (void)rc; // we don't currently use the return code

        // Trim trailing newlines and carriage returns
        while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
            result.pop_back();

        return result;
    }

    std::string FileDialog::OpenFile()
    {
        // Use zenity to show a file open dialog. If zenity is not installed or fails,
        // an empty string is returned.
        // --file-selection prints the selected path on stdout.
        const char* cmd = "zenity --file-selection 2>/dev/null";
        return RunCommandCapture(cmd);
    }

    std::string FileDialog::SaveFile()
    {
        // Use zenity save dialog. --confirm-overwrite will prompt if the file exists.
        const char* cmd = "zenity --file-selection --save --confirm-overwrite 2>/dev/null";
        return RunCommandCapture(cmd);
    }
} // namespace Termina

#endif

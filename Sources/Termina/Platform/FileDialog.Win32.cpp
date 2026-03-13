#include "FileDialog.hpp"

#if defined(TRMN_WINDOWS)

#include <windows.h>
#include <commdlg.h>
#include <string>
#include <vector>
#include <algorithm>

namespace Termina {
    // Convert a wide (UTF-16) string to a UTF-8 std::string.
    static std::string WideToUTF8(const std::wstring& w)
    {
        if (w.empty()) return {};
        int required = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (required <= 0) return {};
        std::string out;
        out.resize(required - 1); // exclude null terminator
        WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, &out[0], required, nullptr, nullptr);
        return out;
    }

    std::string FileDialog::OpenFile()
    {
        // Use a large buffer to support long paths.
        std::vector<wchar_t> buffer(32768);
        OPENFILENAMEW ofn{};
        ofn.lStructSize = sizeof(OPENFILENAMEW);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = buffer.data();
        ofn.lpstrFile[0] = L'\0';
        ofn.nMaxFile = static_cast<DWORD>(buffer.size());

        // Filter is double-null terminated wide string. Provide a basic filter.
        ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.txt\0\0";
        ofn.nFilterIndex = 1;

        // Important: prevent the dialog from changing the current working directory.
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;

        if (GetOpenFileNameW(&ofn))
        {
            std::string path = WideToUTF8(ofn.lpstrFile);
            std::replace(path.begin(), path.end(), '\\', '/');
            return path;
        }

        return {};
    }

    std::string FileDialog::SaveFile()
    {
        std::vector<wchar_t> buffer(32768);
        OPENFILENAMEW ofn{};
        ofn.lStructSize = sizeof(OPENFILENAMEW);
        ofn.hwndOwner = nullptr;
        ofn.lpstrFile = buffer.data();
        ofn.lpstrFile[0] = L'\0';
        ofn.nMaxFile = static_cast<DWORD>(buffer.size());

        ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.txt\0\0";
        ofn.nFilterIndex = 1;

        // Prevent changing the working directory; prompt if overwriting.
        ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_EXPLORER;

        if (GetSaveFileNameW(&ofn))
        {
            std::string path = WideToUTF8(ofn.lpstrFile);
            std::replace(path.begin(), path.end(), '\\', '/');
            return path;
        }

        return {};
    }
} // namespace Termina

#endif

#include "pch.h"
#include "ProcessRunner.h"
#include "ProcessRunner.g.cpp"

#include <winrt/Windows.Storage.h>

#include <Windows.h>

#include <iomanip>
#include <sstream>

using namespace winrt;
using namespace Windows::Storage;

namespace winrt::TestComponent::implementation
{
    uint64_t ProcessRunner::CallFileHandlingApp(hstring const& inputFilePath, hstring const& outputFileName)
    {
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        std::wstringstream argsBuilder;
        argsBuilder << std::quoted(L"FileHandlingApp\\FileHandlingApp.exe") << L' ' << std::quoted(std::wstring_view(inputFilePath)) << L' '
            << std::quoted(std::wstring_view(ApplicationData::Current().LocalFolder().Path())) << L' ' << std::quoted(std::wstring_view(outputFileName));

        if (!CreateProcess(nullptr, argsBuilder.str().data(), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi)) {
            return GetLastError();
        }

        WaitForSingleObject(pi.hProcess, INFINITE);

        unsigned short host, guest;
        IsWow64Process2(pi.hProcess, &guest, &host);
        // Make sure it really runs in WoW64
        assert(host == IMAGE_FILE_MACHINE_AMD64 || host == IMAGE_FILE_MACHINE_ARM64);
        assert(guest == IMAGE_FILE_MACHINE_I386);

        unsigned long code;
        if (!GetExitCodeProcess(pi.hProcess, &code)) {
            code = GetLastError();
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return code;
    }
}

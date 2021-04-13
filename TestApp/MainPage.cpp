#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Popups.h>
#include <winrt/Windows.UI.Xaml.h>

#include <iomanip>
#include <sstream>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Storage;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;

using namespace winrt::TestApp::implementation;

MainPage::MainPage()
{
    InitializeComponent();
}

hstring MainPage::FilePath()
{
    return unbox_value_or<hstring>(GetValue(_filePathProperty), L"");
}

void MainPage::FilePath(const hstring& value)
{
    SetValue(_filePathProperty, box_value(value));
    copyButton().IsEnabled(!value.empty());
}

IAsyncAction MainPage::OnBrowseButtonClicked(IInspectable const&, RoutedEventArgs const&)
{
    using namespace Windows::Storage;
    Pickers::FileOpenPicker picker;
    picker.FileTypeFilter().Append(L"*");
    auto pickedFile = co_await picker.PickSingleFileAsync();
    if (pickedFile) {
        auto tempFile = co_await pickedFile.CopyAsync(ApplicationData::Current().TemporaryFolder(), pickedFile.Name(), NameCollisionOption::ReplaceExisting);
        if (tempFile) {
            FilePath(tempFile.Path());
        }
    }
}

IAsyncAction MainPage::OnCopyButtonClicked(IInspectable const&, RoutedEventArgs const&)
{
    _outputFileName = outputNameTextBox().Text();

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::wstringstream argsBuilder;
    argsBuilder << std::quoted(L"FileHandlingApp\\FileHandlingApp.exe") << L' ' << std::quoted(std::wstring_view(FilePath())) << L' '
        << std::quoted(std::wstring_view(ApplicationData::Current().LocalFolder().Path())) << L' ' << std::quoted(std::wstring_view(_outputFileName));
    auto args = argsBuilder.str();

    if (!CreateProcess(nullptr, args.data(), nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi)) {
        openButton().IsEnabled(false);
        co_await MessageDialog(L"Failed to start process: " + std::to_wstring(GetLastError())).ShowAsync();
        co_return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    unsigned short host, guest;
    IsWow64Process2(pi.hProcess, &guest, &host);
    // Make sure it really runs in WoW64
    assert(host == IMAGE_FILE_MACHINE_AMD64 || host == IMAGE_FILE_MACHINE_ARM64);
    assert(guest == IMAGE_FILE_MACHINE_I386);

    if (unsigned long code; !GetExitCodeProcess(pi.hProcess, &code) || code != 0) {
        openButton().IsEnabled(false);
        co_await MessageDialog(L"Process exited with error: " + std::to_wstring(code)).ShowAsync();
    }
    else {
        openButton().IsEnabled(true);
        auto tempFile = co_await StorageFile::GetFileFromPathAsync(FilePath());
        co_await tempFile.DeleteAsync();
        FilePath(hstring());
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

IAsyncAction MainPage::OnOpenButtonClicked(IInspectable const&, RoutedEventArgs const&)
{
    auto outFile = co_await ApplicationData::Current().LocalFolder().GetFileAsync(_outputFileName);
    contentTextBlock().Text(co_await FileIO::ReadTextAsync(outFile));
}

DependencyProperty MainPage::_filePathProperty = DependencyProperty::Register(
    L"FilePath",
    winrt::xaml_typename<hstring>(),
    winrt::xaml_typename<TestApp::MainPage>(),
    Windows::UI::Xaml::PropertyMetadata{ nullptr }
);

DependencyProperty MainPage::FilePathProperty()
{
    return _filePathProperty;
}

void MainPage::FilePathProperty(DependencyProperty value)
{
    _filePathProperty = value;
}

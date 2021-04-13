#pragma once

#include "MainPage.g.h"

namespace winrt::TestApp::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        hstring FilePath();
        void FilePath(const hstring& value);

        static Windows::UI::Xaml::DependencyProperty FilePathProperty();
        static void FilePathProperty(Windows::UI::Xaml::DependencyProperty value);

        winrt::Windows::Foundation::IAsyncAction OnBrowseButtonClicked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        winrt::Windows::Foundation::IAsyncAction OnCopyButtonClicked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);
        winrt::Windows::Foundation::IAsyncAction OnOpenButtonClicked(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const& e);

    private:
        static Windows::UI::Xaml::DependencyProperty _filePathProperty;
        hstring _outputFileName;
    };
}

namespace winrt::TestApp::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}

using System;
using TestLibrary;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace TestApp
{
    public sealed partial class MainPage : Page
    {
        private readonly MainViewModel _viewModel;

        public MainPage()
        {
            InitializeComponent();
            _viewModel = new MainViewModel();
        }

        private bool StringHasValue(string s) => !string.IsNullOrEmpty(s);

        private async void OnCopyButtonClicked(object sender, RoutedEventArgs e)
        {
            var result = await _viewModel.CopyFileToLocalFolder(outputNameTextBox.Text);
            if (!result.DidCreateSuccessfully)
            {
                await new MessageDialog($"Failed to start process: {result.ExitCode}").ShowAsync();
            }
            else if (result.ExitCode != 0)
            {
                await new MessageDialog($"Process exited with error: {result.ExitCode}").ShowAsync();
            }
            else
            {
                openButton.IsEnabled = true;
            }
        }

        private async void OnOpenButtonClicked(object sender, RoutedEventArgs e)
        {
            contentTextBlock.Text = await _viewModel.OpenOutputFile();
        }
    }
}

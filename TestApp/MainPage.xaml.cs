using System;

using TestComponent;

using Windows.Storage;
using Windows.Storage.Pickers;
using Windows.UI.Popups;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace TestApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            InitializeComponent();
        }

        string FilePath
        {
            get => (string)GetValue(FilePathProperty);
            set
            {
                SetValue(FilePathProperty, value);
                copyButton.IsEnabled = !string.IsNullOrEmpty(value);
            }
        }

        private static readonly DependencyProperty FilePathProperty = DependencyProperty.Register(
            nameof(FilePath), typeof(string), typeof(MainPage), null);

        private string _outputFileName;

        private async void OnBrowseButtonClicked(object sender, RoutedEventArgs e)
        {
            var picker = new FileOpenPicker();
            picker.FileTypeFilter.Add("*");
            var pickedFile = await picker.PickSingleFileAsync();
            if (pickedFile != null)
            {
                var tempFile = await pickedFile.CopyAsync(ApplicationData.Current.TemporaryFolder, pickedFile.Name, NameCollisionOption.ReplaceExisting);
                if (tempFile != null)
                {
                    FilePath = tempFile.Path;
                }
            }
        }

        private async void OnCopyButtonClicked(object sender, RoutedEventArgs e)
        {
            _outputFileName = outputNameTextBox.Text;

            ulong exitCode = ProcessRunner.CallFileHandlingApp(FilePath, _outputFileName);
            if (exitCode != 0)
            {
                openButton.IsEnabled = false;
                await new MessageDialog($"Process exited with error: {exitCode}").ShowAsync();
            }
            else
            {
                openButton.IsEnabled = true;
                var tempFile = await StorageFile.GetFileFromPathAsync(FilePath);
                await tempFile.DeleteAsync();
                FilePath = "";
            }
        }

        private async void OnOpenButtonClicked(object sender, RoutedEventArgs e)
        {
            var outFile = await ApplicationData.Current.LocalFolder.GetFileAsync(_outputFileName);
            if (outFile != null)
            {
                contentTextBlock.Text = await FileIO.ReadTextAsync(outFile);
            }
        }
    }
}

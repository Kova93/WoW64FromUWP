using System;
using System.Threading.Tasks;
using TestComponent;
using Windows.Storage;
using Windows.Storage.Pickers;

namespace TestLibrary
{
    public class MainViewModel : BindableBase
    {
        private string _outputFileName;

        private string _filePath;
        public string FilePath
        {
            get { return _filePath; }
            private set
            {
                SetProperty(ref _filePath, value);
            }
        }

        public async void BrowseInputFileAndCopyToTempFolder()
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

        public async Task<ProcessResult> CopyFileToLocalFolder(string outputFileName)
        {
            var result = ProcessRunner.CallFileHandlingApp(FilePath, outputFileName);
            if (result.DidCreateSuccessfully && result.ExitCode == 0)
            {
                var tempFile = await StorageFile.GetFileFromPathAsync(FilePath);
                await tempFile.DeleteAsync();
                FilePath = "";
                _outputFileName = outputFileName;
            }
            return result;
        }

        public async Task<string> OpenOutputFile()
        {
            var outFile = await ApplicationData.Current.LocalFolder.GetFileAsync(_outputFileName);
            if (outFile != null)
            {
                return await FileIO.ReadTextAsync(outFile);
            }
            else
            {
                return "";
            }
        }
    }
}

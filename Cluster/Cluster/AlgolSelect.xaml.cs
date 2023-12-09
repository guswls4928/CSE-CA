using Algorithm;
#if ANDROID
using Android.Media;
#endif

#if IOS
using Foundation;
using ImageIO;
#endif

namespace Cluster;

public partial class AlgolSelect : ContentPage
{
    private TaskCompletionSource<Tuple<List<Algorithm.ImageNode>, AlgorithmInterface>> _tcs
    = new TaskCompletionSource<Tuple<List<Algorithm.ImageNode>, AlgorithmInterface>>();
    private List<Algorithm.ImageNode> imgList = new List<Algorithm.ImageNode>();
    public AlgolSelect()
    {
        InitializeComponent();
        algorithmPicker.ItemsSource = new List<string> { "Default" }; // Populate with actual algorithm names
    }

    private async void OnLoadButtonClicked(object sender, EventArgs e)
    {
        
        try
        {
            var customFileType = new FilePickerFileType(new Dictionary<DevicePlatform, IEnumerable<string>>
                {
                    { DevicePlatform.iOS, new[] { "public.image" } }, // or use UTType.Image for iOS
                    { DevicePlatform.Android, new[] { "image/*" } },
                });

            var pickResult = await FilePicker.PickMultipleAsync(new PickOptions
            {
                PickerTitle = "Please select images",
                FileTypes = customFileType,
            });

            if (pickResult != null)
            {
                foreach (var image in pickResult)
                {
                    AddIfValidExifGPS(image.FullPath, imgList);
                }

                imageCountLabel.Text = $"{imgList.Count} images have valid EXIF GPS coordinates";
                goButton.IsEnabled = imgList.Count > 0;
            }
        }
        catch (Exception ex)
        {
            // Handle exceptions, such as no permissions
            await DisplayAlert("Error", "An error occurred while picking images.", "OK");
        }
    }

    private bool AddIfValidExifGPS(string imagePath, List<Algorithm.ImageNode> imgList)
    {
        double? latitude = null;
        double? longitude = null;

#if ANDROID
    // Android specific code
    var exif = new ExifInterface(imagePath); // imagePath should be the full path to the image
    float[] latLong = new float[2];
    if (exif.GetLatLong(latLong))
    {
        latitude = latLong[0];
        longitude = latLong[1];
    }
#elif IOS
        // iOS specific code
        using var url = new NSUrl(imagePath); // imagePath should be the full path to the image
        using var imageSource = CGImageSource.FromUrl(url);
        var properties = imageSource.CopyProperties(new CGImageOptions(), 0);
        if (properties.TryGetValue(CGImageProperties.GPSDictionary, out var gpsDictValue) && gpsDictValue is NSDictionary gpsDict)
        {
            if (gpsDict.TryGetValue(CGImageProperties.GPSLatitude, out var latValue) && latValue is NSNumber latNumber &&
                gpsDict.TryGetValue(CGImageProperties.GPSLongitude, out var lonValue) && lonValue is NSNumber lonNumber)
            {
                latitude = latNumber.DoubleValue;
                longitude = lonNumber.DoubleValue;
            }
        }
#else
    throw new NotImplementedException("EXIF processing not implemented for this platform.");
#endif
        if (latitude.HasValue && longitude.HasValue)
        {
            Algorithm.Point p = new Algorithm.Point();
            Algorithm.ImageNode img = new Algorithm.ImageNode();
            img.FileName = imagePath;
            p.X = longitude.Value;
            p.Y = latitude.Value;
            img.Position = p;

            imgList.Add(img);
            return true;
        }
        else
        {
            return false;
        }
    }


    private async void OnGoButtonClicked(object sender, EventArgs e)
    {
        AlgorithmInterface algol = null;
        switch (algorithmPicker.SelectedIndex)
        {
            case 0:
                algol = new AlgorithmInterface(new Extern.Default());
                break;
            default:
                _tcs.SetResult(null);
                await Navigation.PopModalAsync();
                return;
        }

        _tcs.SetResult(new Tuple<List<Algorithm.ImageNode>, AlgorithmInterface>(imgList, algol));
        await Navigation.PopModalAsync();
    }

    internal Task<Tuple<List<Algorithm.ImageNode>, AlgorithmInterface>> WaitForResultAsync()
    {
        return _tcs.Task;
    }

}
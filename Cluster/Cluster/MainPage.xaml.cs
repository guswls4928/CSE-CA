using Algorithm;
using Microsoft.Maui.Controls.Maps;
using Microsoft.Maui.Maps;
using System.Diagnostics.Metrics;
using System.Timers;
using Timer = System.Timers.Timer;
namespace Cluster
{
    public partial class MainPage : ContentPage
    {

        private Algorithm.AlgorithmInterface activeAlgorithm;


        private System.Threading.Timer _pollingTimer;
        private MapSpan _lastMapSpan;
        private MapSpan _lastValidSpan;
        private DateTime _lastUpdateTime;
        private bool _interactionFlag = false;
        private readonly TimeSpan _interactionThreshold = TimeSpan.FromSeconds(0.1);

        public MainPage()
        {
            InitializeComponent();

            _lastMapSpan = DisplayMap.VisibleRegion;
            _lastUpdateTime = DateTime.Now;
            SetupPollingTimer();

            

            DisplayMap.MoveToRegion(new MapSpan(new Location(37.541, 126.986), 35, 42));
        }

        private void AddPins(Algorithm.Benchmark benchmark)
        {
            foreach(Algorithm.ImageCluster item in benchmark.Clusters)
            {
                ImageCluster cluster = new ImageCluster();
                cluster.MapCoords = new MapPos(item.Position.X, item.Position.Y);
                cluster.ImageSource = item.Representative.FileName;
                cluster.cnt.Text = $"{item.Count}";

                OverlayCanvas.Children.Add(cluster);
            }

            foreach (var item in OverlayCanvas.Children)
            {
                if (item is ImageCluster cluster)
                {
                    cluster.Position = ScreenPos.fromMapPos(cluster.MapCoords, DisplayMap.VisibleRegion,
                        DisplayMap.Width, DisplayMap.Height);
                }
            }
        }


        private void SetupPollingTimer()
        {
            _pollingTimer = new System.Threading.Timer(CheckMapState, null, 0, 5);
        }

        private void CheckMapState(object state)
        {
#pragma warning disable CS0618 // Type or member is obsolete
            Device.BeginInvokeOnMainThread(() =>
            {
                var currentSpan = DisplayMap.VisibleRegion;
                if (currentSpan != null && !Equals(currentSpan, _lastMapSpan))
                {

                    if (isRegionInvalid(currentSpan)) { 
                        DisplayMap.MoveToRegion(new MapSpan(new Location(37.541, 126.986), 35, 42));
                        mapBg.BackgroundColor = Colors.Red;
                        mapBg.Opacity = 0.6;
                        lblState.Text = "INVALID_REGION";
                        lblState.TextColor = Colors.Red;
                    }
                    else
                    {
                        mapBg.BackgroundColor = Colors.Blue;
                        mapBg.Opacity = 0.3;
                        _lastValidSpan = currentSpan;

                        lblDisplayDimensions.Text = $"DP:= (W={DisplayMap.VisibleRegion.LongitudeDegrees:F4}°  " +
                        $"H={DisplayMap.VisibleRegion.LatitudeDegrees:F4}°)";
                        lblCenterPosition.Text = $"CEN:= {GetViewportCenter()}";
                        lblState.Text = "SHIFTING";
                        lblState.TextColor = Colors.Blue;
                        invokeStat.TextColor = Colors.Blue;
                        foreach (var item in OverlayCanvas.Children)
                        {
                            if (item is ImageCluster cluster)
                            {
                                cluster.Position = ScreenPos.fromMapPos(cluster.MapCoords, DisplayMap.VisibleRegion,
                                    DisplayMap.Width, DisplayMap.Height);
                            }
                        }

                        _lastMapSpan = currentSpan;
                        _lastUpdateTime = DateTime.Now;
                        _interactionFlag = true;
                    }
                }
                else if ((DateTime.Now - _lastUpdateTime) > _interactionThreshold && _interactionFlag)
                {
                    mapBg.BackgroundColor = Colors.Black;
                    mapBg.Opacity = 0.3;
                    _interactionFlag = false;
                    lblState.Text = "INVOKING";
                    lblState.TextColor = Colors.Purple;
                    OnMapInteractionEnded();
                }
            });
#pragma warning restore CS0618 // Type or member is obsolete
        }

        private bool isRegionInvalid(MapSpan region)
        {
            if (region == null) return true;
            return region.LatitudeDegrees == 90 || region.LongitudeDegrees == 180;
        }

        private void OnMapInteractionEnded()
        {
            // Implement what should happen when map interaction ends
            // This method will be called after 0.2 seconds of inactivity following an interaction
            if (activeAlgorithm != null)
            {
                var iterBench = activeAlgorithm.Iterate(DisplayMap.VisibleRegion);
                SetInvokeStatus(iterBench, false);
                OverlayCanvas.Children.Clear();
                AddPins(iterBench);
            }

            mapBg.BackgroundColor = Colors.Transparent;
            mapBg.Opacity = 0.3;
            lblState.Text = "IDLE";
            lblState.TextColor = Colors.Green;
        }

        private (double Latitude, double Longitude) GetViewportCenter()
        {
            var center = DisplayMap.VisibleRegion?.Center;
            if (center == null) return (0, 0);

            int decimalPlaces = 4; // Number of decimal places you want
            double roundedLatitude = Math.Round(center.Latitude, decimalPlaces);
            double roundedLongitude = Math.Round(center.Longitude, decimalPlaces);

            return (roundedLatitude, roundedLongitude);
        }

        private async void Button_Pressed(object sender, EventArgs e)
        {
            bool answer = await DisplayAlert("Confirm", "Proceeding will dispose previous algorithm. Proceed?", "Yes", "No");
            if (answer)
            {
                var popupPage = new AlgolSelect();
                await Navigation.PushModalAsync(popupPage);

                var result = await popupPage.WaitForResultAsync();
                if (result != null)
                {
                    var returnedList = result.Item1;
                    var returnedAlgol = result.Item2;
                    
                    if (returnedAlgol != null && returnedList != null && returnedList.Count > 0)
                    {
                        if (activeAlgorithm != null) { activeAlgorithm.Dispose(); }
                        activeAlgorithm = returnedAlgol;

                        OverlayCanvas.Children.Clear();
                        var initret = returnedAlgol.Initialize(returnedList, (uint)DisplayMap.Width, (uint)DisplayMap.Height);
                        var inititer = returnedAlgol.Iterate(DisplayMap.VisibleRegion);

                        var t = Extern.Kmeans.testFunc(2.2, 3.3);
                        int tst = Extern.Kmeans.noTest(returnedAlgol.clusterAlgorithm._algorithmInstance);
                        SetInvokeStatus(initret, true);
                        AddPins(inititer);
                    }
                }
            }
        }

        private void SetInvokeStatus(Algorithm.Benchmark benchmark, bool isInit)
        {
            invokeStat.Text = $"CALLTYPE: {(isInit? "INIT": "ITER")}  Internal Elapsed: {benchmark.Elapsed}ms\nComp: {benchmark.CompareCount}t  MaxMem: {benchmark.MaxNodes}nodes\nDeviation: {benchmark.Deviation}";
            invokeStat.TextColor = Colors.Red;
        }

    }

}

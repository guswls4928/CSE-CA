using Microsoft.Maui.Controls.Maps;
using Microsoft.Maui.Maps;
using System.Diagnostics.Metrics;
using System.Timers;
using Timer = System.Timers.Timer;
namespace Cluster
{
    public partial class MainPage : ContentPage
    {

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
                    }
                    else
                    {
                        mapBg.BackgroundColor = Colors.Blue;
                        mapBg.Opacity = 0.3;
                        _lastValidSpan = currentSpan;
                        lblZoomLevel.Text = $"WidthLong:{DisplayMap.VisibleRegion.LongitudeDegrees}\n" +
                        $"HeightLat:{DisplayMap.VisibleRegion.LatitudeDegrees}";

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
            mapBg.BackgroundColor = Colors.Transparent;
            mapBg.Opacity = 0.3;
        }


        private void OnMapUpdateTimerElapsed(object sender, ElapsedEventArgs e)
        {
            UpdateMapInfo();
        }

        private void UpdateMapInfo()
        {
#pragma warning disable CS0612 // Type or member is obsolete
            Device.BeginInvokeOnMainThread(() =>
            {
                if (DisplayMap.VisibleRegion != null)
                {
                    lblZoomLevel.Text = $"Zoom Level: {GetZoomLevel()}";
                    lblCenterPosition.Text = $"Center Position: {GetViewportCenter()}";
                }
            });
#pragma warning restore CS0612 // Type or member is obsolete
        }

        private double GetZoomLevel()
        {
            var region = DisplayMap.VisibleRegion;
            if (region == null) return 0;

            // Example calculation for zoom level
            var zoomLevel = Math.Log(360 / region.LatitudeDegrees) / Math.Log(2);
            return zoomLevel;
        }

        private (double Latitude, double Longitude) GetViewportCenter()
        {
            var center = DisplayMap.VisibleRegion?.Center;
            if (center == null) return (0, 0);
            return (center.Latitude, center.Longitude);
        }

        private void Button_Pressed(object sender, EventArgs e)
        {
            var l = new List<Algorithm.ImageNode>();
            var p = new Algorithm.Point(); p.Y = 37.541; p.X = 126.986;
            var img = new Algorithm.ImageNode();
            img.FileName = "dotnet_bot.png";
            img.Position = p;

            var p2 = new Algorithm.Point(); p2.Y = 40; p2.X = 130;
            var img2 = new Algorithm.ImageNode();
            img2.FileName = "dotnet_bot.png";
            img2.Position = p2;

            l.Add(img);
            l.Add(img2);

            var a = new Algorithm.AlgorithmInterface();

            var b = a.Initialize(l, (uint)DisplayMap.Width, (uint)DisplayMap.Height);

            var c = a.Iterate(DisplayMap.VisibleRegion);

            AddPins(c);
        }
    }

}

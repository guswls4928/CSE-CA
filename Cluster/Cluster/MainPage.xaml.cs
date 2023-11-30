using Microsoft.Maui.Controls.Maps;
using Microsoft.Maui.Maps;
namespace Cluster
{
    public partial class MainPage : ContentPage
    {
        int count = 0;

        public MainPage()
        {
            InitializeComponent();
        }

        public void AddMarker(double latitude, double longitude)
        {
            var pin = new Pin
            {
                Label = $"{count++}",
                Type = PinType.Place,
                Location = new Location(latitude, longitude)
            };
            MyMap.Pins.Add(pin);
        }

        public (double Latitude, double Longitude) GetViewportCenter()
        {
            var center = MyMap.VisibleRegion?.Center;
            if (center == null) return (0, 0);
            return (center.Latitude, center.Longitude);
        }

        private void onv1(object sender, EventArgs e)
        {
            AddMarker(GetViewportCenter().Latitude, GetViewportCenter().Longitude);
        }

        private void OnMapTapped(object sender, EventArgs e)
        {
            if (sender is Microsoft.Maui.Controls.Maps.Map)
            {
                // Handle map tap event
            }
        }


    }

}

using Microsoft.Maui.Maps;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Cluster
{
    public class ScreenPos
    {
        public double x { get; set; }
        public double y { get; set; }

        public ScreenPos(double x, double y)
        {
            this.x = x;
            this.y = y;
        }

        public static ScreenPos fromMapPos(MapPos mapPos, MapSpan screenRegion, double visualWidth, double visualHeight)
        {
            double xFromCenter = visualWidth * (mapPos.longtitude - screenRegion.Center.Longitude) / screenRegion.LongitudeDegrees;
            double yFromCenter = -visualHeight * (mapPos.latitude - screenRegion.Center.Latitude) / screenRegion.LatitudeDegrees;
            double centerX = visualWidth / 2;
            double centerY = visualHeight / 2;

            return new ScreenPos(centerX + xFromCenter - 40, centerY + yFromCenter - 75);
        }
    }

    public class MapPos
    {
        public double latitude { get; set; }
        public double longtitude { get; set; }

        public MapPos(double lon, double lat)
        {
            latitude = lat;
            longtitude = lon;
        }
    }
}

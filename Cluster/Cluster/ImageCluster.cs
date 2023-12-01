using Microsoft.Maui.Controls;
using Microsoft.Maui.Controls.Shapes;
using Microsoft.Maui.Graphics;
using System.Runtime.CompilerServices;

namespace Cluster;

public class ImageCluster : ContentView
{

    #region Image Source Property
    public static readonly BindableProperty ImageSourceProperty =
        BindableProperty.Create(
            nameof(ImageSource),
            typeof(ImageSource),
            typeof(ImageCluster),
            defaultBindingMode: BindingMode.OneWay,
            propertyChanged: OnImageSourcePropertyChanged);

    public ImageSource ImageSource
    {
        get => (ImageSource)GetValue(ImageSourceProperty);
        set => SetValue(ImageSourceProperty, value);
    }

    private static void OnImageSourcePropertyChanged(BindableObject bindable, object oldValue, object newValue)
    {
        var control = (ImageCluster)bindable;
        var imageSource = (ImageSource)newValue;

        // Assuming the Image is the third child, as per the constructor setup
        Image? img = (control.Content as Grid)?.Children[3] as Image;
        if (img != null)
        {
            img.Source = imageSource;
        }
    }
    #endregion

    #region Screen Relative Position Property
    public static readonly BindableProperty PositionProperty = BindableProperty.Create(
    nameof(Position),
    typeof(ScreenPos),
    typeof(ImageCluster),
    new ScreenPos(0, 0),
    propertyChanged: OnScreenPositionChanged);

    public ScreenPos Position
    {
        get => (ScreenPos)GetValue(PositionProperty);
        set => SetValue(PositionProperty, value);
    }

    private static void OnScreenPositionChanged(BindableObject bindable, object oldValue, object newValue)
    {
        var control = (ImageCluster)bindable;
        var position = (ScreenPos)newValue;
        AbsoluteLayout.SetLayoutBounds(control, new Rect(position.x, position.y, control.WidthRequest, control.HeightRequest));
    }
    #endregion

    #region On-map coordinates (latlong) property
    public static readonly BindableProperty MapCoordsProperty = BindableProperty.Create(
    nameof(MapCoords),
    typeof(MapPos),
    typeof(ImageCluster),
    new MapPos(0, 0),
    propertyChanged: OnMapPositionChanged);

    public MapPos MapCoords
    {
        get => (MapPos)GetValue(MapCoordsProperty);
        set => SetValue(MapCoordsProperty, value);
    }

    private static void OnMapPositionChanged(BindableObject bindable, object oldValue, object newValue)
    {
        var control = (ImageCluster)bindable;
        var position = (MapPos)newValue;
        //control.Position = ScreenPos.fromMapPos(newValue);
    }
    #endregion

    public ImageCluster()
    {
        // Rectangle
        var rectangle = new Rectangle
        {
            HeightRequest = 60,
            WidthRequest = 80,
            VerticalOptions = LayoutOptions.Start,
            HorizontalOptions = LayoutOptions.Start,
            Fill = SolidColorBrush.DarkSlateGray,
            Stroke = SolidColorBrush.Transparent,
            StrokeThickness = 0
        };

        // Triangle
        var triangle = new Polygon
        {
            VerticalOptions = LayoutOptions.Start,
            HorizontalOptions = LayoutOptions.Start,
            Points = new PointCollection { new Point(34, 60), new Point(40, 75), new Point(46, 60) },
            Fill = SolidColorBrush.DarkSlateGray,
            Stroke = SolidColorBrush.Transparent,
            StrokeThickness = 0
        };

        // Image
        var image = new Image
        {

            VerticalOptions = LayoutOptions.Start,
            HorizontalOptions = LayoutOptions.Start,
            Source = ImageSource,
            HeightRequest = 54,
            WidthRequest = 72,
            Margin = new Thickness(4, 3, 0, 0),
            Aspect = Aspect.AspectFit
        };

        var imgBg = new Rectangle
        {
            HeightRequest = 54,
            WidthRequest = 72,
            Margin = new Thickness(4, 3, 0, 0),
            VerticalOptions = LayoutOptions.Start,
            HorizontalOptions = LayoutOptions.Start,
            Fill = SolidColorBrush.White,
            Stroke = SolidColorBrush.Transparent,
            StrokeThickness = 0
        };

        var cnt = new Label
        {
            //HeightRequest = 54,
            //WidthRequest = 72,
            Margin = new Thickness(0, 3, 4, 0),
            VerticalOptions = LayoutOptions.Start,
            HorizontalOptions = LayoutOptions.End,
            BackgroundColor = Colors.CadetBlue,
            TextColor = Colors.White,
            Text = "333"
        };

        // Stack the shapes vertically
        Content = new Grid
        {
            Children = { rectangle, triangle, imgBg, image, cnt },
            ColumnSpacing = 0,
            RowSpacing = 0,
            Padding = 0

        };
    }
}

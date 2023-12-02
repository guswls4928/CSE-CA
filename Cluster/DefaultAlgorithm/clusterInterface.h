#include <string>
#include <vector>
#include <stdint.h>

#ifndef IMGCLUSTER_H
#define IMGCLUSTER_H
namespace ImgCluster {
    using std::string;

    class Vector;
    class Point {
    public:
        double x, y;
        Point() :x(0), y(0) {}
        Point(double x, double y) : x(x), y(y) {}

        Point operator +(Vector& mov);

        Point operator -(Vector& mov);

        Vector operator -(Point& sub);

        Vector toVector();
    };

    class Vector {
    public:
        double x, y;
        Vector(double x, double y) : x(x), y(y) {}

        Vector operator +(Vector& add) { return Vector(x + add.x, y + add.y); }
        Vector operator -(Vector& sub) { return Vector(x - sub.x, y - sub.y); }
        double operator *(Vector& dot) { return x * dot.x + y * dot.y; }
    };


    class Rectangle {
    public:
        Rectangle(double x, double y, double w, double h) : x(x), y(y), w(w), h(h) {}
        Rectangle(Point p1, Point p2) {
            w = std::abs(p1.x - p2.x);
            h = std::abs(p1.y - p2.y);
            x = std::min(p1.x, p2.x);
            y = std::min(p1.y, p2.y);
        }
        Point getPoint(double iftop0, double ifleft0) {
            return Point(iftop0 ? x + w : x, ifleft0 ? y + h : y);
        }
        double x, y;
        double w, h;
    };

    // The image
    struct ImageNode {
        string fileName;
        Point pos;
    };

    typedef std::vector<ImageNode> Images;

    /* This class is not an actual datastructure, but closer to a record type that holds together
    the display data of ImageClusters as result of an iteration of algorithm*/
    struct ImageCluster {
        // Representative Image of the cluster
        ImageNode repr;

        // Center position(gps position) of the cluster
        Point pos;

        // Number of images in the cluster
        int count;
    };

    typedef std::vector<ImageCluster> ImageClusters;

    struct Benchmark {
        unsigned long compareCnt;
        // Max(number of image nodes that exist in each from all given timeframes)
        unsigned long maxNodes;
        // Time taken : before return - immediately after call
        int elapsed;
        // Standard deviation error
        double deviation;
        // The resulting new Cluster
        ImageClusters clusters;
    };

    // Implement the Algorithm by inheriting this class
    // You may add any 'cache' data in implementations of this abstract class
    class ClusterAlgorithm {
    public:
        /**
         * @brief
         *
         * Passes the raw initial data to the Algorithm class.
         * Must be run at least once before iterate() is run
         *
         * @param imageList Raw list of images, in form of std::vector<ImgCluster::ImageNode> ( typedef'ed as 'ImgCluster::Images' )
         * @param mapWidth Absolute horizontal pixel count of map
         * @param mapHeight Absolute vertical pixel count of map
         * @param screenWidth Absolute horizontal pixel count of screen
         * @param screenHeight Absolute vertical pixel count of screen
         * @return returns ImageNode::Benchmark, a struct that contains benchmark info and an empty 'ImageCluster::ImageClusters'
         */
        virtual Benchmark init(const Images& imageList, unsigned int screenWidth, unsigned int screenHeight);
        /**
         * @brief
         *
         * Clusters ImageNodes based on map resizing and movement
         *
         * @param screenRegion The region of the screen. Represented in absolute pixels in full map
         * @return returns ImageNode::Benchmark, a struct that contains benchmark info and display info of image clusters(not the actual clusters)
         */
        virtual Benchmark iterate(const Rectangle& screenRegion);
    };
}

#endif
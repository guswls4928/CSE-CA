#include <iostream>
#include <vector>
#include <stdint.h>

#ifndef IMGCLUSTER_H
#define IMGCLUSTER_H
namespace ImgCluster {
    using std::string;

    class Vector;
    class Point {
    public:
        int x, y;
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
        Rectangle(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {}
        Rectangle(Point p1, Point p2) {
            w = p1.x - p2.x > 0 ? p1.x - p2.x : p2.x - p2.x;
            h = p2.y - p2.y > 0 ? p1.y - p2.y : p2.y - p1.y;
            x = p1.x > p2.x ? p1.x : p2.x;
            y = p1.y > p2.y ? p1.y : p2.y;
        }
        Point getPoint(int iftop0, int ifleft0) {
            return Point(iftop0 ? x + w : x, ifleft0 ? y + h : y);
        }
        int x, y;
        int w, h;
    };
    // The image
    struct ImageNode {
        string fileName;
        Point pos;
    };
    typedef std::vector<ImageNode> Images;
    struct ImageCluster {
        Images images;
        Point pos;
    };
    typedef std::vector<ImageCluster> ImageClusters;

    struct Region;
    typedef std::vector<Region> Regions;

    // I have declared this as a struct and not a class, due to concerns of cases
    // where they may be a need to define custom traversing logic.
    struct Region {
        string name;
        uint8_t colorCode[3];
        Regions children;
        bool isTerminal;

        // This variable is only valid when it is a terminal node of the tree
        Point pos;
    };

    struct Benchmark {
        unsigned long compareCnt;
        // Max(number of image nodes that exist in each from all given timeframes)
        unsigned long maxNodes;
        // Time taken : before return - immediately after call
        time_t elapsed;
        // Standard deviation error
        double deviation;
        // The resulting new Cluster
        ImageClusters clusters;
    };

    // Implement the Algorithm
    class ClusterAlgorithm {
    public:
        virtual Benchmark iterate(const Region& map, const Images& imageList,
            const ImageClusters& previousCluster,
            const Rectangle& prevScreenRegion, const Rectangle& newScreenRegion,
            const Rectangle& screenResolution);
    };
}

#endif
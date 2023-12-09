#include "clusterInterface.h"
using namespace ImgCluster;

class SampleAlgorithm {
private:
    Images imgList;
    unsigned int swidth;
    unsigned int sheight;
public:
    Benchmark init(const Images& imageList, unsigned int screenWidth, unsigned int screenHeight) {
        swidth = screenWidth;
        sheight = screenHeight;
        imgList = imageList;
        Benchmark b;
        ImageClusters clusters;
        for (const ImageNode& img : imgList) {
            ImageCluster tmp;
            tmp.count = 1;
            tmp.pos.x = img.pos.x;
            tmp.pos.y = img.pos.y;
            tmp.repr = img;
            clusters.push_back(tmp);
        }
        b.clusters = clusters;
        b.compareCnt = 709;
        b.deviation = 127;
        b.elapsed = 5.888;
        b.maxNodes = 304;
        return b;
    }

    Benchmark iterate(const Rectangle& screenRegion) {
        ImageClusters clusters;
        for (const ImageNode& img : imgList) {
            ImageCluster tmp;
            tmp.count = 1;
            tmp.pos.x = img.pos.x;
            tmp.pos.y = img.pos.y;
            tmp.repr = img;
            clusters.push_back(tmp);
        }

        Benchmark ret;
        ret.compareCnt = 200;
        ret.deviation = 3.3;
        ret.elapsed = 3.65;
        ret.maxNodes = 70;
        ret.clusters = clusters;
        return ret;
    }
};
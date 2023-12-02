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
        return Benchmark{
            //compareCnt
            400,
            //maxNodes
            (unsigned int)imgList.size(),
            //elapsed
            20,
            //deviation
            0,
            //clusters
            {}
        };
    }

    Benchmark iterate(const Rectangle& screenRegion) {
        ImageClusters clusters;
        for (const ImageNode& img : imgList)
            clusters.push_back(ImageCluster{ img, img.pos, 1 });
        return Benchmark{
            //compareCnt
            200,
            //maxNodes
            (unsigned int)imgList.size(),
            //elapsed
            10,
            //deviation
            7.7,
            //clusters
            clusters
        };
    }
};
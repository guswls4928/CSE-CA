#include "clusterInterface.h"
#include <chrono>
#include <cmath>
class DBSCAN{
public:
    DBSCAN() = default;
    DBSCAN(ImgCluster::Images& images, ImgCluster::Rectangle& rect, int minPts, double eps);
    ImgCluster::ImageClusters dbscan(ImgCluster::Rectangle& rect, double eps, int minPts, int& compareCnt);
    void radiusSearch(const ImgCluster::Images& data, int idx, double eps, std::vector<std::pair<int, double>>& matches, int& compareCnt);
    void idxToPos(std::vector<std::vector<int>> idx, ImgCluster::ImageClusters clusters);
    double calculateDeviation(ImgCluster::Benchmark bm);


    virtual int findTargetImgList(const ImgCluster::Rectangle& screenRegion);

    virtual ImgCluster::Benchmark init(const ImgCluster::Images& imageList, unsigned int screenWidth, unsigned int screenHeight);
    virtual ImgCluster::Benchmark iterate(const ImgCluster::Rectangle& screenRegion);
private:
    ImgCluster::Images imageList;
    ImgCluster::Images targetImgList;
    unsigned int screenWidth, screenHeight;
    std::vector<ImgCluster::Images> clusterImgList;
};


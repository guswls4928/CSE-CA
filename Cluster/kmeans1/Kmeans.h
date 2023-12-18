//
// Created by 임준영 on 2023/12/10.
//

#include "clusterInterface.h"
#include <chrono>
#include <map>
#include <cmath>

#ifndef ALGORITHM_PROJECT_KMEANS_H
#define ALGORITHM_PROJECT_KMEANS_H

using namespace ImgCluster;

class Kmeans {
private:
    Images imgList;
    // Images targetImgList;
    // key: img , value: imageNode, cluster number
    std::map<int, std::pair<ImageNode, int> > targetImgMap;
    // key: clusterIdx, value: ImageNode, distance: double
    //std::map<int, std::pair<ImageNode, double> > tmpClusterCenter;
    // selectClusterCenter 에서 초기화
    std::vector<std::pair<double, double>> meanPosition;
    int compareCnt = 0;
    int targetImgSize;
    int clusterSize = 0;
    const Rectangle screenRegion;
    ImageClusters clusterList;

    unsigned int screenWidth;
    unsigned int screenHeight;

    Point leftTopPoint;
    Point rightTopPoint;
    Point leftBotPoint;
    Point rightBotPoint;
    void findTargetImgList(const Rectangle& screenRegion);
    void kmeans();
    void selectClusterCenter(const Rectangle &screenRegion);
    double calculateManhattanDistance(ImageNode img1, ImageNode img2);
    double calculateManhattanDistance(Point img1, Point img2);
    double calculateDeviation();
    ImageNode findNearestImg(double x, double y);
public:
    Benchmark init(const Images& imageList, unsigned int screenWidth, unsigned int screenHeight);
    Benchmark iterate(const Rectangle& screenRegion) ;

};


#endif //ALGORITHM_PROJECT_KMEANS_H

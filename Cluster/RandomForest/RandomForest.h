#ifndef RANDOM_FOREST_H
#define RANDOM_FOREST_H

#include "clusterInterface.h"
#include <random>
#include <unordered_map>
#include <cmath>
#include <ctime>

using namespace std;
using namespace ImgCluster;

class RandomForest{
    private:
        Images imgList;
        unsigned long mapWidth;
        unsigned long mapHeight;
        unsigned int screenWidth;
        unsigned int screenHeight;
        clock_t start, end;

        unsigned int cnt;

        int numTrees = 3;
        int numClusters = 5;

        ImageClusters clusterList;
        Images data;
        Images centers;

        void findTarget(const Rectangle& screenRegion);
        void randomForestClustering();
        void initializeClusters();
        double euclideanDistance(const Point a, const Point b);
        int assignToCluster(const Point point);
        double calDev();
    public:
        Benchmark init(const Images& imageList, unsigned long mapWidth, unsigned long mapHeight, unsigned int screenWidth, unsigned int screenHeight);
        Benchmark iterate(const Rectangle& screenRegion);
};

#endif
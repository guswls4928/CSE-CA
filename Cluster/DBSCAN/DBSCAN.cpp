#include "clusterInterface.h"
#include <chrono>
#include <cmath>
<<<<<<<< Updated upstream:Cluster/DBSCAN/DBSCAN.cpp
using ImgCluster::Rectangle;

class DBSCAN {
========
class DBSCAN : ImgCluster::ClusterAlgorithm {
>>>>>>>> Stashed changes:Cluster/DefaultAlgorithm/DBSCAN.cpp
    // TODO: need to set eps value
public:
    //DBSCAN(ImgCluster::Images& images, ImgCluster::Rectangle& rect, int minPts, double eps);
    ImageClusters dbscan(ImgCluster::Rectangle& rect, double eps, int minPts);
    void radiusSearch(const Images& data, int idx, double eps, std::vector<std::pair<int, double>>& matches);
    void idxToPos(std::vector<std::vector<int>> idx, ImgCluster::ImageClusters clusters);

    virtual void findTargetImgList(const Rectangle& screenRegion);

    virtual Benchmark init(const Images& imageList, unsigned int screenWidth, unsigned int screenHeight);
    virtual Benchmark iterate(const Rectangle& screenRegion);
private:
    ImgCluster::Images imageList;
    ImgCluster::Images targetImgList;
    unsigned int screenWidth, screenHeight;
};

void DBSCAN::idxToPos(std::vector<std::vector<int>> idx, ImgCluster::ImageClusters clusters) {
    for (auto i : idx) { // i: vector<int> representing image index in cluster
        ImgCluster::ImageCluster imgCluster;
        for (int j : i) { // j: image index in cluster
            imgCluster.count++;
            imgCluster.pos.x += imageList[j].pos.x;
            imgCluster.pos.y += imageList[j].pos.y;
        }
        imgCluster.pos.x /= imgCluster.count;
        imgCluster.pos.y /= imgCluster.count;
        imgCluster.repr = imageList[i[0]];
        clusters.push_back(imgCluster);
    }
}

// copy from Kmeans.cpp
void DBSCAN::findTargetImgList(const Rectangle& screenRegion) {
    targetImgList.clear();

<<<<<<<< Updated upstream:Cluster/DBSCAN/DBSCAN.cpp
    // 좌상단
    Point leftTopPoint = screenRegion.getPoint(0, 0);
    // 우상단
    Point rightTopPoint = screenRegion.getPoint(0, 1);
    // 좌하단
    Point leftBotPoint = screenRegion.getPoint(1, 0);
    // 우하단
    Point rightBotPoint = screenRegion.getPoint(1, 1);
========
    /*
        Point getPoint(double iftop0, double ifleft0) {
            return Point(iftop0 ? x + w : x, ifleft0 ? y + h : y);
        }
    */

    // 좌상단
    Point leftTopPoint(screenRegion.x, screenRegion.y);
    // 우상단
    Point rightTopPoint(screenRegion.x, screenRegion.y + screenRegion.h);
    // 좌하단
    Point leftBotPoint(screenRegion.x + screenRegion.w, screenRegion.y);
    // 우하단
    Point rightBotPoint(screenRegion.x + screenRegion.w, screenRegion.y + screenRegion.h);
>>>>>>>> Stashed changes:Cluster/DefaultAlgorithm/DBSCAN.cpp

    bool flag = false;
    for (auto iter : imageList) {
        flag = false;
        ImageNode img = iter;
        // img의 x좌표가 화면 내에 존재하는지?
        if (leftBotPoint.x <= img.pos.x && img.pos.x <= rightBotPoint.x) {
            // img의 y좌표가 화면 내에 존재하는지?
            if (leftTopPoint.y <= img.pos.y && img.pos.y <= leftBotPoint.y) flag = true;
        }
        // 위 조건을 만족하면 target으로 지정.
        if (flag) {
            targetImgList.push_back(img);
        }
    }
}

Benchmark DBSCAN::init(const Images& imageList, unsigned int screenWidth, unsigned int screenHeight) {
    Benchmark benchmark;
    std::chrono::high_resolution_clock::time_point start, end;

    start = std::chrono::high_resolution_clock::now();

    this->imageList = imageList;
    benchmark.compareCnt = 0;
    benchmark.maxNodes = 0;
    benchmark.elapsed = 0;
    benchmark.deviation = 0;
    benchmark.clusters.clear();

<<<<<<<< Updated upstream:Cluster/DBSCAN/DBSCAN.cpp
    findTargetImgList(Rectangle(0, 0, screenWidth, screenHeight));
========
>>>>>>>> Stashed changes:Cluster/DefaultAlgorithm/DBSCAN.cpp
    Rectangle rect = Rectangle(0, 0, screenWidth, screenHeight);
    findTargetImgList(rect);
    ImageClusters cl = dbscan(rect, 1, 1);
    benchmark.clusters = cl;

    end = std::chrono::high_resolution_clock::now();
    benchmark.elapsed = static_cast<time_t>((end - start).count());
    // TODO: need to set compareCnt, maxNodes, deviation, clusters in benchmark
    return benchmark;
};

Benchmark DBSCAN::iterate(const Rectangle& screenRegion) {
    Benchmark benchmark;
    std::chrono::high_resolution_clock::time_point start, end;

    start = std::chrono::high_resolution_clock::now();

    benchmark.compareCnt = 0;
    benchmark.maxNodes = 0;
    benchmark.elapsed = 0;
    benchmark.deviation = 0;
    benchmark.clusters.clear();

    findTargetImgList(screenRegion);
    Rectangle rect = screenRegion;
    ImageClusters cl = dbscan(rect, 1, 1);
    benchmark.clusters = cl;

    end = std::chrono::high_resolution_clock::now();
    benchmark.elapsed = static_cast<time_t>((end - start).count());
    // TODO: need to set compareCnt, maxNodes, deviation, clusters in benchmark
    return benchmark;
};

ImageClusters DBSCAN::dbscan(ImgCluster::Rectangle& rect, double eps, int minPts) {
    Images& data = targetImgList;                                // target image list
    auto visited = std::vector<bool>(data.size());              // visited
    std::vector<std::vector<int>> clusters;                      // clusters
    std::vector<std::pair<int, double>> matches;                 // images in radius of eps
    std::vector<std::pair<int, double>> sub_matches;             // images in radius of eps

    for (int i = 0; i < data.size(); i++) {
        if (visited[i]) continue;
        radiusSearch(data, i, eps, matches);
        if (matches.size() < minPts) continue;

        std::vector<int> cluster;

        while (matches.empty() == false) {
            int idx = matches.back().first;
            matches.pop_back();
            if (visited[idx]) continue;
            visited[idx] = true;

            cluster.push_back(idx);
            radiusSearch(data, idx, eps, sub_matches);
            if (sub_matches.size() >= minPts) {
                for (auto& match : sub_matches) {
                    matches.push_back(match);
                }
            }
        }
        clusters.push_back(cluster);
    }
    ImageClusters imgClusters;
    idxToPos(clusters, imgClusters);

    return imgClusters;
}

// TODO: need to implement radiusSearch using kd-tree
void radiusSearch(const Images& imgData, int idx, double eps, std::vector<std::pair<int, double>>& matches) {
    std::vector<std::pair<double, double>> data;
    for (auto iter : imgData) {
        data.push_back(std::make_pair(iter.pos.x, iter.pos.y));
    }
    matches.clear();
    for (int i = 0; i < data.size(); i++) {
        if (i == idx) continue;
        double dist = std::sqrt(std::pow(data[i].first - data[idx].first, 2) + std::pow(data[i].second - data[idx].second, 2));
        if (dist < eps) {
            matches.push_back(std::make_pair(i, dist));
        }
    }
}

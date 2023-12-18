#include "clusterInterface.cpp"
#include <chrono>
#include <cmath>
class DBSCAN : ImgCluster::ClusterAlgorithm {
public:
    DBSCAN(ImgCluster::Images& images, ImgCluster::Rectangle& rect, int minPts, double eps);
    ImageClusters dbscan(ImgCluster::Rectangle& rect, double eps, int minPts, int& compareCnt);
    void radiusSearch(const Images& data, int idx, double eps, std::vector<std::pair<int, double>>& matches, int& compareCnt);
    void idxToPos(std::vector<std::vector<int>> idx, ImgCluster::ImageClusters clusters);

    virtual int findTargetImgList(const Rectangle& screenRegion);

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
int DBSCAN::findTargetImgList(const Rectangle& screenRegion) {
    targetImgList.clear();

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

    int laxis = screenRegion.x > screenRegion.y ? screenRegion.x : screenRegion.y;
    int saxis = screenRegion.x < screenRegion.y ? screenRegion.x : screenRegion.y;

    int eps = saxis / 3; // set eps as 1/3 of smaller axis

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
    return eps;
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

    Rectangle rect = Rectangle(0, 0, screenWidth, screenHeight);
    int eps = findTargetImgList(rect);
    int compareCnt = 0;
    ImageClusters cl = dbscan(rect, eps, 1, compareCnt);

    end = std::chrono::high_resolution_clock::now();

    benchmark.elapsed = static_cast<time_t>((end - start).count());
    benchmark.clusters = cl;
    benchmark.maxNodes = targetImgList.size();
    benchmark.compareCnt = compareCnt;
    // TODO: need to set deviation in benchmark
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

    int eps = findTargetImgList(screenRegion);
    Rectangle rect = screenRegion;
    int compareCnt = 0;
    ImageClusters cl = dbscan(rect, eps, 1, compareCnt);

    end = std::chrono::high_resolution_clock::now();

    
    benchmark.elapsed = static_cast<time_t>((end - start).count());
    benchmark.clusters = cl;
    benchmark.compareCnt = compareCnt;
    benchmark.maxNodes = targetImgList.size();

    // TODO: need to set deviation in benchmark
    return benchmark;
};

ImageClusters DBSCAN::dbscan(ImgCluster::Rectangle& rect, double eps, int minPts, int& compareCnt) {
    Images& data = targetImgList;                                // target image list
    auto visited = std::vector<bool>(data.size());               // visited
    std::vector<std::vector<int>> clusters;                      // clusters
    std::vector<std::pair<int, double>> matches;                 // images in radius of eps
    std::vector<std::pair<int, double>> sub_matches;             // images in radius of eps

    for (int i = 0; i < data.size(); i++) {
        if (visited[i]) continue;
        radiusSearch(data, i, eps, matches, compareCnt);
        if (matches.size() < minPts) continue;

        std::vector<int> cluster;

        while (matches.empty() == false) {
            int idx = matches.back().first;
            matches.pop_back();
            if (visited[idx]) continue;
            visited[idx] = true;

            cluster.push_back(idx);
            radiusSearch(data, idx, eps, sub_matches, compareCnt);
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
void radiusSearch(const Images& imgData, int idx, double eps, std::vector<std::pair<int, double>>& matches, int& compareCnt) {
    std::vector<std::pair<double, double>> data;
    for (auto iter : imgData) {
        data.push_back(std::make_pair(iter.pos.x, iter.pos.y));
    }
    matches.clear();
    for (int i = 0; i < data.size(); i++) {
        if (i == idx) continue;
        double dist = std::sqrt(std::pow(data[i].first - data[idx].first, 2) + std::pow(data[i].second - data[idx].second, 2));
        if (++compareCnt && dist < eps) {
            matches.push_back(std::make_pair(i, dist));
        }
    }
}

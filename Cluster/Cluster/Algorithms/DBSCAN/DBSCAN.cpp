#include "clusterInterface.cpp"
#include <chrono>
class DBSCAN : ImgCluster::ClusterAlgorithm {
    // TODO: need to set eps value
public:
    DBSCAN(ImgCluster::Images& images, ImgCluster::Rectangle& rect, int minPts, double eps);
    auto dbscan(ImgCluster::Rectangle& rect, double eps, int minPts = 1);
    void radiusSearch(const Images& data, size_t idx, float eps, std::vector<std::pair<size_t, float>>& matches);
    
    virtual void findTargetImgList(const Rectangle& screenRegion);

    virtual Benchmark init(const Images& imageList, unsigned int screenWidth, unsigned int screenHeight);
    virtual Benchmark iterate(const Rectangle& screenRegion);
private:
    ImgCluster::Images imageList;
    ImgCluster::Images targetImgList;
    unsigned int screenWidth, screenHeight;
};

// copy from Kmeans.cpp
void DBSCAN::findTargetImgList(const Rectangle &screenRegion) {
    targetImgList.clear();

    // 좌상단
    Point leftTopPoint = screenRegion.getPoint(0, 0);
    // 우상단
    Point rightTopPoint = screenRegion.getPoint(0, 1);
    // 좌하단
    Point leftBotPoint = screenRegion.getPoint(1, 0);
    // 우하단
    Point rightBotPoint = screenRegion.getPoint(1, 1);

    bool flag = false;
    for (auto iter : imageList) {
        flag = false;
        ImageNode img = iter;
        // img의 x좌표가 화면 내에 존재하는지?
        if (leftBotPoint.x <= img.pos.x && img.pos.x <= rightBotPoint.x ) {
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

    findTargetImgList(Rectangle(0, 0, screenWidth, screenHeight));    
    Rectangle rect = Rectangle(0, 0, screenWidth, screenHeight);
    dbscan(rect, 1, 1);

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
    dbscan(rect, 1, 1);

    end = std::chrono::high_resolution_clock::now();
    benchmark.elapsed = static_cast<time_t>((end - start).count());
    // TODO: need to set compareCnt, maxNodes, deviation, clusters in benchmark
    return benchmark;
};

auto DBSCAN::dbscan(ImgCluster::Rectangle& rect, double eps, int minPts = 1) {
    Images& data = targetImgList;                               // target image list
    auto visited  = std::vector<bool>(data.size());             // visited
    auto clusters = std::vector<std::vector<size_t>>();         // clusters
    auto matches  = std::vector<std::pair<size_t, float>>();    // images in radius of eps
    auto sub_matches = std::vector<std::pair<size_t, float>>(); // images in radius of eps

    for(size_t i = 0; i < data.size(); i++) {
        if(visited[i]) continue;
        radiusSearch(data, i, eps, matches);
        if(matches.size() < minPts) continue;

        auto cluster = std::vector<size_t>(); 

        while(matches.empty() == false) {
            auto idx = matches.back().first;
            matches.pop_back();
            if(visited[idx]) continue;
            visited[idx] = true;

            cluster.push_back(idx);
            radiusSearch(data, idx, eps, sub_matches);
            if(sub_matches.size() >= minPts) {
                for(auto& match : sub_matches) {
                    matches.push_back(match);
                }
            }
        }
        clusters.push_back(cluster);
    }
    return clusters;
}

// TODO: need to implement radiusSearch using kd-tree
void radiusSearch(const Images& imgData, size_t idx, float eps, std::vector<std::pair<size_t, float>>& matches) {
    std::vector<std::pair<double, double>> data;
    for (auto iter : imgData) {
        data.push_back(std::make_pair(iter.pos.x, iter.pos.y));
    }
    matches.clear();
    for(size_t i = 0; i < data.size(); i++) {
        if(i == idx) continue;
        auto dist = std::sqrt(std::pow(data[i].first - data[idx].first, 2) + std::pow(data[i].second - data[idx].second, 2));
        if(dist < eps) {
            matches.push_back(std::make_pair(i, dist));
        }
    }
} 

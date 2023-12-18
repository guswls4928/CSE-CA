#include "DBSCAN.h"
DBSCAN::DBSCAN(ImgCluster::Images& images, ImgCluster::Rectangle& rect, int minPts, double eps) {
    screenWidth = rect.w;
    screenHeight = rect.h;
    init(images, screenWidth, screenHeight);
}
/*
double calculateMean(const ImgCluster::ImageClusters& clusters) {
    double sum = 0.0;
    int count = 0;
    for (const auto& cluster : clusters) {
        for (const auto& image : cluster) {
            sum += image.value; // replace `value` with the actual attribute you want to calculate the mean for
            count++;
        }
    }
    return sum / count;
}

double calculateVariance(const ImgCluster::ImageClusters& clusters, double mean) {
    double sum = 0.0;
    int count = 0;
    for (const auto& cluster : clusters) {
        for (const auto& image : cluster) {
            double diff = image.value - mean; // replace `value` with the actual attribute you want to calculate the variance for
            sum += diff * diff;
            count++;
        }
    }
    return sum / count;
}

double calculateStandardDeviation(double variance) {
    return std::sqrt(variance);
}
*/
double DBSCAN::calculateDeviation(ImgCluster::Benchmark bm) {
    double deviation = 0;
    ImgCluster::Point meanPt;

    for(int i = 0; i < clusterImgList.size(); i++) { // i: cluster index
//      for(int j = 0; j < clusterImgList[i].size(); j++) { // j: image index in cluster
//          meanPt.x += clusterImgList[i][0].pos.x;
//		    meanPt.y += clusterImgList[i][0].pos.y;
//	    }
//      meanPt.x /= clusterImgList[i].size();
//      meanPt.y /= clusterImgList[i].size();

        meanPt = bm.clusters[i].pos;

        ImgCluster::Point diffPt;
        for (int j = 0; j < clusterImgList[i].size(); j++) {
            diffPt.x += std::pow(clusterImgList[i][j].pos.x - meanPt.x, 2);
            diffPt.y += std::pow(clusterImgList[i][j].pos.y - meanPt.y, 2);
        }

        deviation += std::sqrt(diffPt.x + diffPt.y);
	}

    deviation /= clusterImgList.size();

    return deviation;
}

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
int DBSCAN::findTargetImgList(const ImgCluster::Rectangle& screenRegion) {
    targetImgList.clear();

    /*
        Point getPoint(double iftop0, double ifleft0) {
            return Point(iftop0 ? x + w : x, ifleft0 ? y + h : y);
        }
    */

    // �»��
    ImgCluster::Point leftTopPoint(screenRegion.x, screenRegion.y);
    // ����
    ImgCluster::Point rightTopPoint(screenRegion.x, screenRegion.y + screenRegion.h);
    // ���ϴ�
    ImgCluster::Point leftBotPoint(screenRegion.x + screenRegion.w, screenRegion.y);
    // ���ϴ�
    ImgCluster::Point rightBotPoint(screenRegion.x + screenRegion.w, screenRegion.y + screenRegion.h);

    int laxis = screenRegion.x > screenRegion.y ? screenRegion.x : screenRegion.y;
    int saxis = screenRegion.x < screenRegion.y ? screenRegion.x : screenRegion.y;

    int eps = saxis / 3; // set eps as 1/3 of smaller axis

    bool flag = false;
    for (auto iter : imageList) {
        flag = false;
        ImgCluster::ImageNode img = iter;
        // img�� x��ǥ�� ȭ�� ���� �����ϴ���?
        if (leftBotPoint.x <= img.pos.x && img.pos.x <= rightBotPoint.x) {
            // img�� y��ǥ�� ȭ�� ���� �����ϴ���?
            if (leftTopPoint.y <= img.pos.y && img.pos.y <= leftBotPoint.y) flag = true;
        }
        // �� ������ �����ϸ� target���� ����.
        if (flag) {
            targetImgList.push_back(img);
        }
    }
    return eps;
}

ImgCluster::Benchmark DBSCAN::init(const ImgCluster::Images& imageList, unsigned int screenWidth, unsigned int screenHeight) {
    ImgCluster::Benchmark benchmark;
    std::chrono::high_resolution_clock::time_point start, end;

    start = std::chrono::high_resolution_clock::now();

    this->imageList = imageList;
    benchmark.compareCnt = 0;
    benchmark.maxNodes = 0;
    benchmark.elapsed = 0;
    benchmark.deviation = 0;
    benchmark.clusters.clear();

    ImgCluster::Rectangle rect = ImgCluster::Rectangle(0, 0, screenWidth, screenHeight);
    int eps = findTargetImgList(rect);
    int compareCnt = 0;
    ImgCluster::ImageClusters cl = dbscan(rect, eps, 1, compareCnt);

    end = std::chrono::high_resolution_clock::now();


    benchmark.elapsed = static_cast<time_t>((end - start).count());
    benchmark.clusters = cl;
    benchmark.deviation = calculateDeviation(benchmark); // need to run after set benchmark.clusters
    benchmark.maxNodes = targetImgList.size();
    benchmark.compareCnt = compareCnt;
    return benchmark;
};

ImgCluster::Benchmark DBSCAN::iterate(const ImgCluster::Rectangle& screenRegion) {
    ImgCluster::Benchmark benchmark;
    std::chrono::high_resolution_clock::time_point start, end;

    start = std::chrono::high_resolution_clock::now();

    benchmark.compareCnt = 0;
    benchmark.maxNodes = 0;
    benchmark.elapsed = 0;
    benchmark.deviation = 0;
    benchmark.clusters.clear();

    int eps = findTargetImgList(screenRegion);
    ImgCluster::Rectangle rect = screenRegion;
    int compareCnt = 0;
    ImgCluster::ImageClusters cl = dbscan(rect, eps, 1, compareCnt);

    end = std::chrono::high_resolution_clock::now();


    benchmark.elapsed = static_cast<time_t>((end - start).count());
    benchmark.clusters = cl;
    benchmark.deviation = calculateDeviation(benchmark); // need to run after set benchmark.clusters
    benchmark.compareCnt = compareCnt;
    benchmark.maxNodes = targetImgList.size();

    // TODO: need to set deviation in benchmark
    return benchmark;
};

ImgCluster::ImageClusters DBSCAN::dbscan(ImgCluster::Rectangle& rect, double eps, int minPts, int& compareCnt) {
    ImgCluster::Images& data = targetImgList;                                // target image list
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

    ImgCluster::ImageClusters imgClusters;
    idxToPos(clusters, imgClusters);

    return imgClusters;
}

// TODO: need to implement radiusSearch using kd-tree
void DBSCAN::radiusSearch(const ImgCluster::Images& imgData, int idx, double eps, std::vector<std::pair<int, double>>& matches, int& compareCnt) {
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

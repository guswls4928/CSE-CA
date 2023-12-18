#include "RandomForest.h"

Benchmark RandomForest::init(const Images& imageList, unsigned long mapWidth, unsigned long mapHeight, unsigned int screenWidth, unsigned int screenHeight){
    this->imgList = imageList;
    this->screenHeight = screenHeight;
    this->screenWidth = screenWidth;
    this->mapHeight = mapHeight;
    this->mapWidth = mapWidth;
    cnt = 0;

    clusterAssignments.resize(numTrees, vector<int>(data.size(), 0));
    
    Benchmark res;
    res.compareCnt = 0;
    res.deviation = 0;
    res.elapsed = 0;
    res.maxNodes = imageList.size();
}

void RandomForest::findTarget(const Rectangle& screenRegion){
    Point leftTop = screenRegion.getPoint(0, 0);
    Point rightBottom = screenRegion.getPoint(1, 1);

    for(auto i = imgList.begin(); i != imgList.end(); ++i){
        ImageNode img = *i;
        if(leftTop.x <= img.pos.x && rightBottom.y <= img.pos.y && img.pos.x <= rightBottom.x && img.pos.y <= leftTop.y){
            data.push_back(img);
        }
    }
}

double RandomForest::euclideanDistance(const Point a, const Point b){
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void RandomForest::initializeClusters(){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, data.size() - 1);

    unordered_map<int, bool> chosen;

    for (int i = 0; i < numClusters; ++i) {
        int index = dis(gen);
        while (chosen.find(index) != chosen.end()) {
            index = dis(gen);
        }
        chosen[index] = true;
        centers.push_back(data[index]);
    }
}

int RandomForest::assignToCluster(const Point point){
    double minDistance = numeric_limits<double>::max();
    int cluster = 0;

    for (size_t i = 0; i < centers.size(); ++i) {
        double dist = euclideanDistance(point, centers[i].pos);
        if (dist < minDistance) {
            minDistance = dist;
            cluster = i;
        }
    }

    return cluster;
}

void RandomForest::randomForestClustering(){
    for (int tree = 0; tree < numTrees; ++tree) {
        initializeClusters();

        for (size_t i = 0; i < data.size(); ++i) {
            clusterAssignments[tree][i] = assignToCluster(data[i].pos);
        }
    }

    clusterList = {};
    for (int i = 0; i < numClusters; ++i){
        ImageCluster cluster;
        for (size_t i = 0; i < data.size(); ++i){
            cluster.count++;
        }
        cluster.repr = centers[i];
        cluster.pos = centers[i].pos;
        clusterList.push_back(cluster);
    }
}

double RandomForest::calDev(){
    int cluster;
    double distx = 0;
    double disty = 0;
    for(int i = 0; i < data.size(); i++){
        cluster = clusterAssignments[0][i];
        distx += abs(centers[i].pos.x - data[i].pos.x);
        disty += abs(centers[i].pos.y - data[i].pos.y);
    }
    distx /= data.size();
    disty /= data.size();
    double diff = 0;
    for(int i = 0; i < data.size(); i++){
        diff += pow(distx - data[i].pos.x, 2) + pow(disty - data[i].pos.y, 2);
    }
    return sqrt(diff);
}

Benchmark RandomForest::iterate(const Rectangle &screenRegion){
    start = clock();
    findTarget(screenRegion);
    randomForestClustering();
    end = clock();

    Benchmark res;
    res.compareCnt = cnt;
    res.deviation = calDev();
    res.elapsed = (double)(end - start) / CLOCKS_PER_SEC;;
    res.maxNodes = imgList.size();
    res.clusters = clusterList;
    
    return res;
}
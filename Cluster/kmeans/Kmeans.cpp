//
// Created by 임준영 on 2023/12/10.
//
#include "Kmeans.h"

Benchmark Kmeans::init(const Images &imageList, unsigned int screenWidth, unsigned int screenHeight){
    auto start = std::chrono::high_resolution_clock::now();
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->imgList = imageList;


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    Benchmark ret;
    ret.compareCnt = 0;
    // ?
    ret.maxNodes = imgList.size();
    // 표준편차?
    ret.deviation = 0;
    // 시간
    ret.elapsed = static_cast<time_t>(duration.count());
    return ret;
}

void Kmeans::findTargetImgList(const Rectangle &screenRegion) {
    //targetImgList.clear();
    targetImgMap.clear();
    targetImgSize = 0;

    // 좌상단
    leftTopPoint = screenRegion.getPoint(0, 0);
    // 우상단
    rightTopPoint = screenRegion.getPoint(0, 1);
    // 좌하단
    leftBotPoint = screenRegion.getPoint(1, 0);
    // 우하단
    rightBotPoint = screenRegion.getPoint(1, 1);

    bool flag = false;
    auto end = imgList.end();
    for (auto iter = imgList.begin(); iter != end; ++iter) {
        flag = false;
        ImageNode img = (*iter);
        // img의 x좌표가 화면 내에 존재하는지?
        if (leftBotPoint.x <= img.pos.x && img.pos.x <= rightBotPoint.x ) {
            // img의 y좌표가 화면 내에 존재하는지?
            if (leftTopPoint.y <= img.pos.y && img.pos.y <= leftBotPoint.y) flag = true;
        }
        // 위 조건을 만족하면 target으로 지정.
        if (flag) {
            //targetImgList.push_back(img);
            targetImgMap[targetImgSize++] = std::pair(img, -1);
        }
    }
    selectClusterCenter(screenRegion);
}

void Kmeans::selectClusterCenter(const Rectangle &screenRegion){
    clusterList.clear();
    meanPosition.clear();

    // 4개 보다 적은 수의 사진만 있는 경우
    // 사진 각각이 그냥 클러스터가 됨.
    if(targetImgSize < 4){
        for(int i = 0; i < targetImgSize; i++){
            ImageCluster tmpCluster;
            tmpCluster.pos = targetImgMap[i].first.pos;
            tmpCluster.count = 1;
            tmpCluster.repr = targetImgMap[i].first;
            clusterList.push_back(tmpCluster);
        }
        clusterSize = targetImgSize;
    }
    else {
        // 3x3 격자로 잘랐을 때 점이 만나는 지점을 클러스터 초기 중심으로 설정.
        double x1 = (leftTopPoint.x + rightTopPoint.x) / 3;
        double x2 = (leftTopPoint.x + rightTopPoint.x) * 2 / 3;
        double y1 = (leftTopPoint.y + rightTopPoint.y) / 3;
        double y2 = (leftTopPoint.y + rightTopPoint.y) / 3;
        bool isDuplicated = false;
        // 중심 좌표에서 가까운 이미지 4개 선택
        ImageNode imageNodes[4];
        imageNodes[0] = findNearestImg(x1, y1);
        imageNodes[1] = findNearestImg(x1, y2);
        imageNodes[2] = findNearestImg(x2, y1);
        imageNodes[3] = findNearestImg(x2, y2);

        // 겹치는 이미지라면 넣지 않고 겹치지 않으면 넣음.
        for (int i = 0; i < 4; ++i) {
            isDuplicated = false;
            ImageNode &tmpImg = imageNodes[i];
            // 겹치는지 체크
            for (auto iter = clusterList.begin(); iter != clusterList.end(); ++iter) {
                if (tmpImg.pos.x == (*iter).pos.x && tmpImg.pos.y == (*iter).pos.y) { isDuplicated = true; }
            }
            // 겹치지 않으면 추가
            if (!isDuplicated) {
                ImageCluster tmpCluster;
                tmpCluster.pos = tmpImg.pos;
                tmpCluster.count = 1;
                tmpCluster.repr = tmpImg;
                clusterList.push_back(tmpCluster);
                meanPosition.push_back(std::pair(0,0));
            }
        }
        clusterSize = clusterList.size();
    }
}

// 특정 점으로부터 가장 가까운 이미지를 찾는 함수
ImageNode Kmeans::findNearestImg(double x, double y) {
    Point tmpPoint(x, y);
    ImageNode resultImgNode;
    double minDistance = 1000000000;
    double tmpDistance = 0;
    for (auto iter = targetImgMap.begin(); iter != targetImgMap.end(); ++iter) {
        ImageNode &tmpImageNode = (*iter).second.first;
        tmpDistance = calculateManhattanDistance(tmpPoint, tmpImageNode.pos);
        compareCnt++;
        if (minDistance > tmpDistance) {
            minDistance = tmpDistance;
            resultImgNode = tmpImageNode;
        }
    }
    return resultImgNode;
}

// 맨하탄 거리를 계산하는 함수
double Kmeans::calculateManhattanDistance(ImageNode img1, ImageNode img2) {
    double distance = (img1.pos.x - img2.pos.x) + (img1.pos.y - img2.pos.y);
    return distance > 0 ? distance : -1 * distance;
}
double Kmeans::calculateManhattanDistance(Point img1, Point img2) {
    double distance = (img1.x - img2.x) + (img1.y - img2.y);
    return distance > 0 ? distance : -1 * distance;
}

void Kmeans::kmeans(){
    double minDistance = 100000000;
    double distance = 0;
    int newClusterNum = 0;
    if(targetImgSize < 4){
        return;
    }
    bool nonUpdate = false;
    while (true) {
        // 클러스터 업데이트가 없다면 종료
        if (nonUpdate) {
            break;
        }
        nonUpdate = true;

        // 각 사진의 가장 가까운 중심 찾기
        for (int i = 0; i < targetImgSize; ++i) {
            // 각 사진
            //ImageNode &tmpImg = targetImgList[i];
            std::pair<ImageNode, int> &tmpImgPair = targetImgMap[i];
            ImageNode &tmpImg = tmpImgPair.first;
            int &tmpClusterNumber = tmpImgPair.second;
            minDistance = 0;
            newClusterNum = 0;
            for (int j = 0; j < clusterSize; ++j) {
                // 각 클러스터
                ImageCluster &tmpCluster = clusterList[j];
                // 거리 계산
                distance = calculateManhattanDistance(tmpImg.pos, tmpCluster.pos);
                compareCnt++;
                // 최솟값 찾기
                if (minDistance > distance) {
                    minDistance = distance;
                    newClusterNum = j;
                }
            }
            // 각 클러스터에 추가
            // 현재 클러스터와 새로운 클러스터가 다르면 클러스터 업데이트.
            compareCnt++;
            if (tmpClusterNumber != newClusterNum) {
                nonUpdate = false;
                ImageCluster &currentCluster = clusterList[tmpClusterNumber];
                ImageCluster &newCluster = clusterList[newClusterNum];
                // 클러스터에 속하는 이미지 개수 업데이트
                if(currentCluster.count != 0){
                    currentCluster.count--;
                }
                newCluster.count++;
                // 현재 이미지가 속한 클러스터 변경
                tmpClusterNumber = newClusterNum;
            }
        }

        // 클러스터 센터 업데이트, 클러스터에 업데이트가 있는 경우에만 수행함.
        if (!nonUpdate) {
            // 모든 이미지에 대해 각 클러스터의 평균 좌표를 구하기 위해 각 이미지의 좌표를 모두 더함.
            for(int i = 0; i < targetImgSize; i++){
                ImageNode &tmpImageNode = targetImgMap[i].first;
                int &clusterNumber = targetImgMap[i].second;
                //double tmpDistance = calculateManhattanDistance(cluster.pos, tmpImageNode.pos);
                meanPosition[clusterNumber].first += tmpImageNode.pos.x;
                meanPosition[clusterNumber].second += tmpImageNode.pos.y;
            }
            // 각 클러스터에 포함된 이미지의 위치 평균을 구하고 가장 가까운 사진을 클러스터 센터로 지정
            for (int i = 0; i < clusterSize; i++) {
                ImageCluster &cluster = clusterList[i];
                // 평균 구하기
                if(cluster.count != 0){
                    meanPosition[i].first /= cluster.count;
                    meanPosition[i].second /= cluster.count;

                    // 가장 가까운 사진을 찾고 그것을 클러스터 대표로 변경함.
                    ImageNode tmpImg = findNearestImg(meanPosition[i].first, meanPosition[i].second);
                    cluster.repr = tmpImg;
                    cluster.pos = tmpImg.pos;
                } else {
                    // count가 0이라면 클러스터에 속한 이미지가 없는 것이므로, 변경하지 않음.
                }
            }
        }
    }
}

double Kmeans::calculateDeviation(){
    //std::vector<double> diffDistance;
    double diffDistance = 0;
    double meanX = 0;
    double meanY = 0;
    double power2Value = 0;
    for(int i = 0; i < targetImgSize; i++){
        ImageNode &tmpImageNode = targetImgMap[i].first;
        int &clusterNumber = targetImgMap[i].second;
        ImageCluster &cluster = clusterList[clusterNumber];

        diffDistance = cluster.pos.x - tmpImageNode.pos.x;
        meanX += diffDistance > 0 ? diffDistance : -1 * diffDistance;

        diffDistance = cluster.pos.y - tmpImageNode.pos.y;
        meanY += diffDistance > 0 ? diffDistance : -1 * diffDistance;
    }
    meanX = meanX / targetImgSize;
    meanY = meanY / targetImgSize;

    for(int i = 0; i < targetImgSize; i++){
        ImageNode &tmpImageNode = targetImgMap[i].first;
        int &clusterNumber = targetImgMap[i].second;
        ImageCluster &cluster = clusterList[clusterNumber];

        // 평균과 각 점의 거리의 제곱
        diffDistance = pow(meanX - tmpImageNode.pos.x, 2) + pow(meanY - tmpImageNode.pos.y, 2);
        // 그걸 다 더함.
        power2Value += diffDistance;
    }

    power2Value /= targetImgSize;

    return sqrt(power2Value);
}

Benchmark Kmeans::iterate(const Rectangle &screenRegion){
    compareCnt = 0;
    auto start = std::chrono::high_resolution_clock::now();

    findTargetImgList(screenRegion);

    kmeans();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    Benchmark ret;
    ret.elapsed = static_cast<time_t>(duration.count());
    ret.compareCnt = compareCnt;
    ret.deviation = calculateDeviation();
    ret.maxNodes = targetImgSize + targetImgSize;
    ret.clusters = clusterList;
    return ret;
}

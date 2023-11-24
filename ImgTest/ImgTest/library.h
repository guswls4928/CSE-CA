#pragma once
#ifndef MAPRENDERER_LIBRARY_H
#define MAPRENDERER_LIBRARY_H
#include "clusterInterface.h"
#include <cstdint>
#include <functional>

#define ADJSPREAD 0.7
#define DIAGONALSPREAD 0.35
#define FORCEQUIT 0.1
#define MAXPROPORTION 0.5

typedef struct LabeledQueue {
    std::vector<ImgCluster::Region*> queue;
    uint8_t paint[3];
};

typedef std::vector<LabeledQueue> LabeledQueues;

class MapRenderer {
public:
    MapRenderer(int seed, unsigned int mapWidth, unsigned int mapHeight, int divCap, unsigned int minRegionSize);
    MapRenderer(const ImgCluster::Region& map, unsigned int mapWidth, unsigned int mapHeight);

    std::vector<uint8_t> FullBitmap;
    void RenderBitmap();
private:
    ImgCluster::Region map;
    unsigned int mapWidth;
    unsigned int mapHeight;
    void generateMap(int seed, unsigned int mapWidth, unsigned int mapHeight, int divCap, unsigned int minRegionSize);
    inline bool colorMatch(uint8_t* a, uint8_t* b);
    bool spread(int& seed, unsigned int mapWidth, unsigned int mapHeight, int divCap, unsigned int minRegionSize, ImgCluster::Region* targetRegion);
    bool allQueuesEmpty(const LabeledQueues& queues);

};

#endif //MAPRENDERER_LIBRARY_H

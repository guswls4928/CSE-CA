#include "library.h"

MapRenderer::MapRenderer(const ImgCluster::Region& map, unsigned int mapWidth, unsigned int mapHeight)
    : map(map), mapWidth(mapWidth), mapHeight(mapHeight) {
    RenderBitmap();
}

MapRenderer::MapRenderer(int seed, unsigned int mapWidth, unsigned int mapHeight, int divCap, unsigned int minRegionSize)
    : mapWidth(mapWidth), mapHeight(mapHeight) {

    generateMap(seed, mapWidth, mapHeight, divCap, minRegionSize);
}

void MapRenderer::RenderBitmap() {
    FullBitmap.resize(mapWidth * mapHeight * 3); // Allocate space for 24-bit bitmap (3 bytes per pixel)

    // Recursive lambda function for depth-first search
    std::function<void(const ImgCluster::Region&)> dfs = [&](const ImgCluster::Region& node) {
        if (node.isTerminal) {
            // Assuming 'pos' is the position of the pixel in the image
            int index = (node.pos.y * mapWidth + node.pos.x) * 3; // 3 bytes per pixel for RGB
            FullBitmap[index] = node.colorCode[2];     // Blue
            FullBitmap[index + 1] = node.colorCode[1]; // Green
            FullBitmap[index + 2] = node.colorCode[0]; // Red
        }
        else {
            for (const auto& child : node.children) {
                dfs(child); // Recursively call dfs for the child nodes
            }
        }
    };

    dfs(map); // Start DFS from the root of the tree
}


void MapRenderer::generateMap(int seed, unsigned int mapWidth, unsigned int mapHeight, int divCap, unsigned int minRegionSize) {
    
    ImgCluster::Regions canvas(mapWidth * mapHeight);
    for (unsigned int r = 0; r < mapHeight; r++)
    {
        for (unsigned int c = 0; c < mapWidth; c++) {
            canvas[r * mapWidth + c].colorCode[0] = 0;
            canvas[r * mapWidth + c].colorCode[1] = 0;
            canvas[r * mapWidth + c].colorCode[2] = 255;

            canvas[r * mapWidth + c].isTerminal = true;

            canvas[r * mapWidth + c].pos = ImgCluster::Point(c, r);
        }
    }
    map = ImgCluster::Region{ "root", {0,0,0}, canvas, false };

    LabeledQueues labeledQueues;
    labeledQueues.push_back({ std::vector<ImgCluster::Region*>(0), {0,0,255}});
    labeledQueues[0].queue.push_back(&map);

    //TODO: use dfs to call spread

}

inline bool MapRenderer::colorMatch(uint8_t* a, uint8_t* b)
{
    return *a == *b && *(a + 1) == *(b + 1) && *(a + 2) == *(b + 2);
}

bool MapRenderer::spread(int& seed, unsigned int mapWidth, unsigned int mapHeight, int divCap, unsigned int minRegionSize, ImgCluster::Region* targetRegion)
{
    if (targetRegion->children.size() < minRegionSize) return false;

    size_t originalSize = targetRegion->children.size();

    LabeledQueues labeledQueues; // Prepare labeled queues for BFS spread

    srand(seed++);
    int N = rand() % divCap;
    //..if N>originalSize then N=originalSize
    //..pick N number of objects from target->children and save them to Region[N]
    for (int i = 0; i < N; i++) {
        LabeledQueue nq;
        //..nq.queue.push_back(Region[i])
        //todo: nq.paint = colorgenerate
        labeledQueues.push_back(nq);
    }

    while (!allQueuesEmpty(labeledQueues)) {
        for (LabeledQueue& queue : labeledQueues) {
            if (queue.queue.empty()) continue;

            ImgCluster::Region* region = queue.queue.front();
            queue.queue.erase(queue.queue.begin()); // Pop the first item

            // Define lambda for checking and adding adjacent tiles
            auto addAdjacent = [&](int x, int y, double probability) {
                if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
                    // Calculate index for the one-dimensional array representation
                    int index = y * mapWidth + x;
                    // Check if it's valid and has the same color as the parent region
                    if (colorMatch(map.children[index].colorCode, targetRegion->colorCode)) {
                        if ((double)rand() / RAND_MAX < probability) {
                            // Paint the tile with the queue's assigned color
                            memcpy(map.children[index].colorCode, queue.paint, 3);
                            // Remove from targetRegion and add to the paired Region's children
                            // ... Code to manipulate the children vectors
                            queue.queue.push_back(&map.children[index]);
                        }
                    }
                }
            };

            // Check all adjacent tiles
            addAdjacent(region->pos.x - 1, region->pos.y, ADJSPREAD); // Left
            addAdjacent(region->pos.x + 1, region->pos.y, ADJSPREAD); // Right
            addAdjacent(region->pos.x, region->pos.y - 1, ADJSPREAD); // Up
            addAdjacent(region->pos.x, region->pos.y + 1, ADJSPREAD); // Down
            // ... Repeat for diagonals with DIAGONALSPREAD

            // Clear the queue if it grows too large
            if (queue.queue.size() > originalSize * MAXPROPORTION) {
                queue.queue.clear();
            }
        }
    }

    return true;
}


bool MapRenderer::allQueuesEmpty(const LabeledQueues& queues)
{
    for (LabeledQueue q : queues) {
        if (q.queue.size()) return false;
    }
    return true;
}



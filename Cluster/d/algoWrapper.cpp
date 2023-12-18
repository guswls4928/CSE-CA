#include "DBSCAN.h"


extern "C" {
    namespace Wrapper {
#pragma pack(push, 1)
        struct Point {
            double x, y;
        };
#pragma pack(pop)

#pragma pack(push, 1)
        struct ImageNode {
            char* fileName;
            Point pos;
        };
#pragma pack(pop)

#pragma pack(push, 1)
        struct Rectangle {
            double x, y;
            double w, h;
        };
#pragma pack(pop)

#pragma pack(push, 1)
        struct ImgArray {
            ImageNode* head;
            int count;
        };
#pragma pack(pop)

#pragma pack(push, 1)
        struct ImageCluster {
            ImageNode repr;
            Point pos;
            int count;
        };
#pragma pack(pop)

#pragma pack(push, 1)
        struct ClusterArray {
            ImageCluster* head;
            int count;
        };
#pragma pack(pop)

#pragma pack(push, 1)
        struct Benchmark {
            int compareCnt;
            int maxNodes;
            int elapsed;
            double deviation;
            ClusterArray clusters;
        };
#pragma pack(pop)

#pragma pack(push, 1)
        struct SAMPLE {
            double a;
            int b;
            double c;
        };
#pragma pack(pop)

        ImgCluster::Rectangle convertRect(struct Rectangle rect) {
            return ImgCluster::Rectangle(rect.x, rect.y, rect.w, rect.h);
        }

        ImgCluster::Images convertVector(struct ImgArray images) {
            ImgCluster::Images result;
            for (size_t i = 0; i < images.count; i++) {
                ImgCluster::ImageNode tmp;
                tmp.fileName = images.head[i].fileName;
                tmp.pos.x = images.head[i].pos.x;
                tmp.pos.y = images.head[i].pos.y;
                result.push_back(tmp);
            }
            return result;
        }

        struct Benchmark convertBenchmark(ImgCluster::Benchmark b) {
            Wrapper::ImageCluster* alloc = new ImageCluster[b.clusters.size()];
            for (int i = 0; i < b.clusters.size(); i++) {
                alloc[i].count = b.clusters[i].count;
                alloc[i].pos.x = b.clusters[i].pos.x;
                alloc[i].pos.y = b.clusters[i].pos.y;
                alloc[i].repr.fileName = strdup(b.clusters[i].repr.fileName.c_str());
                alloc[i].repr.pos.x = b.clusters[i].repr.pos.x;
                alloc[i].repr.pos.y = b.clusters[i].repr.pos.y;

            }
            ClusterArray arr;
            arr.count = b.clusters.size();
            arr.head = alloc;
            Benchmark ret;
            ret.clusters = arr;
            ret.compareCnt = b.compareCnt;
            ret.maxNodes = b.maxNodes;
            ret.deviation = b.deviation;
            ret.elapsed = b.elapsed;

            return ret;
        }

        void* createAlgoClass() {
            return new DBSCAN();
            //return new int[4];
        }

        SAMPLE testFunc(double p1, double p2) { 
            SAMPLE t;
            t.a = p1;
            t.c = p2;
            t.b = 7;

            return t;
        }

        Benchmark InitAlgorithm(ImgArray imageList, unsigned int screenWidth, unsigned int screenHeight, void* algoClass) {
            return convertBenchmark(((DBSCAN*)algoClass)->init(convertVector(imageList), screenWidth, screenHeight));
        }

        Benchmark IterateAlgorithm(void* algoClass, struct Rectangle screenRegion) {
            return convertBenchmark(((DBSCAN*)algoClass)->iterate(convertRect(screenRegion)));
        }

        void DeleteAlgorithm(void* algoClass) {
            delete ((DBSCAN*)algoClass);
        }
    }
}
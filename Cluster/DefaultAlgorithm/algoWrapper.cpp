#include "SampleImplementation.h"


extern "C" {
    namespace Wrapper {

        struct Point {
            double x, y;
        };

        struct ImageNode {
            const char* fileName;
            Point pos;
        };

        struct Rectangle {
            double x, y;
            double w, h;
        };

        struct ImgArray {
            ImageNode* head;
            size_t count;
        };

        struct ImageCluster {
            ImageNode repr;
            Point pos;
            int count;
        };

        struct ClusterArray {
            ImageCluster* head;
            size_t count;
        };

        struct Benchmark {
            unsigned long compareCnt;
            unsigned long maxNodes;
            int elapsed;
            double deviation;
            ClusterArray clusters;
        };

        ImgCluster::Rectangle convertRect(struct Rectangle rect) {
            return ImgCluster::Rectangle(rect.x, rect.y, rect.w, rect.h);
        }

        ImgCluster::Images convertVector(struct ImgArray images) {
            ImgCluster::Images result;
            for (size_t i = 0; i < images.count; i++)
                result.push_back({ images.head[i].fileName,
                                  {images.head[i].pos.x, images.head[i].pos.y} });
            return result;
        }

        struct Benchmark convertBenchmark(ImgCluster::Benchmark b) {
            Wrapper::ImageCluster* alloc = new ImageCluster[b.clusters.size()];
            for (int i = 0; i < b.clusters.size(); i++) {
                alloc[i] = { {b.clusters[i].repr.fileName.c_str(), {b.clusters[i].repr.pos.x, b.clusters[i].repr.pos.y}},
                            {b.clusters[i].pos.x, b.clusters[i].pos.y},
                            b.clusters[i].count };
            }

            return Benchmark{
                b.compareCnt,
                b.maxNodes,
                b.elapsed,
                b.deviation,
                {alloc, b.clusters.size()}
            };
        }

        void* createAlgoClass() {
            return new SampleAlgorithm();
            //return new int[4];
        }

        Benchmark InitAlgorithm(ImgArray imageList, unsigned int screenWidth, unsigned int screenHeight, void* algoClass) {
            return convertBenchmark(((SampleAlgorithm*)algoClass)->init(convertVector(imageList), screenWidth, screenHeight));
        }

        Benchmark IterateAlgorithm(void* algoClass, struct Rectangle screenRegion) {
            return convertBenchmark(((SampleAlgorithm*)algoClass)->iterate(convertRect(screenRegion)));
        }

        void DeleteAlgorithm(void* algoClass) {
            delete ((SampleAlgorithm*)algoClass);
        }

    }
}
using Extern;
using Microsoft.Maui.Maps;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Algorithm
{
    public struct Rectangle
    {
        public double X, Y, Width, Height;
    }

    public struct Point
    {
        public double X, Y;
    }

    public struct ImageNode
    {
        public string FileName;
        public Point Position;
    }

    public struct ImageCluster
    {
        public ImageNode Representative;
        public Point Position;
        public int Count;
    }

    public struct Benchmark
    {
        public int CompareCount;
        public int MaxNodes;
        public int Elapsed;
        public double Deviation;
        public List<ImageCluster> Clusters;
    }

    internal class AlgorithmInterface : IDisposable
    {
        ClusterAlgorithmWrapper clusterAlgorithm;
        public AlgorithmInterface(ClusterAlgorithmWrapper clusterAlgorithm)
        {
            this.clusterAlgorithm = clusterAlgorithm;
        }

        public Benchmark Initialize(List<ImageNode> imageNodes, uint screenWidth, uint screenHeight)
        {
            var unmanagedImageNodes = imageNodes.Select(node => new Extern.ImageNode
            {
                fileName = ClusterAlgorithmWrapper.StringToUnmanagedString(node.FileName),
                pos = new Extern.Point { x = node.Position.X, y = node.Position.Y }
            }).ToList();

            var imgArray = new Extern.ImgArray
            {
                head = ClusterAlgorithmWrapper.MarshalImgArrayToUnmanaged(unmanagedImageNodes),
                count = imageNodes.Count
            };


            var benchmark = clusterAlgorithm.Init(imgArray, screenWidth, screenHeight);
            // Free unmanaged resources
            foreach (var unmanagedNode in unmanagedImageNodes)
            {
                Marshal.FreeHGlobal(unmanagedNode.fileName);
            }
            Marshal.FreeHGlobal(imgArray.head);

            return ConvertToManagedBenchmark(benchmark);
        }

        public Benchmark Iterate(MapSpan span)
        {
            var unmanagedScreenRegion = new Extern.Rectangle
            {
                x = span.Center.Longitude - span.LongitudeDegrees / 2,
                y = span.Center.Latitude - span.LatitudeDegrees / 2,
                w = span.LongitudeDegrees,
                h = span.LatitudeDegrees
            };

            var benchmark = clusterAlgorithm.Iterate(unmanagedScreenRegion);

            return ConvertToManagedBenchmark(benchmark);
        }

        private Benchmark ConvertToManagedBenchmark(Extern.Benchmark unmanagedBenchmark)
        {
            var managedClusters = ClusterAlgorithmWrapper.MarshalClusterArrayToManaged(unmanagedBenchmark.clusters.head, unmanagedBenchmark.clusters.count)
                .Select(c => new ImageCluster
                {
                    Representative = new ImageNode
                    {
                        FileName = ClusterAlgorithmWrapper.UnmanagedStringToString(c.repr.fileName),
                        Position = new Point { X = c.repr.pos.x, Y = c.repr.pos.y }
                    },
                    Position = new Point { X = c.pos.x, Y = c.pos.y },
                    Count = c.count
                }).ToList();

            return new Benchmark
            {
                CompareCount = unmanagedBenchmark.compareCnt,
                MaxNodes = unmanagedBenchmark.maxNodes,
                Elapsed = unmanagedBenchmark.elapsed,
                Deviation = unmanagedBenchmark.deviation,
                Clusters = managedClusters
            };
        }

        public void Dispose()
        {
            clusterAlgorithm.Dispose();
        }
    }
}

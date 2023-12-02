using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Extern
{
    #region compatClasses
    struct Rectangle
    {
        public double x, y;
        public double w, h;
    };

    struct Point
    {
        public double x, y;
    }

    struct ImageNode
    {
        public IntPtr fileName; // Points to a char array (C-style string) in unmanaged memory
        public Point pos;
    }

    struct ImgArray
    {
        public IntPtr head; // Points to an array of ImageNode in unmanaged memory
        public int count;
    };

    struct ClusterArray
    {
        public IntPtr head; // Points to an array of ImageCluster in unmanaged memory
        public int count;
    };

    struct ImageCluster
    {
        public ImageNode repr;
        public Point pos;
        public int count;
    };

    struct Benchmark
    {
        public int compareCnt;
        public int maxNodes;
        public int elapsed;
        public double deviation;
        public ClusterArray clusters;
    }
    #endregion


    internal class ClusterAlgorithmWrapper
    {
        // Define the library names
        public const string LibraryName0 = "libDefaultAlgorithm.so"; // and so on for other libraries

        // Define delegates matching the signatures of the C++ functions
        [DllImport(LibraryName0, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr CreateAlgoClass();

        [DllImport(LibraryName0, CallingConvention = CallingConvention.Cdecl)]
        public static extern Benchmark InitAlgorithm(ImgArray imageList, uint screenWidth, uint screenHeight, IntPtr algoClass);

        [DllImport(LibraryName0, CallingConvention = CallingConvention.Cdecl)]
        public static extern Benchmark IterateAlgorithm(IntPtr algoClass, Rectangle screenRegion);

        [DllImport(LibraryName0, CallingConvention = CallingConvention.Cdecl)]
        public static extern void DeleteAlgorithm(IntPtr algoClass);

        public IntPtr _algorithmInstance;

        public ClusterAlgorithmWrapper()
        {
            _algorithmInstance = CreateAlgoClass();
        }

        public Benchmark Init(ImgArray imageList, uint screenWidth, uint screenHeight)
        {
            return InitAlgorithm(imageList, screenWidth, screenHeight, _algorithmInstance);
        }

        public Benchmark Iterate(Rectangle screenRegion)
        {
            return IterateAlgorithm(_algorithmInstance, screenRegion);
        }

        public void Dispose()
        {
            if (_algorithmInstance != IntPtr.Zero)
            {
                DeleteAlgorithm(_algorithmInstance);
                _algorithmInstance = IntPtr.Zero;
            }
        }


        public static IntPtr MarshalImgArrayToUnmanaged(List<ImageNode> imageNodes)
        {
            var sizeOfImageNode = Marshal.SizeOf(typeof(ImageNode));
            var ptr = Marshal.AllocHGlobal(sizeOfImageNode * imageNodes.Count);

            for (int i = 0; i < imageNodes.Count; i++)
            {
                Marshal.StructureToPtr(imageNodes[i], IntPtr.Add(ptr, i * sizeOfImageNode), false);
            }

            return ptr;
        }

        public static List<ImageNode> MarshalImgArrayToManaged(IntPtr ptr, int count)
        {
            var sizeOfImageNode = Marshal.SizeOf(typeof(ImageNode));
            var imageNodes = new List<ImageNode>();

            for (int i = 0; i < count; i++)
            {
                imageNodes.Add(Marshal.PtrToStructure<ImageNode>(IntPtr.Add(ptr, i * sizeOfImageNode)));
            }

            Marshal.FreeHGlobal(ptr);
            return imageNodes;
        }

        public static IntPtr MarshalClusterArrayToUnmanaged(List<ImageCluster> clusters)
        {
            var sizeOfImageCluster = Marshal.SizeOf(typeof(ImageCluster));
            var ptr = Marshal.AllocHGlobal(sizeOfImageCluster * clusters.Count);

            for (int i = 0; i < clusters.Count; i++)
            {
                Marshal.StructureToPtr(clusters[i], IntPtr.Add(ptr, i * sizeOfImageCluster), false);
            }

            return ptr;
        }

        public static List<ImageCluster> MarshalClusterArrayToManaged(IntPtr ptr, int count)
        {
            var sizeOfImageCluster = Marshal.SizeOf(typeof(ImageCluster));
            var clusters = new List<ImageCluster>();

            for (int i = 0; i < count; i++)
            {
                clusters.Add(Marshal.PtrToStructure<ImageCluster>(IntPtr.Add(ptr, i * sizeOfImageCluster)));
            }

            Marshal.FreeHGlobal(ptr);
            return clusters;
        }

        public static IntPtr StringToUnmanagedString(string str)
        {
            if (str == null)
            {
                return IntPtr.Zero;
            }

            // Allocate unmanaged memory and copy the string (including null terminator)
            int byteCount = Encoding.UTF8.GetByteCount(str) + 1;
            IntPtr unmanagedString = Marshal.AllocHGlobal(byteCount);
            Marshal.Copy(Encoding.UTF8.GetBytes(str + '\0'), 0, unmanagedString, byteCount);

            return unmanagedString;
        }

        public static string UnmanagedStringToString(IntPtr unmanagedString)
        {
            if (unmanagedString == IntPtr.Zero)
            {
                return null;
            }

            // Determine the length of the C-style string
            int length = 0;
            while (Marshal.ReadByte(unmanagedString, length) != 0)
            {
                length++;
            }

            byte[] buffer = new byte[length];
            Marshal.Copy(unmanagedString, buffer, 0, length);

            return Encoding.UTF8.GetString(buffer);
        }

    }
}

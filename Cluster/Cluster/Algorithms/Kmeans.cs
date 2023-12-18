using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Extern
{
    internal partial class Kmeans : ClusterAlgorithmWrapper
    {
#if ANDROID
        public const string LibraryName0 = "libKMEANS.so"; // and so on for other libraries
#elif IOS
        public const string LibraryName0 = "KMEANS";
#else
        public const string LibraryName0 = "KMEANS.dll";
#endif
        [LibraryImport(LibraryName0)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        public static partial SAMPLE testFunc(double p1, double p2);

        // Define delegates matching the signatures of the C++ functions
        [LibraryImport(LibraryName0)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        public static partial IntPtr createAlgoClass();

        [LibraryImport(LibraryName0)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        public static partial Benchmark InitAlgorithm(ImgArray imageList, uint screenWidth, uint screenHeight, IntPtr algoClass);

        [LibraryImport(LibraryName0)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        public static partial Benchmark IterateAlgorithm(IntPtr algoClass, Rectangle screenRegion);

        [LibraryImport(LibraryName0)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        public static partial void DeleteAlgorithm(IntPtr algoClass);

        [LibraryImport(LibraryName0)]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        public static partial int noTest(IntPtr algoClass);

        public Kmeans()
        {
            _algorithmInstance = createAlgoClass();
        }

        public override Benchmark Init(ImgArray imageList, uint screenWidth, uint screenHeight)
        {
            return InitAlgorithm(imageList, screenWidth, screenHeight, _algorithmInstance);
        }

        public override Benchmark Iterate(Rectangle screenRegion)
        {
            return IterateAlgorithm(_algorithmInstance, screenRegion);
        }

        public override void Dispose()
        {
            if (_algorithmInstance != IntPtr.Zero)
            {
                DeleteAlgorithm(_algorithmInstance);
                _algorithmInstance = IntPtr.Zero;
            }
        }
    }
}

#include "library.h"
#include <fstream>

void SaveBitmapToBMP(const std::vector<uint8_t>& bitmap, unsigned int width, unsigned int height, const std::string& filename) {
    const uint32_t pixelsSize = width * height * 3;
    const uint32_t paddingAmount = (4 - (width * 3) % 4) % 4;
    const uint32_t scanlineSize = width * 3 + paddingAmount;
    const uint32_t fileSize = 14 + 40 + scanlineSize * height; // File header + DIB header + Pixel data

    std::ofstream file(filename, std::ios::out | std::ios::binary);

    if (!file) {
        std::cerr << "Could not open the file for writing.\n";
        return;
    }

    // BMP Header
    unsigned char fileHeader[14] = {
        'B', 'M',             // Signature
        0, 0, 0, 0,           // Image file size in bytes
        0, 0, 0, 0,           // Reserved
        54, 0, 0, 0           // Start of pixel array
    };

    // DIB Header
    unsigned char dibHeader[40] = {
        40, 0, 0, 0,          // DIB Header size
        0, 0, 0, 0,           // Image width
        0, 0, 0, 0,           // Image height
        1, 0,                 // Number of color planes
        24, 0,                // Bits per pixel
        0, 0, 0, 0,           // Compression
        0, 0, 0, 0,           // Image size (can be zero for no compression)
        0x13, 0x0B, 0, 0,     // Horizontal resolution (pixels per meter)
        0x13, 0x0B, 0, 0,     // Vertical resolution (pixels per meter)
        0, 0, 0, 0,           // Colors in color table
        0, 0, 0, 0            // Important color count
    };

    std::memcpy(&fileHeader[2], &fileSize, 4);
    std::memcpy(&dibHeader[4], &width, 4);
    int32_t negativeHeight = -static_cast<int32_t>(height); // Negative to indicate top-down bitmap
    std::memcpy(&dibHeader[8], &negativeHeight, 4);

    file.write(reinterpret_cast<char*>(fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<char*>(dibHeader), sizeof(dibHeader));

    // Write pixel data
    for (uint32_t y = 0; y < height; ++y) {
        file.write(reinterpret_cast<const char*>(&bitmap[y * width * 3]), width * 3);
        file.write(reinterpret_cast<const char*>(paddingAmount ? "\0\0\0" : ""), paddingAmount);
    }

    file.close();
}

int main() {
    int seed = 12345; // Example seed
    unsigned int width = 800; // Example width
    unsigned int height = 600; // Example height
    int divCap = 5; // Example divCap

    // Create an instance of MapRenderer
    MapRenderer mapRenderer(seed, width, height, divCap);

    mapRenderer.RenderBitmap();

    // Save the bitmap to a BMP file
    SaveBitmapToBMP(mapRenderer.FullBitmap, width, height, "output.bmp");

    std::cout << "Map rendering complete and saved to output.bmp" << std::endl;
    return 0;
}
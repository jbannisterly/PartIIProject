#include "image_debug.hpp"

DebugImage::DebugImage(int inWidth, int inHeight) :
width {inWidth},
height {inHeight}
{
    debugImage.resize(width * height * 3, 0);
}

void DebugImage::DebugCross(int x, int y, int width, int crossSize, std::array<uint8_t, 3> colour) {
    for (int i = -crossSize; i < crossSize; i++) {
        for (int j = 0; j < 3; j++) {
            debugImage[(x + y * width + i) * 3 + j] = colour[j];
            debugImage[(x + (y + i) * width) * 3 + j] = colour[j];
        }
    }
}

void DebugImage::DebugCentre(FinderCandidate finder, std::array<uint8_t, 3> colour) {
    for (int i = -finder.width / 2; i < finder.width; i++) {
        for (int j = 0; j < 3; j++) {
            debugImage[(int(finder.x) + int(finder.y) * width + i) * 3 + j] = colour[j];
        }
    }
}

std::vector<uint8_t> DebugImage::Uint8ToPixels(std::vector<uint8_t> &data){
    std::vector<uint8_t> pixels;
    pixels.reserve(data.size() * 3);

    for (int i = 0; i < data.size(); i++){
        if (debugImage[i] == 0){
            pixels[i * 3] = data[i * 3];
            pixels[i * 3 + 1] = data[i * 3 + 1];
            pixels[i * 3 + 2] = data[i * 3 + 2];
        }else{
            pixels[i * 3 + 0] = debugImage[i * 3];
            pixels[i * 3 + 1] = debugImage[i * 3 + 1];
            pixels[i * 3 + 2] = debugImage[i * 3 + 2];
        }
    }

    return pixels;
}
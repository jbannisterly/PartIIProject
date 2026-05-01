#include "image_debug.hpp"

DebugImage::DebugImage(int inWidth, int inHeight) :
width {inWidth},
height {inHeight}
{
    debugImage.resize(width * height * 3, 0);
    debugMask.resize(width * height, 0);
}

void DebugImage::DebugCross(int x, int y, int width, int crossSize, std::array<uint8_t, 3> colour) {
    for (int i = -crossSize; i < crossSize; i++) {
        for (int j = 0; j < 3; j++) {
            debugImage[(x + y * width + i) * 3 + j] = colour[j];
            debugImage[(x + (y + i) * width) * 3 + j] = colour[j];
        }
        debugMask[(x + y * width + i)] = 1;
        debugMask[(x + (y + i) * width)] = 1;
    }
}

void DebugImage::DebugCentre(FinderCandidate finder, std::array<uint8_t, 3> colour) {
    for (int i = -finder.width / 2; i < finder.width; i++) {
        for (int j = 0; j < 3; j++) {
            debugImage[(int(finder.x) + int(finder.y) * width + i) * 3 + j] = colour[j];
        }  
        debugMask[(int(finder.x) + int(finder.y) * width + i)] = 1;
    }
}

std::vector<uint8_t> DebugImage::BackgroundImage(std::vector<uint8_t> &data){
    std::vector<uint8_t> pixels(debugImage);

    for (int i = 0; i < debugMask.size(); i++){
        if (debugMask[i] == 0){
            pixels[i * 3] = data[i * 3];
            pixels[i * 3 + 1] = data[i * 3 + 1];
            pixels[i * 3 + 2] = data[i * 3 + 2];
        }
    }

    return pixels;
}

void DebugImage::WriteImage(std::string path, std::vector<uint8_t> &background) {
    std::cout << "Attempting to write to " << path << std::endl;

    std::vector<uint8_t> output = BackgroundImage(background);

    cv::Mat debugMat(height, width, CV_8UC3, output.data());
    
    if (path != "") {
        std::cout << "debugg ing image print" << std::endl;
        cv::imwrite(path, debugMat);
    }

    std::cout << "Written to " << path << std::endl;
}
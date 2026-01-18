#include <vector>
#include <cstdint>
#include <array>
#include "finder_patterns.hpp"
#include <opencv2/opencv.hpp>

class DebugImage {
    private:

    std::vector<uint8_t> debugImage; 
    std::vector<uint8_t> debugMask;
    int height;
    int width;

    public:

    DebugImage(int inWidth, int inHeight);
    void DebugCross(int x, int y, int width, int crossSize, std::array<uint8_t, 3> colour = {255, 0, 0});
    void DebugCentre(FinderCandidate finder, std::array<uint8_t, 3> colour = {0, 255, 0});
    std::vector<uint8_t> BackgroundImage(std::vector<uint8_t> &data);
    void WriteImage(std::string path, std::vector<uint8_t> &background);
};
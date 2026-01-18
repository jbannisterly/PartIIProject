#include <vector>
#include <cstdint>
#include <opencv2/opencv.hpp>

namespace ImageProcessing {
    std::vector<double> Greyscale(std::vector<uint8_t> &image, int pixels);
    std::vector<double> GetBlurred(std::vector<double> &imageData, int nY, int nX);
    std::vector<uint8_t> Threshold(std::vector<double> &image, int nY, int nX);
}
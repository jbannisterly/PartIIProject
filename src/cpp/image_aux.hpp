#include <opencv2/opencv.hpp>

namespace ImageAux {
    std::vector<uint8_t> MatToBytes(cv::Mat image);
    cv::Mat Project(cv::Mat input, std::array<int, 8> inputCoords, cv::Size size);
}
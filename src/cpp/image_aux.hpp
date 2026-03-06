#include <opencv2/opencv.hpp>

namespace ImageAux {
    std::vector<uint8_t> MatToBytes(cv::Mat image);
    cv::Mat Project(cv::Mat input, std::array<int, 8> inputCoords, cv::Size size);

    std::vector<uint8_t> Erode(std::vector<uint8_t> &toErode, int width, int height);
    std::vector<uint8_t> Dilate(std::vector<uint8_t> &toDilate, int width, int height);
}
#include "alignment.hpp"

int main() {
    std::string filePath = "output/img/output_distorted.png";
    cv::Mat image = cv::imread(filePath);

    AlignmentData alignData = Alignment::GetBounds(image);

    for (int i = 0; i < 4; i++) {
        std::cout << alignData.bounds[i].x << std::endl;
        std::cout << alignData.bounds[i].y << std::endl;
    }

    std::cout << alignData.estimatedWidth << std::endl;
    std::cout << alignData.estimatedHeight << std::endl;

    return 0;
}
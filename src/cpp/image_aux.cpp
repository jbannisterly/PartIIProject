#include "image_aux.hpp"

using namespace cv;

namespace ImageAux {
    std::vector<uint8_t> MatToBytes(Mat image){
        int channels = image.channels();
        int rows = image.rows;
        int cols = image.cols;
        int totalLength = rows * cols * channels;

        std::vector<uint8_t> rawData;
        rawData.reserve(totalLength);
        uint8_t* rowPointer;

        for (int i = 0; i < rows; i++){
            rowPointer = image.ptr(i);
            for (int j = 0; j < cols * channels; j++){
                rawData.push_back(rowPointer[j]);
            }
        }

        return rawData;
    }

    Mat Project(Mat input, std::array<int, 8> inputCoords, Size size){
        Mat output;

        const int cornerIndices[4] = {1, 2, 0, 3};

        std::cout << "Start Coords" << std::endl;
        Point2f startCoords[4];
        for (int i = 0; i < 4; i++){
            startCoords[i] = Point2f(inputCoords[cornerIndices[i] * 2], inputCoords[cornerIndices[i] * 2 + 1]);
            std::cout << startCoords[i] << std::endl;
        }

        Point2f endCoords[4];
        endCoords[0] = Point2f(0,0);
        endCoords[1] = Point2f(size.width - 1, 0);
        endCoords[2] = Point2f(0, size.height - 1);
        endCoords[3] = Point2f(size.width - 1, size.height - 1);

        Mat transform = getPerspectiveTransform(startCoords, endCoords);

        std::cout << transform << std::endl;

        warpPerspective(input.clone(), output, transform, size);

        return output;
    }
}
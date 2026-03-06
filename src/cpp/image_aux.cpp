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

        return output.clone();
    }


    std::vector<uint8_t>  Erode(std::vector<uint8_t> &toErode, int width, int height) {
        std::vector<uint8_t> result;
        result.reserve(width * height);

        bool filled;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                filled = false;
                if (toErode[i * width + j] > 0) filled = true;
                if (i > 0 && toErode[(i - 1) * width + j] > 0) filled = true;
                if (j > 0 && toErode[i * width + (j - 1)] > 0) filled = true;
                if (i < height - 1 && toErode[(i + 1) * width + j] > 0) filled = true;
                if (j < width - 1 && toErode[i * width + j + 1] > 0) filled = true;

                if (filled) {
                    result.push_back(255);
                } else {
                    result.push_back(0);
                }
            }
        }

        return result;
    }
    
    std::vector<uint8_t> Dilate(std::vector<uint8_t> &toDilate, int width, int height) {
        std::vector<uint8_t> result;
        result.reserve(width * height);

        bool filled;
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                filled = true;
                if (toDilate[i * width + j] == 0) filled = false;
                if (i > 0 && toDilate[(i - 1) * width + j] == 0) filled = false;
                if (j > 0 && toDilate[i * width + (j - 1)] == 0) filled = false;
                if (i < height - 1 && toDilate[(i + 1) * width + j] == 0) filled = false;
                if (j < width - 1 && toDilate[i * width + j + 1] == 0) filled = false;

                if (filled) {
                    result.push_back(255);
                } else {
                    result.push_back(0);
                }
            }
        }

        return result;
    }

}
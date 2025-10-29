#include "image_aux.hpp"

using namespace cv;

uint8_t* MatToBytes(Mat image){
    int channels = image.channels();
    int rows = image.rows;
    int cols = image.cols;
    int totalLength = rows * cols * channels;

    uint8_t* rawData = new uint8_t[totalLength];
    uint8_t* rowPointer;

    for (int i = 0; i < rows; i++){
        rowPointer = image.ptr(i);
        for (int j = 0; j < cols * channels; j++){
            rawData[i * cols * channels + j] = rowPointer[j];
        }
    }

    return rawData;
}
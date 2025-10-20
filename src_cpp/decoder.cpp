#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(){

    Mat image = imread("output/output.png");

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

    uint8_t currentByte;
    uint8_t* bytePointer = new uint8_t[totalLength / 8];

    for (int i = 0; i < totalLength; i++){
        if (i % 3 == 0) {
            currentByte = currentByte << 1 | ((rawData[i] & 128) >> 7);
        }
        if (i % 24 == 21){
            bytePointer[i / 24] = currentByte;
        }
    }

    uint16_t messageLength = ((uint16_t*)bytePointer)[0];

    for (int i = 2; i < messageLength / 8 + 2; i++){
        std::cout << bytePointer[i];
    }
}
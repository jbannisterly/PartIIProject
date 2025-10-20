#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstring>

using namespace cv;

int main(){
    char* message = "Hello World\0";
    uint16_t length = strlen(message);

    uint8_t* byteData = new uint8_t[length + 2];
    
    ((uint16_t*)byteData)[0] = length << 3; // convert to number of bits
    std::memcpy(byteData + 2, message, length);

    uint8_t* rawData = new uint8_t[25 * 25 * 3];

    for (int i = 0; i < length + 2; i++){
        uint8_t currentByte = byteData[i];
        for (int j = 0; j < 8; j++){
            uint8_t data = (currentByte & 128) ? 255 : 0;
            currentByte <<= 1;
            for (int k = 0; k < 3; k++){
                rawData[i * 24 + j * 3 + k] = data;
            }
        }
    }

    Mat image (25, 25, CV_8UC3);
    image.data = rawData;

    imwrite("output/output.png", image);
}
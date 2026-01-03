#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstring>
#include "barcode_layout.hpp"
#include "compressor.hpp"
#include "error_correction_schifra.hpp"
#include <vector>
#include "_config.hpp"

using namespace cv;

std::vector<uint8_t> EncodeMessage(char* message){
    uint16_t length = strlen(message);

    uint8_t* rawData = new uint8_t[length];

    memcpy(rawData, message, length);

    std::vector<uint8_t> compressedData = Compression::compress(rawData, length);

    ErrorCorrection<64, 16> error;
    std::vector<uint8_t> errorCorrectionData = error.Encode(compressedData);
    
    uint8_t* byteData = new uint8_t[errorCorrectionData.size()];
    ((uint16_t*)byteData)[0] = compressedData.size() << 3; // convert to number of bits
    std::memcpy(byteData + 2, errorCorrectionData.data(), errorCorrectionData.size());

    std::vector vecData(byteData, byteData + errorCorrectionData.size());

    return vecData;
}

struct PixelData{
    uint8_t* data;
    int length;
};

PixelData* MessageToPixels(uint8_t* byteData, uint16_t length){
    PixelData* pixelData = new PixelData();

    pixelData->data = (uint8_t*)malloc(length * 24 * sizeof(uint8_t));
    pixelData->length = length * 24;

    for (int i = 0; i < length; i++){
        uint8_t currentByte = byteData[i];
        for (int j = 0; j < 8; j++){
            uint8_t data = (currentByte & 128) ? 255 : 0;
            currentByte <<= 1;
            for (int k = 0; k < 3; k++){
                pixelData->data[i * 24 + j * 3 + k] = data;
            }
        }
    }

    return pixelData;
}

uint8_t* PixelsToBarcode(PixelData* pixels){
    int size = BARCODE_SIZE * BARCODE_SIZE * 3;
    BarcodeLayout* barcode = GetBarcode();

    int sourceCounter = 0;
    int targetCounter = 0;

    while(sourceCounter < pixels->length){
        if (barcode->mask[targetCounter] > 0){
            memcpy(barcode->data + targetCounter * 3, pixels->data + sourceCounter, 3);
            sourceCounter += 3;
        }
        targetCounter++;
    }

    free(barcode->mask);

    return barcode->data;
}



int main(){
    char* message = "Hello World 1\nHello World 2\nHello World 3\n";

    std::vector<uint8_t> byteData = EncodeMessage(message);
    PixelData* pixelData = MessageToPixels(byteData.data(), byteData.size());
    uint8_t* imageData = PixelsToBarcode(pixelData);

    Mat image(BARCODE_SIZE, BARCODE_SIZE, CV_8UC3);
    image.data = imageData;

    imwrite("output/img/output.png", image);
}
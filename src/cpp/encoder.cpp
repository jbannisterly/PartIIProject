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

std::vector<std::vector<uint8_t>> SplitBytes(std::vector<uint8_t> &data, std::vector<int> &errorCorrectionLevels, int errorCorrectionChunkSize) {
    std::vector<std::vector<uint8_t>> splitBytes;
    splitBytes.reserve(errorCorrectionLevels.size());
    
    int bytesPerChunk = 0;
    
    for (int i = 0; i < errorCorrectionLevels.size(); i++) {
        bytesPerChunk += errorCorrectionChunkSize - errorCorrectionLevels[i];
    }

    int nChunks = int(ceil(data.size() / (float)bytesPerChunk));

    int startIndex = 0;
    int endIndex = 0;

    for (int i = 0; i < errorCorrectionLevels.size(); i++) {
        int bytesPerSplit = (errorCorrectionChunkSize - errorCorrectionLevels[i]) * nChunks;
        endIndex = startIndex + bytesPerSplit;

        std::cout << bytesPerSplit << std::endl;
        std::cout << endIndex << std::endl;

        if (endIndex > data.size()) {
            endIndex = data.size();
            std::cout << "Too big" << std::endl;
        }
        std::vector<uint8_t> splitData(data.cbegin() + startIndex, data.cbegin() + endIndex);

        splitBytes.push_back(splitData);
        startIndex = endIndex;
    }

    return splitBytes;
}

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

std::vector<uint8_t> MessageToPixels(std::vector<uint8_t> byteData) {
    std::vector<uint8_t> pixelData;

    pixelData.reserve(byteData.size() * 24);

    for (int i = 0; i < byteData.size(); i++){
        uint8_t currentByte = byteData[i];
        for (int j = 0; j < 8; j++){
            uint8_t data = (currentByte & 128) ? 255 : 0;
            currentByte <<= 1;
            for (int k = 0; k < 3; k++){
                pixelData.push_back(data);
            }
        }
    }

    return pixelData;
}

std::vector<uint8_t> PixelsToBarcode(std::vector<uint8_t> pixels){
    int size = BARCODE_SIZE * BARCODE_SIZE * 3;
    BarcodeLayout barcode = GetBarcode();

    int sourceCounter = 0;
    int targetCounter = 0;

    while(sourceCounter < pixels.size()){
        if (barcode.mask[targetCounter] > 0){
            for (int i = 0; i < 3; i++) {
                barcode.data[targetCounter * 3 + i] = pixels[sourceCounter];
                sourceCounter++;
            }
        }
        targetCounter++;
    }

    return barcode.data;
}

int main(){
    char* message = "Hello World 1\nHello World 2\nHello World 3\n";

    std::vector<uint8_t> byteData = EncodeMessage(message);
    std::vector<uint8_t> pixelData = MessageToPixels(byteData);
    std::vector<uint8_t> imageData = PixelsToBarcode(pixelData);

    Mat image(BARCODE_SIZE, BARCODE_SIZE, CV_8UC3);
    image.data = imageData.data();

    imwrite("output/img/output.png", image);
}
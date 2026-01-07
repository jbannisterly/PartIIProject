#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstring>
#include "barcode_layout.hpp"
#include "compressor.hpp"
#include "error_correction_schifra.hpp"
#include <vector>
#include "colours.hpp"
#include "_config.hpp"

using namespace cv;

std::vector<std::vector<uint8_t>> ErrorCorrectionSplit(std::vector<std::vector<uint8_t>> &splitBytes, std::vector<ErrorCorrectionVirtual*> &errorCorrectors) {
    std::vector<std::vector<uint8_t>> splitError;
    splitError.reserve(splitBytes.size());
    
    for (int i = 0; i < splitBytes.size(); i++) {
        splitError.push_back(errorCorrectors[i]->Encode(splitBytes[i]));
    }

    return splitError;
}

std::vector<std::vector<uint8_t>> SplitBytes(std::vector<uint8_t> &data, std::vector<ErrorCorrectionVirtual*> &errorCorrectors) {
    std::vector<std::vector<uint8_t>> splitBytes;
    splitBytes.reserve(errorCorrectors.size());
    
    int bytesPerChunk = 0;
    
    for (int i = 0; i < errorCorrectors.size(); i++) {
        bytesPerChunk += errorCorrectors[i]->getDataLen();
    }

    int nChunks = int(ceil(data.size() / (float)bytesPerChunk));

    int startIndex = 0;
    int endIndex = 0;
    int padding = 0;

    for (int i = 0; i < errorCorrectors.size(); i++) {
        int bytesPerSplit = (errorCorrectors[i]->getDataLen()) * nChunks;
        endIndex = startIndex + bytesPerSplit;

        std::cout << bytesPerSplit << std::endl;
        std::cout << endIndex << std::endl;

        if (endIndex > data.size()) {
            padding = data.size() - endIndex;
            endIndex = data.size();
            std::cout << "Too big" << std::endl;
        }
        std::vector<uint8_t> splitData(data.cbegin() + startIndex, data.cbegin() + endIndex);
        for (int j = 0; j < padding; j++) {
            splitData.push_back(0);
        }

        splitBytes.push_back(splitData);
        startIndex = endIndex;
    }

    return splitBytes;
}

std::vector<std::vector<uint8_t>> EncodeMessage(std::vector<uint8_t> rawData, std::vector<ErrorCorrectionVirtual*> &errorCorrectors){
    // uint16_t length = strlen(message);

    // uint8_t* rawData = new uint8_t[length];

    // memcpy(rawData, message, length);

    std::vector<uint8_t> compressedData = Compression::compress(rawData);

    std::cout << "Compressed length " << compressedData.size() << std::endl; 
    
    compressedData.insert(compressedData.begin(), {
        (compressedData.size() >> 0) & 255,
        (compressedData.size() >> 8) & 255
    });

    std::vector<std::vector<uint8_t>> splitData = SplitBytes(compressedData, errorCorrectors);

    std::cout << "Split length " << splitData[0].size() << std::endl;

    std::vector<std::vector<uint8_t>> errorSplitData = ErrorCorrectionSplit(splitData,errorCorrectors);

    std::cout << "Error length " << errorSplitData[0].size() << std::endl;

    return errorSplitData;
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

    std::vector<ErrorCorrectionVirtual*> errorCorrectors;
    errorCorrectors.push_back(new ErrorCorrection<16, 4>());
    errorCorrectors.push_back(new ErrorCorrection<16, 4>());
    errorCorrectors.push_back(new ErrorCorrection<16, 4>());

    // std::vector<uint8_t> byteData = EncodeMessage(message);

    std::vector<uint8_t> byteData;
    for (int i = 0; i < 100; i++) {
        byteData.push_back(i);
    }

    std::vector<std::vector<uint8_t>> encodedData = EncodeMessage(byteData, errorCorrectors);
    
    // std::vector<std::vector<uint8_t> pixelData = MessageToPixels(byteData, errorCorrectors);
    std::vector<Colour> colours = {
        Colour(0, 0, 0),
        Colour(0, 0, 255),
        Colour(255, 0, 0),
        Colour(0, 255, 0),
        Colour(255, 0, 255),
        Colour(0, 255, 255),
        Colour(255, 255, 0),
        Colour(255, 255, 255),
    };
    ColourScheme colourScheme(colours);
    ColourPixels colourPix(colourScheme);
    std::vector<uint8_t> pixelData = colourPix.DataToPixels(encodedData);
    std::vector<uint8_t> imageData = PixelsToBarcode(pixelData);

    Mat image(BARCODE_SIZE, BARCODE_SIZE, CV_8UC3);
    image.data = imageData.data();

    imwrite("output/img/output.png", image);
}
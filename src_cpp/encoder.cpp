#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstring>

using namespace cv;

uint8_t* EncodeMessage(char* message){
    uint16_t length = strlen(message);

    uint8_t* byteData = new uint8_t[length + 2];
    
    ((uint16_t*)byteData)[0] = length << 3; // convert to number of bits
    std::memcpy(byteData + 2, message, length);

    return byteData;
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

struct BarcodeLayout{
    uint8_t* data;
    uint8_t* mask;
};

void DrawSquare(BarcodeLayout barcode, uint8_t colour, int positionX, int positionY, int barcodeWidth, int squareSize){
    for (int i = 0; i < squareSize; i++){
        int offset = ((i + positionY) * barcodeWidth + positionX);
        memset(barcode.data + offset * 3, colour, squareSize * 3);
        memset(barcode.mask + offset, 0, squareSize);
    }
}

BarcodeLayout* GetBarcode(){
    BarcodeLayout* barcode = new BarcodeLayout();
    int size = 25 * 25;
    barcode->data = (uint8_t*)malloc(sizeof(uint8_t) * size * 3);
    barcode->mask = (uint8_t*)malloc(sizeof(uint8_t) * size);

    memset(barcode->data, 0, size * 3);
    memset(barcode->mask, 255, size);

    int posX[3] = {0, 18, 0};
    int posY[3] = {0, 0, 18};

    DrawSquare(*barcode, 255, 0, 0, 25, 8);
    DrawSquare(*barcode, 255, 17, 0, 25, 8);
    DrawSquare(*barcode, 255, 0, 17, 25, 8);


    for (int i = 0; i < 3; i++){
        DrawSquare(*barcode, 0, posX[i], posY[i], 25, 7);
        DrawSquare(*barcode, 255, posX[i] + 1, posY[i] + 1, 25, 5);
        DrawSquare(*barcode, 0, posX[i] + 2, posY[i] + 2, 25, 3);
    }

    return barcode;
}

uint8_t* PixelsToBarcode(PixelData* pixels){
    int size = 25 * 25 * 3;
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
    char* message = "Hello World\n";

    uint8_t* byteData = EncodeMessage(message);
    PixelData* pixelData = MessageToPixels(byteData, strlen(message) + 2);
    uint8_t* imageData = PixelsToBarcode(pixelData);

    Mat image(25, 25, CV_8UC3);
    image.data = imageData;

    imwrite("output/output.png", image);
}
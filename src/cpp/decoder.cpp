#include <iostream>
#include <opencv2/opencv.hpp>
#include "barcode_layout.hpp"
#include "image_aux.hpp"
#include "compressor.hpp"
#include "error_correction_schifra.hpp"

using namespace cv;

std::vector<uint8_t> BarcodeToPixels(std::vector<uint8_t> barcode, int barcodeSize){
    BarcodeLayout barcodeLayout = GetBarcode();
    std::vector<uint8_t> pixels;
    pixels.reserve(barcodeSize * 3);
    int pixelCounter = 0;

    for (int i = 0; i < barcodeSize; i++){
        if (barcodeLayout.mask[i] > 0){
            for (int j = 0; j < 3; j++) {
                pixels.push_back(barcode[i * 3 + j]);
            }
        } 
    }

    return pixels;
}

int main(){
    try {
        Mat image = imread("output/img/output_align.png");

        int channels = image.channels();
        int rows = image.rows;
        int cols = image.cols;
        int totalLength = rows * cols * channels;

        std::vector<uint8_t> imageBytes = MatToBytes(image);
        std::vector<uint8_t> rawData = BarcodeToPixels(imageBytes, totalLength);

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

        uint16_t messageLength = ((uint16_t*)bytePointer)[0] / 8;
        uint16_t errorCorrectionMessageLength = std::ceil((float)messageLength / 64) * 64;

        std::vector<uint8_t> vectorData(bytePointer + 2, bytePointer + 2 + errorCorrectionMessageLength);
        ErrorCorrection<64, 16> errorCorrector;
        std::vector<uint8_t> deErrored = errorCorrector.Decode(vectorData);
        std::vector<uint8_t> decompressed = Compression::decompress(deErrored.data(), messageLength); 

        for (int i = 0; i < decompressed.size(); i++){
            std::cout << (char)decompressed[i];
        }

    } catch (const ExceptionDecompression& e){
        std::cout << e.what() << std::endl;
    }
}
#include <iostream>
#include <opencv2/opencv.hpp>
#include "barcode_layout.hpp"
#include "image_aux.hpp"
#include "compressor.hpp"
#include "error_correction_schifra.hpp"
#include "colours.hpp"

using namespace cv;

int GetErrorCorrectionLen(int compressedLen, std::vector<ErrorCorrectionVirtual*> errorCorrectors) {
    int bytesPerChunk = 0;
    
    for (int i = 0; i < errorCorrectors.size(); i++) {
        bytesPerChunk += errorCorrectors[i]->getDataLen();
    }

    int nChunks = int(ceil(compressedLen / (float)bytesPerChunk));

    return nChunks * errorCorrectors[0]->getBlockLen();
}

int GetCompressedLen(std::vector<uint8_t> &pixels, ColourPixels colourPix, ErrorCorrectionVirtual* errorCorrection) {
    std::vector<std::vector<uint8_t>> header = colourPix.PixelsToData(pixels, 0, errorCorrection->getBlockLen());
    std::vector<uint8_t> headerCorrected = errorCorrection->Decode(header[0]);
    
    return ((int)headerCorrected[0] + ((int)headerCorrected[1]) << 8);
}

std::vector<std::vector<uint8_t>> ErrorDecodeSplit(std::vector<std::vector<uint8_t>> &splitBytes, std::vector<ErrorCorrectionVirtual*> &errorCorrectors) {
    std::vector<std::vector<uint8_t>> splitError;
    splitError.reserve(splitBytes.size());
    
    for (int i = 0; i < splitBytes.size(); i++) {
        splitError.push_back(errorCorrectors[i]->Decode(splitBytes[i]));
    }

    return splitError;
}

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

    std::vector<Colour> colours = {
        Colour(0, 0, 0),
        Colour(255, 255, 255),
        Colour(255, 0, 0),
        Colour(255, 255, 0),
        Colour(0, 255, 0),
        Colour(255, 0, 255),
        Colour(0, 0, 255),
        Colour(0, 255, 255),
    };
        ColourScheme colourScheme(colours);
        ColourPixels colourPix(colourScheme);

        std::vector<ErrorCorrectionVirtual*> errorCorrectors;
        errorCorrectors.push_back(new ErrorCorrection<16, 4>());
        errorCorrectors.push_back(new ErrorCorrection<16, 4>());
        errorCorrectors.push_back(new ErrorCorrection<16, 4>());

        int compressedLen = GetCompressedLen(rawData, colourPix, errorCorrectors[0]);
        int nPixels = GetErrorCorrectionLen(compressedLen, errorCorrectors);

        std::cout << "compressed len " << compressedLen << std::endl;
        std::cout << "nPixels " << nPixels << std::endl;

        // std::vector<std::vector<uint8_t>> splitData = colourPix.PixelsToData(rawData);

        // std::cout << "sd len " << splitData[0].size();


        // std::vector<std::vector<uint8_t>> correctedSplitData = ErrorDecodeSplit(splitData, errorCorrectors);

        // uint16_t messageLength = ((uint16_t*)bytePointer)[0] / 8;
        // uint16_t errorCorrectionMessageLength = std::ceil((float)messageLength / 64) * 64;

        // std::vector<uint8_t> vectorData(bytePointer + 2, bytePointer + 2 + errorCorrectionMessageLength);
        // ErrorCorrection<64, 16> errorCorrector;
        // std::vector<uint8_t> deErrored = errorCorrector.Decode(vectorData);


        // std::vector<uint8_t> decompressed = Compression::decompress(deErrored.data(), messageLength); 

        // for (int i = 0; i < decompressed.size(); i++){
        //     std::cout << (char)decompressed[i];
        // }

    } catch (const ExceptionDecompression& e){
        std::cout << e.what() << std::endl;
    }
}
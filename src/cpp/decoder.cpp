#include <iostream>
#include <opencv2/opencv.hpp>
#include "barcode_layout.hpp"
#include "image_aux.hpp"
#include "compressor.hpp"
#include "colours.hpp"
#include <fstream>
#include "split_error.hpp"
#include "split_bytes.hpp"
#include "barcode_writer.hpp"
#include "colour_palletes.hpp"
#include "error_layout.hpp"

using namespace cv;

int GetErrorCorrectionLen(int compressedLen, std::vector<ErrorCorrectionVirtual*> errorCorrectors) {
    int bytesPerChunk = 0;
    
    for (int i = 0; i < errorCorrectors.size(); i++) {
        bytesPerChunk += errorCorrectors[i]->getDataLen();
    }

    std::cout << "bytes per chunk " << bytesPerChunk << std::endl;

    int nChunks = int(ceil(compressedLen / (float)bytesPerChunk));

    return nChunks * errorCorrectors[0]->getBlockLen();
}

int GetCompressedLen(std::vector<uint8_t> &pixels, ColourPixels colourPix, ErrorCorrectionVirtual* errorCorrection) {
    std::vector<std::vector<uint8_t>> header = colourPix.PixelsToData(pixels, 0, errorCorrection->getBlockLen());
    std::vector<uint8_t> headerCorrected = errorCorrection->Decode(header[0]);
    
    return ((int)headerCorrected[0] | ((int)headerCorrected[1]) << 8) + 2;
}

int main(){
    try {
        Mat image = imread("output/img/output_align.png");

        int channels = image.channels();
        int rows = image.rows;
        int cols = image.cols;
        int totalLength = rows * cols * channels;

        std::vector<uint8_t> imageBytes = MatToBytes(image);

        BarcodeWriter writer(GetBarcode());
        std::vector<uint8_t> rawData = writer.BarcodeToPixels(imageBytes, totalLength);

        ColourPixels colourPix = ColourPalletes::Bit_3();
        std::vector<ErrorCorrectionVirtual*> errorCorrectors = ErrorLayout::Bit_3();

        int compressedLen = GetCompressedLen(rawData, colourPix, errorCorrectors[0]);
        int nPixels = GetErrorCorrectionLen(compressedLen, errorCorrectors);

        std::vector<std::vector<uint8_t>> splitData = colourPix.PixelsToData(rawData, 0, nPixels);

        SplitError splitError(errorCorrectors);
        std::vector<std::vector<uint8_t>> correctedSplitData = splitError.Decode(splitData);

        std::vector connectedData = SplitBytes::Decode(correctedSplitData, compressedLen);

        std::vector<uint8_t> decompressed = Compression::decompress(connectedData.data() + 2, compressedLen - 2); 

        for (int i = 0; i < decompressed.size(); i++){
            std::cout << (char)decompressed[i];
        }

    } catch (const ExceptionDecompression& e){
        std::cout << e.what() << std::endl;
    }
}
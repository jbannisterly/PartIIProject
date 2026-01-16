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
#include "header_data.hpp"

using namespace cv;

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

        int compressedLen = HeaderData::GetCompressedLen(rawData, colourPix, errorCorrectors[0]);
        int nPixels = HeaderData::GetErrorCorrectionLen(compressedLen, errorCorrectors);

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
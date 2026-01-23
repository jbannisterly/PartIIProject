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
#include "alignment.hpp"
#include <string>

using namespace cv;

int main(int argc, char* argv[]){
        Mat inputImage = imread("output/img/output_distorted2.png", IMREAD_COLOR);
        std::array<int, 8> inputCoords;

        for (int i = 0; i < 8; i++) {
            inputCoords[i] = std::stod(std::string(argv[i + 1]));
            std::cout << inputImage.channels() << std::endl;
        }
        Size imageSize(std::stod(std::string(argv[9])), std::stod(std::string(argv[10])));
        std::cout << inputImage.channels() << std::endl;

        Mat image = ImageAux::Project(inputImage, inputCoords, imageSize);

        std::cout << image.channels() << std::endl;

        imwrite("./output/img/image_test.png", image);

        int channels = image.channels();
        int rows = image.rows;
        int cols = image.cols;
        int totalLength = rows * cols * channels;

        std::vector<uint8_t> imageBytes = ImageAux::MatToBytes(image);

        BarcodeWriter writer(GetBarcode4Detailed());
        std::vector<uint8_t> rawData = writer.BarcodeToPixels(imageBytes, totalLength);

        ColourPixels colourPix = ColourPalletes::Bit_3();
        std::vector<ErrorCorrectionVirtual*> errorCorrectors = ErrorLayout::Bit_3();

        int compressedLen = HeaderData::GetCompressedLen(rawData, colourPix, errorCorrectors[0]);
        int nPixels = HeaderData::GetErrorCorrectionLen(compressedLen, errorCorrectors);

        std::vector<std::vector<uint8_t>> splitData = colourPix.PixelsToData(rawData, 0, nPixels);

        SplitError splitError(errorCorrectors);
        std::vector<std::vector<uint8_t>> correctedSplitData = splitError.Decode(splitData);

        std::cout << splitError.GetErrorCount() << std::endl;

}
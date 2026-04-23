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
#include "colour_correction.hpp"

using namespace cv;

int main(int argc, char *argv[]){

    std::string path = "output/img/output_align.png";

    if (argc > 1) {
        path = std::string(argv[1]);
    }

    try {
        Mat image = imread(path);

        int channels = image.channels();
        int rows = image.rows;
        int cols = image.cols;
        int totalLength = rows * cols;

        BarcodeConfig config(rows, cols);
        BarcodeLayout layout = GetBarcode4Circle(config);


        std::vector<uint8_t> imageBytes = ImageAux::MatToBytes(image);

        // imageBytes = ColourCorrection::MethodAverage(imageBytes, layout, true);

        BarcodeWriter writer(layout);
        std::vector<uint8_t> rawData = writer.BarcodeToPixels(imageBytes, totalLength);

        ColourPixels colourPix = ColourPalletes::Bit_3();
        std::vector<ErrorCorrectionVirtual*> errorCorrectors = ErrorLayout::Bit_3();

        int compressedLen = HeaderData::GetCompressedLen(rawData, colourPix, errorCorrectors[0]);
        int nPixels = HeaderData::GetErrorCorrectionLen(compressedLen, errorCorrectors);

        std::vector<std::vector<uint8_t>> splitData = colourPix.PixelsToData(rawData, 0, nPixels);

        SplitError splitError(errorCorrectors);
        std::vector<std::vector<uint8_t>> correctedSplitData = splitError.Decode(splitData);

        std::cout << splitError.GetErrorCount() << " errors" << std::endl;

        std::vector<uint8_t> connectedData = SplitBytes::Decode(correctedSplitData, compressedLen + 2);

        std::cout << connectedData.size() << " length" << std::endl;

        std::vector<uint8_t> decompressed = Compression::decompress(connectedData.data() + 2, compressedLen); 

        std::cout << decompressed.size() << " length 2" << std::endl;

        for (int i = 0; i < decompressed.size(); i++){
            std::cout << (char)decompressed[i];
        }
        std::cout << std::endl;

    } catch (const ExceptionDecompression& e){
        std::cout << e.what() << std::endl;
    }
}
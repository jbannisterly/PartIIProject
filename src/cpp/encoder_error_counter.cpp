#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstring>
#include "barcode_layout.hpp"
#include "compressor.hpp"
#include <vector>
#include "colours.hpp"
#include "_config.hpp"
#include "fstream"
#include "split_error.hpp"
#include "split_bytes.hpp"
#include "barcode_writer.hpp"
#include "colour_palletes.hpp"
#include "error_layout.hpp"
#include "header_data.hpp"
#include "random_data_gen.hpp"
#include <filesystem>
#include <string>

using namespace cv;


int main(){
    const int _SCALE = 16;
    BarcodeConfig config(200, 150);
    std::string outputDirectory = "output/img/evaluation/colour_test/";
    std::filesystem::create_directory(outputDirectory);

    for (int i = 1; i < 9; i++) {
        int bitDepth = i;

        BarcodeLayout layout = GetBarcode4Circle(config);
        ColourPixels colourPix(bitDepth);
        int barcodeCapacity = GetCapacity(layout) / 8;

        std::cout << barcodeCapacity << std::endl;

        std::vector<std::vector<uint8_t>> splitData = RandomDataGen::GenerateRandomData(100, barcodeCapacity, bitDepth * 3);
        std::vector<uint8_t> pixelData = colourPix.DataToPixels(splitData);

        BarcodeWriter writer(layout);
        std::vector<uint8_t> imageData = writer.PixelsToBarcode(pixelData, _SCALE, true);

        Mat image(config.barcodeHeight * _SCALE, config.barcodeWidth * _SCALE, CV_8UC3);
        image.data = imageData.data();

        imwrite(outputDirectory + "output_" +  std::to_string(bitDepth) + "bits.png", image);
    }
}
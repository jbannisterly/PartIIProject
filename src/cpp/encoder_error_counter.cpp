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

using namespace cv;

int main(){
    BarcodeLayout layout = GetBarcode4Detailed();
    ColourPixels colourPix(8);
    int barcodeCapacity = GetCapacity(layout) / 8;

    std::cout << barcodeCapacity << std::endl;

    std::vector<std::vector<uint8_t>> splitData = RandomDataGen::GenerateRandomData(100, barcodeCapacity, 24);
    std::vector<uint8_t> pixelData = colourPix.DataToPixels(splitData);

    BarcodeWriter writer(layout);
    std::vector<uint8_t> imageData = writer.PixelsToBarcode(pixelData);

    Mat image(BARCODE_HEIGHT, BARCODE_WIDTH, CV_8UC3);
    image.data = imageData.data();

    imwrite("output/img/output.png", image);
}
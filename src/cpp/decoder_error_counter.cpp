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
#include "random_data_gen.hpp"

using namespace cv;

double Compare(std::vector<uint8_t> &original, std::vector<uint8_t> &recovered) {
    int count = 0;
    
    for (int i = 0; i < original.size(); i++) {

        for (int j = 0; j < 8; j++) {
            if ((original[i] & (1 << j)) == (recovered[i] & (1 << j))) {
                count++;
            }
        }
    }

    return (double)count / original.size() / 8;
}

int main(){
    Mat image = imread("output/img/output_align.png");
    BarcodeLayout layout = GetBarcode4Detailed();
    int nPixels = (GetCapacity(layout) / 8) * 8;

    int channels = image.channels();
    int rows = image.rows;
    int cols = image.cols;
    int totalLength = rows * cols * channels;

    std::vector<uint8_t> imageBytes = ImageAux::MatToBytes(image);

    BarcodeWriter writer(layout);
    std::vector<uint8_t> rawData = writer.BarcodeToPixels(imageBytes, totalLength);

    ColourPixels colourPix(8);

    std::vector<std::vector<uint8_t>> recoveredData = colourPix.PixelsToData(rawData, 0, nPixels / 8);
    std::vector<std::vector<uint8_t>> trueData = RandomDataGen::GenerateRandomData(100, nPixels / 8, 24);

    std::vector<double> accuracy;
    accuracy.reserve(trueData.size());
    for (int i = 0; i < trueData.size(); i++) {
        accuracy.push_back(Compare(trueData[i], recoveredData[i]));
    }

    for (int i = 0; i < accuracy.size(); i++) {
        std::cout << accuracy[i] << std::endl;
    }

}
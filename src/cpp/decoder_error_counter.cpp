#include <iostream>
#include <opencv2/opencv.hpp>
#include "barcode_layout.hpp"
#include "image_aux.hpp"
#include "compressor.hpp"
#include "colours.hpp"
#include <fstream>
#include "barcode_writer.hpp"
#include "colour_palletes.hpp"
#include "error_layout.hpp"
#include "header_data.hpp"
#include "random_data_gen.hpp"
#include "colour_correction.hpp"
#include <array>

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

double MutualInformation(std::vector<uint8_t> &original, std::vector<uint8_t> &recovered) {
    std::array<double, 4> count = {0, 0, 0, 0};
    
    for (int i = 0; i < original.size(); i++) {

        for (int j = 0; j < 8; j++) {
            int index  = 0;
            if ((original[i] & (1 << j))) {
                index += 1;
            } 
            if (recovered[i] & (1 << j)) {
                index += 2;
            }

            count[index] += 1;
        }
    }

    double total = count[0] + count[1] + count[2] + count[3];

    double mi = 0;

    for (int i = 0; i < count.size(); i++) {
        double px = (count[i] + count[(i + 1) % 4]) / total;
        double py = (count[i] + count[(i + 2) % 4]) / total;
        double pxy = count[i] / total;
        if (pxy > 0) {
            mi += pxy * log2(pxy / px / py);
        }
    }

    return mi;
}

void RunTest(std::string path, int bitDepth) {
    BarcodeConfig config(200, 150);


    Mat image = imread(path);
    BarcodeLayout layout = GetBarcode4Detailed(config);
    int nPixels = (GetCapacity(layout) / 8) * 8;

    int channels = image.channels();
    int rows = image.rows;
    int cols = image.cols;
    int totalLength = rows * cols * channels;

    std::vector<uint8_t> imageBytes = ImageAux::MatToBytes(image);

    imageBytes = ColourCorrection::MethodQuartersAverage(imageBytes, layout, true);

    Mat correctedImage(image.rows, image.cols, CV_8UC3, imageBytes.data());
    imwrite("output/img/output_corrected.png", correctedImage);

    BarcodeWriter writer(layout);
    std::vector<uint8_t> rawData = writer.BarcodeToPixels(imageBytes, totalLength);

    ColourPixels colourPix(bitDepth);

    std::vector<std::vector<uint8_t>> recoveredData = colourPix.PixelsToData(rawData, 0, nPixels / 8);
    std::vector<std::vector<uint8_t>> trueData = RandomDataGen::GenerateRandomData(100, nPixels / 8, bitDepth * 3);

    std::vector<double> accuracy;
    accuracy.reserve(trueData.size());
    for (int i = 0; i < trueData.size(); i++) {
        accuracy.push_back(Compare(trueData[i], recoveredData[i]));
    }

    for (int i = 0; i < accuracy.size(); i++) {
        // std::cout << accuracy[i] << std::endl;
    }

    double capacity = 0;
    for (int i = 0; i < trueData.size(); i++) {
        capacity += (MutualInformation(trueData[i], recoveredData[i]));
    }
    std::cout << "\"" << path << "\"" << ":"  << capacity << "," << std::endl;

}

int main(){

    for (int i = 1; i <= 8; i++) {
        RunTest("output/img/evaluation/colour_test/photo_indoors_" + std::to_string(i) + "_bits_align.png", i);
        RunTest("output/img/evaluation/colour_test/photo_outdoors_" + std::to_string(i) + "_bits_align.png", i);
        RunTest("output/img/evaluation/colour_test/photo_lamp_" + std::to_string(i) + "_bits_align.png", i);
        RunTest("output/img/evaluation/colour_test/photo_flash_" + std::to_string(i) + "_bits_align.png", i);
    }

}
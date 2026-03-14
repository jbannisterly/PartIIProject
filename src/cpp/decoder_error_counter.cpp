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
    const int _BIT_DEPTH = 2;

    Mat image = imread("output/img/output_align.png");
    BarcodeLayout layout = GetBarcode4Detailed();
    int nPixels = (GetCapacity(layout) / 8) * 8;

    int channels = image.channels();
    int rows = image.rows;
    int cols = image.cols;
    int totalLength = rows * cols * channels;

    std::vector<uint8_t> imageBytes = ImageAux::MatToBytes(image);

    imageBytes = ColourCorrection::MethodQuarters(imageBytes, layout);

    Mat correctedImage(image.rows, image.cols, CV_8UC3, imageBytes.data());
    imwrite("output/img/output_corrected.png", correctedImage);

    BarcodeWriter writer(layout);
    std::vector<uint8_t> rawData = writer.BarcodeToPixels(imageBytes, totalLength);

    ColourPixels colourPix(_BIT_DEPTH);

    std::vector<std::vector<uint8_t>> recoveredData = colourPix.PixelsToData(rawData, 0, nPixels / 8);
    std::vector<std::vector<uint8_t>> trueData = RandomDataGen::GenerateRandomData(100, nPixels / 8, _BIT_DEPTH * 3);

    std::vector<double> accuracy;
    accuracy.reserve(trueData.size());
    for (int i = 0; i < trueData.size(); i++) {
        accuracy.push_back(Compare(trueData[i], recoveredData[i]));
    }

    for (int i = 0; i < accuracy.size(); i++) {
        std::cout << accuracy[i] << std::endl;
    }

    double capacity = 0;
    for (int i = 0; i < accuracy.size(); i++) {
        capacity += 1 - ((1 - accuracy[i]) * 2);
    }
    std::cout << "bits per pixel " << capacity << std::endl;
}
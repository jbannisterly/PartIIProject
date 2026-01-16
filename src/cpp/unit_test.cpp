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
#include "image_aux.hpp"

void TestResult(bool result, std::string testName) {
    if (result) {
        std::cout << testName << " test passed" << std::endl;
    } else {
        std::cout << testName << " test failed" << std::endl;
    }
}

bool CompressorTest(std::vector<uint8_t> &data) {
    Compression compressor;
    
    std::vector<uint8_t> compressedData = compressor.compress(data);
    std::vector<uint8_t> recoveredData = compressor.decompress(compressedData.data(), compressedData.size());

    return recoveredData == data;
}

bool SplitTest(std::vector<uint8_t> &data, std::vector<ErrorCorrectionVirtual*> errorCorrectors) {
    std::vector<std::vector<uint8_t>> split = SplitBytes::Encode(data, errorCorrectors);
    std::vector<uint8_t> recovered = SplitBytes::Decode(split, data.size());

    return recovered == data;
}

bool ErrorCorrectionTest(std::vector<std::vector<uint8_t>> &data, std::vector<ErrorCorrectionVirtual*> errorCorrectors) {
    SplitError split(errorCorrectors);

    std::vector<std::vector<uint8_t>> correction = split.Encode(data);
    std::vector<std::vector<uint8_t>> recovered = split.Decode(correction);

    return recovered == data;
}

bool DataToPixelsTest(std::vector<std::vector<uint8_t>> &data, ColourPixels colourPix) {
    std::vector<uint8_t> pixelData = colourPix.DataToPixels(data);
    std::vector<std::vector<uint8_t>> recovered = colourPix.PixelsToData(pixelData, 0, data[0].size());

    return recovered == data;
}

bool LengthTest(std::vector<uint8_t> &data, std::vector<ErrorCorrectionVirtual*> errorCorrectors, ColourPixels colourPix) {
    std::vector<uint8_t> dataCopy = data;
    SplitError split(errorCorrectors);

    HeaderData::PrependLength(dataCopy);
    std::vector<std::vector<uint8_t>> splitData = SplitBytes::Encode(dataCopy, errorCorrectors);
    std::vector<uint8_t> pixels = colourPix.DataToPixels(splitData);

    int recoveredLength = HeaderData::GetCompressedLen(pixels, colourPix, errorCorrectors[0]);

    return recoveredLength == data.size();
}

int main() {
    std::vector<uint8_t> data;
    data.reserve(2000);
    for (int i = 0; i < 2000; i++) {
        data.push_back(i % 256);
    }

    std::vector<std::vector<uint8_t>> splitData;
    for (int i = 0; i < 3; i++) {
        std::vector<uint8_t> dataPart;
        for (int j = 0; j < 669; j++) {
            dataPart.push_back(j);
        }
        splitData.push_back(dataPart);
    }

    std::vector<ErrorCorrectionVirtual*> errorCorrectors = ErrorLayout::Bit_3();
    ColourPixels colourPix = ColourPalletes::Bit_3();

    TestResult(CompressorTest(data), "compressor");
    TestResult(SplitTest(data, errorCorrectors), "split");
    TestResult(ErrorCorrectionTest(splitData, errorCorrectors), "split error");
    TestResult(DataToPixelsTest(splitData, colourPix), "data to pixels");
    TestResult(LengthTest(data, errorCorrectors, colourPix), "length");
}
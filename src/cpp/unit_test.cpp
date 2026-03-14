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
#include <gtest/gtest.h>

void TestResult(bool result, std::string testName) {
    if (result) {
        std::cout << testName << " test passed" << std::endl;
    } else {
        std::cout << testName << " test failed" << std::endl;
    }
}

std::vector<uint8_t> GetData() {
    std::vector<uint8_t> data;
    data.reserve(2000);
    for (int i = 0; i < 2000; i++) {
        data.push_back(i % 256);
    }

    return data;
}

std::vector<std::vector<uint8_t>> GetSplitData() {
    std::vector<std::vector<uint8_t>> splitData;
    for (int i = 0; i < 3; i++) {
        std::vector<uint8_t> dataPart;
        for (int j = 0; j < 669; j++) {
            dataPart.push_back(j);
        }
        splitData.push_back(dataPart);
    }

    return splitData;
}

TEST(CompressorTest, Test1) {
    Compression compressor;
    
    std::vector<uint8_t> data = GetData();

    std::vector<uint8_t> compressedData = compressor.compress(data);
    std::vector<uint8_t> recoveredData = compressor.decompress(compressedData.data(), compressedData.size());

    EXPECT_EQ(recoveredData, data);
}

TEST(SplitTest, Test1) {
    std::vector<uint8_t> data = GetData();
    std::vector<ErrorCorrectionVirtual*> errorCorrectors = ErrorLayout::Bit_3();
    
    std::vector<std::vector<uint8_t>> splitData = SplitBytes::Encode(data, errorCorrectors);
    std::vector<uint8_t> recovered = SplitBytes::Decode(splitData, data.size());

    EXPECT_EQ(recovered, data);
}

TEST(ErrorCorrectionTest, Test1) {
    std::vector<std::vector<uint8_t>> splitData = GetSplitData(); 
    std::vector<ErrorCorrectionVirtual*> errorCorrectors = ErrorLayout::Bit_3();
    SplitError split(errorCorrectors);

    std::vector<std::vector<uint8_t>> correction = split.Encode(splitData);
    std::vector<std::vector<uint8_t>> recovered = split.Decode(correction);

    EXPECT_EQ(recovered, splitData);
}

TEST(DataToPixelsTest, Test1) {
    ColourPixels colourPix = ColourPalletes::Bit_3();
    std::vector<std::vector<uint8_t>> splitData = GetSplitData(); 


    std::vector<uint8_t> pixelData = colourPix.DataToPixels(splitData);
    std::vector<std::vector<uint8_t>> recovered = colourPix.PixelsToData(pixelData, 0, splitData[0].size());

    EXPECT_EQ(recovered, splitData);
}

TEST(LengthTest, Test1) {
    std::vector<uint8_t> data = GetData();    
    std::vector<ErrorCorrectionVirtual*> errorCorrectors = ErrorLayout::Bit_3();
    ColourPixels colourPix = ColourPalletes::Bit_3();
    std::vector<uint8_t> dataCopy = data;
    SplitError split(errorCorrectors);

    HeaderData::PrependLength(dataCopy);
    std::vector<std::vector<uint8_t>> splitData = SplitBytes::Encode(dataCopy, errorCorrectors);
    std::vector<uint8_t> pixels = colourPix.DataToPixels(splitData);

    int recoveredLength = HeaderData::GetCompressedLen(pixels, colourPix, errorCorrectors[0]);

    EXPECT_EQ(recoveredLength, data.size());
}

int main() {

    testing::InitGoogleTest();

    // std::vector<ErrorCorrectionVirtual*> errorCorrectors = ErrorLayout::Bit_3();
    // ColourPixels colourPix = ColourPalletes::Bit_3();

    // TestResult(SplitTest(data, errorCorrectors), "split");
    // TestResult(ErrorCorrectionTest(splitData, errorCorrectors), "split error");
    // TestResult(DataToPixelsTest(splitData, colourPix), "data to pixels");
    // TestResult(LengthTest(data, errorCorrectors, colourPix), "length");

    return RUN_ALL_TESTS();
}
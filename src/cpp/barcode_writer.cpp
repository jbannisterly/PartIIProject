#include "barcode_writer.hpp"
#include "barcode_image_editor.hpp"
#include <random>
#include <iostream>
#include <array>

std::vector<int> RandomPermutation(int size, int seed) {
    std::vector<int> data;
    data.reserve(size);
    srand(seed);
    
    for (int i = 0; i < size; i++) {
        data.push_back(i);
    }

    for (int i = 0; i < size; i++) {
        int t1 = random() % size;
        int t2 = random() % size;
        int t3 = data[t1];
        data[t1] = data[t2];
        data[t2] = t3;
    }

    return data;
}

std::vector<uint8_t> ShufflePixels(std::vector<uint8_t> &pixels, std::vector<int> &permutation) {
    std::vector<uint8_t> shuffled;
    shuffled.resize(permutation.size() * 3, 0);
    
    for (int i = 0; i < pixels.size(); i++) {
        int index = permutation[i];
        for (int j = 0; j < 3; j++) {
            shuffled[index * 3 + j] = pixels[i * 3 + j];
        }
    }

    return shuffled;
}

std::vector<uint8_t> DeShufflePixels(std::vector<uint8_t> &shuffled, std::vector<int> &permutation) {
    std::vector<uint8_t> pixels;
    pixels.resize(permutation.size() * 3, 0);
    
    for (int i = 0; i < permutation.size(); i++) {
        int index = permutation[i];
        for (int j = 0; j < 3; j++) {
            pixels[i * 3 + j] = shuffled[index * 3 + j];
        }
    }

    return pixels;
}

void DrawRings(std::vector<uint8_t> &scaledData, double positionX, double positionY, int barcodeWidth, int scale, bool invertColours=false) {
    std::array<uint8_t, 3> colours = {0, 255, 0};
    if (invertColours) colours = {255, 0, 255};
    BarcodeImageEditor::DrawCircle(scaledData, scale * barcodeWidth, positionX * scale, positionY * scale, 3.5 * scale, colours[0]);
    BarcodeImageEditor::DrawCircle(scaledData, scale * barcodeWidth, positionX * scale, positionY * scale, 2.5 * scale, colours[1]);
    BarcodeImageEditor::DrawCircle(scaledData, scale * barcodeWidth, positionX * scale, positionY * scale, 1.5 * scale, colours[2]);
}

std::vector<uint8_t> BarcodeWriter::PixelsToBarcode(std::vector<uint8_t> &pixels, int scale){
    BarcodeLayout layout = templateLayout;
    int capacity = layout.GetDataSize();
    std::vector<int> permutation = RandomPermutation(capacity, 0);
    std::vector<uint8_t> shuffled = ShufflePixels(pixels, permutation);
    
    int sourceCounter = 0;
    int targetCounter = 0;

    for (int i = 0; i < layout.mask.size(); i++) {
        if (layout.mask[i] > 0) {
            for (int j = 0; j < 3; j++) {
                layout.data[i * 3 + j] = (i * 92 + j * 180) % 256;
            }
        }
    }

    while(sourceCounter < permutation.size()){
        if (layout.mask[targetCounter] > 0){
            for (int i = 0; i < 3; i++) {
                layout.data[targetCounter * 3 + i] = shuffled[sourceCounter * 3 + i];
            }
            sourceCounter++;
        }
        targetCounter++;
    }

    std::vector<uint8_t> scaledData = BarcodeImageEditor::Scale(layout.data, scale, layout.barcodeWidth);

    DrawRings(scaledData, 4.5, 4.5, layout.barcodeWidth, scale);
    DrawRings(scaledData, layout.barcodeWidth - 4.5, 4.5, layout.barcodeWidth, scale);
    DrawRings(scaledData, 4.5, layout.data.size() / 3 / layout.barcodeWidth - 4.5, layout.barcodeWidth, scale);
    DrawRings(scaledData, layout.data.size() / 3 / layout.barcodeWidth - 4.5, layout.data.size() / 3 / layout.barcodeWidth - 4.5, layout.barcodeWidth, scale, true);


    return scaledData;
}

std::vector<uint8_t> BarcodeWriter::BarcodeToPixels(std::vector<uint8_t> barcode, int barcodeSize){
    BarcodeLayout layout = templateLayout;
    permutation = RandomPermutation(layout.GetDataSize(), 0);

    std::vector<uint8_t> shuffled;
    shuffled.reserve(barcodeSize * 3);
    int pixelCounter = 0;

    for (int i = 0; i < barcodeSize; i++){
        if (templateLayout.mask[i] > 0){
            for (int j = 0; j < 3; j++) {
                shuffled.push_back(barcode[i * 3 + j]);
            }
            pixelCounter++;
        } 
    }

    std::vector<uint8_t> pixels = DeShufflePixels(shuffled, permutation);

    return pixels;
}

BarcodeWriter::BarcodeWriter(BarcodeLayout inLayout) : 
templateLayout{inLayout} {}

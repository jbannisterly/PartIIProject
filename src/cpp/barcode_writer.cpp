#include "barcode_writer.hpp"

std::vector<uint8_t> BarcodeWriter::PixelsToBarcode(std::vector<uint8_t> &pixels){
    BarcodeLayout layout = templateLayout;
    
    int sourceCounter = 0;
    int targetCounter = 0;

    for (int i = 0; i < layout.mask.size(); i++) {
        if (layout.mask[i] > 0) {
            for (int j = 0; j < 3; j++) {
                layout.data[i * 3 + j] = (i * 92 + j * 180) % 256;
            }
        }
    }

    while(sourceCounter < pixels.size()){
        if (layout.mask[targetCounter] > 0){
            for (int i = 0; i < 3; i++) {
                layout.data[targetCounter * 3 + i] = pixels[sourceCounter];
                sourceCounter++;
            }
        }
        targetCounter++;
    }



    return layout.data;
}

std::vector<uint8_t> BarcodeWriter::BarcodeToPixels(std::vector<uint8_t> barcode, int barcodeSize){
    std::vector<uint8_t> pixels;
    pixels.reserve(barcodeSize * 3);
    int pixelCounter = 0;

    for (int i = 0; i < barcodeSize; i++){
        if (templateLayout.mask[i] > 0){
            for (int j = 0; j < 3; j++) {
                pixels.push_back(barcode[i * 3 + j]);
            }
        } 
    }

    return pixels;
}

BarcodeWriter::BarcodeWriter(BarcodeLayout inLayout) : 
templateLayout{inLayout} {}

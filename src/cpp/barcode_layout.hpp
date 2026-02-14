#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

struct BarcodeLayout{
    std::vector<uint8_t> data;
    std::vector<uint8_t> mask;
    int barcodeWidth;
    
    int GetDataSize() {
        int size = 0;
        
        for (int i = 0; i < mask.size(); i++) {
            if (mask[i] > 0) {
                size++;
            }
        }

        return size;
    }
};

BarcodeLayout GetBarcode3();
BarcodeLayout GetBarcode4Simple();
BarcodeLayout GetBarcode4Detailed();
BarcodeLayout GetBarcode4Circle();

int GetCapacity(BarcodeLayout barcode);
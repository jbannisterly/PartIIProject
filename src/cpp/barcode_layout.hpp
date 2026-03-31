#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>
#include "_config.hpp"

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

BarcodeLayout GetBarcode3(BarcodeConfig config);
BarcodeLayout GetBarcode4Simple(BarcodeConfig config);
BarcodeLayout GetBarcode4Detailed(BarcodeConfig config);
BarcodeLayout GetBarcode4Circle(BarcodeConfig config);

int GetCapacity(BarcodeLayout barcode);
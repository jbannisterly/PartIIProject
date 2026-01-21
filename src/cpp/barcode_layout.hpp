#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

struct BarcodeLayout{
    std::vector<uint8_t> data;
    std::vector<uint8_t> mask;
};

BarcodeLayout GetBarcode3();
BarcodeLayout GetBarcode4Simple();
BarcodeLayout GetBarcode4Detailed();

int GetCapacity(BarcodeLayout barcode);
#pragma once

#include <vector>
#include <cstdint>
#include <array>
#include "barcode_layout.hpp"

namespace ColourCorrection {
    std::vector<uint8_t> MethodAverage(std::vector<uint8_t> &data, BarcodeLayout layout, bool allPixels=false);
    std::vector<uint8_t> MethodHighLowAvg(std::vector<uint8_t> &data, BarcodeLayout layout);
    std::vector<uint8_t> MethodQuarters(std::vector<uint8_t> &data, BarcodeLayout layout);
    std::vector<uint8_t> MethodQuartersAverage(std::vector<uint8_t> &data, BarcodeLayout layout, bool allPixels=false);

}
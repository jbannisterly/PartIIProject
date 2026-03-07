#pragma once

#include <vector>
#include <cstdint>
#include "barcode_layout.hpp"

namespace ColourCorrection {
    std::vector<uint8_t> MethodAverage(std::vector<uint8_t> &data, BarcodeLayout layout);
    std::vector<uint8_t> MethodHighLowAvg(std::vector<uint8_t> &data, BarcodeLayout layout);
}
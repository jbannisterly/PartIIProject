#pragma once

#include <vector>
#include <cstdint>
#include <cmath>
#include "colours.hpp"
#include "error_correction_schifra.hpp"

namespace HeaderData {
    void PrependLength(std::vector<uint8_t> &data);
    int GetErrorCorrectionLen(int compressedLen, std::vector<ErrorCorrectionVirtual*> &errorCorrectors);
    int GetCompressedLen(std::vector<uint8_t> &pixels, ColourPixels colourPix, ErrorCorrectionVirtual* errorCorrection);
}
#include <vector>
#include <cstdint>
#include <iostream>
#include <cmath>
#include "error_correction_schifra.hpp"

namespace SplitBytes {
    std::vector<std::vector<uint8_t>> Encode(std::vector<uint8_t> &data, std::vector<ErrorCorrectionVirtual*> &errorCorrectors);
    std::vector<uint8_t> Decode(std::vector<std::vector<uint8_t>> &splitData, int dataLength);
}

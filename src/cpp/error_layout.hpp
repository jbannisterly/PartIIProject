#pragma once

#include <vector>
#include "error_correction_schifra.hpp"
#include <array>

namespace ErrorLayout {
    std::vector<ErrorCorrectionVirtual*> Bit_3();
    std::vector<ErrorCorrectionVirtual*> Bit_3_extra();
    std::vector<ErrorCorrectionVirtual*> Bit_9();
}
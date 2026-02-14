#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace BarcodeImageEditor {
    std::vector<uint8_t> Scale(std::vector<uint8_t> &data, int scale, int originalWidth);
    void DrawCircle(std::vector<uint8_t> &data);
}
#pragma once

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace BarcodeImageEditor {
    std::vector<uint8_t> Scale(std::vector<uint8_t> &data, int scale, int originalWidth);
    void DrawCircle(std::vector<uint8_t> &data, int barcodeWidth, double circleCentreX, double circleCentreY, double circleRadius, uint8_t colour);
}
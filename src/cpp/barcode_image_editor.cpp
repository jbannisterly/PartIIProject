#include "barcode_image_editor.hpp"
#include <iostream>

std::vector<uint8_t> BarcodeImageEditor::Scale(std::vector<uint8_t> &data, int scale, int originalWidth) {
    int originalHeight = data.size() / originalWidth / 3;
    
    std::vector<uint8_t> dataScaled;
    dataScaled.reserve(data.size() * scale * scale);

    for (int i = 0; i < originalHeight * scale; i++) {
        for (int j = 0; j < originalWidth * scale; j++) {
            for (int k = 0; k < 3; k++) {
                dataScaled.push_back(data[(int(i / scale) * originalWidth + int(j / scale)) * 3 + k]);
                // std::cout << (int(i / scale) * originalWidth + int(j / scale)) * 3 + k << std::endl;
            }
        }
    }

    return dataScaled;
}

void BarcodeImageEditor::DrawCircle(std::vector<uint8_t> &data, int barcodeWidth, double circleCentreX, double circleCentreY, double circleRadius, uint8_t colour) {

    for (int i = circleCentreY - circleRadius; i < circleCentreY + circleRadius; i++) {
        for (int j = circleCentreX - circleRadius; j < circleCentreX + circleCentreX; j++) {
            if ((i - circleCentreY) * (i - circleCentreY) + (j - circleCentreX) * (j - circleCentreX) < circleRadius * circleRadius) {
                for (int k = 0; k < 3; k++) {
                    data[(i * barcodeWidth + j) * 3 + k] = colour;
                }
            }
        }
    }
}

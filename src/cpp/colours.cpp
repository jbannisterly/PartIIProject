#include "colours.hpp"
#include <iostream>
#include <math.h>

int GetBit(std::vector<uint8_t> &data, int bitNo) {
    uint8_t byte = data[int(bitNo / 8)];
    int bit = bitNo % 8;
    return (byte >> bit) & 1;
}

std::vector<uint8_t> ColourPixels::DataToPixels(std::vector<std::vector<uint8_t>> &data) {
    int bitsPerPixel = data.size();
    int nPixels = data[0].size() * 8; // data stores bytes, so convert to bits

    std::vector<uint8_t> pixels;
    pixels.reserve(3 * nPixels);

    for (int i = 0; i < nPixels; i++) {
        int pixelValue = 0;
        for (int j = 0; j < bitsPerPixel; j++) {
            pixelValue |= GetBit(data[j], i) << j;
        }

        Colour pixelColour = colourScheme.colours[pixelValue];

        pixels.push_back(pixelColour.b);
        pixels.push_back(pixelColour.g);
        pixels.push_back(pixelColour.r);
    }

    std::cout << pixels.size() << std::endl;

    return pixels;
}

std::vector<std::vector<uint8_t>> ColourPixels::PixelsToData(std::vector<uint8_t> &pixels) {
    return std::vector<std::vector<uint8_t>>();
}
#include "colours.hpp"
#include <iostream>
#include <math.h>

// bit layout is 76543210
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

        pixels.push_back(pixelColour.r);
        pixels.push_back(pixelColour.g);
        pixels.push_back(pixelColour.b);
    }

    return pixels;
}

std::vector<uint8_t> ColourPixels::PixelsToColourIndex(std::vector<uint8_t> &pixels, int start, int end) {
    std::vector<uint8_t> indices;
    indices.reserve(end - start);

    for (int i = start; i < end; i++) {
        uint8_t index = 0;
        double minDistance = 1000;
        Colour lookup(pixels[i * 3], pixels[i * 3 + 1], pixels[i * 3 + 2]);
        for (int j = 0; j < colourScheme.colours.size(); j++) {
            double distance = Colour::distance(colourScheme.colours[j], lookup);
            if (distance < minDistance) {
                minDistance = distance;
                index = j;
            }
        }
        indices.push_back(index);
    }

    return indices;
}

std::vector<uint8_t> ColourPixels::ColoursToData(std::vector<uint8_t> &colourIndex, int splitIndex, int start, int end) {
    std::vector<uint8_t> data;
    data.reserve(end - start);

    for (int i = start; i < end; i++) {
        uint8_t currentByte = 0;
        for (int j = 0; j < 8; j++) {
            uint8_t currentBit = (colourIndex[i * 8 + j] >> splitIndex) & 1;
            currentByte |= currentBit << j;
        }
        data.push_back(currentByte);
    }

    return data;
}

std::vector<std::vector<uint8_t>> ColourPixels::PixelsToData(std::vector<uint8_t> &pixels, int startByte, int endByte) {
    int bitsPerPixel = log2(colourScheme.colours.size());
    
    std::vector<uint8_t> colourIndex = PixelsToColourIndex(pixels, startByte * 8, endByte * 8);
    std::vector<std::vector<uint8_t>> data;

    for (int i = 0; i < bitsPerPixel; i++) {
        std::vector<uint8_t> splitData = ColoursToData(colourIndex, i, startByte, endByte);
        data.push_back(splitData);
    }
    
    return data;
}
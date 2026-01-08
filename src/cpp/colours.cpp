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

        pixels.push_back(pixelColour.b);
        pixels.push_back(pixelColour.g);
        pixels.push_back(pixelColour.r);
    }

    std::cout << pixels.size() << std::endl;

    return pixels;
}

std::vector<uint8_t> ColourPixels::PixelsToColourIndex(std::vector<uint8_t> &pixels) {
    std::vector<uint8_t> indices;
    indices.reserve(pixels.size() / 3);

    for (int i = 0; i < indices.size(); i++) {
        uint8_t index = 0;
        double minDistance = 1000;
        Colour lookup(pixels[index * 3], pixels[index * 3 + 1], pixels[index * 3 + 2]);
        for (int j = 0; j < colourScheme.colours.size(); i++) {
            double distance = Colour::distance(colourScheme.colours[i], lookup);
            if (distance < minDistance) {
                minDistance = distance;
                index = j;
            }
        }
        indices.push_back(index);
    }

    return indices;
}

// TODO: this
std::vector<uint8_t> ColourPixels::ColoursToData(std::vector<uint8_t> &colourIndex, int splitIndex) {
    std::vector<uint8_t> data;
    data.reserve(colourIndex.size() / 8);

    for (int i = 0; i < data.size(); i++) {
        uint8_t currentByte = 0;
        for (int j = 0; j < 8; j++) {
            uint8_t currentBit = (colourIndex[i * 8 + j] >> splitIndex) & 1;
            currentByte |= currentBit << j;
        }
        data.push_back(currentByte);
    }

    return data;
}

std::vector<std::vector<uint8_t>> ColourPixels::PixelsToData(std::vector<uint8_t> &pixels) {
    int bitsPerPixel = log2(colourScheme.colours.size());
    
    std::vector<uint8_t> colourIndex = PixelsToColourIndex(pixels);
    std::vector<std::vector<uint8_t>> data;

    for (int i = 0; i < bitsPerPixel; i++) {
        std::vector<uint8_t> splitData = ColoursToData(colourIndex, i);
        data.push_back(splitData);
    }
    
    return data;
}
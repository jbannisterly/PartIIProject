#include "colours.hpp"
#include <iostream>
#include <math.h>
#include <array>

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
        uint pixelValue = 0;
        for (int j = 0; j < bitsPerPixel; j++) {
            pixelValue |= GetBit(data[j], i) << j;
        }

        Colour pixelColour;

        switch (rule) {
            case ColourRules::COLOUR_SCHEME:
                pixelColour = colourScheme.colours[pixelValue];
                break;
            case ColourRules::BIT_DEPTH:
                for (int i = 0; i < bitDepth; i++) {
                    pixelColour.r |= ((pixelValue >> (i * 3 + 0)) & 1) << (i);
                    pixelColour.g |= ((pixelValue >> (i * 3 + 1)) & 1) << (i);
                    pixelColour.b |= ((pixelValue >> (i * 3 + 2)) & 1) << (i);
                }
                pixelColour.r = pixelColour.r * (double)255 / ((1 << bitDepth) - 1);
                pixelColour.g = pixelColour.g * (double)255 / ((1 << bitDepth) - 1);
                pixelColour.b = pixelColour.b * (double)255 / ((1 << bitDepth) - 1);
                break;
        }
    
        pixels.push_back(pixelColour.r);
        pixels.push_back(pixelColour.g);
        pixels.push_back(pixelColour.b);
    }

    return pixels;
}

std::vector<uint> ColourPixels::PixelsToColourIndexColourScheme(std::vector<uint8_t> &pixels, int start, int end) {
    std::vector<uint> indices;
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

std::vector<uint> ColourPixels::PixelsToColourIndexBitDepth(std::vector<uint8_t> &pixels, int start, int end) {
    std::vector<uint> indices;
    int nColours = 1 << bitDepth; 
    indices.reserve(end - start);

    for (int i = start; i < end; i++) {
        uint index = 0;
        std::array<uint, 3> nearestData;
        for (int j = 0; j < 3; j++) {
            nearestData[j] = round(pixels[i * 3 + j] / ((double)255 / (nColours - 1)));
        }
        for (int j = 0; j < bitDepth; j++) {
            for (int k = 0; k < 3; k++) {
                index |= ((nearestData[k] >> j) & 1) << (j * 3 + k);
            }
        }
        indices.push_back(index);
    }

    return indices;
}

std::vector<uint8_t> ColourPixels::ColoursToData(std::vector<uint> &colourIndex, int splitIndex, int start, int end) {
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
    int bitsPerPixel;
    
    std::vector<uint> colourIndex;
    switch (rule) {
        case ColourRules::BIT_DEPTH:
            bitsPerPixel = bitDepth * 3;
            colourIndex = PixelsToColourIndexBitDepth(pixels, startByte * 8, endByte * 8);
            break;
        case ColourRules::COLOUR_SCHEME:
            bitsPerPixel = log2(colourScheme.colours.size());
            colourIndex = PixelsToColourIndexColourScheme(pixels, startByte * 8, endByte * 8);
            break;
    }
    std::vector<std::vector<uint8_t>> data;

    for (int i = 0; i < bitsPerPixel; i++) {
        std::vector<uint8_t> splitData = ColoursToData(colourIndex, i, startByte, endByte);
        data.push_back(splitData);
    }
    
    return data;
}
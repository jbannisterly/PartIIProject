#include "colour_palletes.hpp"

namespace ColourPalletes {
    ColourPixels Bit_3() {
        std::vector<Colour> colours = {
            Colour(0, 0, 0),
            Colour(255, 255, 255),
            Colour(255, 0, 0),
            Colour(255, 255, 0),
            Colour(0, 255, 0),
            Colour(255, 0, 255),
            Colour(0, 0, 255),
            Colour(0, 255, 255),
        };

        ColourScheme colourScheme(colours);
        ColourPixels colourPixels(colourScheme);

        return colourPixels;
    }
}


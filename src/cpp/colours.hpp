#include <vector>
#include <cstdint>

struct Colour {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Colour(uint8_t inR, uint8_t inG, uint8_t inB) :
    r {inR} ,
    g {inG} ,
    b {inB} { }

    bool operator==(const Colour &colour) {
        return colour.b == this->b && colour.g == this->g && colour.r == this->r;
    }

    static double distance(const Colour &colour1, const Colour &colour2) {
        return  std::abs(colour1.b - colour2.b) + 
                std::abs(colour1.g - colour2.g) + 
                std::abs(colour1.r - colour2.r);
    }
};

struct ColourScheme {
    std::vector<Colour> colours;

    ColourScheme(std::vector<Colour> &inColours) :
    colours {inColours} { }
};

class ColourPixels {
    private:

    ColourScheme colourScheme;
    std::vector<uint8_t> PixelsToColourIndex(std::vector<uint8_t> &pixels);
    std::vector<uint8_t> ColoursToData(std::vector<uint8_t> &colourIndex, int splitIndex);

    public:

    std::vector<uint8_t> DataToPixels(std::vector<std::vector<uint8_t>> &data);
    std::vector<std::vector<uint8_t>> PixelsToData(std::vector<uint8_t> &pixels);

    ColourPixels (ColourScheme inColourScheme) :
    colourScheme {inColourScheme} { }
};
#pragma once
#include <vector>
#include <cstdint>
#include <functional>

struct Position{
    double x;
    double y;

    Position(double inX, double inY){
        x = inX;
        y = inY;
    }
};

struct VerticalData{
    std::vector<uint8_t> data;
    int height;
    int width;
};

struct FinderCandidate{
    double y = 0;
    double x = 0;
    double width = 0;

    FinderCandidate() {}

    FinderCandidate(double initY, double initX, double initW) :
    y { initY },
    x { initX },
    width { initW }
    {}

    void Debug() {
        // std::cout << x << " " << y << " " << width << std::endl;
    }

};

namespace FinderPatterns {
    std::vector<FinderCandidate> FinderPatterns(int patternSize, std::vector<uint8_t> &data, int sizeY, int sizeX, std::function<bool (std::vector<int>)> patternValid, bool firstWhite = true);
    
}
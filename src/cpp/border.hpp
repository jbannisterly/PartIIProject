#pragma once

#include <vector>
#include <stdint.h>

namespace Border {    
    struct Coord {
        int x;
        int y;
        int width;

        Coord (int position, int inWidth) :
        width{inWidth}
        {
            x = position % inWidth;
            y = int(position / inWidth);
        }

        Coord (int inX, int inY, int inWidth) :
        width{inWidth} ,
        x {inX},
        y {inY}
        {}

        int Index() {
            return x + width * y;
        }

        Coord Move(int offsetX, int offsetY) {
            return Coord(x + offsetX, y + offsetY, width);
        }
    };

    struct Border {
        std::vector<Coord> borderMembers;
    };

    std::vector<Border> GetBorders(std::vector<uint8_t> &threshold, int width);
    std::vector<int> GetInside(std::vector<Border> &borders, int width);
}
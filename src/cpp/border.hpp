#pragma once

#include <vector>
#include <stdint.h>
#include "finder_patterns.hpp"

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

        FinderCandidate toFinderCandidate() {
            int minX = 10000;
            int minY = 10000;
            int maxX = 0;
            int maxY = 0;

            for (int i = 0; i < borderMembers.size(); i++) {
                if (borderMembers[i].x < minX) minX  = borderMembers[i].x;
                if (borderMembers[i].y < minY) minY  = borderMembers[i].y;
                if (borderMembers[i].x > maxX) maxX  = borderMembers[i].x;
                if (borderMembers[i].y > maxY) maxY  = borderMembers[i].y;
            }

            return FinderCandidate((minY + maxY) / 2, (minX + maxX) / 2, ((maxX - minX)  + (maxY - minY)) / 2);
        }
    };

    std::vector<Border> GetBorders(std::vector<uint8_t> &threshold, int width);
    std::vector<int> GetInside(std::vector<Border> &borders, int width);
    std::vector<FinderCandidate> Finders(std::vector<Border> &borders, std::vector<int> &validIndices, int width);
}
#include "border.hpp"

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

// flood fill
Border::Border GetBorder(std::vector<uint8_t> &threshold, int width, std::vector<uint8_t> &valid, int startPosition) {
    
    Border::Border border;
    std::vector<Coord> toVisit;
    toVisit.push_back(Coord(startPosition, width));
    uint8_t originalColour = threshold[toVisit[0].Index()];
    int height = int(threshold.size() / width);

    while(toVisit.size() > 0) {
        Coord visiting = toVisit[toVisit.size() - 1];
        toVisit.pop_back();

        if (valid[visiting.Index()] && threshold[visiting.Index()] == originalColour) {
            valid[visiting.Index()] = false;
            border.borderMembers.push_back(visiting.Index());

            if (visiting.x < width) toVisit.push_back(Coord(visiting.x + 1, visiting.y, width));
            if (visiting.x > 0) toVisit.push_back(Coord(visiting.x - 1, visiting.y, width));
            if (visiting.y < height) toVisit.push_back(Coord(visiting.x, visiting.y + 1, width));
            if (visiting.y > 0) toVisit.push_back(Coord(visiting.x, visiting.y - 1, width));
        }
    }

    return border;
}

std::vector<Border::Border> Border::GetBorders(std::vector<uint8_t> &threshold, int width) {
    std::vector<Border> borders;
    std::vector<uint8_t> valid;
    valid.resize(threshold.size(), 255);

    for(int i = 0; i < threshold.size(); i++) {
        if (valid[i] > 0) {
            borders.push_back(GetBorder(threshold, width, valid, i));
        }
    }

    return borders;
}
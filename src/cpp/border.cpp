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

struct BoundingBox {
    Coord offset = Coord(0,0,0);
    int boundingWidth = 0;
    int boundingHeight = 0;
    std::vector<uint8_t> data;

    int SurroundCount(Coord index) {
        int count = 0;

        if (index.x > 0 && data[Coord(index.x - 1, index.y, boundingWidth).Index()]) count++;
        if (index.y > 0 && data[Coord(index.x, index.y - 1, boundingWidth).Index()]) count++;
        if (index.x < boundingWidth - 1 && data[Coord(index.x + 1, index.y, boundingWidth).Index()]) count++;
        if (index.y < boundingHeight - 1 && data[Coord(index.x, index.y + 1, boundingWidth).Index()]) count++;

        return count;
    }

    BoundingBox(std::vector<Coord> &border, int width) {
        int minX = 10000000;
        int minY = 10000000;
        int maxX = 0;
        int maxY = 0;
    
        for (int i = 0; i < border.size(); i++) {
            if (border[i].x > maxX) maxX = border[i].x;
            if (border[i].y > maxY) maxY = border[i].y;
            if (border[i].x < minX) minX = border[i].x;
            if (border[i].y < minY) minY = border[i].y;
        }

        maxX ++;
        maxY ++;

        offset = Coord(minX, minY, width);
        boundingWidth = maxX - minX;
        boundingHeight = maxY - minY;

        data.resize(boundingWidth * boundingHeight);
        
        for (int i = 0; i < border.size(); i++) {
            data[Coord(border[i].x - offset.x, border[i].y - offset.y, boundingWidth).Index()] = 255;
        }
    }
};

Border::Border ReduceBorder(std::vector<Coord> border, int width) {
    BoundingBox bounds = BoundingBox(border, width);
    bool changeMade = true;

    while(changeMade) {
        changeMade = false;

        for (int i = 0; i < bounds.boundingHeight; i++) {
            for (int j=  0; j < bounds.boundingWidth; j++) {
                if (bounds.data[Coord(j, i, bounds.boundingWidth).Index()] > 0) {
                    int surround = bounds.SurroundCount(Coord(j, i, bounds.boundingWidth));

                    if (surround < 2) {
                        changeMade = true;
                        bounds.data[Coord(j, i, bounds.boundingWidth).Index()] = 0;
                    }
                }
            }
        }
    }

    Border::Border reduced;

    for (int i = 0; i < bounds.boundingHeight; i++) {
        for (int j = 0; j < bounds.boundingWidth; j++) {
            if (bounds.data[Coord(j, i, bounds.boundingWidth).Index()] > 0) {
                reduced.borderMembers.push_back(Coord(j + bounds.offset.x, i + bounds.offset.y, width).Index());
            }
        }    
    }

    return reduced;
}

// flood fill
Border::Border GetBorder(std::vector<uint8_t> &threshold, int width, std::vector<uint8_t> &valid, int startPosition) {
    
    std::vector<Coord> border;
    std::vector<Coord> toVisit;
    toVisit.push_back(Coord(startPosition, width));
    uint8_t originalColour = threshold[toVisit[0].Index()];
    int height = int(threshold.size() / width);

    while(toVisit.size() > 0) {
        Coord visiting = toVisit[toVisit.size() - 1];
        toVisit.pop_back();

        if (valid.at(visiting.Index()) && threshold.at(visiting.Index()) == originalColour) {
            valid.at(visiting.Index()) = false;
            border.push_back(visiting);

            if (visiting.x < width - 1) toVisit.push_back(Coord(visiting.x + 1, visiting.y, width));
            if (visiting.x > 0) toVisit.push_back(Coord(visiting.x - 1, visiting.y, width));
            if (visiting.y < height - 1) toVisit.push_back(Coord(visiting.x, visiting.y + 1, width));
            if (visiting.y > 0) toVisit.push_back(Coord(visiting.x, visiting.y - 1, width));
        }
    }

    return ReduceBorder(border, width);
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
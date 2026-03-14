#include "border.hpp"
#include <iostream>

struct BoundingBox {
    Border::Coord offset = Border::Coord(0,0,0);
    int boundingWidth = 0;
    int boundingHeight = 0;
    std::vector<uint8_t> data;
    std::vector<Border::Coord> borderCopy;

    int SurroundCount(Border::Coord index) {
        int count = 0;

        if (index.x > 0 && data[Border::Coord(index.x - 1, index.y, boundingWidth).Index()]) count++;
        if (index.y > 0 && data[Border::Coord(index.x, index.y - 1, boundingWidth).Index()]) count++;
        if (index.x < boundingWidth - 1 && data[Border::Coord(index.x + 1, index.y, boundingWidth).Index()]) count++;
        if (index.y < boundingHeight - 1 && data[Border::Coord(index.x, index.y + 1, boundingWidth).Index()]) count++;

        return count;
    }

    void ConvertToData() {
        data.resize(boundingWidth * boundingHeight);
        
        for (int i = 0; i < borderCopy.size(); i++) {
            data[Border::Coord(borderCopy[i].x - offset.x, borderCopy[i].y - offset.y, boundingWidth).Index()] = 255;
        }
    }

    BoundingBox(Border::Coord inOffset, int inBoundingWidth, int inBoundingHeight):
    boundingHeight{inBoundingHeight},
    boundingWidth{inBoundingWidth},
    offset{inOffset}
    {}

    BoundingBox(std::vector<Border::Coord> &border, int width) {
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

        maxX += 2;
        maxY += 2;
        minX -= 1;
        minY -= 1;

        offset = Border::Coord(minX, minY, width);
        boundingWidth = maxX - minX;
        boundingHeight = maxY - minY;

        borderCopy = border;

    }

    bool Contains(BoundingBox &toCheck) {
        if (toCheck.offset.x > this->offset.x &&
            toCheck.offset.x + toCheck.boundingWidth < this->offset.x + this->boundingWidth &&
            toCheck.offset.y > this->offset.y &&
            toCheck.offset.y + toCheck.boundingHeight < this->offset.y + this->boundingHeight
        ) {
            return true;
        }

        return false;
    }
};

struct Node {
    int originalIndex;
    BoundingBox parent;
    std::vector<Node> children;

    Node(BoundingBox &inParent, int inOriginalIndex):  
    parent{inParent},
    originalIndex{inOriginalIndex}
    {}

    void AddChild(Node &target) {
        bool added = false;

        for (int i = 0; i < children.size(); i++) {
            if (!added) {
                if (children[i].parent.Contains(target.parent)) {
                    children[i].AddChild(target);
                    added = true;
                } 
            }
            if (!added) {
                if (target.parent.Contains(children[i].parent)) {
                    target.AddChild(children[i]);
                    children[i] = target;
                    added = true;
                }
            }
        }

        if (!added) {
            children.push_back(target);
        }
    }

    int MaxDepth() {
        int maxDepth = 0;
        for (int i = 0; i < children.size(); i++) {
            int depth = children[i].MaxDepth();

            if (depth > maxDepth) {
                maxDepth = depth;
            }
        }

        return maxDepth + 1;
    }

    void PrettyPrint(int depth) {
        for (int i = 0; i < depth; i++) {
            std::cout << "  ";
        }
        std::cout << parent.offset.x << "," << parent.offset.y << " " << parent.offset.x + parent.boundingWidth << "," << parent.offset.y + parent.boundingHeight;
        std::cout << std::endl;

        for (int i = 0; i < children.size(); i++) {
            children[i].PrettyPrint(depth + 4);
        }
    }
};

Border::Coord FirstInside(BoundingBox &bounds) {
    for (int i = 0; i < bounds.boundingHeight; i++) {
        int changesX = 0;
        int previousX = bounds.data[0 + i * bounds.boundingWidth];
        for (int j = 0; j < bounds.boundingWidth; j++) {
            uint8_t currentDataX = bounds.data[j + i * bounds.boundingWidth];

            if (currentDataX != previousX) {
                changesX++;
            }             
            
            previousX = currentDataX;

            if (changesX == 1) {
                int changesY = 0;
                int previousY = bounds.data[j + 0 * bounds.boundingWidth];

                for (int k = 0; k < i; k++) {
                    uint8_t currentDataY = bounds.data[j + k * bounds.boundingWidth];

                    if (currentDataY != previousY) {
                        changesY++;
                    }

                    previousY = currentDataY;
                }

                if (changesY == 1) {
                    return Border::Coord(j, i, bounds.boundingWidth);
                }
            }
            else if (changesX > 1) {
                break;  
            }
        }
    }

    return Border::Coord(-1, -1, 0);
}

BoundingBox FloodFill(BoundingBox &original, Border::Coord startCoord, int& nFilled) {
    std::vector<Border::Coord> toFill;
    toFill.push_back(startCoord);
    uint8_t firstColour = original.data[startCoord.Index()];

    while(toFill.size() > 0) {
        // std::cout << toFill.size() << " " << original.boundingWidth << "," << original.boundingHeight << " " << startCoord.width << " " << firstColour <<  std::endl;
        Border::Coord currentPixel = toFill[toFill.size() - 1];
        toFill.pop_back();

        if (original.data[currentPixel.Index()] == firstColour) {
            original.data[currentPixel.Index()] = 255 - firstColour;
            
            if(currentPixel.x > 0) toFill.push_back(Border::Coord(currentPixel.x - 1, currentPixel.y, original.boundingWidth));
            if(currentPixel.y > 0) toFill.push_back(Border::Coord(currentPixel.x, currentPixel.y - 1, original.boundingWidth));
            if(currentPixel.x < original.boundingWidth - 1) toFill.push_back(Border::Coord(currentPixel.x + 1, currentPixel.y, original.boundingWidth));
            if(currentPixel.y < original.boundingHeight - 1) toFill.push_back(Border::Coord(currentPixel.x, currentPixel.y + 1, original.boundingWidth));

        }
        nFilled++;
    }

    return original;
}

// over approximation
bool IsDoughnut(std::vector<Border::Coord> &border, int width) {
    BoundingBox bounds = BoundingBox(border, width);
    if (bounds.boundingHeight * bounds.boundingWidth > 50000) return false; // too big
    // std::cout << "small enough" << std::endl;
    bounds.ConvertToData(); 
    Border::Coord inside = FirstInside(bounds);

    if (inside.x < 0) return true; // solid
    // return false;

    if (inside.x < 0) return false; // no interior Border::Coordinate found
    // std::cout << "about to fill" << std::endl;

    int nFilled = 0;

    BoundingBox newBounds = FloodFill(bounds, inside, nFilled);
    if (nFilled < 100) return false; // hole not big enough
    Border::Coord newInside = FirstInside(bounds);

    if (newInside.x < 0) return true; // flood fill filled the single hole
    return false;
}

std::vector<Border::Coord> ReduceBorder(std::vector<Border::Coord> border, int width) {
    BoundingBox bounds = BoundingBox(border, width);
    bounds.ConvertToData();
    bool changeMade = true;

    while(changeMade) {
        changeMade = false;

        for (int i = 0; i < bounds.boundingHeight; i++) {
            for (int j=  0; j < bounds.boundingWidth; j++) {
                if (bounds.data[Border::Coord(j, i, bounds.boundingWidth).Index()] > 0) {
                    int surround = bounds.SurroundCount(Border::Coord(j, i, bounds.boundingWidth));

                    if (surround < 2) {
                        changeMade = true;
                        bounds.data[Border::Coord(j, i, bounds.boundingWidth).Index()] = 0;
                    }
                }
            }
        }
    }

    std::vector<Border::Coord> reduced;

    for (int i = 0; i < bounds.boundingHeight; i++) {
        for (int j = 0; j < bounds.boundingWidth; j++) {
            if (bounds.data[Border::Coord(j, i, bounds.boundingWidth).Index()] > 0) {
                reduced.push_back(Border::Coord(j + bounds.offset.x, i + bounds.offset.y, width));
            }
        }    
    }

    return reduced;
}

// flood fill
Border::Border GetBorder(std::vector<uint8_t> &threshold, int width, std::vector<uint8_t> &valid, int startPosition) {
    
    if (startPosition >= 135 * width  + 236) {
        std::cout << "debug here!!!!!" << std::endl;
    }

    std::vector<Border::Coord> border;
    std::vector<Border::Coord> toVisit;
    toVisit.push_back(Border::Coord(startPosition, width));
    uint8_t originalColour = threshold[toVisit[0].Index()];
    int height = int(threshold.size() / width);

    while(toVisit.size() > 0) {
        Border::Coord visiting = toVisit[toVisit.size() - 1];
        toVisit.pop_back();

        if (valid.at(visiting.Index()) && threshold.at(visiting.Index()) == originalColour) {
            valid.at(visiting.Index()) = false;
            border.push_back(visiting);

            if (visiting.x < width - 1) toVisit.push_back(Border::Coord(visiting.x + 1, visiting.y, width));
            if (visiting.x > 0) toVisit.push_back(Border::Coord(visiting.x - 1, visiting.y, width));
            if (visiting.y < height - 1) toVisit.push_back(Border::Coord(visiting.x, visiting.y + 1, width));
            if (visiting.y > 0) toVisit.push_back(Border::Coord(visiting.x, visiting.y - 1, width));
        }
    }

    std::vector<Border::Coord> reduced = ReduceBorder(border, width);
    // std::vector<Border::Coord> reduced = border;

    if (!IsDoughnut(reduced, width)) {
        // std::cout << "Not a doughnut" << std::endl;
        reduced = std::vector<Border::Coord>();
    } else {
        // std::cout << "Doughnut found!" << std::endl;
    }

    Border::Border result;

    for (int i = 0; i < reduced.size(); i++) {
        result.borderMembers.push_back(reduced[i]);
    }

    return result;
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

Node BuildTree(std::vector<BoundingBox> &candidates, int width) {
    BoundingBox parent = BoundingBox(Border::Coord(0, width), 10000, 10000);
    Node parentNode(parent, -1);

    for (int i = 0; i < candidates.size(); i++) {
        BoundingBox candidate(candidates[i]);
        Node candidateNode(candidate, i);
        std::cout << "making " << i << " of " << candidates.size() << std::endl;
        parentNode.AddChild(candidateNode);
    }

    return parentNode;
}

std::vector<int> Border::GetInside(std::vector<Border> &borders, int width) {
    std::vector<BoundingBox> candidates;
    candidates.reserve(borders.size());
    
    std::cout << "inside start" << std::endl;



    for (int i = 0; i < borders.size(); i++) {
        candidates.push_back(BoundingBox(borders[i].borderMembers, width));
    }

    Node tree = BuildTree(candidates, width);

    tree.PrettyPrint(0);

    std::cout << "tree built!" << std::endl;

    std::vector<int> deepNodes;

    for (int i = 0; i < tree.children.size(); i++) {
        int maxDepth = tree.children[i].MaxDepth();
        if (maxDepth > 1) {
            if (tree.children[i].children.size() == 1) {
                if (tree.children[i].parent.boundingWidth < tree.children[i].children[0].parent.boundingWidth * 2) {
                    if (tree.children[i].parent.boundingHeight < tree.children[i].children[0].parent.boundingHeight * 2) {
                        deepNodes.push_back(tree.children[i].originalIndex);
                    }
                }
            }
        }
    }

    return deepNodes;
}

std::vector<FinderCandidate> SortFinders(std::vector<FinderCandidate> unsorted) {
    std::vector<FinderCandidate> sorted;
    int bestScore = -1000;
    int bestIndex = 0;

    for (int i = 0; i < unsorted.size(); i++) {
        int score = unsorted[i].y - unsorted[i].x;

        if (score > bestScore) {
            bestIndex = i;
            bestScore = score;
        }
    }
    sorted.push_back(unsorted[bestIndex]);

    bestScore = -1000;
    for (int i = 0; i < unsorted.size(); i++) {
        int score = -unsorted[i].y - unsorted[i].x;

        if (score > bestScore) {
            bestIndex = i;
            bestScore = score;
        }
    }
    sorted.push_back(unsorted[bestIndex]);

    bestScore = -1000;
    for (int i = 0; i < unsorted.size(); i++) {
        int score = -unsorted[i].y + unsorted[i].x;

        if (score > bestScore) {
            bestIndex = i;
            bestScore = score;
        }
    }
    sorted.push_back(unsorted[bestIndex]);

    bestScore = -1000;
    for (int i = 0; i < unsorted.size(); i++) {
        int score = +unsorted[i].y + unsorted[i].x;

        if (score > bestScore) {
            bestIndex = i;
            bestScore = score;
        }
    }
    sorted.push_back(unsorted[bestIndex]);

    return sorted;
}

std::vector<FinderCandidate> Border::Finders(std::vector<Border> &borders, std::vector<int> &validIndices, int width) {
    std::vector<FinderCandidate> candidates;
    std::cout << "n valid indices " << validIndices.size() << std::endl;
    for (int i = 0; i < validIndices.size(); i++) {
        FinderCandidate candidate = borders[validIndices[i]].toFinderCandidate();
        candidates.push_back(candidate);
    }

    return SortFinders(candidates);
}



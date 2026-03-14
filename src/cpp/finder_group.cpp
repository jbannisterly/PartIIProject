#include "finder_group.hpp"
#include "_config.hpp"
#include <iostream>

Position* FinderGroup::VerticalOffset(){
    FinderCandidate centre = Centre();
    double startX = centre.x - centre.width * 4 / 7;
    double startY = centre.y - centre.width * 6 / 7;
    if (startX < 0) startX = 0;
    if (startY < 0) startY = 0;

    Position* offset = new Position(startX, startY);

    return offset;
}

    // Get rectangle around candidates to check for finder pattern
VerticalData FinderGroup::VerticalSample(std::vector<uint8_t> data, int dataX, int dataY){
    FinderCandidate centre = Centre();
    Position* offset = VerticalOffset();
    int startX = offset->x;
    int startY = offset->y;

    VerticalData sample;
        
    sample.height = centre.width * 12 / 7;
    sample.width = centre.width * 8 / 7;

    if (sample.height + startY > dataY) sample.height = dataY - startY;

    sample.data = std::vector<uint8_t>();
    sample.data.reserve(sample.height * sample.width);

    for (int xx = startX; xx < startX + sample.width; xx++){
        for (int yy = startY; yy < startY + sample.height; yy++){
            // std::cout << xx << " " << yy << std::endl;
            sample.data.push_back(data[yy * dataX + xx]);
        }
    }

    return sample;
}

std::vector<FinderCandidate> FinderGroup::FinderPatternVertical(int patternSize, VerticalData vertical, std::function<bool (std::vector<int>)> patternValid, bool firstWhite){
    Position* startPosition = VerticalOffset();
    std::vector<FinderCandidate> verticalCandidates = FinderPatterns::FinderPatterns(patternSize, vertical.data, vertical.width, vertical.height, patternValid, firstWhite);

    for (int i = 0; i < verticalCandidates.size(); i++){
        double temp;
        temp = verticalCandidates[i].x;
        verticalCandidates[i].x = verticalCandidates[i].y + startPosition->x;
        verticalCandidates[i].y = temp + startPosition->y;
    }

    // free(startPosition);

    return verticalCandidates;
}

bool FinderGroup::TryAddCandidate(FinderCandidate toAdd){
    if (candidates.size() == 0){
        candidates.push_back(toAdd);
        return true;
    }

    if (toAdd.width > candidates[0].width + TOLERANCE_PIXELS) return false;
    if (toAdd.width < candidates[0].width - TOLERANCE_PIXELS) return false;
    if (toAdd.x > candidates[0].x + 30) return false;
    if (toAdd.x < candidates[0].x - 30) return false;
    if (toAdd.y > candidates[0].y + candidates[0].width * 3 / 7) return false;
    if (toAdd.y < candidates[0].y - candidates[0].width * 3 / 7) return false;

    candidates.push_back(toAdd);
    return true;
}

bool FinderGroup::isValid(std::vector<uint8_t> threshold, int dataX, int dataY, int patternSize, std::function<bool (std::vector<int>)> patternValid, bool firstWhite){
    VerticalData vertical = VerticalSample(threshold, dataX, dataY);

    if (vertical.data.size() == 0) return false;
    verticalCandidates = FinderPatternVertical(patternSize, vertical, patternValid, firstWhite);

    bool verticalFound = verticalCandidates.size() > 0;

    return verticalFound;
}

int FinderGroup::size(){
    return candidates.size();
}

FinderCandidate FinderGroup::Centre(){
    FinderCandidate centre(0,0,0);

    for (int i = 0; i < candidates.size(); i++){
        centre.x += candidates[i].x;
        centre.y += candidates[i].y;
        centre.width += candidates[i].width;
    }

    centre.x /= candidates.size();
    centre.y /= candidates.size();
    centre.width /= candidates.size();

    return centre;
}

FinderCandidate FinderGroup::CentreRefined(){
    FinderCandidate centre = Centre();

    if (verticalCandidates.size() > 0){
        int nValidCandidates = 0;

        std::cout << "would have been " << centre.y << std::endl;

        centre.y = 0;
        for (int i = 0; i < verticalCandidates.size(); i++){
            if (verticalCandidates[i].width > 0){
                nValidCandidates++;
                centre.y += verticalCandidates[i].y;
                // std::cout << verticalCandidates[i].width << std::endl;
            }
        }
        if (nValidCandidates > 0){
            centre.y /= nValidCandidates;
        }else{
            return Centre();
        }

        std::cout << "vertical gives " << centre.y << std::endl;
        std::cout << "using " << nValidCandidates << " samples" << std::endl;
    }

    return centre;
}

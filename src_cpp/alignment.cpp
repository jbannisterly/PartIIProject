#include "_config.hpp"
#include "alignment.hpp"
#include <opencv2/opencv.hpp>
#include "image_aux.hpp"
#include <vector>
#include <stdio.h>
#include "vector_helper.hpp"

using namespace cv;

uint8_t* Debug;

struct Position{
    int x;
    int y;

    Position(int inX, int inY){
        x = inX;
        y = inY;
    }
};

float* Greyscale(uint8_t* image, int pixels){
    float* grey = (float*)malloc(sizeof(float) * pixels);

    for (int i = 0; i < pixels; i++){
        grey[i] = (77 * image[i * 3] + 151 * image[i * 3 + 1] + 28 * image[i * 3 + 2]) / 256;
    }

    return grey;
}

int GetIndexPadding(int nY, int nX, int y, int x){
    int yy = y > 0 ? (y < nY ? y : nY - 1) : 0;
    int xx = x > 0 ? (x < nX ? x : nX - 1) : 0;
    return yy * nX + xx;
}

uint8_t* Threshold(float* image, int nY, int nX){
    uint8_t* threshold = (uint8_t*)malloc(sizeof(uint8_t) * nY * nX);
    const float WINDOW_SCALE = (2 * WINDOW_SIZE + 1) * (2 * WINDOW_SIZE + 1);

    for (int yy = 0; yy < nY; yy++){
        float m = 0;
        for (int xi = -WINDOW_SIZE; xi <= WINDOW_SIZE; xi++){
            for (int yi = -WINDOW_SIZE; yi <= WINDOW_SIZE; yi++){
                m += image[GetIndexPadding(nY, nX, yy + yi, xi - 1)] / WINDOW_SCALE;
            }
        }
        for (int xx = 0; xx < nX; xx++){
            for (int yi = -WINDOW_SIZE; yi <= WINDOW_SIZE; yi++){
                m += image[GetIndexPadding(nY, nX, yy + yi, xx + WINDOW_SIZE)] / WINDOW_SCALE;
                m -= image[GetIndexPadding(nY, nX, yy + yi, xx - WINDOW_SIZE)] / WINDOW_SCALE;
            }

            if (image[yy * nX + xx] > 180){
                threshold[yy * nX + xx] = 255;
            }else{
                if(image[yy * nX + xx] * 1.1 >= m - 10){
                    threshold[yy * nX + xx] = 255;
                }else{
                    threshold[yy * nX + xx] = 0;
                }
            }
        }
    }

    return threshold;
}

int CountSame(uint8_t* data, int index, int size){
    int i = 0; 
    uint8_t value = data[index];
    while(index + i < size && data[index + i] == value){
        i++;
    }

    return i;
}

struct FinderCandidate{
    int y;
    int x;
    int width;

    FinderCandidate(int initY, int initX, int initW){
        y = initY;
        x = initX;
        width = initW;
    }
};

std::vector<FinderCandidate> FinderPatterns(uint8_t* data, int sizeY, int sizeX){
    int index = 0;
    int w[5];
    int wIndex = 0;
    int size = sizeX * sizeY;
    std::vector<FinderCandidate> finder;
    
    for (int i = 0; i < 5; i++){
        w[i] = CountSame(data, wIndex, size);
        wIndex += w[i];
    }

    while(wIndex < size){
        float avg = 0;
        for (int i = 0; i < 5; i++) {
            avg += w[i] / 7;
        }
        bool valid = true;
        for (int i = 0; i < 5; i++){
            if (i == 2){
                if (w[i] > 3 * avg + TOLERANCE_PIXELS) valid = false; // tolerance is 10x more than paper
                if (w[i] < 3 * avg - TOLERANCE_PIXELS) valid = false;
                if (w[2] < w[0] + w[1]) valid = false;
                if (w[2] < w[3] + w[4]) valid = false;
            }else{
                if (w[i] > avg + TOLERANCE_PIXELS) valid = false;
                if (w[i] < avg - TOLERANCE_PIXELS * 0.75) valid = false;
            }
        }

        if(valid && data[wIndex] > 0){
            int centre = int(wIndex - w[4] - w[3] - w[2] / 2);
            int width = avg * 7;
            finder.push_back(FinderCandidate(centre / sizeX, centre % sizeX, width));
            for (int counter = int(-avg * 7 / 2); counter < avg * 7 / 2; counter++){
                // data[centre + counter] = 127; // visualise centre
            }
        }

        // Read another
        for (int i = 0; i < 4; i++){
            w[i] = w[i + 1];
        }
        for (int i = 4; i < 5; i++){
            w[i] = CountSame(data, wIndex, size);
            wIndex += w[i];
        }
    }

    return finder;
} 

struct VerticalData{
    uint8_t* data;
    int height;
    int width;
};

class FinderGroup{
    public:
    FinderCandidate Centre();
    FinderCandidate CentreRefined();

    private:
    std::vector<FinderCandidate> candidates;
    std::vector<FinderCandidate> verticalCandidates;

    Position* VerticalOffset(){
        FinderCandidate centre = Centre();
        int startX = centre.x - centre.width * 1.3 / 7;
        int startY = centre.y - centre.width * 5.5 / 7;
        if (startX < 0) startX = 0;
        if (startY < 0) startY = 0;

        Position* offset = new Position(startX, startY);

        return offset;
    }

    // Get rectangle around candidates to check for finder pattern
    VerticalData VerticalSample(uint8_t* data, int dataX, int dataY){
        FinderCandidate centre = Centre();
        Position* offset = VerticalOffset();
        int startX = offset->x;
        int startY = offset->y;

        VerticalData sample;
        
        sample.height = centre.width * 11 / 7;
        sample.width = centre.width * 2.6 / 7;

        sample.data = (uint8_t*)malloc(sizeof(uint8_t) * sample.height * sample.width);

        int index = 0;
        for (int xx = startX; xx < startX + sample.width; xx++){
            for (int yy = startY; yy < startY + sample.height; yy++){
                sample.data[index] = data[yy * dataX + xx];
                index++;
            }
        }

        return sample;
    }

    std::vector<FinderCandidate> FinderPatternVertical(VerticalData vertical){
        Position* startPosition = VerticalOffset();
        std::vector<FinderCandidate> verticalCandidates = FinderPatterns(vertical.data, vertical.width, vertical.height);

        for (int i = 0; i < verticalCandidates.size(); i++){
            int temp;
            temp = verticalCandidates[i].x;
            verticalCandidates[i].x = verticalCandidates[i].y + startPosition->x;
            verticalCandidates[i].y = temp + startPosition->y;
        }

        free(startPosition);

        return verticalCandidates;
    }

    public:
    bool TryAddCandidate(FinderCandidate toAdd){
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

    bool isValid(uint8_t* imageData, int dataX, int dataY){
        VerticalData vertical = VerticalSample(imageData, dataX, dataY);
        verticalCandidates = FinderPatternVertical(vertical);

        bool verticalFound = verticalCandidates.size() > 0;

        return verticalFound;
    }

    int size(){
        return candidates.size();
    }
};

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

    return centre;

    if (verticalCandidates.size() > 0){
        int validCandidates = 0;

        std::cout << "would have been " << centre.y << std::endl;

        centre.y = 0;
        for (int i = 0; i < verticalCandidates.size(); i++){
            if (verticalCandidates[i].width > 0){
                validCandidates++;
                centre.y += verticalCandidates[i].y;
                std::cout << verticalCandidates[i].width << std::endl;
            }
        }
        if (validCandidates > 0){
            centre.y /= validCandidates;
        }else{
            return Centre();
        }

        std::cout << "vertical gives " << centre.y << std::endl;
        std::cout << "using " << validCandidates << " samples" << std::endl;
    }

    return centre;
}

std::vector<FinderGroup> GroupFinders(std::vector<FinderCandidate> candidates){
    std::vector<FinderGroup> finderGroup;

    for (int i = 0; i < candidates.size(); i++){
        int j = 0;
        bool newNeeded = true;
        while(j < finderGroup.size()){
            if (finderGroup[j].TryAddCandidate(candidates[i])){
                j = finderGroup.size();
                newNeeded = false;
            }
            j++;
        }
        if (newNeeded) {
            FinderGroup newGroup = FinderGroup();
            newGroup.TryAddCandidate(candidates[i]);
            finderGroup.push_back(newGroup);
        }
    }

    return finderGroup;
}

uint8_t* Uint8ToPixels(uint8_t* data, int nPixels){
    uint8_t* pixels = (uint8_t*)malloc(sizeof(uint8_t) * nPixels * 3);

    for (int i = 0; i < nPixels; i++){
        if (Debug[i] == 0){
            pixels[i * 3] = data[i];
            pixels[i * 3 + 1] = data[i];
            pixels[i * 3 + 2] = data[i];
        }else{
            pixels[i * 3 + 2] = Debug[i];
        }
    }

    return pixels;
}

uint8_t* FloatToPixels(float* floats, int nPixels){
    uint8_t* pixels = (uint8_t*)malloc(sizeof(uint8_t) * nPixels * 3);

    for (int i = 0; i < nPixels; i++){
        pixels[i * 3] = uint8_t(floats[i]);
        pixels[i * 3 + 1] = uint8_t(floats[i]);
        pixels[i * 3 + 2] = uint8_t(floats[i]);
    }

    return pixels;
}

std::vector<FinderCandidate> GetCentres(std::vector<FinderGroup> finderGroups, std::vector<int> finderGroupsValid){
    std::vector<FinderCandidate> centres;

    int maxSize = finderGroups[finderGroupsValid[0]].size();
    for (int i = 0; i < finderGroupsValid.size(); i++){
        if(finderGroups[finderGroupsValid[i]].size() * 3 > maxSize){
            FinderCandidate centre = finderGroups[finderGroupsValid[i]].CentreRefined();
            centres.push_back(centre);
        }else{
            break;
        }
    }

    return centres;
}

int DistanceSquared(FinderCandidate a, FinderCandidate b){
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

FinderCandidate* OrderCentres(std::vector<FinderCandidate> centres){
    int sumDistanceSquared[3];
    for (int i = 0; i < 3; i++){
        sumDistanceSquared[i] = 0;
        for (int j = 0; j < 3; j++){
            sumDistanceSquared[i] += DistanceSquared(centres[i], centres[j]);
        }
    }

    int minSum = sumDistanceSquared[0];
    int minIndex = 0;
    for (int i = 0; i < 3; i++){
        if (sumDistanceSquared[i] < minSum){
            minSum = sumDistanceSquared[i];
            minIndex = i;
        }
    }    
    
    bool clockwise = Clockwise(
        Vec3(centres[0].x, centres[0].y, 0),
        Vec3(centres[1].x, centres[1].y, 0),
        Vec3(centres[2].x, centres[2].y, 0)
    );

    // p1 -- p2
    // |
    // |
    // p0

    FinderCandidate* candidatesSorted = (FinderCandidate*)malloc(sizeof(FinderCandidate) * 3);
    candidatesSorted[1] = centres[minIndex];
    if (clockwise){
        candidatesSorted[0] = centres[(minIndex + 2) % 3];
        candidatesSorted[2] = centres[(minIndex + 1) % 3];
    }else{
        candidatesSorted[0] = centres[(minIndex + 1) % 3];
        candidatesSorted[2] = centres[(minIndex + 2) % 3];
    }

    return candidatesSorted;
}

#if METHOD_PATTERN_4 == 1
void BoundingBoxMissingCorner(FinderCandidate* finders, Vec3* findersVec, Vec3* bounds){
    bounds[3] = bounds[0] + bounds[2] - bounds[1];
}
#endif

#if METHOD_PATTERN_4 == 2
void BoundingBoxMissingCorner(FinderCandidate* finders, Vec3* findersVec, Vec3* bounds){
    const float MODULEOFFSET = 4.5;
    Vec3 bottomLeftDir = (Normalise(findersVec[0] - findersVec[1]) * finders[0].width * (MODULEOFFSET / 7)) + findersVec[0] - bounds[0];
    Vec3 topRightDir = (Normalise(findersVec[2] - findersVec[1]) * finders[2].width * (MODULEOFFSET / 7)) + findersVec[2] - bounds[2];

    bounds[3] = Intersection(bottomLeftDir, topRightDir, bounds[0], bounds[2]);    
}
#endif

Vec3* BoundingBox(FinderCandidate* finders){
    const float MODULEOFFSET = 4.5;

    Vec3* bounds = (Vec3*)malloc(sizeof(Vec3) * 4);
    Vec3* findersVec = (Vec3*)malloc(sizeof(Vec3) * 3);
    
    for (int i = 0; i < 3; i++){
        findersVec[i] = Vec3(finders[i].x, finders[i].y, 0);
    }

    std::cout << finders[0].width;

    bounds[0] = findersVec[0] + (Normalise(findersVec[0] - findersVec[2]) * finders[0].width * sqrt(2) * (MODULEOFFSET / 7));
    bounds[1] = findersVec[1] + ((Normalise(findersVec[1] - findersVec[2]) + 
                Normalise(findersVec[1] - findersVec[0]))
                * finders[1].width) * (MODULEOFFSET / 7);
    bounds[2] = findersVec[2] + (Normalise(findersVec[2] - findersVec[0]) * finders[2].width * sqrt(2) * (MODULEOFFSET / 7));

    BoundingBoxMissingCorner(finders, findersVec, bounds);

    free(findersVec);

    return bounds;
}

Mat Project(Mat input, int* inputCoords, Position size){
    Mat output;

    const int cornerIndices[4] = {1, 2, 0, 3};

    Point2f startCoords[4];
    for (int i = 0; i < 4; i++){
        startCoords[i] = Point2f(inputCoords[cornerIndices[i] * 2], inputCoords[cornerIndices[i] * 2 + 1]);
        std::cout << startCoords[i] << std::endl;
    }

    Point2f endCoords[4];
    endCoords[0] = Point2f(0,0);
    endCoords[1] = Point2f(size.x - 1, 0);
    endCoords[2] = Point2f(0, size.y - 1);
    endCoords[3] = Point2f(size.x - 1, size.y - 1);

    Mat transform = getPerspectiveTransform(startCoords, endCoords);

    std::cout << transform;

    warpPerspective(input, output, transform, Size(size.x, size.y));

    return output;
}

int main(){
    const char* filePath = "output/output_distorted.png";
    const char* filePathOut = "output/output_align.png";
    const char* filePathDebug = "output/output_debug.png";

    Mat image = imread(filePath);
    int nPixels = image.cols * image.rows; 
    Debug = (uint8_t*)malloc(sizeof(uint8_t) * nPixels);
    for(int i = 0; i < nPixels; i++) Debug[i] = 0;

    uint8_t* data = MatToBytes(image);

    float* grey = Greyscale(data, nPixels);
    uint8_t* threshold = Threshold(grey, image.rows, image.cols);


    std::vector<FinderCandidate> finder = FinderPatterns(threshold, image.rows, image.cols);
    std::vector<FinderGroup> finderGroups = GroupFinders(finder);

    std::vector<int> finderGroupsValid;

    for (int i = 0; i < finderGroups.size(); i++){
        bool valid = finderGroups[i].isValid(data, image.cols, image.rows);
        if (valid){
            finderGroupsValid.push_back(i);
        }
    }

    int size[finderGroups.size()];

    for (int i = 0; i < finderGroups.size(); i++){
        size[i] = finderGroups[i].size();
    }

    std::sort(finderGroupsValid.begin(), finderGroupsValid.end(), [&size](int a, int b){
        return size[a] > size[b];
    });

    std::vector<FinderCandidate> centres = GetCentres(finderGroups, finderGroupsValid);
    FinderCandidate* centresSorted = OrderCentres(centres);

    Vec3* bounds = BoundingBox(centresSorted);

    for (int i = 0; i < 4; i++){
        std::cout << bounds[i].x << "," << bounds[i].y << std::endl;
        std::cout << centres[i].x << "," << centres[i].y << std::endl;
        for (int j = -10; j < 10; j++){
            Debug[int(centres[i].x) + int(centres[i].y) * image.cols + j] = 255;
            Debug[int(centres[i].x) + (int(centres[i].y) + j) * image.cols] = 255;
            Debug[int(bounds[i].x) + int(bounds[i].y) * image.cols + j] = 127;
            Debug[int(bounds[i].x) + (int(bounds[i].y) + j) * image.cols] = 127;

        }
    }

    uint8_t* pixels = Uint8ToPixels(threshold, nPixels);

    Mat outputImage;
    Mat debugImage(image.rows, image.cols, CV_8UC3);
    debugImage.data = pixels;

    int* projectCoords = (int*)malloc(sizeof(int) * 8);
    for (int i = 0; i < 4; i++){
        projectCoords[i * 2] = bounds[i].x;
        projectCoords[i * 2 + 1] = bounds[i].y;
    }

    outputImage = Project(image, projectCoords, Position(27, 27)); 

    imwrite(filePathOut, outputImage);
    imwrite(filePathDebug, debugImage);
}
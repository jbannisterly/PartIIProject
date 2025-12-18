#include "_config.hpp"
#include "alignment.hpp"
#include <opencv2/opencv.hpp>
#include "image_aux.hpp"
#include <vector>
#include <stdio.h>
#include "vector_helper.hpp"
#include "profiling.hpp"

using namespace cv;

std::vector<uint8_t> Debug;

struct Position{
    double x;
    double y;

    Position(double inX, double inY){
        x = inX;
        y = inY;
    }
};

std::vector<double> Greyscale(uint8_t* image, int pixels){
    std::vector<double> grey;
    grey.reserve(pixels);;

    for (int i = 0; i < pixels; i++){
        grey[i] = ((77 * image[i * 3] + 151 * image[i * 3 + 1] + 28 * image[i * 3 + 2]) / 256);
    }

    return grey;
}

int GetIndexPadding(int nY, int nX, int y, int x){
    int yy = y > 0 ? (y < nY ? y : nY - 1) : 0;
    int xx = x > 0 ? (x < nX ? x : nX - 1) : 0;
    return yy * nX + xx;
}

double* GetBlurred(std::vector<double> &imageData, int nY, int nX){
    Mat data = Mat(nY, nX, CV_64F, imageData.data());
    Mat* blurred = new Mat(nY, nX, CV_64F);

    std::cout << "pre box filter" << std::endl;

    boxFilter(data, *blurred, -1, Size(WINDOW_SIZE * 2 + 1, WINDOW_SIZE * 2 + 1), Point(-1, -1), true, CV_HAL_BORDER_REPLICATE);

    std::cout << "post box filter" << std::endl;

    return (double*)blurred->ptr();
}

std::vector<uint8_t> Threshold(std::vector<double> &image, int nY, int nX){
    Timer t = Timer();
    t.StartTimer();

    std::vector<uint8_t> threshold;
    threshold.reserve(nY * nX);
    const double WINDOW_SCALE = (2 * WINDOW_SIZE + 1) * (2 * WINDOW_SIZE + 1);
    
#if BLUR_FORMULA == 1
    double* blurred = GetBlurred(image, nY, nX);
#endif

    std::cout << "did blur" << std::endl;

    for (int yy = 0; yy < nY; yy++){
#if BLUR_FORMULA == 0
        double m = 0;
        for (int xi = -WINDOW_SIZE; xi <= WINDOW_SIZE; xi++){
            for (int yi = -WINDOW_SIZE; yi <= WINDOW_SIZE; yi++){
                m += image[GetIndexPadding(nY, nX, yy + yi, xi - 1)] / WINDOW_SCALE;
            }
        }
#endif
        for (int xx = 0; xx < nX; xx++){
#if BLUR_FORMULA == 0
            for (int yi = -WINDOW_SIZE; yi <= WINDOW_SIZE; yi++){
                m += image[GetIndexPadding(nY, nX, yy + yi, xx + WINDOW_SIZE)] / WINDOW_SCALE;
                m -= image[GetIndexPadding(nY, nX, yy + yi, xx - WINDOW_SIZE)] / WINDOW_SCALE;
            }
#endif

            // if (m != blurred[yy * nX + xx]){
            //     std::cout << m << " " << blurred[yy * nX + xx] << std::endl;
            // }

            if (image[yy * nX + xx] > 180){
                threshold.push_back(255);
            }else{
#if BLUR_FORMULA == 0
                if(image[yy * nX + xx] * 1.1 >= m - 10){
#endif
#if BLUR_FORMULA == 1
                if(image[yy * nX + xx] * 1.1 >= blurred[yy * nX + xx] - 10){
#endif
                    threshold.push_back(255);
                }else{
                    threshold.push_back(0);
                }
            }
        }
    }

    std::cout << t.GetElapsed() << std::endl;

    return threshold;
}

int CountSame(std::vector<uint8_t> &data, int index, int size){
    int i = 0; 
    uint8_t value = data[index];

    while(index + i < size && data[index + i] == value){
        i++;
    }

    return i;
}

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

};

std::vector<FinderCandidate> FinderPatterns(std::vector<uint8_t> &data, int sizeY, int sizeX){
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
        double avg = 0;
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
            double centre = wIndex - w[4] - w[3] - w[2] / 2;
            double width = avg * 7;
            finder.push_back(FinderCandidate(centre / sizeX, centre - int(centre / sizeX) * sizeX, width));
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

    // std::cout << "Finder size: " << finder.size() << std::endl;

    // for (int i = 0; i < finder.size(); i++){
    //     std::cout << ((finder))[i].y;
    // }
    // std::cout << std::endl;

    // std::cout << "Returning finder" << std::endl;

    return finder;
} 

struct VerticalData{
    std::vector<uint8_t> data;
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
        double startX = centre.x - centre.width * 1.3 / 7;
        double startY = centre.y - centre.width * 5.5 / 7;
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

        sample.data = std::vector<uint8_t>();
        sample.data.reserve(sample.height * sample.width);

        for (int xx = startX; xx < startX + sample.width; xx++){
            for (int yy = startY; yy < startY + sample.height; yy++){
                sample.data.push_back(data[yy * dataX + xx]);
            }
        }

        return sample;
    }

    std::vector<FinderCandidate> FinderPatternVertical(VerticalData vertical){
        Position* startPosition = VerticalOffset();
        std::vector<FinderCandidate> verticalCandidates = FinderPatterns(vertical.data, vertical.width, vertical.height);

        for (int i = 0; i < verticalCandidates.size(); i++){
            double temp;
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

        if (vertical.data.size() == 0) return false;
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

std::vector<FinderGroup> GroupFinders(std::vector<FinderCandidate> &candidates){
    std::vector<FinderGroup> finderGroup;

    for (int i = 0; i < candidates.size(); i++){
        int j = 0;
        bool newNeeded = true;
        while(j < finderGroup.size()){
            if (finderGroup.at(j).TryAddCandidate(candidates[i])){
                j = finderGroup.size();
                newNeeded = false;
            }
            j++;
        }
        if (newNeeded) {
            FinderGroup newGroup = FinderGroup();
            newGroup.TryAddCandidate(candidates.at(i));
            finderGroup.push_back(newGroup);
        }
    }

    return finderGroup;
}

std::vector<uint8_t> Uint8ToPixels(std::vector<uint8_t> &data){
    std::vector<uint8_t> pixels;
    pixels.reserve(data.size() * 3);

    for (int i = 0; i < data.size(); i++){
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

std::vector<uint8_t> DoubleToPixels(double* doubles, int nPixels){
    std::vector<uint8_t> pixels;
    pixels.reserve(nPixels * 3);
    
    for (int i = 0; i < nPixels; i++){
        pixels[i * 3] = uint8_t(doubles[i]);
        pixels[i * 3 + 1] = uint8_t(doubles[i]);
        pixels[i * 3 + 2] = uint8_t(doubles[i]);
    }

    return pixels;
}

std::vector<FinderCandidate> GetCentres(std::vector<FinderGroup> &finderGroups, std::vector<int> &finderGroupsValid){
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

std::array<FinderCandidate, 3> OrderCentres(std::vector<FinderCandidate> &centres){
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

    std::array<FinderCandidate, 3> candidatesSorted;
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
void BoundingBoxMissingCorner(std::array<FinderCandidate, 3> &finders, std::array<Vec3, 3>  &findersVec, std::array<Vec3, 4> &bounds){
    const double MODULEOFFSET = 4.5;
    Vec3 bottomLeftDir = ((findersVec[0] - findersVec[1]).Normalise() * finders[0].width * (MODULEOFFSET / 7)) + findersVec[0] - bounds[0];
    Vec3 topRightDir = ((findersVec[2] - findersVec[1]).Normalise() * finders[2].width * (MODULEOFFSET / 7)) + findersVec[2] - bounds[2];

    bounds[3] = Intersection(bottomLeftDir, topRightDir, bounds[0], bounds[2]);    
}
#endif

#if METHOD_PATTERN_4 == 3
void BoundingBoxMissingCorner(std::array<FinderCandidate, 3> &finders, std::array<Vec3, 3>  &findersVec, std::array<Vec3, 4> &bounds){
    const double MODULEOFFSET = 4.5;
    Vec3 bottomLeftDir = bounds[1] - bounds[2];
    Vec3 topRightDir = bounds[1] - bounds[0];

    bounds[3] = Intersection(bottomLeftDir, topRightDir, bounds[0], bounds[2]);    
}
#endif

std::array<Vec3, 4> BoundingBox(std::array<FinderCandidate, 3> finders){
    const double MODULEOFFSET = 4.5;

    std::array<Vec3, 3> findersVec;
    std::array<Vec3, 4> bounds;
    
    for (int i = 0; i < 3; i++){
        findersVec[i] = Vec3(finders[i].x, finders[i].y, 0);
    }

    std::cout << finders[0].width << std::endl;

    bounds[0] = findersVec[0] + ((findersVec[0] - findersVec[2]).Normalise() * finders[0].width * sqrt(2) * (MODULEOFFSET / 7));
    bounds[1] = findersVec[1] + (((findersVec[1] - findersVec[2]).Normalise() + 
                (findersVec[1] - findersVec[0]).Normalise())
                * finders[1].width) * (MODULEOFFSET / 7);
    bounds[2] = findersVec[2] + ((findersVec[2] - findersVec[0]).Normalise() * finders[2].width * sqrt(2) * (MODULEOFFSET / 7));

    BoundingBoxMissingCorner(finders, findersVec, bounds);

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

    std::cout << transform << std::endl;

    warpPerspective(input.clone(), output, transform, Size(size.x, size.y));

    return output;
}

int EstimateBarcodeSize(std::array<Vec3, 4> bounds, Vec3* centres){
    double centreDistanceH = (centres[1] - centres[2]).Magnitude();
    double boundsDistanceH = (bounds[1] - bounds[2]).Magnitude();

    double centreDistanceV = (centres[0] - centres[1]).Magnitude();
    double boundsDistanceV = (bounds[0] - bounds[1]).Magnitude();


    double sizeEstimateH = 8 * centreDistanceH / (boundsDistanceH - centreDistanceH) + 8;
    double sizeEstimateV = 8 * centreDistanceV / (boundsDistanceV - centreDistanceV) + 8;

    std::cout << "estimated width:  " << sizeEstimateV << std::endl;
    std::cout << "estimated height: " << sizeEstimateH << std::endl;
    return int((sizeEstimateH + sizeEstimateV) / 2);
}

Mat AlignImage(Mat inputImage, int projectionSize, int pixelOffsetExpand){
    int nPixels = inputImage.cols * inputImage.rows; 
    Debug.reserve(nPixels);
    for(int i = 0; i < nPixels; i++) Debug[i] = 0;

    uint8_t* data = MatToBytes(inputImage);

    std::vector<double> grey = Greyscale(data, nPixels);
    std::vector<uint8_t> threshold = Threshold(grey, inputImage.rows, inputImage.cols);

    std::cout << "Threshold" << std::endl;

    std::vector<FinderCandidate> finder = FinderPatterns(threshold, inputImage.rows, inputImage.cols);

    std::cout << "Copied finder" << std::endl;

    std::cout << finder.size() << std::endl;

    std::vector<FinderGroup> finderGroups = GroupFinders(finder);

    std::cout << "Finder Groups" << std::endl;
    std::cout << finderGroups.size() << std::endl;

    std::vector<int> finderGroupsValid;

    for (int i = 0; i < finderGroups.size(); i++){
        bool valid = finderGroups[i].isValid(data, inputImage.cols, inputImage.rows);
        if (valid){
            finderGroupsValid.push_back(i);
        }
    }

    std::cout << finderGroupsValid.size() << std::endl;

    int size[finderGroups.size()];

    for (int i = 0; i < finderGroups.size(); i++){
        size[i] = finderGroups[i].size();
    }

    std::sort(finderGroupsValid.begin(), finderGroupsValid.end(), [&size](int a, int b){
        return size[a] > size[b];
    });

    std::cout << "sorted" << std::endl;

    std::vector<FinderCandidate> centres = GetCentres(finderGroups, finderGroupsValid);
    std::array<FinderCandidate, 3> centresSorted = OrderCentres(centres);

    std::cout << "centres" << std::endl;

    std::array<Vec3, 4> bounds = BoundingBox(centresSorted);

    // TODO: check bounds of bounds and centres so they are in the image
    // centres is length 3, but 4 are accessed

    for (int i = 0; i < 3; i++){
        std::cout << bounds[i].x << "," << bounds[i].y << std::endl;
        std::cout << centres[i].x << "," << centres[i].y << std::endl;
        for (int j = -10; j < 10; j++){
            Debug[int(centres[i].x) + int(centres[i].y) * inputImage.cols + j] = 255;
            Debug[int(centres[i].x) + (int(centres[i].y) + j) * inputImage.cols] = 255;
            Debug[int(bounds[i].x) + int(bounds[i].y) * inputImage.cols + j] = 127;
            Debug[int(bounds[i].x) + (int(bounds[i].y) + j) * inputImage.cols] = 127;
        }
    }

    std::vector<uint8_t> pixels = Uint8ToPixels(threshold);

    Mat outputImage;

    int projectCoords[8];
    for (int i = 0; i < 4; i++){
        projectCoords[i * 2] = bounds[i].x;
        projectCoords[i * 2 + 1] = bounds[i].y;
    }

    Vec3 centresVec[3];
    for (int i = 0; i < 4; i++){
        centresVec[i] = Vec3(centresSorted[i].x, centresSorted[i].y, 0);
    }

    const int adjustmentDirection[8] = {-1, 1, -1, -1, 1, -1, 1, 1};
    int* projectCoordsAdjusted = new int[8];
    for (int i = 0; i < 8; i++){
        projectCoordsAdjusted[i] = projectCoords[i] + pixelOffsetExpand * adjustmentDirection[i];
    }

    EstimateBarcodeSize(bounds, centresVec);

    std::cout << "Estimated size" << std::endl;

    outputImage = Project(inputImage, projectCoordsAdjusted, Position(projectionSize, projectionSize)); 

    std::cout << "Projected image" << std::endl;

    Mat debugImage(inputImage.rows, inputImage.cols, CV_8UC3, pixels.data());
    imwrite("output/output_debug2.png", debugImage);

    return outputImage;
}

int main(){
    std::string filePathOut = "output/output_align_";
    const char* filePath = "output/output_distorted.png";
    // const char* filePathOut = "output/output_align_";
    const char* filePathOutFinal = "ouput/output_align.png";
    const char* filePathDebug = "output/output_debug.png";
    const int ITERATIONS = 3;

    Mat image = imread(filePath);
    Mat nextImage;

    for (int i = 0; i < ITERATIONS; i++){
        nextImage = AlignImage(image.clone(), image.rows, 100);
        imwrite(filePathOut + std::to_string(i) + ".png", nextImage);
        image = nextImage;
    }

    
    Mat outputImage = AlignImage(nextImage, BARCODE_SIZE, 0);
    std::cout << "writing the final image" << std::endl;
    imwrite(filePathOutFinal, outputImage);
    std::cout << "Finished program" << std::endl;
}
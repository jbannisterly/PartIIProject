#include "_config.hpp"
#include "alignment.hpp"
#include <opencv2/opencv.hpp>
#include "image_aux.hpp"
#include <vector>
#include <stdio.h>
#include "vector_helper.hpp"
#include "profiling.hpp"
#include <functional>
#include "image_aux.hpp"
#include "image_processing.hpp"
#include "image_debug.hpp"
#include "finder_group.hpp"
#include "finder_patterns.hpp"
#include "pattern_valid.hpp"

using namespace cv;

int GetIndexPadding(int nY, int nX, int y, int x){
    int yy = y > 0 ? (y < nY ? y : nY - 1) : 0;
    int xx = x > 0 ? (x < nX ? x : nX - 1) : 0;
    return yy * nX + xx;
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

std::vector<uint8_t> Uint8ToPixels(std::vector<uint8_t> &greyscale){
    std::vector<uint8_t> pixels;
    pixels.reserve(greyscale.size() * 3);

    for (int i = 0; i < greyscale.size(); i++){
        pixels[i * 3] = greyscale[i];
        pixels[i * 3 + 1] = greyscale[i];
        pixels[i * 3 + 2] = greyscale[i];
    }

    return pixels;
}

std::vector<uint8_t> DoubleToPixels(std::vector<double> &doubles){
    std::vector<uint8_t> pixels;
    pixels.reserve(doubles.size() * 3);
    
    for (int i = 0; i < doubles.size(); i++){
        pixels[i * 3] = uint8_t(doubles[i]);
        pixels[i * 3 + 1] = uint8_t(doubles[i]);
        pixels[i * 3 + 2] = uint8_t(doubles[i]);
    }

    return pixels;
}

std::vector<FinderCandidate> GetCentres(std::vector<FinderGroup> &finderGroups, std::vector<int> &finderGroupsValidIndex){
    std::vector<FinderCandidate> centres;

    int maxSize = finderGroups[finderGroupsValidIndex[0]].size();
    for (int i = 0; i < finderGroupsValidIndex.size(); i++){
        if(finderGroups[finderGroupsValidIndex[i]].size() * 3 > maxSize){
            FinderCandidate centre = finderGroups[finderGroupsValidIndex[i]].CentreRefined();
            centres.push_back(centre);
            // std::cout << "Centre size " << centre.width << " Centre count " << finderGroups[finderGroupsValidIndex[i]].size() << std::endl;
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

    bounds[0] = findersVec[0] + ((findersVec[0] - findersVec[2]).Normalise() * finders[0].width * sqrt(2) * (MODULEOFFSET / 7));
    bounds[1] = findersVec[1] + (((findersVec[1] - findersVec[2]).Normalise() + 
                (findersVec[1] - findersVec[0]).Normalise())
                * finders[1].width) * (MODULEOFFSET / 7);
    bounds[2] = findersVec[2] + ((findersVec[2] - findersVec[0]).Normalise() * finders[2].width * sqrt(2) * (MODULEOFFSET / 7));

    BoundingBoxMissingCorner(finders, findersVec, bounds);

    return bounds;
}

std::array<Vec3, 4> BoundingBox(std::array<FinderCandidate, 3> finders, FinderCandidate finder4){
    const double MODULEOFFSET = 4;
    const double MODULEOFFSET4 = 5;

    std::array<Vec3, 4> findersVec;
    std::array<Vec3, 4> bounds;
    
    for (int i = 0; i < 3; i++){
        findersVec[i] = Vec3(finders[i].x, finders[i].y, 0);
    }
    findersVec[3] = Vec3(finder4.x, finder4.y, 0);

    bounds[0] = findersVec[0] + ((findersVec[0] - findersVec[2]).Normalise() * finders[0].width * sqrt(2) * (MODULEOFFSET / 7));
    bounds[1] = findersVec[1] + ((findersVec[1] - findersVec[3]).Normalise() * finders[1].width * sqrt(2) * (MODULEOFFSET / 7));
    bounds[2] = findersVec[2] + ((findersVec[2] - findersVec[0]).Normalise() * finders[2].width * sqrt(2) * (MODULEOFFSET / 7));
    bounds[3] = findersVec[3] + ((findersVec[3] - findersVec[1]).Normalise() * finders[3].width * sqrt(2) * (MODULEOFFSET / 7));

    return bounds;
}

Mat Project(Mat input, std::array<int, 8> inputCoords, Position size){
    Mat output;

    const int cornerIndices[4] = {1, 2, 0, 3};

    std::cout << "Start Coords" << std::endl;
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


    double sizeEstimateH = 8 * boundsDistanceH / (boundsDistanceH - centreDistanceH) + 1;
    double sizeEstimateV = 8 * boundsDistanceV / (boundsDistanceV - centreDistanceV) + 1;

    std::cout << "estimated width:  " << sizeEstimateH << std::endl;
    std::cout << "estimated height: " << sizeEstimateV << std::endl;
    return int((sizeEstimateH + sizeEstimateV) / 2);
}

std::vector<FinderCandidate> GetAlignmentCentres(int patternSize, std::vector<uint8_t> &threshold, std::vector<uint8_t> &data, cv::Mat inputImage, std::function<bool (std::vector<int>)> patternValid, bool firstWhite, FinderCandidate estimatedCentre, DebugImage debugImage) {
    std::vector<FinderCandidate> finder = FinderPatterns::FinderPatterns(patternSize, threshold, inputImage.rows, inputImage.cols, patternValid, firstWhite);

    if (patternSize == 3) {
        for (int i = 0; i < finder.size(); i++) {
            debugImage.DebugCentre(finder[i]);
        }
    }

    std::vector<FinderGroup> finderGroups = GroupFinders(finder);
    std::vector<int> finderGroupsValidIndex;

    for (int i = 0; i < finderGroups.size(); i++){
        bool valid = finderGroups[i].isValid(threshold, inputImage.cols, inputImage.rows, patternSize, patternValid, firstWhite);
        if (valid){
            finderGroupsValidIndex.push_back(i);
        }
    }

    int quality[finderGroups.size()];

    for (int i = 0; i < finderGroups.size(); i++){
        int qualNX = finderGroups[i].size();
        int qualWidth = finderGroups[i].Centre().width;
        double distance = 
                pow((finderGroups[i].Centre().x / inputImage.cols - estimatedCentre.x), 2)
            +   pow((finderGroups[i].Centre().y / inputImage.rows - estimatedCentre.y), 2);
        quality[i] = qualNX * qualWidth * (1 - distance);
    }

    std::sort(finderGroupsValidIndex.begin(), finderGroupsValidIndex.end(), [&quality](int a, int b){
        return quality[a] > quality[b];
    });
    std::vector<FinderCandidate> centres = GetCentres(finderGroups, finderGroupsValidIndex);

    return centres;
}

Mat AlignImage(Mat inputImage, int projectionSize, int pixelOffsetExpand, std::string debugPath = ""){
    int nPixels = inputImage.cols * inputImage.rows;
    DebugImage debug(inputImage.cols, inputImage.rows);

    std::vector<uint8_t> data = ImageAux::MatToBytes(inputImage);

    std::vector<double> grey = ImageProcessing::Greyscale(data, nPixels);
    std::vector<uint8_t> threshold = ImageProcessing::Threshold(grey, inputImage.rows, inputImage.cols);


    Mat thresholdImage(inputImage.rows, inputImage.cols, CV_8U, threshold.data());
    
    if (debugPath != "") {
        imwrite("output/img/output_threshold.png", thresholdImage);
    }

    std::cout << "Threshold" << std::endl;

    std::vector<FinderCandidate> centres = GetAlignmentCentres(5, threshold, data, inputImage, PatternValid::PatternStandard, false, FinderCandidate(.5, .5, 0), debug);
    std::vector<FinderCandidate> centres4 = GetAlignmentCentres(5, threshold, data, inputImage, PatternValid::PatternStandard, true, FinderCandidate(1., 1., 0.), debug);

    std::array<FinderCandidate, 3> centresOrdered = OrderCentres(centres);

    for (int i = 0; i < centresOrdered.size(); i++) {
        debug.DebugCross(int(centresOrdered[i].x), int(centresOrdered[i].y), inputImage.cols, 10);
    }
    debug.DebugCross(int(centres4[0].x), int(centres4[0].y), inputImage.cols, 10);
    
    std::cout << "centres" << std::endl;

    std::array<Vec3, 4> bounds = BoundingBox(centresOrdered, centres4[0]);

    for (int i = 0; i < bounds.size(); i++) {
        debug.DebugCross(int(bounds[i].x), int(bounds[i].y), inputImage.cols, 10, {0, 0, 255});
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
        centresVec[i] = Vec3(centresOrdered[i].x, centresOrdered[i].y, 0);
    }

    const int adjustmentDirection[8] = {-1, 1, -1, -1, 1, -1, 1, 1};
    std::array<int, 8> projectCoordsAdjusted;
    for (int i = 0; i < 8; i++){
        projectCoordsAdjusted[i] = projectCoords[i] + pixelOffsetExpand * adjustmentDirection[i];
    }

    EstimateBarcodeSize(bounds, centresVec);

    std::cout << "Estimated size" << std::endl;

    outputImage = Project(inputImage, projectCoordsAdjusted, Position(projectionSize, projectionSize)); 

    std::cout << "Projected image" << std::endl;

    Mat debugImage(inputImage.rows, inputImage.cols, CV_8UC3, pixels.data());
    
    if (debugPath != "") {
        imwrite(debugPath, debugImage);
    }

    return outputImage;
}

int main(){
    std::string filePathDebug = "output/img/output_debug_";
    std::string filePathOut = "output/img/output_align_";
    std::string filePath = "output/img/output_distorted.png";
    std::string filePathOutFinal = "output/img/output_align.png";
    const int ITERATIONS = 5;

    Mat image = imread(filePath);
    Mat nextImage;

    for (int i = 0; i < ITERATIONS; i++){
        nextImage = AlignImage(image.clone(), image.rows, 100, filePathDebug + std::to_string(i) +  ".png");
        imwrite(filePathOut + std::to_string(i) + ".png", nextImage);
        image = nextImage;
    }

    
    Mat outputImage = AlignImage(nextImage, BARCODE_SIZE, 0, filePathDebug + "final.png");
    std::cout << "writing the final image" << std::endl;
    imwrite(filePathOutFinal, outputImage);
    std::cout << "Finished program" << std::endl;
}
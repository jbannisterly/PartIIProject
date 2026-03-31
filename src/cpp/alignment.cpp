#include "alignment.hpp"
#include <optional>
#include <iostream>
#include <cstdlib>

using namespace cv;

namespace Alignment {

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
                FinderCandidate centre = finderGroups[finderGroupsValidIndex[i]].Centre();
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

    cv::Size2i EstimateBarcodeSize(std::array<Vec3, 4> bounds, std::array<Vec3, 4> centres){
        double centreDistanceH = (centres[1] - centres[2]).Magnitude();
        double boundsDistanceH = (bounds[1] - bounds[2]).Magnitude();

        double centreDistanceV = (centres[0] - centres[1]).Magnitude();
        double boundsDistanceV = (bounds[0] - bounds[1]).Magnitude();


        double sizeEstimateH = 8 * boundsDistanceH / (boundsDistanceH - centreDistanceH) + 1;
        double sizeEstimateV = 8 * boundsDistanceV / (boundsDistanceV - centreDistanceV) + 1;

        std::cout << "estimated width:  " << sizeEstimateH << std::endl;
        std::cout << "estimated height: " << sizeEstimateV << std::endl;
        return Size2i(round(sizeEstimateH), round(sizeEstimateV));
    }

    int GLOBAL_DEBUG_COUNTER = 0;

    std::vector<FinderCandidate> GetAlignmentCentres(int patternSize, std::vector<uint8_t> &threshold, std::vector<uint8_t> &data, cv::Mat inputImage, std::function<bool (std::vector<int>)> patternValid, bool firstWhite, std::vector<FinderCandidate> estimatedCentre, std::optional<DebugImage*> debug={}) {
    
        std::vector<FinderCandidate> finder = FinderPatterns::FinderPatterns(patternSize, threshold, inputImage.rows, inputImage.cols, patternValid, firstWhite);

        if (firstWhite) {
            for (int i = 0; i < finder.size(); i++) {
                // debug.value()->DebugCentre(finder[i], {0, 0, 255});
            }
        }

        std::vector<FinderGroup> finderGroups = GroupFinders(finder);

        if (firstWhite) {
            for (int i = 0; i < finderGroups.size(); i++) {
                // debug.value()->DebugCross(finderGroups[i].CentreRefined().x, finderGroups[i].CentreRefined().y, inputImage.cols , finderGroups[i].size(), {0, 255, 255});
            }

            int max = 0;
        int maxind = 0;
        for (int i = 0; i < finderGroups.size(); i++) {
            if (finderGroups[i].size() > max) {
                max = finderGroups[i].size();
                maxind = i;
            }
        }
        finderGroups[maxind].isValid(threshold, inputImage.cols, inputImage.rows, patternSize, patternValid, firstWhite);

        }


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
            double bestDistance = 100;
            for (int j = 0; j < estimatedCentre.size(); j++) {
                double distance = 
                    pow((finderGroups[i].Centre().x / inputImage.cols - estimatedCentre[j].x), 2)
                +   pow((finderGroups[i].Centre().y / inputImage.rows - estimatedCentre[j].y), 2);
                if (distance < bestDistance) {
                    bestDistance = distance;
                }
            }
            quality[i] = qualNX * qualWidth * pow((1 - bestDistance), 3);
        }

        std::sort(finderGroupsValidIndex.begin(), finderGroupsValidIndex.end(), [&quality](int a, int b){
            return quality[a] > quality[b];
        });

        std::vector<FinderCandidate> centres = GetCentres(finderGroups, finderGroupsValidIndex);

        return centres;
    }


    AlignmentData GetBounds(Mat inputImage, std::string debugPath) {
        AlignmentData alignmentData;
    
        int nPixels = inputImage.cols * inputImage.rows;
        DebugImage debug(inputImage.cols, inputImage.rows);

        std::vector<uint8_t> data = ImageAux::MatToBytes(inputImage);

        std::vector<double> grey = ImageProcessing::Greyscale(data, nPixels);
        std::vector<uint8_t> threshold = ImageProcessing::Threshold(grey, inputImage.rows, inputImage.cols);

        std::vector<uint8_t> debugBackground(data);
        Mat thresholdImage(inputImage.rows, inputImage.cols, CV_8U, threshold.data());
    
        if (debugPath != "" || true) {
            imwrite("output/img/output_threshold.png", thresholdImage);
        }

        std::vector<FinderCandidate> centres0 = GetAlignmentCentres(5, threshold, data, inputImage, PatternValid::PatternStandard, false, {FinderCandidate(1, 0, 0)});
        std::vector<FinderCandidate> centres1 = GetAlignmentCentres(5, threshold, data, inputImage, PatternValid::PatternStandard, false, {FinderCandidate(0, 0, 0)});
        std::vector<FinderCandidate> centres2 = GetAlignmentCentres(5, threshold, data, inputImage, PatternValid::PatternStandard, false, {FinderCandidate(0, 1, 0)});
        std::vector<FinderCandidate> centres3 = GetAlignmentCentres(5, threshold, data, inputImage, PatternValid::PatternStandard, true, {FinderCandidate(1., 1., 0.)}, &debug);

        std::cout << centres0.size() << " " << centres1.size() << " " << centres2.size() << " " << centres3.size();

        std::array<FinderCandidate, 3> centresOrdered = {centres0[0], centres1[0], centres2[0]};

        for (int i = 0; i < centresOrdered.size(); i++) {
            debug.DebugCross(int(centresOrdered[i].x), int(centresOrdered[i].y), inputImage.cols, centresOrdered[i].width, {0, 255, 0});
        }
        debug.DebugCross(int(centres3[0].x), int(centres3[0].y), inputImage.cols, centres3[0].width, {0, 255, 0});

        std::array<Vec3, 4> bounds = BoundingBox::BoundingBoxRectangle(centresOrdered, centres3[0]);

        for (int i = 0; i < bounds.size(); i++) {
            debug.DebugCross(int(bounds[i].x), int(bounds[i].y), inputImage.cols, 10, {0, 0, 255});
        }

        std::cout << "POINT A" << std::endl;

        debug.WriteImage(debugPath, debugBackground);

        std::cout << "POINT B - writted debug" << std::endl;

        alignmentData.bounds = bounds;

        std::array<Vec3, 4> centresVec;
        for (int i = 0; i < 3; i++){
            centresVec[i] = Vec3(centresOrdered[i].x, centresOrdered[i].y, 0);
        }
        centresVec[3] = Vec3(centres3[0].x, centres3[0].y, 0);

        cv::Size2i estimatedSize = EstimateBarcodeSize(bounds, centresVec);
        alignmentData.estimatedHeight = estimatedSize.height;
        alignmentData.estimatedWidth = estimatedSize.width;

        return alignmentData;
    }

    AlignmentData GetBoundsBorderMethod(Mat inputImage, std::string debugPath) {
        GLOBAL_DEBUG_COUNTER++;

        AlignmentData alignmentData;
    
        int nPixels = inputImage.cols * inputImage.rows;
        DebugImage debug(inputImage.cols, inputImage.rows);

        std::vector<uint8_t> data = ImageAux::MatToBytes(inputImage);

       std::vector<double> grey = ImageProcessing::Greyscale(data, nPixels);
        std::vector<uint8_t> threshold = ImageProcessing::Threshold(grey, inputImage.rows, inputImage.cols);

        threshold = ImageAux::Dilate(threshold, inputImage.cols, inputImage.rows);
        threshold = ImageAux::Erode(threshold, inputImage.cols, inputImage.rows);
        threshold = ImageAux::Erode(threshold, inputImage.cols, inputImage.rows);
        threshold = ImageAux::Dilate(threshold, inputImage.cols, inputImage.rows);

        std::vector<uint8_t> debugBackground(data);
        Mat thresholdImage(inputImage.rows, inputImage.cols, CV_8U, threshold.data());
    
        if (debugPath != "" || true) {
            imwrite("output/img/output_threshold.png", thresholdImage);
        }

        std::vector<Border::Border> borders = Border::GetBorders(threshold, inputImage.cols);
        std::vector<int> deepBorderIndex = Border::GetInside(borders, inputImage.cols);

        std::vector<uint8_t> debugBorders;
        debugBorders.resize(threshold.size() * 3, 0);
        for (int i = 0; i < deepBorderIndex.size(); i++) {
            uint8_t r = rand() % 128 + 100;
            uint8_t g = rand() % 128 + 100;
            uint8_t b = rand() % 128 + 100;

            for (int j = 0; j < borders[deepBorderIndex[i]].borderMembers.size(); j++) {
                debugBorders[borders[deepBorderIndex[i]].borderMembers[j].Index() * 3] = r;
                debugBorders[borders[deepBorderIndex[i]].borderMembers[j].Index() * 3 + 1] = g;
                debugBorders[borders[deepBorderIndex[i]].borderMembers[j].Index() * 3 + 2] = b;
            }
        }

        Mat borderImage(inputImage.rows, inputImage.cols, CV_8UC3, debugBorders.data());
    
        if (debugPath != "") {
            imwrite("output/img/output_border_debug.png", borderImage);
        }

        std::vector<uint8_t> debugBorders_all;
        debugBorders_all.resize(threshold.size() * 3, 0);
        for (int i = 0; i < borders.size(); i++) {
            uint8_t r = rand() % 128 + 100;
            uint8_t g = rand() % 128 + 100;
            uint8_t b = rand() % 128 + 100;

            for (int j = 0; j < borders[i].borderMembers.size(); j++) {
                debugBorders_all[borders[i].borderMembers[j].Index() * 3] = r;
                debugBorders_all[borders[i].borderMembers[j].Index() * 3 + 1] = g;
                debugBorders_all[borders[i].borderMembers[j].Index() * 3 + 2] = b;
            }
        }

        Mat borderImage_all(inputImage.rows, inputImage.cols, CV_8UC3, debugBorders_all.data());
    
        if (debugPath != "") {
            imwrite("output/img/output_border_debug_all_fills.png", borderImage_all);
        }

        std::vector<FinderCandidate> centres = Border::Finders(borders, deepBorderIndex, inputImage.cols);
        std::array<FinderCandidate, 3> centresOrdered = {centres[0], centres[1], centres[2]};
        std::array<Vec3, 4> bounds = BoundingBox::BoundingBoxRectangle(centresOrdered, centres[3]);

        for (int i = 0; i < bounds.size(); i++) {
            debug.DebugCross(int(bounds[i].x), int(bounds[i].y), inputImage.cols, 10, {0, 0, 255});
        }

        debug.WriteImage(debugPath, debugBackground);
        std::cout << "debuggeed to " << debugPath << std::endl;

        alignmentData.bounds = bounds;

        std::array<Vec3, 4> centresVec;
        for (int i = 0; i < 3; i++){
            centresVec[i] = Vec3(centresOrdered[i].x, centresOrdered[i].y, 0);
        }
        centresVec[3] = Vec3(centres[3].x, centres[3].y, 0);

        cv::Size2i estimatedSize = EstimateBarcodeSize(bounds, centresVec);
        alignmentData.estimatedHeight = estimatedSize.height;
        alignmentData.estimatedWidth = estimatedSize.width;

        return alignmentData;
    }

    Mat AlignImage(Mat inputImage, int projectionHeight, int projectionWidth, double fractionExpand, std::string debugPath){
        Mat outputImage;

        AlignmentData alignment = GetBounds(inputImage, debugPath);
        std::array<Vec3, 4> bounds = alignment.bounds;

        std::array<Vec3, 4> expandedCoords = BoundingBox::ExpansionBox(bounds, fractionExpand);
        std::array<int, 8> projectCoordsAdjusted;
        for (int i = 0; i < 4; i++) {
            projectCoordsAdjusted[i * 2 + 0] = expandedCoords[i].x;
            projectCoordsAdjusted[i * 2 + 1] = expandedCoords[i].y;
        }

        outputImage = ImageAux::Project(inputImage, projectCoordsAdjusted, Size(projectionWidth, projectionHeight)); 

        return outputImage.clone();
    }

}
#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "_config.hpp"
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
#include "bounding_box.hpp"
#include "border.hpp"

struct AlignmentData {
    std::array<Vec3, 4> bounds;
    int estimatedWidth;
    int estimatedHeight;
};

namespace Alignment {
    cv::Mat AlignImage(cv::Mat inputImage, std::vector<int> &projectionHeight, std::vector<int> &projectionWidth, double fractionExpand, std::string debugPath = "", int projectionScale = 1, PatternType pattern=PatternType::CIRCLE);
    AlignmentData GetBounds(cv::Mat inputImage, std::string debugPath = "");
    AlignmentData GetBoundsBorderMethod(cv::Mat inputImage, std::string debugPath = "");
}


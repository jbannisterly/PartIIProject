#include "image_processing.hpp"
#include <iostream>
#include "_config.hpp"

using namespace cv;

namespace ImageProcessing {

    int GetIndexPadding(int nY, int nX, int y, int x){
        int yy = y > 0 ? (y < nY ? y : nY - 1) : 0;
        int xx = x > 0 ? (x < nX ? x : nX - 1) : 0;
        return yy * nX + xx;
    }


    std::vector<double> Greyscale(std::vector<uint8_t> &image, int pixels){
        std::vector<double> grey;
        std::cout << "pixels is " << pixels << std::endl;
        std::cout << "image size is " << image.size() << std::endl;
        grey.reserve(pixels);;

        for (int i = 0; i < pixels; i++){
            grey.push_back((77 * image[i * 3] + 151 * image[i * 3 + 1] + 28 * image[i * 3 + 2]) / 256);
        }

        return grey;
    }

    std::vector<double> GetBlurred(std::vector<double> &imageData, int nY, int nX){
        Mat data = Mat(nY, nX, CV_64F, imageData.data());
        Mat blurred = Mat(nY, nX, CV_64F);

        std::cout << "pre box filter" << std::endl;

        boxFilter(data, blurred, -1, Size(WINDOW_SIZE * 2 + 1, WINDOW_SIZE * 2 + 1), Point(-1, -1), true, CV_HAL_BORDER_REPLICATE);

        std::cout << "post box filter" << std::endl;

        std::vector<double> blurredVec((double*)blurred.ptr(), (double*)blurred.ptr() + imageData.size());

        return blurredVec;
    }

    std::vector<uint8_t> Threshold(std::vector<double> &image, int nY, int nX){
        std::vector<uint8_t> threshold;
        threshold.reserve(nY * nX);
        const double WINDOW_SCALE = (2 * WINDOW_SIZE + 1) * (2 * WINDOW_SIZE + 1);
    
#if BLUR_FORMULA == 1
        std::vector<double> blurred = GetBlurred(image, nY, nX);
#endif

        std::cout << "did blur" << std::endl;

        for (int yy = 0; yy < nY; yy++){
#if BLUR_FORMULA == 0
            double m = 0;
            for (int xi = -WINDOW_SIZE; xi <= WINDOW_SIZE; xi++){
                for (int yi = -WINDOW_SIZE; yi <= WINDOW_SIZE; yi++){
                    m += image[GetIndexPadding(nY, nX, yy + yi, xi - 1)] / WINDOW_SCALE;
                }
            }}

#endif
            for (int xx = 0; xx < nX; xx++){
#if BLUR_FORMULA == 0
                for (int yi = -WINDOW_SIZE; yi <= WINDOW_SIZE; yi++){
                    m += image[GetIndexPadding(nY, nX, yy + yi, xx + WINDOW_SIZE)] / WINDOW_SCALE;
                    m -= image[GetIndexPadding(nY, nX, yy + yi, xx - WINDOW_SIZE)] / WINDOW_SCALE;
                }
#endif
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

        return threshold;
    }
}

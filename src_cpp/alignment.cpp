#include "alignment.hpp"
#include <opencv2/opencv.hpp>
#include "image_aux.hpp"

#include <stdio.h>

using namespace cv;

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
    const float WINDOW_SIZE = 200;
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
                threshold[yy * nX + xx] = 0;
            }else{
                if(image[yy * nX + xx] * 1.1 >= m - 10){
                    threshold[yy * nX + xx] = 0;
                }else{
                    threshold[yy * nX + xx] = 255;
                }
            }
        }
    }

    return threshold;
}



uint8_t* Uint8ToPixels(uint8_t* data, int nPixels){
    uint8_t* pixels = (uint8_t*)malloc(sizeof(uint8_t) * nPixels * 3);

    for (int i = 0; i < nPixels; i++){
        pixels[i * 3] = data[i];
        pixels[i * 3 + 1] = data[i];
        pixels[i * 3 + 2] = data[i];
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

int main(){
    const char* filePath = "output/output_distorted.png";
    const char* filePathOut = "output/output_align.png";

    Mat image = imread(filePath);
    int nPixels = image.cols * image.rows; 
    uint8_t* data = MatToBytes(image);

    float* grey = Greyscale(data, nPixels);
    uint8_t* threshold = Threshold(grey, image.rows, image.cols);
    uint8_t* pixels = Uint8ToPixels(threshold, nPixels);

    Mat outputImage(image.rows, image.cols, CV_8UC3);
    outputImage.data = pixels;

    imwrite(filePathOut, outputImage);
}
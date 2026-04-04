#include "alignment.hpp"

using namespace cv;

int main(){
    BarcodeConfig config(100, 150);
    std::string filePathDebug = "output/img/output_debug_";
    std::string filePathOut = "output/img/output_align_";
    std::string filePath = "output/img/output_distorted.png";
    std::string filePathOutFinal = "output/img/output_align.png";

    Mat image = imread(filePath);
    Mat nextImage;

    for (int i = 0; i < ALIGNMENT_ITERATIONS; i++){
        nextImage = Alignment::AlignImage(image.clone(), config.barcodeHeight * 16, config.barcodeWidth * 16, 0.1, filePathDebug + std::to_string(i));
        imwrite(filePathOut + std::to_string(i) + ".png", nextImage);
        image = nextImage;
    }

    
    Mat outputImage = Alignment::AlignImage(nextImage, config.barcodeHeight, config.barcodeWidth, 0, filePathDebug + "final.png");
    std::cout << "writing the final image" << std::endl;
    imwrite(filePathOutFinal, outputImage);
    std::cout << "Finished program" << std::endl;
}
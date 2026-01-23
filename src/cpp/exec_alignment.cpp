#include "alignment.hpp"

using namespace cv;

int main(){
    std::string filePathDebug = "output/img/output_debug_";
    std::string filePathOut = "output/img/output_align_";
    std::string filePath = "output/img/output_distorted.png";
    std::string filePathOutFinal = "output/img/output_align.png";

    Mat image = imread(filePath);
    Mat nextImage;

    for (int i = 0; i < ALIGNMENT_ITERATIONS; i++){
        nextImage = Alignment::AlignImage(image.clone(), BARCODE_HEIGHT * 16, BARCODE_WIDTH * 16, 0.1, filePathDebug + std::to_string(i) +  ".png");
        imwrite(filePathOut + std::to_string(i) + ".png", nextImage);
        image = nextImage;
    }

    
    Mat outputImage = Alignment::AlignImage(nextImage, BARCODE_HEIGHT, BARCODE_WIDTH, 0, filePathDebug + "final.png");
    std::cout << "writing the final image" << std::endl;
    imwrite(filePathOutFinal, outputImage);
    std::cout << "Finished program" << std::endl;
}
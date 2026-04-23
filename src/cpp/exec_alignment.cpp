#include "alignment.hpp"

using namespace cv;

int main(int argc, char* argv[]){
    BarcodeConfig config(100, 150);

    std::vector<int> sizes;
    sizes.push_back(50);
    sizes.push_back(100);
    sizes.push_back(150);
    sizes.push_back(200);

    std::string filePathDebug = "output/img/output_debug_";
    std::string filePathOut = "output/img/output_align_";
    std::string filePath = "output/img/output_distorted.png";
    std::string filePathOutFinal = "output/img/output_align.png";

    std::vector<std::string> filePathsOut;
    std::vector<std::string> filePaths;

    for (int i = 1; i < argc; i++) {
        filePaths.push_back(argv[i] + std::string(".png"));
        filePathsOut.push_back(argv[i] + std::string("_align.png"));
    }
    if (argc == 1) {
        filePaths.push_back(filePath);
        filePathsOut.push_back(filePathOutFinal);
    }

    for (int i = 0; i < filePaths.size(); i++) {
        filePath = filePaths[i];
        filePathOutFinal = filePathsOut[i];

        Mat image = imread(filePath);
        Mat nextImage;

        for (int i = 0; i < ALIGNMENT_ITERATIONS; i++){
            try {
                nextImage = Alignment::AlignImage(image.clone(), sizes, sizes, 0.1, filePathDebug + std::to_string(i), 16);
                imwrite(filePathOut + std::to_string(i) + ".png", nextImage);
                image = nextImage;
            } catch(Exception e) {
                std::cout << "error, skipping iterations at " << i << std::endl;
                i = ALIGNMENT_ITERATIONS;
            }
        }

    
        Mat outputImage = Alignment::AlignImage(nextImage, sizes, sizes, 0, filePathDebug + "final");
        std::cout << "writing the final image" << std::endl;
        imwrite(filePathOutFinal, outputImage);
        std::cout << filePathOutFinal << std::endl;
        std::cout << "Finished program" << std::endl;
    }

}
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstring>
#include "barcode_layout.hpp"
#include "compressor.hpp"
// #include "error_correction_schifra.hpp"
#include <vector>
#include "colours.hpp"
#include "_config.hpp"
#include "fstream"
#include "splitError.hpp"
#include "splitBytes.hpp"

using namespace cv;


std::vector<std::vector<uint8_t>> EncodeMessage(std::vector<uint8_t> rawData, std::vector<ErrorCorrectionVirtual*> &errorCorrectors){
    // uint16_t length = strlen(message);

    // uint8_t* rawData = new uint8_t[length];

    // memcpy(rawData, message, length);

    // std::vector<uint8_t> compressedData = Compression::compress(rawData);
    std::vector<uint8_t> compressedData;
    compressedData.reserve(2000);
    for (int i = 0; i < 2000; i++) {
        compressedData.push_back(i % 256);
    }

    std::cout << "Compressed length " << compressedData.size() << std::endl; 
    
    compressedData.insert(compressedData.begin(), {
        (compressedData.size() >> 0) & 255,
        (compressedData.size() >> 8) & 255
    });

    SplitBytes splitter;
    std::vector<std::vector<uint8_t>> splitData = splitter.Encode(compressedData, errorCorrectors);


    for (int i = 0; i < splitData.size(); i++) {
        std::cout << "csd " << i << " " << splitData[i].size() << std::endl;
    }

    SplitError split(errorCorrectors);
    std::vector<std::vector<uint8_t>> errorSplitData = split.Encode(splitData);

    std::ofstream outFile("output/debug/split_encode");
        for (int i = 0; i < errorSplitData.size(); i++) {
        for (int j = 0; j < errorSplitData[i].size(); j++) {
            outFile << int(errorSplitData[i][j]) << "\n";
        }
        outFile << "---\n"; 
        }
    outFile.close();

    for (int i = 0; i < errorSplitData.size(); i++) {
        std::cout << "sd " << i << " " << errorSplitData[i].size() << std::endl;
    }

    return errorSplitData;
}

std::vector<uint8_t> MessageToPixels(std::vector<uint8_t> byteData) {
    std::vector<uint8_t> pixelData;

    pixelData.reserve(byteData.size() * 24);

    for (int i = 0; i < byteData.size(); i++){
        uint8_t currentByte = byteData[i];
        for (int j = 0; j < 8; j++){
            uint8_t data = (currentByte & 128) ? 255 : 0;
            currentByte <<= 1;
            for (int k = 0; k < 3; k++){
                pixelData.push_back(data);
            }
        }
    }

    return pixelData;
}

std::vector<uint8_t> PixelsToBarcode(std::vector<uint8_t> pixels){
    int size = BARCODE_SIZE * BARCODE_SIZE * 3;
    BarcodeLayout barcode = GetBarcode();

    int sourceCounter = 0;
    int targetCounter = 0;

    while(sourceCounter < pixels.size()){
        if (barcode.mask[targetCounter] > 0){
            for (int i = 0; i < 3; i++) {
                barcode.data[targetCounter * 3 + i] = pixels[sourceCounter];
                sourceCounter++;
            }
        }
        targetCounter++;
    }

    return barcode.data;
}

int main(){
    std::string message = "A barcode or bar code is a method of representing data in a visual, machine-readable symbolic form. Initially, barcodes represented data by varying the widths, spacings and sizes of parallel lines. These barcodes, commonly referred to as linear or one-dimensional (1D), can be scanned by optical scanners known as barcode readers.\nLater, two-dimensional (2D) variants were developed, using rectangles, dots, hexagons and other patterns, called matrix codes or 2D barcodes, although they do not use bars in the traditional sense. Both can be read using purpose-built 2D optical scanners, which exist in a few different forms. Matrix codes can also be read by a digital camera connected to a microcomputer running software that takes a photographic image of the barcode and analyzes the image to deconstruct and decode the code. A mobile device with a built-in camera, such as a smartphone, can function as this type of barcode reader using specialized application software and is suitable for both 1D and 2D codes.\nBarcoded rolling stock in the UK, 1962\nThe barcode was invented by Norman Joseph Woodland and Bernard Silver and patented in the US in 1952.[1] The invention was based on Morse code[2] that was extended to thin and thick bars. However, it took over twenty years before this invention became commercially successful. UK magazine Modern Railways December 1962 pages 387–389 record how British Railways had already perfected a barcode-reading system capable of correctly reading rolling stock travelling at 100 mph (160 km/h) with no mistakes. An early use of one type of barcode in an industrial context was sponsored by the Association of American Railroads in the late 1960s. Developed by General Telephone and Electronics (GTE) and called KarTrak ACI (Automatic Car Identification), this scheme involved placing colored stripes in various combinations on steel plates which were affixed to the sides of railroad rolling stock. Two plates were used per car, one on each side, with the arrangement of the colored stripes encoding information such as ownership, type of equipment, and identification number.[3] The plates were read by a trackside scanner located, for instance, at the entrance to a classification yard, while the car was moving past.[4] The project was abandoned after about ten years because the system proved unreliable after long-term use.[3]\nBarcodes became commercially successful after they were adopted to automate supermarket checkout systems, a task for which they have become almost universal. The Uniform Grocery Product Code Council had chosen, in 1973, the barcode design developed by George Laurer. Laurer's barcode, with vertical bars, printed better than the circular barcode developed by Woodland and Silver.[5] Their use has spread to many other tasks that are generically referred to as automatic identification and data capture (AIDC). The first successful system using barcodes was in the UK supermarket group Sainsbury's in 1972 using shelf-mounted barcodes which were developed by Plessey.[6][7] In June 1974, Marsh supermarket in Troy, Ohio used a scanner made by Photographic Sciences Corporation to scan the Universal Product Code (UPC) barcode on a pack of Wrigley's chewing gum.[8][5] QR codes, a specific type of 2D barcode, rose in popularity in the second decade of the 2000s due to the growth in smartphone ownership.[9]\nOther systems have made inroads in the AIDC market, but the simplicity, universality and low cost of barcodes has limited the role of these other systems, particularly before technologies such as radio-frequency identification (RFID) became available after 2023. ";

    std::vector<ErrorCorrectionVirtual*> errorCorrectors;
    errorCorrectors.push_back(new ErrorCorrection<255, 32>());
    errorCorrectors.push_back(new ErrorCorrection<255, 32>());
    errorCorrectors.push_back(new ErrorCorrection<255, 32>());

    std::vector<uint8_t> byteData;
    byteData.reserve(message.size());
    for (int i = 0; i < message.size(); i++) {
        byteData.push_back(message[i]);
    }

    std::cout << "Initial length " << message.size() << std::endl;

    std::vector<std::vector<uint8_t>> encodedData = EncodeMessage(byteData, errorCorrectors);
    
    std::vector<Colour> colours = {
        Colour(0, 0, 0),
        Colour(255, 255, 255),
        Colour(255, 0, 0),
        Colour(255, 255, 0),
        Colour(0, 255, 0),
        Colour(255, 0, 255),
        Colour(0, 0, 255),
        Colour(0, 255, 255),
    };
    ColourScheme colourScheme(colours);
    ColourPixels colourPix(colourScheme);

    std::vector<uint8_t> pixelData = colourPix.DataToPixels(encodedData);

    std::cout << "n pixels " << pixelData.size() / 3 / 8 << std::endl;

    std::vector<uint8_t> imageData = PixelsToBarcode(pixelData);

    Mat image(BARCODE_SIZE, BARCODE_SIZE, CV_8UC3);
    image.data = imageData.data();

    imwrite("output/img/output.png", image);
}
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstring>
#include "barcode_layout.hpp"
#include "compressor.hpp"
#include <vector>
#include "colours.hpp"
#include "_config.hpp"
#include "fstream"
#include "split_error.hpp"
#include "split_bytes.hpp"
#include "barcode_writer.hpp"
#include "colour_palletes.hpp"
#include "error_layout.hpp"
#include "header_data.hpp"

using namespace cv;

int main(){
    std::string message = "A barcode or bar code is a method of representing data in a visual, machine-readable symbolic form. Initially, barcodes represented data by varying the widths, spacings and sizes of parallel lines. These barcodes, commonly referred to as linear or one-dimensional (1D), can be scanned by optical scanners known as barcode readers.\nLater, two-dimensional (2D) variants were developed, using rectangles, dots, hexagons and other patterns, called matrix codes or 2D barcodes, although they do not use bars in the traditional sense. Both can be read using purpose-built 2D optical scanners, which exist in a few different forms. Matrix codes can also be read by a digital camera connected to a microcomputer running software that takes a photographic image of the barcode and analyzes the image to deconstruct and decode the code. A mobile device with a built-in camera, such as a smartphone, can function as this type of barcode reader using specialized application software and is suitable for both 1D and 2D codes.\nBarcoded rolling stock in the UK, 1962\nThe barcode was invented by Norman Joseph Woodland and Bernard Silver and patented in the US in 1952.[1] The invention was based on Morse code[2] that was extended to thin and thick bars. However, it took over twenty years before this invention became commercially successful. UK magazine Modern Railways December 1962 pages 387–389 record how British Railways had already perfected a barcode-reading system capable of correctly reading rolling stock travelling at 100 mph (160 km/h) with no mistakes. An early use of one type of barcode in an industrial context was sponsored by the Association of American Railroads in the late 1960s. Developed by General Telephone and Electronics (GTE) and called KarTrak ACI (Automatic Car Identification), this scheme involved placing colored stripes in various combinations on steel plates which were affixed to the sides of railroad rolling stock. Two plates were used per car, one on each side, with the arrangement of the colored stripes encoding information such as ownership, type of equipment, and identification number.[3] The plates were read by a trackside scanner located, for instance, at the entrance to a classification yard, while the car was moving past.[4] The project was abandoned after about ten years because the system proved unreliable after long-term use.[3]\nBarcodes became commercially successful after they were adopted to automate supermarket checkout systems, a task for which they have become almost universal. The Uniform Grocery Product Code Council had chosen, in 1973, the barcode design developed by George Laurer. Laurer's barcode, with vertical bars, printed better than the circular barcode developed by Woodland and Silver.[5] Their use has spread to many other tasks that are generically referred to as automatic identification and data capture (AIDC). The first successful system using barcodes was in the UK supermarket group Sainsbury's in 1972 using shelf-mounted barcodes which were developed by Plessey.[6][7] In June 1974, Marsh supermarket in Troy, Ohio used a scanner made by Photographic Sciences Corporation to scan the Universal Product Code (UPC) barcode on a pack of Wrigley's chewing gum.[8][5] QR codes, a specific type of 2D barcode, rose in popularity in the second decade of the 2000s due to the growth in smartphone ownership.[9]\nOther systems have made inroads in the AIDC market, but the simplicity, universality and low cost of barcodes has limited the role of these other systems, particularly before technologies such as radio-frequency identification (RFID) became available after 2023. ";

    std::vector<uint8_t> byteData;
    byteData.reserve(message.size());
    for (int i = 0; i < message.size(); i++) {
        byteData.push_back(message[i]);
    }

    std::vector<ErrorCorrectionVirtual*> errorCorrectors = ErrorLayout::Bit_3();
    ColourPixels colourPix = ColourPalletes::Bit_3();

    std::vector<uint8_t> compressedData = Compression::compress(byteData);
    HeaderData::PrependLength(compressedData);

    std::vector<std::vector<uint8_t>> splitData = SplitBytes::Encode(compressedData, errorCorrectors);

    SplitError split(errorCorrectors);
    std::vector<std::vector<uint8_t>> errorSplitData = split.Encode(splitData);

    std::vector<uint8_t> pixelData = colourPix.DataToPixels(errorSplitData);

    BarcodeWriter writer(GetBarcode4Detailed());
    std::vector<uint8_t> imageData = writer.PixelsToBarcode(pixelData);

    Mat image(BARCODE_SIZE, BARCODE_SIZE, CV_8UC3);
    image.data = imageData.data();

    imwrite("output/img/output.png", image);
}
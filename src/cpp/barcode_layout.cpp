#include "barcode_layout.hpp"

const int FINDER_SIZE = 1;

void DrawSquare(BarcodeLayout &barcode, uint8_t colour, int positionX, int positionY, int barcodeWidth, int squareWidth, int squareHeight, int mask=0){
    for (int i = 0; i < squareHeight; i++){
        int offset = ((i + positionY) * barcodeWidth + positionX);

        for (int j = 0; j < squareWidth; j++) {
            for (int k = 0; k < 3; k++) {
                barcode.data[offset * 3 + j * 3 + k] = colour;
            }
            barcode.mask[offset + j] = mask;
        }
    }
}

BarcodeLayout GetBarcodeBase(BarcodeConfig config) {
    BarcodeLayout barcode = BarcodeLayout();
    int size = config.barcodeHeight * config.barcodeWidth;

    barcode.data.resize(size * 3, 0);
    barcode.mask.resize(size, 0);

    DrawSquare(barcode, 255, 0, 0, config.barcodeWidth, config.barcodeWidth, config.barcodeHeight);
    DrawSquare(barcode, 0, 1, 1, config.barcodeWidth, config.barcodeWidth - 2, config.barcodeHeight - 2, 255);

    return barcode;
}

BarcodeLayout GetBarcode3(BarcodeConfig config){
    BarcodeLayout barcode = GetBarcodeBase(config);


    int posX[3] = {FINDER_SIZE, config.barcodeWidth - 8 * FINDER_SIZE , FINDER_SIZE};
    int posY[3] = {FINDER_SIZE, FINDER_SIZE, config.barcodeHeight - 8 * FINDER_SIZE };

    DrawSquare(barcode, 255, 1, 1, config.barcodeWidth, 8 * FINDER_SIZE, 8 * FINDER_SIZE);
    DrawSquare(barcode, 255, config.barcodeWidth - 8 * FINDER_SIZE - 1, 1, config.barcodeWidth, 8 * FINDER_SIZE, 8 * FINDER_SIZE);
    DrawSquare(barcode, 255, 1, config.barcodeHeight - 8 * FINDER_SIZE - 1, config.barcodeWidth, 8 * FINDER_SIZE, 8 * FINDER_SIZE);

    for (int i = 0; i < 3; i++){
        DrawSquare(barcode, 0, posX[i], posY[i], config.barcodeWidth, 7 * FINDER_SIZE, 7 * FINDER_SIZE);
        DrawSquare(barcode, 255, posX[i] + 1 * FINDER_SIZE, posY[i] + 1 * FINDER_SIZE, config.barcodeWidth, 5 * FINDER_SIZE, 5 * FINDER_SIZE);
        DrawSquare(barcode, 0, posX[i] + 2 * FINDER_SIZE, posY[i] + 2 * FINDER_SIZE, config.barcodeWidth, 3 * FINDER_SIZE, 3 * FINDER_SIZE);
    }

    barcode.barcodeWidth = config.barcodeWidth;

    return barcode;
}

BarcodeLayout GetBarcode4Simple(BarcodeConfig config) {
    BarcodeLayout barcode = GetBarcode3(config);

    DrawSquare(barcode, 255, config.barcodeWidth - 9, config.barcodeHeight - 9, config.barcodeWidth, 7, 7);
    DrawSquare(barcode, 0, config.barcodeWidth - 8, config.barcodeHeight - 8, config.barcodeWidth, 5, 5);
    DrawSquare(barcode, 255, config.barcodeWidth - 7, config.barcodeHeight - 7, config.barcodeWidth, 3, 3);

    barcode.barcodeWidth = config.barcodeWidth;

    return barcode;
}

BarcodeLayout GetBarcode4Detailed(BarcodeConfig config) {
    BarcodeLayout barcode = GetBarcode3(config);

    DrawSquare(barcode, 0, config.barcodeWidth - 9 * FINDER_SIZE, config.barcodeHeight - 9 * FINDER_SIZE, config.barcodeWidth, 9 * FINDER_SIZE, 9 * FINDER_SIZE);
    DrawSquare(barcode, 255, config.barcodeWidth - 8 * FINDER_SIZE, config.barcodeHeight - 8 * FINDER_SIZE, config.barcodeWidth, 7 * FINDER_SIZE, 7 * FINDER_SIZE);
    DrawSquare(barcode, 0, config.barcodeWidth - 7 * FINDER_SIZE, config.barcodeHeight - 7 * FINDER_SIZE, config.barcodeWidth, 5 * FINDER_SIZE, 5 * FINDER_SIZE);
    DrawSquare(barcode, 255, config.barcodeWidth - 6 * FINDER_SIZE, config.barcodeHeight - 6 * FINDER_SIZE, config.barcodeWidth, 3 * FINDER_SIZE, 3 * FINDER_SIZE);

    barcode.barcodeWidth = config.barcodeWidth;

    return barcode;
}

BarcodeLayout GetBarcode4Circle(BarcodeConfig config) {
    BarcodeLayout barcode = GetBarcode3(config);

    int posX[4] = {FINDER_SIZE, config.barcodeWidth - 8 * FINDER_SIZE, FINDER_SIZE, config.barcodeWidth - 8 * FINDER_SIZE};
    int posY[4] = {FINDER_SIZE, FINDER_SIZE, config.barcodeHeight - 8 * FINDER_SIZE, config.barcodeHeight - 8 * FINDER_SIZE};


    DrawSquare(barcode, 255, 1, 1, config.barcodeWidth, 8 * FINDER_SIZE, 8 * FINDER_SIZE);
    DrawSquare(barcode, 255, config.barcodeWidth - 8 * FINDER_SIZE - 1, 1, config.barcodeWidth, 8 * FINDER_SIZE, 8 * FINDER_SIZE);
    DrawSquare(barcode, 255, 1, config.barcodeHeight - 8 * FINDER_SIZE - 1, config.barcodeWidth, 8 * FINDER_SIZE, 8 * FINDER_SIZE);
    DrawSquare(barcode, 0, config.barcodeWidth - 9 * FINDER_SIZE, config.barcodeHeight - 9 * FINDER_SIZE, config.barcodeWidth, 9 * FINDER_SIZE, 9 * FINDER_SIZE);


    barcode.barcodeWidth = config.barcodeWidth;

    return barcode;
}

int GetCapacity(BarcodeLayout barcode) {
    int capacity = 0;

    for (int i = 0; i < barcode.mask.size(); i++) {
        if (barcode.mask[i] > 0) {
            capacity++;
        }
    }

    return capacity;
}
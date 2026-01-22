#include "barcode_layout.hpp"
#include "_config.hpp"

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

BarcodeLayout GetBarcode3(){
    BarcodeLayout barcode = BarcodeLayout();
    int size = BARCODE_HEIGHT * BARCODE_WIDTH;

    barcode.data.resize(size * 3, 0);
    barcode.mask.resize(size, 0);

    DrawSquare(barcode, 255, 0, 0, BARCODE_WIDTH, BARCODE_WIDTH, BARCODE_HEIGHT);
    DrawSquare(barcode, 0, 1, 1, BARCODE_WIDTH, BARCODE_WIDTH - 2, BARCODE_HEIGHT - 2, 255);

    int posX[3] = {1, BARCODE_WIDTH - 8, 1};
    int posY[3] = {1, 1, BARCODE_HEIGHT - 8};

    DrawSquare(barcode, 255, 1, 1, BARCODE_WIDTH, 8, 8);
    DrawSquare(barcode, 255, BARCODE_WIDTH - 9, 1, BARCODE_WIDTH, 8, 8);
    DrawSquare(barcode, 255, 1, BARCODE_HEIGHT - 9, BARCODE_WIDTH, 8, 8);

    for (int i = 0; i < 3; i++){
        DrawSquare(barcode, 0, posX[i], posY[i], BARCODE_WIDTH, 7, 7);
        DrawSquare(barcode, 255, posX[i] + 1, posY[i] + 1, BARCODE_WIDTH, 5, 5);
        DrawSquare(barcode, 0, posX[i] + 2, posY[i] + 2, BARCODE_WIDTH, 3, 3);
    }

    return barcode;
}

BarcodeLayout GetBarcode4Simple() {
    BarcodeLayout barcode = GetBarcode3();

    DrawSquare(barcode, 255, BARCODE_WIDTH - 9, BARCODE_HEIGHT - 9, BARCODE_WIDTH, 7, 7);
    DrawSquare(barcode, 0, BARCODE_WIDTH - 8, BARCODE_HEIGHT - 8, BARCODE_WIDTH, 5, 5);
    DrawSquare(barcode, 255, BARCODE_WIDTH - 7, BARCODE_HEIGHT - 7, BARCODE_WIDTH, 3, 3);

    return barcode;
}

BarcodeLayout GetBarcode4Detailed() {
    BarcodeLayout barcode = GetBarcode3();

    DrawSquare(barcode, 0, BARCODE_WIDTH - 9, BARCODE_HEIGHT - 9, BARCODE_WIDTH, 9, 9);
    DrawSquare(barcode, 255, BARCODE_WIDTH - 8, BARCODE_HEIGHT - 8, BARCODE_WIDTH, 7, 7);
    DrawSquare(barcode, 0, BARCODE_WIDTH - 7, BARCODE_HEIGHT - 7, BARCODE_WIDTH, 5, 5);
    DrawSquare(barcode, 255, BARCODE_WIDTH - 6, BARCODE_HEIGHT - 6, BARCODE_WIDTH, 3, 3);

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
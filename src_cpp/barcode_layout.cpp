#include "barcode_layout.hpp"
#include "_config.hpp"

void DrawSquare(BarcodeLayout barcode, uint8_t colour, int positionX, int positionY, int barcodeWidth, int squareSize, int mask=0){
    for (int i = 0; i < squareSize; i++){
        int offset = ((i + positionY) * barcodeWidth + positionX);
        memset(barcode.data + offset * 3, colour, squareSize * 3);
        memset(barcode.mask + offset, mask, squareSize);
    }
}

BarcodeLayout* GetBarcode(){
    BarcodeLayout* barcode = new BarcodeLayout();
    int size = BARCODE_SIZE * BARCODE_SIZE;
    barcode->data = (uint8_t*)malloc(sizeof(uint8_t) * size * 3);
    barcode->mask = (uint8_t*)malloc(sizeof(uint8_t) * size);

    memset(barcode->data, 0, size * 3);
    memset(barcode->mask, 255, size);

    DrawSquare(*barcode, 255, 0, 0, BARCODE_SIZE, BARCODE_SIZE);
    DrawSquare(*barcode, 0, 1, 1, BARCODE_SIZE, BARCODE_SIZE - 2, 255);

    int posX[3] = {1, BARCODE_SIZE - 8, 1};
    int posY[3] = {1, 1, BARCODE_SIZE - 8};

    DrawSquare(*barcode, 255, 1, 1, BARCODE_SIZE, 8);
    DrawSquare(*barcode, 255, BARCODE_SIZE - 9, 1, BARCODE_SIZE, 8);
    DrawSquare(*barcode, 255, 1, BARCODE_SIZE - 9, BARCODE_SIZE, 8);

    for (int i = 0; i < 3; i++){
        DrawSquare(*barcode, 0, posX[i], posY[i], BARCODE_SIZE, 7);
        DrawSquare(*barcode, 255, posX[i] + 1, posY[i] + 1, BARCODE_SIZE, 5);
        DrawSquare(*barcode, 0, posX[i] + 2, posY[i] + 2, BARCODE_SIZE, 3);
    }

    return barcode;
}

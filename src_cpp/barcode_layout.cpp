#include "barcode_layout.hpp"

void DrawSquare(BarcodeLayout barcode, uint8_t colour, int positionX, int positionY, int barcodeWidth, int squareSize){
    for (int i = 0; i < squareSize; i++){
        int offset = ((i + positionY) * barcodeWidth + positionX);
        memset(barcode.data + offset * 3, colour, squareSize * 3);
        memset(barcode.mask + offset, 0, squareSize);
    }
}

BarcodeLayout* GetBarcode(){
    BarcodeLayout* barcode = new BarcodeLayout();
    int size = 25 * 25;
    barcode->data = (uint8_t*)malloc(sizeof(uint8_t) * size * 3);
    barcode->mask = (uint8_t*)malloc(sizeof(uint8_t) * size);

    memset(barcode->data, 0, size * 3);
    memset(barcode->mask, 255, size);

    int posX[3] = {0, 18, 0};
    int posY[3] = {0, 0, 18};

    DrawSquare(*barcode, 255, 0, 0, 25, 8);
    DrawSquare(*barcode, 255, 17, 0, 25, 8);
    DrawSquare(*barcode, 255, 0, 17, 25, 8);

    for (int i = 0; i < 3; i++){
        DrawSquare(*barcode, 0, posX[i], posY[i], 25, 7);
        DrawSquare(*barcode, 255, posX[i] + 1, posY[i] + 1, 25, 5);
        DrawSquare(*barcode, 0, posX[i] + 2, posY[i] + 2, 25, 3);
    }

    return barcode;
}

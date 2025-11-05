#include "barcode_layout.hpp"

void DrawSquare(BarcodeLayout barcode, uint8_t colour, int positionX, int positionY, int barcodeWidth, int squareSize, int mask=0){
    for (int i = 0; i < squareSize; i++){
        int offset = ((i + positionY) * barcodeWidth + positionX);
        memset(barcode.data + offset * 3, colour, squareSize * 3);
        memset(barcode.mask + offset, mask, squareSize);
    }
}

BarcodeLayout* GetBarcode(){
    BarcodeLayout* barcode = new BarcodeLayout();
    const int BARCODE_WIDTH = 27;
    int size = BARCODE_WIDTH * BARCODE_WIDTH;
    barcode->data = (uint8_t*)malloc(sizeof(uint8_t) * size * 3);
    barcode->mask = (uint8_t*)malloc(sizeof(uint8_t) * size);

    memset(barcode->data, 0, size * 3);
    memset(barcode->mask, 255, size);

    DrawSquare(*barcode, 255, 0, 0, BARCODE_WIDTH, BARCODE_WIDTH);
    DrawSquare(*barcode, 0, 1, 1, BARCODE_WIDTH, BARCODE_WIDTH - 2, 255);

    int posX[3] = {1, 19, 1};
    int posY[3] = {1, 1, 19};

    DrawSquare(*barcode, 255, 1, 1, BARCODE_WIDTH, 8);
    DrawSquare(*barcode, 255, 18, 1, BARCODE_WIDTH, 8);
    DrawSquare(*barcode, 255, 1, 18, BARCODE_WIDTH, 8);

    for (int i = 0; i < 3; i++){
        DrawSquare(*barcode, 0, posX[i], posY[i], BARCODE_WIDTH, 7);
        DrawSquare(*barcode, 255, posX[i] + 1, posY[i] + 1, BARCODE_WIDTH, 5);
        DrawSquare(*barcode, 0, posX[i] + 2, posY[i] + 2, BARCODE_WIDTH, 3);
    }

    return barcode;
}

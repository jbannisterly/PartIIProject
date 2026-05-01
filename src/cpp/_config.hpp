#pragma once

#define METHOD_PATTERN_4 3
#define TOLERANCE_PIXELS 10
#define WINDOW_SIZE 50
#define BLUR_FORMULA 1
#define ALIGNMENT_ITERATIONS 5

struct BarcodeConfig {
    int barcodeHeight;
    int barcodeWidth;

    BarcodeConfig(int inBarcodeHeight, int inBarcodeWidth) {
        barcodeHeight = inBarcodeHeight;
        barcodeWidth = inBarcodeWidth;
    }
};

enum PatternType {
    THREE, SIMPLE, DETAILED, CIRCLE
};
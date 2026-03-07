#include "barcode_layout.hpp"

class BarcodeWriter {
    private:

    const BarcodeLayout templateLayout;
    std::vector<int> permutation;

    public:

    std::vector<uint8_t> PixelsToBarcode(std::vector<uint8_t> &pixels, int scale=1);
    std::vector<uint8_t> PixelsToBarcodeLegacy(std::vector<uint8_t> &pixels);
    std::vector<uint8_t> BarcodeToPixels(std::vector<uint8_t> barcode, int barcodeSize);
    std::vector<uint8_t> BarcodeToPixelsLegacy(std::vector<uint8_t> barcode, int barcodeSize);
    BarcodeWriter(BarcodeLayout inLayout);
};
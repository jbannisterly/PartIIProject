#include <cstdint>
#include <cstdlib>
#include <cstring>

struct BarcodeLayout{
    uint8_t* data;
    uint8_t* mask;
};

BarcodeLayout* GetBarcode();
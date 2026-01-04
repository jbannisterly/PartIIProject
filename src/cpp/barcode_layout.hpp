#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

struct BarcodeLayout{
    std::vector<uint8_t> data;
    std::vector<uint8_t> mask;
};

BarcodeLayout GetBarcode();
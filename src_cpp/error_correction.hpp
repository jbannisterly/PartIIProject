#include <stdint.h>

template<size_t blockLen, size_t fecLen> 
class ErrorCorrection{
    public:

    std::vector<uint8_t> Encode(std::vector<uint8_t> rawData);
    std::vector<uint8_t> Decode(std::vector<uint8_t> encodedData);
};
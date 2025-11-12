#include "compressor.hpp"

struct DataLen NoCompression::compress(uint8_t* data, int len){
    return DataLen(data, len);
}

struct DataLen NoCompression::decompress(uint8_t* data, int len){
    return DataLen(data, len);
}
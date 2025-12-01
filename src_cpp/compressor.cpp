#include "compressor.hpp"
#include <cstdlib>
#include <iostream>

struct DataLen NoCompression::compress(uint8_t* data, int len){
    return DataLen(data, len);
}

struct DataLen NoCompression::decompress(uint8_t* data, int len){
    return DataLen(data, len);
}

struct DataLen Compression::compress(uint8_t* data, int len){
    uint8_t* compressed = (uint8_t*)malloc(sizeof(uint8_t) * len * 2);

    for (int i = 0; i < len * 2; i++){
        compressed[i] = 100;
    }

    ulong compressedLen = len * 2;
    compress2(compressed, &compressedLen, data, uLongf(len), 2);

    return DataLen(compressed, int(compressedLen));
}

struct DataLen Compression::decompress(uint8_t* data, int len){
    uLongf decompressedSize = len * 2;
    uint8_t* decompressed;
    uLongf compressedLength = len;

    int error = -1;
    while(error){
        decompressedSize *= 2;
        decompressed = (uint8_t*)malloc(sizeof(uint8_t) * decompressedSize);
        error = uncompress(decompressed, &decompressedSize, data, compressedLength);
    }
    
    return DataLen(decompressed, decompressedSize);
}
#include "compressor.hpp"
#include <cstdlib>
#include <iostream>
#include <stdint.h>

std::vector<uint8_t> NoCompression::compress(uint8_t* data, int len){
    return std::vector<uint8_t> (data, data + len);
}

std::vector<uint8_t> NoCompression::decompress(uint8_t* data, int len){
    return std::vector<uint8_t>(data, data + len);
}

std::vector<uint8_t> Compression::compress(uint8_t* data, int len){
    uint8_t* compressed = (uint8_t*)malloc(sizeof(uint8_t) * len * 2);

    for (int i = 0; i < len * 2; i++){
        compressed[i] = 100;
    }

    ulong compressedLen = len * 2;
    compress2(compressed, &compressedLen, data, uLongf(len), 2);

    std::vector<uint8_t> compressedVector(compressed, compressed + compressedLen);
    free(compressed);

    return compressedVector;
}

std::vector<uint8_t> Compression::decompress(uint8_t* data, int len){
    uLongf decompressedSize = len * 2;
    uint8_t* decompressed;
    uLongf compressedLength = len;

    int error = -1;
    while(error){
        decompressedSize *= 2;
        decompressed = (uint8_t*)malloc(sizeof(uint8_t) * decompressedSize);
        error = uncompress(decompressed, &decompressedSize, data, compressedLength);
    }
    
    std::vector<uint8_t> decompressedVector(decompressed, decompressed + decompressedSize);
    free(decompressed);

    return decompressedVector;
}
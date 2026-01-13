#include "compressor.hpp"
#include <cstdlib>
#include <iostream>
#include <stdint.h>

std::vector<uint8_t> NoCompression::compress(std::vector<uint8_t> &data){
    return std::vector<uint8_t> (data);
}

std::vector<uint8_t> NoCompression::decompress(uint8_t* data, int len){
    return std::vector<uint8_t>(data, data + len);
}

std::vector<uint8_t> Compression::compress(std::vector<uint8_t> &data){
    uint8_t* compressed = (uint8_t*)malloc(sizeof(uint8_t) * data.size() * 2);
    ulong compressedLen = data.size() * 2;

    for (int i = 0; i < compressedLen; i++){
        compressed[i] = 100;
    }

    compress2(compressed, &compressedLen, data.data(), uLongf(data.size()), 2);

    std::vector<uint8_t> compressedVector(compressed, compressed + compressedLen);
    free(compressed);

    std::cout << "compressed" << std::endl;
    for (int i = 0; i < compressedVector.size(); i++) {
        std::cout << int(compressedVector[i]) << " ";
    }
    std::cout << std::endl;

    return compressedVector;
}

std::vector<uint8_t> Compression::decompress(uint8_t* data, int len){
    uLongf decompressedSize = len * 2;
    uint8_t* decompressed = (uint8_t*)malloc(sizeof(uint8_t));
    uLongf compressedLength = len;

    std::cout << "compressed" << std::endl;
    for (int i = 0; i < len; i++) {
        std::cout << int(data[i]) << " ";
    }
    std::cout << std::endl;

    int error = -1;
    while(error){
        free(decompressed);
        decompressedSize *= 2;
        decompressed = (uint8_t*)malloc(sizeof(uint8_t) * decompressedSize);
        error = uncompress(decompressed, &decompressedSize, data, compressedLength);
        if (error == Z_DATA_ERROR) {
            throw new ExceptionDecompression();
        }
    }
    
    std::vector<uint8_t> decompressedVector(decompressed, decompressed + decompressedSize);
    free(decompressed);

    return decompressedVector;
}
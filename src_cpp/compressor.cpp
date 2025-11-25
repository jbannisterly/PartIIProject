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
    ulong compressedLen = len * 2;
    std::cout << "compress\n";
    std::cout << compress2(compressed, &compressedLen, data, uLongf(len), 2) << std::endl;
    for (int i = 0; i < len; i++){
        std::cout << int(data[i]) << " " << int(compressed[i]) << " " << compressed[i] << std::endl;
    }
    return DataLen(compressed, int(compressedLen));
}

struct DataLen Compression::decompress(uint8_t* data, int len){
    uLongf decompressedSize = len * 2;
    uint8_t* decompressed;
    uLongf compressedLength = len;

    std::cout << "decompress\n";

    int error = -1;
    while(error){
        decompressedSize *= 2;
        decompressed = (uint8_t*)malloc(sizeof(uint8_t) * decompressedSize);
        error = uncompress(decompressed, &decompressedSize, data, compressedLength);
        std::cout << error << std::endl;
    }

    for (int i = 0; i < decompressedSize; i++){
        std::cout << int(decompressed[i]);
    }
    
    return DataLen(decompressed, decompressedSize);
}
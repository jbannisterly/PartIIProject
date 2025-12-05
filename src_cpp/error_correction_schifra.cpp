#include <schifra/schifra_galois_field.hpp>
#include <schifra/schifra_galois_field_polynomial.hpp>
#include <schifra/schifra_sequential_root_generator_polynomial_creator.hpp>
#include <schifra/schifra_reed_solomon_encoder.hpp>
#include <schifra/schifra_reed_solomon_decoder.hpp>
#include <schifra/schifra_reed_solomon_block.hpp>
#include <schifra/schifra_error_processes.hpp>
#include <iostream>
#include <stdint.h>
#include <cmath> 
#include "error_correction.hpp"

template<size_t blockLen, size_t fecLen> 
std::vector<uint8_t> ErrorCorrection<blockLen, fecLen>::Encode(std::vector<uint8_t> rawData){
    int nBlocks = std::ceil((float)rawData.size() / (blockLen - fecLen));

    std::vector<uint8_t> encodedData;

    schifra::galois::field field(8, schifra::galois::primitive_polynomial_size06, schifra::galois::primitive_polynomial06);
    schifra::galois::field_polynomial genPoly(field);
    schifra::make_sequential_root_generator_polynomial(field, 120, fecLen, genPoly);
    schifra::reed_solomon::encoder<blockLen, fecLen, (blockLen - fecLen)> encoder(field, genPoly);

    schifra::reed_solomon::block<blockLen, fecLen> block;
    std::string inData;

    for (int i = 0; i < nBlocks; i++){
        if (i == nBlocks - 1){
            inData = std::string((char*)rawData.data() + i * (blockLen - fecLen), rawData.size() % (blockLen - fecLen));
            inData.resize((blockLen - fecLen), 0);
        }else{
            inData = std::string((char*)rawData.data() + i * (blockLen - fecLen), (blockLen - fecLen));
        }
        encoder.encode(inData, block);

        for (int j = 0; j < blockLen; j++){
            encodedData.push_back(block.data[j]);
        }
    }

    return encodedData;
}

template<size_t blockLen, size_t fecLen> 
std::vector<uint8_t> ErrorCorrection<blockLen, fecLen>::Decode(std::vector<uint8_t> encodedData){
    int nBlocks = encodedData.size() / blockLen;

    std::vector<uint8_t> decodedData;

    schifra::galois::field field(8, schifra::galois::primitive_polynomial_size06, schifra::galois::primitive_polynomial06);
    schifra::reed_solomon::decoder<blockLen, fecLen, (blockLen - fecLen)> decoder(field, 120);

    schifra::reed_solomon::block<blockLen, fecLen> block;

    for (int i = 0; i < nBlocks; i++){
        std::string data((char*)encodedData.data() + i * blockLen, blockLen  - fecLen);
        std::string fec((char*)encodedData.data() + i * blockLen + blockLen - fecLen, fecLen); 

        block = schifra::reed_solomon::block<blockLen, fecLen>(data, fec);

        decoder.decode(block);

        for (int j = 0; j < (blockLen - fecLen); j++){
            decodedData.push_back(block.data[j]);
        }
    }

    return decodedData;
}


int main(){
    ErrorCorrection<128, 64> err;

    std::vector<uint8_t> input;

    for (int i = 0; i < 1000; i++) {
        input.push_back((i * 3) % 78);
    }

    std::vector<uint8_t> output = err.Encode(input);

    for (int i = 100; i < 120; i++){ // noise
        output[i] = 88;
    }

    std::vector<uint8_t> decoded = err.Decode(output);

    std::cout << decoded.size() << std::endl;
    for (int i = 0; i < input.size(); i++) {
        if (input[i] != decoded[i]) std::cout << "fail at " << i << std::endl;
    }

    return 0;
}
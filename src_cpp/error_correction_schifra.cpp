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

std::vector<uint8_t> Encode(std::vector<uint8_t> rawData){
    int nBlocks = std::ceil((float)rawData.size() / 223);

    std::vector<uint8_t> encodedData;

    schifra::galois::field field(8, schifra::galois::primitive_polynomial_size06, schifra::galois::primitive_polynomial06);
    schifra::galois::field_polynomial genPoly(field);
    schifra::make_sequential_root_generator_polynomial(field, 120, 32, genPoly);
    schifra::reed_solomon::encoder<255, 32, 223> encoder(field, genPoly);

    schifra::reed_solomon::block<255, 32> block;
    std::string inData;

    for (int i = 0; i < nBlocks; i++){
        if (i == nBlocks - 1){
            inData = std::string((char*)rawData.data(), rawData.size() % 223);
            inData.resize(223, 0);
        }else{
            inData = std::string((char*)rawData.data(), 223);
        }
        encoder.encode(inData, block);

        for (int j = 0; j < 256; j++){
            encodedData.push_back(block.data[j]);
        }
    }

    return encodedData;
}

int main(){
    std::vector<uint8_t> input;

    for (int i = 0; i < 1000; i++) {
        input.push_back((i * 3) % 78);
    }

    std::vector<uint8_t> output = Encode(input);

    std::cout << output.size() << std::endl;
    for (int i = 0; i < output.size(); i++) std::cout << (int)output[i] << " ";

    return 0;
}
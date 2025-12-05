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
            inData = std::string((char*)rawData.data() + i * 223, rawData.size() % 223);
            inData.resize(223, 0);
        }else{
            inData = std::string((char*)rawData.data() + i * 223, 223);
        }
        encoder.encode(inData, block);

        for (int j = 0; j < 255; j++){
            encodedData.push_back(block.data[j]);
        }
    }

    return encodedData;
}

std::vector<uint8_t> Decode(std::vector<uint8_t> encodedData){
    int nBlocks = encodedData.size() / 255;

    std::vector<uint8_t> decodedData;

    schifra::galois::field field(8, schifra::galois::primitive_polynomial_size06, schifra::galois::primitive_polynomial06);
    schifra::reed_solomon::decoder<255, 32, 223> decoder(field, 120);

    schifra::reed_solomon::block<255, 32> block;

    for (int i = 0; i < nBlocks; i++){
        std::string data((char*)encodedData.data() + i * 255, 223);
        std::string fec((char*)encodedData.data() + i * 255 + 223, 32); 

        block = schifra::reed_solomon::block<255, 32>(data, fec);

        decoder.decode(block);

        for (int j = 0; j < 223; j++){
            decodedData.push_back(block.data[j]);
        }
    }

    return decodedData;
}

int main(){
    std::vector<uint8_t> input;

    for (int i = 0; i < 1000; i++) {
        input.push_back((i * 3) % 78);
    }

    std::vector<uint8_t> output = Encode(input);

    for (int i = 0; i < 10; i++){ // noise
        output[i] = 88;
    }

    std::vector<uint8_t> decoded = Decode(output);

    std::cout << decoded.size() << std::endl;
    for (int i = 0; i < input.size(); i++) {
        if (input[i] != decoded[i]) std::cout << "fail";
    }

    return 0;
}
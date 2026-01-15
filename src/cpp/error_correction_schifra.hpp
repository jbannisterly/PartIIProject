#pragma once

#include <stdint.h>
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
#include <fstream>

class ErrorCorrectionVirtual {
    public:
    
    virtual std::vector<uint8_t> Encode(std::vector<uint8_t> &rawData) = 0;
    virtual std::vector<uint8_t> Decode(std::vector<uint8_t> encodedData) = 0;
    virtual uint8_t getBlockLen() = 0;
    virtual uint8_t getFECLen() = 0;
    virtual uint8_t getDataLen() = 0;
    // virtual ~ErrorCorrectionVirtual() {}
    // virtual ErrorCorrectionVirtual() {}
    // virtual ErrorCorrectionVirtual(&ErrorCorrectionVirtual) {}

};

template<int blockLen, int fecLen>
class ErrorCorrection: public ErrorCorrectionVirtual {
    public:

    std::vector<uint8_t> Encode(std::vector<uint8_t> &rawData) override {
        int nBlocks = std::ceil((float)rawData.size() / (blockLen - fecLen));

        std::vector<uint8_t> encodedData;

        schifra::galois::field field(8, schifra::galois::primitive_polynomial_size06, schifra::galois::primitive_polynomial06);
        schifra::galois::field_polynomial genPoly(field);
        schifra::make_sequential_root_generator_polynomial(field, 120, fecLen, genPoly);
        schifra::reed_solomon::encoder<blockLen, fecLen, (blockLen - fecLen)> encoder(field, genPoly);

        schifra::reed_solomon::block<blockLen, fecLen> block;
        std::string inData;

        int padding = nBlocks * (blockLen - fecLen) - rawData.size();
        std::vector<uint8_t> paddedData;
        paddedData.reserve(nBlocks * (blockLen - fecLen));
        paddedData.insert(paddedData.end(), rawData.begin(), rawData.end());
        for (int i = 0; i < padding; i++) {
            paddedData.push_back(0);
        }

        for (int i = 0; i < nBlocks; i++){
            inData = std::string((char*)paddedData.data() + i * (blockLen - fecLen), (blockLen - fecLen));
            inData.resize(blockLen, 0);
            encoder.encode(inData, block);

            std::string blockDataOut;
            blockDataOut.resize(blockLen);
            block.data_to_string(blockDataOut);
            std::cout << blockDataOut.size();

            for (int j = 0; j < blockLen; j++){
                encodedData.push_back(block.data[j]);
            }
        }

        return encodedData;
    }

    std::vector<uint8_t> Decode(std::vector<uint8_t> encodedData) override {
        int nBlocks = encodedData.size() / blockLen;

        std::vector<uint8_t> decodedData;

        schifra::galois::field field(8, schifra::galois::primitive_polynomial_size06, schifra::galois::primitive_polynomial06);
        schifra::reed_solomon::decoder<blockLen, fecLen, (blockLen - fecLen)> decoder(field, 120);

        schifra::reed_solomon::block<blockLen, fecLen> block;

        for (int i = 0; i < nBlocks; i++){
            std::string data((char*)encodedData.data() + i * blockLen, blockLen  - fecLen);
            std::string fec((char*)encodedData.data() + i * blockLen + blockLen - fecLen, fecLen); 

            block = schifra::reed_solomon::block<blockLen, fecLen>(data, fec);
            if (!decoder.decode(block)) {
                std::cout << "Error in decoding" << std::endl;
            }

            for (int j = 0; j < (blockLen - fecLen); j++){
                decodedData.push_back(block.data[j]);
            }
        }

        return decodedData;
    }

    uint8_t getBlockLen() override {
        return blockLen;
    }

    uint8_t getFECLen() override {
        return fecLen;
    }

    uint8_t getDataLen() override {
        return blockLen - fecLen;
    }
};
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

class ErrorCorrectionVirtual {
    public:
    
    virtual std::vector<uint8_t> Encode(std::vector<uint8_t> rawData) = 0;
    virtual std::vector<uint8_t> Decode(std::vector<uint8_t> encodedData) = 0;
    virtual uint8_t getBlockLen() = 0;
    virtual uint8_t getFECLen() = 0;
    virtual uint8_t getDataLen() = 0;
    // virtual ~ErrorCorrectionVirtual() {}
    // virtual ErrorCorrectionVirtual() {}
    // virtual ErrorCorrectionVirtual(&ErrorCorrectionVirtual) {}

};

template<uint8_t blockLen, uint8_t fecLen>
class ErrorCorrection: public ErrorCorrectionVirtual {
    public:

    std::vector<uint8_t> Encode(std::vector<uint8_t> rawData) override {
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
            decoder.decode(block);

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
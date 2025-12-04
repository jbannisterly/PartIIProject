#include <schifra/schifra_galois_field.hpp>
#include <schifra/schifra_galois_field_polynomial.hpp>
#include <schifra/schifra_sequential_root_generator_polynomial_creator.hpp>
#include <schifra/schifra_reed_solomon_encoder.hpp>
#include <schifra/schifra_reed_solomon_decoder.hpp>
#include <schifra/schifra_reed_solomon_block.hpp>
#include <schifra/schifra_error_processes.hpp>
#include <iostream>



int main(){
    schifra::galois::field field(8, schifra::galois::primitive_polynomial_size06, schifra::galois::primitive_polynomial06);
    
    schifra::galois::field_polynomial genPoly(field);
    schifra::make_sequential_root_generator_polynomial(field, 120, 32, genPoly);
    schifra::reed_solomon::encoder<255, 32, 223> encoder(field, genPoly);

    std::string message = "Hello world. Lorem Ipsum Dolor Sit Amet.";

    message.resize(255, 0);

    schifra::reed_solomon::block<255, 32> block;
    encoder.encode(message, block);

    for (int i = 0; i < 256; i++){
        std::cout << block.data[i] << " ";
    }

    return 0;
}
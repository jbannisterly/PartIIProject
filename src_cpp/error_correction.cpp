#include <iostream>
#include "error_correction.hpp"

class Galois{
    public:
    uint32_t data;

    int degree(){
        int i = 0;
        uint32_t value = this->data;
        while(value){
            value >>= 1;
            i++;
        }

        return i - 1;
    }

    Galois(uint32_t inData): data{inData}{
        
    }

    Galois operator+(Galois g){
        return Galois(this->data ^ g.data);    
    }

    Galois operator%(Galois g){
        uint32_t mask = 1;
        uint32_t a = this->data;
        uint32_t b = g.data;
        int degree = g.degree();

        mask <<= 31;
        b <<= (31 - degree);

        for (int i = 0; i <= (31-degree); i++){
            if (mask & a){
                a ^= b;
            }
            
            mask >>= 1;
            b >>= 1;
        }

        return Galois(a);
    }

    Galois operator*(Galois g){
        uint32_t result = 0;
        uint32_t mask = 1;
        uint32_t d0 = this->data;
        uint32_t d1 = g.data;

        for (int i = 0; i < 32; i++){
            if (mask & d1) {
                result ^= d0;
            }
            d0 <<= 1;
            mask <<= 1;
        }

        return Galois(result);
    }
};

int main(){
    Galois g1(7);
    Galois g2(1 + 16);

    std::cout << (g2 % g1).data << std::endl;
}
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

class Polynomial{
    Galois irreducible;
    std::vector<Galois> coefficients;
    
    public:

    Polynomial(Galois inIrreducible, std::vector<Galois> inCoefficients): 
    irreducible{inIrreducible}, 
    coefficients{inCoefficients}
    {

    }

    Galois Evaluate(Galois inX){
        Galois x(inX.data);
        Galois y(coefficients[0]);

        for (int i = 1; i < coefficients.size(); i++){
            y = y + ((x * coefficients[i]) % irreducible);
            std::cout << ((x * coefficients[i]) % irreducible).data << std::endl;
            std::cout << x.data << " " << y.data << std::endl;
            x = (x * inX) % irreducible;
        }
    
        return y;
    }
};

int main(){
    Galois g1(6);
    Galois g2(4);
    Galois g3(2);

    Galois irr(13);

    std::vector<Galois> coefficients;

    coefficients.push_back(g1);
    coefficients.push_back(g2);
    coefficients.push_back(g3);

    Polynomial polynomial(irr, coefficients);

    for (int i = 0; i < 3; i++){
        Galois result = polynomial.Evaluate(Galois(i));
        std::cout << i << " " << result.data << std::endl << std::endl;
    }

    std::cout << (Galois(8) % Galois(13)).data << std::endl;
}
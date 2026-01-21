#include "random_data_gen.hpp"

namespace RandomDataGen {
    std::vector<uint8_t> GenerateRandomData(int seed, int length) {
        std::vector<uint8_t> data;
        data.reserve(length);
        std::srand(seed);

        for (int i = 0; i < length; i++) {
            data.push_back(std::rand() & 255);
        }

        return data;
    }

    std::vector<std::vector<uint8_t>> GenerateRandomData(int seed, int nPixels, int nChannels) {
        std::vector<std::vector<uint8_t>> data;
        data.reserve(nChannels);

        for (int i = 0; i < nChannels; i++) {
            data.push_back(GenerateRandomData(seed + i, nPixels));
        } 

        return data;
    }
}

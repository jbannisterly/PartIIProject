#include <vector>
#include <cstdint>
#include <cstdlib>

namespace RandomDataGen {
    std::vector<uint8_t> GenerateRandomData(int seed, int length);
    std::vector<std::vector<uint8_t>> GenerateRandomData(int seed, int nPixels, int nChannels);
}   
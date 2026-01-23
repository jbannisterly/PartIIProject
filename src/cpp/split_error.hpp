#include <vector>
#include <cstdint>
#include "error_correction_schifra.hpp"

class SplitError {
    private:

    std::vector<ErrorCorrectionVirtual*> errorCorrectors;

    public:

    std::vector<std::vector<uint8_t>> Encode(std::vector<std::vector<uint8_t>> &splitBytes);
    std::vector<std::vector<uint8_t>> Decode(std::vector<std::vector<uint8_t>> &splitBytes);
    SplitError(std::vector<ErrorCorrectionVirtual*> &errorCorrectors);
    int GetErrorCount();

};
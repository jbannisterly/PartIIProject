#include "splitError.hpp"

    std::vector<std::vector<uint8_t>> SplitError::Encode(std::vector<std::vector<uint8_t>> &splitBytes) {
        std::vector<std::vector<uint8_t>> splitError;
        splitError.reserve(splitBytes.size());

        for (int i = 0; i < splitBytes.size(); i++) {
            std::vector<uint8_t> encodedSplit = errorCorrectors[i]->Encode(splitBytes[i]);
            splitError.push_back(encodedSplit);
        }

        return splitError;
    }

    std::vector<std::vector<uint8_t>> SplitError::Decode(std::vector<std::vector<uint8_t>> &splitBytes) {
        std::vector<std::vector<uint8_t>> splitError;
        splitError.reserve(splitBytes.size());
    
        for (int i = 0; i < splitBytes.size(); i++) {
            splitError.push_back(errorCorrectors[i]->Decode(splitBytes[i]));
        }

        return splitError;
    }

    SplitError::SplitError(std::vector<ErrorCorrectionVirtual*> &inErrorCorrectors) :
    errorCorrectors{inErrorCorrectors} {}

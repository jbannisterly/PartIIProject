#include "split_bytes.hpp"

namespace SplitBytes {
    std::vector<std::vector<uint8_t>> Encode(std::vector<uint8_t> &data, std::vector<ErrorCorrectionVirtual*> &errorCorrectors) {
        std::vector<std::vector<uint8_t>> splitBytes;
        splitBytes.reserve(errorCorrectors.size());
    
        int bytesPerChunk = 0;
    
        for (int i = 0; i < errorCorrectors.size(); i++) {
            bytesPerChunk += errorCorrectors[i]->getDataLen();
        }

        int nChunks = int(ceil(data.size() / (float)bytesPerChunk));

        int startIndex = 0;
        int endIndex = 0;
        int padding = 0;

        for (int i = 0; i < errorCorrectors.size(); i++) {
            int bytesPerSplit = (errorCorrectors[i]->getDataLen()) * nChunks;
            endIndex = startIndex + bytesPerSplit;

            if (endIndex > data.size()) {
                padding = endIndex - data.size();
                endIndex = data.size();
            }
            std::vector<uint8_t> splitData(data.cbegin() + startIndex, data.cbegin() + endIndex);
            for (int j = 0; j < padding; j++) {
                splitData.push_back(0);
            }

            splitBytes.push_back(splitData);
            startIndex = endIndex;
        }

        return splitBytes;
    }

    std::vector<uint8_t> Decode(std::vector<std::vector<uint8_t>> &splitData, int dataLength) {
        std::vector<uint8_t> connectedData;
        connectedData.reserve(dataLength);

        for (int i = 0; i < splitData.size(); i++) {
            connectedData.insert(connectedData.end(), splitData[i].begin(), splitData[i].end());
        }
        return connectedData;
    }
}
#include "header_data.hpp"

namespace HeaderData {

    void PrependLength(std::vector<uint8_t> &data, bool compressed) {
        uint8_t compressedFlag;

        if (compressed) {
            compressedFlag = 255;
        } else {
            compressedFlag = 0;
        }

        data.insert(data.begin(), {
            compressedFlag,
            (uint8_t)((data.size() >> 0) & 255),
            (uint8_t)((data.size() >> 8) & 255)
        });
    }

    int GetErrorCorrectionLen(int compressedLen, std::vector<ErrorCorrectionVirtual*> &errorCorrectors) {
        int bytesPerChunk = 0;
    
        for (int i = 0; i < errorCorrectors.size(); i++) {
            bytesPerChunk += errorCorrectors[i]->getDataLen();
        }

        int nChunks = int(ceil(compressedLen / (float)bytesPerChunk));

        return nChunks * errorCorrectors[0]->getBlockLen();
    }

    int GetCompressedLen(std::vector<uint8_t> &pixels, ColourPixels colourPix, ErrorCorrectionVirtual* errorCorrection) {
        std::vector<std::vector<uint8_t>> header = colourPix.PixelsToData(pixels, 0, errorCorrection->getBlockLen());
        std::vector<uint8_t> headerCorrected = errorCorrection->Decode(header[0]);
    
        return ((int)headerCorrected[1] | ((int)headerCorrected[2]) << 8);
    }

    bool GetCompressedFlag(std::vector<uint8_t> &pixels, ColourPixels colourPix, ErrorCorrectionVirtual* errorCorrection) {
        std::vector<std::vector<uint8_t>> header = colourPix.PixelsToData(pixels, 0, errorCorrection->getBlockLen());
        std::vector<uint8_t> headerCorrected = errorCorrection->Decode(header[0]);
    
        int setBits = 0;
        for (int i = 0; i < 8; i++) {
            if (((headerCorrected[0] >> i) & 1) == 1) {
                setBits++;
            }
        }

        return setBits > 4;
    }

}
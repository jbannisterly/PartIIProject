#include "header_data.hpp"

namespace HeaderData {

    void PrependLength(std::vector<uint8_t> &data) {
        data.insert(data.begin(), {
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
    
        return ((int)headerCorrected[0] | ((int)headerCorrected[1]) << 8) + 2;
    }

}
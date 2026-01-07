#include <zlib.h>
#include <stdint.h>
#include <vector>

struct DataLen{
    uint8_t* data;
    int len;

    DataLen(uint8_t* inData,int inLen):
    data{inData},
    len{inLen}
    {}
};

class NoCompression{
    public:

    static std::vector<uint8_t> compress(std::vector<uint8_t> &data);
    static std::vector<uint8_t> decompress(uint8_t* data, int len);
};

class Compression{
    public:

    static std::vector<uint8_t> compress(std::vector<uint8_t> &data);
    static std::vector<uint8_t> decompress(uint8_t* data, int len);
};

class ExceptionDecompression: std::exception {
    public:
    
    const char* what() const noexcept override{
        return "Cannot decompress data.";
    }
};

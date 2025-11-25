#include <cstdint>
#include <zlib.h>

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

    static DataLen compress(uint8_t* data, int len);
    static DataLen decompress(uint8_t* data, int len);
};

class Compression{
    public:

    static DataLen compress(uint8_t* data, int len);
    static DataLen decompress(uint8_t* data, int len);
};
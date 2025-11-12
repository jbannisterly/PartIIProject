#include <cstdint>

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
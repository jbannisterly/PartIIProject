#pragma once

#include <vector>
#include <stdint.h>

namespace Border {
    struct Border {
        std::vector<int> borderMembers;
    };

    std::vector<Border> GetBorders(std::vector<uint8_t> &threshold, int width);

}
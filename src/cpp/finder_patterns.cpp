#include "finder_patterns.hpp"
#include <iostream>

namespace FinderPatterns {

int CountSame(std::vector<uint8_t> &data, int index, int size){
    int i = 0; 
    if (index == data.size()) return 0;
    uint8_t value = data[index];

    while(index + i < size && data[index + i] == value){
        i++;
    }

    return i;
}

    std::vector<FinderCandidate> FinderPatterns(int patternSize, std::vector<uint8_t> &data, int sizeY, int sizeX, std::function<bool (std::vector<int>)> patternValid, bool firstWhite){    
    int index = 0;
    std::vector<int> w;
    w.resize(patternSize);
    int wIndex = 0;
    int size = sizeX * sizeY;
    std::vector<FinderCandidate> finder;
    
    for (int i = 0; i < patternSize; i++){
        w[i] = CountSame(data, wIndex, size);
        wIndex += w[i];
    }

    while(wIndex < size){
        double width = 0;
        for (int i = 0; i < patternSize; i++) {
            width += w[i];
        }
        bool valid = patternValid(w);
        if(valid && width > 0 && (
            (firstWhite && data[wIndex] < 255) ||
            (!firstWhite && data[wIndex] > 100)
            )
        ){
            double centre = wIndex;
            for (int j = 0; j < patternSize; j++){
                centre -= w[j] / 2;
            }

            finder.push_back(FinderCandidate(centre / sizeX, centre - int(centre / sizeX) * sizeX, width));
        }

        // Read another
        for (int i = 0; i < patternSize - 1; i++){
            w[i] = w[i + 1];
        }
        for (int i = patternSize - 1; i < patternSize; i++){
            w[i] = CountSame(data, wIndex, size);
            wIndex += w[i];
        }
    }

    return finder;
} 

}
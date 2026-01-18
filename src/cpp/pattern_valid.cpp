#include "pattern_valid.hpp"
#include "_config.hpp"

namespace PatternValid {
    bool PatternStandard(std::vector<int> w){
        int avg = 0;
        for (int i = 0; i < 5; i++){
            avg += w[i] / 7;
        }

        for (int i = 0; i < 5; i++){
            if (i == 2){
                if (w[i] > 3 * avg + TOLERANCE_PIXELS) return false; // tolerance is 10x more than paper
                if (w[i] < 3 * avg - TOLERANCE_PIXELS) return false;
                if (w[2] < w[0] + w[1]) return false;
                if (w[2] < w[3] + w[4]) return false;
            }else{
                if (w[i] > avg + TOLERANCE_PIXELS) return false;
                if (w[i] < avg - TOLERANCE_PIXELS * 0.75) return false;
            }
        }

        return true;
    }

    bool PatternSmall(std::vector<int> w){
        const int TOLERANCE = 20;

        int avg = 0;
        for (int i = 0; i < 3; i++){
            avg += w[i] / 5;
        }

        for (int i = 0; i < 3; i++){
            if (i == 1){
            if (w[i] > 3 * avg + TOLERANCE) return false; // tolerance is 10x more than paper
            if (w[i] < 3 * avg - TOLERANCE) return false;
                if (w[1] < w[0] + w[2]) return false;
            }else{
                if (w[i] > avg + TOLERANCE) return false;
                if (w[i] < avg - TOLERANCE * 0.75) return false;
            }
        }

        return true;
    }
}
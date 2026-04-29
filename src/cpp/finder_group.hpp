#include "finder_patterns.hpp"
#include <vector>
#include <cstdint>
#include <functional>

class FinderGroup{
    public:
    FinderCandidate Centre();
    FinderCandidate CentreRefined();

    bool TryAddCandidate(FinderCandidate toAdd);
    bool isValid(std::vector<uint8_t> &threshold, int dataX, int dataY, int patternSize, std::function<bool (std::vector<int>)> patternValid, bool firstWhite);
    int size();
    
    private:
    std::vector<FinderCandidate> candidates;
    std::vector<FinderCandidate> verticalCandidates;

    Position VerticalOffset();

    // Get rectangle around candidates to check for finder pattern
    VerticalData VerticalSample(std::vector<uint8_t> &data, int dataX, int dataY);
    std::vector<FinderCandidate> FinderPatternVertical(int patternSize, VerticalData vertical, std::function<bool (std::vector<int>)> patternValid, bool firstWhite);
};
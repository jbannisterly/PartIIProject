#include <vector>
#include <cstdint>
#include "vector_helper.hpp"
#include "finder_patterns.hpp"
#include "cmath"

namespace BoundingBox {
    std::array<Vec3, 4> BoundingBox(std::array<FinderCandidate, 3> finders, FinderCandidate finder4);
    std::array<Vec3, 4> BoundingBox(std::array<FinderCandidate, 3> finders);
    std::array<Vec3, 4> BoundingBoxRectangle(std::array<FinderCandidate, 3> finders, FinderCandidate finder4);
    std::array<Vec3, 4> BoundingBoxSimple(std::array<FinderCandidate, 3> finders, FinderCandidate finder4);
    std::array<Vec3, 4> ExpansionBox(std::array<Vec3, 4> initialBounds, double expansionFactor);
}
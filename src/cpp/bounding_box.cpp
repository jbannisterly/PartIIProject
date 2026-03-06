#include "bounding_box.hpp"

namespace BoundingBox {
    void BoundingBoxMissingCorner_1(std::array<FinderCandidate, 3> &finders, std::array<Vec3, 3> &findersVec,  std::array<Vec3, 4> bounds){
        bounds[3] = bounds[0] + bounds[2] - bounds[1];
    }

    void BoundingBoxMissingCorner_2(std::array<FinderCandidate, 3> &finders, std::array<Vec3, 3>  &findersVec, std::array<Vec3, 4> &bounds){
        const double MODULEOFFSET = 4.5;
        Vec3 bottomLeftDir = ((findersVec[0] - findersVec[1]).Normalise() * finders[0].width * (MODULEOFFSET / 7)) + findersVec[0] - bounds[0];
        Vec3 topRightDir = ((findersVec[2] - findersVec[1]).Normalise() * finders[2].width * (MODULEOFFSET / 7)) + findersVec[2] - bounds[2];

        bounds[3] = Intersection(bottomLeftDir, topRightDir, bounds[0], bounds[2]);    
    }

    void BoundingBoxMissingCorner_3(std::array<FinderCandidate, 3> &finders, std::array<Vec3, 3>  &findersVec, std::array<Vec3, 4> &bounds){
        const double MODULEOFFSET = 4.5;
        Vec3 bottomLeftDir = bounds[1] - bounds[2];
        Vec3 topRightDir = bounds[1] - bounds[0];

        bounds[3] = Intersection(bottomLeftDir, topRightDir, bounds[0], bounds[2]);    
    }

    std::array<Vec3, 4> BoundingBox(std::array<FinderCandidate, 3> finders){
        const double MODULEOFFSET = 4.5;

        std::array<Vec3, 3> findersVec;
        std::array<Vec3, 4> bounds;
    
        for (int i = 0; i < 3; i++){
            findersVec[i] = Vec3(finders[i].x, finders[i].y, 0);
        }

        bounds[0] = findersVec[0] + ((findersVec[0] - findersVec[2]).Normalise() * finders[0].width * sqrt(2) * (MODULEOFFSET / 7));
        bounds[1] = findersVec[1] + (((findersVec[1] - findersVec[2]).Normalise() + 
                    (findersVec[1] - findersVec[0]).Normalise())
                    * finders[1].width) * (MODULEOFFSET / 7);
        bounds[2] = findersVec[2] + ((findersVec[2] - findersVec[0]).Normalise() * finders[2].width * sqrt(2) * (MODULEOFFSET / 7));

        BoundingBoxMissingCorner_3(finders, findersVec, bounds);

        return bounds;
    }

    std::array<Vec3, 4> BoundingBox(std::array<FinderCandidate, 3> finders, FinderCandidate finder4){
        const double MODULEOFFSET = 4;

        std::array<Vec3, 4> findersVec;
        std::array<Vec3, 4> bounds;
    
        for (int i = 0; i < 3; i++){
            findersVec[i] = Vec3(finders[i].x, finders[i].y, 0);
        }   
        findersVec[3] = Vec3(finder4.x, finder4.y, 0);

        bounds[0] = findersVec[0] + ((findersVec[0] - findersVec[2]).Normalise() * finders[0].width * sqrt(2) * (MODULEOFFSET / 7));
        bounds[1] = findersVec[1] + ((findersVec[1] - findersVec[3]).Normalise() * finders[1].width * sqrt(2) * (MODULEOFFSET / 7));
        bounds[2] = findersVec[2] + ((findersVec[2] - findersVec[0]).Normalise() * finders[2].width * sqrt(2) * (MODULEOFFSET / 7));
        bounds[3] = findersVec[3] + ((findersVec[3] - findersVec[1]).Normalise() * finders[3].width * sqrt(2) * (MODULEOFFSET / 7));

        return bounds;
    }

    std::array<Vec3, 4> BoundingBoxRectangle(std::array<FinderCandidate, 3> finders, FinderCandidate finder4){
        const double MODULEOFFSET = 4;

        std::array<Vec3, 4> findersVec;
        std::array<Vec3, 4> bounds;
    
        for (int i = 0; i < 3; i++){
            findersVec[i] = Vec3(finders[i].x, finders[i].y, 0);
        }   
        findersVec[3] = Vec3(finder4.x, finder4.y, 0);

        bounds[0] = findersVec[0] + ((findersVec[0] - findersVec[1]).Normalise() + (findersVec[0] - findersVec[3]).Normalise()) * finders[0].width * (MODULEOFFSET / 7);
        bounds[1] = findersVec[1] + ((findersVec[1] - findersVec[2]).Normalise() + (findersVec[1] - findersVec[0]).Normalise()) * finders[1].width * (MODULEOFFSET / 7);
        bounds[2] = findersVec[2] + ((findersVec[2] - findersVec[3]).Normalise() + (findersVec[2] - findersVec[1]).Normalise()) * finders[2].width * (MODULEOFFSET / 7);
        bounds[3] = findersVec[3] + ((findersVec[3] - findersVec[0]).Normalise() + (findersVec[3] - findersVec[2]).Normalise()) * finder4.width * (MODULEOFFSET / 7);

        return bounds;
    }

    std::array<Vec3, 4> ExpansionBox(std::array<Vec3, 4> initialBounds, double expansionFactor){
        std::array<Vec3, 4> bounds;

        bounds[0] = initialBounds[0] + ((initialBounds[0] - initialBounds[1]) + (initialBounds[0] - initialBounds[3])) * expansionFactor;
        bounds[1] = initialBounds[1] + ((initialBounds[1] - initialBounds[2]) + (initialBounds[1] - initialBounds[0])) * expansionFactor;
        bounds[2] = initialBounds[2] + ((initialBounds[2] - initialBounds[3]) + (initialBounds[2] - initialBounds[1])) * expansionFactor;
        bounds[3] = initialBounds[3] + ((initialBounds[3] - initialBounds[0]) + (initialBounds[3] - initialBounds[2])) * expansionFactor;

        return bounds;
    }
}
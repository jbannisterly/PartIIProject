#pragma once

class Vec3{
    public:
    double x;
    double y;
    double z;

    Vec3() {}

    Vec3(const double inX, const double inY, const double inZ) :
    x{ inX },
    y{ inY },
    z{ inZ }  {
        
    }

    void Print();

    double Magnitude();
    Vec3 Normalise();
};


Vec3 operator-(Vec3 a, Vec3 b);
Vec3 operator*(Vec3 a, Vec3 b);
bool Clockwise(Vec3 a, Vec3 b, Vec3 c);
Vec3 operator*(Vec3 a, double scale);
Vec3 operator+(Vec3 a, Vec3 b);
Vec3 Intersection(Vec3 dirA, Vec3 dirB, Vec3 posA, Vec3 posB);

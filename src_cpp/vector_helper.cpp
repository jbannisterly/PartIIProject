#include "vector_helper.hpp"

Vec3 operator-(Vec3 a, Vec3 b){
    return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 operator*(Vec3 a, Vec3 b){
    return Vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

bool Clockwise(Vec3 a, Vec3 b, Vec3 c){
    Vec3 cross = (a - b) * (c - a);
    return cross.z < 0;
}
#include "vector_helper.hpp"
#include <cmath>
#include <iostream>

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

float Magnitude(Vec3 a){
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Vec3 Normalise(Vec3 a){
    Vec3 nomalised = a * (1 / Magnitude(a));
    return a * (1 / Magnitude(a));
}

Vec3 operator*(Vec3 a, float scale){
    return Vec3((a.x * scale), (a.y * scale), (a.z * scale));
}

Vec3 operator+(Vec3 a, Vec3 b){
    return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

void Vec3::Print(){
    std::cout << x << "," << y << "," << z << std::endl;
}
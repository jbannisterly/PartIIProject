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

double Magnitude(Vec3 a){
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Vec3 Normalise(Vec3 a){
    Vec3 normalised = a * (1 / Magnitude(a));
    return normalised;
}

Vec3 operator*(Vec3 a, double scale){
    return Vec3((a.x * scale), (a.y * scale), (a.z * scale));
}

Vec3 operator+(Vec3 a, Vec3 b){
    return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

void Vec3::Print(){
    std::cout << x << "," << y << "," << z << std::endl;
}

Vec3 Intersection(Vec3 dirA, Vec3 dirB, Vec3 posA, Vec3 posB){
    double mu = (dirA.x * (posA.y - posB.y) + dirA.y * (posB.x - posB.x)) / (dirA.x * dirB.y - dirA.y * dirB.x);   

    return posB + (dirB * mu);
}
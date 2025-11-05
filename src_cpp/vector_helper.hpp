class Vec3{
    public:
    float x;
    float y;
    float z;

    Vec3(float inX, float inY, float inZ){
        this->x = inX;
        this->y = inY;
        this->z = inZ;
    }

    void Print();
};

Vec3 operator-(Vec3 a, Vec3 b);
Vec3 operator*(Vec3 a, Vec3 b);
bool Clockwise(Vec3 a, Vec3 b, Vec3 c);
float Magnitude(Vec3 a);
Vec3 Normalise(Vec3 a);
Vec3 operator*(Vec3 a, float scale);
Vec3 operator+(Vec3 a, Vec3 b);
Vec3 Intersection(Vec3 dirA, Vec3 dirB, Vec3 posA, Vec3 posB);
class Vec3{
    public:
    double x;
    double y;
    double z;

    Vec3(const double inX, const double inY, const double inZ) :
    x{ inX },
    y{ inY },
    z{ inZ }  {
        
    }

    void Print();
};

// change to double

Vec3 operator-(Vec3 a, Vec3 b);
Vec3 operator*(Vec3 a, Vec3 b);
bool Clockwise(Vec3 a, Vec3 b, Vec3 c);
double Magnitude(Vec3 a); // put inside class
Vec3 Normalise(Vec3 a); //same 
Vec3 operator*(Vec3 a, double scale);
Vec3 operator+(Vec3 a, Vec3 b);
Vec3 Intersection(Vec3 dirA, Vec3 dirB, Vec3 posA, Vec3 posB);

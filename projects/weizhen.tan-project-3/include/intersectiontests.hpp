#include <math.hpp>

constexpr float cEpsilon = 1e-5f;

enum SideResult
{
    eINSIDE = -1,
    eOVERLAPPING = 0,
    eOUTSIDE = 1
};

bool CheckNaN(float _f);
bool CheckNaN(vec3 _v);

bool OverlapPointSphere(vec3 const& _p, vec3 const& _c, float _r);
bool OverlapSphereSphere(vec3 const& _c1, float _r1, vec3 const& _c2, float _r2);

bool OverlapPointAabb(vec3 const& _p, vec3 const& _min, vec3 const& _max);
bool OverlapAabbAabb(vec3 const& _min1, vec3 const& _max1, vec3 const& _min2, vec3 const& _max2);

float IntersectionTimeRayPlane(vec3 const& _s, vec3 const& _dir, vec3 const& _n, float _d);
std::tuple<float, vec3> IntersectionTimeRayTriangle(vec3 const& _s, vec3 const& _dir, vec3 const& _A, vec3 const& _B, vec3 const& _C);
float IntersectionTimeRaySphere(vec3 const& _s, vec3 const& _dir, vec3 const& _c, float _r);
float IntersectionTimeRayAabb(vec3 const& _s, vec3 const& _dir, vec3 const& _min, vec3 const& _max);

SideResult ClassifyPlanePoint(vec3 const& _n, float _d, vec3 const& _p);
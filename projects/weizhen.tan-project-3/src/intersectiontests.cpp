
#include <intersectiontests.hpp>

bool CheckNaN(float _f) { return _f != _f; }
bool CheckNaN(vec3 _v) { return (CheckNaN(_v.x) || CheckNaN(_v.y) || CheckNaN(_v.z)); }

bool OverlapPointSphere(vec3 const& _p, vec3 const& _c, float _r)
{
	if (CheckNaN(_p) || CheckNaN(_c) || CheckNaN(_r)) { return false; }
	return glm::distance2(_p, _c) <= _r * _r;
}
bool OverlapSphereSphere(vec3 const& _c1, float _r1, vec3 const& _c2, float _r2)
{
	if (CheckNaN(_c1) || CheckNaN(_r1) || CheckNaN(_c2) || CheckNaN(_r2)) { return false; }
	//return glm::distance2(_c1, _c2) <= (_r1 + _r2) * (_r1 + _r2);
	return OverlapPointSphere(_c2, _c1, _r1 + _r2);
}

bool OverlapPointAabb(vec3 const& _p, vec3 const& _min, vec3 const& _max)
{
	if (CheckNaN(_p) || CheckNaN(_min) || CheckNaN(_max)) { return false; }
	return !(_p.x < _min.x || _p.x > _max.x 
		  || _p.y < _min.y || _p.y > _max.y 
		  || _p.z < _min.z || _p.z > _max.z);
}

bool OverlapAabbAabb(vec3 const& _min1, vec3 const& _max1, vec3 const& _min2, vec3 const& _max2)
{
	if (CheckNaN(_min1) || CheckNaN(_max1) || CheckNaN(_min2) || CheckNaN(_max2)) { return false; }
	return OverlapPointAabb(_min1, _min2, _max2) || OverlapPointAabb(_max1, _min2, _max2) ||
		OverlapPointAabb(_min2, _min1, _max1) || OverlapPointAabb(_max2, _min1, _max1);
}

float IntersectionTimeRayPlane(vec3 const& _s, vec3 const& _dir, vec3 const& _n, float _d)
{
	float v = dot(_n, _dir);
	if (ClassifyPlanePoint(_n, _d, _s) == eOVERLAPPING || (v >= -cEpsilon && v <= cEpsilon)) { return -1; }
	float t = (_d - dot(_n, _s)) / v;
	return (t < 0) ? -1 : t;
}

std::tuple<float, vec3> IntersectionTimeRayTriangle(vec3 const& _s, vec3 const& _dir, vec3 const& _A, vec3 const& _B, vec3 const& _C)
{
	// edge vectors of tri
	vec3 ab = _B - _A;
	vec3 ac = _C - _A;
	vec3 ray_cross_e2 = cross(_dir, ac);
	float det = dot(ab, ray_cross_e2);

	if (det > -cEpsilon && det < cEpsilon) { return { -1, vec3(0) }; } // ray parallel to tri

	float inv_det = 1.f / det;
	vec3 s = _s - _A;

	float u = inv_det * dot(s, ray_cross_e2);
	if (u < 0 || u > 1) { return { -1, vec3(0) }; }

	vec3 s_cross_e1 = cross(s, ab);
	float v = inv_det * dot(_dir, s_cross_e1);
	if (v < 0 || u + v > 1) { return { -1, vec3(0) }; }

	float t = inv_det * dot(ac, s_cross_e1);
	if (t < 0) { return { -1, vec3(0) }; }

	return { t, vec3(u, v, 1.f - u - v) };
}

float IntersectionTimeRaySphere(vec3 const& _s, vec3 const& _dir, vec3 const& _c, float _r)
{
	vec3 L = _s - _c;
	float a = dot(_dir, _dir);
	float b = dot(L, _dir);
	float c = dot(L, L) - _r * _r;
	// ray’s origin outside s (c > 0) and ray pointing away from s (b > 0)
	if (c >= cEpsilon && b >= cEpsilon) { return -1; }
	float discr = b * b - c;
	// no intersection
	if (discr <= cEpsilon) { return -1; }
	// smallest +ve t value 
	float t1 = (-b + sqrt(discr)) / (2 * a);
	float t2 = (-b - sqrt(discr)) / (2 * a);
	if (t1 > 0.f && t2 > 0.f) 
	{
		return (t1 > t2) ? t2 : t1;
	}
	else // return +ve value
	{
		return (t1 > 0.0f) ? t1 : ((t2 > 0.0f) ? t2 : -1);
	}
	return -1;
}

float IntersectionTimeRayAabb(vec3 const& _s, vec3 const& _dir, vec3 const& _min, vec3 const& _max)
{
	// TODO: update to non branching code for optimization
	float tmin = -1.f; // set to -FLT_MAX to get first hit on line
	float tmax = FLT_MAX; // set to max distance ray can travel (for segment)
	// for all three slabs
	for (int i = 0; i < 3; i++)
	{
		if (abs(_dir[i]) < cEpsilon)
		{
			// origin not within slab
			if (_s[i] < _min[i] || _s[i] > _max[i]) { return -1; }
		}
		else
		{
			// compute intersection t value of ray with near and far plane of slab
			float invD = 1.0f / _dir[i];
			float t1 = (_min[i] - _s[i]) * invD;
			float t2 = (_max[i] - _s[i]) * invD;
			// t1 - near plane, t2 - far plane
			if (t1 > t2)
			{
				float tmp = t1;
				t1 = t2;
				t2 = tmp;
			}
			if (t1 > tmin) { tmin = t1; }
			if (t2 < tmax) { tmax = t2; }

		}
	}
	if (tmin > tmax) { return -1; }
	if (tmin < 0 && tmax > 0) { return 0; }
	return tmin;
}

SideResult ClassifyPlanePoint(vec3 const& _n, float _d, vec3 const& _p)
{
	if (CheckNaN(_n) || CheckNaN(_d) || CheckNaN(_p)) { return eOUTSIDE; }
	float dist = dot(normalize(_n), _p) - _d;
	return dist >= cEpsilon ? eOUTSIDE : dist <= -cEpsilon ? eINSIDE : eOVERLAPPING;
}

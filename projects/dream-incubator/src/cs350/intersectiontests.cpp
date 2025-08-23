/**
@file    intersectiontests.cpp
@author  weizhen.tan@digipen.edu
@date    27/05/2025

This file contains the definition for intersection test functions.

*//*__________________________________________________________________________*/

#include <cs350/intersectiontests.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */


bool OverlapSphereSphere(vec3 const& _c1, float _r1, vec3 const& _c2, float _r2)
{
	if (CheckNaN(_c1) || CheckNaN(_r1) || CheckNaN(_c2) || CheckNaN(_r2)) { return false; }
	//return distance2(_c1, _c2) <= (_r1 + _r2) * (_r1 + _r2);
	return OverlapPointSphere(_c2, _c1, _r1 + _r2);
}
bool OverlapSphereAabb(vec3 const& _c, float _r, vec3 const& _min, vec3 const& _max)
{
	float d2{ 0.f }; 
	for (int i = 0; i < 3; ++i) 
	{
		if (_c[i] < _min[i]) { d2 += (_min[i] - _c[i]) * (_min[i] - _c[i]	); }
		if (_c[i] > _max[i]) { d2 += (_c[i] - _max[i]) * (_c[i] - _max[i]); }
	} return d2 <= _r * _r;
}
bool OverlapAabbAabb(vec3 const& _min1, vec3 const& _max1, vec3 const& _min2, vec3 const& _max2)
{
	if (CheckNaN(_min1) || CheckNaN(_max1) || CheckNaN(_min2) || CheckNaN(_max2)) { return false; }
	if (_max1[0] < _min2[0] || _min1[0] > _max2[0]) { return false; }
	if (_max1[1] < _min2[1] || _min1[1] > _max2[1]) { return false; }
	if (_max1[2] < _min2[2] || _min1[2] > _max2[2]) { return false; }
	return true;
}

bool OverlapPointSphere(vec3 const& _p, vec3 const& _c, float _r)
{
	if (CheckNaN(_p) || CheckNaN(_c) || CheckNaN(_r)) { return false; }
	return distance2(_p, _c) <= _r * _r;
}
bool OverlapPointAabb(vec3 const& _p, vec3 const& _min, vec3 const& _max)
{
	if (CheckNaN(_p) || CheckNaN(_min) || CheckNaN(_max)) { return false; }
	return !(_p.x < _min.x || _p.x > _max.x 
		  || _p.y < _min.y || _p.y > _max.y 
		  || _p.z < _min.z || _p.z > _max.z);
}
bool OverlapPointTriangle(vec3 const& _p, vec3 const& _A, vec3 const& _B, vec3 const& _C)
{
	vec3 a =_A - _p; vec3 b = _B - _p; vec3 c = _C - _p;
	vec3 u = cross(b, c); vec3 v = cross(c, a);
	if (dot(u, v) < 0.0f) { return false; }
	vec3 w = cross(a, b);
	if (dot(u, w) < 0.0f) { return false; }
	return true;
}
SIDE_RESULT ClassifyPointPlane(vec3 const& _p, vec3 const& _n, float _d)
{
	if (CheckNaN(_n) || CheckNaN(_d) || CheckNaN(_p)) { return OUTSIDE; }
	float dist = dot(normalize(_n), _p) - _d;
	return dist >= cEpsilon ? OUTSIDE : dist <= -cEpsilon ? INSIDE : OVERLAPPING;
}


float IntersectionTimeRayPlane(vec3 const& _s, vec3 const& _dir, vec3 const& _n, float _d)
{
	float v = dot(_n, _dir);
	if (ClassifyPointPlane(_s, _n, _d) == OVERLAPPING || (v >= -cEpsilon && v <= cEpsilon)) { return -1; }
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
	if (OverlapPointSphere(_s, _c, _r)) { return -1; } // ray starts inside sphere
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
	if (t1 > 0.f && t2 > 0.f)  { return (t1 > t2) ? t2 : t1; }
	else // return +ve value
	{ return (t1 > 0.0f) ? t1 : ((t2 > 0.0f) ? t2 : -1); }
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

SIDE_RESULT ClassifyPlaneSphere(vec3 const& _n, float _d, vec3 const& _c, float _r)
{
	if (CheckNaN(_n) || CheckNaN(_d) || CheckNaN(_c) || CheckNaN(_r)) { return OUTSIDE; }
	float dist = dot(normalize(_n), _c) - _d;
	return dist > _r ? OUTSIDE : dist < -_r ? INSIDE : OVERLAPPING;
}

SIDE_RESULT ClassifyPlaneAabb(vec3 const& _n, float _d, vec3 const& _min, vec3 const& _max)
{
	if (CheckNaN(_n) || CheckNaN(_d) || CheckNaN(_min) || CheckNaN(_max)) { return OUTSIDE; }
	vec3 ctr = (_max + _min) * 0.5f;
	vec3 extents = _max - ctr;
	float radius = extents.x * abs(_n.x) + extents.y * abs(_n.y) + extents.z * abs(_n.z);
	float dist = dot(normalize(_n), ctr) - _d;
	return dist > radius ? OUTSIDE : dist < -radius ? INSIDE : OVERLAPPING;
}

SIDE_RESULT ClassifyPlaneObb(vec3 const& _n, float _d, vec3 const& _c, vec3 const& _e, vec3* const _u)
{
	if (CheckNaN(_n) || CheckNaN(_d) || CheckNaN(_c) || CheckNaN(_e)) { return OUTSIDE; }
	float p0 = dot(_n, *_u);
	float p1 = dot(_n, *(_u + 1));
	float p2 = dot(_n, *(_u + 2));
	float radius = (_e.x * abs(p0) + _e.y * abs(p1) + _e.z * abs(p2));
	float dist = dot(_n, _c) - _d;
	return dist > radius ? OUTSIDE : dist < -radius ? INSIDE : OVERLAPPING;
	/*vec3 n = transpose(mat3(*_u, *(_u + 1), *(_u + 2))) * _n;
	float s0 = n.x < 0 ? -1 : 1;
	float s1 = n.y < 0 ? -1 : 1;
	float s2 = n.z < 0 ? -1 : 1;
	vec3 nvtx = _c - (s0 * _e.x * (*_u))
		- (s1 * _e.y * (*(_u + 1)))
		- (s2 * _e.z * (*(_u + 2)));
	float dist = dot(_n, nvtx) - _d;
	if (dist > 0) { return OUTSIDE; }
	vec3 pvtx = _c + (s0 * _e.x * (*_u))
		+ (s1 * _e.y * (*(_u + 1)))
		+ (s2 * _e.z * (*(_u + 2)));
	dist = dot(_n, pvtx) - _d;
	if (dist < 0) { return INSIDE; }
	return OVERLAPPING;*/
}


bool CheckNaN(float _f) { return _f != _f; }
bool CheckNaN(vec3 _v) { return (CheckNaN(_v.x) || CheckNaN(_v.y) || CheckNaN(_v.z)); }
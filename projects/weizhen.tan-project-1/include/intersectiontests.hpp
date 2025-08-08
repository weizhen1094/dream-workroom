/**
@file    intersectiontests.hpp
@author  weizhen.tan@digipen.edu
@date    27/05/2025

This file contains the declaration for intersection test functions.

*//*__________________________________________________________________________*/

#ifndef INTERSECTION_TESTS_HPP
#define INTERSECTION_TESTS_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <math.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

constexpr float cEpsilon = 1e-5f;

enum SIDE_RESULT
{
    INSIDE = -1,
    OVERLAPPING = 0,
    OUTSIDE = 1
};

/**
 * @brief Checks Sphere vs Sphere intersection
 * @param _c1 - center of sphere 1
 * @param _r1 - radius of sphere 1
 * @param _c2 - center of sphere 2
 * @param _r2 - radius of sphere 2
 * @return true if spheres are intersecting
 */
bool OverlapSphereSphere(vec3 const& _c1, float _r1, vec3 const& _c2, float _r2);
/**
 * @brief Checks Sphere vs AABB intersection
 * @param _c - center of sphere 
 * @param _r - radius of sphere 
 * @param _min - min of AABB
 * @param _max - max of AABB
 * @return true if sphere and AABB are intersecting
 */
bool OverlapSphereAabb(vec3 const& _c, float _r, vec3 const& _min, vec3 const& _max);
/**
 * @brief Checks AABB vs AABB intersection
 * @param _min1 - min of AABB 1
 * @param _max1 - max of AABB 1
 * @param _min2 - min of AABB 2
 * @param _max2 - max of AABB 2
 * @return true if AABBs are intersecting
 */
bool OverlapAabbAabb(vec3 const& _min1, vec3 const& _max1, vec3 const& _min2, vec3 const& _max2);

/**
 * @brief Checks Point vs Sphere intersection
 * @param _p - position of point
 * @param _c - center of sphere
 * @param _r - radius of sphere
 * @return true if point is in sphere
 */
bool OverlapPointSphere(vec3 const& _p, vec3 const& _c, float _r);
/**
 * @brief Checks Point vs AABB intersection
 * @param _p - position of point
 * @param _min - min of AABB
 * @param _max - max of AABB
 * @return true if point is in AABB
 */
bool OverlapPointAabb(vec3 const& _p, vec3 const& _min, vec3 const& _max);
/**
 * @brief Checks Point vs Triangle intersection
 * @param _p - position of point
 * @param _A - v0 of triangle
 * @param _B - v1 of triangle
 * @param _C - v2 of triangle
 * @return true if point is in Triangle
 */
bool OverlapPointTriangle(vec3 const& _p, vec3 const& _A, vec3 const& _B, vec3 const& _C);
/**
 * @brief Checks Point vs Plane intersection
 * @param _p - position of point
 * @param _n - normalized normal of plane
 * @param _d - d of plane eqn
 * @return if point is inside/overlapping/outside Plane
 */
SIDE_RESULT ClassifyPointPlane(vec3 const& _p, vec3 const& _n, float _d);

/**
 * @brief Checks Ray vs Sphere intersection
 * @param _s - start position of ray
 * @param _dir - direction of ray
 * @param _c - center of sphere
 * @param _r - radius of sphere
 * @return exit time
 */
float IntersectionTimeRaySphere(vec3 const& _s, vec3 const& _dir, vec3 const& _c, float _r);
/**
 * @brief Checks Ray vs AABB intersection
 * @param _s - start position of ray
 * @param _dir - direction of ray
 * @param _min - min of AABB
 * @param _max - max of AABB
 * @return exit time
 */
float IntersectionTimeRayAabb(vec3 const& _s, vec3 const& _dir, vec3 const& _min, vec3 const& _max);
float IntersectionTimeRayPlane(vec3 const& _s, vec3 const& _dir, vec3 const& _n, float _d);
/**
 * @brief Checks Ray vs Triangle intersection
 * @param _s - start position of ray
 * @param _dir - direction of ray
 * @param _A - v0 of triangle
 * @param _B - v1 of triangle
 * @param _C - v2 of triangle
 * @return exit time and barycentric coordinates
 */
std::tuple<float, vec3> IntersectionTimeRayTriangle(vec3 const& _s, vec3 const& _dir, vec3 const& _A, vec3 const& _B, vec3 const& _C);

/**
 * @brief Checks Plane vs AABB intersection
 * @param _n - normalized normal of plane
 * @param _d - d of plane eqn
 * @param _min - min of AABB
 * @param _max - max of AABB
 * @return if AABB is inside/overlapping/outside Plane
 */
SIDE_RESULT ClassifyPlaneAabb(vec3 const& _n, float _d, vec3 const& _min, vec3 const& _max);
/**
 * @brief Checks Plane vs Sphere intersection
 * @param _n - normalized normal of plane
 * @param _d - d of plane eqn
 * @param _c - center of sphere 
 * @param _r - radius of sphere 
 * @return if Sphere is inside/overlapping/outside Plane
 */
SIDE_RESULT ClassifyPlaneSphere(vec3 const& _n, float _d, vec3 const& _c, float _r);

/**
 * Helper functions
 */

bool CheckNaN(float _f);
bool CheckNaN(vec3 _v);


#endif /* INTERSECTION_TESTS_HPP */
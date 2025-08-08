/**
@file    boundingvolume.hpp
@author  weizhen.tan@digipen.edu
@date    27/05/2025

This file contains the declaration for BoundingVolume, Aabb, Sphere and Obb structs,
bounding volume construction functions and helper functions to merge bounding volumes.

*//*__________________________________________________________________________*/

#ifndef BOUNDING_VOLUME_COMP_HPP
#define BOUNDING_VOLUME_COMP_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <map>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <math.hpp>
#include <intersectiontests.hpp> // check if point in sphere
#include <buffer.hpp>
#include <components/transform.hpp> // updateBV()
/*                                                                   includes
----------------------------------------------------------------------------- */

enum BV_TYPE : int
{
    AABB,
    BSPHERE_Ritters,
    BSPHERE_Larssons,
    BSPHERE_PCA,
    OBB_PCA,
    BV_TYPE_TOTAL,
};

static std::string to_string(BV_TYPE _type)
{
    switch (_type)
    {
    case AABB:		        return "AABB";
    case BSPHERE_Ritters:   return "BSphere Ritters";
    case BSPHERE_Larssons:  return "BSphere Larssons";
    case BSPHERE_PCA:		return "BSphere PCA";
    case OBB_PCA:		    return "OBB PCA";
    }
    return "";
}

// color for bv to show bv type
static const std::unordered_map<BV_TYPE, vec3> bvColors =
{
    {BV_TYPE::AABB,             vec3(1.f,0.f,0.f) },
    {BV_TYPE::BSPHERE_Ritters,  vec3(0.f,0.f,1.f) },
    {BV_TYPE::BSPHERE_Larssons, vec3(1.f,1.f,0.f) },
    {BV_TYPE::BSPHERE_PCA,      vec3(0.f,1.f,1.f) },
    {BV_TYPE::OBB_PCA,          vec3(0.f,1.f,0.f) },
};

enum VFC // view frustum culling classification
 {
    INSIDE,
    OUTSIDE,
    INTERSECTING,
};

// color for objects to show if classification with frustum
static const std::unordered_map<VFC, vec3> vfcColors =
{
    {VFC::INSIDE,       vec3(0.f,1.f,0.f) },
    {VFC::OUTSIDE,      vec3(0.f,0.f,1.f) },
    {VFC::INTERSECTING, vec3(1.f,1.f,0.f) },
};

struct BoundingVolume
{
    ///< shown in inspector
    BV_TYPE type    { AABB };
    bool isActive  { true };
    // not shown in inspector
    VFC vfc         { INSIDE };
    mat4 modelMat   { mat4(1.f) };
    int depth{0}; // for storing bvh lvl so render system knows what level this bv is on

    virtual void InitBV(std::string mdl) = 0; // set ctr and extends to bv in model coord
    virtual void UpdateBV(Transform&) = 0; // calc bv in world
     
    BoundingVolume(BV_TYPE _type = AABB, bool _isActive = true) 
    : type(_type), isActive(_isActive), vfc(INSIDE), modelMat(mat4(1.f)) {}

    static std::unordered_map<std::string, 
            std::vector<std::unique_ptr<BoundingVolume>>> s_BVs;
};

using BVList = std::vector<std::shared_ptr<BoundingVolume>>;

struct Sphere : public BoundingVolume
{
    vec3 center  { vec3(0) };
    float radius { 1.f };
    Sphere(BV_TYPE _type = BSPHERE_Ritters) 
        : BoundingVolume(_type, false), center(vec3(0)), radius(1.f) { }
    void UpdateBV(Transform& _xform) override
    {
        center = _xform.getMtx() * vec4(center,1);
        radius = radius * max(max(abs(_xform.scale.x), abs(_xform.scale.y)), abs(_xform.scale.z));
        modelMat = translate(mat4(1.0f), center) * scale(mat4(1.0f), vec3(radius));
    };
    void InitBV(std::string mdl) override
    {
        center = dynamic_cast<Sphere&>(*BoundingVolume::s_BVs[mdl][type]).center;
        radius = dynamic_cast<Sphere&>(*BoundingVolume::s_BVs[mdl][type]).radius;
        modelMat = BoundingVolume::s_BVs[mdl][type]->modelMat;
    }
};

struct Aabb : BoundingVolume
{
    vec3 center      { vec3(0) };
    vec3 halfExtents { vec3(1) };
    vec3 GetMin() { return center - halfExtents; };
    vec3 GetMax() { return center + halfExtents; };
    Aabb(BV_TYPE _type = AABB) : BoundingVolume(_type), center(vec3(0)), halfExtents(vec3(1)) { }
    void UpdateBV(Transform& _xform) override
    {
        vec3 min = GetMin(); vec3 max = GetMax();
        vec3 vtx[8] 
        {
            vec3(min.x, min.y, min.z),
            vec3(max.x, min.y, min.z),
            vec3(max.x, max.y, min.z),
            vec3(max.x, max.y, max.z),
            vec3(min.x, max.y, max.z),
            vec3(min.x, min.y, max.z),
            vec3(max.x, min.y, max.z),
            vec3(min.x, max.y, min.z),
        };
        for (unsigned i = 0; i < 8; ++i)
        { vtx[i] = _xform.getMtx() * vec4(vtx[i], 1); }
        vec3 minWorld = vec3(FLT_MAX); vec3 maxWorld = vec3(-FLT_MAX);
        for (unsigned i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                minWorld[j] = minWorld[j] > vtx[i][j] ? vtx[i][j] : minWorld[j];
                maxWorld[j] = maxWorld[j] < vtx[i][j] ? vtx[i][j] : maxWorld[j];
            }
        }
        center = (minWorld + maxWorld) * 0.5f;
        halfExtents = (maxWorld - minWorld) * 0.5f;
        modelMat = translate(mat4(1.0f), center) * scale(mat4(1.0f), halfExtents);
    };
    void InitBV(std::string mdl) override
    {
        center = dynamic_cast<Aabb&>(*BoundingVolume::s_BVs[mdl][type]).center;
        halfExtents = dynamic_cast<Aabb&>(*BoundingVolume::s_BVs[mdl][type]).halfExtents;
        modelMat = BoundingVolume::s_BVs[mdl][type]->modelMat;
    }
};

struct Obb : Aabb
{
    vec3 axes[3]     { vec3(1,0,0), vec3(0,1,0), vec3(0,0,1) };
    void UpdateBV(Transform& _xform) override
    {
        center = _xform.getMtx() * vec4(center, 1);
        halfExtents = _xform.getMtx() * vec4(halfExtents, 1);
        for (unsigned i = 0; i < 3; ++i)
        { axes[i] = normalize(_xform.getMtx() * vec4(axes[i], 1)); }
        modelMat = _xform.getMtx() * modelMat;
    };
    Obb() : Aabb(OBB_PCA) 
    { axes[0] = vec3(1, 0, 0);  axes[1] = vec3(0, 1, 0); axes[2] = vec3(0, 0, 1); }
    void InitBV(std::string mdl) override
    {
        axes[0] = dynamic_cast<Obb&>(*BoundingVolume::s_BVs[mdl][type]).axes[0];
        axes[1] = dynamic_cast<Obb&>(*BoundingVolume::s_BVs[mdl][type]).axes[1];
        axes[2] = dynamic_cast<Obb&>(*BoundingVolume::s_BVs[mdl][type]).axes[2];
        center = dynamic_cast<Aabb&>(*BoundingVolume::s_BVs[mdl][type]).center;
        halfExtents = dynamic_cast<Aabb&>(*BoundingVolume::s_BVs[mdl][type]).halfExtents;
        modelMat = BoundingVolume::s_BVs[mdl][type]->modelMat;
    }
};

static Aabb CreateAabbBruteForce(std::vector<Vertex> const& _vertices)
{
    Aabb ret;
    if (_vertices.empty()) { return ret; }

    vec3 min = vec3(FLT_MAX); vec3 max = vec3(-FLT_MAX);
	for (size_t i = 0; i < _vertices.size(); ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			min[j] = min[j] > _vertices[i].position[j] ? _vertices[i].position[j] : min[j];
			max[j] = max[j] < _vertices[i].position[j] ? _vertices[i].position[j] : max[j];
		}
	}

    ret.center = (min + max) * 0.5f;
    ret.halfExtents = (max - min) * 0.5f;
    
    mat4 trans = translate(mat4(1.0f), ret.center);
    mat4 sc = scale(mat4(1.0f), ret.halfExtents);
    ret.modelMat = trans * sc;

    return ret;
}

static Sphere CreateSphereRitters(std::vector<Vertex> const& _vertices)
{
    Sphere ret; ret.type = BSPHERE_Ritters;
    if (_vertices.empty()) { return ret; }

	// 1st pass - most separate point in each axis
	vec3 min[3] = { vec3(FLT_MAX), vec3(FLT_MAX), vec3(FLT_MAX) };
	vec3 max[3] = { vec3(-FLT_MAX), vec3(-FLT_MAX), vec3(-FLT_MAX) };
	for (size_t i = 0; i < _vertices.size(); ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			min[j] = min[j][j] > _vertices[i].position[j] ? _vertices[i].position : min[j];
			max[j] = max[j][j] < _vertices[i].position[j] ? _vertices[i].position : max[j];
		}
	}

	// furthest pair of point
	int idx{ 0 }; float dist = 0;
	for (int i = 0; i < 3; ++i)
	{
		if (dist < distance2(min[i], max[i]))
		{ idx = i; dist = distance2(min[i], max[i]); }
	}

	// initial sphere
    ret.center = (min[idx] + max[idx]) * 0.5f;
    ret.radius = sqrt(dist) * 0.5f;

	// expand
	for (int i = 0; i < _vertices.size(); ++i)
	{
		if (!OverlapPointSphere(_vertices[i].position, ret.center, ret.radius))
		{
			vec3 b = ret.center - ret.radius * normalize(_vertices[i].position - ret.center);
			// regenerate sphere
            ret.center = (_vertices[i].position + b) * 0.5f;
            ret.radius = distance(ret.center, b);
		}
	}

    mat4 trans = translate(mat4(1.0f), ret.center);
    mat4 sc = scale(mat4(1.0f), vec3(ret.radius));
    ret.modelMat = trans * sc;

    return ret;
}

static Sphere CreateSphereLarssons(std::vector<Vertex> const& _vertices)
{
    Sphere ret; ret.type = BSPHERE_Larssons;
    if (_vertices.empty()) { return ret; }

    // EPOS-6 direction dictionary: ±X, ±Y, ±Z
    std::vector<vec3> directionDictionary = 
    {
    {1, 0, 0}, {-1, 0, 0},
    {0, 1, 0}, {0, -1, 0},
    {0, 0, 1}, {0, 0, -1}
    };
    // Step 1: Find extremal points
    std::vector<vec3> extremalPoints;
    for (const auto& dir : directionDictionary) 
    {
        float maxProj = -std::numeric_limits<float>::infinity();
        float minProj = std::numeric_limits<float>::infinity();
        vec3 pMax, pMin;
        for (const auto& v : _vertices) 
        {
            float proj = dot(v.position, dir);
            if (proj > maxProj) 
            {
                maxProj = proj;
                pMax = v.position;
            }
            if (proj < minProj) 
            {
                minProj = proj;
                pMin = v.position;
            }
        }
        extremalPoints.push_back(pMax);
        extremalPoints.push_back(pMin);
    }
    // Step 2: Find farthest pair among extremal points
    float maxDistance = 0.0f;
    vec3 p1, p2;
    for (size_t i = 0; i < extremalPoints.size(); ++i) 
    {
        for (size_t j = i + 1; j < extremalPoints.size(); ++j) 
        {
            float dist = distance2(extremalPoints[i], extremalPoints[j]);
            if (dist > maxDistance) {
                maxDistance = dist;
                p1 = extremalPoints[i];
                p2 = extremalPoints[j];
            }
        }
    }
    // Step 3: Initial sphere from farthest pair
    ret.center = (p1 + p2) * 0.5f;
    ret.radius = sqrt(maxDistance) * 0.5f;
    // Step 4: Grow to include all points
    for (const auto& v : _vertices) {
        float dist = distance(ret.center, v.position);
        if (dist > ret.radius)
        {
            float newRadius = (ret.radius + dist) * 0.5f;
            vec3 direction = normalize(v.position - ret.center);
            ret.center += (newRadius - ret.radius) * direction;
            ret.radius = newRadius;
        }
    }

    mat4 trans = translate(mat4(1.0f), ret.center);
    mat4 sc = scale(mat4(1.0f), vec3(ret.radius));
    ret.modelMat = trans * sc;

    return ret;

}

static Sphere CreateSpherePCA(std::vector<Vertex> const& _vertices)
{
    Sphere ret; ret.type = BSPHERE_PCA;
    if (_vertices.empty()) { return ret; }
    // Step 1: Convert vertices to a matrix for computation
    Eigen::MatrixXf points(_vertices.size(), 3);
    for (size_t i = 0; i < _vertices.size(); ++i)
    { points.row(i) << _vertices[i].position.x, _vertices[i].position.y, _vertices[i].position.z; }
    // Step 2: Compute the centroid (mean position)
    Eigen::Vector3f centroid = points.colwise().mean();
    // Step 3: Center the data by subtracting the centroid // shift points s.t. centroid is at origin
    Eigen::MatrixXf centered = points.rowwise() - centroid.transpose();
    // Step 4: Compute the covariance matrix
    Eigen::Matrix3f covariance = (centered.adjoint() * centered) / static_cast<float>(_vertices.size());
    // Step 5: Compute eigenvalues and eigenvectors of the covariance matrix
    //    Solve for eigenvalues and eigenvectors of covariance using a solver
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> solver(covariance);
    if (solver.info() != Eigen::ComputationInfo::Success) { return ret; }
    Eigen::MatrixXf eigenVectors = solver.eigenvectors(); // each col is eigenVector
    // Step 6: Radius Estimation
    // Project points onto principal axes
    Eigen::MatrixXf rotated = centered * eigenVectors;
    // Measure max distance from centroid
    Eigen::Vector3f minExtents = rotated.colwise().minCoeff();
    Eigen::Vector3f maxExtents = rotated.colwise().maxCoeff();
    ret.radius = (maxExtents - minExtents).norm() * 0.5f;
    Eigen::Vector3f centerOffset = eigenVectors * (minExtents + (maxExtents - minExtents) * 0.5f);
    Eigen::Vector3f center = centroid + centerOffset;
    ret.center =  vec3(center[0],center[1], center[2]);

    mat4 trans = translate(mat4(1.0f), ret.center);
    mat4 sc = scale(mat4(1.0f), vec3(ret.radius));
    ret.modelMat = trans * sc;

    return ret;
}

static std::tuple <std::vector<vec3>,Obb> CreateObbPCA(std::vector<Vertex> const& _vertices)
{
    Obb obb; obb.type = OBB_PCA;
    std::vector<vec3> vtx;
    if (_vertices.empty())  { return { vtx, obb }; }

    // Step 1: Convert vertices to a matrix for computation
    Eigen::MatrixXf points(_vertices.size(), 3);
    for (size_t i = 0; i < _vertices.size(); ++i)
    { points.row(i) << _vertices[i].position.x, _vertices[i].position.y, _vertices[i].position.z; }
    // Step 2: Compute the centroid (mean position)
    Eigen::Vector3f centroid = points.colwise().mean();
    // Step 3: Center the data by subtracting the centroid // shift points s.t. centroid is at origin
    Eigen::MatrixXf centered = points.rowwise() - centroid.transpose();
    // Step 4: Compute the covariance matrix
    Eigen::Matrix3f covariance = (centered.adjoint() * centered) / static_cast<float>(_vertices.size());
    // Step 5: Compute eigenvalues and eigenvectors of the covariance matrix
    //    Solve for eigenvalues and eigenvectors of covariance using a solver
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> solver(covariance);

    if (solver.info() != Eigen::ComputationInfo::Success) { return { vtx,obb }; }
    
    Eigen::Matrix3f eigenVectors = solver.eigenvectors(); // each col is eigenVector

    // Step 6: Set OBB axes (principal directions)
    for (int i = 0; i < 3; ++i)
    {
        vec3 axis { eigenVectors(0, i), eigenVectors(1, i), eigenVectors(2, i) };
        obb.axes[i] = normalize(axis);
    }
    // Step 7: Rotate points into the PCA-aligned frame
    Eigen::MatrixXf rotated = centered * eigenVectors;
    // Step 8: Compute extents in the PCA-aligned space
    Eigen::Vector3f minExtents = rotated.colwise().minCoeff();
    Eigen::Vector3f maxExtents = rotated.colwise().maxCoeff();
    Eigen::Vector3f halfExts = (maxExtents - minExtents) * 0.5f;
    // Step 9: Set half-extents
    obb.halfExtents = vec3(halfExts(0), halfExts(1), halfExts(2));
    // Step 10: Compute OBB center in world space
    Eigen::Vector3f centerOffset = eigenVectors * (minExtents + halfExts);
    Eigen::Vector3f obbCenter = centroid + centerOffset;
    obb.center = vec3(obbCenter[0], obbCenter[1], obbCenter[2]);

    // compute OBB vertices in world space
    vtx =
    {
        obb.center - obb.axes[0] * obb.halfExtents[0] + obb.axes[1] * obb.halfExtents[1] - obb.axes[2] * obb.halfExtents[2],
        obb.center - obb.axes[0] * obb.halfExtents[0] - obb.axes[1] * obb.halfExtents[1] - obb.axes[2] * obb.halfExtents[2],
        obb.center + obb.axes[0] * obb.halfExtents[0] - obb.axes[1] * obb.halfExtents[1] - obb.axes[2] * obb.halfExtents[2],
        obb.center + obb.axes[0] * obb.halfExtents[0] + obb.axes[1] * obb.halfExtents[1] - obb.axes[2] * obb.halfExtents[2],
        obb.center - obb.axes[0] * obb.halfExtents[0] + obb.axes[1] * obb.halfExtents[1] + obb.axes[2] * obb.halfExtents[2],
        obb.center - obb.axes[0] * obb.halfExtents[0] - obb.axes[1] * obb.halfExtents[1] + obb.axes[2] * obb.halfExtents[2],
        obb.center + obb.axes[0] * obb.halfExtents[0] - obb.axes[1] * obb.halfExtents[1] + obb.axes[2] * obb.halfExtents[2],
        obb.center + obb.axes[0] * obb.halfExtents[0] + obb.axes[1] * obb.halfExtents[1] + obb.axes[2] * obb.halfExtents[2],
    };

    return { vtx, obb };
}

static Aabb MergeAabbs(std::vector<std::shared_ptr<Aabb>> const& _aabbs)
{
    Aabb ret;
    if (_aabbs.empty()) { return ret; }

    vec3 min = _aabbs[0]->GetMin(); vec3 max = _aabbs[0]->GetMax();
    for (size_t i = 0; i < _aabbs.size(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            min[j] = min[j] > _aabbs[i]->GetMin()[j] ? _aabbs[i]->GetMin()[j] : min[j];
            max[j] = max[j] < _aabbs[i]->GetMax()[j] ? _aabbs[i]->GetMax()[j] : max[j];
        }
    }

    ret.center = (min + max) * 0.5f;
    ret.halfExtents = (max - min) * 0.5f;

    mat4 trans = translate(mat4(1.0f), ret.center);
    mat4 sc = scale(mat4(1.0f), ret.halfExtents);
    ret.modelMat = trans * sc;

    return ret;
}
static Sphere MergeTwoSpheres(Sphere const& _sphere, Sphere const& _sphere1)
{
    Sphere ret = _sphere;
    // check if sphere is enclosing
    vec3 d = _sphere.center - _sphere1.center;
    float dist2 = dot(d, d);
    float r = _sphere.radius - _sphere1.radius;
    if (r * r >= dist2) 
    {
        if (_sphere.radius >= _sphere1.radius) { return _sphere; }
        else { return _sphere1; }
    }
    else // partially overlapping or disjoint
    {
        float dist = sqrt(dist2);
        ret.radius = (dist + _sphere.radius + _sphere1.radius) * 0.5f;
        ret.center = _sphere.center;
        if (dist > cEpsilon)
        {  ret.center = (_sphere.center + _sphere1.center + r * d / dist) * 0.5f; }
    }
    return ret;
}

static void DebugSphere(Sphere const& _s)
{
    std::cout << "sphere ctr:" << to_string(_s.center) << std::endl;
    std::cout << "sphere r:" << _s.radius << std::endl;
}
static Sphere MergeSpheres(std::vector<std::shared_ptr<Sphere>> _spheres)
{
    Sphere ret;
    if (_spheres.empty()) { return ret; }
    ret = *_spheres[0]; 
    while (_spheres.size() > 1)
    {
        ret = MergeTwoSpheres(*_spheres.front(), *_spheres.back());
        _spheres.erase(_spheres.begin());
        _spheres.erase(_spheres.end()-1);
        _spheres.push_back(std::make_shared<Sphere>(ret));
    } 
    ret.modelMat = translate(mat4(1.0f), ret.center) * scale(mat4(1.0f), vec3(ret.radius));
    return ret;
}

enum CARDINAL_AXES
{
    X, Y, Z
};

static CARDINAL_AXES GetAxisWithLargestSpread(std::vector<vec3> _ctrs)
{
    Eigen::MatrixXf points(_ctrs.size(), 3);
    for (size_t i = 0; i < _ctrs.size(); ++i)
    { points.row(i) << _ctrs[i].x, _ctrs[i].y, _ctrs[i].z; }
    Eigen::Vector3f centroid = points.colwise().mean();
    Eigen::Vector3f sd2 = points.row(0) - centroid.transpose();
    for (size_t i = 1; i < _ctrs.size(); ++i)
    { sd2 += (points.row(i) - centroid.transpose()); }
    sd2 = sd2 / _ctrs.size(); // variance
    return sd2[0] > sd2[1] ? (sd2[0] > sd2[2] ? X : Z) : (sd2[1] > sd2[2] ? Y : Z);
};

#endif /* BOUNDING_VOLUME_COMP_HPP */
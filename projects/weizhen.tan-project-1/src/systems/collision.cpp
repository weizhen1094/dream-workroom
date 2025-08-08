/**
@file    collision.cpp
@author  weizhen.tan@digipen.edu
@date    15/07/2025

This file contains the definition of the Collision system class.

*//*__________________________________________________________________________*/

#include <systems/collision.hpp>
#include <intersectiontests.hpp>
#include <ecs.hpp>
#include <components/transform.hpp>
#include <components/renderable.hpp>
#include <components/boundingvolume.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

void Collision::Init() 
{
}

void Collision::Update() 
{
    auto viewMesh = ECS.registry().view<Transform, Renderable>();
    viewMesh.each([](auto _ent, Transform& _xform, Renderable& _mesh)
    {
        // check if mesh is primitive
        std::string const& mesh = _mesh.GetMeshType();
        if (mesh.find(".obj") == std::string::npos)
        {
            ECS.registry().view<Transform, Renderable>().each([&_ent, &_xform, &mesh](auto _ent1, Transform& _xform1, Renderable& _mesh1)
            {
                    std::string const& mesh1 = _mesh1.GetMeshType();
                    if (mesh1.find(".obj") == std::string::npos)
                    {
                        if (_ent != _ent1)
                        {
                            if (mesh == "Sphere")
                            {
                                Sphere s; s.UpdateBV(_xform);
                                if (mesh1 == "Sphere")
                                {
                                    Sphere s1; s1.UpdateBV(_xform1);
                                    _xform.hasCollided = _xform1.hasCollided = OverlapSphereSphere(s.center, s.radius, s1.center, s1.radius);
                                }
                                if (mesh1 == "AABB")
                                {
                                    Aabb aabb; aabb.UpdateBV(_xform1);
                                    _xform.hasCollided = _xform1.hasCollided = OverlapSphereAabb(s.center, s.radius, aabb.GetMin(), aabb.GetMax());
                                }
                                if (mesh1 == "Point3D")
                                {  _xform.hasCollided = _xform1.hasCollided = OverlapPointSphere(_xform1.position, s.center, s.radius); }
                                if (mesh1 == "Ray")
                                {
                                    vec3 right = vec3(_xform1.getMtx()[0][0], _xform1.getMtx()[0][1], _xform1.getMtx()[0][2]);
                                    _xform.hasCollided = _xform1.hasCollided = IntersectionTimeRaySphere(_xform1.position, right, s.center, s.radius) > 0;
                                }
                                if (mesh1 == "Plane")
                                {
                                    vec3 forward = normalize(vec3(_xform1.getMtx()[2][0], _xform1.getMtx()[2][1], _xform1.getMtx()[2][2]));
                                    _xform.hasCollided = _xform1.hasCollided = ClassifyPlaneSphere(forward, dot(_xform1.position, forward), s.center, s.radius) == SIDE_RESULT::OVERLAPPING;
                                }
                            }
                            if (mesh == "AABB")
                            {
                                Aabb aabb; aabb.UpdateBV(_xform);
                                if (mesh1 == "AABB")
                                {
                                    Aabb aabb1; aabb1.UpdateBV(_xform1);
                                    _xform.hasCollided = _xform1.hasCollided = OverlapAabbAabb(aabb.GetMin(), aabb.GetMax(), aabb1.GetMin(), aabb1.GetMax());
                                }
                                if (mesh1 == "Point3D")
                                {
                                    _xform.hasCollided = _xform1.hasCollided = OverlapPointAabb(_xform1.position, aabb.GetMin(), aabb.GetMax());
                                }
                                if (mesh1 == "Ray")
                                {
                                    vec3 right = vec3(_xform1.getMtx()[0][0], _xform1.getMtx()[0][1], _xform1.getMtx()[0][2]);
                                    _xform.hasCollided = _xform1.hasCollided = IntersectionTimeRayAabb(_xform1.position, right, aabb.GetMin(), aabb.GetMax()) > 0;
                                }
                                if (mesh1 == "Plane")
                                {
                                    vec3 forward = normalize(vec3(_xform1.getMtx()[2][0], _xform1.getMtx()[2][1], _xform1.getMtx()[2][2]));
                                    _xform.hasCollided = _xform1.hasCollided = ClassifyPlaneAabb(forward, dot(_xform1.position, forward), aabb.GetMin(), aabb.GetMax()) == SIDE_RESULT::OVERLAPPING;
                                }
                            }
                            if (mesh == "Point3D")
                            {
                                if (mesh1 == "Plane")
                                {
                                    vec3 forward = vec3(_xform1.getMtx()[2][0], _xform1.getMtx()[2][1], _xform1.getMtx()[2][2]);
                                    _xform.hasCollided = _xform1.hasCollided = ClassifyPointPlane(_xform.position, forward, dot(_xform1.position, forward)) == SIDE_RESULT::OVERLAPPING;
                                }
                                if (mesh1 == "Triangle")
                                {
                                    vec3 a = _xform1.getMtx() * vec4( -0.5f, -0.5f, 0.0f,1.f);
                                    vec3 b = _xform1.getMtx() * vec4( 0.5f, -0.5f, 0.0f, 1.f);
                                    vec3 c = _xform1.getMtx() * vec4( 0.0f, 0.5f, 0.0f, 1.f);
                                    _xform.hasCollided = _xform1.hasCollided = OverlapPointTriangle(_xform.position, a, b, c);
                                }
                            }
                            if (mesh == "Ray")
                            {
                                vec3 right = vec3(_xform.getMtx()[0][0], _xform.getMtx()[0][1], _xform.getMtx()[0][2]);
                                if (mesh1 == "Plane")
                                {
                                    vec3 forward = normalize(vec3(_xform1.getMtx()[2][0], _xform1.getMtx()[2][1], _xform1.getMtx()[2][2]));
                                    _xform.hasCollided = _xform1.hasCollided = IntersectionTimeRayPlane(_xform.position, right, forward, dot(_xform1.position, forward)) > 0;
                                }
                                if (mesh1 == "Triangle")
                                {
                                    vec3 a = _xform1.getMtx() * vec4(-0.5f, -0.5f, 0.0f, 1.f);
                                    vec3 b = _xform1.getMtx() * vec4(0.5f, -0.5f, 0.0f, 1.f);
                                    vec3 c = _xform1.getMtx() * vec4(0.0f, 0.5f, 0.0f, 1.f);
                                    auto[t, barycentric] = IntersectionTimeRayTriangle(_xform.position, right, a, b, c);
                                    _xform.hasCollided = _xform1.hasCollided = t > 0;
                                }
                            }
                        }
                    }
            });
        }
    });
}

void Collision::CleanUp() 
{
}

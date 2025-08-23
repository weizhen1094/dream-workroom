/**
@file    collision.cpp
@author  weizhen.tan@digipen.edu
@date    15/07/2025

This file contains the definition of the Collision system class.

*//*__________________________________________________________________________*/

#include <systems/collision.hpp>
#include <cs350/intersectiontests.hpp>
#include <ecs.hpp>
#include <components/transform.hpp>
#include <components/renderable.hpp>
#include <components/boundingvolume.hpp>
#include <components/camera.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

void Collision::Init() 
{
}

void Collision::Update() 
{
    
    // view frustum culling
    auto viewCam = ECS.registry().view<EntityName, Transform, Camera>();
    viewCam.each([](EntityName& _name, Transform& _xformCam, Camera& _cam)
    {

    bool shouldUpdate = _cam.isFocused || _xformCam.isDirty;
    ECS.registry().view<Transform>().each([&shouldUpdate](Transform& _xform)
        {if (_xform.isDirty) { shouldUpdate = true; }});

    if (/*shouldUpdate &&*/ _name.value.find("Main") != std::string::npos)
    {
    auto viewBV = ECS.registry().view<EntityName, Transform, BVList>();
    viewBV.each([&_cam, &_xformCam](auto _ent, EntityName& _name, Transform& _xform, BVList& _bvList)
    {
            mat4 camMtx = _cam.projMtx;
            vec4 r0 = vec4(camMtx[0][0], camMtx[1][0], camMtx[2][0], camMtx[3][0]);
            vec4 r1 = vec4(camMtx[0][1], camMtx[1][1], camMtx[2][1], camMtx[3][1]);
            vec4 r2 = vec4(camMtx[0][2], camMtx[1][2], camMtx[2][2], camMtx[3][2]);
            vec4 r3 = vec4(camMtx[0][3], camMtx[1][3], camMtx[2][3], camMtx[3][3]);
            vec4 frustumPlanes[6];
            frustumPlanes[0] = -r0 - r3; frustumPlanes[1] = r0 - r3; // l r
            frustumPlanes[2] = -r1 - r3; frustumPlanes[3] = r1 - r3; // b t
            frustumPlanes[4] = -r2 - r3; frustumPlanes[5] = -r2 + r3; // n f
            
            SIDE_RESULT vfc = INSIDE;
            for (unsigned i = 0; i < 6; ++i)
            {
                mat4 invViewMtx = inverse(_cam.viewMtx);
                vec3 n = invViewMtx * frustumPlanes[i];
                n = normalize(n);
                float d = frustumPlanes[i].w / length(n) + dot(n, vec3(invViewMtx[3]));

                for (auto& bv : _bvList) // test each bv against the 6 frustums
                {
                    if (bv == nullptr || !bv->isActive) { continue; }
                
                    switch (bv->type)
                    {
                    case AABB: 
                    {
                        SIDE_RESULT currPlane = ClassifyPlaneAabb(n, d,
                                std::dynamic_pointer_cast<Aabb>(bv)->GetMin(), std::dynamic_pointer_cast<Aabb>(bv)->GetMax());
                        if (currPlane == OUTSIDE) { vfc = OUTSIDE; break; }
                        else if (currPlane == OVERLAPPING) { vfc = OVERLAPPING; }
                    } break;
                    case BSPHERE_Ritters:
                    case BSPHERE_Larssons:
                    case BSPHERE_PCA:
                    {
                        SIDE_RESULT currPlane = ClassifyPlaneSphere(n, d,
                            std::dynamic_pointer_cast<Sphere>(bv)->center, std::dynamic_pointer_cast<Sphere>(bv)->radius);
                        if (currPlane == OUTSIDE) { vfc = OUTSIDE; break; }
                        else if (currPlane == OVERLAPPING) { vfc = OVERLAPPING; }
                    } break;
                    case OBB_PCA:
                        SIDE_RESULT currPlane = ClassifyPlaneObb(n, d,
                            std::dynamic_pointer_cast<Obb>(bv)->center, std::dynamic_pointer_cast<Obb>(bv)->halfExtents, std::dynamic_pointer_cast<Obb>(bv)->axes);
                        if (currPlane == OUTSIDE) { vfc = OUTSIDE; break; }
                        else if (currPlane == OVERLAPPING) { vfc = OVERLAPPING; }
                    break;
                    };
                }
                if (vfc == OUTSIDE)
                {  
                    for (auto& bv : _bvList) { bv->vfc = OUTSIDE; }
                    break; // one of the bv is outside skip the other tests and set all to outside
                }
                else if (vfc == OVERLAPPING)
                { for (auto& bv : _bvList) { bv->vfc = OVERLAPPING; } }
                else { for (auto& bv : _bvList) { bv->vfc = vfc; } }
            }
    });
    }
    });
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

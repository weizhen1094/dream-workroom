/**
@file    render.cpp
@author  weizhen.tan@digipen.edu
@date    25/05/2025

This file contains the declaration of the Render system class.

*//*__________________________________________________________________________*/

#include <systems/render.hpp>
#include <ecs.hpp>
#include <shader.hpp>
#include <components/camera.hpp>
#include <components/light.hpp>
#include <components/transform.hpp>
#include <components/renderable.hpp>
#include <components/material.hpp>
#include <components/boundingvolume.hpp>
#include <bvhierarchy.hpp>
#include <filesystem>
/*                                                                   includes
----------------------------------------------------------------------------- */
std::unordered_map<std::string, std::vector<std::unique_ptr<Buffer>>> Renderable::s_Buffers;
uint32_t  Renderable::s_ProgramID;
std::unordered_map<std::string,
    std::vector<std::unique_ptr<BoundingVolume>>> BoundingVolume::s_BVs;

void Render::Init()
{
    // load shader
    if (Renderable::GetShaderPgm() == NULL)
    {
        Renderable::GetShaderPgm() = LoadShaders(
            "../projects/weizhen.tan-project-4/shaders/ColorVertexShader.vert", 
            "../projects/weizhen.tan-project-4/shaders/ColorFragmentShader.frag");
    }
    // create meshes
    auto [name, vtx, idx, primitiveType]    = GeneratePlaneGeometry(vec4(1.0f));
    Renderable::GetBuffers()[name].push_back(std::make_unique<Buffer>(vtx, primitiveType, true, &idx));
    std::tie(name, vtx, primitiveType)      = GeneratePoint3DGeometry(vec4(1.0f));
    Renderable::GetBuffers()[name].push_back(std::make_unique<Buffer>(vtx, primitiveType));
    std::tie(name, vtx, primitiveType)      = GenerateTriangleGeometry(vec4(1.0f));
    Renderable::GetBuffers()[name].push_back(std::make_unique<Buffer>(vtx, primitiveType));
    std::tie(name, vtx, idx, primitiveType) = GenerateSphereGeometry(vec4(1.0f));
    Renderable::GetBuffers()[name].push_back(std::make_unique<Buffer>(vtx, primitiveType, true, &idx));
    std::tie(name, vtx, idx, primitiveType) = GenerateAABBGeometry(vec4(1.0f));
    Renderable::GetBuffers()[name].push_back(std::make_unique<Buffer>(vtx, primitiveType, true, &idx));
    std::tie(name, vtx, primitiveType)      = GenerateRayGeometry(vec4(1.0f));
    Renderable::GetBuffers()[name].push_back(std::make_unique<Buffer>(vtx, primitiveType));
    auto [nameAABB_8vtx, vtxAABB_8vtx, idxAABB_8vtx, primitiveTypeAABB_8vtx] = GenerateAABB_8vtxGeometry(vec4(1.0f));
    Renderable::GetBuffers()[nameAABB_8vtx].push_back(std::make_unique<Buffer>(vtxAABB_8vtx, primitiveTypeAABB_8vtx, true, &idxAABB_8vtx));
        // TODO: create class to load objs to encapsulate assimp // ignore .mtl
        Assimp::Importer importer;
        // load all .txt in "../models/UNC Power Plant Data Files (Bhargav)/" and append .obj listed in file to objPath
        std::string folderPath = "../models/UNC Power Plant Data Files (Bhargav)/";
        std::vector<std::string> txtFilePath =  {   "4a.txt", "4b.txt",
                                                    "5a.txt", "5b.txt", "5c.txt",
                                                    "6a.txt", "6b.txt"  };
        bool isPowerPlant = true;
        std::vector<std::string> objPath = isPowerPlant ? GetPowerPlantPath(folderPath) : 
            std::vector<std::string>{
             "../models/4Sphere.obj",
            "../models/bunny.obj",
            // "../models/bunny_high_poly.obj",
            // "../models/cube.obj",
            //  "../models/cube2.obj",
             "../models/cup.obj",
             "../models/head.obj",
             "../models/lucy_princeton.obj",
             "../models/ogre.obj",
             "../models/quad.obj",
             "../models/rhino.obj",
             "../models/sphere.obj",
             // "../models/sphere_modified.obj",
              "../models/starwars1.obj",
              //"../models/triangle.obj",
        };
        if (isPowerPlant)
        {
            for (std::string& path : txtFilePath)
            {
                std::vector<std::string> tmp = GetPowerPlantPath(folderPath + path);
                objPath.insert(objPath.end(), tmp.begin(), tmp.end());
            }
        }
        for (std::string& path : objPath)
        {
            const aiScene* scene = importer.ReadFile(isPowerPlant ? folderPath + path : path, aiProcessPreset_TargetRealtime_Fast);
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {  std::cout << "assimp error: " << importer.GetErrorString() << std::endl;  return; }
            if (isPowerPlant) { scene->mRootNode->mName = path; } 
            ProcessNode(scene->mRootNode, scene); // load meshes recursively
            std::cout << "assimp loaded: " << scene->mRootNode->mName.C_Str() << std::endl;
        }

    // calculate bv for loaded objs TODO: objs dynamically added to proj dir during runtime
    for (auto& [name, buf] : Renderable::GetBuffers())
    {
        if (name.find(".obj") != std::string::npos && name.find("OBB") == std::string::npos)
        {
            // generate all types of BV for this model
            std::vector<Vertex> tmp;
            for (auto& mesh : buf)  {  tmp.insert(tmp.end(), mesh->GetVertices().begin(), mesh->GetVertices().end()); }
            BoundingVolume::s_BVs[name].push_back(std::make_unique<Aabb>(CreateAabbBruteForce(tmp)));
            BoundingVolume::s_BVs[name].push_back(std::make_unique<Sphere>(CreateSphereRitters(tmp)));
            BoundingVolume::s_BVs[name].push_back(std::make_unique<Sphere>(CreateSphereLarssons(tmp)));
            BoundingVolume::s_BVs[name].push_back(std::make_unique<Sphere>(CreateSpherePCA(tmp)));
            // generate unique OBB mesh for each model
            std::vector<Vertex> vtxOBB_8vtx = vtxAABB_8vtx;
            std::string nameOBB = "OBB " + name;
            auto [v, obb] = CreateObbPCA(tmp);
            unsigned i = 0; for (auto& vtx : vtxOBB_8vtx) { vtx.position = v[i]; ++i; }
            Renderable::GetBuffers()[nameOBB].push_back(std::make_unique<Buffer>(vtxOBB_8vtx, primitiveTypeAABB_8vtx, true, &idxAABB_8vtx));
            BoundingVolume::s_BVs[name].push_back(std::make_unique<Obb>(obb));
        }
    }
}

void Render::Update()
{
    Buffer::ClearBuffers();

    auto viewCamera = ECS.registry().view<Transform, Camera, Framebuffer>();
    viewCamera.each([](auto _ent, Transform& _xform, Camera& _cam, Framebuffer& _fb)
    {
        // bind fbo
        _fb.BindFBO();


        // viewports
        Buffer::SetViewport(_fb.GetFramebufferPosition(), _fb.GetFramebufferSize());
        
        Buffer::ClearBuffers();


        UseShader(Renderable::GetShaderPgm());

            // pass light to shader
            auto viewLight = ECS.registry().view<Transform, Light>();
            viewLight.each([](Transform& _xform, Light& _light)
            {
                SetUniform(Renderable::GetShaderPgm(), "light.position", _xform.position);
                SetUniform(Renderable::GetShaderPgm(), "light.type", static_cast<int>(_light.type));
                SetUniform(Renderable::GetShaderPgm(), "light.color", _light.color);
                vec3 right = vec3(_xform.getMtx()[0][0], _xform.getMtx()[0][1], _xform.getMtx()[0][2]);
                SetUniform(Renderable::GetShaderPgm(), "light.direction", right);
                SetUniform(Renderable::GetShaderPgm(), "light.intensity", _light.intensity);
               // SetUniform(Renderable::GetShaderPgm(), "light.range", _light.range); // TODO: for spot/point
               // SetUniform(Renderable::GetShaderPgm(), "light.angle", _light.angle); // for spot
            });

            // pass view and proj mtx to shaders
            _cam.UpdateCamera(_xform.getMtx(), _fb.GetFramebufferSize());
            SetUniform(Renderable::GetShaderPgm(), "viewMtx", _cam.viewMtx);
            SetUniform(Renderable::GetShaderPgm(), "projMtx", _cam.projMtx);

            // draw wireframe frustum of other cameras
            ECS.registry().view<Camera, Transform>().each([&_ent](auto _ent1, Camera& _cam2, Transform& _xform2)
            {
                    if (_ent != _ent1)
                    {
                        Renderable::GetBuffers()["AABB_8vtx"][0]->BindVAO();
                        SetUniform(Renderable::GetShaderPgm(), "material.kAmbient", vec3(1.f));
                        SetUniform(Renderable::GetShaderPgm(), "material.kDiffuse", vec3(1.f));
                        SetUniform(Renderable::GetShaderPgm(), "material.kSpecular", vec3(1.f));
                        SetUniform(Renderable::GetShaderPgm(), "vertexTransform", _xform2.getMtx() * inverse(_cam2.projMtx) * inverse(_cam2.viewMtx) * inverse(_xform2.getMtx()));
                        Renderable::GetBuffers()["AABB_8vtx"][0]->Draw();
                        Renderable::GetBuffers()["AABB_8vtx"][0]->UnBindVAO();
                    }
            });

        UnUseShader();

        // render bv
        auto viewBV = ECS.registry().view</*RenderOrder, */Transform, BVList>();
        viewBV.each([](auto _ent,/*RenderOrder& _renderOrder,*/ Transform& _xform, BVList& _bvArr)
            {
                for (auto bv : _bvArr)
                {
                    if (bv == nullptr || !bv->isActive) { continue; }

                    std::string mdl; bool hasMdl = ECS.registry().view<Renderable>().contains(_ent);
                    if (hasMdl) 
                    { 
                        mdl = ECS.registry().get<Renderable>(_ent).GetMeshType(); 
                        ECS.registry().get<Material>(_ent).kAmbient = vfcColors.at(bv->vfc);
                        ECS.registry().get<Material>(_ent).kDiffuse = vfcColors.at(bv->vfc);
                    }

                    std::string mesh;
                    switch (bv->type)
                    {
                    case AABB:                  mesh = "AABB_8vtx";     break;
                    case BSPHERE_Ritters:
                    case BSPHERE_Larssons:
                    case BSPHERE_PCA:           mesh = "Sphere";        break;
                    case OBB_PCA: if (hasMdl) { mesh = "OBB " + mdl; }  break;
                    };
                    UseShader(Renderable::GetShaderPgm());
                    Buffer::SetPolygonMode(Buffer::POLYGON_MODE::LINE);
                    Renderable::GetBuffers()[mesh][0]->BindVAO();
                    if (hasMdl) // color bv based on type
                    {
                        SetUniform(Renderable::GetShaderPgm(), "material.kAmbient", bvColors.at(bv->type));
                        SetUniform(Renderable::GetShaderPgm(), "material.kDiffuse", bvColors.at(bv->type));
                    }
                    else // color bv based on bvh tree level
                    {
                        SetUniform(Renderable::GetShaderPgm(), "material.kAmbient", GetColors()[bv->depth]);
                        SetUniform(Renderable::GetShaderPgm(), "material.kDiffuse", GetColors()[bv->depth]);
                    }
                    SetUniform(Renderable::GetShaderPgm(), "material.kSpecular", vec3(1));
                    SetUniform(Renderable::GetShaderPgm(), "material.shininess", 1.f);
                    SetUniform(Renderable::GetShaderPgm(), "vertexTransform", bv->modelMat);
                    Renderable::GetBuffers()[mesh][0]->Draw();
                    Renderable::GetBuffers()[mesh][0]->UnBindVAO();
                    Buffer::SetPolygonMode(Buffer::POLYGON_MODE::FILL);
                    UnUseShader();
                }
            });

        // render mesh 
        auto viewMesh = ECS.registry().view</*RenderOrder, */Transform, Renderable, Material>();
        viewMesh.each([](auto _ent,/*RenderOrder& _renderOrder,*/ Transform& _xform, Renderable& _model, Material& _mat)
            {
                if (!_model.GetIsHidden()) 
                {
                if (_model.GetIsWireframe()) { Buffer::SetPolygonMode(Buffer::POLYGON_MODE::LINE); }

                //if (_model.GetMeshType() == "Point3D") { Buffer::SetPointSize(length2(_xform.scale)); }
                std::string mdl = _model.GetMeshType();
                if (_model.GetMeshType() == "Point3D") { _xform.scale = vec3(0.1f); mdl = "Sphere"; }

                mat4 modelMat = _xform.getMtx();
                
                UseShader(Renderable::GetShaderPgm());
                    for (auto& mesh : Renderable::GetBuffers()[mdl])
                    {
                        mesh->BindVAO();
                        SetUniform(Renderable::GetShaderPgm(), "material.kAmbient",
                            _xform.hasCollided ? vec3(1.f) - _mat.kAmbient : _mat.kAmbient);
                        SetUniform(Renderable::GetShaderPgm(), "material.kDiffuse", 
                            _xform.hasCollided ? vec3(1.f) - _mat.kDiffuse : _mat.kDiffuse);
                        SetUniform(Renderable::GetShaderPgm(), "material.kSpecular", _mat.kSpecular);
                        SetUniform(Renderable::GetShaderPgm(), "material.shininess", _mat.shininess);
                        SetUniform(Renderable::GetShaderPgm(), "vertexTransform", modelMat);
                        mesh->Draw();
                        mesh->UnBindVAO();
                    }
                UnUseShader();

                if (_model.GetIsWireframe()) { Buffer::SetPolygonMode(Buffer::POLYGON_MODE::FILL); }
                }
            });




        // unbind fbo
        _fb.UnBindFBO();
    });
}

void Render::CleanUp()
{
    DeleteShader(Renderable::GetShaderPgm());
}
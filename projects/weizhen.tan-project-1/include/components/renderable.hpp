/**
@file    renderable.hpp
@author  weizhen.tan@digipen.edu
@date    08/05/2025

This file contains the declaration of the Renderable class and helper functions
for generating core primitive geometries and loading models using assimp.

*//*__________________________________________________________________________*/

#ifndef RENDERABLE_HPP
#define RENDERABLE_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <memory>
#include <buffer.hpp>
    #include <assimp/Importer.hpp> // TODO: remove after load obj class 
    #include <assimp/scene.h>          
    #include <assimp/postprocess.h> 
/*                                                                   includes
----------------------------------------------------------------------------- */

/**
 * @class Renderable
 * @brief This class is responsible for storing the needed data for rendering
 * a geometry.
 */
class Renderable 
{
public:

    /**
     * @brief ctor, initialize the renderable object (shaders, buffers, etc.).
     * @param _meshType - mesh type to be rendered
     */
    Renderable(std::string _meshType = "AABB") : m_IsHidden(false), m_IsWireframe(false), m_MeshType(_meshType) { };
    ~Renderable() { };

    void SetMeshType(std::string _meshType) { m_MeshType = _meshType; }
    std::string GetMeshType() { return m_MeshType; }
    bool& GetIsWireframe() { return m_IsWireframe; }
    bool& GetIsHidden() { return m_IsHidden; }

    static std::unordered_map<std::string, std::vector<std::unique_ptr<Buffer>>>& GetBuffers() { return s_Buffers; }
    static uint32_t& GetShaderPgm() { return s_ProgramID; }

private:

    bool m_IsHidden; ///< should mesh render 
    bool m_IsWireframe; ///< should mesh render in wireframe
    std::string m_MeshType; ///< if mesh is from .obj file
    static uint32_t s_ProgramID; ///< ID of the compiled shader program
    static std::unordered_map<std::string, std::vector<std::unique_ptr<Buffer>>> s_Buffers; ///< buffer objects of different meshes
};



/**
  * Helper functions to generate geometries
  */

static std::tuple<std::string, std::vector<Vertex>, Buffer::PRIMITIVE_TYPE> 
GeneratePoint3DGeometry(vec4 _clr)
{  std::vector<Vertex> vtx = { {vec3(0),  vec3(0), _clr, vec2(0)} };  return { "Point3D", vtx, Buffer::POINTS}; }

static std::tuple<std::string, std::vector<Vertex>, std::vector<uint32_t>, Buffer::PRIMITIVE_TYPE>
GeneratePlaneGeometry(vec4 _clr)
{
    std::vector<Vertex> vtx = {
   {{-0.5f, -0.5f, 0.0f },  {0.0f, 1.0f, 0.0f}, _clr, {0.0f, 0.0f}},
   {{ 0.5f, -0.5f, 0.0f },  {0.0f, 1.0f, 0.0f}, _clr, {1.0f, 0.0f}},
   {{ 0.5f, 0.5f,  0.0f },   {0.0f, 1.0f, 0.0f}, _clr, {1.0f, 1.0f}},
   {{-0.5f, 0.5f,  0.0f },   {0.0f, 1.0f, 0.0f}, _clr, {0.0f, 1.0f}}
    };
    std::vector<uint32_t> idx = { 0, 1, 2, 2, 3, 0 };
    return { "Plane", vtx, idx, Buffer::TRIANGLES};
}

static std::tuple<std::string, std::vector<Vertex>, Buffer::PRIMITIVE_TYPE>
GenerateTriangleGeometry(vec4 _clr)
{
    std::vector<Vertex> vtx = {
   {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, _clr, {0.0f, 0.0f}},
   {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, _clr, {1.0f, 0.0f}},
   {{ 0.0f, 0.5f, 0.0f},  {0.0f, 1.0f, 0.0f}, _clr, {1.0f, 1.0f}}
    }; return { "Triangle", vtx, Buffer::TRIANGLES};
}

static std::tuple<std::string, std::vector<Vertex>, std::vector<uint32_t>, Buffer::PRIMITIVE_TYPE>
GenerateSphereGeometry(vec4 _clr)
{
    std::vector<Vertex> vtx; std::vector<uint32_t> idx;
    unsigned numStack = 18; unsigned numSlice = 36;

    float degreePerStack = 180.f / numStack;
    float degreePerSlice = 360.f / numSlice;

    for (unsigned stack = 0; stack < numStack + 1; ++stack)
    {
        float degToRad = static_cast<float>(std::numbers::pi) / 180.f;
        float phi =  (-90.f + stack * degreePerStack) * degToRad;
        for (unsigned slice = 0; slice <= numSlice; ++slice)
        {
            float theta = slice * degreePerSlice * degToRad;
            float x = cos(phi) * cos(theta);
            float y = sin(phi);
            float z = cos(phi) * sin(theta);
            vtx.push_back({ vec3(x,y,z), vec3(x,y,z), _clr, vec2(0) });
        }
    }
    for (unsigned stack = 0; stack < numStack; ++stack)
    {
        for (unsigned slice = 0; slice <= numSlice; ++slice)
        {
            idx.push_back((numSlice + 1) * stack + slice);
            idx.push_back((numSlice + 1) * (stack + 1) + slice);
        }
    }

    return { "Sphere", vtx, idx, Buffer::TRIANGLE_STRIP};
}

static std::tuple<std::string, std::vector<Vertex>, std::vector<uint32_t>, Buffer::PRIMITIVE_TYPE>
GenerateAABBGeometry(vec4 _clr)
{
    std::vector<Vertex> vtx =
    {
        // front
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, _clr, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}, _clr, {1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}, _clr, {1.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f},   {0.0f, 0.0f, 1.0f}, _clr, {0.0f, 1.0f}},
       
        // back
        {{-0.5f, -0.5f, 0.5f},  {0.0f, 0.0f, -1.0f}, _clr, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f},   {0.0f, 0.0f, -1.0f}, _clr, {1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.5f},   {0.0f, 0.0f, -1.0f}, _clr, {1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f},    {0.0f, 0.0f, -1.0f}, _clr, {0.0f, 1.0f}},

        // left
        {{-0.5f, -0.5f, 0.5f},  {-1.0f, 0.0f, 0.0f}, _clr, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f},   {-1.0f, 0.0f, 0.0f}, _clr, {1.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, _clr, {1.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f},  {-1.0f, 0.0f, 0.0f}, _clr, {0.0f, 1.0f}},

        // right
        {{0.5f, -0.5f, 0.5f},   {1.0f, 0.0f, 0.0f}, _clr, {0.0f, 0.0f}},
        {{0.5f, 0.5f, 0.5f},    {1.0f, 0.0f, 0.0f}, _clr, {1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}, _clr, {1.0f, 1.0f}},
        {{0.5f, 0.5f, -0.5f},   {1.0f, 0.0f, 0.0f}, _clr, {0.0f, 1.0f}},

        // top
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, _clr, {0.0f, 0.0f}},
        {{-0.5f, 0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}, _clr, {1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}, _clr, {1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f},   {0.0f, 1.0f, 0.0f}, _clr, {0.0f, 1.0f}},

        // btm
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, _clr, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.5f},  {0.0f, -1.0f, 0.0f}, _clr, {1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f},  {0.0f, -1.0f, 0.0f}, _clr, {1.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f},   {0.0f, -1.0f, 0.0f}, _clr, {0.0f, 1.0f}},

    };

    std::vector<uint32_t> idx
    {
        0, 3, 2, 1, 3, 0,       // front
        6, 7, 4, 4, 7, 5,       // back
        8, 11, 10, 9, 11, 8,    // left
        14, 15, 12, 12, 15, 13, // right
        16, 19, 18, 17, 19, 16, // top
        22, 23, 20, 20, 23, 21, // btm
    };

    return { "AABB", vtx, idx, Buffer::TRIANGLES};
}

static std::tuple<std::string, std::vector<Vertex>, Buffer::PRIMITIVE_TYPE>
GenerateRayGeometry(vec4 _clr)
{
    std::vector<Vertex> vtx = 
    { {vec3(0,0,0),  vec3(0), _clr, vec2(0)}, 
      {vec3(100000,0,0),  vec3(0), _clr, vec2(0)} };
    return { "Ray", vtx, Buffer::LINES};
}

static std::tuple<std::string, std::vector<Vertex>, std::vector<uint32_t>, Buffer::PRIMITIVE_TYPE>
GenerateAABB_8vtxGeometry(vec4 _clr)
{
    std::vector<Vertex> vtx =
    {
        {{ -1.0f, 1.0f, -1.0f },  vec3(0), _clr, vec2(0)},
        {{ -1.0f, -1.0f, -1.0f },   vec3(0), _clr, vec2(0)},

        {{ 1.0f, -1.0f, -1.0f },    vec3(0), _clr, vec2(0)},
        {{ 1.0f, 1.0f, -1.0f },  vec3(0), _clr, vec2(0)},

        {{ -1.0f, 1.0f, 1.0f },  vec3(0), _clr, vec2(0)},
        {{ -1.0f, -1.0f, 1.0f },  vec3(0), _clr, vec2(0)},

        {{ 1.0f, -1.0f, 1.0f },   vec3(0), _clr, vec2(0)},
        {{ 1.0f, 1.0f, 1.0f }, vec3(0), _clr, vec2(0)},
    };
    std::vector<uint32_t> idx
    {
        0, 1, 2, 3,
        0, 4, 5, 1,
        2, 6, 7, 4,
        5, 6, 7, 3
    };
    return { "AABB_8vtx", vtx, idx, Buffer::LINE_LOOP };
}

/**
  * Helper functions to load meshes 
  */

static void ProcessNode(aiNode* _node, aiScene const* _scene)
{
    for (unsigned i = 0; i < _node->mNumMeshes; ++i)
    {
        aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
        std::vector<Vertex> vtx;
        for (int i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex v;
            v.position.x = mesh->mVertices[i].x;
            v.position.y = mesh->mVertices[i].y;
            v.position.z = mesh->mVertices[i].z;
            if (mesh->HasNormals())
            {
                v.normal.x = mesh->mNormals[i].x;
                v.normal.y = mesh->mNormals[i].y;
                v.normal.z = mesh->mNormals[i].z;
            }
            else { v.normal = vec3(0.f); }
            v.color = vec4(1.f);
            if (mesh->mTextureCoords[0])
            {
                v.uv.x = mesh->mTextureCoords[0][i].x;
                v.uv.y = mesh->mTextureCoords[0][i].y;
            }
            else { v.uv = vec2(0.f); }
            vtx.push_back(v);
        }
        std::vector<uint32_t> idx;
        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (int j = 0; j < face.mNumIndices; ++j)
            {
                idx.push_back(face.mIndices[j]);
            }
        }
        Renderable::GetBuffers()[_scene->mRootNode->mName.C_Str()].push_back(std::make_unique<Buffer>(vtx, static_cast<Buffer::PRIMITIVE_TYPE>(Buffer::PRIMITIVE_TYPE::TRIANGLES/*mesh->mPrimitiveTypes*/), true, &idx));
    }

    for (unsigned i = 0; i < _node->mNumChildren; ++i) 
    { ProcessNode(_node->mChildren[i], _scene); }
}



#endif /* RENDERABLE_HPP */
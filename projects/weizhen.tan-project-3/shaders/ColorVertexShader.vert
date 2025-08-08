#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 vertexTransform;

out vec3 pos;
out vec3 nml;

uniform mat4 viewMtx;
uniform mat4 projMtx;

void main()
{
    mat4 mdlViewMtx = viewMtx * vertexTransform; 

    mat3 nmlMtx = mat3(vec3(mdlViewMtx[0]), vec3(mdlViewMtx[1]), vec3(mdlViewMtx[2])); 
    nml = normalize(nmlMtx * aNormal);

    vec4 posInView = mdlViewMtx * vec4(aPosition, 1.0f);
    pos = posInView.xyz;
    gl_Position = projMtx * posInView; 
}

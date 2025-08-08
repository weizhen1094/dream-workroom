/**
@file    shader.hpp

This file contains the declaration of helper functions to load, compile, link, 
use and set uniforms for shaders.

*//*__________________________________________________________________________*/
#pragma once

#include <GL/glew.h>
#include <math.hpp>

/**
 * @brief Loads and compiles vertex and fragment shaders from given file paths, links them into a shader program.
 *
 * @param vertex_file_path Path to the vertex shader source file.
 * @param fragment_file_path Path to the fragment shader source file.
 * @return GLuint ID of the created shader program.
 */
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);

/**
 * @brief Loads separable vertex and fragment shaders and sets up a program pipeline.
 *
 * @param vertex_file_path Path to the vertex shader source file.
 * @param fragment_file_path Path to the fragment shader source file.
 * @param programIDs Output array to store vertex and fragment shader program IDs (size must be 2).
 * @return GLuint ID of the OpenGL pipeline object.
 */
GLuint LoadPipeline(const char* vertex_file_path, const char* fragment_file_path, GLuint* programIDs);

void UseShader(GLuint _shaderPgm);
void UnUseShader();
void DeleteShader(GLuint _shaderPgm);

void SetUniform(GLuint _shaderPgm, std::string const& _name, int const _val);
void SetUniform(GLuint _shaderPgm, std::string const& _name, float const _val);
void SetUniform(GLuint _shaderPgm, std::string const& _name, float const _x, float const _y);
void SetUniform(GLuint _shaderPgm, std::string const& _name, float const _x, float const _y, float const _z);
void SetUniform(GLuint _shaderPgm, std::string const& _name, float const _x, float const _y, float const _z, float const _w);
void SetUniform(GLuint _shaderPgm, std::string const& _name, vec2 const& _val);
void SetUniform(GLuint _shaderPgm, std::string const& _name, vec3 const& _val);
void SetUniform(GLuint _shaderPgm, std::string const& _name, vec4 const& _val);
void SetUniform(GLuint _shaderPgm, std::string const& _name, mat3 const& _val);
void SetUniform(GLuint _shaderPgm, std::string const& _name, mat4 const& _val);

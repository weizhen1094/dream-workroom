/**
@file    buffer.hpp
@author  weizhen.tan@digipen.edu
@date    08/05/2025

This file contains the Buffer and FramebufferObject class that encapsulates 
all OpenGL logic.

*//*__________________________________________________________________________*/

#ifndef BUFFER_HPP
#define BUFFER_HPP
/*                                                                      guard
----------------------------------------------------------------------------- */

#include <GL/glew.h> 
#include <iostream> // for debug output to console
#include <vector>
#include <math.hpp>
/*                                                                   includes
----------------------------------------------------------------------------- */

#define OPENGL_4_5_ABOVE // enable for glCreateBuffers, else glGen 

/**
 * @struct Vertex
 * @brief This struct holds the data for a vertex. pos, nml, clr, uv
 */
struct Vertex
{
	vec3 position;
	vec3 normal;
	vec4 color;
	vec2 uv;
};

/**
 * @class Buffer
 * @brief This class is responsible for creation and binding of Vertex Buffer Object(VBO),
 * Vertex Array Object(VAO), attribute layout specification, Element Buffer Object(EBO),
 * cleanup of buffer resources.
 */
class Buffer 
{

public:

	enum PRIMITIVE_TYPE
	{
		TRIANGLES		= GL_TRIANGLES,
		TRIANGLE_STRIP	= GL_TRIANGLE_STRIP,
		TRIANGLE_FAN	= GL_TRIANGLE_FAN,
		LINES			= GL_LINES,
		LINE_STRIP		= GL_LINE_STRIP,
		LINE_LOOP		= GL_LINE_LOOP,
		POINTS			= GL_POINTS,
		PRIMITIVE_TYPE_TOTAL,
	};

	enum POLYGON_MODE
	{
		FILL  = GL_FILL,
		LINE  = GL_LINE,
		POINT = GL_POINT,
		POLYGON_MODE_TOTAL,
	};

	// TODO: add enum wrappers for faces and blending? 

	/**
	 * OpenGL Callbacks
	 */

	static void ClearBuffers() // TODO:? do these really belong here..
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Set background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear frame and depth
		glEnable(GL_DEPTH_TEST);
	}
	static void SetPolygonMode(POLYGON_MODE _mode)  { glPolygonMode(GL_FRONT_AND_BACK, _mode); }
	static void SetPointSize(float _size)			{ glPointSize(_size); }
	static void SetViewport(vec2 _pos, vec2 _size)				
	{
		glViewport(static_cast<int>(_pos.x), static_cast<int>(_pos.y),
			static_cast<int>(_size.x), static_cast<int>(_size.y));
	}
	static void CheckOpenGLError()
	{
		GLenum err = glGetError();
		while (err != GL_NO_ERROR)
		{
			std::string errType;
			switch (err)
			{
				case GL_INVALID_ENUM:		errType = "GL_INVALID_ENUM"; break;
				case GL_INVALID_VALUE:		errType = "GL_INVALID_VALUE"; break;
				case GL_INVALID_OPERATION:	errType = "GL_INVALID_OPERATION"; break;
				case GL_INVALID_FRAMEBUFFER_OPERATION: errType = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
				case GL_OUT_OF_MEMORY:		errType = "GL_OUT_OF_MEMORY"; break;
			}
			std::cout << "[OpenGL Error: " + errType + "] " << glewGetErrorString(err) << std::endl;
		}
	}

	void BindVAO()	 { glBindVertexArray(m_VAO); }
	void Draw()	
	{ m_IsIndexedDrawing ? glDrawElements(m_PrimitiveType, m_DrawCount, GL_UNSIGNED_INT, nullptr) : glDrawArrays(m_PrimitiveType, 0, m_DrawCount); };
	void UnBindVAO() { glBindVertexArray(0); }

	std::vector<Vertex>& GetVertices() { return m_Vertices; };

	/**
	 * @brief ctor, sets up buffer
	 * @param _vtx - vertex data
	 * @param _primitiveType - primitive type
	 * @param _isIndexedDrawing - has indices, if yes provide index data
	 * @param _idx - pointer to index data
	 */
	Buffer(std::vector<Vertex> const& _vtx, PRIMITIVE_TYPE _primitiveType,
		bool _isIndexedDrawing = false, std::vector<uint32_t>* _idx = nullptr) // setup buffer
		: m_Vertices(_vtx), m_VBO(), m_VAO(), m_EBO(), m_PrimitiveType(_primitiveType),
		m_DrawCount(static_cast<uint32_t>(_isIndexedDrawing ? _idx->size() : _vtx.size())), m_IsIndexedDrawing(_isIndexedDrawing)
	{

		// create VAO VBO EBO
#ifdef OPENGL_4_5_ABOVE 
		glCreateVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glCreateBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, _vtx.size() * sizeof(Vertex), _vtx.data(), GL_STATIC_DRAW);

		if (_isIndexedDrawing) 
		{
			glCreateBuffers(1, &m_EBO);
			glNamedBufferStorage(m_EBO, sizeof(uint32_t) * _idx->size(), reinterpret_cast<GLvoid*>(_idx->data()), GL_DYNAMIC_STORAGE_BIT);
			glVertexArrayElementBuffer(m_VAO, m_EBO);
		}
#else
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, _vtx.size() * sizeof(Vertex), _vtx.data(), GL_STATIC_DRAW);

		if (_isIndexedDrawing)
		{
			glGenBuffers(1, &m_EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _idx->size() * sizeof(uint32_t), reinterpret_cast<GLvoid*>(_idx->data()), GL_STATIC_DRAW);
		}
#endif
		// attribute layout
		
			// pos
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			// nml
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec3)));

			// clr
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec3) * 2));

			// tex coord
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec3) * 2 + (sizeof(vec4))));

		UnBindVAO();
	}
	/**
	 * @brief dtor, cleans up resources
	 */
	~Buffer()
	{
		glDeleteBuffers(1, &m_EBO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteVertexArrays(1, &m_VAO);
	}

private:

	std::vector<Vertex> m_Vertices;
	uint32_t m_VBO;			///< handle to vbo
	uint32_t m_VAO;			///< handle to vao
	uint32_t m_EBO;			///< handle to ebo
	PRIMITIVE_TYPE m_PrimitiveType;	///< opengl primitive type
	uint32_t m_DrawCount;		///< the number of elements in the per-vertex data buffers

	bool m_IsIndexedDrawing;
};

/**
 * @class Framebuffer
 * @brief This class is responsible for creation and binding of Framebuffer Object(FBO).
 */
class Framebuffer
{

public:

	void BindFBO() { glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); }
	void UnBindFBO() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	void CreateFBO(vec2 _size)
	{
		m_FBOSize = _size;
		if (m_FBO != NULL)		{ glDeleteFramebuffers(1, &m_FBO); }
		if (m_Texture != NULL)  { glDeleteTextures(1, &m_Texture); }

		// create and bind fbo
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		// create texture
		glGenTextures(1, &m_Texture);
		glBindTexture(GL_TEXTURE_2D, m_Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<int>(_size.x), static_cast<int>(_size.y), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		// attach tex to fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture, 0);

		// create rbo
		uint32_t rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, static_cast<int>(_size.x), static_cast<int>(_size.y));
		// attach depth buffer to  fbo
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		// set draw buffer
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);

		// unbind and check if complete
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		GLenum err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (err != GL_FRAMEBUFFER_COMPLETE)
		{ throw std::runtime_error(std::string("[OpenGL Framebuffer Error: " + std::to_string(err) + "\n")); }
	}

	uint32_t const& GetFramebufferHdl() const { return m_FBO; };
	uint32_t const& GetFramebufferTexture() const { return m_Texture; };
	vec2 GetFramebufferPosition() { return m_FBOPos; };
	vec2 GetFramebufferSize()	  { return m_FBOSize; };

	Framebuffer() : m_FBO(0), m_Texture(0), m_FBOPos(vec2(0)), m_FBOSize(vec2(0)) {};
	~Framebuffer() {};

private:

	uint32_t m_FBO;		 ///< handle to fbo
	uint32_t m_Texture;  ///< handle framebuffer texture

	vec2 m_FBOPos;
	vec2 m_FBOSize;

};

#endif /* BUFFER_HPP */
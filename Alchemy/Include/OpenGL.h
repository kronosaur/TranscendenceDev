#pragma once

#include "OpenGLIncludes.h"
#include "OpenGLShader.h"
#include <vector>

class OpenGLTexture {
public:
	OpenGLTexture(void) { }
	OpenGLTexture(void* texture, int width, int height);
	~OpenGLTexture(void);
	void initTexture2D(void* texture, int width, int height);
	void bindTexture2D(GLenum glTexture) { glActiveTexture(glTexture); glBindTexture(GL_TEXTURE_2D, m_pTextureID[0]); }
	void unbindTexture2D(void) { glBindTexture(GL_TEXTURE_2D, 0); }
	void updateTexture2D(void* texture, int width, int height);
	unsigned int* getTexture(void) { return m_pTextureID; }

private:
	unsigned int m_pTextureID[1];
	unsigned int pboID[2];
};

class OpenGLVAO {
public:
	OpenGLVAO(void) { };
	OpenGLVAO(std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos);
	OpenGLVAO(std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos,
		std::vector<std::vector<float>> texcoords);
	~OpenGLVAO(void);
	void initVAO(std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos);
	void initVAO(std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos,
		std::vector<std::vector<float>> texcoords);
	void setShader(Shader* shader) { m_pShader = shader; }
	void addTexture2D(void* texture);
	void removeTexture();
	Shader* getShader(void) { return m_pShader; }
	unsigned int* getVAO(void) { return vaoID; }

private:
	Shader *m_pShader;
	unsigned int m_iNumArrays;
	unsigned int m_iNumTexArrays;
	unsigned int vaoID[128];
	unsigned int vboID[128];
	unsigned int eboID[128];
};

class OpenGLContext {
public:
	OpenGLContext(void) { };
	OpenGLContext(HWND hwnd);
	~OpenGLContext(void) { };
	bool initOpenGL(HWND hwnd, HDC hdc);
	void setupQuads(void);
	void resize(int w, int h);
	void renderScene(void);
	void testRender();
	void testShaders();
	void testTextures(OpenGLTexture *texture);
	void prepSquareCanvas();
	void prepTextureCanvas();
	void prepTestScene();
	void swapBuffers(HWND hwnd);
	void getWGLError();
	void getWGLSwapError();

private:
	int m_iWindowWidth;
	int m_iWindowHeight;

	Shader *m_pTestShader;

	// Projection, view and model matrices respectively
	glm::mat4 m_pMatrix;
	glm::mat4 m_vMatrix;
	glm::mat4 m_mMatrix;

	std::vector<OpenGLVAO*> vaos;
	std::vector<OpenGLTexture*> textures;

	unsigned int vaoID[1];
	unsigned int vboID[1];
	unsigned int eboID[1];

protected:
	HGLRC m_renderContext;
	HDC m_deviceContext;
	HWND m_windowID;
};

#pragma once
#include "OpenGLIncludes.h"
#include "OpenGLVAO.h"
#include <set>

// Helper class to define bounds
class QuadBounds {
public:
	QuadBounds(int upleft_X, int upleft_Y, int downright_X, int downright_Y) {
		value = std::make_tuple(upleft_X, upleft_Y, downright_X, downright_Y
		);
	}
	~QuadBounds(void) { };
private:
	std::tuple<int, int, int, int> value;
};

class OpenGLTexture {
public:
	OpenGLTexture(void* texture, int width, int height, bool isOpaque);
	OpenGLTexture(int width, int height);
	~OpenGLTexture(void);
	void initTexture2D(int width, int height);
	void initTexture2D(void* texture, int width, int height);
	void bindTexture2D (GLenum glTexture) const { glActiveTexture(glTexture); glBindTexture(GL_TEXTURE_2D, m_pTextureID[0]); }
	void unbindTexture2D (void) const { glBindTexture(GL_TEXTURE_2D, 0); }
	void updateTexture2D(void* texture, int width, int height);
	void initTextureFromOpenGLThread(void);
	unsigned int* getTexture(void) { return m_pTextureID; }
	OpenGLTexture *GenerateGlowMap(unsigned int fbo, OpenGLVAO* vao, OpenGLShader* shader, glm::vec2 texQuadSize);
	OpenGLTexture *GenerateGlowMap(unsigned int fbo, OpenGLVAO* vao, OpenGLShader* shader, std::tuple<int, int>texQuadSize, std::tuple<int, int>texStartPoint);
	OpenGLTexture *getGlowMap(void) { return m_pGlowMap.get(); }

private:
	unsigned int m_pTextureID[1];
	unsigned int pboID[2];
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	std::set<QuadBounds> m_pCompletedGlowmapTiles;
	GLint m_pixelFormat;
	GLint m_pixelType;
	void* m_pTextureToInitFrom = nullptr;
	std::unique_ptr<OpenGLTexture> m_pGlowMap = nullptr;
	bool m_isOpaque;
	bool m_bIsInited = false;
};

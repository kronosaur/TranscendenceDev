#pragma once
#include "OpenGLIncludes.h"
#include "OpenGLVAO.h"

class OpenGLTexture {
public:
	OpenGLTexture(void) { }
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
	OpenGLTexture *getGlowMap(void) { return m_pGlowMap.get(); }

private:
	unsigned int m_pTextureID[1];
	unsigned int pboID[2];
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	GLint m_pixelFormat;
	GLint m_pixelType;
	void* m_pTextureToInitFrom = nullptr;
	std::unique_ptr<OpenGLTexture> m_pGlowMap = nullptr;
	bool m_isOpaque;
	bool m_bIsInited = false;
};

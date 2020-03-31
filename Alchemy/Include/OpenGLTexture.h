#pragma once
#include "OpenGLIncludes.h"
#include "OpenGLVAO.h"
#include <unordered_set >

// Helper class to define bounds
class QuadBounds {
public:
	QuadBounds(float upleft_X, float upleft_Y, float size_X, float size_Y): value(std::make_tuple(upleft_X, upleft_Y, size_X, size_Y)) {};
	~QuadBounds(void) { };
	friend bool operator == (const QuadBounds& lhs, const QuadBounds& rhs) {
		return (std::get<0>(lhs.value) == std::get<0>(rhs.value) && std::get<1>(lhs.value) == std::get<1>(rhs.value) && std::get<2>(lhs.value) == std::get<2>(rhs.value) && std::get<3>(lhs.value) == std::get<3>(rhs.value));
	}
	size_t getHash() const {
		return std::hash<float>{}(std::get<0>(value)) ^
			(std::hash<float>{}(std::get<1>(value)) << 1) ^
			(std::hash<float>{}(std::get<2>(value)) << 2) ^
			(std::hash<float>{}(std::get<3>(value)) << 3);
	}
	std::tuple<float, float, float, float> getValue() const { return value; }
private:
	std::tuple<float, float, float, float> value;
};

class QBEquals {
public:
	size_t operator()(const QuadBounds &qb1, const QuadBounds &qb2) const {
		return qb1.getHash() == qb2.getHash();
	};
};


class QBHash {
public:
	size_t operator()(const QuadBounds &quadBounds) const {
		return quadBounds.getHash();
	};
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
	OpenGLTexture *GenerateGlowMap(unsigned int fbo, OpenGLVAO* vao, OpenGLShader* shader, const std::tuple<float, float>texQuadSize, const std::tuple<float, float>texStartPoint);
	OpenGLTexture *getGlowMap(void) { return m_pGlowMap.get(); }

private:
	unsigned int m_pTextureID[1];
	unsigned int pboID[2];
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	std::unordered_set <QuadBounds, QBHash, QBEquals> m_pCompletedGlowmapTiles;
	GLint m_pixelFormat;
	GLint m_pixelType;
	void* m_pTextureToInitFrom = nullptr;
	std::unique_ptr<OpenGLTexture> m_pGlowMap = nullptr;
	bool m_isOpaque;
	bool m_bIsInited = false;
};

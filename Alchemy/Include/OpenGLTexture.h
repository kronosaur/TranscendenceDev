#pragma once
#include "OpenGLIncludes.h"
#include "OpenGLVAO.h"
#include <unordered_set>

// Helper class to define bounds
class GlowmapTile {
public:
	GlowmapTile(float upleft_X, float upleft_Y, float size_X, float size_Y, float gridsize_X, float gridsize_Y): value(std::make_tuple(upleft_X, upleft_Y, size_X, size_Y, gridsize_X, gridsize_Y)) {};
	~GlowmapTile(void) { };
	friend bool operator == (const GlowmapTile& lhs, const GlowmapTile& rhs) {
		return (std::get<0>(lhs.value) == std::get<0>(rhs.value)
			&& std::get<1>(lhs.value) == std::get<1>(rhs.value)
			&& std::get<2>(lhs.value) == std::get<2>(rhs.value)
			&& std::get<3>(lhs.value) == std::get<3>(rhs.value)
			&& std::get<4>(lhs.value) == std::get<4>(rhs.value)
			&& std::get<5>(lhs.value) == std::get<5>(rhs.value));
	}
	size_t getHash() const {
		return std::hash<float>{}(std::get<0>(value))
			^ (std::hash<float>{}(std::get<1>(value)) << 1)
			^ (std::hash<float>{}(std::get<2>(value)) << 2)
			^ (std::hash<float>{}(std::get<3>(value)) << 3)
			^ (std::hash<float>{}(std::get<4>(value)) << 4)
			^ (std::hash<float>{}(std::get<5>(value)) << 5);
	}
	std::tuple<float, float, float, float, float, float> getValue() const { return value; }
private:
	std::tuple<float, float, float, float, float, float> value;
};

class QBEquals {
public:
	size_t operator()(const GlowmapTile &qb1, const GlowmapTile &qb2) const {
		return qb1.getHash() == qb2.getHash();
	};
};


class QBHash {
public:
	size_t operator()(const GlowmapTile &quadBounds) const {
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
	OpenGLTexture *GenerateGlowMap(unsigned int fbo, OpenGLVAO* vao, OpenGLShader* shader, const glm::vec2 texQuadSize, const glm::vec2 texStartPoint, const glm::vec2 texGridSize);
	OpenGLTexture *getGlowMap(void) { return m_pGlowMap.get(); }
	void requestGlowmapTile(float upleft_X, float upleft_Y, float size_X, float size_Y, float gridsize_X, float gridsize_Y) {
		// This function should be called when we request a render with this texture
		// We should not add a request if it is already in the completed queue
		auto glowmapTile = GlowmapTile(upleft_X, upleft_Y, size_X, size_Y, gridsize_X, gridsize_Y);
		bool tileAlreadyRendered = m_CompletedGlowmapTiles.find(glowmapTile) != m_CompletedGlowmapTiles.end();
		if (!tileAlreadyRendered) {
			m_GlowmapTilesToRender.insert(glowmapTile);
		}
	}
	void populateGlowmaps(unsigned int fbo, OpenGLVAO* vao, OpenGLShader* shader) {
		for (const auto& tileToRender : m_GlowmapTilesToRender) {
			auto [ texStart_x, texStart_y, texQuadSize_x, texQuadSize_y, texGridSize_x, texGridSize_y ] = tileToRender.getValue();
			glm::vec2 texStart(texStart_x, texStart_y); // Starting point of this texture sprite sheet, 1.0 being bottom/rightmost and 0.0 being top/leftmost
			glm::vec2 texSize(texQuadSize_x, texQuadSize_y); // Size of the ENTIRE sprite sheet, 1.0 being 100% of the quad in X/Y dir
			glm::vec2 texGridSize(texGridSize_x, texGridSize_y); // Size of each frame of this sprite sheet, 1.0 being 100% of the quad in X/Y dir
			GenerateGlowMap(fbo, vao, shader, texSize, texStart, texGridSize);
			m_CompletedGlowmapTiles.insert(tileToRender);
		}
		m_GlowmapTilesToRender.clear();
	}

private:
	unsigned int m_pTextureID[1];
	unsigned int pboID[2];
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	std::unordered_set <GlowmapTile, QBHash, QBEquals> m_CompletedGlowmapTiles;
	std::unordered_set <GlowmapTile, QBHash, QBEquals> m_GlowmapTilesToRender;
	GLint m_pixelFormat;
	GLint m_pixelType;
	void* m_pTextureToInitFrom = nullptr;
	std::unique_ptr<OpenGLTexture> m_pGlowMap = nullptr;
	bool m_isOpaque;
	bool m_bIsInited = false;
};

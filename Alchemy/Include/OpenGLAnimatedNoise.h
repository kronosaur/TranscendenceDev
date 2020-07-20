#pragma once
#include "OpenGLIncludes.h"
#include "OpenGLVAO.h"
#include <unordered_set>

class OpenGLAnimatedNoise {
	// Class for storing procedurally generated 3D noise
public:
	OpenGLAnimatedNoise(int width, int height, int frames);
	~OpenGLAnimatedNoise(void);
	void initTexture3D(int width, int height, int frames);
	void bindTexture3D(GLenum glTexture) const { glActiveTexture(glTexture); glBindTexture(GL_TEXTURE_3D, m_pTextureID[0]); }
	void unbindTexture3D(void) const { glBindTexture(GL_TEXTURE_3D, 0); }
	unsigned int* getTexture(void) { return m_pTextureID; }
	void populateTexture3D(unsigned int fbo, OpenGLVAO* vao, OpenGLShader* shader);
	/*
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
			auto[texStart_x, texStart_y, texQuadSize_x, texQuadSize_y, texGridSize_x, texGridSize_y] = tileToRender.getValue();
			glm::vec2 texStart(texStart_x, texStart_y); // Starting point of this texture sprite sheet, 1.0 being bottom/rightmost and 0.0 being top/leftmost
			glm::vec2 texSize(texQuadSize_x, texQuadSize_y); // Size of the ENTIRE sprite sheet, 1.0 being 100% of the quad in X/Y dir
			glm::vec2 texGridSize(texGridSize_x, texGridSize_y); // Size of each frame of this sprite sheet, 1.0 being 100% of the quad in X/Y dir
			GenerateGlowMap(fbo, vao, shader, texSize, texStart, texGridSize);
			m_CompletedGlowmapTiles.insert(tileToRender);
		}
		m_GlowmapTilesToRender.clear();
	}
	*/

private:
	unsigned int m_pTextureID[1];
	unsigned int pboID[2];
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	unsigned int m_iNumFrames;
	GLint m_pixelFormat;
	GLint m_pixelType;
	bool m_isOpaque;
	bool m_bIsInited = false;
};

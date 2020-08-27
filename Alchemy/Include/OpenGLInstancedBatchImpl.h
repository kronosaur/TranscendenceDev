#pragma once
#include "OpenGLInstancedBatch.h"

// Each instanced batch type for a specific shader is derived from a class generated from the OpenGLInstancedBatchRenderRequest template.

class OpenGLInstancedBatchRenderRequestTexture : public OpenGLInstancedBatchRenderRequest<glm::vec2, glm::vec2, glm::vec2, glm::vec2, float, glm::vec4, float> {
public:
	OpenGLInstancedBatchRenderRequestTexture(
		glm::vec2 texPositions,
		glm::vec2 canvasQuadSizes,
		glm::vec2 canvasPositions,
		glm::vec2 textureQuadSizes,
		float alphaStrength,
		glm::vec4 glowColor,
		float glowNoise) : OpenGLInstancedBatchRenderRequest{ texPositions, canvasQuadSizes, canvasPositions, textureQuadSizes, alphaStrength, glowColor, glowNoise } {};
	OpenGLVAO& getVAOForInstancedBatchType() override { if (!vao) { vao = std::move(setUpVAO()); } return *(vao.get()); }
	int getRenderRequestSize() override { return sizeof(*this); }
private:
	static std::unique_ptr<OpenGLVAO> vao;
};

class OpenGLInstancedBatchRenderRequestRay : public OpenGLInstancedBatchRenderRequest<glm::vec4, float, glm::ivec4, glm::ivec4, glm::vec4, glm::vec3, glm::vec3, float, int, int> {
public:
	OpenGLInstancedBatchRenderRequestRay(
		glm::vec4 sizeAndPosition,
		float rotation,
		glm::ivec4 shapes,
		glm::ivec4 styles,
		glm::vec4 intensitiesAndCycles,
		glm::vec3 primaryColor,
		glm::vec3 secondaryColor,
		float seed,
		int effectType,
		int blendMode) : OpenGLInstancedBatchRenderRequest{ sizeAndPosition, rotation, shapes, styles, intensitiesAndCycles, primaryColor, secondaryColor, seed, effectType, blendMode } {};
	OpenGLVAO& getVAOForInstancedBatchType() override { if (!vao) { vao = std::move(setUpVAO()); } return *(vao.get()); }
	int getRenderRequestSize() override { return sizeof(*this); }
private:
	static std::unique_ptr<OpenGLVAO> vao;
};

class OpenGLInstancedBatchRenderRequestOrb : public OpenGLInstancedBatchRenderRequest<glm::vec4, float, float, float, int, int, int, int, int, int, int, glm::vec3, glm::vec3, float > {
public:
	OpenGLInstancedBatchRenderRequestOrb(
		glm::vec4 sizeAndPosition,
		float rotation,
		float intensity,
		float opacity,
		int animation,
		int style,
		int detail,
		int distortion,
		int animationSeed,
		int lifetime,
		int currFrame,
		glm::vec3 primaryColor,
		glm::vec3 secondaryColor,
		float secondaryOpacity) : OpenGLInstancedBatchRenderRequest{ sizeAndPosition, rotation, intensity, opacity, animation, style, detail, distortion, animationSeed, lifetime, currFrame, primaryColor, secondaryColor, secondaryOpacity } {};
	OpenGLVAO& getVAOForInstancedBatchType() override { if (!vao) { vao = std::move(setUpVAO()); } return *(vao.get()); }
	int getRenderRequestSize() override { return sizeof(*this); }
private:
	static std::unique_ptr<OpenGLVAO> vao;
};

// Define uniforms for each shader

typedef OpenGLInstancedBatch<OpenGLInstancedBatchRenderRequestRay, std::tuple<float, glm::vec2, const OpenGLAnimatedNoise*>> OpenGLInstancedBatchRay;
typedef OpenGLInstancedBatch<OpenGLInstancedBatchRenderRequestOrb, std::tuple<float, glm::vec2>> OpenGLInstancedBatchOrb;
typedef OpenGLInstancedBatch<OpenGLInstancedBatchRenderRequestTexture, std::tuple<OpenGLTexture*, OpenGLTexture*, int, const OpenGLAnimatedNoise*>> OpenGLInstancedBatchTexture;

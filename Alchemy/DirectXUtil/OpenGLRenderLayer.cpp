#include "OpenGL.h"
#include "OpenGLInstancedBatchImpl.h"
#include "PreComp.h"
#include <mutex>

namespace {

} // namespace 


OpenGLRenderLayer::~OpenGLRenderLayer(void)
{
	clear();
}

void OpenGLRenderLayer::addTextureToRenderQueue(glm::vec2 vTexPositions, glm::vec2 vSpriteSheetPositions, glm::vec2 vCanvasQuadSizes, glm::vec2 vCanvasPositions, glm::vec2 vTextureQuadSizes, glm::vec4 glowColor, float alphaStrength,
											 float glowNoise, int numFramesPerRow, int numFramesPerCol, OpenGLTexture* image, float startingDepth)
{
	// Note, image is a pointer to the CG32bitPixel* we want to use as a texture. We can use CG32bitPixel->GetPixelArray() to get this pointer.
	// To get the width and height, we can use pSource->GetWidth() and pSource->GetHeight() respectively.
	// Note that we don't initialize the textures here, but instead do it when we render - this is because this function can be called by
	// different threads, but it's a very bad idea to run OpenGL on multiple threads at the same time - so texture initialization (which involves
	// OpenGL function calls) must all be done on the OpenGL thread
	const std::unique_lock<std::mutex> lock(m_texRenderQueueAddMutex);

	// Check to see if we have a render queue with that texture already loaded.
	ASSERT(image);
	if (!m_texRenderBatches.count(image))
	{
		// If we don't have a render queue with that texture loaded, then add one.
		// Note, we clear after every render, in order to prevent seg fault issues; also creating an instanced batch is not very expensive anymore.
		m_texRenderBatches[image] = new OpenGLInstancedBatchTexture();
	}

	// Initialize a glowmap tile request here, and save it in the MRQ. We consume this when we generate textures, to render glowmaps.
	// Only do this if we need to render a glowy thing.
	if (glowColor[3] > 0.0) {
		image->requestGlowmapTile(vSpriteSheetPositions[0], vSpriteSheetPositions[1], float(numFramesPerRow * vTextureQuadSizes[0]), float(numFramesPerCol * vTextureQuadSizes[1]), vTextureQuadSizes[0], vTextureQuadSizes[1]);
		m_texturesNeedingGlowmaps.push_back(image);
	}

	// Add this quad to the render queue.
	auto renderRequest = OpenGLInstancedBatchRenderRequestTexture(vTexPositions, vCanvasQuadSizes, vCanvasPositions, vTextureQuadSizes, alphaStrength, glowColor, glowNoise);
	renderRequest.set_depth(startingDepth);
	m_texRenderBatches[image]->addObjToRender(renderRequest);
}

void OpenGLRenderLayer::addRayToEffectRenderQueue(glm::vec3 vPrimaryColor, glm::vec3 vSecondaryColor, glm::vec4 sizeAndPosition, glm::ivec2 shapes, glm::vec3 intensitiesAndCycles, glm::ivec3 styles, float rotation, float startingDepth)
{
	auto renderRequest = OpenGLInstancedBatchRenderRequestRay(sizeAndPosition, rotation, shapes, styles, intensitiesAndCycles, vPrimaryColor, vSecondaryColor);
	renderRequest.set_depth(startingDepth);
	m_rayRenderBatch.addObjToRender(renderRequest);
}

void OpenGLRenderLayer::addLightningToEffectRenderQueue(glm::vec3 vPrimaryColor, glm::vec3 vSecondaryColor, glm::vec4 sizeAndPosition, glm::ivec2 shapes, float rotation, float seed, float startingDepth)
{
	auto renderRequest = OpenGLInstancedBatchRenderRequestLightning(sizeAndPosition, rotation, shapes, seed, vPrimaryColor, vSecondaryColor);
	renderRequest.set_depth(startingDepth);
	m_lightningRenderBatch.addObjToRender(renderRequest);
}

void OpenGLRenderLayer::addOrbToEffectRenderQueue(glm::vec4 sizeAndPosition,
	float rotation,
	float radius,
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
	float startingDepth)
{
	auto renderRequest = OpenGLInstancedBatchRenderRequestOrb(sizeAndPosition, rotation, radius, intensity, opacity, animation, style, detail, distortion, animationSeed, lifetime, currFrame, primaryColor, secondaryColor);
	renderRequest.set_depth(startingDepth);
	m_orbRenderBatch.addObjToRender(renderRequest);
}

void OpenGLRenderLayer::renderAllQueues(float &depthLevel, float depthDelta, int currentTick, glm::ivec2 canvasDimensions, OpenGLShader *objectTextureShader, OpenGLShader *rayShader, OpenGLShader *lightningShader, OpenGLShader *glowmapShader, OpenGLShader *orbShader, unsigned int fbo, OpenGLVAO* canvasVAO)
{
	// For each render queue in the ships render queue, render that render queue. We need to set the texture and do a glBindTexture before doing so.
	// Render order is Texture, Orb, Ray, Lightning
	
	// Delete textures scheduled for deletion.
	// TODO: Remove? We don't seem to use m_texturesForDeletion anymore
	if (m_texturesForDeletion.size() > 0)
		for (const std::shared_ptr<OpenGLTexture> textureToDelete : m_texturesForDeletion) {
			// If any value exists here, delete it from m_texRenderBatches if it exists there
			if (m_texRenderBatches.find(textureToDelete.get()) != m_texRenderBatches.end()) {
				m_texRenderBatches.erase(textureToDelete.get());
			}
		}
	m_texturesForDeletion.clear();

	for (const auto &p : m_texRenderBatches)
	{
		OpenGLTexture *pTextureToUse = p.first;
		// Initialize the texture if necessary; we do this here because all OpenGL calls must be made on the same thread
		pTextureToUse->initTextureFromOpenGLThread();
		OpenGLInstancedBatchTexture *pInstancedRenderQueue = p.second;
		// TODO: Set the depths here before rendering. This will ensure that we always render from back to front, which should solve most issues with blending.

		std::array<std::string, 3> textureUniformNames = { "obj_texture", "glow_map", "current_tick" };
		pInstancedRenderQueue->setUniforms(textureUniformNames, pTextureToUse, pTextureToUse->getGlowMap() ? pTextureToUse->getGlowMap() : pTextureToUse, currentTick);
		pInstancedRenderQueue->Render(objectTextureShader, depthLevel, depthDelta, currentTick, false);
		pInstancedRenderQueue->clear();
	}

	std::array<std::string, 2> rayAndLightningUniformNames = { "current_tick", "aCanvasAdjustedDimensions" };

	m_rayRenderBatch.setUniforms(rayAndLightningUniformNames, float(currentTick), canvasDimensions);
	m_rayRenderBatch.Render(rayShader, depthLevel, depthDelta, currentTick);
	m_lightningRenderBatch.setUniforms(rayAndLightningUniformNames, float(currentTick), canvasDimensions);
	m_lightningRenderBatch.Render(lightningShader, depthLevel, depthDelta, currentTick);
	m_orbRenderBatch.setUniforms(rayAndLightningUniformNames, float(currentTick), canvasDimensions);
	m_orbRenderBatch.Render(orbShader, depthLevel, depthDelta, currentTick);

	m_texRenderBatches.clear();
}

void OpenGLRenderLayer::GenerateGlowmaps(unsigned int fbo, OpenGLVAO *canvasVAO, OpenGLShader* glowmapShader) {
	for (auto pTextureToUse : m_texturesNeedingGlowmaps)
	{
		// Generate a glow map for this texture if needed.
		// Glow map must be done in different block after actual rendering because otherwise it causes flickering issues
		// TODO: Move to a block that occurs after all layers are done rendering, or better yet, only generate glowmaps on demand.
		pTextureToUse->populateGlowmaps(fbo, canvasVAO, glowmapShader);
	}
}

void OpenGLRenderLayer::clear(void)
{
	// Clear our queue vectors, and our textures.
	// TODO: Do this...
}

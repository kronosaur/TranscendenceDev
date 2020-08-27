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

void OpenGLRenderLayer::addRayToEffectRenderQueue(glm::vec3 vPrimaryColor, glm::vec3 vSecondaryColor, glm::vec4 sizeAndPosition, glm::ivec4 shapes, glm::vec3 intensitiesAndCycles, glm::ivec4 styles, float rotation, float startingDepth, OpenGLRenderLayer::blendMode blendMode)
{
	glm::vec4 intensityAndCyclesV4(intensitiesAndCycles[0], intensitiesAndCycles[1], intensitiesAndCycles[2], 0.0);
	auto renderRequest = OpenGLInstancedBatchRenderRequestRay(sizeAndPosition, rotation, shapes, styles, intensityAndCyclesV4, vPrimaryColor, vSecondaryColor, 0, OpenGLRenderLayer::effectType::effectTypeRay, blendMode);
	renderRequest.set_depth(startingDepth);
	addProceduralEffectToProperRenderQueue(renderRequest, blendMode);
}

void OpenGLRenderLayer::addLightningToEffectRenderQueue(glm::vec3 vPrimaryColor, glm::vec3 vSecondaryColor, glm::vec4 sizeAndPosition, glm::ivec4 shapes, float rotation, float seed, float startingDepth, OpenGLRenderLayer::blendMode blendMode)
{
	auto renderRequest = OpenGLInstancedBatchRenderRequestRay(sizeAndPosition, rotation, shapes, glm::ivec4(0, 0, 0, 0), glm::vec4(0, 0, 0, 0), vPrimaryColor, vSecondaryColor, seed, OpenGLRenderLayer::effectType::effectTypeLightning, blendMode);
	renderRequest.set_depth(startingDepth);
	addProceduralEffectToProperRenderQueue(renderRequest, blendMode);
}

void OpenGLRenderLayer::addOrbToEffectRenderQueue(glm::vec4 sizeAndPosition,
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
	float secondaryOpacity,
	float startingDepth,
	OpenGLRenderLayer::blendMode blendMode)
	// aShapes: orbLifetime, orbCurrFrame, orbDistortion, orbDetail
    // aStyles: orbStyle, orbAnimation, opacity, blank
    // aFloatParams: intensity, blank, opacityAdj, orbSecondaryOpacity
{

	glm::ivec4 shapes(lifetime, currFrame, distortion, detail);
	glm::ivec4 styles(style, animation, 0, 0);
	glm::vec4 floatParams(intensity, secondaryOpacity, opacity, 0.0);

	auto renderRequest = OpenGLInstancedBatchRenderRequestRay(sizeAndPosition, rotation, shapes, styles, floatParams, primaryColor, secondaryColor, float(animationSeed), OpenGLRenderLayer::effectType::effectTypeOrb, blendMode);
	renderRequest.set_depth(startingDepth);
	addProceduralEffectToProperRenderQueue(renderRequest, blendMode);
}

void OpenGLRenderLayer::renderAllQueues(float &depthLevel, float depthDelta, int currentTick, glm::ivec2 canvasDimensions, OpenGLShader *objectTextureShader, OpenGLShader *rayShader, OpenGLShader *glowmapShader, OpenGLShader *orbShader, unsigned int fbo, OpenGLVAO* canvasVAO, const OpenGLAnimatedNoise* perlinNoise)
{
	// For each render queue in the ships render queue, render that render queue. We need to set the texture and do a glBindTexture before doing so.
	// Render order is Texture, Orb, Ray, Lightning
	
	// TODO: Render in proper order - by order requested in CPU code. We can do this via the following:
	// Use a while loop that repeats while we still have stuff left in our render batches.
	// In this loop, iterate through our batches, and find the ones with the first and second largest depths
	// for their last elements. On the one with the first largest, render all of the objects up to and excluding
	// the first one whose depth is smaller than the second largest. Delete those from CPU memory. Remember that all batches
	// are in order to render.

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

	std::vector<std::pair<OpenGLShader*, OpenGLInstancedBatchInterface*>> batchesToRender;

	// Set uniforms for all render batches and append them to our list of batches to render
	// Note, setUniforms is persistent across calls to any Render() function since it sets things in the batch object, not in OpenGL
	for (const auto& p : m_texRenderBatches)
	{
		OpenGLTexture* pTextureToUse = p.first;
		// Initialize the texture if necessary; we do this here because all OpenGL calls must be made on the same thread
		pTextureToUse->initTextureFromOpenGLThread();
		OpenGLInstancedBatchTexture* pInstancedRenderQueue = p.second;
		// TODO: Set the depths here before rendering. This will ensure that we always render from back to front, which should solve most issues with blending.
		std::array<std::string, 4> textureUniformNames = { "obj_texture", "glow_map", "current_tick", "perlin_noise" };
		pInstancedRenderQueue->setUniforms(textureUniformNames, pTextureToUse, pTextureToUse->getGlowMap() ? pTextureToUse->getGlowMap() : pTextureToUse, currentTick, perlinNoise);
		batchesToRender.push_back(std::pair(objectTextureShader, pInstancedRenderQueue));
	}
	std::array<std::string, 3> rayAndLightningUniformNames = { "current_tick", "aCanvasAdjustedDimensions", "perlin_noise" };
	m_rayRenderBatchBlendNormal.setUniforms(rayAndLightningUniformNames, float(currentTick), canvasDimensions, perlinNoise);
	batchesToRender.push_back(std::pair(rayShader, &m_rayRenderBatchBlendNormal));
	m_rayRenderBatchBlendScreen.setUniforms(rayAndLightningUniformNames, float(currentTick), canvasDimensions, perlinNoise);
	batchesToRender.push_back(std::pair(rayShader, &m_rayRenderBatchBlendScreen));

	int iDeepestBatchIndex = -1;
	int iSecondDeepestBatchIndex = -1;
	float fDeepestBatchLastElementDepth = 0.0;
	float fSecondDeepestBatchLastElementDepth = 0.0;

	blendMode prevBlendMode = blendMode::blendNormal;

	while (true) {
		// Phase 0: Reset tracking vars
		iDeepestBatchIndex = -1;
		iSecondDeepestBatchIndex = -1;
		fDeepestBatchLastElementDepth = -1.0;
		fSecondDeepestBatchLastElementDepth = -1.0;

		int iCurrBatchIndex = 0;
		// Phase 1: Get deepest and second deepest batches
		for (const auto& p : batchesToRender) {
			OpenGLInstancedBatchInterface* pInstancedRenderBatch = p.second;
			float fBatchDepth = pInstancedRenderBatch->getDepthOfDeepestObject();
			if (fBatchDepth > fDeepestBatchLastElementDepth) {
				iSecondDeepestBatchIndex = iDeepestBatchIndex;
				fSecondDeepestBatchLastElementDepth = fDeepestBatchLastElementDepth;
				iDeepestBatchIndex = iCurrBatchIndex;
				fDeepestBatchLastElementDepth = fBatchDepth;
			}
			iCurrBatchIndex += 1;
		}

		// Phase 2: If deepest batch depth is less than zero, we're done.
		if (fDeepestBatchLastElementDepth < 0) {
			break;
		}

		// Phase 3: Render from deepest batch up to depth of second deepest batch's deepest object
		auto batchToRenderShader = batchesToRender[iDeepestBatchIndex].first;
		auto batchToRender = batchesToRender[iDeepestBatchIndex].second;
		int currBlendMode = batchToRender->getBlendMode();

		if (currBlendMode != int(prevBlendMode)) {
			switch (currBlendMode) {
				case int(blendMode::blendNormal) :
					prevBlendMode = blendMode::blendNormal;
					setBlendModeNormal();
					break;
				case int(blendMode::blendScreen) :
				    prevBlendMode = blendMode::blendScreen;
				    setBlendModeScreen();
				    break;
				default:
					prevBlendMode = blendMode::blendNormal;
					setBlendModeNormal();
			}
		}

		batchToRender->RenderUpToGivenDepth(batchToRenderShader, fSecondDeepestBatchLastElementDepth);
	}
	for (const auto& p : batchesToRender) {
		OpenGLInstancedBatchInterface* pInstancedRenderBatch = p.second;
		pInstancedRenderBatch->clear();
	}
	/*
	for (const auto& p : batchesToRender) {
		OpenGLShader* pShaderToUse = p.first;
		OpenGLInstancedBatchInterface* pInstancedRenderQueue = p.second;
		// Initialize the texture if necessary; we do this here because all OpenGL calls must be made on the same thread
		pInstancedRenderQueue->Render(pShaderToUse, depthLevel, depthDelta);
		pInstancedRenderQueue->clear();
	}
	*/

	m_texRenderBatches.clear();
	setBlendModeNormal();
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

#include "OpenGL.h"
#include "PreComp.h"
#include <mutex>

const float OpenGLMasterRenderQueue::m_fDepthDelta = 0.000001f; // Up to one million different depth levels
const float OpenGLMasterRenderQueue::m_fDepthStart = 0.999998f; // Up to one million different depth levels

namespace {
	ContainerTyped<glm::vec2>* getTextureCoordinates(OpenGLInstancedBatchTexture *instancedBatchTexture) {
		return reinterpret_cast<ContainerTyped<glm::vec2>*>(instancedBatchTexture->getParameterForObject(0));
	}

	ContainerTyped<glm::vec2>* getTextureSizes(OpenGLInstancedBatchTexture *instancedBatchTexture) {
		return reinterpret_cast<ContainerTyped<glm::vec2>*>(instancedBatchTexture->getParameterForObject(3));
	}
} // namespace 

OpenGLMasterRenderQueue::OpenGLMasterRenderQueue(void)
{
	// Initialize the VAO.
	initializeCanvasVAO();
	// Depth level starts at one minus the delta.
	m_fDepthLevel = m_fDepthStart - m_fDepthDelta;
	m_iCurrentTick = 0;
	// Initialize the FBO.
	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &rbo);
	m_pGlowmapShader = new OpenGLShader("./shaders/glowmap_vertex_shader.glsl", "./shaders/glowmap_fragment_shader.glsl");
	m_pObjectTextureShader = new OpenGLShader("./shaders/instanced_vertex_shader.glsl", "./shaders/instanced_fragment_shader.glsl");
	m_pRayShader = new OpenGLShader("./shaders/ray_vertex_shader.glsl", "./shaders/ray_fragment_shader.glsl");
	m_pLightningShader = new OpenGLShader("./shaders/lightning_vertex_shader.glsl", "./shaders/lightning_fragment_shader.glsl");

}

OpenGLMasterRenderQueue::~OpenGLMasterRenderQueue(void)
{
	clear();
	//deinitCanvasVAO();
	// TODO: Delete render queues, delete VAOs properly (specifically the instanced VAO bits)
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);
	delete m_pGlowmapShader;
	delete m_pObjectTextureShader;
	delete m_pRayShader;
}

void OpenGLMasterRenderQueue::initializeCanvasVAO()
{
	// Prepare the background canvas.

	float fSize = 1.0f;
	float posZ = 0.999999f;

	std::vector<float> vertices{
		fSize, fSize, posZ,
		fSize, -fSize, posZ,
		-fSize, -fSize, posZ,
		-fSize, fSize, posZ,
	};

	std::vector<float> colors{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};

	std::vector<float> texcoords{
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
	};

	std::vector<unsigned int> indices{
		0, 1, 3,
		1, 2, 3
	};

	std::vector<std::vector<float>> vbos{ vertices, colors };
	std::vector<std::vector<float>> texcoord_arr{ texcoords };
	std::vector<std::vector<unsigned int>> ebos{ indices, indices, indices };

	m_pCanvasVAO = new OpenGLVAO(vbos, ebos, texcoord_arr);
	m_pCanvasVAO->setShader(m_pGlowmapShader);

}

void OpenGLMasterRenderQueue::deinitCanvasVAO(void)
{
	// TODO(heliogenesis): Move this VAO to the parent class once it's done
	unsigned int *canvasVAO = m_pCanvasVAO->getinstancedVBO();
	glDeleteBuffers(16, &canvasVAO[0]);
	delete[] m_pCanvasVAO;
}

void OpenGLMasterRenderQueue::addShipToRenderQueue(int startPixelX, int startPixelY, int sizePixelX,
 int sizePixelY, int posPixelX, int posPixelY, int canvasHeight, int canvasWidth, OpenGLTexture *image, int texWidth, int texHeight, 
	int texQuadWidth, int texQuadHeight, int numFramesPerRow, int numFramesPerCol, int spriteSheetStartX, int spriteSheetStartY, float alphaStrength,
	float glowR, float glowG, float glowB, float glowA, float glowNoise)
	{
	// Note, image is a pointer to the CG32bitPixel* we want to use as a texture. We can use CG32bitPixel->GetPixelArray() to get this pointer.
	// To get the width and height, we can use pSource->GetWidth() and pSource->GetHeight() respectively.
	// Note that we don't initialize the textures here, but instead do it when we render - this is because this function can be called by
	// different threads, but it's a very bad idea to run OpenGL on multiple threads at the same time - so texture initialization (which involves
	// OpenGL function calls) must all be done on the OpenGL thread

	const std::unique_lock<std::mutex> lock(m_shipRenderQueueAddMutex);

	// Check to see if we have a render queue with that texture already loaded.
	ASSERT(image);
	if (!m_shipRenderQueues.count(image))
	{
		// If we don't have a render queue with that texture loaded, then add one.
		m_shipRenderQueues[image] = new OpenGLInstancedBatchTexture();
	}
	// Add this quad to the render queue.
	glm::vec2 vTexPositions((float)startPixelX / (float)texWidth, (float)startPixelY / (float)texHeight);
	glm::vec2 vSpriteSheetPositions((float)spriteSheetStartX / (float)texWidth, (float)spriteSheetStartY / (float)texHeight);
	glm::vec2 vCanvasQuadSizes((float)sizePixelX / (float)canvasWidth, (float)sizePixelY / (float)canvasHeight);
	glm::vec2 vCanvasPositions((float)posPixelX / (float)canvasWidth, (float)posPixelY / (float)canvasHeight);
	glm::vec2 vTextureQuadSizes((float)texQuadWidth / (float)texWidth, (float)texQuadHeight / (float)texHeight);
	glm::vec4 glowColor(glowR, glowG, glowB, glowA);

	// Initialize a glowmap tile request here, and save it in the MRQ. We consume this when we generate textures, to render glowmaps.
	image->requestGlowmapTile(vSpriteSheetPositions[0], vSpriteSheetPositions[1], float(numFramesPerRow * vTextureQuadSizes[0]), float(numFramesPerCol * vTextureQuadSizes[1]), vTextureQuadSizes[0], vTextureQuadSizes[1]);

	m_shipRenderQueues[image]->addObjToRender(vTexPositions, vCanvasQuadSizes, vCanvasPositions, vTextureQuadSizes, alphaStrength, glowColor, glowNoise);
	}

void OpenGLMasterRenderQueue::addRayToEffectRenderQueue(int posPixelX, int posPixelY, int sizePixelX, int sizePixelY, int canvasSizeX, int canvasSizeY, float rotation,
	                                                    int iColorTypes, int iOpacityTypes, int iWidthAdjType, int iReshape, int iTexture, std::tuple<int, int, int> primaryColor,
	                                                    std::tuple<int, int, int> secondaryColor, int iIntensity, float waveCyclePos, int opacityAdj)
	{
	glm::vec3 vPrimaryColor = glm::vec3(std::get<0>(primaryColor), std::get<1>(primaryColor), std::get<2>(primaryColor)) / float(255.0);
	glm::vec3 vSecondaryColor = glm::vec3(std::get<0>(secondaryColor), std::get<1>(secondaryColor), std::get<2>(secondaryColor)) / float(255.0);

	glm::vec4 sizeAndPosition((float)sizePixelX, (float)sizePixelY,
		(float)posPixelX / (float)canvasSizeX, (float)posPixelY / (float)canvasSizeY);
	glm::ivec2 shapes(iWidthAdjType, iReshape);
	glm::vec3 intensitiesAndCycles(float(iIntensity), waveCyclePos, float(opacityAdj) / 255.0f);
	glm::ivec3 styles(iColorTypes, iOpacityTypes, iTexture);

	m_effectRayRenderQueue.addObjToRender(sizeAndPosition, rotation, shapes, styles, intensitiesAndCycles, vPrimaryColor, vSecondaryColor);
	}

void OpenGLMasterRenderQueue::addLightningToEffectRenderQueue(int posPixelX, int posPixelY, int sizePixelX, int sizePixelY, int canvasSizeX, int canvasSizeY, float rotation,
	int iWidthAdjType, int iReshape, std::tuple<int, int, int> primaryColor,
	std::tuple<int, int, int> secondaryColor, float seed)
{
	glm::vec3 vPrimaryColor = glm::vec3(std::get<0>(primaryColor), std::get<1>(primaryColor), std::get<2>(primaryColor)) / float(255.0);
	glm::vec3 vSecondaryColor = glm::vec3(std::get<0>(secondaryColor), std::get<1>(secondaryColor), std::get<2>(secondaryColor)) / float(255.0);
	glm::vec4 sizeAndPosition((float)sizePixelX, (float)sizePixelY,
		(float)posPixelX / (float)canvasSizeX, (float)posPixelY / (float)canvasSizeY);
	glm::ivec2 shapes(iWidthAdjType, iReshape);

	m_effectLightningRenderQueue.addObjToRender(sizeAndPosition, rotation, shapes, seed, vPrimaryColor, vSecondaryColor);
}

void OpenGLMasterRenderQueue::renderAllQueues(void)
{
	// For each render queue in the ships render queue, render that render queue. We need to set the texture and do a glBindTexture before doing so.
		// Delete textures scheduled for deletion.
	if (m_texturesForDeletion.size() > 0)
		for (const std::shared_ptr<OpenGLTexture> textureToDelete : m_texturesForDeletion) {
			// If any value exists here, delete it from m_shipRenderQueues if it exists there
			if (m_shipRenderQueues.find(textureToDelete.get()) != m_shipRenderQueues.end()) {
				m_shipRenderQueues.erase(textureToDelete.get());
			}
		}
	m_texturesForDeletion.clear();

	for (const auto &p : m_shipRenderQueues)
	{
		OpenGLTexture *pTextureToUse = p.first;
		// Initialize the texture if necessary; we do this here because all OpenGL calls must be made on the same thread
		pTextureToUse->initTextureFromOpenGLThread();
		OpenGLInstancedBatchTexture *pInstancedRenderQueue = p.second;
		// TODO: Set the depths here before rendering. This will ensure that we always render from back to front, which should solve most issues with blending.

		float depthLevel = m_fDepthLevel;
		std::array<std::string, 3> textureUniformNames = { "obj_texture", "glow_map", "current_tick" };
		pInstancedRenderQueue->setUniforms(textureUniformNames, pTextureToUse, pTextureToUse->getGlowMap() ? pTextureToUse->getGlowMap() : pTextureToUse, m_iCurrentTick);
		pInstancedRenderQueue->Render(m_pObjectTextureShader, depthLevel, m_fDepthDelta, m_iCurrentTick, false);
		m_fDepthLevel = depthLevel;
		pInstancedRenderQueue->clear();
	}

	std::array<std::string, 2> rayAndLightningUniformNames = { "current_tick", "aCanvasAdjustedDimensions" };

	m_effectRayRenderQueue.setUniforms(rayAndLightningUniformNames, float(m_iCurrentTick), glm::ivec2(m_iCanvasWidth, m_iCanvasHeight));
	m_effectRayRenderQueue.Render(m_pRayShader, m_fDepthLevel, m_fDepthDelta, m_iCurrentTick);
	m_effectLightningRenderQueue.setUniforms(rayAndLightningUniformNames, float(m_iCurrentTick), glm::ivec2(m_iCanvasWidth, m_iCanvasHeight));
	m_effectLightningRenderQueue.Render(m_pLightningShader, m_fDepthLevel, m_fDepthDelta, m_iCurrentTick);
	// Reset the depth level.
	m_fDepthLevel = m_fDepthStart - m_fDepthDelta;

	for (const auto &p : m_shipRenderQueues)
	{
		// Generate a glow map for this texture if needed.
		// Glow map must be done in different block after actual rendering because otherwise it causes flickering issues
		OpenGLTexture *pTextureToUse = p.first;
		pTextureToUse->populateGlowmaps(fbo, m_pCanvasVAO, m_pGlowmapShader);
	}
}

void OpenGLMasterRenderQueue::clear(void)
{
	// Clear our queue vectors, and our textures.
	// TODO: Do this...
}

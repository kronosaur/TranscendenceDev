#include "OpenGL.h"
#include "OpenGLInstancedBatchImpl.h"
#include "PreComp.h"
#include <mutex>

const float OpenGLMasterRenderQueue::m_fDepthDelta = 0.000001f; // Up to one million different depth levels
const float OpenGLMasterRenderQueue::m_fDepthStart = 0.999998f; // Up to one million different depth levels

namespace {

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
	// TODO: Replace m_p*Shader with unique_ptrs
	m_pGlowmapShader = new OpenGLShader("./shaders/glowmap_vertex_shader.glsl", "./shaders/glowmap_fragment_shader.glsl");
	m_pObjectTextureShader = new OpenGLShader("./shaders/instanced_vertex_shader.glsl", "./shaders/instanced_fragment_shader.glsl");
	m_pRayShader = new OpenGLShader("./shaders/ray_vertex_shader.glsl", "./shaders/ray_fragment_shader.glsl");
	m_pLightningShader = new OpenGLShader("./shaders/lightning_vertex_shader.glsl", "./shaders/lightning_fragment_shader.glsl");
	m_pActiveRenderLayer = &m_renderLayers[0];
#ifdef OPENGL_FPS_COUNTER_ENABLE
	m_pOpenGLIndicatorFont = std::make_unique<CG16bitFont>();
	m_pOpenGLIndicatorFont->Create(CONSTLIT("Arial"), -16);
#endif
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
	glm::vec2 vTexPositions((float)startPixelX / (float)texWidth, (float)startPixelY / (float)texHeight);
	glm::vec2 vSpriteSheetPositions((float)spriteSheetStartX / (float)texWidth, (float)spriteSheetStartY / (float)texHeight);
	glm::vec2 vCanvasQuadSizes((float)sizePixelX / (float)canvasWidth, (float)sizePixelY / (float)canvasHeight);
	glm::vec2 vCanvasPositions((float)posPixelX / (float)canvasWidth, (float)posPixelY / (float)canvasHeight);
	glm::vec2 vTextureQuadSizes((float)texQuadWidth / (float)texWidth, (float)texQuadHeight / (float)texHeight);
	glm::vec4 glowColor(glowR, glowG, glowB, glowA);

	// Initialize a glowmap tile request here, and save it in the MRQ. We consume this when we generate textures, to render glowmaps.
	image->requestGlowmapTile(vSpriteSheetPositions[0], vSpriteSheetPositions[1], float(numFramesPerRow * vTextureQuadSizes[0]), float(numFramesPerCol * vTextureQuadSizes[1]), vTextureQuadSizes[0], vTextureQuadSizes[1]);

	m_pActiveRenderLayer->addShipToRenderQueue(vTexPositions, vSpriteSheetPositions, vCanvasQuadSizes, vCanvasPositions, vTextureQuadSizes, glowColor, alphaStrength,
		glowNoise, numFramesPerRow, numFramesPerCol, image, m_fDepthLevel);
	m_fDepthLevel -= m_fDepthDelta;
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

	m_pActiveRenderLayer->addRayToEffectRenderQueue(vPrimaryColor, vSecondaryColor, sizeAndPosition, shapes, intensitiesAndCycles, styles, rotation, m_fDepthLevel);
	m_fDepthLevel -= m_fDepthDelta;
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

	m_pActiveRenderLayer->addLightningToEffectRenderQueue(vPrimaryColor, vSecondaryColor, sizeAndPosition, shapes, rotation, seed, m_fDepthLevel);
	m_fDepthLevel -= m_fDepthDelta;
}

void OpenGLMasterRenderQueue::renderAllQueues(void)
{
	for (OpenGLRenderLayer &renderLayer : m_renderLayers) {
		renderLayer.renderAllQueues(m_fDepthLevel, m_fDepthDelta, m_iCurrentTick, glm::ivec2(m_iCanvasWidth, m_iCanvasHeight), m_pObjectTextureShader,
			m_pRayShader, m_pLightningShader, m_pGlowmapShader, fbo, m_pCanvasVAO);
	}
	for (OpenGLRenderLayer &renderLayer : m_renderLayers) {
		renderLayer.GenerateGlowmaps(fbo, m_pCanvasVAO, m_pGlowmapShader);
	}
	m_fDepthLevel = m_fDepthStart - m_fDepthDelta;
}

void OpenGLMasterRenderQueue::clear(void)
{
	// Clear our queue vectors, and our textures.
	// TODO: Do this...
}

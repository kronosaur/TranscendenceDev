#include "OpenGL.h"
#include "PreComp.h"

const float OpenGLMasterRenderQueue::m_fDepthDelta = 0.000001f; // Up to one million different depth levels
const float OpenGLMasterRenderQueue::m_fDepthStart = 0.999998f; // Up to one million different depth levels

OpenGLMasterRenderQueue::OpenGLMasterRenderQueue(void)
{
	// Initialize the VAO.
	initializeVAO();
	// Depth level starts at one minus the delta.
	m_fDepthLevel = m_fDepthStart - m_fDepthDelta;
	// Initialize the FBO.
	glGenFramebuffers(1, &fbo);
	glGenRenderbuffers(1, &rbo);
	m_pGlowmapShader = new Shader("./shaders/glowmap_vertex_shader.glsl", "./shaders/glowmap_fragment_shader.glsl"); // TODO: fix
	initializeCanvasVAO();
}

OpenGLMasterRenderQueue::~OpenGLMasterRenderQueue(void)
{
	clear();
	deinitVAO();
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);
	delete m_pGlowmapShader;
}

void OpenGLMasterRenderQueue::initializeCanvasVAO()
{
	// Prepare the background canvas.
	//Shader* pTestShader = new Shader("./shaders/test_vertex_shader.glsl", "./shaders/test_fragment_shader.glsl");

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

void OpenGLMasterRenderQueue::deinitVAO(void)
{
	// TODO(heliogenesis): Move this VAO to the parent class once it's done
	unsigned int *instancedVBO = m_pVao->getinstancedVBO();
	glDeleteBuffers(16, &instancedVBO[0]);
	delete[] m_pVao;
}

void OpenGLMasterRenderQueue::addShipToRenderQueue(int startPixelX, int startPixelY, int sizePixelX, int sizePixelY, int posPixelX,
	int posPixelY, int canvasHeight, int canvasWidth, GLvoid *image, int texWidth, int texHeight, int texQuadWidth, int texQuadHeight,
	float alphaStrength, float glowR, float glowG, float glowB, float glowA, float glowNoise)
	{
	// Note, image is a pointer to the CG32bitPixel* we want to use as a texture. We can use CG32bitPixel->GetPixelArray() to get this pointer.
	// To get the width and height, we can use pSource->GetWidth() and pSource->GetHeight() respectively.

	// Check to see if we've rendered that texture before.
	if (!m_textures.count(image))
		{
		// If not, then add that texture to the list on our master render queue.
		OpenGLTexture *pTextureToUse = new OpenGLTexture(image, texWidth, texHeight);
		pTextureToUse->GenerateGlowMap(fbo, m_pCanvasVAO, m_pGlowmapShader, glm::vec2(float(texQuadWidth), float(texQuadHeight)));
		m_textures[image] = pTextureToUse;
		}

	// Check to see if we have a render queue with that texture already loaded.
	if (!m_shipRenderQueues.count(m_textures[image]))
		{
		OpenGLTexture *pTextureToUse = m_textures[image];
		// If we don't have a render queue with that texture loaded, then add one.
		m_shipRenderQueues[pTextureToUse] = new OpenGLInstancedRenderQueue();
		}
	// Add this quad to the render queue.
	glm::vec4 glow(glowR, glowG, glowB, glowA);
	m_shipRenderQueues[m_textures[image]]->addObjToRender(startPixelX, startPixelY, sizePixelX, sizePixelY, posPixelX, posPixelY, canvasHeight, canvasWidth, texHeight, texWidth, texQuadWidth, texQuadHeight, alphaStrength, glow, glowNoise);
	}

void OpenGLMasterRenderQueue::renderAllQueues(void)
{
	// For each render queue in the ships render queue, render that render queue. We need to set the texture and do a glBindTexture before doing so.
	for (const auto &p : m_shipRenderQueues)
	{
		OpenGLTexture *pTextureToUse = p.first;
		OpenGLInstancedRenderQueue *pInstancedRenderQueue = p.second;
		// TODO: Set the depths here before rendering. This will ensure that we always render from back to front, which should solve most issues with blending.
		float depthLevel = m_fDepthLevel;
		pInstancedRenderQueue->Render(m_pObjectTextureShader, m_pVao, pTextureToUse, depthLevel, m_fDepthDelta, m_iCurrentTick);
		m_fDepthLevel = depthLevel;
	}
	// Reset the depth level.
	m_fDepthLevel = m_fDepthStart - m_fDepthDelta;
}

void OpenGLMasterRenderQueue::initializeVAO(void)
{
	// First, we need to initialize the quad's vertices. Create a single VAO that will
	// be the basis for all of our quads rendered using this queue.
	// TODO(heliogenesis): Allow passing in of the texture for loading. Maybe move
	// this VAO to the parent class once it's done?
	float fSize = 0.5f;
	float posZ = 0.9f; // TODO(heliogenesis): Fix this

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

	std::vector<unsigned int> indices{
		0, 1, 3,
		1, 2, 3
	};

	std::vector<std::vector<float>> vbos{ vertices };
	std::vector<std::vector<unsigned int>> ebos{ indices };

	m_pVao = new OpenGLVAO(vbos, ebos);
	unsigned int iVAOID = m_pVao->getVAO()[0];
	unsigned int *instancedVBO = m_pVao->getinstancedVBO();
	glBindVertexArray(iVAOID);
	glGenBuffers(16, &instancedVBO[0]);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[0]);
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[1]);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[2]);
	glVertexAttribPointer((GLuint)3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[3]);
	glVertexAttribPointer((GLuint)4, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(5);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[4]);
	glVertexAttribPointer((GLuint)5, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(6);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[5]);
	glVertexAttribPointer((GLuint)6, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(7);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[6]);
	glVertexAttribPointer((GLuint)7, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(8);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[7]);
	glVertexAttribPointer((GLuint)8, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	// Ones below these lines are placeholders...
	glEnableVertexAttribArray(9);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[8]);
	glVertexAttribPointer((GLuint)9, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(10);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[9]);
	glVertexAttribPointer((GLuint)10, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(11);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[10]);
	glVertexAttribPointer((GLuint)11, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(12);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[11]);
	glVertexAttribPointer((GLuint)12, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(13);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[12]);
	glVertexAttribPointer((GLuint)13, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(14);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[13]);
	glVertexAttribPointer((GLuint)14, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(15);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[14]);
	glVertexAttribPointer((GLuint)15, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);
	glVertexAttribDivisor(10, 1);
	glVertexAttribDivisor(11, 1);
	glVertexAttribDivisor(12, 1);
	glVertexAttribDivisor(13, 1);
	glVertexAttribDivisor(14, 1);
	glVertexAttribDivisor(15, 1);
	glBindVertexArray(0);
}

void OpenGLMasterRenderQueue::clear(void)
{
	// Clear our queue vectors, and our textures.
	// TODO: Do this...
}

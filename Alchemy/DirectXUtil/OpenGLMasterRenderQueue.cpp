#include "OpenGL.h"
#include "PreComp.h"

OpenGLMasterRenderQueue::OpenGLMasterRenderQueue(void)
{
	// Initialize the VAO.
	initializeVAO();
}

OpenGLMasterRenderQueue::~OpenGLMasterRenderQueue(void)
{
	clear();
	deinitVAO();
}

void OpenGLMasterRenderQueue::deinitVAO(void)
{
	// TODO(heliogenesis): Move this VAO to the parent class once it's done
	unsigned int *instancedVBO = m_pVao->getinstancedVBO();
	glDeleteBuffers(4, &instancedVBO[0]);
	delete[] m_pVao;
}

void OpenGLMasterRenderQueue::addShipToRenderQueue(int startPixelX, int startPixelY, int sizePixelX, int sizePixelY, int posPixelX, int posPixelY, int canvasHeight, int canvasWidth, GLvoid *image, int texWidth, int texHeight)
	{
	// Note, image is a pointer to the CG32bitPixel* we want to use as a texture. We can use CG32bitPixel->GetPixelArray() to get this pointer.
	// To get the width and height, we can use pSource->GetWidth() and pSource->GetHeight() respectively.

	// Check to see if we've rendered that texture before.
	if (!m_textures.count(image))
		{
		// If not, then add that texture to the list on our master render queue.
		OpenGLTexture *pTextureToUse = new OpenGLTexture(image, texWidth, texHeight);
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
	m_shipRenderQueues[m_textures[image]]->addObjToRender(startPixelX, startPixelY, sizePixelX, sizePixelY, posPixelX, posPixelY, canvasHeight, canvasWidth, texHeight, texWidth);
	}

void OpenGLMasterRenderQueue::renderAllQueues(void)
{
	// For each render queue in the ships render queue, render that render queue. We need to set the texture and do a glBindTexture before doing so.
	for (const auto &p : m_shipRenderQueues)
	{
		OpenGLTexture *pTextureToUse = p.first;
		OpenGLInstancedRenderQueue *pInstancedRenderQueue = p.second;
		pInstancedRenderQueue->Render(m_pObjectTextureShader, m_pVao, pTextureToUse);
	}
}

void OpenGLMasterRenderQueue::initializeVAO(void)
{
	// First, we need to initialize the quad's vertices. Create a single VAO that will
	// be the basis for all of our quads rendered using this queue.
	// TODO(heliogenesis): Allow passing in of the texture for loading. Maybe move
	// this VAO to the parent class once it's done?
	float fSize = 0.5f;
	float posZ = 0.0f; // TODO(heliogenesis): Fix this

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
	glGenBuffers(4, &instancedVBO[0]);
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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glBindVertexArray(0);
}

void OpenGLMasterRenderQueue::clear(void)
{
	// Clear our queue vectors, and our textures.
	// TODO: Do this...
}

#include "OpenGL.h"
#include "PreComp.h"

OpenGLInstancedRenderQueue::OpenGLInstancedRenderQueue(void) 
	{
	initializeVAO();
	}

OpenGLInstancedRenderQueue::~OpenGLInstancedRenderQueue(void)
	{
	clear();
	deinitVAO();
	}

void OpenGLInstancedRenderQueue::deinitVAO(void)
	{
	// TODO(heliogenesis): Move this VAO to the parent class once it's done
	unsigned int *instancedVBO = vao->getinstancedVBO();
	glDeleteBuffers(3, &instancedVBO[0]);
	delete [] vao;
	}

void OpenGLInstancedRenderQueue::initializeVAO(void)
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

	std::vector<std::vector<float>> vbos{ vertices, colors };
	std::vector<std::vector<unsigned int>> ebos{ indices, indices };

	vao = new OpenGLVAO(vbos, ebos);
	unsigned int iVAOID = vao->getVAO()[0];
	unsigned int *instancedVBO = vao->getinstancedVBO();
	glBindVertexArray(iVAOID);
	glGenBuffers(3, &instancedVBO[0]);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[0]);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[1]);
	glVertexAttribPointer((GLuint)3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[2]);
	glVertexAttribPointer((GLuint)4, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glBindVertexArray(0);
	
	}

void OpenGLInstancedRenderQueue::Render(Shader *shader, OpenGLVAO *vao)
	{
	if (m_iNumObjectsToRender > 0)
		{
		unsigned int iVAOID = vao->getVAO()[0];
		unsigned int *instancedVBO = vao->getinstancedVBO();
		glBindVertexArray(iVAOID);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_iNumObjectsToRender, &m_texturePositionsFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_iNumObjectsToRender, &m_quadSizesFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_iNumObjectsToRender, &m_canvasPositionsFloat[0], GL_STATIC_DRAW);
		shader->bind();
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_iNumObjectsToRender);
		shader->unbind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		clear();
		}
	
	}

void OpenGLInstancedRenderQueue::clear(void)
	{
	m_texturePositionsFloat.clear();
	m_quadSizesFloat.clear();
	m_canvasPositionsFloat.clear();
	m_iNumObjectsToRender = 0;
	}

void OpenGLInstancedRenderQueue::addObjToRender(int startPixelX, int startPixelY,
	int sizePixelX, int sizePixelY, int posPixelX, int posPixelY, int canvasHeight,
	int canvasWidth)
	{
	glm::vec2 texPos((float)startPixelX / (float)canvasHeight,
		(float)startPixelY / (float)canvasWidth);
	glm::vec2 size((float)sizePixelX / (float)canvasHeight,
		(float)sizePixelY / (float)canvasWidth);
	glm::vec2 canvasPos((float)posPixelX / (float)canvasHeight,
		(float)posPixelY / (float)canvasWidth);

	m_texturePositionsFloat.insert(m_texturePositionsFloat.begin(), texPos);
	m_quadSizesFloat.insert(m_quadSizesFloat.begin(), size);
	m_canvasPositionsFloat.insert(m_canvasPositionsFloat.begin(), canvasPos);
	m_iNumObjectsToRender++;
	}
#include "OpenGL.h"
#include "PreComp.h"

OpenGLInstancedLightningRenderQueue::OpenGLInstancedLightningRenderQueue(void)
{
}

OpenGLInstancedLightningRenderQueue::~OpenGLInstancedLightningRenderQueue(void)
{
	clear();
}

// TODO: Add a RenderLine function. Maybe even rename to LineRenderQueue.

void OpenGLInstancedLightningRenderQueue::Render(OpenGLShader *shader, OpenGLVAO *quad, float &startingDepth, float incDepth, int currentTick)
{
	if (m_iNumObjectsToRender > 0)
	{
		for (int i = 0; i < m_iNumObjectsToRender; i++)
		{
			m_depthsFloat.insert(m_depthsFloat.end(), startingDepth);
			startingDepth -= incDepth;
		}
		unsigned int iVAOID = quad->getVAO()[0];
		unsigned int *instancedVBO = quad->getinstancedVBO();
		glBindVertexArray(iVAOID);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m_iNumObjectsToRender, &m_quadSizesAndCanvasPositionsFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_iNumObjectsToRender, &m_rotationsFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_iNumObjectsToRender, &m_depthsFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::ivec2) * m_iNumObjectsToRender, &m_shapesInt[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_iNumObjectsToRender, &m_seedsFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[5]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_iNumObjectsToRender, &m_primaryColorsFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[6]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_iNumObjectsToRender, &m_secondaryColorsFloat[0], GL_STATIC_DRAW);
		shader->bind();
		glUniform1f(glGetUniformLocation(shader->id(), "current_tick"), float(currentTick));
		glUniform2f(glGetUniformLocation(shader->id(), "aCanvasAdjustedDimensions"), float(m_iCanvasWidth), float(m_iCanvasHeight));
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_iNumObjectsToRender);
		shader->unbind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	clear();
}


void OpenGLInstancedLightningRenderQueue::clear(void)
{
	m_quadSizesAndCanvasPositionsFloat.clear();
	m_rotationsFloat.clear();
	m_shapesInt.clear();
	m_seedsFloat.clear();
	m_primaryColorsFloat.clear();
	m_secondaryColorsFloat.clear();
	m_depthsFloat.clear();
	m_iNumObjectsToRender = 0;
}

void OpenGLInstancedLightningRenderQueue::addObjToRender(int sizePixelX, int sizePixelY, int posPixelX, int posPixelY, int canvasHeight, int canvasWidth, float rotation, int iWidthAdjType, int iReshape,
	glm::vec3 primaryColor, glm::vec3 secondaryColor, float seed)
{
	glm::vec4 sizeAndPosition((float)sizePixelX, (float)sizePixelY,
		(float)posPixelX / (float)canvasWidth, (float)posPixelY / (float)canvasHeight);
	glm::ivec2 shapes(iWidthAdjType, iReshape);

	m_quadSizesAndCanvasPositionsFloat.insert(m_quadSizesAndCanvasPositionsFloat.end(), sizeAndPosition);
	m_rotationsFloat.insert(m_rotationsFloat.end(), rotation);
	m_shapesInt.insert(m_shapesInt.end(), shapes);
	m_seedsFloat.insert(m_seedsFloat.end(), seed);
	m_primaryColorsFloat.insert(m_primaryColorsFloat.end(), primaryColor);
	m_secondaryColorsFloat.insert(m_secondaryColorsFloat.end(), secondaryColor);
	m_iCanvasHeight = canvasHeight;
	m_iCanvasWidth = canvasWidth;

	m_iNumObjectsToRender++;
}

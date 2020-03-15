#include "OpenGL.h"
#include "PreComp.h"

OpenGLInstancedRenderQueue::OpenGLInstancedRenderQueue(void) 
	{
	}

OpenGLInstancedRenderQueue::~OpenGLInstancedRenderQueue(void)
	{
	clear();
	}

void OpenGLInstancedRenderQueue::RenderNonInstanced(OpenGLShader *shader, OpenGLVAO *quad, OpenGLTexture *texture)
	{
	// TODO(heliogenesis): Allow usage of an array of textures.
	//glUniform1i(glGetUniformLocation(shader->id(), "obj_texture"), 0);
	//texture->bindTexture2D(GL_TEXTURE0);
	for (int i = 0; i < m_iNumObjectsToRender; i++)
	{
		unsigned int iVAOID = quad->getVAO()[0];
		unsigned int *instancedVBO = quad->getinstancedVBO();
		glBindVertexArray(iVAOID);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2), &m_texturePositionsFloat[i], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2), &m_quadSizesFloat[i], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2), &m_canvasPositionsFloat[i], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2), &m_textureSizesFloat[i], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float), &m_alphaStrengthsFloat[i], GL_DYNAMIC_DRAW);
		shader->bind();
		glUniform1i(glGetUniformLocation(shader->id(), "obj_texture"), 0);
		texture->getGlowMap()->bindTexture2D(GL_TEXTURE0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		shader->unbind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	clear();
	}

void OpenGLInstancedRenderQueue::PopulateGlowMaps(OpenGLTexture *texture, unsigned int fbo, OpenGLVAO* vao, OpenGLShader* glowmapShader, int texQuadWidth, int texQuadHeight) {
	// Initialize the texture if necessary; we do this here because all OpenGL calls must be made on the same thread
	texture->initTextureFromOpenGLThread();

	// Generate a glow map for this texture if needed.
	// TODO: Store the texture quad width and height on the OpenGLTexture object temporarily, so glowmap can be created later
	if (!texture->getGlowMap()) {
		texture->GenerateGlowMap(fbo, vao, glowmapShader, glm::vec2(float(texQuadWidth), float(texQuadHeight)));
	}
}

void OpenGLInstancedRenderQueue::Render(OpenGLShader *shader, OpenGLVAO *quad, OpenGLTexture *texture, float &startingDepth, float incDepth, int currentTick)
	{
	// TODO(heliogenesis): Allow usage of an array of textures.
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_iNumObjectsToRender, &m_texturePositionsFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_iNumObjectsToRender, &m_quadSizesFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_iNumObjectsToRender, &m_canvasPositionsFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_iNumObjectsToRender, &m_textureSizesFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[4]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_iNumObjectsToRender, &m_alphaStrengthsFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[5]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_iNumObjectsToRender, &m_depthsFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[6]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * m_iNumObjectsToRender, &m_glowColorFloat[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[7]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_iNumObjectsToRender, &m_glowNoiseFactorFloat[0], GL_STATIC_DRAW);
		shader->bind();
		glUniform1i(glGetUniformLocation(shader->id(), "obj_texture"), 0);
		glUniform1i(glGetUniformLocation(shader->id(), "glow_map"), 1);
		glUniform1i(glGetUniformLocation(shader->id(), "current_tick"), currentTick);
		texture->bindTexture2D(GL_TEXTURE0);
		// TODO: Maybe attach the glowmap population function to a new function that replaces the below line?
		// Instead of calling texture->getGlowMap()->bindTexture2D(), we call texture->bindGlowMap() which will initialize and populate the glowmap
		// if needed
		texture->getGlowMap()->bindTexture2D(GL_TEXTURE1);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_iNumObjectsToRender);
		shader->unbind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		}
	clear();
	}
	

void OpenGLInstancedRenderQueue::clear(void)
	{
	m_texturePositionsFloat.clear();
	m_quadSizesFloat.clear();
	m_canvasPositionsFloat.clear();
	m_textureSizesFloat.clear();
	m_alphaStrengthsFloat.clear();
	m_glowColorFloat.clear();
	m_glowNoiseFactorFloat.clear();
	m_depthsFloat.clear();
	m_iNumObjectsToRender = 0;
	}

void OpenGLInstancedRenderQueue::addObjToRender(int startPixelX, int startPixelY,
	int sizePixelX, int sizePixelY, int posPixelX, int posPixelY, int canvasHeight,
	int canvasWidth, int texHeight, int texWidth, int texQuadWidth, int texQuadHeight, float alphaStrength,
	glm::vec4 glow, float glowNoise)
	{
	glm::vec2 texPos((float)startPixelX / (float)texWidth,
		(float)startPixelY / (float)texHeight);
	glm::vec2 texSize((float)texQuadWidth / (float)texWidth,
		(float)texQuadHeight / (float)texHeight);
	glm::vec2 size((float)sizePixelX / (float)canvasWidth,
		(float)sizePixelY / (float)canvasHeight);
	glm::vec2 canvasPos((float)posPixelX / (float)canvasWidth,
		(float)posPixelY / (float)canvasHeight);
	//glm::vec2 texSize((float)texWidth, (float)texHeight);

	m_texturePositionsFloat.insert(m_texturePositionsFloat.end(), texPos);
	m_quadSizesFloat.insert(m_quadSizesFloat.end(), size);
	m_canvasPositionsFloat.insert(m_canvasPositionsFloat.end(), canvasPos);
	m_textureSizesFloat.insert(m_textureSizesFloat.end(), texSize);
	m_alphaStrengthsFloat.insert(m_alphaStrengthsFloat.end(), alphaStrength);
	m_glowColorFloat.insert(m_glowColorFloat.end(), glow);
	m_glowNoiseFactorFloat.insert(m_glowNoiseFactorFloat.end(), glowNoise);
	//m_depthsFloat.insert(m_depthsFloat.end(), depth);
	m_iNumObjectsToRender++;
	}

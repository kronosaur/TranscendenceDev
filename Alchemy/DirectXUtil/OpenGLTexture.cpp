#include "OpenGL.h"
#include "PreComp.h"

OpenGLTexture::OpenGLTexture(int width, int height)
{
	m_iHeight = height;
	m_iWidth = width;
	m_isOpaque = false;
	m_pTextureID[0] = 0;
	pboID[0] = 0;
	pboID[1] = 0;
}

void OpenGLTexture::initTexture2D(int width, int height)
{

	int iNumOfChannels = 4;
	int iDataSize = width * height * iNumOfChannels;
	glGetInternalformativ(GL_TEXTURE_2D, GL_RGBA8, GL_TEXTURE_IMAGE_FORMAT, 1, &m_pixelFormat);
	glGetInternalformativ(GL_TEXTURE_2D, GL_RGBA8, GL_TEXTURE_IMAGE_TYPE, 1, &m_pixelType);

	glGenBuffers(2, &pboID[0]);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[0]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, iDataSize, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, iDataSize, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glGenTextures(1, &m_pTextureID[0]);
	glBindTexture(GL_TEXTURE_2D, m_pTextureID[0]);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texture);
	// Using glTexStorage2D + glTexSubImage2D gives acceptable performance unlike initialization with glTexImage2D. Not sure why.
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texture);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_iHeight = height;
	m_iWidth = width;
}

OpenGLTexture::OpenGLTexture (void* texture, int width, int height, bool isOpaque)
	{
	m_iHeight = height;
	m_iWidth = width;
	m_pTextureToInitFrom = texture;
	m_isOpaque = isOpaque;
	m_pTextureID[0] = 0;
	pboID[0] = 0;
	pboID[1] = 0;
	}

void OpenGLTexture::initTexture2D (GLvoid* texture, int width, int height)
	{
	int iNumOfChannels = 4;
	int iDataSize = width * height * iNumOfChannels;
	glGetInternalformativ(GL_TEXTURE_2D, GL_RGBA8, GL_TEXTURE_IMAGE_FORMAT, 1, &m_pixelFormat);
	glGetInternalformativ(GL_TEXTURE_2D, GL_RGBA8, GL_TEXTURE_IMAGE_TYPE, 1, &m_pixelType);

	glGenBuffers(2, &pboID[0]);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[0]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, iDataSize, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, iDataSize, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glGenTextures(1, &m_pTextureID[0]);
	glBindTexture(GL_TEXTURE_2D, m_pTextureID[0]);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texture);
	// Using glTexStorage2D + glTexSubImage2D gives acceptable performance unlike initialization with glTexImage2D. Not sure why.
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texture);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_iHeight = height;
	m_iWidth = width;
	}

void OpenGLTexture::initTextureFromOpenGLThread ()
	{
	if (!m_bIsInited) {
		if (m_pTextureToInitFrom) {
			initTexture2D(m_pTextureToInitFrom, m_iWidth, m_iHeight);
			m_pTextureToInitFrom = nullptr;
		}
		else {
			initTexture2D(m_iWidth, m_iHeight);
		}
		m_bIsInited = true;
	}
	}

void OpenGLTexture::updateTexture2D (GLvoid* texture, int width, int height)
	{
	int iNumOfChannels = 4;
	static int iPBOInd = 0;
	int iPBOIndPlus1 = 0;
	int iDataSize = width * height * iNumOfChannels;

	// Code adapted from Song Ho Ahn's PBO test code, found at http://www.songho.ca/opengl/gl_pbo.html
	iPBOInd = (iPBOInd + 1) % 2;
	iPBOIndPlus1 = (iPBOInd + 1) % 2;
	glBindTexture(GL_TEXTURE_2D, m_pTextureID[0]);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[iPBOInd]);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[iPBOIndPlus1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, iDataSize, 0, GL_STREAM_DRAW);
	GLubyte* pMappedBuffer = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	if (pMappedBuffer)
	{
		memcpy(pMappedBuffer, texture, iDataSize);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	}

OpenGLTexture::~OpenGLTexture ()
	{
	auto t1 = &m_pTextureID[0];
	auto t2 = &pboID[0];
	if (pboID[0] != 0 && pboID[1] != 0) {
		glDeleteBuffers(2, &pboID[0]);
	}
	if (m_pTextureID[0] != 0) {
		glDeleteTextures(1, &m_pTextureID[0]);
	}
	m_iHeight = 0;
	m_iWidth = 0;
	}

OpenGLTexture* OpenGLTexture::GenerateGlowMap (unsigned int fbo, OpenGLVAO* vao, OpenGLShader* shader, glm::vec2 texQuadSize)
	{
	// Generate a glow map. Kernel is a multivariate gaussian.
	// Vertical pass
	if (m_iWidth > 0 && m_iHeight > 0)
	{
		OpenGLTexture pTempTexture = OpenGLTexture(m_iWidth, m_iHeight);
		pTempTexture.initTextureFromOpenGLThread();
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTempTexture.getTexture()[0], 0);
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_iWidth, m_iHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			::kernelDebugLogPattern("[OpenGL] Framebuffer is not complete p1");
		// Render to the new texture
		glViewport(0, 0, m_iWidth, m_iHeight); // Set the viewport size to fill the window
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader->bind();
		glm::mat4 rotationMatrix = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(0.0, 0.0, 0.0, 1.0));
		int rotationMatrixLocation = glGetUniformLocation(shader->id(), "rotationMatrix");
		glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
		glUniform1i(glGetUniformLocation(shader->id(), "ourTexture"), 0);
		glUniform2f(glGetUniformLocation(shader->id(), "aTexStartPoint"), 600.0f, 600.0f);
		glUniform2f(glGetUniformLocation(shader->id(), "aTexQuadSizes"), texQuadSize[0], texQuadSize[1]);
		glUniform2i(glGetUniformLocation(shader->id(), "aTexTotalSize"), m_iWidth, m_iHeight);
		glUniform1i(glGetUniformLocation(shader->id(), "kernelSize"), std::min(25, std::max(3, int(std::min(texQuadSize[0], texQuadSize[1]) / 10))));
		glUniform1i(glGetUniformLocation(shader->id(), "use_x_axis"), GL_TRUE);
		glUniform1i(glGetUniformLocation(shader->id(), "second_pass"), GL_FALSE);

		this->bindTexture2D(GL_TEXTURE0);
		glBindVertexArray((vao->getVAO())[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Second pass
		m_pGlowMap = std::make_unique<OpenGLTexture>(m_iWidth, m_iHeight);
		m_pGlowMap.get()->initTextureFromOpenGLThread();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pGlowMap->getTexture()[0], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			::kernelDebugLogPattern("[OpenGL] Framebuffer is not complete p2");
		// Render to the new texture
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(shader->id(), "use_x_axis"), GL_FALSE);
		glUniform1i(glGetUniformLocation(shader->id(), "second_pass"), GL_TRUE);

		pTempTexture.bindTexture2D(GL_TEXTURE0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Clean up
		glBindVertexArray(0); // Unbind our Vertex Array Object
		this->unbindTexture2D();
		shader->unbind(); // Unbind our shader
		// Unbind the frame buffer and delete our rbo
		glDeleteRenderbuffers(1, &rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return m_pGlowMap.get();
	}
	else {
		return nullptr;
	}

	}


OpenGLTexture* OpenGLTexture::GenerateGlowMap(unsigned int fbo, OpenGLVAO* vao, OpenGLShader* shader, const glm::vec2 texQuadSize, const glm::vec2 texStartPoint, const glm::vec2 texGridSize)
{
	// Generate a glow map. Kernel is a multivariate gaussian.

	// TODO: Wrap this function in a for loop and make it private; the for loop should iterate through m_GlowmapTilesToRender and move completed ones to the completed queue
	if (!m_pGlowMap) {
		m_pGlowMap = std::make_unique<OpenGLTexture>(m_iWidth, m_iHeight);
		m_pGlowMap.get()->initTextureFromOpenGLThread();
	}
	if (m_iWidth > 0 && m_iHeight > 0)
	{
		// TODO: Instead of what we are doing right now, we should render the entire texture in one go with instanced rendering in two passes;
		// that will have much better performance. What we can do is make it so that we paint one quad onto the texture, using the current vertex
		// shader, but we do subdivision (gridding) in the fragment shader stage. We need to supply the quad size there.
		// Vertical pass
		// First, create a texture with the same size as the output.
		OpenGLTexture pTempTexture = OpenGLTexture(m_iWidth, m_iHeight);
		pTempTexture.initTextureFromOpenGLThread();
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTempTexture.getTexture()[0], 0);
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_iWidth, m_iHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			::kernelDebugLogPattern("[OpenGL] Framebuffer is not complete p1");
		// Render to the new texture
		glViewport(0, 0, m_iWidth, m_iHeight); // Set the viewport size to fill the window
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader->bind();
		glm::mat4 rotationMatrix = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(0.0, 0.0, 0.0, 1.0));
		int rotationMatrixLocation = glGetUniformLocation(shader->id(), "rotationMatrix");

		float texStartPoint_x = texStartPoint[0];
		float texStartPoint_y = texStartPoint[1];
		float texQuadSize_x = texQuadSize[0];
		float texQuadSize_y = texQuadSize[1];
		float texGridSize_x = texGridSize[0];
		float texGridSize_y = texGridSize[1];
		::kernelDebugLogPattern("[OpenGL] size=%d x %d, pos=%d, %d", int(texQuadSize_x), int(texQuadSize_y), int(texStartPoint_x), int(texStartPoint_y));

		glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
		glUniform1i(glGetUniformLocation(shader->id(), "ourTexture"), 0);
		glUniform2f(glGetUniformLocation(shader->id(), "aTexStartPoint"), texStartPoint_x, texStartPoint_y);
		glUniform2f(glGetUniformLocation(shader->id(), "aTexQuadSizes"), texQuadSize_x, texQuadSize_y);
		glUniform2f(glGetUniformLocation(shader->id(), "gridSquareSize"), texGridSize_x, texGridSize_y);
		glUniform1i(glGetUniformLocation(shader->id(), "kernelSize"), std::min(25, std::max(3, int(std::min(texQuadSize_x * m_iWidth, texQuadSize_y * m_iHeight) / 10))));
		glUniform1i(glGetUniformLocation(shader->id(), "use_x_axis"), GL_TRUE);
		glUniform1i(glGetUniformLocation(shader->id(), "second_pass"), GL_FALSE);

		this->bindTexture2D(GL_TEXTURE0);
		glBindVertexArray((vao->getVAO())[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Second pass
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pGlowMap->getTexture()[0], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			::kernelDebugLogPattern("[OpenGL] Framebuffer is not complete p2");
		// Render to the new texture
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(glGetUniformLocation(shader->id(), "use_x_axis"), GL_FALSE);
		glUniform1i(glGetUniformLocation(shader->id(), "second_pass"), GL_TRUE);

		pTempTexture.bindTexture2D(GL_TEXTURE0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Clean up
		glBindVertexArray(0); // Unbind our Vertex Array Object
		this->unbindTexture2D();
		shader->unbind(); // Unbind our shader
		// Unbind the frame buffer and delete our rbo
		glDeleteRenderbuffers(1, &rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return m_pGlowMap.get();
	}
	else {
		return nullptr;
	}

}


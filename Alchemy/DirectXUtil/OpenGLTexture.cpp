#include "OpenGL.h"
#include "PreComp.h"

OpenGLTexture::OpenGLTexture(int width, int height)
{
	initTexture2D(width, height);
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

OpenGLTexture::OpenGLTexture (void* texture, int width, int height)
	{
	initTexture2D(texture, width, height);
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

	//glBindTexture(GL_TEXTURE_2D, m_pTextureID[0]);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture);
	//glFinish();
	//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, texture);
	}

OpenGLTexture::~OpenGLTexture ()
	{
	glDeleteTextures(1, &m_pTextureID[0]);
	glDeleteBuffers(2, &pboID[0]);
	}

OpenGLTexture* OpenGLTexture::GenerateGlowMap (unsigned int fbo, OpenGLVAO* vao, Shader* shader, glm::vec2 texQuadSize)
	{
	// Generate a glow map. Kernel is a multivariate gaussian.
	// Vertical pass
	OpenGLTexture pTempTexture = OpenGLTexture(m_iWidth, m_iHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTempTexture.getTexture()[0], 0);
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_iWidth, m_iHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		::kernelDebugLogPattern("[OpenGL] Framebuffer is not complete");
	// Render to the new texture
	glViewport(0, 0, m_iWidth, m_iHeight); // Set the viewport size to fill the window
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader->bind();
	glm::mat4 rotationMatrix = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(0.0, 0.0, 0.0, 1.0));
	int rotationMatrixLocation = glGetUniformLocation(shader->id(), "rotationMatrix");
	glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
	glUniform1i(glGetUniformLocation(shader->id(), "ourTexture"), 0);
	glUniform2f(glGetUniformLocation(shader->id(), "quadSize"), texQuadSize[0], texQuadSize[1]);
	glUniform1i(glGetUniformLocation(shader->id(), "kernelSize"), std::max(3, int(std::min(texQuadSize[0], texQuadSize[1]) / 10)));
	glUniform1i(glGetUniformLocation(shader->id(), "use_x_axis"), GL_TRUE);
	glUniform1i(glGetUniformLocation(shader->id(), "second_pass"), GL_FALSE);

	this->bindTexture2D(GL_TEXTURE0);
	glBindVertexArray((vao->getVAO())[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Second pass
	m_pGlowMap = new OpenGLTexture(m_iWidth, m_iHeight);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pGlowMap->getTexture()[0], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		::kernelDebugLogPattern("[OpenGL] Framebuffer is not complete");
	// Render to the new texture
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	rotationMatrixLocation = glGetUniformLocation(shader->id(), "rotationMatrix");
	glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
	glUniform1i(glGetUniformLocation(shader->id(), "ourTexture"), 0);
	glUniform2f(glGetUniformLocation(shader->id(), "quadSize"), texQuadSize[0], texQuadSize[1]);
	glUniform1i(glGetUniformLocation(shader->id(), "kernelSize"), std::max(3, int(std::min(texQuadSize[0], texQuadSize[1]) / 10)));
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
	return m_pGlowMap;

	}



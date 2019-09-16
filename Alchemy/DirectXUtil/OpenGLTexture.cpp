#include "OpenGL.h"
#include "PreComp.h"

OpenGLTexture::OpenGLTexture(void* texture, int width, int height)
	{
	initTexture2D(texture, width, height);
	}

void OpenGLTexture::initTexture2D(GLvoid* texture, int width, int height)
	{

	int iNumOfChannels = 4;
	int iDataSize = width * height * iNumOfChannels;
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
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texture);
	// Using glTexStorage2D + glTexSubImage2D gives acceptable performance unlike initialization with glTexImage2D. Not sure why.
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, texture);
	// glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	::kernelDebugLogPattern("[OpenGL] Texture resolution: %d x %d", width, height);
	}

void OpenGLTexture::updateTexture2D(GLvoid* texture, int width, int height)
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

OpenGLTexture::~OpenGLTexture()
	{
	glDeleteTextures(1, &m_pTextureID[0]);
	glDeleteBuffers(2, &pboID[0]);
	}



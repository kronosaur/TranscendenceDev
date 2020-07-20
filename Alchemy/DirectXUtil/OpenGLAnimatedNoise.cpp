#pragma once
#include "OpenGL.h"
#include "OpenGLAnimatedNoise.h"
#include "PreComp.h"

OpenGLAnimatedNoise::OpenGLAnimatedNoise(int width, int height, int frames)
{
	m_iHeight = height;
	m_iWidth = width;
	m_iNumFrames = frames;
	m_isOpaque = false;
	m_pTextureID[0] = 0;
	pboID[0] = 0;
	pboID[1] = 0;
}

void OpenGLAnimatedNoise::initTexture3D(int width, int height, int frames)
{

	int iNumOfChannels = 4;
	int iDataSize = width * height * frames * iNumOfChannels;
	glGetInternalformativ(GL_TEXTURE_3D, GL_RGBA8, GL_TEXTURE_IMAGE_FORMAT, 1, &m_pixelFormat);
	glGetInternalformativ(GL_TEXTURE_3D, GL_RGBA8, GL_TEXTURE_IMAGE_TYPE, 1, &m_pixelType);

	glGenBuffers(2, &pboID[0]);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[0]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, iDataSize, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID[1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, iDataSize, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glGenTextures(1, &m_pTextureID[0]);
	glBindTexture(GL_TEXTURE_3D, m_pTextureID[0]);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Using glTexStorage2D + glTexSubImage2D gives acceptable performance unlike initialization with glTexImage2D. Not sure why.
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, width, height, frames);
	glBindTexture(GL_TEXTURE_3D, 0);
	m_iHeight = height;
	m_iWidth = width;
	m_iNumFrames = frames;
}

OpenGLAnimatedNoise::~OpenGLAnimatedNoise()
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

void OpenGLAnimatedNoise::populateTexture3D(unsigned int fbo, OpenGLVAO* vao, OpenGLShader* shader)
{
	// Generate a glow map. Kernel is a multivariate gaussian.
	if (m_iWidth > 0 && m_iHeight > 0 && m_iNumFrames > 0)
	{
		for (int i = 0; i < static_cast<int>(m_iNumFrames); i++) {
			// TODO: Do not create any temp textures; they aren't necessary. Render each slice of the 3D cube texture one by one.
		// See https://stackoverflow.com/questions/17504750/opengl-how-to-render-object-to-3d-texture-as-a-volumetric-billboard
			float currTime = static_cast<float>(abs(i - (static_cast<int>(m_iNumFrames) / 2))); // currTime is the distance of this frame from the 'center frame'; we use this to make it continuous
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, getTexture()[0], 0, i);
			unsigned int rbo;
			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_iWidth, m_iHeight);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				::kernelDebugLogPattern("[OpenGL] Framebuffer is not complete 3d");
			// Render to the new texture
			glViewport(0, 0, m_iWidth, m_iHeight); // Set the viewport size to fill the window
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader->bind();
			glm::mat4 rotationMatrix = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0), glm::vec4(0.0, 1.0, 0.0, 0.0), glm::vec4(0.0, 0.0, 1.0, 0.0), glm::vec4(0.0, 0.0, 0.0, 1.0));
			int rotationMatrixLocation = glGetUniformLocation(shader->id(), "rotationMatrix");


			glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, &rotationMatrix[0][0]);
			glUniform1f(glGetUniformLocation(shader->id(), "time"), currTime);

			//this->bindTexture3D(GL_TEXTURE0);
			glBindVertexArray((vao->getVAO())[0]);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			// Clean up
			glBindVertexArray(0); // Unbind our Vertex Array Object
			//this->unbindTexture3D();
			shader->unbind(); // Unbind our shader
			// Unbind the frame buffer and delete our rbo
			glDeleteRenderbuffers(1, &rbo);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}


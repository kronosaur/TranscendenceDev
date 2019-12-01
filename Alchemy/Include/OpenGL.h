#pragma once

/*
DESIGN DOC FOR OPENGL'ING TRANS!

-First, start with something simple. Say, the spaceObject PAINT function. Either enable batched painting for specific object classes on a class by class basis,
or modify the Image.PaintImage function (CObjectImageArray.cpp::1344) to blt to an OpenGL quad instead of the canvas. First is recommended, since that is ultimately what we want to do.

-We will need to come up with the following things:
1. Render queue to organize all the things we need to paint, and how to best paint them (i.e. instancing)
2. Function to replace CObjectImageArray::PaintImage

The simple and hacky way to go is to attach an OpenGL render queue object (which we'll have to define ourselves) to the CG32bitImage class. Note that the Dest for most spaceobject renderings is the same
as the canvas object we blt to a quad in the render phase, so we can use OpenGL to do batched or naive rendering as we see fit. Once we have the render queue object, we can then decide what to do with our
objects to render. Start with a simple naive render of 'load texture, render, load next texture'. In the Paint functions, we can generate and attach objects to the render queues.

For spaceobjects with image textures, we will probably need a custom OpenGLVAO object. Features we may need include: rotation index vao, vao specifying texture rows/columns, bool vao specifying whether to
go by row order or column order (or maybe use different shader for that), vao specifying texture size, 

Things needed per spobject:
-Start position pixel (supplied by function)
-Size of the ship object in pixels
-Position of the ship object in pixel coords
-Size of the screen in pixels
-Texture to use for the ship

The spobject render queue contains the following globals:
-Vector with order of textures to render
-Subqueue containing which texture to use for the ship 

*/

#include "OpenGLIncludes.h"
#include "OpenGLShader.h"
#include <vector>

class OpenGLInstancedRenderQueue {
	// Contains all the objects to render for a single given texture.
public:
	OpenGLInstancedRenderQueue (void) { }
	~OpenGLInstancedRenderQueue (void);
	void addObjToRender(int startPixelX, int startPixelY, int sizePixelX, int sizePixelY, int posPixelX, int posPixelY);
	void addObjToRender(float startFX, float startFY, float sizeFX, float sizeFY, float posFX, float posFY);
private:
	std::vector<glm::vec2> startPositionsFloat;
	std::vector<glm::vec2> quadSizesFloat;
	std::vector<glm::vec2> canvasPositionsFloat;
};
/*
class OpenGLMasterRenderQueue {
public:
	OpenGLTextureRenderQueue(void) { }
	~OpenGLTextureRenderQueue(void);
	void pushSpaceObjectOntoQueue();
private:

};
*/
class OpenGLTexture {
public:
	OpenGLTexture (void) { }
	OpenGLTexture (void* texture, int width, int height);
	~OpenGLTexture (void);
	void initTexture2D (void* texture, int width, int height);
	void bindTexture2D (GLenum glTexture) { glActiveTexture(glTexture); glBindTexture(GL_TEXTURE_2D, m_pTextureID[0]); }
	void unbindTexture2D (void) { glBindTexture(GL_TEXTURE_2D, 0); }
	void updateTexture2D (void* texture, int width, int height);
	unsigned int* getTexture (void) { return m_pTextureID; }

private:
	unsigned int m_pTextureID[1];
	unsigned int pboID[2];
};

class OpenGLVAO {
public:
	OpenGLVAO (void) { };
	OpenGLVAO (std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos);
	OpenGLVAO (std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos,
		std::vector<std::vector<float>> texcoords);
	~OpenGLVAO (void);
	void initVAO (std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos);
	void initVAO (std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos,
		std::vector<std::vector<float>> texcoords);
	void setShader (Shader* shader) { m_pShader = shader; }
	void addTexture2D (void* texture);
	void removeTexture ();
	Shader* getShader (void) { return m_pShader; }
	unsigned int* getVAO (void) { return vaoID; }

private:
	Shader *m_pShader;
	unsigned int m_iNumArrays;
	unsigned int m_iNumTexArrays;
	unsigned int vaoID[128];
	unsigned int vboID[128];
	unsigned int eboID[128];
};

class OpenGLContext {
public:
	OpenGLContext (void) { };
	OpenGLContext (HWND hwnd);
	~OpenGLContext (void) { };
	bool initOpenGL (HWND hwnd, HDC hdc);
	void setupQuads (void);
	void resize (int w, int h);
	void ackResize () { m_bResized = false; };
	bool pollResize () { return m_bResized; };
	void renderScene (void);
	void testRender ();
	void testShaders ();
	void testTextures (OpenGLTexture *texture);
	void prepSquareCanvas ();
	void prepTextureCanvas ();
	void prepTestScene ();
	void swapBuffers (HWND hwnd);
	void getWGLError ();
	void getWGLSwapError ();

private:
	int m_iWindowWidth;
	int m_iWindowHeight;
	bool m_bResized;

	Shader *m_pTestShader;

	// Projection, view and model matrices respectively
	glm::mat4 m_pMatrix;
	glm::mat4 m_vMatrix;
	glm::mat4 m_mMatrix;

	std::vector<OpenGLVAO*> vaos;
	std::vector<OpenGLTexture*> textures;

	unsigned int vaoID[1];
	unsigned int vboID[1];
	unsigned int eboID[1];

protected:
	HGLRC m_renderContext;
	HDC m_deviceContext;
	HWND m_windowID;
};

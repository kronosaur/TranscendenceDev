#include "OpenGLIncludes.h"
#include "OpenGLShader.h"
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <vector>


class OpenGLVAO {
public:
	OpenGLVAO(void) { };
	OpenGLVAO(std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos);
	OpenGLVAO(std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos,
		std::vector<std::vector<float>> texcoords);
	~OpenGLVAO(void);
	void initVAO(std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos);
	void initVAO(std::vector<std::vector<float>> vbos,
		std::vector<std::vector<unsigned int>> ebos,
		std::vector<std::vector<float>> texcoords);
	void setShader(OpenGLShader* shader) { m_pShader = shader; }
	void addTexture2D(void* texture);
	void removeTexture();
	OpenGLShader* getShader(void) { return m_pShader; }
	unsigned int* getVAO(void) { return vaoID; }
	unsigned int* getinstancedVBO(void) { return instancedVboID; }

private:
	OpenGLShader *m_pShader;
	unsigned int m_iNumArrays;
	unsigned int m_iNumTexArrays;
	unsigned int vaoID[128];
	unsigned int vboID[128];
	unsigned int eboID[128];
	unsigned int instancedVboID[128];
};

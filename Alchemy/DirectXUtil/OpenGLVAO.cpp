#include "OpenGL.h"
#include "PreComp.h"

OpenGLVAO::OpenGLVAO(std::vector<std::vector<float>> vbos,
	std::vector<std::vector<unsigned int>> ebos) 
	{
	initVAO(vbos, ebos);
	}

OpenGLVAO::OpenGLVAO(std::vector<std::vector<float>> vbos,
	std::vector<std::vector<unsigned int>> ebos,
	std::vector<std::vector<float>> texcoords)
{
	initVAO(vbos, ebos, texcoords);
}

void OpenGLVAO::initVAO(std::vector<std::vector<float>> vbos,
	std::vector<std::vector<unsigned int>> ebos,
	std::vector<std::vector<float>> texcoords)
{
	m_iNumArrays = vbos.size();
	m_iNumTexArrays = texcoords.size();


	glGenVertexArrays(1, &vaoID[0]); // One VAO per object; it contains our VBOs/EBOs
	glGenBuffers(m_iNumArrays + m_iNumTexArrays, &vboID[0]); // VBO stores vertex/colour/other info
	glGenBuffers(m_iNumArrays + m_iNumTexArrays, &eboID[0]); // EBO can store e.g. indices per triangle
	glBindVertexArray(vaoID[0]);

	for (unsigned int i = 0; i < m_iNumArrays; i++)
	{
		std::vector<float> vbo = vbos[i];
		std::vector<unsigned int> ebo = ebos[i];
		int iVBOSize = vbo.size();
		int iEBOSize = ebo.size();
		glBindBuffer(GL_ARRAY_BUFFER, vboID[i]);
		glBufferData(GL_ARRAY_BUFFER, iVBOSize * sizeof(GLfloat), &vbo[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iEBOSize * sizeof(GLfloat), &ebo[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)i, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(i); // Enable the vertex attrib array
	}

	for (unsigned int i = 0; i < m_iNumTexArrays; i++)
	{
		int iEBOIndex = i + m_iNumArrays;
		std::vector<float> texcoord = texcoords[i];
		std::vector<unsigned int> ebo = ebos[iEBOIndex];
		int iTexCoordSize = texcoord.size();
		int iEBOSize = ebo.size();
		glBindBuffer(GL_ARRAY_BUFFER, vboID[iEBOIndex]);
		glBufferData(GL_ARRAY_BUFFER, iTexCoordSize * sizeof(GLfloat), &texcoord[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID[iEBOIndex]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iEBOSize * sizeof(GLfloat), &ebo[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)iEBOIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(iEBOIndex); // Enable the vertex attrib array
	}

	// Unbind buffer and vertex array so we don't accidentally overwrite anything
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void OpenGLVAO::initVAO(std::vector<std::vector<float>> vbos,
	std::vector<std::vector<unsigned int>> ebos)
	{
	m_iNumArrays = vbos.size();
	m_iNumTexArrays = 0;


	glGenVertexArrays(1, &vaoID[0]); // One VAO per object; it contains our VBOs/EBOs
	glGenBuffers(m_iNumArrays, &vboID[0]); // VBO stores vertex/colour/other info
	glGenBuffers(m_iNumArrays, &eboID[0]); // EBO can store e.g. indices per triangle
	glBindVertexArray(vaoID[0]);

	for (unsigned int i = 0; i < m_iNumArrays; i++)
		{
		std::vector<float> vbo = vbos[i];
		std::vector<unsigned int> ebo = ebos[i];
		int iVBOSize = vbo.size();
		int iEBOSize = ebo.size();
		glBindBuffer(GL_ARRAY_BUFFER, vboID[i]);
		glBufferData(GL_ARRAY_BUFFER, iVBOSize * sizeof(GLfloat), &vbo[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iEBOSize * sizeof(GLfloat), &ebo[0], GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)i, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(i); // Enable the vertex attrib array
		}

	// Unbind buffer and vertex array so we don't accidentally overwrite anything
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	}

OpenGLVAO::~OpenGLVAO() 
	{
	glDeleteVertexArrays(1, &vaoID[0]);
	glDeleteBuffers(m_iNumArrays, &vboID[0]);
	glDeleteBuffers(m_iNumArrays, &eboID[0]);
	}
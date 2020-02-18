#include "OpenGLIncludes.h"
#include "OpenGLVAO.h"
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <vector>
#include <array>

// DEBUG INCLUDES
#include <iostream>

// Vector of arbitrary type for the instanced batch template class
// See https://stackoverflow.com/questions/51046949/constructing-array-of-c-vectors-of-different-types
class ContainerBase
{
public:
	virtual ~ContainerBase() = 0;
	//virtual std::vector<int> getValues() = 0;
	virtual void clear() = 0;
};

inline ContainerBase::~ContainerBase() = default;

template<class T>
class ContainerTyped : public ContainerBase
{
public:
	std::vector<T>& getValues() { return values; }
	void clear() { values.clear(); }
private:
	std::vector<T> values;
};

// First typename is a tuple type that contains all uniforms
// Second and onwards are shader arguments
template<typename uniformTuple, typename ... shaderArgs> class OpenGLInstancedBatch {
public:
	OpenGLInstancedBatch(void) {
		m_iNumObjectsToRender = 0;
	};
	~OpenGLInstancedBatch(void) {
		clear();
	};
	void clear(void) {
		m_depthsFloat.clear();
		m_iNumObjectsToRender = 0;
		for (int i = 0; i < int(m_shaderParameterVectors.size()); i++) {
			m_shaderParameterVectors[i].get()->clear();
		}
	};
	OpenGLVAO* CreateVAO();
	void DebugRender() {
		// Print out the elements of each array.
		for (int i = 0; i < m_numShaderArgs; i++) {
			std::cout << "On shader argument " << i;
			for (int j = 0; j < m_iNumObjectsToRender; j++) {
				//std::cout << static_cast<ContainerTyped<int>*>(m_shaderParameterVectors[i].get())->getValues()[j];
			}
		}
	};
	void Render(OpenGLShader *shader, OpenGLVAO *vao, float &startingDepth, float incDepth, int currentTick) {
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
			// iteratively work through all 
			//glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[6]);
			//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_iNumObjectsToRender, &m_secondaryColorsFloat[0], GL_STATIC_DRAW);
			shader->bind();
			//set uniforms
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_iNumObjectsToRender);
			shader->unbind();
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		clear();
	};
	void addObjToRender(shaderArgs ... shaderArgsList) {
		if (m_shaderParameterVectors.size() == 0) {
			addObjToRenderHelper(true, 0, shaderArgsList...);
		}
		else {
			addObjToRenderHelper(false, 0, shaderArgsList...);
		}
		m_iNumObjectsToRender += 1;
	};
	int getNumObjectsToRender(void) { return m_iNumObjectsToRender; }
	void setUniforms(std::vector<std::string> uniformNames, uniformTuple uniformValues) { m_uniformNames = uniformNames; m_uniformValues = m_uniformValues; }
	void setUniformValues(uniformTuple uniformValues) { m_uniformValues = m_uniformValues; }
	void setUniformNames(std::vector<std::string> uniformNames) { m_uniformNames = uniformNames; }
private:
	// BEGIN DEBUG ONLY FUNCTIONS
	//void print_all(shaderArgs const&... shaderArgSet) {
	//	((os << args), ...);
	//}
	// END DEBUG ONLY FUNCTIONS

	// Function to get the gl type of a glm type (e.g. GL_FLOAT, GL_INT, etc)
	GLenum const getGLType(const int &arg) { return GL_INT; }
	GLenum const getGLType(const glm::ivec1 &arg) { return GL_INT; }
	GLenum const getGLType(const glm::ivec2 &arg) { return GL_INT; }
	GLenum const getGLType(const glm::ivec3 &arg) { return GL_INT; }
	GLenum const getGLType(const glm::ivec4 &arg) { return GL_INT; }
	GLenum const getGLType(const float &arg) { return GL_DOUBLE; }
	GLenum const getGLType(const double &arg) { return GL_DOUBLE; }
	GLenum const getGLType(const glm::vec1 &arg) { return GL_DOUBLE; }
	GLenum const getGLType(const glm::vec2 &arg) { return GL_DOUBLE; }
	GLenum const getGLType(const glm::vec3 &arg) { return GL_DOUBLE; }
	GLenum const getGLType(const glm::vec4 &arg) { return GL_DOUBLE; }

	// Functions to get the number of individual elements in glm types
	int const getGLElemCount(const int &arg) { return 1; }
	int const getGLElemCount(const float &arg) { return 1; }
	int const getGLElemCount(const double &arg) { return 1; }
	int const getGLElemCount(const glm::vec1 &arg) { return 1; }
	int const getGLElemCount(const glm::vec2 &arg) { return 2; }
	int const getGLElemCount(const glm::vec3 &arg) { return 3; }
	int const getGLElemCount(const glm::vec4 &arg) { return 4; }
	int const getGLElemCount(const glm::ivec1 &arg) { return 1; }
	int const getGLElemCount(const glm::ivec2 &arg) { return 2; }
	int const getGLElemCount(const glm::ivec3 &arg) { return 3; }
	int const getGLElemCount(const glm::ivec4 &arg) { return 4; }


	// Helper functions for handling parameter packs
	template<typename firstArg, typename ... otherShaderArgs> void addObjToRenderHelper(bool doInit, int currentShaderArg, firstArg a1, otherShaderArgs ... rest) {
		// Initialize shader parameter container if needed
		if (doInit) {
			m_shaderParameterVectors.push_back(std::make_unique<ContainerTyped<firstArg>>());
			m_paramSizes[currentShaderArg] = sizeof(a1);
			m_paramElemCounts[currentShaderArg] = getGLElemCount(a1);
			m_paramGLTypes[currentShaderArg] = getGLType(a1);
		}
		ContainerTyped<firstArg>* pShaderParameterVector = static_cast<ContainerTyped<firstArg>*>(m_shaderParameterVectors[currentShaderArg].get());
		pShaderParameterVector->getValues().push_back(a1);
		addObjToRenderHelper(doInit, currentShaderArg + 1, rest...);
	};
	template<typename firstArg> void addObjToRenderHelper(bool doInit, int currentShaderArg, firstArg a1) {
		if (doInit) {
			m_shaderParameterVectors.push_back(std::make_unique<ContainerTyped<firstArg>>());
			m_paramSizes[currentShaderArg] = sizeof(a1);
			m_paramElemCounts[currentShaderArg] = getGLElemCount(a1);
			m_paramGLTypes[currentShaderArg] = getGLType(a1);
		}

		ContainerTyped<firstArg>* pShaderParameterVector = static_cast<ContainerTyped<firstArg>*>(m_shaderParameterVectors[currentShaderArg].get());
		pShaderParameterVector->getValues().push_back(a1);
	};
	template<typename firstArg, typename ... otherShaderArgs> void CreateVAOHelper(firstArg a1, otherShaderArgs ... rest);
	template<typename firstArg> void CreateVAOHelper(firstArg a1);

	// Internal variables
	std::vector<std::unique_ptr<ContainerBase>> m_shaderParameterVectors; // TODO: replace with a std::array
	std::array<GLenum, sizeof...(shaderArgs)> m_paramGLTypes; // the GL type - GL_FLOAT or GL_INT - of each parameter
	std::array<int, sizeof...(shaderArgs)> m_paramElemCounts; // number of elements in each parameter
	std::array<int, sizeof...(shaderArgs)> m_paramSizes; // size of each param in *BYTES*
	std::vector<float> m_depthsFloat;
	int m_iNumObjectsToRender;
	std::vector <std::string> m_uniformNames;
	uniformTuple m_uniformValues;
	static const std::size_t m_numShaderArgs = sizeof...(shaderArgs);
};

#include "OpenGLIncludes.h"
#include "OpenGLVAO.h"
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <vector>

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
			//std::cout << "On shader argument " << i;
			for (int j = 0; j < m_iNumObjectsToRender; j++) {
				//std::cout << m_shaderParameterVectors[i][j];
			}
		}
	};
	void Render(OpenGLShader *shader, OpenGLVAO *vao, float &startingDepth, float incDepth, int currentTick);
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
	template<typename firstArg, typename ... otherShaderArgs> void addObjToRenderHelper(bool doInit, int currentShaderArg, firstArg a1, otherShaderArgs ... rest) {
		// Initialize shader parameter container if needed
		if (doInit) {
			m_shaderParameterVectors.push_back(std::make_unique<ContainerTyped<firstArg>>());
		}
		ContainerTyped<firstArg>* pShaderParameterVector = static_cast<ContainerTyped<firstArg>*>(m_shaderParameterVectors[currentShaderArg].get());
		pShaderParameterVector->getValues().push_back(a1);
		addObjToRenderHelper(doInit, currentShaderArg + 1, rest...);
	};
	template<typename firstArg> void addObjToRenderHelper(bool doInit, int currentShaderArg, firstArg a1) {
		if (doInit) {
			m_shaderParameterVectors.push_back(std::make_unique<ContainerTyped<firstArg>>());
		}

		ContainerTyped<firstArg>* pShaderParameterVector = static_cast<ContainerTyped<firstArg>*>(m_shaderParameterVectors[currentShaderArg].get());
		pShaderParameterVector->getValues().push_back(a1);
	};
	template<typename firstArg, typename ... otherShaderArgs> void CreateVAOHelper(firstArg a1, otherShaderArgs ... rest);
	template<typename firstArg> void CreateVAOHelper(firstArg a1);
	std::vector<std::unique_ptr<ContainerBase>> m_shaderParameterVectors;
	std::vector<float> m_depthsFloat;
	int m_iNumObjectsToRender;
	std::vector <std::string> m_uniformNames;
	uniformTuple m_uniformValues;
	static const std::size_t m_numShaderArgs = sizeof...(shaderArgs);
};

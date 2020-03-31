#pragma once
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

template <typename arg1, typename ... arg2> class OpenGLInstancedBatch;

// First typename is a tuple type that contains all uniforms
// Second and onwards are shader arguments
template<typename ... uniformArgs, typename ... shaderArgs> class OpenGLInstancedBatch <std::tuple<uniformArgs...>, shaderArgs...> {
public:
	OpenGLInstancedBatch(void) {
		m_iNumObjectsToRender = 0;
		m_bIsInitialized = false;
	};
	~OpenGLInstancedBatch(void) {
		clear();
	};
	void clear(void) {
		m_depthsFloat.clear();
		m_iNumObjectsToRender = 0;
		for (int i = 0; i < int(m_shaderParameterVectors.size()); i++) {
			if (m_shaderParameterVectors[i].get()) {
				m_shaderParameterVectors[i].get()->clear();
			}
		}
	};
	void InitVAO(void) {
		if (!m_pVao && m_bIsInitialized) {
			// First, we need to initialize the quad's vertices. Create a single VAO that will
			// be the basis for all of our quads rendered using this queue.
			float fSize = 0.5f;
			float posZ = 0.9f; // TODO(heliogenesis): Fix this

			std::vector<float> vertices{
				fSize, fSize, posZ,
				fSize, -fSize, posZ,
				-fSize, -fSize, posZ,
				-fSize, fSize, posZ,
			};

			std::vector<float> colors{
				1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f,
				1.0f, 1.0f, 1.0f
			};

			std::vector<unsigned int> indices{
				0, 1, 3,
				1, 2, 3
			};

			std::vector<std::vector<float>> vbos{ vertices };
			std::vector<std::vector<unsigned int>> ebos{ indices };

			m_pVao = std::make_unique<OpenGLVAO>(vbos, ebos);
			unsigned int iVAOID = m_pVao.get()->getVAO()[0];
			unsigned int *instancedVBO = m_pVao.get()->getinstancedVBO();
			glBindVertexArray(iVAOID);
			glGenBuffers(m_numShaderArgs + 1, &instancedVBO[0]);
			// We need to initialize a vertex array pointer for each shader argument we want to handle, and one more for depth
			// Depth must be the LAST argument passed to our shader for this instanced batch class to work
			for (int argIndex = 0; argIndex < m_numShaderArgs; argIndex++) {
				glEnableVertexAttribArray(argIndex + 1);
				glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[argIndex]);
				setGLVertexAttribPointer((GLuint)argIndex + 1, m_paramElemCounts[argIndex], m_paramGLTypes[argIndex], m_paramSizes[argIndex]);
				glVertexAttribDivisor(argIndex + 1, 1);
			}
			// Depth
			glEnableVertexAttribArray(m_numShaderArgs + 1);
			glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[m_numShaderArgs]);
			setGLVertexAttribPointer((GLuint)m_numShaderArgs + 1, 1, GL_FLOAT, sizeof(float));
			glVertexAttribDivisor(m_numShaderArgs + 1, 1);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
	};
	void DebugRender() {
		// Print out the elements of each array.
		for (int i = 0; i < m_numShaderArgs; i++) {
			std::cout << "On shader argument " << i;
			for (int j = 0; j < m_iNumObjectsToRender; j++) {
				//std::cout << static_cast<ContainerTyped<int>*>(m_shaderParameterVectors[i].get())->getValues()[j];
			}
		}
	};
	void Render(const OpenGLShader *shader, float &startingDepth, float incDepth, int currentTick, bool clearRenderQueue=true) {
		InitVAO();
		if (m_iNumObjectsToRender > 0)
		{
			for (int i = 0; i < m_iNumObjectsToRender; i++)
			{
				m_depthsFloat.insert(m_depthsFloat.end(), startingDepth);
				startingDepth -= incDepth;
			}
			unsigned int iVAOID = m_pVao.get()->getVAO()[0];
			unsigned int *instancedVBO = m_pVao.get()->getinstancedVBO();
			glBindVertexArray(iVAOID);
			// iteratively work through all shader arguments
			for (int argIndex = 0; argIndex < m_numShaderArgs; argIndex++) {
				glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[argIndex]);
				glBufferData(GL_ARRAY_BUFFER, m_paramSizes[argIndex] * m_iNumObjectsToRender, static_cast<ContainerTyped<int>*>(m_shaderParameterVectors[argIndex].get())->getValues().data(), GL_STATIC_DRAW);
			}
			glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[m_numShaderArgs]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(int) * m_iNumObjectsToRender, m_depthsFloat.data(), GL_STATIC_DRAW);
			shader->bind();
			//int uniformArgIndex = 0;
			//set uniforms
			m_iNumTexturesBound = 0;
			std::apply
			(
				[this, shader](uniformArgs const&... tupleArgs)
			{
				//std::make_tuple(setUniformValue(shader, uniformArgIndex, tupleArgs)...);
				setGLUniformValues(shader, 0, tupleArgs...);
				//uniformArgIndex++;
			}, m_uniformValues
			);
			glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_iNumObjectsToRender);
			shader->unbind();
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		if (clearRenderQueue) {
			clear();
		}
	};
	void addObjToRender(shaderArgs ... shaderArgsList) {
		addObjToRenderHelper(0, shaderArgsList...);
		m_iNumObjectsToRender += 1;
	};
	int getNumObjectsToRender(void) { return m_iNumObjectsToRender; }
	void setUniforms(std::array<std::string, sizeof...(uniformArgs)> uniformNames, uniformArgs... uniformValues) { m_uniformNames = uniformNames; m_uniformValues = std::make_tuple(uniformValues...); }
	void setCanvasDimensions(std::tuple<int, int> canvasDimensions) { m_canvasDimensions = canvasDimensions; }
	void setUniformValues(std::tuple<uniformArgs...> uniformValues) { m_uniformValues = m_uniformValues; }
	void setUniformNames(std::array<std::string, sizeof...(uniformArgs)> uniformNames) { m_uniformNames = uniformNames; }
	ContainerBase* getParameterForObject(int paramIndex) {
		// Note that this container must be cast manually to the correct value.
		return m_shaderParameterVectors[paramIndex].get();
	}
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
	GLenum const getGLType(const float &arg) { return GL_FLOAT; }
	GLenum const getGLType(const double &arg) { return GL_DOUBLE; }
	GLenum const getGLType(const glm::vec1 &arg) { return GL_FLOAT; }
	GLenum const getGLType(const glm::vec2 &arg) { return GL_FLOAT; }
	GLenum const getGLType(const glm::vec3 &arg) { return GL_FLOAT; }
	GLenum const getGLType(const glm::vec4 &arg) { return GL_FLOAT; }

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

	// Functions to manage uniforms
	template<typename firstUniformArg, typename ... otherUniformArgs> void setGLUniformValues(const OpenGLShader* shader, int uniformArgIndex, firstUniformArg firstUniformValue, otherUniformArgs...rest) {
		setGLUniformValue(shader, uniformArgIndex, firstUniformValue);
		setGLUniformValues(shader, uniformArgIndex + 1, rest...);
		std::cout << "Adding argument " << uniformArgIndex << " to uniforms...";
	}
	template<typename firstUniformArg> void setGLUniformValues(const OpenGLShader* shader, int uniformArgIndex, firstUniformArg firstUniformValue) {
		setGLUniformValue(shader, uniformArgIndex, firstUniformValue);
		std::cout << "Adding argument " << uniformArgIndex << " to uniforms...";
	}
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const int tupleArg) { glUniform1i(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), tupleArg); }
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const glm::ivec1 tupleArg) { glUniform1i(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), tupleArg[0]); }
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const glm::ivec2 tupleArg) { glUniform2i(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), tupleArg[0], tupleArg[1]); }
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const glm::ivec3 tupleArg) { glUniform3i(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), tupleArg[0], tupleArg[1], tupleArg[2]); }
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const glm::ivec4 tupleArg) { glUniform4i(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), tupleArg[0], tupleArg[1], tupleArg[2], tupleArg[3]); }
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const float tupleArg) { glUniform1f(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), tupleArg); }
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const glm::vec1 tupleArg) { glUniform1f(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), tupleArg[0]); }
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const glm::vec2 tupleArg) { glUniform2f(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), tupleArg[0], tupleArg[1]); }
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const glm::vec3 tupleArg) { glUniform3f(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), tupleArg[0], tupleArg[1], tupleArg[2]); }
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const glm::vec4 tupleArg) { glUniform4f(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), tupleArg[0], tupleArg[1], tupleArg[2], tupleArg[3]); }
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const OpenGLTexture* tupleArg) {
		glUniform1i(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), m_iNumTexturesBound);
		tupleArg->bindTexture2D(GL_TEXTURE0 + m_iNumTexturesBound);
		m_iNumTexturesBound += 1;
	}

	void setGLVertexAttribPointer(GLuint argIndex, GLint size, GLenum type, GLsizei stride) {
		if (type == GL_FLOAT) {
			glVertexAttribPointer((GLuint)argIndex, size, type, GL_FALSE, stride, (void*)0);
		}
		else {
			glVertexAttribIPointer((GLuint)argIndex, size, type, stride, (void*)0);
		}
	}

	// Helper functions for handling parameter packs
	template<typename firstArg> void initInstancedBatchArg(int currentShaderArg, firstArg a1) {
		if (!m_bIsInitialized) {
			m_shaderParameterVectors[currentShaderArg] = std::make_unique<ContainerTyped<firstArg>>();
			m_paramSizes[currentShaderArg] = sizeof(a1);
			m_paramElemCounts[currentShaderArg] = getGLElemCount(a1);
			m_paramGLTypes[currentShaderArg] = getGLType(a1);
			if ((currentShaderArg + 1) >= m_numShaderArgs) {
				// TODO: Move this to something in the Render phase; due to OpenGL threading issues
				// We should initialize the VAO separately, maybe do it during rendering if we haven't done it already
				//InitVAO();
				m_bIsInitialized = true;
			}
		}
	};
	template<typename firstArg, typename ... otherShaderArgs> void addObjToRenderHelper(int currentShaderArg, firstArg a1, otherShaderArgs ... rest) {
		// Initialize shader parameter container if needed
		initInstancedBatchArg(currentShaderArg, a1);
		ContainerTyped<firstArg>* pShaderParameterVector = static_cast<ContainerTyped<firstArg>*>(m_shaderParameterVectors[currentShaderArg].get());
		pShaderParameterVector->getValues().push_back(a1);
		addObjToRenderHelper(currentShaderArg + 1, rest...);
	};
	template<typename firstArg> void addObjToRenderHelper(int currentShaderArg, firstArg a1) {
		initInstancedBatchArg(currentShaderArg, a1);
		ContainerTyped<firstArg>* pShaderParameterVector = static_cast<ContainerTyped<firstArg>*>(m_shaderParameterVectors[currentShaderArg].get());
		pShaderParameterVector->getValues().push_back(a1);
	};
	template<typename firstArg, typename ... otherShaderArgs> void CreateVAOHelper(firstArg a1, otherShaderArgs ... rest);
	template<typename firstArg> void CreateVAOHelper(firstArg a1);

	// Internal variables
	bool m_bIsInitialized;
	std::unique_ptr<OpenGLVAO> m_pVao = nullptr;
	std::array<std::unique_ptr<ContainerBase>, sizeof...(shaderArgs)> m_shaderParameterVectors; // TODO: replace with a std::array
	std::array<GLenum, sizeof...(shaderArgs)> m_paramGLTypes; // the GL type - GL_FLOAT or GL_INT - of each parameter
	std::array<int, sizeof...(shaderArgs)> m_paramElemCounts; // number of elements in each parameter
	std::array<int, sizeof...(shaderArgs)> m_paramSizes; // size of each param in *BYTES*
	std::vector<float> m_depthsFloat;
	int m_iNumObjectsToRender;
	std::array<std::string, sizeof...(uniformArgs)> m_uniformNames;
	std::tuple<uniformArgs...> m_uniformValues;
	std::tuple<int, int> m_canvasDimensions;
	int m_iNumTexturesBound;
	static const std::size_t m_numShaderArgs = sizeof...(shaderArgs);
};

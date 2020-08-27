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

template <typename ... shaderArgs>
class OpenGLInstancedBatchRenderRequest {
	// Abstract class describing a render request for an OpenGLInstancedBatch.
	// The objective is to move all functions and all information regarding shader arguments here; the OpenGLInstancedBatch shouldn't know anything about how many shader args there are.
	// We should store depth as a unique shader argument, separate from the rest.
	// Note, uniforms are not handled here, as "shader arguments" includes all inputs to a shader that are NOT uniforms.
	// We may want to make this a template class, and typedef specific versions just as we do with OpenGLInstancedBatch.
	// We can even make this a class template, and have specific render request types inherit from (specializations of) this template,
	// e.g. OpenGLInstancedBatchTextureRenderRequest inherits from OpenGLInstancedBatchRenderRequest<foo, bar, ...>
	//template <typename uniformArgs, typename ... shaderArgs>
	//friend class OpenGLInstancedBatch;
public:
	OpenGLInstancedBatchRenderRequest(shaderArgs ... shaderArgsList) {
		setShaderArguments(shaderArgsList...);
	}
	~OpenGLInstancedBatchRenderRequest() {};
	void setShaderArguments(shaderArgs ... shaderArgsList) {
		m_shaderArgs = std::make_tuple(shaderArgsList...);
	}
	std::unique_ptr<OpenGLVAO> setUpVAO() {
		// Set up the VAO
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

		std::unique_ptr<OpenGLVAO> pVao = std::make_unique<OpenGLVAO>(vbos, ebos);
		unsigned int iVAOID = pVao.get()->getVAO()[0];
		unsigned int *instancedVBO = pVao.get()->getinstancedVBO();
		glBindVertexArray(iVAOID);
		glGenBuffers(1, &instancedVBO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[0]);
		// We need to initialize a vertex array pointer for each shader argument we want to handle, and one more for depth
		// Depth must be the LAST argument passed to our shader for this instanced batch class to work
		auto paramElemCounts = getShaderArgumentElementCounts();
		auto paramGLTypes = getShaderArgumentTypes();
		auto paramSizes = getShaderArgumentSizes();
		auto paramLocations = getShaderArgumentLocations();
		auto renderRequestSize = getRenderRequestSize();
		int currArgPos = int(&m_depth) - int(this);
		std::cout << currArgPos;

		for (int argIndex = 0; argIndex < m_numShaderArgs; argIndex++) {
			glEnableVertexAttribArray(argIndex + 1);
			setGLVertexAttribPointer((GLuint)argIndex + 1, paramElemCounts[argIndex], paramGLTypes[argIndex], renderRequestSize, paramLocations[argIndex]);
			glVertexAttribDivisor(argIndex + 1, 1);
			currArgPos += paramSizes[argIndex];
		}
		// Depth
		glEnableVertexAttribArray(m_numShaderArgs + 1);
		setGLVertexAttribPointer((GLuint)m_numShaderArgs + 1, 1, GL_FLOAT, renderRequestSize, int(&m_depth) - int(this));
		glVertexAttribDivisor(m_numShaderArgs + 1, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		return pVao;
	};
	virtual OpenGLVAO& getVAOForInstancedBatchType() = 0;
	virtual int getRenderRequestSize() = 0;
	//virtual std::vector<void*> getShaderArgumentPtrs(); // Returns a vector of pointers to the shader arguments, which are all private variables unique to each inheriting class
	std::vector<GLint> getShaderArgumentSizes() {
		std::vector<GLint> shaderArgumentSizes;
		std::apply
		(
			[this, &shaderArgumentSizes](shaderArgs const&... tupleArgs)
		{
			getShaderArgumentSizesHelper(shaderArgumentSizes, tupleArgs...);
		}, m_shaderArgs
		);
		return shaderArgumentSizes;
	};
	std::vector<int> getShaderArgumentTypes() {
		std::vector<int> shaderArgumentTypes;
		std::apply
		(
			[this, &shaderArgumentTypes](shaderArgs const&... tupleArgs)
		{
			getShaderArgumentTypesHelper(shaderArgumentTypes, tupleArgs...);
		}, m_shaderArgs
		);
		return shaderArgumentTypes;
	};
	std::vector<GLenum> getShaderArgumentElementCounts() {
		std::vector<GLenum> shaderArgumentElementCounts;
		std::apply
		(
			[this, &shaderArgumentElementCounts](shaderArgs const&... tupleArgs)
		{
			getShaderArgumentElementCountsHelper(shaderArgumentElementCounts, tupleArgs...);
		}, m_shaderArgs
		);
		return shaderArgumentElementCounts;
	};
	void set_depth(float depth) {
		m_depth = depth;
	}
	float getDepth() { return m_depth; }

	void object_is_not_an_instanced_batch_render_request_type() {

	}
	//virtual std::vector<GLenum> getShaderArgumentTypes();
private:
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

	std::vector<int> getShaderArgumentLocations() {
		// We need this function because Tuple is not standard order (the ordering of the arguments is not predictable).
		std::vector<int> shaderArgumentLocations;
		std::apply
		(
			[this, &shaderArgumentLocations](shaderArgs const&... tupleArgs)
		{
			getShaderArgumentLocationsHelper(shaderArgumentLocations, tupleArgs...);
		}, m_shaderArgs
		);
		return shaderArgumentLocations;
	};

	template<typename firstArg, typename ... otherShaderArgs> void getShaderArgumentLocationsHelper(std::vector<int>& outputVector, firstArg &a1, otherShaderArgs& ... rest) {
		outputVector.push_back(int(&a1) - int(this));
		getShaderArgumentLocationsHelper(outputVector, rest...);
	};
	template<typename firstArg> void getShaderArgumentLocationsHelper(std::vector<int>& outputVector, firstArg &a1) {
		outputVector.push_back(int(&a1) - int(this));
	};

	template<typename firstArg, typename ... otherShaderArgs> void getShaderArgumentSizesHelper(std::vector<GLint>& outputVector, firstArg a1, otherShaderArgs ... rest) {
		outputVector.push_back(sizeof(a1));
		getShaderArgumentSizesHelper(outputVector, rest...);
	};
	template<typename firstArg> void getShaderArgumentSizesHelper(std::vector<GLint>& outputVector, firstArg a1) {
		outputVector.push_back(sizeof(a1));
	};
	template<typename firstArg, typename ... otherShaderArgs> void getShaderArgumentElementCountsHelper(std::vector<GLenum>& outputVector, firstArg a1, otherShaderArgs ... rest) {
		outputVector.push_back(getGLElemCount(a1));
		getShaderArgumentElementCountsHelper(outputVector, rest...);
	};
	template<typename firstArg> void getShaderArgumentElementCountsHelper(std::vector<GLenum>& outputVector, firstArg a1) {
		outputVector.push_back(getGLElemCount(a1));
	};
	template<typename firstArg, typename ... otherShaderArgs> void getShaderArgumentTypesHelper(std::vector<int>& outputVector, firstArg a1, otherShaderArgs ... rest) {
		outputVector.push_back(getGLType(a1));
		getShaderArgumentTypesHelper(outputVector, rest...);
	};
	template<typename firstArg> void getShaderArgumentTypesHelper(std::vector<int>& outputVector, firstArg a1) {
		outputVector.push_back(getGLType(a1));
	};

	void setGLVertexAttribPointer(GLuint argIndex, GLint size, GLenum type, GLsizei stride, int currArgPos) {
		if (type == GL_FLOAT) {
			glVertexAttribPointer((GLuint)argIndex, size, type, GL_FALSE, stride, (void*)currArgPos);
		}
		else {
			glVertexAttribIPointer((GLuint)argIndex, size, type, stride, (void*)currArgPos);
		}
	}

	std::tuple<shaderArgs...> m_shaderArgs;
	float m_depth = 0.0;
	static const std::size_t m_numShaderArgs = sizeof...(shaderArgs); // TODO: Move to implementations of this abstract class, also test that this works!!!!
};

class OpenGLInstancedBatchInterface {
public:
	virtual void RenderUpToGivenDepth(const OpenGLShader* shader, const float minDepth, bool clearRenderQueue = true) = 0;
	virtual void Render(const OpenGLShader* shader, float& startingDepth, float incDepth, bool clearRenderQueue = true, bool manuallySetDepth = true) = 0;
	virtual void clear(void) = 0;
	virtual int getNumObjectsToRender(void) = 0;
	virtual float getDepthOfDeepestObject() = 0;
	virtual bool getHasObjectsStillRequiringRendering() = 0;
	virtual void setBlendMode(int blendMode) = 0;
	virtual int getBlendMode() = 0;
};

// Second typename is a tuple type that contains all uniforms
template<typename shaderRenderRequest, typename ... uniformArgs> class OpenGLInstancedBatch <shaderRenderRequest, std::tuple<uniformArgs...>> : public OpenGLInstancedBatchInterface {
public:
	OpenGLInstancedBatch(void) {
		//static_assert(std::is_base_of<OpenGLInstancedBatchRenderRequest, shaderRenderRequest>::value, "Derived not derived from OpenGLInstancedBatchRenderRequest");
		m_iFirstUnrenderedElementIndex = 0;
		m_iNumTexturesBound = 0;
	};
	~OpenGLInstancedBatch(void) {
		clear();
	};
	void clear(void) override {
		m_depthsFloat.clear();
		m_renderRequests.clear();
		m_iFirstUnrenderedElementIndex = 0;
		m_iNumTexturesBound = 0;
	};

	void Render(const OpenGLShader *shader, float &startingDepth, float incDepth, bool clearRenderQueue=true, bool manuallySetDepth=true) override {
		int iNumObjectsToRender = m_renderRequests.size();
		if (iNumObjectsToRender > 0)
		{
			if (!manuallySetDepth)
			{
				for (int i = 0; i < iNumObjectsToRender; i++)
				{
					m_renderRequests[i].set_depth(startingDepth);
					startingDepth -= incDepth;
				}
			}
			RenderNItems(shader, iNumObjectsToRender, 0);
		}
		if (clearRenderQueue) {
			clear();
		}
	};
	void RenderUpToGivenDepth(const OpenGLShader* shader, const float minDepth, bool clearRenderQueue = true) {
		unsigned int iNumObjectsToRender = m_renderRequests.size();
		if (iNumObjectsToRender > 0)
		{
			// Find the first object that has a depth equal to or less than the given depth
			unsigned int i = m_iFirstUnrenderedElementIndex;
			while ((i < iNumObjectsToRender) && m_renderRequests[i].getDepth() > minDepth) {
				i += 1;
			}

			int iNumObjectsForThisDrawCall = i - m_iFirstUnrenderedElementIndex;
			RenderNItems(shader, iNumObjectsForThisDrawCall, m_iFirstUnrenderedElementIndex);
			m_iFirstUnrenderedElementIndex = i;
		}
		if (clearRenderQueue && (iNumObjectsToRender <= m_iFirstUnrenderedElementIndex)) {
			clear();
		}
	};
	void RenderNItems(const OpenGLShader* shader, const int numItemsToRender, const int firstItemToRender) {
		OpenGLVAO& vao = m_renderRequests[0].getVAOForInstancedBatchType();
		unsigned int iVAOID = vao.getVAO()[0];
		unsigned int* instancedVBO = vao.getinstancedVBO();
		glBindVertexArray(iVAOID);
		glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(shaderRenderRequest) * numItemsToRender, &m_renderRequests.at(firstItemToRender), GL_STATIC_DRAW);
		shader->bind();
		//set uniforms
		m_iNumTexturesBound = 0;
		std::apply
		(
			[this, shader](uniformArgs const&... tupleArgs)
		{
			setGLUniformValues(shader, 0, tupleArgs...);
		}, m_uniformValues
		);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, numItemsToRender);
		shader->unbind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	void RenderAllNonInstanced(const OpenGLShader *shader, float &startingDepth, float incDepth, int currentTick, bool clearRenderQueue = true) {
		int iNumObjectsToRender = m_renderRequests.size();
		if (iNumObjectsToRender > 0)
		{
			OpenGLVAO &vao = m_renderRequests[0].getVAOForInstancedBatchType();
			unsigned int iVAOID = vao.getVAO()[0];
			unsigned int *instancedVBO = vao.getinstancedVBO();
			glBindVertexArray(iVAOID);
			glBindBuffer(GL_ARRAY_BUFFER, instancedVBO[0]);
			shader->bind();
			//set uniforms
			m_iNumTexturesBound = 0;
			std::apply
			(
				[this, shader](uniformArgs const&... tupleArgs)
			{
				setGLUniformValues(shader, 0, tupleArgs...);
			}, m_uniformValues
			);
			for (int i = 0; i < iNumObjectsToRender; i++)
			{
				m_renderRequests[i].set_depth(startingDepth);
				startingDepth -= incDepth;
				glBufferData(GL_ARRAY_BUFFER, sizeof(shaderRenderRequest), &m_renderRequests[i], GL_STATIC_DRAW);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			}
			shader->unbind();
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		if (clearRenderQueue) {
			clear();
		}
	};
	void addObjToRender(shaderRenderRequest renderRequest) {
		m_renderRequests.push_back(renderRequest);
	};
	int getNumObjectsToRender(void) override { return m_renderRequests.size(); }
	void setUniforms(std::array<std::string, sizeof...(uniformArgs)> uniformNames, uniformArgs... uniformValues) { m_uniformNames = uniformNames; m_uniformValues = std::make_tuple(uniformValues...); }
	void setCanvasDimensions(std::tuple<int, int> canvasDimensions) { m_canvasDimensions = canvasDimensions; }
	void setUniformValues(std::tuple<uniformArgs...> uniformValues) { m_uniformValues = m_uniformValues; }
	void setUniformNames(std::array<std::string, sizeof...(uniformArgs)> uniformNames) { m_uniformNames = uniformNames; }

	bool getHasObjectsStillRequiringRendering() override {
		return m_renderRequests.size() <= m_iFirstUnrenderedElementIndex;
	}
	float getDepthOfDeepestObject() override {
		// Return depth of the deepest element that still needs to be rendered.
		// Return -1 if no objects need to be rendered.
		if (m_renderRequests.size() > 0) {
			return m_renderRequests[m_iFirstUnrenderedElementIndex].getDepth();
		}
		return -1.0;
	}
	void setBlendMode(int blendMode) override { m_iBlendMode = blendMode; }
	int getBlendMode() override { return m_iBlendMode; }
private:
	// Functions to manage uniforms
	template<typename firstUniformArg, typename ... otherUniformArgs> void setGLUniformValues(const OpenGLShader* shader, int uniformArgIndex, firstUniformArg firstUniformValue, otherUniformArgs...rest) {
		setGLUniformValue(shader, uniformArgIndex, firstUniformValue);
		setGLUniformValues(shader, uniformArgIndex + 1, rest...);
	}
	template<typename firstUniformArg> void setGLUniformValues(const OpenGLShader* shader, int uniformArgIndex, firstUniformArg firstUniformValue) {
		setGLUniformValue(shader, uniformArgIndex, firstUniformValue);
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
	void setGLUniformValue(const OpenGLShader* shader, int uniformArgIndex, const OpenGLAnimatedNoise* tupleArg) {
		glUniform1i(glGetUniformLocation(shader->id(), m_uniformNames[uniformArgIndex].c_str()), m_iNumTexturesBound);
		tupleArg->bindTexture3D(GL_TEXTURE0 + m_iNumTexturesBound);
		m_iNumTexturesBound += 1;
	}

	// Internal variables
	std::vector<shaderRenderRequest> m_renderRequests;
	std::vector<float> m_depthsFloat;
	std::array<std::string, sizeof...(uniformArgs)> m_uniformNames;
	std::tuple<uniformArgs...> m_uniformValues;
	std::tuple<int, int> m_canvasDimensions;
	int m_iNumTexturesBound = 0;
	unsigned int m_iFirstUnrenderedElementIndex = 0;
	int m_iBlendMode = 0;
};


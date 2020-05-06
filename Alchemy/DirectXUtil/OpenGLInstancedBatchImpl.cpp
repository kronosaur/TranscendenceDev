#include "PreComp.h"
#include "OpenGL.h"
#include "OpenGLInstancedBatchImpl.h"

//ContainerTyped<glm::vec2>* getTextureCoordinates(OpenGLInstancedBatchTexture *instancedBatchTexture) {
//	return reinterpret_cast<ContainerTyped<glm::vec2>*>(instancedBatchTexture->getParameterForObject(0));
//}

//ContainerTyped<glm::vec2>* getTextureSizes(OpenGLInstancedBatchTexture *instancedBatchTexture) {
//	return reinterpret_cast<ContainerTyped<glm::vec2>*>(instancedBatchTexture->getParameterForObject(3));
//}

std::unique_ptr<OpenGLVAO> OpenGLInstancedBatchRenderRequestTexture::vao = nullptr;

std::unique_ptr<OpenGLVAO> OpenGLInstancedBatchRenderRequestRay::vao = nullptr;

std::unique_ptr<OpenGLVAO> OpenGLInstancedBatchRenderRequestLightning::vao = nullptr;

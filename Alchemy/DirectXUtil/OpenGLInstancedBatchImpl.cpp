#include "PreComp.h"
#include "OpenGL.h"
#include "OpenGLInstancedBatchImpl.h"

// Set the static vao members of all batch render requests to nullptr so that we can set them up ourselves when we initialize them (since
// VAOs must be initialized in OpenGL before we can use them, and each render request type has its own static vao variable)

std::unique_ptr<OpenGLVAO> OpenGLInstancedBatchRenderRequestTexture::vao = nullptr;

std::unique_ptr<OpenGLVAO> OpenGLInstancedBatchRenderRequestRay::vao = nullptr;

std::unique_ptr<OpenGLVAO> OpenGLInstancedBatchRenderRequestLightning::vao = nullptr;

std::unique_ptr<OpenGLVAO> OpenGLInstancedBatchRenderRequestOrb::vao = nullptr;

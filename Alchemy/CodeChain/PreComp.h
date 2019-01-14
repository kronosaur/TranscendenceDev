//	PreComp.h
//
//	CodeChain pre-compiled headers
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#ifdef USE_FOUNDATION
#include "Foundation.h"
#else
#include "Kernel.h"
#include "KernelObjID.h"
using namespace Kernel;
#endif
#include "CodeChain.h"


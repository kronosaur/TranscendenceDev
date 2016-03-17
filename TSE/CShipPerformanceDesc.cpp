//	CShipPerformanceDesc.cpp
//
//	CShipPerformanceDesc class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CShipPerformanceDesc::Init (SShipPerformanceCtx &Ctx)

//  Init
//
//  Initialize the performance descriptor from the parameters accumulated in
//  the context block.

    {
    //  Initialize our maneuvering performance

    m_RotationDesc.InitFromDesc(Ctx.RotationDesc);
    }

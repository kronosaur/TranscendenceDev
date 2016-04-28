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

    //  Compute our drive parameters. We start with the core stats

    m_DriveDesc = Ctx.DriveDesc;

    //  Adjust max speed.

    if (Ctx.rMaxSpeedBonus != 0.0)
        {
        Metric rAdj = Max(0.0, 1.0 + (0.01 * Ctx.rMaxSpeedBonus));
        m_DriveDesc.AdjMaxSpeed(rAdj);
        }

    //  If drive damaged, cut thrust in half

    if (Ctx.bDriveDamaged)
        m_DriveDesc.AdjThrust(0.5);

    //  If we're running at half speed...

    if (Ctx.bHalfSpeed)
        m_DriveDesc.AdjMaxSpeed(0.5);

    //  Cargo space

    m_CargoDesc = Ctx.CargoDesc;
    }

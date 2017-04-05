//	CShipPerformanceDesc.cpp
//
//	CShipPerformanceDesc class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CShipPerformanceDesc CShipPerformanceDesc::m_Null;

void CShipPerformanceDesc::Init (SShipPerformanceCtx &Ctx)

//  Init
//
//  Initialize the performance descriptor from the parameters accumulated in
//  the context block.

    {
    //  Initialize our maneuvering performance

    m_RotationDesc.InitFromDesc(Ctx.RotationDesc);

	//	Initialize reactor desc

	m_ReactorDesc = Ctx.ReactorDesc;

    //  Compute our drive parameters. We start with the core stats

    m_DriveDesc = Ctx.DriveDesc;

    //  Adjust max speed.

    if (Ctx.rMaxSpeedBonus != 0.0)
        {
        Metric rAdj = Max(0.0, 1.0 + (0.01 * Ctx.rMaxSpeedBonus));
        m_DriveDesc.AdjMaxSpeed(rAdj);
        }

	if (Ctx.rArmorSpeedBonus != 0.0)
		m_DriveDesc.AddMaxSpeed(Ctx.rArmorSpeedBonus);

	//	Apply the speed limit

	m_DriveDesc.SetMaxSpeed(Min(m_DriveDesc.GetMaxSpeed(), Ctx.rMaxSpeedLimit));

	//  If drive damaged, cut thrust in half

    if (Ctx.bDriveDamaged)
        m_DriveDesc.AdjThrust(0.5);

    //  If we're running at half speed...

    if (Ctx.bDriveDamaged)
        m_DriveDesc.AdjMaxSpeed(Min(Ctx.rOperatingSpeedAdj, 0.5));
	else if (Ctx.rOperatingSpeedAdj != 1.0)
        m_DriveDesc.AdjMaxSpeed(Ctx.rOperatingSpeedAdj);

    //  Cargo space

	Ctx.CargoDesc.ValidateCargoSpace(Ctx.iMaxCargoSpace);
    m_CargoDesc = Ctx.CargoDesc;

	//	Other flags

	m_fShieldInterference = Ctx.bShieldInterference;

	//	Done

	m_fInitialized = true;
    }

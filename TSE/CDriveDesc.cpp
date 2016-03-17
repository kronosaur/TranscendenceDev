//	CDriveDesc.cpp
//
//	CDriveDesc class
//  Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define INERTIALESS_DRIVE_ATTRIB	CONSTLIT("inertialessDrive")
#define MAX_SPEED_ATTRIB			CONSTLIT("maxSpeed")
#define POWER_USE_ATTRIB			CONSTLIT("powerUse")
#define POWER_USED_ATTRIB			CONSTLIT("powerUsed")
#define THRUST_ATTRIB				CONSTLIT("thrust")

CDriveDesc::CDriveDesc (void) :
        m_dwUNID(0),
        m_rMaxSpeed(0.0),
        m_iThrust(0),
        m_iPowerUse(0),
        m_fInertialess(false)

//  CDriveDesc constructor

    {

    }

void CDriveDesc::Add (const CDriveDesc &Src)

//  Add
//
//  Adds the properties of the drive to this one.

    {
    //  If the source has an UNID, then it means that it is a drive upgradew, so
    //  we take the UNID.

    if (Src.m_dwUNID)
		m_dwUNID = Src.m_dwUNID;

    //  Take the highest max speed.

    m_rMaxSpeed = Max(m_rMaxSpeed, Src.m_rMaxSpeed);

    //  Thrust adds

    m_iThrust += Src.m_iThrust;

    //  We take the power from the new device

    m_iPowerUse = Src.m_iPowerUse;

    //  Take on inertialess

    if (Src.m_fInertialess)
        m_fInertialess = true;
    }

Metric CDriveDesc::AdjMaxSpeed (Metric rAdj)

//  AdjMaxSpeed
//
//  Adjust speed

    {
    if (rAdj >= 0.0)
        m_rMaxSpeed = Min(m_rMaxSpeed * rAdj, LIGHT_SPEED);

    return m_rMaxSpeed;
    }

int CDriveDesc::AdjPowerUse (Metric rAdj)

//  AdjPowerUse
//
//  Adjust power use

    {
    if (rAdj >= 0.0)
        m_iPowerUse = mathRound(m_iPowerUse * rAdj);

    return m_iPowerUse;
    }

int CDriveDesc::AdjThrust (Metric rAdj)

//  AdjThrust
//
//  Adjust thrust

    {
    if (rAdj >= 0.0)
        m_iThrust = mathRound(m_iThrust * rAdj);

    return m_iThrust;
    }

ALERROR CDriveDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID, bool bShipClass)

//  InitFromXML
//
//  Initializes from an XML descriptor

    {
    m_dwUNID = dwUNID;
	m_rMaxSpeed = (double)pDesc->GetAttributeInteger(MAX_SPEED_ATTRIB) * LIGHT_SPEED / 100;

    //  Thrust

	m_iThrust = pDesc->GetAttributeInteger(THRUST_ATTRIB);

    //  Power use. Previous versions called it "powerUsed", so we check both
    //  attributes.

    if (!pDesc->FindAttributeInteger(POWER_USE_ATTRIB, &m_iPowerUse))
    	m_iPowerUse = pDesc->GetAttributeInteger(POWER_USED_ATTRIB);

    //  Other

	m_fInertialess = pDesc->GetAttributeBool(INERTIALESS_DRIVE_ATTRIB);

    return NOERROR;
    }

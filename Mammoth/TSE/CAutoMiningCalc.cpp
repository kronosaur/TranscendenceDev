//	CAutoMiningCalc.cpp
//
//	CAutoMiningCalc class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const CSpaceObject *CAutoMiningCalc::GetAutoTarget (void) const

//	GetAutoTarget
//
//	Returns the currently computed target.

	{
	if (m_pAsteroidTarget && !m_pAsteroidTarget->IsDestroyed())
		return m_pAsteroidTarget;
	else
		return NULL;
	}

void CAutoMiningCalc::Init (const CSpaceObject &PlayerObj)

//	Init
//
//	Initialize at the beginning of system update loop.

	{
	m_bNeedsAutoTarget = false;
	m_pAsteroidTarget = NULL;
	m_rTargetDist2 = MAX_DISTANCE2;

	//	Check to see if the primary weapon has mining

	if (const CDeviceItem Weapon = PlayerObj.GetNamedDeviceItem(devPrimaryWeapon))
		{
		if (Weapon.IsEnabled() && Weapon.IsMiningWeapon())
			m_bNeedsAutoTarget = Weapon.NeedsAutoTarget(&m_iMinFireArc, &m_iMaxFireArc);
		}

	//	Now check the launcher

	if (const CDeviceItem Launcher = PlayerObj.GetNamedDeviceItem(devMissileWeapon))
		{
		int iLauncherMinFireArc, iLauncherMaxFireArc;

		if (Launcher.IsEnabled() 
				&& Launcher.IsMiningWeapon()
				&& Launcher.NeedsAutoTarget(&iLauncherMinFireArc, &iLauncherMaxFireArc))
			{
			if (m_bNeedsAutoTarget)
				CGeometry::CombineArcs(m_iMinFireArc, m_iMaxFireArc, iLauncherMinFireArc, iLauncherMaxFireArc, &m_iMinFireArc, &m_iMaxFireArc);
			else
				{
				m_bNeedsAutoTarget = true;
				m_iMinFireArc = iLauncherMinFireArc;
				m_iMaxFireArc = iLauncherMaxFireArc;
				}
			}
		}
	}

void CAutoMiningCalc::SetTargetIfBetter (const CSpaceObject &Obj, Metric rObjDist2)

//	SetTargetIfBetter
//
//	Sets the object as our current auto target if it is better.

	{
	if (rObjDist2 < m_rTargetDist2)
		{
		m_pAsteroidTarget = &Obj;
		m_rTargetDist2 = rObjDist2;
		}
	}

void CAutoMiningCalc::Update (const CSpaceObject &PlayerObj, const CSpaceObject &Obj)

//	Update
//
//	Update for each object in the system.

	{
	if (!m_bNeedsAutoTarget)
		{ }

	else if (!Obj.CanBeMined())
		{ }

	else if (Obj.IsExplored() && !Obj.HasMinableItem())
		{ }

	//	If the player's weapons has an arc of fire, then limit ourselves to
	//	targets in the arc.

	else if (m_iMinFireArc != m_iMaxFireArc)
		{
		CVector vDist = Obj.GetPos() - PlayerObj.GetPos();
		Metric rDist;
		int iAngle = VectorToPolar(vDist, &rDist);

		if (AngleInArc(iAngle, m_iMinFireArc, m_iMaxFireArc))
			SetTargetIfBetter(Obj, rDist * rDist);
		}

	//	Otherwise, set it if better

	else
		{
		CVector vDist = Obj.GetPos() - PlayerObj.GetPos();
		SetTargetIfBetter(Obj, vDist.Length2());
		}
	}

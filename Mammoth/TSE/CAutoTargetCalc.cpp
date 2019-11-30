//	CAutoTargetCalc.cpp
//
//	CAutoTargetCalc class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const CSpaceObject *CAutoTargetCalc::GetAutoTarget (void) const

//	GetAutoTarget
//
//	Returns the computer auto target.

	{
	//	NOTE: We need to check again to see if the target is destroyed because 
	//	it could have gotten destroyed after it	was picked.

	if (m_pTargetObj && !m_pTargetObj->IsDestroyed())
		return m_pTargetObj;
	else
		return NULL;
	}

void CAutoTargetCalc::Init (const CSpaceObject &PlayerObj)

//	Init
//
//	Initializes. Call this before Update.

	{
	m_pPlayerTarget = PlayerObj.GetTarget(CItemCtx(), IShipController::FLAG_ACTUAL_TARGET);

	//	Check to see if the primary weapon requires autotargetting

	const CInstalledDevice *pWeapon = PlayerObj.GetNamedDevice(devPrimaryWeapon);
	if (pWeapon && pWeapon->IsEnabled())
		{
		CItemCtx ItemCtx(&PlayerObj, pWeapon);
		m_bNeedsAutoTarget = pWeapon->GetClass()->NeedsAutoTarget(ItemCtx, &m_iMinFireArc, &m_iMaxFireArc);
		}

	//	If the primary does not need it, check the missile launcher

	const CInstalledDevice *pLauncher = PlayerObj.GetNamedDevice(devMissileWeapon);
	if (pLauncher && pLauncher->IsEnabled())
		{
		CItemCtx ItemCtx(&PlayerObj, pLauncher);
		int iLauncherMinFireArc, iLauncherMaxFireArc;
		if (pLauncher->GetClass()->NeedsAutoTarget(ItemCtx, &iLauncherMinFireArc, &iLauncherMaxFireArc))
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

	//	Set up perception and max target dist

	m_iPlayerPerception = PlayerObj.GetPerception();
	m_rTargetDist2 = MAX_DISTANCE2;
	}

void CAutoTargetCalc::Update (const CSpaceObject &PlayerObj, const CSpaceObject &Obj)

//	Update
//
//	Checks to see if the given object is a better target for the player than the
//	current one. If so, we pick it as the auto target.

	{
	bool bIsAngryAtPlayer = Obj.IsAngryAt(&PlayerObj);
	CVector vDist = Obj.GetPos() - PlayerObj.GetPos();

	//	If this is the player's current target, then see if we can actually
	//	hit it with any of our weapons.

	if (m_pPlayerTarget == Obj)
		{
		Metric rDist;
		int iAngle = VectorToPolar(vDist, &rDist);

		//	If we're out of range of both the primary and the launcher, then 
		//	we remember that fact.

		if (rDist > PlayerObj.GetMaxWeaponRange())
			m_bPlayerTargetOutOfRange = true;

		//	If we have a fire arc and we're outside the arc, then we're not in
		//	range either.

		else if (m_iMinFireArc != m_iMaxFireArc
				&& !AngleInArc(iAngle, m_iMinFireArc, m_iMaxFireArc))
			m_bPlayerTargetOutOfRange = true;

		//	If we're outside detection range, then we can't keep the target

		else if (rDist > Obj.GetDetectionRange(m_iPlayerPerception))
			m_bPlayerTargetOutOfRange = true;

		//	Otherwise, if this object is angry at the player, then it is a
		//	valid auto-target

		else if (bIsAngryAtPlayer && Obj.CanBeAttacked())
			SetTargetIfBetter(Obj, rDist * rDist);
		}

	//	If the object cannot be attacked, then it is never an auto-target.

	else if (!Obj.CanBeAttacked())
		{ }

	//	If this object is not angry at us, then it can never be an auto-
	//	target.

	else if (!bIsAngryAtPlayer)
		{ }

	//	If the player's weapons has an arc of fire, then limit ourselves to
	//	targets in the arc.

	else if (m_iMinFireArc != m_iMaxFireArc)
		{
		Metric rDist;
		int iAngle = VectorToPolar(vDist, &rDist);

		if (AngleInArc(iAngle, m_iMinFireArc, m_iMaxFireArc)
				&& rDist <= Obj.GetDetectionRange(m_iPlayerPerception))
			SetTargetIfBetter(Obj, rDist * rDist);
		}

	//	Otherwise, just find the nearest target

	else
		{
		Metric rDist2 = vDist.Length2();

		if (rDist2 <= Obj.GetDetectionRange2(m_iPlayerPerception))
			SetTargetIfBetter(Obj, rDist2);
		}
	}

void CAutoTargetCalc::SetTargetIfBetter (const CSpaceObject &Obj, Metric rObjDist2)

//	SetTarget
//
//	If the given object is better than the currently selected target, then we
//	set the given object as the selected target.

	{
	//	If we don't have a current target, then we always take the given object
	//	as long as it is in range.

	if (m_pTargetObj == NULL)
		{
		if (rObjDist2 < MAX_DISTANCE2)
			{
			m_pTargetObj = &Obj;
			m_rTargetDist2 = rObjDist2;
			m_bTargetCanAttack = Obj.CanAttack();
			}
		}

	//	Otherwise, we need to see if the given object is better than the 
	//	currently selected one.

	else
		{
		bool bCanAttack = Obj.CanAttack();

		//	If the currently selected target can attack and the given object
		//	cannot, then we always skip.

		if (m_bTargetCanAttack && !bCanAttack)
			{ }

		//	Conversely, if the selected target CANNOT attack and the given 
		//	object can, we take the given object even if it is further away.

		else if (!m_bTargetCanAttack 
				&& bCanAttack
				&& rObjDist2 < MAX_DISTANCE2)
			{
			m_pTargetObj = &Obj;
			m_rTargetDist2 = rObjDist2;
			m_bTargetCanAttack = true;
			}

		//	Otherwise, if both have the same CanAttack value, then we compare
		//	based on distance.

		else if (rObjDist2 < m_rTargetDist2)
			{
			m_pTargetObj = &Obj;
			m_rTargetDist2 = rObjDist2;
			m_bTargetCanAttack = bCanAttack;
			}
		}
	}

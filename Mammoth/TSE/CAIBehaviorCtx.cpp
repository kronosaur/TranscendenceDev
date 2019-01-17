//	CAIBehaviorCtx.cpp
//
//	CAIBehaviorCtx class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

const int ATTACK_TIME_THRESHOLD =		150;
const Metric MIN_STATION_TARGET_DIST =	(10.0 * LIGHT_SECOND);
const Metric MIN_TARGET_DIST =			(5.0 * LIGHT_SECOND);
const int MULTI_HIT_WINDOW =			20;
const Metric WALL_RANGE =				(KLICKS_PER_PIXEL * 300.0);

const Metric MIN_STATION_TARGET_DIST2 =	(MIN_STATION_TARGET_DIST * MIN_STATION_TARGET_DIST);
const Metric MIN_TARGET_DIST2 =			(MIN_TARGET_DIST * MIN_TARGET_DIST);
const Metric WALL_RANGE2 =				(WALL_RANGE * WALL_RANGE);

const Metric GRAVITY_WELL_RANGE =		(KLICKS_PER_PIXEL * 800.0);
const Metric GRAVITY_WELL_RANGE2 =		(GRAVITY_WELL_RANGE * GRAVITY_WELL_RANGE);

const Metric MAX_NAV_START_DIST =		(20.0 * LIGHT_SECOND);
const Metric MAX_NAV_START_DIST2 =		(MAX_NAV_START_DIST * MAX_NAV_START_DIST);

const DWORD NAV_PATH_ID_OWNED =			0xffffffff;

//	For purposes of computing flanking distance, we assume that our target 
//	won't move faster than this. In practice, it's OK if they do, it just means
//	that there is a chance we'll be too close to turn properly.

const Metric MAX_TARGET_SPEED =			(0.25 * LIGHT_SPEED);

CAIBehaviorCtx::CAIBehaviorCtx (void) :
		m_iLastTurn(NoRotation),
		m_iLastTurnCount(0),
		m_iManeuverCounter(0),
		m_iLastAttack(0),
		m_pNavPath(NULL),
		m_iNavPathPos(-1),
		m_iBarrierClock(-1),
		m_pUpdateCtx(NULL),
		m_pShields(NULL),
		m_iBestWeapon(devNone),
		m_fDockingRequested(false),
		m_fImmobile(false),
		m_fWaitForShieldsToRegen(false),
		m_fSuperconductingShields(false),
		m_fHasMultipleWeapons(false),
		m_fHasSecondaryWeapons(false),
		m_fHasMultiplePrimaries(false),
		m_fRecalcBestWeapon(true),
		m_fHasEscorts(false),
		m_fFreeNavPath(false),
		m_fHasAvoidPotential(false)

//	CAIBehaviorCtx constructor

	{
	}

CAIBehaviorCtx::~CAIBehaviorCtx (void)

//	CAIBehaviorCtx destructor

	{
	ClearNavPath();
	}

void CAIBehaviorCtx::CalcAvoidPotential (CShip *pShip, CSpaceObject *pTarget)

//	CalcAvoidPotential
//
//	Update m_vPotential.
//
//	Sets a vector pointing away from any dangers and obstacles. The
//	size of the vector is proportional to the danger.

	{
	if (pShip->IsDestinyTime(11))
		{
		//	Start with no potential

		m_vPotential = CVector();
		m_fHasAvoidPotential = false;

		//	Set up

		Metric rDist;
		Metric rMinSeparation2 = GetMinCombatSeparation() * GetMinCombatSeparation();
		Metric rSeparationForce = g_KlicksPerPixel * 40.0 / GetMinCombatSeparation();

		CSystem *pSystem = pShip->GetSystem();
		SSpaceObjectGridEnumerator i;
		pSystem->EnumObjectsInBoxStart(i, pShip->GetPos(), Max(GRAVITY_WELL_RANGE, WALL_RANGE), gridNoBoxCheck);

		while (pSystem->EnumObjectsInBoxHasMore(i))
			{
			CSpaceObject *pObj = pSystem->EnumObjectsInBoxGetNextFast(i);

			if (pObj == NULL || pObj == pShip || pObj == pTarget || pObj->IsDestroyed())
				NULL;
			else if (pObj->HasGravity())
				{
				CVector vTarget = pObj->GetPos() - pShip->GetPos();
				Metric rTargetDist2 = vTarget.Dot(vTarget);

				//	There is a sharp potential away from gravity wells

				if (rTargetDist2 < GRAVITY_WELL_RANGE2)
					{
					CVector vTargetN = vTarget.Normal(&rDist);
					if (rDist > 0.0)
						{
						m_vPotential = m_vPotential - (vTargetN * 500.0 * g_KlicksPerPixel * (GRAVITY_WELL_RANGE / rDist));
						m_fHasAvoidPotential = true;
						}
					}
				}
			else if (pObj->Blocks(pShip))
				{
				CVector vTarget = pObj->GetPos() - pShip->GetPos();
				Metric rTargetDist2 = vTarget.Dot(vTarget);

				//	There is a sharp potential away from walls

				if (rTargetDist2 < WALL_RANGE2)
					{
					//	If we've hit a wall, then we need more precise computations because 
					//	moving aways from the center of the wall might not help.

					if (m_iBarrierClock != -1)
						{
						int iRange;
						int iAngle;
						for (iRange = 1; iRange < 8; iRange++)
							{
							Metric rRange = g_KlicksPerPixel * iRange * 10.0;
							Metric rStrength = g_KlicksPerPixel * (8 - iRange) * 10.0;

							for (iAngle = 0; iAngle < 360; iAngle += 30)
								{
								CVector vTest = PolarToVector(iAngle, 1.0);
								if (pObj->PointInObject(pObj->GetPos(), pShip->GetPos() + (rRange * vTest)))
									{
									m_vPotential = m_vPotential - (rStrength * vTest);
									m_fHasAvoidPotential = true;
									}
								}
							}
						}

					//	Otherwise, move away from the center of the wall

					else
						{
						CVector vTargetN = vTarget.Normal(&rDist);
						if (rDist > 0.0)
							{
							m_vPotential = m_vPotential - (vTargetN * 50.0 * g_KlicksPerPixel * (WALL_RANGE / rDist));
							m_fHasAvoidPotential = true;
							}
						}
					}
				}
			else if (pObj->GetCategory() == CSpaceObject::catShip)
				{
				CVector vTarget = pObj->GetPos() - pShip->GetPos();
				Metric rTargetDist2 = vTarget.Dot(vTarget);

				//	If we get too close to this ship, then move away

				if (rTargetDist2 < rMinSeparation2)
					{
					CVector vTargetN = vTarget.Normal(&rDist);
					if (rDist > 0.0)
						{
						Metric rCloseness = GetMinCombatSeparation() - rDist;
						m_vPotential = m_vPotential - (vTargetN * rSeparationForce * rCloseness);
						m_fHasAvoidPotential = true;
						}
					}
				}
			}

#ifdef DEBUG_AVOID_POTENTIAL
		pShip->SetDebugVector(m_vPotential);
#endif
		}
	}

void CAIBehaviorCtx::CalcBestWeapon (CShip *pShip, CSpaceObject *pTarget, Metric rTargetDist2)

//	CalcBestWeapon
//
//	Initializes:
//
//	m_iBestWeapon
//	m_pBestWeapon
//	m_rBestWeaponRange

	{
	int i;

	if (m_fRecalcBestWeapon)
		{
		ASSERT(pShip);

		m_rMaxWeaponRange = 0.0;

		Metric rBestRange = g_InfiniteDistance;
		int iBestWeapon = -1;
		int iBestWeaponVariant = 0;
		int iBestScore = 0;
		int iPrimaryCount = 0;
		int iBestNonLauncherLevel = 0;
		bool bHasSecondaryWeapons = false;

		//	Loop over all devices to find the best weapon

		for (i = 0; i < pShip->GetDeviceCount(); i++)
			{
			CInstalledDevice *pWeapon = pShip->GetDevice(i);
			CItemCtx ItemCtx(pShip, pWeapon);

			//	If this weapon is not working, then skip it

			if (pWeapon->IsEmpty() || !pWeapon->IsWorking())
				continue;

			//	If this is a secondary weapon, remember that we have some and 
			//	keep track of the best range.

			else if (pWeapon->IsSecondaryWeapon())
				{
				//	Remember the range in case we end up with no good weapons and we need to set 
				//	a course towards the target.

				Metric rRange = pWeapon->GetClass()->GetMaxEffectiveRange(pShip, pWeapon, pTarget);
				if (rRange < rBestRange)
					rBestRange = rRange;

				bHasSecondaryWeapons = true;
				continue;
				}

			//	Skip linked-fire weapons

			else if (pWeapon->IsLinkedFire(ItemCtx))
				continue;

			//	Otherwise, this is a primary weapon or launcher

			else
				{
				//	Compute score

				switch (pWeapon->GetCategory())
					{
					case itemcatWeapon:
						{
						int iScore = CalcWeaponScore(pShip, pTarget, pWeapon, rTargetDist2);
						if (iScore > iBestScore)
							{
							iBestWeapon = i;
							iBestWeaponVariant = 0;
							iBestScore = iScore;
							}

						Metric rMaxRange = pWeapon->GetMaxEffectiveRange(pShip);
						if (rMaxRange > m_rMaxWeaponRange)
							m_rMaxWeaponRange = rMaxRange;

                        int iWeaponLevel = pWeapon->GetLevel();
						if (!pWeapon->GetClass()->IsAmmoWeapon()
								&& iWeaponLevel > iBestNonLauncherLevel)
							iBestNonLauncherLevel = iWeaponLevel;

						iPrimaryCount++;
						break;
						}

					case itemcatLauncher:
						{
						int iCount = pShip->GetMissileCount();
						if (iCount > 0)
							{
							pShip->ReadyFirstMissile();

							for (int j = 0; j < iCount; j++)
								{
								int iScore = CalcWeaponScore(pShip, pTarget, pWeapon, rTargetDist2);

								//	If we only score 1 and we've got secondary weapons, then don't
								//	bother with this missile (we don't want to waste it)

								if (iScore == 1 && HasSecondaryWeapons())
									{
									iScore = 0;

									//	Remember the range in case we end up with no good weapons and we need to set 
									//	a course towards the target.

									Metric rRange = pWeapon->GetClass()->GetMaxEffectiveRange(pShip, pWeapon, pTarget);
									if (rRange < rBestRange)
										rBestRange = rRange;

									if (rRange > m_rMaxWeaponRange)
										m_rMaxWeaponRange = rRange;
									}

								if (iScore > iBestScore)
									{
									iBestWeapon = i;
									iBestWeaponVariant = j;
									iBestScore = iScore;
									}

								pShip->ReadyNextMissile();
								}

							iPrimaryCount++;
							}
						break;
						}
					}
				}
			}

		//	Given the best weapon, select the named device

		if (iBestWeapon != -1)
			{
			m_iBestWeapon = pShip->SelectWeapon(iBestWeapon, iBestWeaponVariant);
			m_pBestWeapon = pShip->GetNamedDevice(m_iBestWeapon);
			m_rBestWeaponRange = m_pBestWeapon->GetClass()->GetMaxEffectiveRange(pShip, m_pBestWeapon, pTarget);

			//	Optimum range varies by ship (destiny)

			int iAdj = 100 + ((pShip->GetDestiny() % 60) - 30);
			m_rBestWeaponRange = m_rBestWeaponRange * (iAdj * 0.01);
			}
		else
			{
			m_iBestWeapon = devNone;
			m_pBestWeapon = NULL;

			//	If we can't find a good weapon, at least set the weapon range so that we close
			//	to secondary weapon range.

			if (bHasSecondaryWeapons)
				m_rBestWeaponRange = rBestRange;
			else
				m_rBestWeaponRange = 60.0 * LIGHT_SECOND;
			}

		//	Adjust by AI settings

		m_rBestWeaponRange = m_rBestWeaponRange * m_AISettings.GetFireRangeAdj() * 0.01;

		//	Set some invariants

		m_fHasSecondaryWeapons = bHasSecondaryWeapons;
		m_iBestNonLauncherWeaponLevel = iBestNonLauncherLevel;
		m_fHasMultipleWeapons = (iPrimaryCount > 1);

		//	Done

		m_fRecalcBestWeapon = false;
		}
	}

void CAIBehaviorCtx::CalcInvariants (CShip *pShip)

//	CalcInvariants
//
//	Calculates some invariant properties of the ship

	{
	int i;

	//	Basic properties

	m_fImmobile = (pShip->GetMaxSpeed() == 0.0);

	//	Primary aim range

	Metric rPrimaryRange = pShip->GetWeaponRange(devPrimaryWeapon);
	Metric rAimRange = (GetFireRangeAdj() * rPrimaryRange) / (100.0 + ((pShip->GetDestiny() % 8) + 4));
	if (rAimRange < 1.5 * MIN_TARGET_DIST)
		rAimRange = 1.5 * MIN_TARGET_DIST;
	m_rPrimaryAimRange2 = rAimRange * rAimRange;

	//	Compute the minimum flanking distance. If we're very maneuverable,
	//	can get in closer because we can turn faster to adjust for the target's
	//	motion.

	Metric rDegreesPerTick = Max(1.0, Min(pShip->GetRotationDesc().GetMaxRotationSpeedDegrees(), 60.0));
	Metric rTanRot = tan(PI * rDegreesPerTick / 180.0);
	Metric rMinFlankDist = Max(MIN_TARGET_DIST, MAX_TARGET_SPEED / rTanRot);

	//	Adjust a little based on destiny so we get some variation, even between
	//	ships of the same class.

	rMinFlankDist *= 1.0 + (0.5 * (pShip->GetDestiny() / 360.0));

	//	And, of course, we can't ever flank outside our weapon range

	m_rFlankDist = Min(rAimRange, rMinFlankDist);

	//	Max turn count

	int iFullRotationTime = Max(1, pShip->GetRotationDesc().GetMaxRotationTimeTicks());
	m_iMaxTurnCount = iFullRotationTime * (1 + (pShip->GetDestiny() % 6));

	//	Chance of premature fire based on turn rate

	m_iPrematureFireChance = (6 * (100 - m_AISettings.GetFireAccuracy())) / iFullRotationTime;

	//	Compute some properties of installed devices

	m_pShields = NULL;
	m_fSuperconductingShields = false;
	m_iBestNonLauncherWeaponLevel = 0;
	m_fHasSecondaryWeapons = false;

	int iPrimaryCount = 0;

	for (i = 0; i < pShip->GetDeviceCount(); i++)
		{
		CInstalledDevice *pDevice = pShip->GetDevice(i);

		if (pDevice->IsEmpty() || !pDevice->IsWorking())
			continue;

		switch (pDevice->GetCategory())
			{
			case itemcatWeapon:
			case itemcatLauncher:
				{
				//	Figure out the best non-launcher level

                int iWeaponLevel = pDevice->GetLevel();
				if (pDevice->GetCategory() != itemcatLauncher
						&& !pDevice->GetClass()->IsAmmoWeapon()
						&& iWeaponLevel > m_iBestNonLauncherWeaponLevel)
					{
					m_iBestNonLauncherWeaponLevel = iWeaponLevel;
					}

				//	Secondary

				if (pDevice->IsSecondaryWeapon())
					m_fHasSecondaryWeapons = true;
				else if (pDevice->GetCategory() == itemcatWeapon)
					iPrimaryCount++;

				break;
				}

			case itemcatShields:
				m_pShields = pDevice;
				if (pDevice->GetClass()->GetUNID() == g_SuperconductingShieldsUNID)
					m_fSuperconductingShields = true;
				break;
			}
		}

	//	Flags

	m_fHasMultiplePrimaries = (iPrimaryCount > 1);
	m_fThrustThroughTurn = ((pShip->GetDestiny() % 100) < 50);
	m_fAvoidExplodingStations = (rAimRange > MIN_STATION_TARGET_DIST);

	//	Weapon

	m_fRecalcBestWeapon = true;
	CalcBestWeapon(pShip, NULL, 0.0);
	}

bool CAIBehaviorCtx::CalcIsBetterTarget (CShip *pShip, CSpaceObject *pCurTarget, CSpaceObject *pNewTarget) const

//	CalcIsBetterTarget
//
//	Returns TRUE if the new target is better than the existing one.

	{
	//	The new target must be a real target

	if (pNewTarget == NULL 
			|| pNewTarget->IsDestroyed()
			|| !pNewTarget->CanAttack()
			|| !pShip->IsEnemy(pNewTarget))
		return false;

	//	See if the new target is better

	else
		{
		CPerceptionCalc Perception(pShip->GetPerception());

		//	Compute the distance to the new target

		Metric rDist2 = (pNewTarget->GetPos() - pShip->GetPos()).Length2();

		//	See if the new target is visible to us. If not, then it cannot be a
		//	better target.

		if (!Perception.CanBeTargeted(pNewTarget, rDist2))
			return false;

		//	If the current target is not valid, then we always switch

		else if (pCurTarget == NULL
				|| pCurTarget->IsDestroyed())
			return true;

		//	There is a 20% chance that we automatically switch to the new target,
		//	regardless of its distance.

		else if (mathRandom(1, 100) <= 20)
			return true;

		//	Otherwise, we see if the attacker is closer than the current target.
		//	If it is, then switch to it.

		else
			{
			Metric rCurTargetDist2 = (pCurTarget->GetPos() - pShip->GetPos()).Length2();
			if (rDist2 < rCurTargetDist2)
				return true;
			else
				return false;
			}
		}
	}

bool CAIBehaviorCtx::CalcNavPath (CShip *pShip, const CVector &vTo)

//	CalcNavPath
//
//	Initializes m_pNavPath and m_iNavPathPos.

	{
	//	We always create a new nav path

	CNavigationPath *pPath;
	CNavigationPath::Create(pShip->GetSystem(), pShip->GetSovereign(), pShip->GetPos(), vTo, &pPath);

	//	Done (we own this nav path, so we pass TRUE).

	SetNavPath(pPath, 0, true);
	return true;
	}

bool CAIBehaviorCtx::CalcNavPath (CShip *pShip, CSpaceObject *pTo)

//	CalcNavPath
//
//	Initializes m_pNavPath and m_iNavPathPos. Call this function
//	before using ImplementFollowNavPath

	{
	int i;
	CSystem *pSystem = pShip->GetSystem();

	ASSERT(pTo);

	//	If the destination moves (e.g., is a ship) then we place a nav path to
	//	where it is currenly and allow the code to recalc nav paths as
	//	appropriate.

	if (pTo->CanThrust())
		return CalcNavPath(pShip, pTo->GetPos());

	//	Figure out an appropriate starting point

	CSpaceObject *pBestObj = NULL;
	Metric rBestDist2 = MAX_NAV_START_DIST2;
	for (i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);
		if (pObj
				&& pObj != pTo
				&& (pObj->IsActiveStargate()
					|| pObj->IsMarker()
					|| (pObj->GetCategory() == CSpaceObject::catStation
						&& pObj->GetScale() == scaleStructure
						&& !pObj->IsIntangible()
						&& pObj->CanObjRequestDock(pShip)
						&& (pObj->IsFriend(pShip) || !pObj->CanAttack()))))
			{
			Metric rDist2 = (pObj->GetPos() - pShip->GetPos()).Length2();
			if (rDist2 < rBestDist2)
				{
				pBestObj = pObj;
				rBestDist2 = rDist2;
				}
			}
		}

	//	If we couldn't find a suitable object, then we just create a private
	//	nav path (which we need to delete later).

	if (pBestObj == NULL)
		return CalcNavPath(pShip, pTo->GetPos());

	//	Get the appropriate nav path from the system

	CSpaceObject *pFrom = pBestObj;
	CNavigationPath *pPath = pSystem->GetNavPath(pShip->GetSovereign(), pFrom, pTo);

	//	Done

	CalcNavPath(pShip, pPath);
	return true;
	}

void CAIBehaviorCtx::CalcNavPath (CShip *pShip, CSpaceObject *pFrom, CSpaceObject *pTo)

//	CalcNavPath
//
//	Initializes m_pNavPath and m_iNavPathPos. Call this function
//	before using ImplementFollowNavPath

	{
	ASSERT(pFrom);
	ASSERT(pTo);

	//	Get the appropriate nav path from the system

	CSystem *pSystem = pShip->GetSystem();
	CNavigationPath *pPath = pSystem->GetNavPath(pShip->GetSovereign(), pFrom, pTo);

	//	Done

	CalcNavPath(pShip, pPath);
	}

void CAIBehaviorCtx::CalcNavPath (CShip *pShip, CNavigationPath *pPath, bool bOwned)

//	CalcNavPath
//
//	Initializes ship state to follow the given path

	{
	int i;

	ASSERT(pPath);

	//	Figure out which nav position we are closest to

	const Metric CLOSE_ENOUGH_DIST = (LIGHT_SECOND * 10.0);
	const Metric CLOSE_ENOUGH_DIST2 = CLOSE_ENOUGH_DIST * CLOSE_ENOUGH_DIST;
	Metric rBestDist2 = (g_InfiniteDistance * g_InfiniteDistance);
	int iBestPoint = -1;

	for (i = 0; i < pPath->GetNavPointCount(); i++)
		{
		CVector vDist = pPath->GetNavPoint(i) - pShip->GetPos();
		Metric rDist2 = vDist.Length2();

		if (rDist2 < rBestDist2)
			{
			rBestDist2 = rDist2;
			iBestPoint = i;

			if (rDist2 < CLOSE_ENOUGH_DIST2)
				break;
			}
		}

	//	Done

	ASSERT(iBestPoint != -1);
	if (iBestPoint == -1)
		iBestPoint = 0;

	SetNavPath(pPath, iBestPoint, bOwned);
	}

void CAIBehaviorCtx::CalcShieldState (CShip *pShip)

//	CalcShieldState
//
//	Updates m_fWaitForShieldsToRegen

	{
	if (m_pShields
			&& !NoShieldRetreat()
			&& pShip->IsDestinyTime(17) 
			&& !m_fSuperconductingShields)
		{
		int iHPLeft, iMaxHP;
		m_pShields->GetStatus(pShip, &iHPLeft, &iMaxHP);

		//	If iMaxHP is 0 then we treat the shields as up. This can happen
		//	if a ship with (e.g.) hull-plate ionizer gets its armor destroyed

		int iLevel = (iMaxHP > 0 ? (iHPLeft * 100 / iMaxHP) : 100);

		//	If we're waiting for shields to regenerate, see if
		//	they are back to 60% or better

		if (m_fWaitForShieldsToRegen)
			{
			if (iLevel >= 60)
				m_fWaitForShieldsToRegen = false;
			}

		//	Otherwise, if shields are down, then we need to wait
		//	until they regenerate

		else
			{
			if (iLevel <= 0)
				m_fWaitForShieldsToRegen = true;
			}
		}
	}

int CAIBehaviorCtx::CalcWeaponScore (CShip *pShip, CSpaceObject *pTarget, CInstalledDevice *pWeapon, Metric rTargetDist2)

//	CalcWeaponScore
//
//	Calculates a score for this weapon

	{
	int iScore = 0;

	//	If this is an EMP weapon adjust the score based on the state of
	//	the target.

	int iEffectiveness = pWeapon->GetClass()->GetWeaponEffectiveness(pShip, pWeapon, pTarget);
	if (iEffectiveness < 0)
		return 0;

	//	Get the range of this weapon

	Metric rRange2 = pWeapon->GetClass()->GetMaxEffectiveRange(pShip, pWeapon, pTarget);
	rRange2 *= rRange2;

	//	If the weapon is out of range of the target then we score 1
	//	(meaning that it is better than nothing (0) but we would rather any
	//	other weapon)

	if (rRange2 < rTargetDist2)
		return 1;

	//	If this weapon will take a while to get ready, then 
	//	lower the score.

	if (pWeapon->GetTimeUntilReady() >= 15)
		return 1;

	//	Get the item for the selected variant (either the weapon
	//	or the ammo)

	CItemType *pType;
	pWeapon->GetClass()->GetSelectedVariantInfo(pShip,
			pWeapon,
			NULL,
			NULL,
			&pType);

	//	Base score is based on the level of the variant

    int iLevel = (pType->IsDevice() ? pWeapon->GetLevel() : pType->GetLevel());
	iScore += iLevel * 10;

	//	Missiles/ammo count for more

	if (pWeapon->GetCategory() == itemcatLauncher
			|| pWeapon->GetClass()->IsAmmoWeapon())
		{
		//	Don't waste missiles on "lesser" targets

		if (pTarget 
				&& pTarget->GetCategory() == CSpaceObject::catShip
				&& !pTarget->IsMultiHull()
				&& pTarget->GetLevel() <= (m_iBestNonLauncherWeaponLevel - 2)
				&& pTarget->GetLevel() <= (iLevel - 2)
				&& !pTarget->IsPlayer())
			return 1;

		//	Otherwise, count for more

		else
			iScore += 20;
		}

	//	Adjust score based on effectiveness

	iScore += iEffectiveness;

	//	If we have multiple primaries, then include damage type effectiveness against
	//	the target.

	if (pTarget && m_fHasMultiplePrimaries)
		{
		int iDamageEffect = pTarget->GetDamageEffectiveness(pShip, pWeapon);
		if (iDamageEffect < 0)
			return 0;
		else
			iScore += (iDamageEffect / 10);
		}

	//	If this weapon aligned, then prefer this weapon

	if (pTarget && pWeapon->IsWeaponAligned(pShip, pTarget))
		iScore += 10;

	//	If this is an area weapon then make sure there aren't too many friendlies around

#if 0
	if (iScore > 0 && pWeapon->IsAreaWeapon(pShip))
		{
		int i;
		CSystem *pSystem = pShip->GetSystem();
		int iFireAngle = (pShip->GetRotation() + pWeapon->GetRotation()) % 360;

		for (i = 0; i < pSystem->GetObjectCount(); i++)
			{
			CSpaceObject *pObj = pSystem->GetObject(i);

			if (pObj && pObj != pShip
					&& pObj->CanAttack()
					&& !IsEnemy(pObj))
				{
				CVector vDist = GetPos() - pObj->GetPos();
				Metric rDist2 = vDist.Length2();

				if (rDist2 < MAX_AREA_WEAPON_CHECK2)
					{
					int iBearing = VectorToPolar(vDist);
					if (AreAnglesAligned(iFireAngle, iBearing, 170))
						return 1;
					}
				}
			}
		}
#endif

	return iScore;
	}

void CAIBehaviorCtx::CancelDocking (CShip *pShip, CSpaceObject *pBase)

//	CancelDocking
//
//	Cancel docking that we've previously requested.

	{
	//	HACK: If pBase is NULL then we need to search the entire system for the
	//	base that we're trying to dock with. This is because we don't bother
	//	storing the obj that we've request to dock with.

	if (pBase == NULL)
		{
		int i;
		CSystem *pSystem = pShip->GetSystem();
		for (i = 0; i < pSystem->GetObjectCount(); i++)
			{
			CSpaceObject *pObj = pSystem->GetObject(i);
			if (pObj && pObj->IsObjDockedOrDocking(pShip))
				{
				pBase = pObj;
				break;
				}
			}

		if (pBase == NULL)
			return;
		}

	//	Cancel

	pBase->Undock(pShip);
	SetDockingRequested(false);
	}

void CAIBehaviorCtx::ClearNavPath (void)

//	ClearNavPath
//
//	Clears the nav path
	
	{
	if (m_pNavPath)
		{
		if (m_fFreeNavPath)
			delete m_pNavPath;

		m_pNavPath = NULL;
		m_iNavPathPos = -1;
		m_fFreeNavPath = false;
		}
	}

void CAIBehaviorCtx::CommunicateWithEscorts (CShip *pShip, MessageTypes iMessage, CSpaceObject *pParam1, DWORD dwParam2)

//	CommunicateWithEscorts
//
//	Sends a message to the ship's escorts

	{
	DEBUG_TRY

	if (HasEscorts())
		{
		bool bEscortsFound = false;

		CSovereign *pSovereign = pShip->GetSovereign();
		for (int i = 0; i < pShip->GetSystem()->GetObjectCount(); i++)
			{
			CSpaceObject *pObj = pShip->GetSystem()->GetObject(i);

			if (pObj 
					&& pObj->GetCategory() == CSpaceObject::catShip
					&& pObj != pShip
					&& pObj->CanAttack()	//	Excludes attached ship sections
					&& pObj->GetEscortPrincipal() == pShip)
				{
				pShip->Communicate(pObj, iMessage, pParam1, dwParam2);
				bEscortsFound = true;
				}
			}

		if (!bEscortsFound)
			SetHasEscorts(false);
		}

	DEBUG_CATCH
	}

void CAIBehaviorCtx::DebugPaintInfo (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	DebugPaintInfo
//
//	Paint debug info

	{
	if (m_pNavPath && g_pUniverse->GetDebugOptions().IsShowNavPathsEnabled())
		m_pNavPath->DebugPaintInfo(Dest, x, y, Ctx.XForm);
	}

bool CAIBehaviorCtx::IsBeingAttacked (int iThreshold) const 

//	IsBeingAttacked
//
//	Returns TRUE if we've been attacked recently.

	{
	return (g_pUniverse->GetTicks() - m_iLastAttack) <= iThreshold;
	}

bool CAIBehaviorCtx::IsSecondAttack (void) const

//	IsSecondAttack
//
//	This is called when we are attacked. We return TRUE if this is the second
//	attack within a certain period of time and if it is not too close to the
//	previous attack (so it is a deliberate second hit).

	{
	int iInterval = g_pUniverse->GetTicks() - GetLastAttack();
	return (iInterval > MULTI_HIT_WINDOW && iInterval < 3 * ATTACK_TIME_THRESHOLD);
	}

void CAIBehaviorCtx::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Load the structure from a stream
//
//	CAISettings	m_AISettings
//	CAIShipControls m_ShipControls
//
//	DWORD		m_iLastTurn
//	DWORD		m_iLastTurnCount
//	DWORD		m_iManeuverCounter
//	DWORD		m_iLastAttack
//	CVector		m_vPotential
//	DWORD		m_pNavPath (ID)
//	DWORD		m_iNavPathPos
//
//	DWORD		flags

	{
	DWORD dwLoad;

	//	CAISettings

	m_AISettings.ReadFromStream(Ctx);
	m_ShipControls.ReadFromStream(Ctx);

	//	State

	Ctx.pStream->Read((char *)&m_iLastTurn, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iLastTurnCount, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iManeuverCounter, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_iLastAttack, sizeof(DWORD));
	Ctx.pStream->Read((char *)&m_vPotential, sizeof(CVector));

	if (Ctx.dwVersion >= 112)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_iBarrierClock = (int)dwLoad;
		}
	else
		m_iBarrierClock = -1;

	//	Nav path

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	if (dwLoad == 0)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_pNavPath = NULL;
		m_iNavPathPos = -1;
		m_fFreeNavPath = false;
		}
	else if (dwLoad == NAV_PATH_ID_OWNED)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_iNavPathPos = (int)dwLoad;

		m_pNavPath = new CNavigationPath;
		m_pNavPath->OnReadFromStream(Ctx);
		m_fFreeNavPath = true;
		}
	else
		{
		DWORD dwNavPathPos;
		Ctx.pStream->Read((char *)&dwNavPathPos, sizeof(DWORD));
		m_iNavPathPos = (int)dwNavPathPos;

		m_pNavPath = Ctx.pSystem->GetNavPathByID(dwLoad);
		if (m_pNavPath == NULL)
			m_iNavPathPos = -1;
		m_fFreeNavPath = false;
		}

	//	Flags

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_fDockingRequested =		((dwLoad & 0x00000001) ? true : false);
	m_fWaitForShieldsToRegen =	((dwLoad & 0x00000002) ? true : false);
	m_fHasEscorts =				((dwLoad & 0x00000004) ? true : false);

	//	These flags do not need to be saved

	m_fRecalcBestWeapon = true;
	m_fHasAvoidPotential = (m_vPotential.Length2() > 1.0);
	}

void CAIBehaviorCtx::SetBarrierClock (CShip *pShip)

//	SetBarrierClock
//
//	This gets called when we hit a barrier.

	{
	//	If our clock is already set, we're still trying to deal with the 
	//	barrier from the last hit.

	if (m_iBarrierClock != -1)
		return;

	//	Set our clock

	int iFullRotationTime = Max(1, pShip->GetRotationDesc().GetMaxRotationTimeTicks());
	m_iBarrierClock = 30 + iFullRotationTime;
	}

void CAIBehaviorCtx::SetLastAttack (int iTick)

//	SetLastAttack
//
//	Set the last attack time (debouncing for quick hits)

	{
	if (iTick - m_iLastAttack > MULTI_HIT_WINDOW)
		m_iLastAttack = iTick;
	}

void CAIBehaviorCtx::Undock (CShip *pShip)

//	Undock
//
//	Make sure we are undocked so that we can fly around

	{
	//	If docking has been requested the we need to cancel it

	if (IsDockingRequested())
		CancelDocking(pShip, NULL);
	
	//	Otherwise, make sure we are undocked. We rely on the fact that Undock
	//	is a NoOp if we're not docked.

	else
		pShip->Undock();
	}

void CAIBehaviorCtx::Update (CShip *pShip)

//	Update
//
//	Called before we start processing

	{
	if (!IsDockingRequested())
		{
		SetManeuver(NoRotation);
		SetThrustDir(CAIShipControls::constNeverThrust);
		}

	if (m_iBarrierClock != -1)
		m_iBarrierClock--;

	//	Clear debug highlight

	if (g_pUniverse->GetDebugOptions().IsShowAIDebugEnbled())
		pShip->Highlight();
	}

void CAIBehaviorCtx::WriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	WriteToStream
//
//	Write the structure
//
//	CAISettings	m_AISettings
//	CAIShipControls m_ShipControls
//
//	DWORD		m_iLastTurn
//	DWORD		m_iLastTurnCount
//	DWORD		m_iManeuverCounter
//	DWORD		m_iLastAttack
//	CVector		m_vPotential
//	DWORD		m_iBarrierClock
//	DWORD		m_pNavPath (ID)
//	DWORD		m_iNavPathPos
//
//	DWORD		flags

	{
	DWORD dwSave;

	//	CAISettings

	m_AISettings.WriteToStream(pStream);
	m_ShipControls.WriteToStream(pSystem, pStream);

	//	State

	pStream->Write((char *)&m_iLastTurn, sizeof(DWORD));
	pStream->Write((char *)&m_iLastTurnCount, sizeof(DWORD));
	pStream->Write((char *)&m_iManeuverCounter, sizeof(DWORD));
	pStream->Write((char *)&m_iLastAttack, sizeof(DWORD));
	pStream->Write((char *)&m_vPotential, sizeof(CVector));

	dwSave = m_iBarrierClock;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	If we don't have a nav path, just write out 0

	if (m_pNavPath == NULL)
		{
		dwSave = 0;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		dwSave = m_iNavPathPos;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}

	//	If we have a shared nav path, write out the nav path ID

	else if (!m_fFreeNavPath)
		{
		dwSave = (m_pNavPath ? m_pNavPath->GetID() : 0);
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		dwSave = m_iNavPathPos;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}

	//	Otherwise we need to save the nav path here.

	else
		{
		dwSave = NAV_PATH_ID_OWNED;
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		dwSave = m_iNavPathPos;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		m_pNavPath->OnWriteToStream(pSystem, pStream);
		}

	//	Flags

	dwSave = 0;
	dwSave |= (m_fDockingRequested ?		0x00000001 : 0);
	dwSave |= (m_fWaitForShieldsToRegen ?	0x00000002 : 0);
	dwSave |= (m_fHasEscorts ?				0x00000004 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}

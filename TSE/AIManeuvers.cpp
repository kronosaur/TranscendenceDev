//	AIManeuvers.cpp
//
//	Implementation of IOrderModule class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

const Metric ATTACK_RANGE =				(20.0 * LIGHT_SECOND);
const Metric CLOSE_RANGE =				(50.0 * LIGHT_SECOND);
const Metric DEFAULT_DIST_CHECK =		(700.0 * KLICKS_PER_PIXEL);
const Metric DOCKING_APPROACH_DISTANCE = (200.0 * KLICKS_PER_PIXEL);
const Metric ESCORT_DISTANCE =			(6.0 * LIGHT_SECOND);
const Metric HIT_NAV_POINT_DIST =		(24.0 * LIGHT_SECOND);
const Metric MAX_DELTA =				(2.0 * KLICKS_PER_PIXEL);
const Metric MAX_DELTA_VEL =			(g_KlicksPerPixel / 2.0);
const Metric MAX_DISTANCE =				(400 * KLICKS_PER_PIXEL);
const Metric MAX_DOCK_DISTANCE =		(15.0 * LIGHT_SECOND);
const Metric MAX_ESCORT_DISTANCE =		(12.0 * LIGHT_SECOND);
const Metric MAX_GATE_DISTANCE =		(90.0 * KLICKS_PER_PIXEL);
const Metric MAX_IN_FORMATION_DELTA	=	(2.0 * KLICKS_PER_PIXEL);
const Metric MAX_TARGET_OF_OPPORTUNITY_RANGE = (20.0 * LIGHT_SECOND);
const int MAX_TARGETS =					10;
const Metric MIN_FLYBY_SPEED =			(2.0 * KLICKS_PER_PIXEL);
const Metric MIN_POTENTIAL2 =			(KLICKS_PER_PIXEL* KLICKS_PER_PIXEL * 25.0);
const Metric MIN_STATION_TARGET_DIST =	(10.0 * LIGHT_SECOND);
const Metric MIN_TARGET_DIST =			(3.0 * LIGHT_SECOND);
const Metric MAX_HEADING_DELTA_V =		g_KlicksPerPixel;
const Metric MAX_HEADING_DELTA_V2 =		MAX_HEADING_DELTA_V * MAX_HEADING_DELTA_V;

const Metric CLOSE_RANGE2 =				(CLOSE_RANGE * CLOSE_RANGE);
const Metric HIT_NAV_POINT_DIST2 =		(HIT_NAV_POINT_DIST * HIT_NAV_POINT_DIST);
const Metric MAX_DELTA2 =				(MAX_DELTA * MAX_DELTA);
const Metric MAX_DELTA_VEL2 =			(MAX_DELTA_VEL * MAX_DELTA_VEL);
const Metric MAX_IN_FORMATION_DELTA2 =	(MAX_IN_FORMATION_DELTA * MAX_IN_FORMATION_DELTA);
const Metric MIN_FLYBY_SPEED2 =			(MIN_FLYBY_SPEED * MIN_FLYBY_SPEED);
const Metric MIN_STATION_TARGET_DIST2 =	(MIN_STATION_TARGET_DIST * MIN_STATION_TARGET_DIST);
const Metric MIN_TARGET_DIST2 =			(MIN_TARGET_DIST * MIN_TARGET_DIST);

const Metric MAX_FLOCK_DIST =			(600.0 * KLICKS_PER_PIXEL);
const Metric MAX_FLOCK_DIST2 =			(MAX_FLOCK_DIST * MAX_FLOCK_DIST);
const Metric FLOCK_SEPARATION_RANGE =	(100.0 * KLICKS_PER_PIXEL);
const Metric FLOCK_SEPARATION_RANGE2 =	(FLOCK_SEPARATION_RANGE * FLOCK_SEPARATION_RANGE);
const Metric FLOCK_COMBAT_RANGE =		(300.0 * KLICKS_PER_PIXEL);
const Metric FLOCK_COMBAT_RANGE2 =		(FLOCK_COMBAT_RANGE * FLOCK_COMBAT_RANGE);

const Metric CLOSE_DELTA_V_RATIO =		0.12;
const Metric MIN_SPEED_RATIO =			0.01;
const Metric FORMATION_MAX_TIME =		5.0;
const Metric POTENTIAL_TO_POS_ADJ = 100.0;

const Metric MAX_IMMOBILE_SPEED =		(1.0 * KLICKS_PER_PIXEL);
const Metric MAX_IMMOBILE_SPEED2 =		MAX_IMMOBILE_SPEED * MAX_IMMOBILE_SPEED;

#ifdef DEBUG_COMBAT
#define DEBUG_COMBAT_OUTPUT(x)			{ pShip->Highlight(strPatternSubst(CONSTLIT("%d: %s"), pShip->GetID(), CString(x))); }
#else
#define DEBUG_COMBAT_OUTPUT(x)
#endif

extern int g_iDebugLine;

void CAIBehaviorCtx::CalcEscortFormation (CShip *pShip, CSpaceObject *pLeader, CVector *retvPos, CVector *retvVel, int *retiFacing)

//	CalcEscortFormation
//
//	Computes the position of the escort

	{
	//	If we're flocking, use flocking algorithm (if a flock is available).

	if (m_AISettings.IsFlocker()
			&& CalcFlockingFormation(pShip, pLeader, retvPos, retvVel, retiFacing))
		;

	//	Otherwise, we do a normal escort.
	//	Pick escort position relative to our current position
	//	at time = 0

	else
		{
		CVector vEscortPos;

		//	Escort position is encoded in order data

		IShipController::SData Data;
		pShip->GetCurrentOrder(NULL, &Data);
		if (Data.IsIntegerOrPair())
			{
			int iAngle = Data.AsInteger();
			int iDistance = Data.AsInteger2();
			if (iDistance == 0)
				iDistance = 6;

			vEscortPos = PolarToVector(pLeader->GetRotation() + iAngle, iDistance * LIGHT_SECOND);
			}

		//	Otherwise, generate a position

		else
			{
			int iAngle = (pLeader->GetRotation() + 45 + 5 * (pShip->GetDestiny() % 54)) % 360;
			Metric rRadius = ESCORT_DISTANCE * (0.75 + (pShip->GetDestiny() / 360.0));
			vEscortPos = PolarToVector(iAngle, rRadius);
			}

		//	Compute absolute positions

		*retvPos = pLeader->GetPos() + vEscortPos;
		*retvVel = pLeader->GetVel();
		*retiFacing = pLeader->GetRotation();
		}
	}

bool CAIBehaviorCtx::CalcFlockingFormation (CShip *pShip,
											CSpaceObject *pLeader,
											CVector *retvPos, 
											CVector *retvVel, 
											int *retiFacing)

//	CalcFlockingFormation
//
//	Calculates the position that this ship should take relative to the rest of the flock. Returns FALSE
//	if the current ship is a leader in the flock.

	{
	DEBUG_TRY

	switch (m_AISettings.GetFlockingStyle())
		{
		case CAISettings::flockCompact:
			//	LATER: Not Yet Implemented.
			return CalcFlockingFormationRandom(pShip, pLeader, retvPos, retvVel, retiFacing);

		case CAISettings::flockRandom:
			return CalcFlockingFormationRandom(pShip, pLeader, retvPos, retvVel, retiFacing);

		default:
			return CalcFlockingFormationCloud(pShip, pLeader, MAX_FLOCK_DIST, FLOCK_SEPARATION_RANGE, retvPos, retvVel, retiFacing);
		}

	DEBUG_CATCH
	}

bool CAIBehaviorCtx::CalcFlockingFormationCloud (CShip *pShip, CSpaceObject *pLeader, Metric rFOVRange, Metric rSeparationRange, CVector *retvPos, CVector *retvVel, int *retiFacing)

//	CalcFlockingFormation
//
//	Calculates the position that this ship should take relative to the rest of the flock. Returns FALSE
//	if the current ship is a leader in the flock.

	{
	int i;
	CVector vFlockPos;
	CVector vFlockVel;
	CVector vFlockHeading;
	CVector vAvoid;
	Metric rFOVRange2 = rFOVRange * rFOVRange;
	Metric rSeparationRange2 = rSeparationRange * rSeparationRange;
	Metric rFlockCount = 0.0;
	Metric rAvoidCount = 0.0;

	for (i = 0; i < pShip->GetSystem()->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pShip->GetSystem()->GetObject(i);

		if (pObj 
				&& pObj->GetSovereign() == pShip->GetSovereign()
				&& pObj->GetCategory() == CSpaceObject::catShip
				&& !pObj->IsIntangible()
				&& pObj != pShip
				&& pObj != pLeader)
			{
			CVector vTarget = pObj->GetPos() - pShip->GetPos();
			Metric rTargetDist2 = vTarget.Dot(vTarget);

			//	Only consider ships within a certain range

			if (rTargetDist2 < rFOVRange2)
				{
				CVector vTargetRot = vTarget.Rotate(360 - pShip->GetRotation());

				//	Only consider ships in front of us

				if (vTargetRot.GetX() > 0.0)
					{
					//	Only ships of a certain destiny

					if (pObj->GetDestiny() > pShip->GetDestiny())
						{
						vFlockPos = vFlockPos + vTarget;
						vFlockVel = vFlockVel + (pObj->GetVel() - pShip->GetVel());
						vFlockHeading = vFlockHeading + PolarToVector(pObj->GetRotation(), 1.0);
						rFlockCount = rFlockCount + 1.0;
						}

					//	Avoid ships that are too close

					if (rTargetDist2 < rSeparationRange2)
						{
						vAvoid = vAvoid + vTarget;
						rAvoidCount = rAvoidCount + 1.0;
						}
					}
				}
			}
		}

	//	If we've got a leader, add separately

	if (pLeader)
		{
		CVector vTarget = pLeader->GetPos() - pShip->GetPos();
		Metric rTargetDist2 = vTarget.Dot(vTarget);

		vFlockPos = vFlockPos + vTarget;
		vFlockVel = vFlockVel + (pLeader->GetVel() - pShip->GetVel());
		vFlockHeading = vFlockHeading + PolarToVector(pLeader->GetRotation(), 1.0);
		rFlockCount = rFlockCount + 1.0;

		//	Avoid ships that are too close

		if (rTargetDist2 < rSeparationRange2)
			{
			vAvoid = vAvoid + vTarget;
			rAvoidCount = rAvoidCount + 1.0;
			}
		}

	//	Compute the averages

	if (rFlockCount > 0.0)
		{
		CVector vAimPos = (vFlockPos / rFlockCount);
		if (rAvoidCount > 0.0)
			{
			int iAimAngle = VectorToPolar(vAimPos);

			CVector vAvoidAverage = (vAvoid / rAvoidCount);
			CVector vAvoidRot = vAvoidAverage.Rotate(360 - iAimAngle);

			CVector vAimPerp = vAimPos.Normal().Perpendicular();

			Metric rAvoidAverage = vAvoidAverage.Length();
			Metric rAvoidMag = 2.0 * (rSeparationRange - rAvoidAverage);

			CVector vAvoidAdj;
			if (rAvoidMag > 0.0)
				{
				if (vAvoidRot.GetY() > 0.0)
					vAvoidAdj = -rAvoidMag * vAimPerp;
				else
					vAvoidAdj = rAvoidMag * vAimPerp;
				}

			vAimPos = vAimPos + vAvoidAdj;
			}

		*retvPos = pShip->GetPos() + vAimPos;
		*retvVel = pShip->GetVel() + (vFlockVel / rFlockCount);
		*retiFacing = VectorToPolar(vFlockHeading);

		return true;
		}
	else
		{
		return false;
		}
	}

bool CAIBehaviorCtx::CalcFlockingFormationRandom (CShip *pShip, CSpaceObject *pLeader, CVector *retvPos, CVector *retvVel, int *retiFacing)

//	CalcFlockingFormationRandom
//
//	Calculates the position that this ship should take relative to the rest of the flock. Returns FALSE
//	if the current ship is a leader in the flock.

	{
	if (pLeader == NULL)
		return false;

	//	Compute the size of the ship

	Metric rSpacing = 0.5 * pShip->GetHitSize();

	//	Pick a random angle and position with respect to the leader

	int iAngle = pShip->GetDestiny();
	Metric rRange = (2 + (pShip->GetDestiny() % 10)) * rSpacing;

	*retvPos = pLeader->GetPos() + PolarToVector(iAngle, rRange);
	*retvVel = pLeader->GetVel();
	*retiFacing = pLeader->GetRotation();

	return true;
	}

CVector CAIBehaviorCtx::CalcFlankPos (CShip *pShip, const CVector &vInterceptPos)

//	CalcFlankPos
//
//	Computes the flanking position based on the interception position (both are
//	relative to the ship position).

	{
	CVector vFlankingLine = vInterceptPos.Normal().Perpendicular();
	vFlankingLine = vFlankingLine * ((pShip->GetDestiny() % 2) == 0 ? GetFlankDist() : -GetFlankDist());

	return vInterceptPos + vFlankingLine;
	}

bool CAIBehaviorCtx::CalcFormationParams (CShip *pShip, 
										  const CVector &vDestPos, 
										  const CVector &vDestVel, 
										  int iDestAngle, 
										  CVector *retvRecommendedVel, 
										  Metric *retrDeltaPos2,
										  Metric *retrDeltaVel2)

//	CalcFormationParams
//
//	Computes the parameters required to hold a formation.
//
//	retvRecommendedVel is the recommended velocity that we need to achieve in
//			order to reach the desired state.
//
//	retrDeltaPos2 is the current distance (^2) to the desired pos
//	retrDelatVel2 is the magnitude (^2) of the recommended velocity
//
//	We return TRUE if the recommended velocity is a small enough fraction of 
//	ship speed.

	{
	CVector vDelta = vDestPos - pShip->GetPos();

	//	Compute the velocity that we want to get to the destination position
	//	as fast as possible.

	Metric rDist;
	CVector vTravel = vDelta.Normal(&rDist) * pShip->GetMaxSpeed();

	//	Ease from the travel velocity to the final velocity as we get closer
	//	to the position.

	CVector vDeltaV;
	if (rDist < MAX_DISTANCE)
		{
		Metric rEase = rDist / MAX_DISTANCE;
		CVector vVel = (rEase * vTravel) + ((1.0 - rEase) * vDestVel);
		vDeltaV = vVel - pShip->GetVel();
		}

	//	Otherwise, proceed at travel speed

	else
		vDeltaV = vTravel - pShip->GetVel();

	Metric rDiff2 = vDeltaV.Length2();

	//	Return our calculations

	if (retvRecommendedVel)
		*retvRecommendedVel = vDeltaV;

	if (retrDeltaPos2)
		*retrDeltaPos2 = vDelta.Length2();

	if (retrDeltaVel2)
		*retrDeltaVel2 = rDiff2;

	//	If we're close enough to the velocity, cheat a little by
	//	accelerating without using the main engine

	Metric rCloseV = CLOSE_DELTA_V_RATIO * pShip->GetMaxSpeed();
	Metric rCloseV2 = (rCloseV * rCloseV);

#ifdef DEBUG_FORMATION
	if (g_pUniverse->GetPlayerShip() 
			&& g_pUniverse->GetPlayerShip()->GetTarget(CItemCtx(), true) == pShip
			&& m_pUpdateCtx->pAnnotations)
		{
		m_pUpdateCtx->pAnnotations->bDebugFormation = true;
		m_pUpdateCtx->pAnnotations->vFormationPos = vDestPos;
		m_pUpdateCtx->pAnnotations->iFormationAngle = iDestAngle;
		m_pUpdateCtx->pAnnotations->vFormationCurPos = pShip->GetPos();
		}
#endif

	return (rDiff2 < rCloseV2);
	}

CVector CAIBehaviorCtx::CalcManeuverCloseOnTarget (CShip *pShip,
												   CSpaceObject *pTarget, 
												   const CVector &vTarget, 
												   Metric rTargetDist2, 
												   bool bFlank)

//	CalcManeuverCloseOnTarget
//
//	Returns the vector that the ship should move in for a close-on-target maneuver

	{
	CVector vInterceptPoint;

#ifdef DEBUG_SHIP
	bool bDebug = pShip->IsSelected();
#endif

	//	If we don't have a target object, then go to the position

	if (pTarget == NULL)
		vInterceptPoint = vTarget;

	//	If we are very far (>10M klicks) from the target then 
	//	compensate for the target's motion.

	else if (rTargetDist2 > 1.0e14)
		{
		//	Compute the speed with which the target is closing
		//	the distance (this may be a negative number). Note
		//	that we care about the target's absolute velocity
		//	(not its relative velocity because we are trying to
		//	adjust our velocity).

		CVector vAbsVel = pTarget->GetVel();
		Metric rClosingSpeed = -vAbsVel.Dot(vTarget.Normal());

		//	Figure out how long it will take to overtake the target's
		//	current position at maximum speed. (This is just a heuristic
		//	that gives us an approximation of the length of an intercept
		//	course.)

		rClosingSpeed = rClosingSpeed + pShip->GetMaxSpeed();
		if (rClosingSpeed > 0.0)
			{
			Metric rTimeToIntercept = vTarget.Length() / (rClosingSpeed);
			vInterceptPoint = vTarget + vAbsVel * rTimeToIntercept;
			}
		else
			vInterceptPoint = vTarget;

#ifdef DEBUG_SHIP
		if (bDebug)
			g_pUniverse->DebugOutput("Adjust for distance: %d at %d%%c", 
					(int)(vTarget.Length() / LIGHT_SECOND),
					(int)(100.0 * rClosingSpeed / LIGHT_SPEED));
#endif
		}
	else
		{
		CVector vTargetVel = pTarget->GetVel() - pShip->GetVel();
		vInterceptPoint = vTarget + (vTargetVel * g_SecondsPerUpdate);

		//	Close on a flanking point

		if (bFlank)
			{
			vInterceptPoint = CalcFlankPos(pShip, vInterceptPoint);

#ifdef DEBUG_SHIP
			if (bDebug)
				g_pUniverse->DebugOutput("Flank target");
#endif
			}
		}

	return vInterceptPoint;
	}

CVector CAIBehaviorCtx::CalcManeuverFormation (CShip *pShip, const CVector vDest, const CVector vDestVel, int iDestFacing)

//	CalcManeuverFormation
//
//	Returns the vector that the ship should move in to achieve the given formation

	{
	//	Compute our formation parameters

	CVector vDeltaV;
	bool bCloseEnough = CalcFormationParams(pShip, vDest, vDestVel, iDestFacing, &vDeltaV);

	//	If we're close enough to the velocity, cheat a little by
	//	accelerating without using the main engine

	if (bCloseEnough)
		{
		if (!pShip->IsParalyzed())
			{
			pShip->Accelerate(vDeltaV * pShip->GetMass() / 2000.0, g_SecondsPerUpdate);

#ifdef DEBUG_ATTACK_TARGET_MANEUVERS
			pShip->SetDebugVector(vDeltaV.Normal() * 100. * g_KlicksPerPixel);
#endif
			}

		return CVector();
		}

	//	Otherwise, thrust with the main engines

	else
		{
		int iAngle = VectorToPolar(vDeltaV);
		return PolarToVector(iAngle, 5.0 * LIGHT_SECOND);
		}
	}

CVector CAIBehaviorCtx::CalcManeuverSpiralIn (CShip *pShip, const CVector &vTarget, int iTrajectory)

//	CalcManeuverSpiralIn
//
//	Returns the vector that the ship should move in for a spiral-in maneuver

	{
	CVector vTangent = (vTarget.Perpendicular()).Normal() * pShip->GetMaxSpeed() * g_SecondsPerUpdate * 8;

	Metric rRadius;
	int iAngle = VectorToPolar(vTangent, &rRadius);

	//	Curve inward

	return PolarToVector(iAngle + 360 - iTrajectory, rRadius);
	}

CVector CAIBehaviorCtx::CalcManeuverSpiralOut (CShip *pShip, const CVector &vTarget, int iTrajectory)

//	CalcManeuverSpiralOut
//
//	Returns the vector that the ship should move in for a spiral-out maneuver

	{
	CVector vTangent = (vTarget.Perpendicular()).Normal() * pShip->GetMaxSpeed() * g_SecondsPerUpdate * 8;

	Metric rRadius;
	int iAngle = VectorToPolar(vTangent, &rRadius);

	//	Handle the case where we vTangent is 0 (i.e., we are on top of the enemy)

	if (rRadius == 0.0)
		rRadius = g_KlicksPerPixel;

	//	Curve out

	return PolarToVector(iAngle + iTrajectory, rRadius);
	}

void CAIBehaviorCtx::ImplementAttackNearestTarget (CShip *pShip, Metric rMaxRange, CSpaceObject **iopTarget, CSpaceObject *pExcludeObj, bool bTurn)

//	ImplementAttackNearestTarget
//
//	Sets m_pTarget to be the nearest target and attacks it. This method
//	should only be used while in a state that does not need m_pTarget.

	{
	DEBUG_TRY

	CSpaceObject *pNewTarget;
	if (pShip->IsDestinyTime(19) 
			&& !m_AISettings.NoTargetsOfOpportunity()
			&& (pNewTarget = pShip->GetNearestVisibleEnemy(rMaxRange, false, pExcludeObj)))
		(*iopTarget) = pNewTarget;

	if (*iopTarget)
		{
		CVector vTarget = (*iopTarget)->GetPos() - pShip->GetPos();
		Metric rTargetDist2 = vTarget.Dot(vTarget);

		//	Don't bother if the target is too far away

		if (rTargetDist2 > rMaxRange * rMaxRange)
			{
			(*iopTarget) = NULL;
			return;
			}

		int iFireDir;
		ImplementFireWeaponOnTarget(pShip, -1, -1, *iopTarget, vTarget, rTargetDist2, &iFireDir);

		//	If we don't have a good weapon, then don't go after this target

		if (GetBestWeapon() == NULL)
			{
			(*iopTarget) = NULL;
			return;
			}

		//	If we have a fire direction, try to turn towards the target

		if (bTurn
				&& iFireDir != -1
				&& !NoDogfights())
			ImplementManeuver(pShip, iFireDir, false);
		}

	DEBUG_CATCH
	}

void CAIBehaviorCtx::ImplementAttackTarget (CShip *pShip, CSpaceObject *pTarget, bool bMaintainCourse, bool bDoNotShoot)

//	ImplementAttackTarget
//
//	Maneuvers towards and attacks target

	{
	DEBUG_TRY

	CVector vTarget = pTarget->GetPos() - pShip->GetPos();
	Metric rTargetDist2 = vTarget.Dot(vTarget);

	//	Fire on the target as best we can

	int iFireDir;
	ImplementFireWeaponOnTarget(pShip, -1, -1, pTarget, vTarget, rTargetDist2, &iFireDir, bDoNotShoot);

#ifdef DEBUG_ATTACK_TARGET
	pShip->SetDebugVector(CVector());
#endif

	CVector vFlockPos;
	CVector vFlockVel;
	int iFlockFacing;

	//	If we're maintaining our course, then no need to maneuver.

	if (bMaintainCourse || IsImmobile())
		NULL;

	//	If we're flocking, then implement flocking maneuverses

	else if (m_AISettings.IsFlocker() 
				&& rTargetDist2 > FLOCK_COMBAT_RANGE2
				&& CalcFlockingFormation(pShip, NULL, &vFlockPos, &vFlockVel, &iFlockFacing))
		ImplementFormationManeuver(pShip, vFlockPos, vFlockVel, pShip->AlignToRotationAngle(iFlockFacing));

	//	Otherwise, implement maneuvers

	else
		{
		//	If the ship has shields then figure out their state.
		//	(We need it inside of ImplementAttackTargetManeuver.)

		CalcShieldState(pShip);

		//	Compute avoidance potential, if necessary

		CalcAvoidPotential(pShip, pTarget);

		//	Maneuver according to combat style. If we maneuvered, then
		//	ignore the fire solution because we've already maneuvered

		if (ImplementAttackTargetManeuver(pShip, pTarget, vTarget, rTargetDist2))
			iFireDir = -1;
		}

	//	Turn towards fire solution, if we have one

	if (iFireDir != -1
			&& !NoDogfights())
		ImplementManeuver(pShip, iFireDir, false);

	DEBUG_CATCH
	}

bool CAIBehaviorCtx::ImplementAttackTargetManeuver (CShip *pShip, CSpaceObject *pTarget, const CVector &vTarget, Metric rTargetDist2)

//	ImplementAttackTargetManeuver
//
//	Implements maneuvers to attack the target based on the ship's combat style
//	and taking into account the hazard potential vector.
//
//	Returns TRUE if a maneuver was made (if FALSE, then ship is free to maneuver to align weapon)

	{
	CVector vDirection;
	bool bNoThrustThroughTurn = false;

	//	Maneuver according to the ship's combat style. This block will initialize
	//	vDirection to point to the desired direction

	switch (GetCombatStyle())
		{
		case aicombatStandard:
			{
			bool bFaster = (pShip->GetMaxSpeed() > pTarget->GetMaxSpeed());

			//	If we're waiting for shields to regenerate, then
			//	spiral away

			if (IsWaitingForShieldsToRegen() && bFaster)
				{
				DEBUG_COMBAT_OUTPUT("Wait for shields");
				vDirection = CombinePotential(CalcManeuverSpiralOut(pShip, vTarget, 75));
				}

			//	If we're not well in range of our primary weapon then
			//	get closer to the target. (Or if we are not moving)

			else if (rTargetDist2 > GetPrimaryAimRange2())
				{
				DEBUG_COMBAT_OUTPUT("Close on target");

				//	Try to flank our target, if we are faster

				vDirection = CombinePotential(CalcManeuverCloseOnTarget(pShip, pTarget, vTarget, rTargetDist2, bFaster));
				}

			//	If we're attacking a static target then find a good spot
			//	and shoot from there.

			else if (!pTarget->CanThrust())
				{
				int iClock = g_pUniverse->GetTicks() / (170 + pShip->GetDestiny() / 3);
				int iAngle = pShip->AlignToRotationAngle((pShip->GetDestiny() + (iClock * 141 * (1 + pShip->GetDestiny()))) % 360);
				Metric rRadius = MIN_STATION_TARGET_DIST + (LIGHT_SECOND * (pShip->GetDestiny() % 100) / 10.0);

				//	This is the position that we want to go to

				CVector vPos;
				if (m_fHasAvoidPotential)
					vPos = pShip->GetPos() + (POTENTIAL_TO_POS_ADJ * GetPotential());
				else
					vPos = pTarget->GetPos() + PolarToVector(iAngle + 180, rRadius);

				//	We don't want to thrust unless we're in position

				bNoThrustThroughTurn = true;

				//	Figure out which way we need to move to end up where we want
				//	(Note that we don't combine the potential because we've already accounted for
				//	it above).

				vDirection = CalcManeuverFormation(pShip, vPos, CVector(), iAngle);
				}

			//	If we're attacking a station, then keep our distance so that
			//	we don't get caught in the explosion

			else if (m_fAvoidExplodingStations
					&& rTargetDist2 < MIN_STATION_TARGET_DIST2 
					&& pTarget->GetMass() > 5000.0)
				{
				DEBUG_COMBAT_OUTPUT("Spiral away to avoid explosion");
				vDirection = CombinePotential(CalcManeuverSpiralOut(pShip, vTarget));
				}

			//	If we're too close to our target, spiral away

			else if (rTargetDist2 < MIN_TARGET_DIST2)
				{
				DEBUG_COMBAT_OUTPUT("Spiral away");
				vDirection = CombinePotential(CalcManeuverSpiralOut(pShip, vTarget));
				}

			//	If we're moving too slowly, move away

			else if (pTarget->CanThrust()
					&& (pShip->GetVel().Length2() < (0.01 * 0.01 * LIGHT_SPEED * LIGHT_SPEED)))
				{
				DEBUG_COMBAT_OUTPUT("Speed away");
				vDirection = CombinePotential(CalcManeuverSpiralOut(pShip, vTarget));
				}

			//	Otherwise, hazard avoidance only

			else
				vDirection = GetPotential();

			break;
			}

		case aicombatAdvanced:
			{
			bool bWeAreFaster = (pShip->GetMaxSpeed() >= pTarget->GetMaxSpeed());

			const int MAX_BRAVERY_TICKS = 300;				//	Number of ticks since last attack to be 100% brave
			const Metric BRAVERY_DECAY_POWER = 2.0;
			const Metric EXTRA_RANGE = LIGHT_SECOND * 8.0;

			//	Compute how brave we are based on the last time we got hit.
			//	rBravery goes from 0.0 (scared) to 1.0 (brave)

			int iLastHit = Max(0, Min(MAX_BRAVERY_TICKS, (g_pUniverse->GetTicks() - m_iLastAttack)));
			const Metric rBravery = pow((Metric)iLastHit / (Metric)MAX_BRAVERY_TICKS, BRAVERY_DECAY_POWER);

			//	Compute the maximum distance at which we'll start firing. If we're feeling brave,
			//	this will be close to the flank distance (which is often close). Otherwise, we'll
			//	try to stack back.

			const Metric rMaxAimDist = GetFlankDist() + (EXTRA_RANGE * (1.0 - rBravery));
			const Metric rMaxAimDist2 = (bWeAreFaster ? Min(GetPrimaryAimRange2(), rMaxAimDist * rMaxAimDist) : GetPrimaryAimRange2());

			//	Minimum distance is a never more than one-half the maximum distance.

			const Metric rMinDist2 = Min(MIN_TARGET_DIST2, 0.25 * rMaxAimDist2);

			//	If we're waiting for shields to regenerate, then
			//	spiral away

			if (IsWaitingForShieldsToRegen()
					&& bWeAreFaster
					&& pShip->GetController()->GetCurrentOrderEx() != IShipController::orderEscort)
				{
				DEBUG_COMBAT_OUTPUT("Wait for shields");
				vDirection = CombinePotential(CalcManeuverSpiralOut(pShip, vTarget, 75));
				}

			//	If we're attacking a static target then find a good spot
			//	and shoot from there.
			//
			//	NOTE: We need to check this before the code below because otherwise we
			//	won't get here.

			else if (!pTarget->CanThrust())
				{
				int iClock = g_pUniverse->GetTicks() / (170 + pShip->GetDestiny() / 3);
				int iAngle = pShip->AlignToRotationAngle((pShip->GetDestiny() + (iClock * 141 * (1 + pShip->GetDestiny()))) % 360);
				Metric rRadius = MIN_STATION_TARGET_DIST + (LIGHT_SECOND * (pShip->GetDestiny() % 100) / 10.0);

				//	This is the position that we want to go to

				CVector vPos;
				if (m_fHasAvoidPotential)
					vPos = pShip->GetPos() + (POTENTIAL_TO_POS_ADJ * GetPotential());
				else
					vPos = pTarget->GetPos() + PolarToVector(iAngle + 180, rRadius);

				//	We don't want to thrust unless we're in position

				bNoThrustThroughTurn = true;

				//	Figure out which way we need to move to end up where we want
				//	(Note that we don't combine the potential because we've already accounted for
				//	it above).

				vDirection = CalcManeuverFormation(pShip, vPos, CVector(), iAngle);
				}

			//	If we're not well in range of our primary weapon then
			//	get closer to the target. (Or if we are not moving)

			else if (rTargetDist2 > rMaxAimDist2)
				{
				//	If we're faster, try to be clever

				if (bWeAreFaster)
					{
					CVector vToTargetN = (pTarget->GetPos() - pShip->GetPos()).Normal();

					//	Compute the position we want

					CVector vPos;
					if (m_fHasAvoidPotential)
						{
						DEBUG_COMBAT_OUTPUT("Avoid hazards");

						vPos = pShip->GetPos() + (POTENTIAL_TO_POS_ADJ * GetPotential());
						}
					else
						{
						DEBUG_COMBAT_OUTPUT("Close to aim point");

						//	Pick a position at the flank distance between us and the target

						vPos = pTarget->GetPos() + (vToTargetN * -rMaxAimDist);
						}

					//	We want to end with a non-zero velocity. Otherwise, we'll be a
					//	sitting duck.

					CVector vVel = pTarget->GetVel() + (vToTargetN * (1.0 - Min(0.7, rBravery)) * pShip->GetMaxSpeed());

					//	Maneuver to that point

					vDirection = CalcManeuverFormation(pShip, vPos, vVel, 0);
					}

				//	Otherwise, we just try to close as best as possible

				else
					{
					DEBUG_COMBAT_OUTPUT("Close on target");
					vDirection = CombinePotential(CalcManeuverCloseOnTarget(pShip, pTarget, vTarget, rTargetDist2));
					}
				}

			//	If we're attacking a station, then keep our distance so that
			//	we don't get caught in the explosion

			else if (m_fAvoidExplodingStations
					&& rTargetDist2 < MIN_STATION_TARGET_DIST2 
					&& pTarget->GetMass() > 5000.0)
				{
				DEBUG_COMBAT_OUTPUT("Spiral away to avoid explosion");
				vDirection = CombinePotential(CalcManeuverSpiralOut(pShip, vTarget));
				}

			//	If we're too close to our target, spiral away

			else if (rTargetDist2 < rMinDist2)
				{
				DEBUG_COMBAT_OUTPUT("Spiral away");
				vDirection = CombinePotential(CalcManeuverSpiralOut(pShip, vTarget));
				}

			//	Otherwise, hazard avoidance only.

			else
				vDirection = GetPotential();

			break;
			}

		case aicombatStandOff:
			{
			Metric rMaxRange2 = m_rBestWeaponRange * m_rBestWeaponRange;
			Metric rIdealRange2 = 0.45 * rMaxRange2;

			//	If we're beyond our weapon's max range, then close on target

			if (rTargetDist2 > rMaxRange2)
				{
				vDirection = CombinePotential(CalcManeuverCloseOnTarget(pShip, pTarget, vTarget, rTargetDist2));
				}

			//	If we're inside the ideal range, then move away from the target

			else if (rTargetDist2 < rIdealRange2)
				{
				vDirection = CombinePotential(CalcManeuverSpiralOut(pShip, vTarget, 45));
				}

			//	Otherwise, hazard avoidance only

			else
				vDirection = GetPotential();

			break;
			}

		case aicombatChase:
			{
			Metric rMaxRange2 = m_rBestWeaponRange * m_rBestWeaponRange;

			//	Compute the angle line along the target's motion (and make sure
			//	it is aligned on a rotation angle, so we can get a shot in)

			int iTargetMotion = (pTarget->CanThrust() ?
					pShip->AlignToRotationAngle(VectorToPolar(pTarget->GetVel()))
					: pShip->AlignToRotationAngle(pShip->GetDestiny()));

			//	Compute the target's angle with respect to us. We want to end up facing
			//	directly towards the target

			int iTargetAngle = VectorToPolar(vTarget);

			//	Pick a point behind the target (and add hazard potential)

			Metric rRange = Min(0.5 * m_rBestWeaponRange, 10.0 * LIGHT_SECOND);
			CVector vPos;
			if (m_fHasAvoidPotential)
				vPos = pShip->GetPos() + (POTENTIAL_TO_POS_ADJ * GetPotential());
			else
				vPos = pTarget->GetPos() + PolarToVector(iTargetMotion + 180, rRange);

			//	Figure out which way we need to move to end up where we want
			//	(Note that we don't combine the potential because we've already accounter for
			//	it above).

			vDirection = CalcManeuverFormation(pShip, vPos, pTarget->GetVel(), iTargetAngle);

			//	We don't want to thrust unless we're in position

			bNoThrustThroughTurn = true;

			//	Debug

#ifdef DEBUG_ATTACK_TARGET
			pTarget->SetDebugVector(vPos - pTarget->GetPos());
#endif
			break;
			}

		case aicombatFlyby:
			{
			Metric rCloseRange2 = 0.25 * GetPrimaryAimRange2();

			//	If we're beyond our weapon's effective range, then close on target

			if (rTargetDist2 > GetPrimaryAimRange2())
				vDirection = CombinePotential(CalcManeuverCloseOnTarget(pShip, pTarget, vTarget, rTargetDist2));

			//	If we're too close to the target, move

			else if (rTargetDist2 < rCloseRange2)
				{
				//	Compute our bearing from the target's perspective

				int iBearing = VectorToPolar(-vTarget);

				//	If the target is facing us, then spiral away

				if (AreAnglesAligned(iBearing, pTarget->GetRotation(), 90))
					vDirection = CombinePotential(CalcManeuverSpiralOut(pShip, vTarget));
				else
					vDirection = GetPotential();
				}

			//	Otherwise, make sure we are moving wrt to the target

			else
				{
				CVector vVDelta = pTarget->GetVel() - pShip->GetVel();
				if (vVDelta.Length2() < MIN_FLYBY_SPEED2)
					{
					if (pShip->GetDestiny() > 180)
						vDirection = CombinePotential(CalcManeuverSpiralIn(pShip, vTarget));
					else
						vDirection = CombinePotential(CalcManeuverCloseOnTarget(pShip, pTarget, vTarget, rTargetDist2));
					}
				else
					vDirection = GetPotential();
				}

			break;
			}

		case aicombatNoRetreat:
			{
			//	If we're not well in range of our primary weapon then
			//	get closer to the target. (Or if we are not moving)

			if (rTargetDist2 > GetPrimaryAimRange2())
				{
				DEBUG_COMBAT_OUTPUT("Close on target");

				//	Try to flank our target, if we are faster

				bool bFlank = (pShip->GetMaxSpeed() > pTarget->GetMaxSpeed());
				vDirection = CombinePotential(CalcManeuverCloseOnTarget(pShip, pTarget, vTarget, rTargetDist2, bFlank));
				}

			//	If we're moving too slowly, spiral out

			else if (pTarget->CanThrust()
					&& (pShip->GetVel().Length2() < (0.01 * 0.01 * LIGHT_SPEED * LIGHT_SPEED)))
				{
				DEBUG_COMBAT_OUTPUT("Speed away");
				vDirection = CombinePotential(CalcManeuverSpiralOut(pShip, vTarget));
				}

			//	No maneuver

			else
				vDirection = GetPotential();
			
			break;
			}
		}

	//	If our direction vector is not long enough, then it means that we
	//	don't need to change course

	if (vDirection.Length2() < MIN_POTENTIAL2)
		return false;

	//	Maneuver to get to our desired direction. 

	ImplementManeuver(pShip, VectorToPolar(vDirection), true, bNoThrustThroughTurn);

#ifdef DEBUG_ATTACK_TARGET
	pShip->SetDebugVector(vDirection.Normal() * 100.0 * g_KlicksPerPixel);
#endif

	//	Done

	return true;
	}

void CAIBehaviorCtx::ImplementCloseOnImmobileTarget (CShip *pShip, CSpaceObject *pTarget, const CVector &vTarget, Metric rTargetDist2, Metric rTargetSpeed)

//	ImplementCloseOnImmobileTarget
//
//	Head towards a target like a station or stargate

	{
	//	If we're close to the target, then we need to be more precise

	if (rTargetDist2 < CLOSE_RANGE2)
		{
		//	Compute a destination direction

		int iDestFacing = VectorToPolar(vTarget);
		if (iDestFacing >= 0 && iDestFacing < 90)
			iDestFacing = pShip->AlignToRotationAngle(45);
		else if (iDestFacing >=90 && iDestFacing < 180)
			iDestFacing = pShip->AlignToRotationAngle(135);
		else if (iDestFacing >=180 && iDestFacing < 270)
			iDestFacing = pShip->AlignToRotationAngle(225);
		else
			iDestFacing = pShip->AlignToRotationAngle(315);

		//	Close in

		ImplementFormationManeuver(pShip, vTarget + pShip->GetPos(), PolarToVector(iDestFacing, rTargetSpeed), iDestFacing);
		}

	//	Otherwise, just head for the target

	else
		ImplementHeadingManeuver(pShip, vTarget);
	}

void CAIBehaviorCtx::ImplementCloseOnTarget (CShip *pShip, CSpaceObject *pTarget, const CVector &vTarget, Metric rTargetDist2, bool bFlank)

//	ImplementCloseOnTarget
//
//	Head towards the target.

	{
	CVector vInterceptPoint;
	CVector vTargetVel;
	if (pTarget)
		vTargetVel = pTarget->GetVel();

#ifdef DEBUG_SHIP
	bool bDebug = pShip->IsSelected();
#endif

	//	If we don't have a target object, then go to the position

	if (pTarget == NULL)
		vInterceptPoint = vTarget;

	//	If this is an immobile target, use a different algorithm

	else if (vTargetVel.Length2() < MAX_IMMOBILE_SPEED2)
		{
		ImplementCloseOnImmobileTarget(pShip, pTarget, vTarget, rTargetDist2, pShip->GetMaxSpeed());
		return;
		}

	//	If we are very far (>10M klicks) from the target then 
	//	compensate for the target's motion.

	else if (rTargetDist2 > 1.0e14)
		{
		//	Compute the speed with which the target is closing
		//	the distance (this may be a negative number). Note
		//	that we care about the target's absolute velocity
		//	(not its relative velocity because we are trying to
		//	adjust our velocity).

		Metric rClosingSpeed = -vTargetVel.Dot(vTarget.Normal());

		//	Figure out how long it will take to overtake the target's
		//	current position at maximum speed. (This is just a heuristic
		//	that gives us an approximation of the length of an intercept
		//	course.)

		rClosingSpeed = rClosingSpeed + pShip->GetMaxSpeed();
		if (rClosingSpeed > 0.0)
			{
			Metric rTimeToIntercept = vTarget.Length() / (rClosingSpeed);
			vInterceptPoint = vTarget + vTargetVel * rTimeToIntercept;
			}
		else
			vInterceptPoint = vTarget;

#ifdef DEBUG_SHIP
		if (bDebug)
			g_pUniverse->DebugOutput("Adjust for distance: %d at %d%%c", 
					(int)(vTarget.Length() / LIGHT_SECOND),
					(int)(100.0 * rClosingSpeed / LIGHT_SPEED));
#endif
		}
	else
		{
		CVector vTargetVel = pTarget->GetVel() - pShip->GetVel();
		vInterceptPoint = vTarget + (vTargetVel * g_SecondsPerUpdate);

		//	Close on a flanking point

		if (bFlank)
			{
			vInterceptPoint = CalcFlankPos(pShip, vInterceptPoint);

#ifdef DEBUG_SHIP
			if (bDebug)
				g_pUniverse->DebugOutput("Flank target");
#endif
			}
		}

	//	Set velocity towards the interception point

	ImplementHeadingManeuver(pShip, vInterceptPoint);
	}

void CAIBehaviorCtx::ImplementDocking (CShip *pShip, CSpaceObject *pTarget)

//	ImplementDocking
//
//	Docks with target

	{
	DEBUG_TRY

	CVector vTarget = pTarget->GetPos() - pShip->GetPos();
	Metric rTargetDist2 = vTarget.Dot(vTarget);

	//	If we've requested docking, then we don't need to do anything

	if (IsDockingRequested())
		{ }

	//	If we're still far from our target, then approach it

	else if (rTargetDist2 > (MAX_DOCK_DISTANCE * MAX_DOCK_DISTANCE))
		ImplementCloseOnImmobileTarget(pShip, pTarget, vTarget, rTargetDist2);

	//	Otherwise, if we're within range, then request docking

	else if (pShip->IsDestinyTime(20))
		{
		if (pTarget->RequestDock(pShip))
			SetDockingRequested(true);
		}

	DEBUG_CATCH
	}

void CAIBehaviorCtx::ImplementEscort (CShip *pShip, CSpaceObject *pBase, CSpaceObject **iopTarget)

//	ImplementEscort
//
//	Basic escort algorithm

	{
	ASSERT(pBase);

	//	Compute the desired escort position

	CVector vEscortPos;
	CVector vEscortVel;
	int iEscortDir;
	CalcEscortFormation(pShip, pBase, &vEscortPos, &vEscortVel, &iEscortDir);

	//	If we're particularly far from our desired position, then we close

	CVector vTarget = vEscortPos - pShip->GetPos();
	Metric rTargetDist2 = vTarget.Dot(vTarget);
	if (rTargetDist2 > (MAX_ESCORT_DISTANCE * MAX_ESCORT_DISTANCE))
		ImplementCloseOnTarget(pShip, pBase, vTarget, rTargetDist2);

	//	Otherwise, we're free to attack our target or maneuver into position

	else
		{
		if (*iopTarget)
			ImplementAttackTarget(pShip, *iopTarget, true);
		else
			ImplementFormationManeuver(pShip, vEscortPos, vEscortVel, pShip->AlignToRotationAngle(iEscortDir));
		}

	//	Attack in our current position

	ImplementAttackNearestTarget(pShip, ATTACK_RANGE, iopTarget, pBase);
	ImplementFireOnTargetsOfOpportunity(pShip, NULL, pBase);
	}

void CAIBehaviorCtx::ImplementEvasiveManeuvers (CShip *pShip, CSpaceObject *pTarget)

//	ImplementEvasiveManeuvers
//
//	Move away from the given target

	{
	CVector vTarget = pTarget->GetPos() - pShip->GetPos();
	int iDirectionToFace = VectorToPolar(-vTarget, NULL);
	ImplementManeuver(pShip, iDirectionToFace, true);
	}

void CAIBehaviorCtx::ImplementFireOnTarget (CShip *pShip, CSpaceObject *pTarget, bool *retbOutOfRange)

//	ImplementFireOnTarget
//
//	Fires on the given target, if aligned and in range

	{
	bool bOutOfRange;

	ASSERT(pTarget);

	CVector vTarget = pTarget->GetPos() - pShip->GetPos();
	Metric rTargetDist2 = vTarget.Dot(vTarget);

	//	Don't bother if the target is too far away

	if (rTargetDist2 > MAX_TARGET_OF_OPPORTUNITY_RANGE * MAX_TARGET_OF_OPPORTUNITY_RANGE)
		bOutOfRange = true;
	else
		{
		ImplementFireWeaponOnTarget(pShip, -1, -1, pTarget, vTarget, rTargetDist2);
		bOutOfRange = false;
		}

	//	Done

	if (retbOutOfRange)
		*retbOutOfRange = bOutOfRange;
	}

void CAIBehaviorCtx::ImplementFireOnTargetsOfOpportunity (CShip *pShip, CSpaceObject *pTarget, CSpaceObject *pExcludeObj)

//	ImplementFireOnTargetsOfOpportunity
//
//	Attacks any targets in the area with secondary weapons

	{
	DEBUG_TRY

	int i;

	//	If this ship has secondary weapons that also fire at will
	//	(note that we don't turn this on normally because it is relatively
	//	expensive)

	if (HasSecondaryWeapons() 
			&& !m_AISettings.NoTargetsOfOpportunity())
		{
		for (i = 0; i < pShip->GetDeviceCount(); i++)
			{
			CInstalledDevice *pDevice = pShip->GetDevice(i);
			if (!pDevice->IsEmpty() && pDevice->IsSecondaryWeapon() && pDevice->IsReady())
				pDevice->SetTriggered(true);
			}
		}

	DEBUG_CATCH
	}

void CAIBehaviorCtx::ImplementFireWeapon (CShip *pShip, DeviceNames iDev)

//	ImplementFireWeapon
//
//	Fires the given weapon blindly

	{
	CInstalledDevice *pWeapon;

	if (iDev == devNone)
		{
		CalcBestWeapon(pShip, NULL, 0.0);
		pWeapon = GetBestWeapon();
		}
	else
		pWeapon = pShip->GetNamedDevice(iDev);

	if (pWeapon && pWeapon->IsReady())
		{
		int iFireAngle = pWeapon->GetDefaultFireAngle(pShip);
		if (CheckForFriendsInLineOfFire(pShip, pWeapon, NULL, iFireAngle, Max(pWeapon->GetMaxEffectiveRange(pShip), DEFAULT_DIST_CHECK)))
			pShip->SetWeaponTriggered(pWeapon);
		}
	}

void CAIBehaviorCtx::ImplementFireWeaponOnTarget (CShip *pShip,
											    int iWeapon,
											    int iWeaponVariant,
											    CSpaceObject *pTarget,
											    const CVector &vTarget,
											    Metric rTargetDist2,
											    int *retiFireDir,
											    bool bDoNotShoot)

//	ImplementFireWeaponOnTarget
//
//	Fires the given weapon (if aligned) on target

	{
	DEBUG_TRY

	int iFireDir = -1;
	int iTick = pShip->GetSystem()->GetTick();

#ifdef DEBUG
	bool bDebug = pShip->IsSelected();
#endif

	ASSERT(pTarget);

	//	Select the appropriate weapon. If we're not given a weapon, then choose the
	//	best one.

	DeviceNames iWeaponToFire;
	Metric rWeaponRange;
	if (iWeapon == -1)
		{
		if (((iTick % 30) == 0)
				&& (m_fHasMultipleWeapons || m_iBestWeapon == devNone))
			ClearBestWeapon();

		CalcBestWeapon(pShip, pTarget, rTargetDist2);
		if (m_iBestWeapon == devNone)
			{
			if (retiFireDir)
				*retiFireDir = -1;

			DEBUG_COMBAT_OUTPUT("Fire: No appropriate weapon found");
			return;
			}

		iWeaponToFire = m_iBestWeapon;
		rWeaponRange = m_rBestWeaponRange;
		}
	else
		{
		iWeaponToFire = pShip->SelectWeapon(iWeapon, iWeaponVariant);
		rWeaponRange = pShip->GetWeaponRange(iWeaponToFire);
		}

	//	See if the chosen weapon can hit the target

	int iAimAngle = pShip->GetRotation();
	int iFireAngle = -1;
	int iFacingAngle = -1;
	bool bAligned;
	bAligned = pShip->IsWeaponAligned(iWeaponToFire,
			pTarget, 
			&iAimAngle,
			&iFireAngle,
			&iFacingAngle);
	bool bAimError = false;

	//	iAimAngle is the direction that we should fire in order to hit
	//	the target.
	//
	//	iFireAngle is the direction in which the weapon will fire.
	//
	//	iFacingAngle is the direction in which the ship should face
	//	in order for the weapon to hit the target.

	//	There is a chance of missing

	if (pShip->GetWeaponIsReady(iWeaponToFire))
		{
		if (bAligned)
			{
			if (mathRandom(1, 100) > GetFireAccuracy())
				{
				bAligned = false;

				//	In this case, we happen to be aligned, but because of inaccuracy
				//	reason we think we're not. We clear the aim angle because for
				//	omnidirectional weapons, we don't want to try to turn towards
				//	the new aim point.

				iAimAngle = -1;
				bAimError = true;
				DEBUG_COMBAT_OUTPUT("Aim error: hold fire when aligned");
				}
			}
		else if (iAimAngle != -1)
			{
			if (mathRandom(1, 100) <= m_iPrematureFireChance)
				{
				int iAimOffset = AngleOffset(iFireAngle, iAimAngle);
				if (iAimOffset < 20)
					{
					bAligned = true;
					bAimError = true;
					DEBUG_COMBAT_OUTPUT("Aim error: fire when not aligned");
					}
				}
			}
		}

	//	Fire

	if (bAligned)
		{
#ifdef DEBUG
		{
		char szDebug[1024];
		if (bAimError)
			wsprintf(szDebug, "%s: false positive  iAim=%d  iFireAngle=%d", pShip->GetNamedDevice(iWeaponToFire)->GetName().GetASCIIZPointer(), iAimAngle, iFireAngle);
		else
			wsprintf(szDebug, "%s: aligned  iAim=%d  iFireAngle=%d", pShip->GetNamedDevice(iWeaponToFire)->GetName().GetASCIIZPointer(), iAimAngle, iFireAngle);

		DEBUG_COMBAT_OUTPUT(szDebug);
		}
#endif

		//	If we're aligned and the weapon is ready, and we're
		//	in range of the target, then fire!

		if (pShip->GetWeaponIsReady(iWeaponToFire)
				&& rTargetDist2 < (rWeaponRange * rWeaponRange))
			{
			CInstalledDevice *pWeapon = pShip->GetNamedDevice(iWeaponToFire);

			if (iWeaponToFire == devPrimaryWeapon)
				{
				if (CheckForFriendsInLineOfFire(pShip, pWeapon, pTarget, iFireAngle, Max(pWeapon->GetMaxEffectiveRange(pShip), DEFAULT_DIST_CHECK)))
					{
					if (!bDoNotShoot)
						pShip->SetWeaponTriggered(pWeapon);
					DEBUG_COMBAT_OUTPUT("FireOnTarget: Fire primary!");
					}
				else
					DEBUG_COMBAT_OUTPUT("FireOnTarget: Friendlies in line of fire");
				}
			else
				{
				if (CheckForFriendsInLineOfFire(pShip, pWeapon, pTarget, iFireAngle, Max(pWeapon->GetMaxEffectiveRange(pShip), DEFAULT_DIST_CHECK)))
					{
					if (!bDoNotShoot)
						pShip->SetWeaponTriggered(pWeapon);
					DEBUG_COMBAT_OUTPUT("FireOnTarget: Fire missile!");
					}
				else
					DEBUG_COMBAT_OUTPUT("FireOnTarget: Friendlies in line of fire");
				}
			}
		}
	else
		{
		DEBUG_COMBAT_OUTPUT("Fire: Weapon NOT aligned");

		//	If the weapon is not aligned, turn to aim

		if (iFacingAngle != -1)
			iFireDir = iFacingAngle;

#ifdef DEBUG_SHIP
		if (bDebug)
			g_pUniverse->DebugOutput("Face target at distance: %d moving at: %d%%c", 
					(int)(vTarget.Length() / LIGHT_SECOND),
					(int)(100.0 * 0 / LIGHT_SPEED));
#endif
		}

	if (retiFireDir)
		*retiFireDir = iFireDir;

	DEBUG_CATCH
	}

void CAIBehaviorCtx::ImplementFollowNavPath (CShip *pShip, bool *retbAtDestination)

//	ImplementFollowNavPath
//
//	Ship follows nav path to its destination

	{
	DEBUG_TRY

	//	Figure out our next point along the path

	CVector vTarget = m_pNavPath->GetNavPoint(m_iNavPathPos) - pShip->GetPos();
	Metric rTargetDist2 = vTarget.Length2();

	//	If we've hit a wall, back up a little bit

	if (m_iBarrierClock != -1)
		{
		vTarget = vTarget.Normal() * g_KlicksPerPixel * -30.0;
		Metric rTargetDist2 = vTarget.Length2();

		ImplementCloseOnImmobileTarget(pShip, NULL, vTarget, rTargetDist2, pShip->GetMaxSpeed() / 4.0);
		}

	//	Otherwise continue

	else
		{
		//	Are we at our target? If so, then we move on to
		//	the next nav point

		if (rTargetDist2 < HIT_NAV_POINT_DIST2)
			{
			//	If we're at the last nav point, then we've reached our
			//	destination.

			if (m_iNavPathPos + 1 >= m_pNavPath->GetNavPointCount())
				{
				if (retbAtDestination)
					*retbAtDestination = true;
				return;
				}

			//	Otherwise, we go to the next nav point

			m_iNavPathPos++;
			vTarget = m_pNavPath->GetNavPoint(m_iNavPathPos) - pShip->GetPos();
			rTargetDist2 = vTarget.Length2();
			}

		//	Navigate towards the next nav point

		ImplementCloseOnImmobileTarget(pShip, NULL, vTarget, rTargetDist2, pShip->GetMaxSpeed() / 2.0);
		}

	//	Done

	if (retbAtDestination)
		*retbAtDestination = false;

	DEBUG_CATCH
	}

void CAIBehaviorCtx::ImplementFormationManeuver (CShip *pShip, const CVector vDest, const CVector vDestVel, int iDestFacing, bool *retbInFormation)

//	ImplementFormationManeuver
//
//	Moves the ship to the given formation point.
//
//	NOTE: iDestFacing must be aligned to one of the ship's rotation angles

	{
	DEBUG_TRY

	//	Compute our formation parameters

	CVector vDeltaV;
	Metric rDeltaPos2;
	Metric rDeltaV2;
	bool bCloseEnough = CalcFormationParams(pShip, vDest, vDestVel, iDestFacing, &vDeltaV, &rDeltaPos2, &rDeltaV2);

	//	If we're close enough to the velocity, cheat a little by
	//	accelerating without using the main engine

	if (bCloseEnough)
		{
		if (!pShip->IsParalyzed())
			pShip->Accelerate(vDeltaV * pShip->GetMass() / 2000.0, g_SecondsPerUpdate);

		ImplementTurnTo(pShip, iDestFacing);
		}

	//	Otherwise, thrust with the main engines

	else
		{
		//	Figure out which direction we need to thrust in to achieve the delta v

		int iAngle = VectorToPolar(vDeltaV);

		//	Turn towards the angle

		ImplementTurnTo(pShip, iAngle);

		//	If we don't need to turn, engage thrust

		if (GetManeuver() == NoRotation)
			SetThrustDir(CAIShipControls::constAlwaysThrust);
		}

	//	See if we're in formation

	if (retbInFormation)
		*retbInFormation = ((rDeltaPos2 < MAX_IN_FORMATION_DELTA2)
				&& (rDeltaV2 < MAX_DELTA_VEL2)
				&& pShip->IsPointingTo(iDestFacing));

	DEBUG_CATCH
	}

void CAIBehaviorCtx::ImplementGating (CShip *pShip, CSpaceObject *pTarget)

//	ImplementGating
//
//	Implementing gating out

	{
	DEBUG_TRY

	ASSERT(pTarget);
	CVector vTarget = pTarget->GetPos() - pShip->GetPos();
	Metric rTargetDist2 = vTarget.Dot(vTarget);

	if (rTargetDist2 < (MAX_GATE_DISTANCE * MAX_GATE_DISTANCE)
			&& rTargetDist2 < pTarget->GetMaxGateDist2())
		pTarget->RequestGate(pShip);
	else if (rTargetDist2 < (DOCKING_APPROACH_DISTANCE * DOCKING_APPROACH_DISTANCE))
		{
		pShip->SetMaxSpeedHalf();
		ImplementCloseOnImmobileTarget(pShip, pTarget, vTarget, rTargetDist2, pShip->GetMaxSpeed() / 2.0);
		}
	else
		ImplementCloseOnImmobileTarget(pShip, pTarget, vTarget, rTargetDist2, pShip->GetMaxSpeed());

	DEBUG_CATCH
	}

void CAIBehaviorCtx::ImplementHeadingManeuver (CShip *pShip, const CVector &vTarget)

//	ImplementHeadingManeuver
//
//	Heads in a straight line towards the given position.

	{
	//	Compute the ideal velocity vector

	CVector vIdeal = vTarget.Normal() * pShip->GetMaxSpeed();

	//	Compute the speed difference that we need to achieve the ideal.

	CVector vDeltaV = vIdeal - pShip->GetVel();

	//	If we're close enough to the ideal speed, then stay the course.

	int iDirectionToFace;
	Metric rDeltaV2 = vDeltaV.Length2();
	if (rDeltaV2 < MAX_HEADING_DELTA_V2)
		iDirectionToFace = VectorToPolar(vTarget, NULL);

	//	Otherwise, head towards the delta V vector

	else
		iDirectionToFace = VectorToPolar(vDeltaV, NULL);

	//	Head in that direction

	m_iManeuverCounter = 0;
	ImplementManeuver(pShip, iDirectionToFace, true, true);
	}

void CAIBehaviorCtx::ImplementHold (CShip *pShip, bool *retbInPlace)

//	ImplementHold
//
//	Stop by thrusting as soon as possible

	{
	bool bInPlace;

	//	Compute our velocity

	CVector vVel = pShip->GetVel();
	Metric rVel2 = vVel.Length2();

	//	If we're going slow enough, we cheat a bit

	if (rVel2 < MAX_DELTA_VEL2)
		{
		ImplementStop(pShip);
		bInPlace = true;
		}

	//	Otherwise, we thrust to cancel our velocity

	else
		{
		//	Figure out which direction we need to thrust in to achieve the delta v

		int iAngle = VectorToPolar(-vVel);

		//	Turn towards the angle

		ImplementTurnTo(pShip, iAngle);

		//	If we don't need to turn, engage thrust

		if (GetManeuver() == NoRotation)
			SetThrustDir(CAIShipControls::constAlwaysThrust);

		bInPlace = false;
		}

	if (retbInPlace)
		*retbInPlace = bInPlace;
	}

void CAIBehaviorCtx::ImplementOrbit (CShip *pShip, CSpaceObject *pBase, Metric rDistance)

//	ImplementOrbit
//
//	Orbit around object

	{
	CVector vTarget = pBase->GetPos() - pShip->GetPos();
	Metric rTargetDist2 = vTarget.Dot(vTarget);

	Metric rMaxDist = rDistance * 1.1;
	Metric rMinDist = rDistance * 0.9;

	int iCourseAdj;
	if (rTargetDist2 > (rMaxDist * rMaxDist))
		iCourseAdj = -30;
	else if (rTargetDist2 < (rMinDist * rMinDist))
		iCourseAdj = +30;
	else
		{
		Metric rTargetDist = sqrt(rTargetDist2);
		iCourseAdj = (int)(30.0 - (60.0 * ((rTargetDist - rMinDist) / (rMaxDist - rMinDist))));
		}

	CVector vTangent = (vTarget.Perpendicular()).Normal() * pShip->GetMaxSpeed() * g_SecondsPerUpdate * 8;
	Metric rRadius;
	int iAngle = VectorToPolar(vTangent, &rRadius);

	//	Curve

	CVector vInterceptPoint = PolarToVector(iAngle + iCourseAdj + 360, rRadius);

	//	Face towards the interception point

	int iDirectionToFace = VectorToPolar(vInterceptPoint, NULL);
	ImplementManeuver(pShip, iDirectionToFace, true);
	}

void CAIBehaviorCtx::ImplementManeuver (CShip *pShip, int iDir, bool bThrust, bool bNoThrustThroughTurn)

//	ImplementManeuver
//
//	Turns the ship towards the given direction. This function sets the variables:
//
//	m_iManeuver
//	m_iThrustDir
//	m_iLastTurn
//	m_iLastTurnCount

	{
#ifdef DEBUG_SHIP
	bool bDebug = pShip->IsSelected();
#endif

	if (iDir != -1)
		{
		int iCurrentDir = pShip->GetRotation();

		//	If we're within a few degrees of where we want to be, then
		//	don't bother changing

		EManeuverTypes iNewManeuver = pShip->GetManeuverToFace(iDir);
		if (iNewManeuver != NoRotation)
			{
			SetManeuver(iNewManeuver);

			//	If we're turning in a new direction now, then reset
			//	our counter

			if (GetManeuver() != NoRotation)
				{
				if (GetManeuver() != m_iLastTurn)
					{
					m_iLastTurn = GetManeuver();
					m_iLastTurnCount = 0;
					}
				else if (pShip->CanThrust())
					{
					m_iLastTurnCount++;

					//	If we've been turning in the same direction
					//	for a while, then arbitrarily turn in the opposite
					//	direction for a while.

					if (m_iLastTurnCount > m_iMaxTurnCount)
						{
						if (GetManeuver() == RotateRight)
							SetManeuver(RotateLeft);
						else
							SetManeuver(RotateRight);
#ifdef DEBUG_SHIP
						if (bDebug)
							g_pUniverse->DebugOutput("Reverse direction");
#endif
						}
					}
				}

#ifdef DEBUG_SHIP
			if (bDebug)
				g_pUniverse->DebugOutput("Turn: %s (%d -> %d)",
						(GetManeuver() == RotateRight ? "right" : 
							(GetManeuver() == RotateLeft ? "left" : "none")),
						iCurrentDir,
						iDir);
#endif

			//	Do not thrust (unless we want to thrust through the turn)

			SetThrustDir(CAIShipControls::constNeverThrust);
			if (bThrust && m_fThrustThroughTurn && !bNoThrustThroughTurn)
				SetThrustDir(CAIShipControls::constAlwaysThrust);
			}
		else
			{
			if (bThrust)
				SetThrustDir(iCurrentDir);
			else
				SetThrustDir(CAIShipControls::constNeverThrust);

#ifdef DEBUG_SHIP
			if (bDebug)
				g_pUniverse->DebugOutput("Turn: none (%d)", iDir);
#endif
			}
		}
	else
		{
		if (bThrust)
			SetThrustDir(pShip->GetRotation());
		else
			SetThrustDir(CAIShipControls::constNeverThrust);

#ifdef DEBUG_SHIP
		if (bDebug)
			g_pUniverse->DebugOutput("Turn: none");
#endif
		}
	}

void CAIBehaviorCtx::ImplementSpiralIn (CShip *pShip, const CVector &vTarget)

//	SpiralIn
//
//	Spiral in towards the target

	{
	DEBUG_TRY

	//	Curve inward

	CVector vInterceptPoint = CalcManeuverSpiralIn(pShip, vTarget);

	//	Face towards the interception point

	int iDirectionToFace = VectorToPolar(vInterceptPoint, NULL);
	ImplementManeuver(pShip, iDirectionToFace, true);

	DEBUG_CATCH
	}

void CAIBehaviorCtx::ImplementSpiralOut (CShip *pShip, const CVector &vTarget, int iTrajectory)

//	SpiralOut
//
//	Spiral out away from the target

	{
	DEBUG_TRY

	//	Curve out

	CVector vInterceptPoint = CalcManeuverSpiralOut(pShip, vTarget, iTrajectory);

	//	Face towards the interception point

	int iDirectionToFace = VectorToPolar(vInterceptPoint, NULL);
	ImplementManeuver(pShip, iDirectionToFace, true);

	DEBUG_CATCH
	}

void CAIBehaviorCtx::ImplementStop (CShip *pShip)

//	ImplementStop
//
//	Stops in space

	{
	Metric rSpeed;
	int iAngle = VectorToPolar(pShip->GetVel(), &rSpeed);
	Metric rThrust = pShip->GetThrust();

	if (rSpeed > g_KlicksPerPixel)
		{
		CVector vAccel = PolarToVector(iAngle, -rThrust);

		if (!pShip->IsParalyzed())
			pShip->Accelerate(vAccel, g_SecondsPerUpdate);
		}
	else
		{
		if (!pShip->IsParalyzed())
			pShip->Accelerate(-pShip->GetVel() * pShip->GetMass() / 2000.0, g_SecondsPerUpdate);
		}
	}

void CAIBehaviorCtx::ImplementTurnTo (CShip *pShip, int iRotation)

//	ImplementTurnTo
//
//	Turn towards the given angle

	{
	SetManeuver(pShip->GetManeuverToFace(iRotation));
	}


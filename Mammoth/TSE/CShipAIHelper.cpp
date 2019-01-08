//	CShipAIHelper.cpp
//
//	CShipAIHelper class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const Metric MAX_DISTANCE =				(400 * KLICKS_PER_PIXEL);
const Metric CLOSE_DELTA_V_RATIO =		0.12;

bool CShipAIHelper::CalcFormationParams (CShip *pShip, 
										 const CVector &vDestPos, 
										 const CVector &vDestVel, 
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

	return (rDiff2 < rCloseV2);
	}


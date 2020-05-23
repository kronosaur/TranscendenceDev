//	CPhysicsForceResolver.cpp
//
//	CPhysicsForceResolver class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

int CPhysicsForceResolver::AllocDesc (CSpaceObject &Obj)

//	AllocDesc
//
//	Alloc a descriptor for the given object.

	{
	int iIndex = m_Forces.GetCount();

	SForceDesc &Desc = *m_Forces.Insert();
	Desc.pObj = &Obj;

	return iIndex;
	}

void CPhysicsForceResolver::BeginUpdate (void)

//	BeginUpdate
//
//	Starts update

	{
	m_Forces.DeleteAll();
	m_Forces.GrowToFit(DEFAULT_ALLOC);
	}

void CPhysicsForceResolver::Update (CSystem &System, const Metric rSecondsPerUpdate)

//	Update
//
//	Sets the velocity for all objects that have a force applied.

	{
	CUsePerformanceCounter Counter(System.GetUniverse(), CONSTLIT("update.ForceResolver"));

	constexpr Metric EPSILON_SPEED = 0.01 * KLICKS_PER_PIXEL;

	for (int i = 0; i < m_Forces.GetCount(); i++)
		{
		const SForceDesc &Desc = m_Forces[i];
		CSpaceObject *pObj = Desc.pObj;
		const Metric rMass = pObj->GetMass();
		if (rMass <= 0.0)
			continue;

		//	Compute a factor incorporating mass.
		//	NOTE: The original code multiplies force by 1000, ostensibly
		//	because we want to convert to klicks per second. But since velocity
		//	is already in klicks per second, the logic is difficult to discern.
		//	We leave it here for backwards compatibility.

		Metric rFactor = rSecondsPerUpdate * 1000.0 / rMass;

		//	Accelerate with the unlimited forces first.

		CVector vVel = pObj->GetVel();
		if (!Desc.vForce.IsNull())
			vVel = vVel + (Desc.vForce * rFactor);

		//	If we also have limited forces, handle those.

		Metric rMaxSpeed;
		if (!Desc.vLimitedForce.IsNull())
			{
			//	We use epsilon because we don't want round-off error to make
			//	the speed creep up.

			rMaxSpeed = Max(pObj->GetMaxSpeed(), vVel.Length() - EPSILON_SPEED);

			vVel = vVel + (Desc.vLimitedForce * rFactor);
			}
		else
			rMaxSpeed = LIGHT_SPEED;

		vVel.Clip(rMaxSpeed);
		pObj->SetVel(vVel);
		}
	}

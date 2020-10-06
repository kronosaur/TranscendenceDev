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
	ASSERT(!Obj.IsDestroyed() && !Obj.IsAnchored());

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
	m_Forces.GrowToFit(DEFAULT_ALLOC);

	for (int i = 0; i < m_Forces.GetCount(); i++)
		{
		const SForceDesc &Desc = m_Forces[i];
		CSpaceObject *pObj = Desc.pObj;
		if (pObj->IsDestroyed() || pObj->IsAnchored())
			{
			pObj->ClearForceDesc();
			m_Forces.Delete(i);
			i--;
			}
		}
	}

void CPhysicsForceResolver::CleanUp (void)

//	CleanUp
//
//	Clean up any cached information.

	{
	for (int i = 0; i < m_Forces.GetCount(); i++)
		{
		const SForceDesc &Desc = m_Forces[i];
		CSpaceObject *pObj = Desc.pObj;
		pObj->ClearForceDesc();
		}

	m_Forces.DeleteAll();
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
		if (pObj->IsDestroyed() || pObj->IsAnchored())
			{
			pObj->ClearForceDesc();
			continue;
			}

		CVector vVel = pObj->GetVel();
		Metric rMaxSpeed = pObj->GetMaxSpeed();
		Metric rClipSpeed = -1.0;

		//	Apply forces

		const Metric rMass = pObj->GetMass();
		if (rMass > 0.0)
			{
			//	Compute a factor incorporating mass.
			//	NOTE: The original code multiplies force by 1000, ostensibly
			//	because we want to convert to klicks per second. But since velocity
			//	is already in klicks per second, the logic is difficult to discern.
			//	We leave it here for backwards compatibility.

			Metric rFactor = rSecondsPerUpdate * 1000.0 / rMass;

			//	Accelerate with the unlimited forces first.

			if (!Desc.vForce.IsNull())
				vVel = vVel + (Desc.vForce * rFactor);

			//	If we also have limited forces, handle those.

			if (!Desc.vLimitedForce.IsNull())
				{
				//	We use epsilon because we don't want round-off error to make
				//	the speed creep up.

				rClipSpeed = Min(Max(rMaxSpeed, vVel.Length() - EPSILON_SPEED), LIGHT_SPEED);

				vVel = vVel + (Desc.vLimitedForce * rFactor);
				}
			else
				rClipSpeed = LIGHT_SPEED;
			}

		//	Apply drag

		if (Desc.rDragFactor < 1.0)
			{
			if (vVel.IsNull())
				;

			//	If we're moving really slowly, force to 0.

			else if (vVel.Length2() < MIN_DRAG_SPEED2)
				vVel = CVector();
			else
				vVel = CVector(vVel.GetX() * Desc.rDragFactor, vVel.GetY() * Desc.rDragFactor);
			}

		if (rClipSpeed >= 0.0)
			vVel.Clip(rClipSpeed);

		pObj->SetVel(vVel);
		pObj->ClearForceDesc();
		}

	m_Forces.DeleteAll();
	}

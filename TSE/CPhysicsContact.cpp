//	CPhysicsContact.cpp
//
//	CPhysicsContact class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const Metric COLLISION_RESTITUTION = 0.5;
const Metric PENETRATION_STEP = 2.0 * g_KlicksPerPixel;

Metric CPhysicsContact::GetSeparatingVel (void) const

//	GetSeparatingVel
//
//	Compute the current velocity along the contact normal.

	{
	if (!m_bSeparatingVel)
		{
		CVector vVel = m_pObj->GetVel() - m_pContactObj->GetVel();
		m_rSeparatingVel = vVel.Dot(m_vContactNormal);
		m_bSeparatingVel = true;
		}

	return m_rSeparatingVel;
	}

void CPhysicsContact::Init (CSpaceObject *pObj, CSpaceObject *pContactObj, const CVector &vNormal, Metric rPenetration, Metric rRestitution)

//	Init
//
//	Initializes a contact

	{
	m_pObj = pObj;
	m_pContactObj = pContactObj;
	m_iType = contactCollision;
	m_vContactNormal = vNormal;
	m_rPenetration = rPenetration;
	m_rRestitution = rRestitution;

	InitDerivedValues();
	}

void CPhysicsContact::InitCollision (CSpaceObject *pObj, CSpaceObject *pContactObj)

//	InitCollision
//
//	Initializes a contact as a collision.

	{
	m_pObj = pObj;
	m_pContactObj = pContactObj;

	//	The contact normal points away from the object that we collided with.
	//	NOTE: The object position should be the NEW position (after movement).
	//
	//	If we hit a barrier, then our contact normal is back along our line
	//	of movement (this is the easiest way to insure that we will be out of 
	//	the barrier).

	if (pContactObj->IsAnchored())
		{
		m_iType = contactBarrierCollision;
		m_vContactNormal = -pObj->GetVel().Normal(&m_rPenetration);
		}
	else
		{
		m_iType = contactCollision;
		m_vContactNormal = (pObj->GetPos() - pContactObj->GetPos()).Normal();

		//	Step back along the contact normal until we are no longer overlapping
		//	the contact object.

		m_rPenetration = PENETRATION_STEP;
		CVector vStep = PENETRATION_STEP * m_vContactNormal;
		CVector vTestPos = pObj->GetPos() + vStep;
		while (pObj->ObjectInObject(vTestPos, pContactObj, pContactObj->GetPos()))
			{
			vTestPos = vTestPos + vStep;
			m_rPenetration += PENETRATION_STEP;
			}
		}

	//	We lose some energy in the bounce

	m_rRestitution = COLLISION_RESTITUTION;

	//	Initialize temps

	InitDerivedValues();
	}

void CPhysicsContact::InitDerivedValues (void)

//	InitDerivedValues
//
//	Initialize temporary values. This must be called by all InitXXX functions.

	{
	m_rInvMass1 = m_pObj->GetInvMass();
	m_rInvMass2 = m_pContactObj->GetInvMass();
	m_rTotalInvMass = m_rInvMass1 + m_rInvMass2;

	m_bSeparatingVel = false;
	}

void CPhysicsContact::OnUpdateDone (void)

//	OnUpdateDone
//
//	Contact has been resolved.

	{
	switch (m_iType)
		{
		case contactCollision:
		case contactBarrierCollision:
			m_pObj->OnBounce(m_pContactObj, m_pObj->GetPos());
			m_pContactObj->OnObjBounce(m_pObj, m_pObj->GetPos());
			break;
		}
	}

void CPhysicsContact::Recalc (const CVector &vMove)

//	Recalc
//
//	One of our objects has moved, so we need to update the penetration.

	{
	m_rPenetration += vMove.Dot(m_vContactNormal);

	//	We also need to recalc the separating velocity.

	m_bSeparatingVel = false;
	}

void CPhysicsContact::Resolve (void)

//	Resolve
//
//	Resolve the contact

	{
	switch (m_iType)
		{
		//	Collisions of ship with barriers get handled differently so that we
		//	avoid getting stuck.

		case contactBarrierCollision:
			m_pObj->SetPos(m_pObj->GetOldPos());
			m_pObj->SetVel(-m_rRestitution * m_pObj->GetVel());
			m_rPenetration = 0.0;
			m_bSeparatingVel = false;
			break;

		default:
			ResolveInterpenetration();
			ResolveVelocity();
			break;
		}
	}

void CPhysicsContact::ResolveInterpenetration (void)

//	ResolveInterpenetration
//
//	Move objects so they are not penetrating.

	{
	if (m_rPenetration <= 0.0 || m_rTotalInvMass <= 0.0)
		return;

	//	Compute the amount of penetration resolution per unit of inverse mass.

	CVector vMovePerInvMass = m_vContactNormal * (m_rPenetration / m_rTotalInvMass);

	//	Move both objects

	m_vMove1 = vMovePerInvMass * m_rInvMass1;
	m_pObj->SetPos(m_pObj->GetPos() + m_vMove1);

	if (m_rInvMass2 > 0.0)
		{
		m_vMove2 = vMovePerInvMass * -m_rInvMass2;
		m_pContactObj->SetPos(m_pContactObj->GetPos() + m_vMove2);
		}

	//	Update the penetration

	m_rPenetration = 0.0;
	}

void CPhysicsContact::ResolveVelocity (void)

//	ResolveVelocity
//
//	Change object velocities.

	{
	Metric rSeparatingV = GetSeparatingVel();

	//	If we're already separating, then nothing to do.

	if (rSeparatingV > 0.0 || m_rTotalInvMass <= 0.0)
		return;

	//	Compute the new separating velocity (the "bound")

	Metric newSeparatingV = -rSeparatingV * m_rRestitution;
	Metric rDeltaV = newSeparatingV - rSeparatingV;

	//	Compute the amount of impulse per unit of inverse mass.

	CVector vImpulsePerInvMass = m_vContactNormal * rDeltaV / m_rTotalInvMass;

	//	Apply impulse to both objects

	m_pObj->DeltaV(vImpulsePerInvMass * m_rInvMass1);

	if (m_rInvMass2 > 0.0)
		m_pContactObj->DeltaV(vImpulsePerInvMass * -m_rInvMass2);

	//	Invalidate separating velocity

	m_bSeparatingVel = false;
	}

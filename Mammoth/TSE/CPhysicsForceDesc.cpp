//	CPhysicsForceDesc.cpp
//
//	CPhysicsForceDesc class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CPhysicsForceDesc::AddForce (CPhysicsForceResolver &ForceResolver, CSpaceObject &Obj, const CVector &vForce)

//	AddForce
//
//	Adds a force on the object.

	{
	auto &Desc = GetDesc(ForceResolver, Obj);
	Desc.vForce = Desc.vForce + vForce;
	}

void CPhysicsForceDesc::AddForceLimited (CPhysicsForceResolver &ForceResolver, CSpaceObject &Obj, const CVector &vForce)

//	AddForceLimited
//
//	Adds a force, but the force cannot force the object to exceed its maximum 
//	speed.

	{
	auto &Desc = GetDesc(ForceResolver, Obj);
	Desc.vLimitedForce = Desc.vLimitedForce + vForce;
	}

CPhysicsForceResolver::SForceDesc &CPhysicsForceDesc::GetDesc (CPhysicsForceResolver &ForceResolver, CSpaceObject &Obj)

//	GetDesc
//
//	Returns the descriptor for this object.

	{
	//	If we don't have a descriptor yet, allocate one.

	if (m_iIndex == -1)
		m_iIndex = ForceResolver.AllocDesc(Obj);

	return ForceResolver.GetDesc(m_iIndex);
	}

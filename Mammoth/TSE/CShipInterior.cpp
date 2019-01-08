//	CShipInterior.cpp
//
//	CShipInterior class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CShipInterior::CalcAttachPos (CShip *pShip, const CShipInteriorDesc &Desc, int iIndex, CSpaceObject **retpAttachedTo, CVector *retvPos) const

//	CalcAttachPos
//
//	Computes the attach position for a compartment.

	{
	const SCompartmentDesc &CompDesc = Desc.GetCompartment(iIndex);
	ASSERT(CompDesc.fIsAttached);

	//	Compute the object to which we are attached.

	CSpaceObject *pAttachedTo;
	if (CompDesc.sAttachID.IsBlank() || !FindAttachedObject(Desc, CompDesc.sAttachID, &pAttachedTo))
		pAttachedTo = pShip;

	//	Compute the position of the attached object.
	//
	//	NOTE: We assume a rotation of 0 and then rotate in 2D to match the 
	//	ship's rotation. We don't do a 3D rotation because the position of
	//	sections is constant no matter the ship rotation.
	//
	//	If we want the section position to change, we need to be able to 
	//	specify object joints as 3D positions.

	CVector vPos;
	CompDesc.AttachPos.CalcCoord(pShip->GetImageScale(), &vPos);
	vPos.Rotate(pShip->GetRotation());
	vPos = pAttachedTo->GetPos() + vPos;

	//	Done

	if (retpAttachedTo)
		*retpAttachedTo = pAttachedTo;

	if (retvPos)
		*retvPos = vPos;
	}

void CShipInterior::CreateAttached (CShip *pShip, const CShipInteriorDesc &Desc)

//	CreateAttached
//
//	Create attached compartments.

	{
	int i;

	CSystem *pSystem = pShip->GetSystem();

	for (i = 0; i < Desc.GetCount(); i++)
		{
		const SCompartmentDesc &Comp = Desc.GetCompartment(i);
		if (!Comp.fIsAttached)
			continue;

		//	Class must be a segment

		if (!Comp.Class->IsShipSection())
			{
			::kernelDebugLogPattern("Ship class %08x must be a ship compartment.", Comp.Class.GetUNID());
			continue;
			}

		//	Compute the position to which we are attached.

		CSpaceObject *pAttachedTo;
		CVector vPos;
		CalcAttachPos(pShip, Desc, i, &pAttachedTo, &vPos);

		//	Create the new section

		CShip *pNewSection;
		if (pSystem->CreateShip(Comp.Class.GetUNID(),
				NULL,
				NULL,
				pShip->GetSovereign(),
				vPos,
				CVector(),
				0,
				NULL,
				NULL,
				&pNewSection) != NOERROR)
			{
			::kernelDebugLogPattern("Unable to create section %08x for ship class %08x", Comp.Class.GetUNID(), pShip->GetClass()->GetUNID());
			continue;
			}

		//	Set the ship as a compartment of us.

		pNewSection->SetAsShipSection(pShip);

		//	We remember this ship.

		SetAttached(i, pNewSection);

		//	Create a joint

		pSystem->AddJoint(CObjectJoint::jointSpine, pAttachedTo, pNewSection);
		}
	}

EDamageResults CShipInterior::Damage (CShip *pShip, const CShipInteriorDesc &Desc, SDamageCtx &Ctx)

//	Damage
//
//	Ship interior takes damage.

	{
	//	Damage requires mass destruction power

    Ctx.iDamage = mathAdjust(Ctx.iDamage, Ctx.Damage.GetMassDestructionAdj());
    if (Ctx.iDamage == 0)
        return damageArmorHit;

	//	Transform the hit position to coordinates relative to the standard image.

	SHitTestCtx HitCtx(pShip, Desc);
	CVector vPos = (Ctx.vHitPos - pShip->GetPos()).Rotate(-pShip->GetRotation() + 360);
	int xHitPos = (int)((vPos.GetX() / g_KlicksPerPixel) + 0.5);
	int yHitPos = -(int)((vPos.GetY() / g_KlicksPerPixel) + 0.5);

	//	See if we hit a device on the ship

	CInstalledDevice *pDevice;
	if (pShip->FindDeviceAtPos(Ctx.vHitPos, &pDevice)
			&& !pDevice->IsDamaged()
			&& mathRandom(1, 100) <= 50)
		pShip->DamageDevice(pDevice, Ctx);

	//	Loop until we've accounted for all damage.

	while (Ctx.iDamage > 0)
		{
		//	Pick a compartment to take damage, based on where the shot hit and
		//	the state of all compartments.

		int iComp = FindNextCompartmentHit(HitCtx, xHitPos, yHitPos);
		if (iComp == -1)
			return damageDestroyed;

		SCompartmentEntry &Comp = m_Compartments[iComp];
		const SCompartmentDesc &CompDesc = Desc.GetCompartment(iComp);

		//	Compartment absorbs hit points

		int iDamageAbsorbed = Min(Comp.iHP, Ctx.iDamage);
		Comp.iHP -= iDamageAbsorbed;
		Ctx.iDamage -= iDamageAbsorbed;

		//	If this is a direct hit on the compartment, then other things
		//	might happen.

		if (Comp.bHit)
			{
			switch (CompDesc.iType)
				{
				//	If this compartment is the engine room, then there is a chance that
				//	the ship will be dead in space.

				case deckMainDrive:
					{
					int iDamage = 100 - (Comp.iHP * 100 / CompDesc.iMaxHP);
					if (mathRandom(1, 100) <= iDamage)
						pShip->DamageDrive(Ctx);
					break;
					}

				//	If this compartment has the cargo hold, then we randomly damage some
				//	items.

				case deckCargo:
					{
					int iDamage = 100 - (Comp.iHP * 100 / CompDesc.iMaxHP);
					if (mathRandom(1, 100) <= iDamage)
						pShip->DamageCargo(Ctx);
					break;
					}
				}
			}
		}

	return damageArmorHit;
	}

void CShipInterior::DetachChain (CShip *pShip, CSpaceObject *pBreak)

//	DetachChain
//
//	Check all our attached objects. If any of them are no longer connected to 
//	the root after pBreak has been removed, then we remove them.

	{
	int i;

	//	Clear our state.
	//
	//	bMarked is TRUE if we have determined whether this section is 
	//	attached or not.
	//
	//	bHit is TRUE if the section is attached (valid only if bMarked).

	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		m_Compartments[i].bHit = false;
		m_Compartments[i].bMarked = false;
		}

	//	Now recursively see which are attached or not.

	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		CSpaceObject *pAttached = GetAttached(i);
		if (pAttached)
			{
			if (!MarkIfAttached(pShip, i, pBreak))
				{
				//	No longer part of a larger ship

				CShip *pShip = m_Compartments[i].pAttached->AsShip();
				if (pShip)
					pShip->SetAsShipSection(NULL);

				//	Remove it.

				m_Compartments[i].pAttached = NULL;
				}
			}
		}
	}

bool CShipInterior::FindAttachedObject (CSpaceObject *pAttached, int *retiIndex) const

//	FindAttachedObject
//
//	Finds the object in our list.

	{
	int i;

	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		if (m_Compartments[i].pAttached == pAttached)
			{
			if (retiIndex)
				*retiIndex = i;

			return true;
			}
		}

	return false;
	}

bool CShipInterior::FindAttachedObject (const CShipInteriorDesc &Desc, const CString &sID, CSpaceObject **retpObj) const

//	FindAttachedObject
//
//	Returns an attached object by ID.

	{
	int i;

	for (i = 0; i < Desc.GetCount(); i++)
		if (strEquals(sID, Desc.GetCompartment(i).sID))
			{
			if (retpObj)
				*retpObj = m_Compartments[i].pAttached;

			return (m_Compartments[i].pAttached != NULL);
			}

	return false;
	}

int CShipInterior::FindNextCompartmentHit (SHitTestCtx &HitCtx, int xHitPos, int yHitPos)

//	FindCompartmentHit
//
//	Returns a pointer to the next unmarked compartment that will take damage 
//	from a hit at the given position. Returns -1 if we have no more 
//	compartments.
//
//	We mark a compartment after it has been returned so that it will not be
//	returned twice.

	{
	int i;

	//	If we haven't yet initialized the hit context, then we do so now.

	if (HitCtx.iPos == -1)
		{
		//	Loop over all compartments and add them in proper order.

		for (i = 0; i < m_Compartments.GetCount(); i++)
			{
			DWORD dwPriority;
			const SCompartmentDesc &CompDesc = HitCtx.Desc.GetCompartment(i);

			//	Attached compartments are not hit via this code (they are
			//	separate objects).

			if (CompDesc.fIsAttached)
				continue;

			//	Check to see if the compartment was hit directly.

			m_Compartments[i].bHit = PointInCompartment(HitCtx, CompDesc, xHitPos, yHitPos);

			//	If the hit position is in the compartment, then it has a high
			//	priority.

			if (m_Compartments[i].bHit)
				dwPriority = 0x00000000 | mathRandom(0, 0xFFFF);

			//	Otherwise, if this is a default compartment, then it has the 
			//	next highest priority.

			else if (CompDesc.fDefault)
				dwPriority = 0x00010000 | mathRandom(0, 0xFFFF);

			//	Otherwise, lowest priority

			else
				dwPriority = 0x00020000 | mathRandom(0, 0xFFFF);

			//	Add to the ordered list.

			HitCtx.HitOrder.Insert(dwPriority, i);
			}

		//	Start at the beginning

		HitCtx.iPos = 0;
		}

	//	If we're at the end of the list, then we're done

	if (HitCtx.iPos	>= HitCtx.HitOrder.GetCount())
		return -1;

	//	Return the compartment and increment the position

	else
		return HitCtx.HitOrder.GetValue(HitCtx.iPos++);
	}

void CShipInterior::GetHitPoints (CShip *pShip, const CShipInteriorDesc &Desc, int *retiHP, int *retiMaxHP) const

//	GetHitPoints
//
//	Returns the hit points of all compartments.

	{
	int i;

	int iMaxHP = 0;
	int iHP = 0;
	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		iMaxHP += Desc.GetCompartment(i).iMaxHP;
		iHP += m_Compartments[i].iHP;
		}

	if (retiHP)
		*retiHP = iHP;

	if (retiMaxHP)
		*retiMaxHP = iMaxHP;
	}

void CShipInterior::Init (const CShipInteriorDesc &Desc)

//	Init
//
//	Initialize the structure

	{
	int i;

	m_Compartments.DeleteAll();
	m_Compartments.InsertEmpty(Desc.GetCount());
	for (i = 0; i < Desc.GetCount(); i++)
		{
		const SCompartmentDesc &CompDesc = Desc.GetCompartment(i);
		SCompartmentEntry &Entry = m_Compartments[i];

		Entry.iHP = CompDesc.iMaxHP;
		}
	}

bool CShipInterior::MarkIfAttached (CShip *pShip, int iSection, CSpaceObject *pBreak)

//	MarkIfAttached
//
//	Returns TRUE if pSection is still attached to pShip even after pBreak has
//	been removed.
//
//	We rely on obj marking to not have to recalculate markings.

	{
	SCompartmentEntry &Section = m_Compartments[iSection];
	CSpaceObject *pSection = Section.pAttached;

	//	If we're already marked it means that we know whether we're attached or not.

	if (Section.bMarked)
		return Section.bHit;

	//	Mark this as processing, so we don't recurse.

	Section.bHit = true;

	//	Loop over all our joints.

	int iAttachedTo;
	CObjectJoint *pNext = pSection->GetFirstJoint();
	while (pNext)
		{
		CSpaceObject *pJoinedTo = pNext->GetOtherObj(pSection);

		//	If we're joined to the ship, then we're clearly attached.

		if (pJoinedTo == pShip)
			{
			Section.bMarked = true;
			Section.bHit = true;
			return true;
			}

		//	If this is a joint to the break, then skip it, since it doesn't
		//	count.

		else if (pJoinedTo == pBreak)
			{ }

		//	Otherwise, see if this is a section.

		else if (FindAttachedObject(pJoinedTo, &iAttachedTo))
			{
			SCompartmentEntry &AttachedTo = m_Compartments[iAttachedTo];

			//	If we already have a result, then see if we're attached.

			if (AttachedTo.bMarked)
				{
				if (AttachedTo.bHit)
					{
					Section.bMarked = true;
					Section.bHit = true;
					return true;
					}
				}

			//	If we're processing, skip

			else if (AttachedTo.bHit)
				{ }

			//	Otherwise, we might need to recurse

			else
				{
				if (MarkIfAttached(pShip, iAttachedTo, pBreak))
					{
					Section.bMarked = true;
					Section.bHit = true;
					return true;
					}
				}
			}

		//	Otherwise, this is some other joined object, so we don't care.

		//	Next

		pNext = pNext->GetNextJoint(pSection);
		}

	//	If we get this far, then we're not attached.

	Section.bMarked = true;
	Section.bHit = false;

	return false;
	}

void CShipInterior::OnDestroyed (CShip *pShip, const SDestroyCtx &Ctx)

//	OnDestroyed
//
//	See if one of our attached objects has been destroyed.

	{
	int i;

	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		CSpaceObject *pAttached = GetAttached(i);
		if (pAttached == Ctx.pObj)
			{
			//	Remove this object from the list.

			m_Compartments[i].pAttached = NULL;

			//	Figure out which other objects in our list need to be detached.

			DetachChain(pShip, pAttached);
			break;
			}
		}
	}

void CShipInterior::OnNewSystem (CSystem *pSystem, CShip *pShip, const CShipInteriorDesc &Desc)

//	OnNewSystem
//
//	Our root object has entered a new system, so we need to recreate the joint
//	structure.

	{
	int i;

	for (i = 0; i < Desc.GetCount(); i++)
		{
		const SCompartmentDesc &Comp = Desc.GetCompartment(i);
		if (!Comp.fIsAttached || m_Compartments[i].pAttached == NULL)
			continue;

		//	Compute the position to which we are attached.

		CSpaceObject *pAttachedTo;
		CVector vPos;
		CalcAttachPos(pShip, Desc, i, &pAttachedTo, &vPos);

		//	Place the object

		m_Compartments[i].pAttached->Place(vPos, pShip->GetVel());
		m_Compartments[i].pAttached->AddToSystem(pSystem);

		//	Create a joint

		pSystem->AddJoint(CObjectJoint::jointSpine, pAttachedTo, m_Compartments[i].pAttached);
		}
	}

void CShipInterior::OnPlace (CShip *pShip, const CVector &vOldPos)

//	OnPlace
//
//	Move all attached compartments.

	{
	int i;
	CVector vMove = pShip->GetPos() - vOldPos;

	for (i = 0; i < m_Compartments.GetCount(); i++)
		if (m_Compartments[i].pAttached)
			m_Compartments[i].pAttached->Place(m_Compartments[i].pAttached->GetPos() + vMove, pShip->GetVel());
	}

bool CShipInterior::PointInCompartment (SHitTestCtx &HitCtx, const SCompartmentDesc &CompDesc, int xHitPos, int yHitPos) const

//	PointInCompartment
//
//	Returns TRUE if the given point is in the given compartment.

	{
	return (xHitPos >= CompDesc.rcPos.left
			&& xHitPos < CompDesc.rcPos.right
			&& yHitPos >= CompDesc.rcPos.top
			&& yHitPos < CompDesc.rcPos.bottom);
	}

void CShipInterior::ReadFromStream (CShip *pShip, const CShipInteriorDesc &Desc, SLoadCtx &Ctx)

//	ReadFromStream
//
//	DWORD			Compartment count
//	
//	For each compartment
//		DWORD		iHP
//		DWORD		pAttached

	{
	int i;

	//	We always allocate to match the descriptor, but we may have stored
	//	a different number of compartments, so we need to deal with that.

	m_Compartments.InsertEmpty(Desc.GetCount());

	DWORD dwCount;
	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	for (i = 0; i < (int)dwCount; i++)
		{
		if (i >= m_Compartments.GetCount())
			{
			DWORD dwDummy;
			Ctx.pStream->Read(dwDummy);
			if (Ctx.dwVersion >= 149)
				Ctx.pStream->Read(dwDummy);
			continue;
			}

		Ctx.pStream->Read(m_Compartments[i].iHP);
		if (Ctx.dwVersion >= 149)
			Ctx.pSystem->ReadObjRefFromStream(Ctx, &m_Compartments[i].pAttached);
		}
	}

bool CShipInterior::RepairHitPoints (CShip *pShip, const CShipInteriorDesc &Desc, int iRepairHP)

//	RepairHitPoints
//
//	Repairs up to iRepairHP hit points. We return TRUE if we needed to be 
//	repaired (regardless of the value if iRepairHP).

	{
	int i;

	//	First compute the current and maximum hit points.

	int iHP = 0;
	int iMaxHP = 0;
	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		iHP += m_Compartments[i].iHP;
		iMaxHP += Desc.GetCompartment(i).iMaxHP;
		}

	//	If no HP, then we're done

	if (iMaxHP <= iHP)
		return false;

	//	Adjust repair HP so we don't overshoot.

	iRepairHP = Min(iRepairHP, iMaxHP - iHP);

	//	Short-circuit, but we return TRUE to indicate that we need to repair
	//	damage.

	if (iRepairHP == 0)
		return true;

	//	Set HP to new value

	SetHitPoints(pShip, Desc, iHP + iRepairHP);

	//	Done

	return true;
	}

void CShipInterior::SetHitPoints (CShip *pShip, const CShipInteriorDesc &Desc, int iHP)

//	SetHitPoints
//
//	Sets total hit points, spreading out across all compartments.

	{
	int i;

	//	First compute the maximum hit points for all compartments so we can
	//	compute a ratio.

	int iMaxHP = 0;
	for (i = 0; i < m_Compartments.GetCount(); i++)
		iMaxHP += Desc.GetCompartment(i).iMaxHP;

	//	If no HP, then we're done

	if (iMaxHP == 0)
		return;

	//	We can't exceed maximum

	iHP = Max(0, Min(iMaxHP, iHP));

	//	Compute the ratio

	Metric rRatio = (Metric)iHP / (Metric)iMaxHP;

	//	Now set the HP for all compartments

	int iAllocated = 0;
	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		int iCompartmentMaxHP = Desc.GetCompartment(i).iMaxHP;
		if (iCompartmentMaxHP > 0)
			m_Compartments[i].iHP = (int)(rRatio * (Metric)m_Compartments[i].iHP / (Metric)iCompartmentMaxHP);
		else
			m_Compartments[i].iHP = 0;

		iAllocated += m_Compartments[i].iHP;
		}

	//	If necessary, we allocate any HP left over (due to round off).

	int iRemaining = iHP - iAllocated;
	for (i = 0; i < m_Compartments.GetCount() && iRemaining > 0; i++)
		{
		int iCompartmentMaxHP = Desc.GetCompartment(i).iMaxHP;
		if (m_Compartments[i].iHP < iCompartmentMaxHP)
			{
			int iInc = Min(iCompartmentMaxHP - m_Compartments[i].iHP, iRemaining);
			m_Compartments[i].iHP += iInc;
			iRemaining -= iInc;
			}
		}
	}

void CShipInterior::WriteToStream (CShip *pShip, IWriteStream *pStream)

//	WriteToStream
//
//	Write the structure

	{
	int i;

	DWORD dwSave = m_Compartments.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	for (i = 0; i < m_Compartments.GetCount(); i++)
		{
		pStream->Write(m_Compartments[i].iHP);
		pShip->GetSystem()->WriteObjRefToStream(m_Compartments[i].pAttached, pStream);
		}
	}

//	CShipInterior.cpp
//
//	CShipInterior class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

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

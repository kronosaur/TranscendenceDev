//	CStationHull.cpp
//
//	CStationHull class
//	Copyright 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_CANNOT_BE_HIT					CONSTLIT("cannotBeHit")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_HULL_TYPE						CONSTLIT("hullType")
#define PROPERTY_IMMUTABLE						CONSTLIT("immutable")
#define PROPERTY_MAX_HP							CONSTLIT("maxHP")
#define PROPERTY_MAX_STRUCTURAL_HP				CONSTLIT("maxStructuralHP")
#define PROPERTY_STRUCTURAL_HP					CONSTLIT("structuralHP")

const int REGEN_PER_DAY_FACTOR =		10;

CStationHull::CStationHull (void) :
		m_fImmutable(false),
		m_fCannotBeHit(false)

//	CStationHull constructor

	{
	}

int CStationHull::CalcAdjustedDamage (SDamageCtx &Ctx) const

//	CalcAdjustedDamage
//
//	Adjusts damage because some hulls require WMD.

	{
	//	Short-circuit

	if (Ctx.iDamage == 0)
		return 0;

	//	Depending on hull type we need special damage to penetrate.

	int iSpecialDamage;
	switch (m_iType)
		{
		//	Multi-hull stations require WMD.

		case CStationHullDesc::hullMultiple:
			iSpecialDamage = Ctx.Damage.GetMassDestructionDamage();
			break;

		//	Stations built on asteroids must be attacked with either WMD or
		//	mining damage.

		case CStationHullDesc::hullAsteroid:
			iSpecialDamage = Max(Ctx.Damage.GetMassDestructionDamage(), Ctx.Damage.GetMiningDamage());
			break;

		//	Underground stations must be attacked with  mining damage.

		case CStationHullDesc::hullUnderground:
			iSpecialDamage = Ctx.Damage.GetMiningDamage();
			break;

		//	For single-hull stations we don't need special damage, except if the
		//	station is abandoned, in which case we need WMD damage.

		case CStationHullDesc::hullSingle:
		default:
			if (IsAbandoned())
				iSpecialDamage = Ctx.Damage.GetMassDestructionDamage();
			else
				iSpecialDamage = -1;
			break;
		}

	//	If we're abandoned and the attack does not have any special damage, then
	//	no damage. NOTE: We check iSpecialDamage rather than iDamageAdj because
	//	iDamageAdj is non-zero even if WMD = 0 (there is always a little bit of
	//	WMD).

	if (IsAbandoned() && iSpecialDamage <= 0)
		return 0;

	//	If we don't need special damage, then we do full damage.

	else if (iSpecialDamage == -1)
		return Ctx.iDamage;

	//	Otherwise, we adjust the damage.

	else
		{
        int iDamageAdj = DamageDesc::GetMassDestructionAdjFromValue(iSpecialDamage);
        return mathAdjust(Ctx.iDamage, iDamageAdj);
		}
	}

ICCItem *CStationHull::FindProperty (const CString &sProperty) const

//	FindProperty
//
//	Returns the property (or NULL if not found).

	{
	if (strEquals(sProperty, PROPERTY_CANNOT_BE_HIT))
		return CCodeChain::CreateBool(!CanBeHit());

	else if (strEquals(sProperty, PROPERTY_HP))
		return CCodeChain::CreateInteger(m_iHitPoints);

	else if (strEquals(sProperty, PROPERTY_HULL_TYPE))
		return CCodeChain::CreateString(CStationHullDesc::GetID(m_iType));

	else if (strEquals(sProperty, PROPERTY_IMMUTABLE))
		return CCodeChain::CreateBool(IsImmutable());

	else if (strEquals(sProperty, PROPERTY_MAX_HP))
		return CCodeChain::CreateInteger(m_iMaxHitPoints);

	else if (strEquals(sProperty, PROPERTY_MAX_STRUCTURAL_HP))
		return CCodeChain::CreateInteger(m_iMaxStructuralHP);

	else if (strEquals(sProperty, PROPERTY_STRUCTURAL_HP))
		return CCodeChain::CreateInteger(m_iStructuralHP);

	else
		return NULL;
	}

EDamageResults CStationHull::GetPassthroughDefault (void) const

//	GetPassthroughDefault
//
//	Returns the default damage result when hit by passthrough

	{
	if (IsImmutable())
		return damageNoDamageNoPassthrough;
	else if (IsAbandoned())
		{
		if (m_iStructuralHP > 0)
			return damageStructuralHit;
		else
			return damageNoDamageNoPassthrough;
		}
	else
		return damageArmorHit;
	}

int CStationHull::GetVisibleDamage (void) const

//	GetVisibleDamage
//
//	Returns the amount of damage (%) that the object has taken

	{
	int iMaxHP;
	int iHP;

	if (IsAbandoned() && m_iStructuralHP > 0)
		{
		iMaxHP = m_iMaxStructuralHP;
		iHP = m_iStructuralHP;
		}
	else
		{
		iMaxHP = m_iMaxHitPoints;
		iHP = m_iHitPoints;
		}

	if (iMaxHP > 0)
		return 100 - (iHP * 100 / iMaxHP);
	else
		return 0;
	}

void CStationHull::GetVisibleDamageDesc (SVisibleDamage &Damage) const

//	GetVisibleDamageDesc
//
//	Returns the amount of damage (%) that the object has taken

	{
	int iMaxHP;
	int iHP;

	Damage.iShieldLevel = -1;

	if (IsAbandoned() && m_iStructuralHP > 0)
		{
		iMaxHP = m_iMaxStructuralHP;
		iHP = m_iStructuralHP;

		Damage.iArmorLevel = -1;
		Damage.iHullLevel = (iMaxHP > 0 ? (iHP * 100 / iMaxHP) : -1);
		}
	else
		{
		iMaxHP = m_iMaxHitPoints;
		iHP = m_iHitPoints;

		Damage.iArmorLevel = (iMaxHP > 0 ? (iHP * 100 / iMaxHP) : -1);
		Damage.iHullLevel = -1;
		}
	}

void CStationHull::Init (const CStationHullDesc &Desc)

//	Init
//
//	Initialize from a hull descriptor. NOTE: This should not be called before
//	design bind.

	{
	//	If we're using scalable armor, then get the appropriate armor level.

	CArmorClass *pArmorClass = Desc.GetArmorClass();
	if (pArmorClass && pArmorClass->GetItemType()->IsScalable())
		m_iArmorLevel = Desc.GetArmorLevel();
	else
		m_iArmorLevel = 0;

	//	Initialize hit points (both current and max HP can be changed by 
	//	specific station objects.

	m_iHitPoints = Desc.GetHitPoints();
	m_iMaxHitPoints = Desc.GetMaxHitPoints();

	m_iStructuralHP = Desc.GetStructuralHP();
	m_iMaxStructuralHP = Desc.GetMaxStructuralHP();

	//	Flags

	m_fImmutable = Desc.IsImmutable();
	m_iType = Desc.GetHullType();
	m_fCannotBeHit = !Desc.CanBeHit();
	}

void CStationHull::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from a stream
//
//	DWORD				Flags
//	DWORD				m_iArmorLevel
//	DWORD				m_iHitPoints
//	DWORD				m_iMaxHitPoints
//	DWORD				m_iStructuralHP
//	DWORD				m_iMaxStructuralHP

	{
	DWORD dwFlags;

	Ctx.pStream->Read(dwFlags);
	m_fImmutable =		((dwFlags & 0x00000001) ? true : false);
	//	0x00000002 Unused at version 176
	m_fCannotBeHit =	((dwFlags & 0x00000004) ? true : false);

	//	We encode hull type in the flags DWORD

	if (Ctx.dwVersion >= 176)
		m_iType = (CStationHullDesc::EHullTypes)((dwFlags & HULL_TYPES_MASK) >> HULL_TYPES_SHIFT);
	else
		{
		bool bMultiHull =	((dwFlags & 0x00000002) ? true : false);
		m_iType = (bMultiHull ? CStationHullDesc::hullMultiple : CStationHullDesc::hullSingle);
		}

	//	Other values

	Ctx.pStream->Read(m_iArmorLevel);
	Ctx.pStream->Read(m_iHitPoints);
	Ctx.pStream->Read(m_iMaxHitPoints);
	Ctx.pStream->Read(m_iStructuralHP);
	Ctx.pStream->Read(m_iMaxStructuralHP);
	}

bool CStationHull::SetPropertyIfFound (const CString &sProperty, ICCItem *pValue, CString *retsError)

//	SetPropertyIfFound
//
//	If the given property is found, we return TRUE; otherwise, we return FALSE.
//
//	If setting the property results in an error, we return TRUE, but retsError is
//	initialized.

	{
	if (strEquals(sProperty, PROPERTY_CANNOT_BE_HIT))
		m_fCannotBeHit = !pValue->IsNil();

	else if (strEquals(sProperty, PROPERTY_HP))
		{
		//	Nil means that we don't want to make a change

		if (pValue->IsNil())
			return true;

		m_iHitPoints = Min(Max(0, pValue->GetIntegerValue()), m_iMaxHitPoints);
		}
	else if (strEquals(sProperty, PROPERTY_IMMUTABLE))
		m_fImmutable = !pValue->IsNil();

	else if (strEquals(sProperty, PROPERTY_MAX_HP))
		{
		//	Nil means that we don't want to make a change

		if (pValue->IsNil())
			return true;

		m_iMaxHitPoints = Max(0, pValue->GetIntegerValue());
		m_iHitPoints = Min(m_iHitPoints, m_iMaxHitPoints);
		}
	else if (strEquals(sProperty, PROPERTY_MAX_STRUCTURAL_HP))
		{
		//	Nil means that we don't want to make a change

		if (pValue->IsNil())
			return true;

		m_iMaxStructuralHP = Max(0, pValue->GetIntegerValue());
		m_iStructuralHP = Min(m_iStructuralHP, m_iMaxStructuralHP);
		}
	else if (strEquals(sProperty, PROPERTY_STRUCTURAL_HP))
		{
		//	Nil means that we don't want to make a change

		if (pValue->IsNil())
			return true;

		m_iStructuralHP = Min(Max(0, pValue->GetIntegerValue()), m_iMaxStructuralHP);
		}
	else
		return false;

	return true;
	}

void CStationHull::UpdateExtended (const CStationHullDesc &Desc, const CTimeSpan &ExtraTime)

//	UpdateExtended
//
//	Repairs over a period of time.

	{
	Metric rRepairPer180;

	if (!IsAbandoned()
			&& m_iMaxHitPoints > 0
			&& m_iHitPoints < m_iMaxHitPoints
			&& (rRepairPer180 = Desc.GetRegenDesc().GetHPPer180(STATION_REPAIR_FREQUENCY)) > 0)
		{
		//	For each day elapsed, we repair based on the regen rate. This is
		//	much less than if we let the regen run for a day because we're 
		//	simulating real repairs (which take time) vs. ad hoc repairs which
		//	can be done quickly.

		m_iHitPoints = Min(m_iMaxHitPoints, m_iHitPoints + mathRound(rRepairPer180 * REGEN_PER_DAY_FACTOR));
		}
	}

bool CStationHull::UpdateRepairDamage (const CStationHullDesc &Desc, int iTick)

//	UpdateRepairDamage
//
//	Repairs damage based on regen settings. Returns FALSE if we do not regenerate
//	any damage.

	{
	if (Desc.GetRegenDesc().IsEmpty())
		return false;

	if (m_iHitPoints < m_iMaxHitPoints)
		m_iHitPoints = Min(m_iMaxHitPoints, m_iHitPoints + Desc.GetRegenDesc().GetRegen(iTick, STATION_REPAIR_FREQUENCY));

	return true;
	}

void CStationHull::WriteToStream (IWriteStream &Stream, CStation *pStation)

//	WriteToStream
//
//	Write to a stream

	{
	DWORD dwFlags = 0;

	dwFlags |= (m_fImmutable ?		0x00000001 : 0);
	//	0x00000002 Unused at version 176
	dwFlags |= (m_fCannotBeHit ?	0x00000004 : 0);

	dwFlags |= (((DWORD)m_iType) << HULL_TYPES_SHIFT) & HULL_TYPES_MASK;

	Stream.Write(dwFlags);

	Stream.Write(m_iArmorLevel);
	Stream.Write(m_iHitPoints);
	Stream.Write(m_iMaxHitPoints);
	Stream.Write(m_iStructuralHP);
	Stream.Write(m_iMaxStructuralHP);
	}

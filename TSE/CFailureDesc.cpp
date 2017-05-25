//	CFailureDesc.cpp
//
//	CFailureDesc class
//  Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define TAG_FAILURE								CONSTLIT("Failure")

#define FIELD_CHANCE							CONSTLIT("chance")
#define FIELD_TYPE								CONSTLIT("type")

static TStaticStringTable<TStaticStringEntry<CFailureDesc::EFailureTypes>, 7> FAILURE_TYPE_TABLE = {
	"custom",				CFailureDesc::failCustom,
	"explosion",			CFailureDesc::failExplosion,
	"heatDamage",			CFailureDesc::failHeatDamage,
	"jammed",				CFailureDesc::failJammed,
	"misfire",				CFailureDesc::failMisfire,
	"noFailure",			CFailureDesc::failNone,
	"safeMode",				CFailureDesc::failSafeMode,
	};

CFailureDesc::CFailureDesc (EProfile iProfile)

//	CFailureDesc constructor

	{
	if (iProfile != profileNone)
		InitFromDefaults(iProfile);
	}

CFailureDesc::EFailureTypes CFailureDesc::Failure (CSpaceObject *pSource, CInstalledDevice *pDevice) const

//	Failure
//
//	Rolls for a random failure and implements it. We return the failure mode.

	{
	if (m_FailTable.GetCount() == 0)
		return failNone;

	EFailureTypes iFailure = m_FailTable.GetAt(m_FailTable.RollPos());
	switch (iFailure)
		{
		case failNone:
			break;

		case failCustom:
			//	LATER: Call OnFailure
			break;

		case failExplosion:
			pSource->OnDeviceStatus(pDevice, CDeviceClass::failWeaponExplosion);
			break;

		case failHeatDamage:
			pSource->DamageItem(pDevice);
			pSource->OnDeviceStatus(pDevice, CDeviceClass::failDeviceOverheat);
			break;

		case failJammed:
			pSource->OnDeviceStatus(pDevice, CDeviceClass::failWeaponJammed);
			break;

		case failMisfire:
			pSource->OnDeviceStatus(pDevice, CDeviceClass::failWeaponMisfire);
			break;

		case failSafeMode:
			pSource->DisableDevice(pDevice);
			pSource->OnDeviceStatus(pDevice, CDeviceClass::failDeviceDisabledByOverheat);
			break;

		default:
			return failNone;
		}

	return iFailure;
	}

ALERROR CFailureDesc::InitFromDefaults (EProfile iDefault)

//	InitFromDefault
//
//	Initializes to a default profile

	{
	switch (iDefault)
		{
		case profileWeaponFailure:
			m_FailTable.Insert(failNone, 10);
			m_FailTable.Insert(failJammed, 60);
			m_FailTable.Insert(failMisfire, 20);
			m_FailTable.Insert(failExplosion, 10);
			break;

		case profileWeaponOverheat:
			m_FailTable.Insert(failNone, 25);
			m_FailTable.Insert(failJammed, 25);
			m_FailTable.Insert(failSafeMode, 25);
			m_FailTable.Insert(failHeatDamage, 25);
			break;

		default:
			ASSERT(false);
			return ERR_FAIL;
		}

	return NOERROR;
	}

ALERROR CFailureDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, EProfile iDefault)

//	InitFromXML
//
//	Initializes from an XML element

	{
	int i;

	//	If we have no XML element, then we come up with defaults based on the 
	//	profile.

	if (pDesc == NULL)
		{
		ASSERT(iDefault != profileNone);
		if (iDefault == profileNone)
			return NOERROR;

		return InitFromDefaults(iDefault);
		}

	//	Loop over all failure modes

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pItem = pDesc->GetContentElement(i);
		if (strEquals(pItem->GetTag(), TAG_FAILURE))
			{
			CString sType = pItem->GetAttribute(FIELD_TYPE);
			EFailureTypes iType = ParseFailureType(sType);
			if (iType == failError)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Unknown failure type: %s."), sType);
				return ERR_FAIL;
				}

			int iChance = pItem->GetAttributeIntegerBounded(FIELD_CHANCE, 0, -1, 10);
			if (iChance > 0)
				m_FailTable.Insert(iType, iChance);
			}
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown item tag: %s."), pItem->GetTag());
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}

CFailureDesc::EFailureTypes CFailureDesc::ParseFailureType (const CString &sValue)

//	ParseFailureTypes
//
//	Convert from a string to a failure type. We return failError if we don't 
//	find a match.

	{
	const TStaticStringEntry<CFailureDesc::EFailureTypes> *pEntry = FAILURE_TYPE_TABLE.GetAt(sValue);
	if (pEntry == NULL)
		return failError;

	return pEntry->Value;
	}

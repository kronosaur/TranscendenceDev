//	COverlayType.cpp
//
//	COverlayType class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define COUNTER_TAG								CONSTLIT("Counter")
#define EFFECT_WHEN_HIT_TAG						CONSTLIT("EffectWhenHit")
#define EFFECT_TAG								CONSTLIT("Effect")
#define ENHANCE_ABILITIES_TAG					CONSTLIT("EnhancementAbilities")
#define HIT_EFFECT_TAG							CONSTLIT("HitEffect")
#define SHIP_ENERGY_FIELD_TYPE_TAG				CONSTLIT("ShipEnergyFieldType")
#define UNDERGROUND_TAG							CONSTLIT("Underground")

#define ABSORB_ADJ_ATTRIB						CONSTLIT("absorbAdj")
#define ALT_EFFECT_ATTRIB						CONSTLIT("altEffect")
#define DISABLE_SHIP_SCREEN_ATTRIB				CONSTLIT("disableShipScreen")
#define DISARM_ATTRIB							CONSTLIT("disarm")
#define DRAG_ATTRIB								CONSTLIT("drag")
#define IGNORE_SHIP_ROTATION_ATTRIB				CONSTLIT("ignoreSourceRotation")
#define PARALYZE_ATTRIB							CONSTLIT("paralyze")
#define ROTATE_WITH_SOURCE_ATTRIB				CONSTLIT("rotateWithSource")
#define SHIELD_OVERLAY_ATTRIB					CONSTLIT("shieldOverlay")
#define SPIN_ATTRIB								CONSTLIT("spin")
#define TIME_STOP_ATTRIB						CONSTLIT("timeStop")
#define UNID_ATTRIB								CONSTLIT("UNID")
#define BONUS_ADJ_ATTRIB						CONSTLIT("weaponBonusAdj")
#define WEAPON_SUPPRESS_ATTRIB					CONSTLIT("weaponSuppress")

#define FIELD_WEAPON_SUPPRESS					CONSTLIT("weaponSuppress")

#define ON_UPDATE_EVENT							CONSTLIT("OnUpdate")

#define SUPPRESS_ALL							CONSTLIT("*")

COverlayType::COverlayType (void)

//	COverlayType constructor

	{
	}

COverlayType::~COverlayType (void)

//	COverlayType destructor

	{
	if (m_pEffect)
		delete m_pEffect;

	if (m_pHitEffect)
		delete m_pHitEffect;
	}

bool COverlayType::AbsorbsWeaponFire (CInstalledDevice *pWeapon)

//	AbsorbsWeaponFire
//
//	Absorbs weapon fire from the ship

	{
	int iType = pWeapon->GetDamageType(CItemCtx(NULL, pWeapon));
	if (iType != -1 && m_WeaponSuppress.InSet(iType))
		return true;
	else
		return false;
	}

bool COverlayType::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Returns meta-data

	{
	int i;

	if (strEquals(sField, FIELD_WEAPON_SUPPRESS))
		{
		if (m_WeaponSuppress.IsEmpty())
			*retsValue = NULL_STR;
		else
			{
			*retsValue = CONSTLIT("=(");

			bool bNeedSeparator = false;
			for (i = 0; i < damageCount; i++)
				if (m_WeaponSuppress.InSet(i))
					{
					if (bNeedSeparator)
						retsValue->Append(CONSTLIT(" "));

					retsValue->Append(::GetDamageType((DamageTypes)i));
					bNeedSeparator = true;
					}

			retsValue->Append(CONSTLIT(")"));
			}
		}
	else
		return CDesignType::FindDataField(sField, retsValue);

	return true;
	}

int COverlayType::GetDamageAbsorbed (CSpaceObject *pSource, SDamageCtx &Ctx)

//	GetDamageAbsorbed
//
//	Returns the amount of damage absorbed

	{
	if (Ctx.Damage.GetDamageType() == damageGeneric)
		{
		//	For generic damage, we absorb the min of all other damage types

		int iMin = 100;
		for (int i = 0; i < damageCount; i++)
			if (m_AbsorbAdj.GetAbsorbAdj((DamageTypes)i) < iMin)
				iMin = m_AbsorbAdj.GetAbsorbAdj((DamageTypes)i);

		return (Ctx.iDamage * iMin) / 100;
		}

	return (Ctx.iDamage * m_AbsorbAdj.GetAbsorbAdj(Ctx.Damage.GetDamageType())) / 100;
	}

int COverlayType::GetMaxHitPoints (const CSpaceObject &Source) const

//	GetMaxHitPoints
//
//	Returns maximum hit points.

	{
	if (!m_Underground.IsEmpty())
		{
		const CSystem *pSystem = Source.GetSystem();
		int iLevel = (pSystem ? pSystem->GetLevel() : 1);
		return m_Underground.GetHitPoints(iLevel);
		}
	else
		return 0;
	}

int COverlayType::GetWeaponBonus (CInstalledDevice *pDevice, CSpaceObject *pSource)

//	GetWeaponBonus
//
//	Returns the bonus for this weapon

	{
	DamageTypes iType = (DamageTypes)pDevice->GetDamageType(CItemCtx(pSource, pDevice));
	if (iType == damageGeneric)
		return 0;

	return m_BonusAdj.GetHPBonus(iType);
	}

void COverlayType::OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	OnAddTypesUsed
//
//	Add types used to the list.

	{
	//	LATER: m_pEffect and m_pHitEffect are always local; we should allow
	//	them to be an design type.
	}

ALERROR COverlayType::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design

	{
	ALERROR error;

	if (error = m_Enhancements.Bind(Ctx))
		return error;

	if (m_pEffect)
		if (error = m_pEffect->BindDesign(Ctx))
			return error;

	if (m_pHitEffect)
		if (error = m_pHitEffect->BindDesign(Ctx))
			return error;

	return NOERROR;
	}

ALERROR COverlayType::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML

	{
	ALERROR error;

	//	Effect

	CXMLElement *pEffect = pDesc->GetContentElementByTag(EFFECT_TAG);
	if (pEffect)
		{
		if (error = CEffectCreator::CreateFromXML(Ctx, 
				pEffect, 
				strPatternSubst(CONSTLIT("%d:e"), GetUNID()), 
				&m_pEffect))
			return ComposeLoadError(Ctx, CONSTLIT("Unable to load effect."));
		}

	pEffect = pDesc->GetContentElementByTag(HIT_EFFECT_TAG);
	if (pEffect == NULL)
		pEffect = pDesc->GetContentElementByTag(EFFECT_WHEN_HIT_TAG);

	if (pEffect)
		{
		if (error = CEffectCreator::CreateFromXML(Ctx, 
				pEffect, 
				strPatternSubst(CONSTLIT("%d:h"), GetUNID()), 
				&m_pHitEffect))
			return ComposeLoadError(Ctx, CONSTLIT("Unable to load hit effect."));

		//	For compatibility with previous versions, if we're using the old
		//	<ShipEnergyFieldType> then altEffect defaults to TRUE. Otherwise, for new
		//	<OverlayType> altEffect defaults to false.

		bool bAltEffect;
		if (pEffect->FindAttributeBool(ALT_EFFECT_ATTRIB, &bAltEffect))
			m_fAltHitEffect = bAltEffect;
		else
			m_fAltHitEffect = strEquals(pDesc->GetTag(), SHIP_ENERGY_FIELD_TYPE_TAG);
		}
	else
		m_fAltHitEffect = false;

	//	Rotation

	m_fRotateWithShip = !pDesc->GetAttributeBool(IGNORE_SHIP_ROTATION_ATTRIB);
	m_fRotateWithSource = pDesc->GetAttributeBool(ROTATE_WITH_SOURCE_ATTRIB);

	//	Enhancements conferred

	CXMLElement *pEnhanceList = pDesc->GetContentElementByTag(ENHANCE_ABILITIES_TAG);
	if (pEnhanceList)
		{
		if (error = m_Enhancements.InitFromXML(Ctx, pEnhanceList, NULL))
			return error;
		}

	//	Damage adjustment

	if (error = m_AbsorbAdj.InitFromDamageAdj(Ctx, pDesc->GetAttribute(ABSORB_ADJ_ATTRIB), false))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Bonus adjustment

	if (error = m_BonusAdj.InitFromHPBonus(Ctx, pDesc->GetAttribute(BONUS_ADJ_ATTRIB)))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Load the weapon suppress

	if (error = m_WeaponSuppress.InitFromXML(pDesc->GetAttribute(WEAPON_SUPPRESS_ATTRIB)))
		return ComposeLoadError(Ctx, CONSTLIT("Unable to load weapon suppress attribute"));

	//	Are we a field/shield overlay (or part of hull)?
	//	By default, we are a shield overlay if we absorb damage.

	bool bValue;
	if (pDesc->FindAttributeBool(SHIELD_OVERLAY_ATTRIB, &bValue))
		m_fShieldOverlay = bValue;
	else
		m_fShieldOverlay = !m_AbsorbAdj.IsEmpty();

	//	Counter

	if (const CXMLElement *pCounter = pDesc->GetContentElementByTag(COUNTER_TAG))
		{
		if (ALERROR error = m_Counter.InitFromXML(Ctx, *pCounter))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Underground

	if (const CXMLElement *pUnderground = pDesc->GetContentElementByTag(UNDERGROUND_TAG))
		{
		if (ALERROR error = m_Underground.InitFromXML(Ctx, *pUnderground))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Options

	m_fDisarmShip = pDesc->GetAttributeBool(DISARM_ATTRIB);
	m_fParalyzeShip = pDesc->GetAttributeBool(PARALYZE_ATTRIB);
	m_fDisableShipScreen = pDesc->GetAttributeBool(DISABLE_SHIP_SCREEN_ATTRIB);
	m_fSpinShip = pDesc->GetAttributeBool(SPIN_ATTRIB);
	m_fTimeStop = pDesc->GetAttributeBool(TIME_STOP_ATTRIB);

	int iDrag;
	if (pDesc->FindAttributeInteger(DRAG_ATTRIB, &iDrag))
		m_rDrag = Min(Max(0, iDrag), 100) / 100.0;
	else
		m_rDrag = 1.0;

	//	Done

	return NOERROR;
	}

CEffectCreator *COverlayType::OnFindEffectCreator (const CString &sUNID)

//	OnFindEffectCreator
//
//	Load the proper effect creator

	{
	char *pPos = sUNID.GetASCIIZPointer();
	if (*pPos != ':')
		return NULL;

	pPos++;

	switch (*pPos)
		{
		case 'e':
			return m_pEffect;

		case 'h':
			return m_pHitEffect;

		default:
			return NULL;
		}
	}

bool COverlayType::RotatesWithSource (const CSpaceObject &Source) const

//	RotatesWithSource
//
//	Returns TRUE if we rotate along with the source.

	{
	if (Source.GetCategory() == CSpaceObject::catShip)
		return (m_fRotateWithShip || m_fRotateWithSource);
	else
		return m_fRotateWithSource;
	}

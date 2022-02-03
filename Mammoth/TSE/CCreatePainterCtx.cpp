//	CCreatePainterCtx.cpp
//
//	CCreatePainterCtx class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_ANCHOR_OBJ				CONSTLIT("anchorObj")
#define FIELD_ATTACKER					CONSTLIT("attacker")
#define FIELD_ARMOR_SEG					CONSTLIT("armorSeg")
#define FIELD_AVERAGE_DAMAGE_HP			CONSTLIT("averageDamageHP")
#define FIELD_CAUSE						CONSTLIT("cause")
#define FIELD_CHARGE_TIME				CONSTLIT("chargeTime")
#define FIELD_DAMAGE_HP					CONSTLIT("damageHP")
#define FIELD_DAMAGE_PER_PROJECTILE		CONSTLIT("damagePerProjectile")
#define FIELD_DAMAGE_TYPE				CONSTLIT("damageType")
#define FIELD_FIRE_CHARGE				CONSTLIT("fireCharge")
#define FIELD_FIRE_REPEAT				CONSTLIT("fireRepeat")
#define FIELD_HIT_DIR					CONSTLIT("hitDir")
#define FIELD_HIT_POS					CONSTLIT("hitPos")
#define FIELD_OBJ_HIT					CONSTLIT("objHit")
#define FIELD_ORDER_GIVER				CONSTLIT("orderGiver")
#define FIELD_OVERLAY_TYPE				CONSTLIT("overlayType")
#define FIELD_PARTICLE_COUNT			CONSTLIT("particleCount")
#define FIELD_REPEATING					CONSTLIT("repeating")
#define FIELD_SPEED						CONSTLIT("speed")
#define FIELD_WEAPON_UNID				CONSTLIT("weaponUNID")

void CCreatePainterCtx::AddDataInteger (const CString &sField, int iValue)

//	AddDataInteger
//
//	Adds data

	{
	SDataEntry *pEntry = m_Data.Insert();
	pEntry->sField = sField;
	pEntry->iValue = iValue;
	}

ICCItem *CCreatePainterCtx::GetData (void)

//	GetData
//
//	Generate data block for create painter

	{
	int i;

	if (m_pData)
		return m_pData;

	//	Initialize

	m_pData = ICCItemPtr(ICCItem::SymbolTable);

	//	Add data

	for (i = 0; i < m_Data.GetCount(); i++)
		m_pData->SetIntegerAt(m_Data[i].sField, m_Data[i].iValue);

	if (m_pAnchor)
		m_pData->SetIntegerAt(FIELD_ANCHOR_OBJ, (int)m_pAnchor);

	//	Set values depending on what we have in context

	if (m_pDamageCtx)
		SetDamageCtxData(m_pData, *m_pDamageCtx);

	//	NOTE: If we have a damage context, we don't set weaponfire data because
	//	it might overwrite it.

	else if (m_pWeaponFireDesc)
		SetWeaponFireDescData(m_pData, *m_pWeaponFireDesc);

	//	Set data for overlays

	else if (m_pOverlay)
		SetOverlayData(m_pData, *m_pOverlay);

	//	Done

	return m_pData;
	}

void CCreatePainterCtx::SetDamageCtxData (ICCItem *pTable, SDamageCtx &DamageCtx) const

//	SetDamageCtxData
//
//	Sets the data from a damage context to the data block

	{
	pTable->SetIntegerAt(FIELD_OBJ_HIT, (int)DamageCtx.pObj);
	pTable->SetIntegerAt(FIELD_ARMOR_SEG, DamageCtx.iSectHit);
	if (DamageCtx.pCause)
		pTable->SetIntegerAt(FIELD_CAUSE, (int)DamageCtx.pCause);

	CSpaceObject *pAttacker = DamageCtx.Attacker.GetObj();
	if (pAttacker)
		pTable->SetIntegerAt(FIELD_ATTACKER, (int)pAttacker);

	CSpaceObject *pOrderGiver = DamageCtx.GetOrderGiver();
	if (pOrderGiver)
		pTable->SetIntegerAt(FIELD_ORDER_GIVER, (int)pAttacker);

	ICCItemPtr pHitPos(CreateListFromVector(DamageCtx.vHitPos));
	pTable->SetAt(FIELD_HIT_POS, pHitPos);

	pTable->SetIntegerAt(FIELD_HIT_DIR, DamageCtx.iDirection);
	pTable->SetIntegerAt(FIELD_DAMAGE_HP, DamageCtx.iDamage);
	pTable->SetIntegerAt(FIELD_AVERAGE_DAMAGE_HP, mathRound(DamageCtx.GetDesc().GetAveDamage()));
	pTable->SetStringAt(FIELD_DAMAGE_TYPE, GetDamageShortName(DamageCtx.Damage.GetDamageType()));

	CItemType *pWeapon = DamageCtx.GetDesc().GetWeaponType();
	DWORD dwWeaponUNID = (pWeapon ? pWeapon->GetUNID() : 0);
	pTable->SetIntegerAt(FIELD_WEAPON_UNID, dwWeaponUNID);
	}

void CCreatePainterCtx::SetDefaultParam (const CString &sParam, const CEffectParamDesc &Value)

//	SetDefaultParam
//
//	Sets a default parameter for the painter

	{
	if (!m_pDefaultParams)
		m_pDefaultParams = TUniquePtr<CEffectParamSet>(new CEffectParamSet);

	m_pDefaultParams->AddParam(sParam, Value);
	}

void CCreatePainterCtx::SetOverlayData (ICCItem *pTable, const COverlay &Overlay) const

//	SetOverlayData
//
//	Sets data associated with an overlay.

	{
	pTable->SetIntegerAt(FIELD_OVERLAY_TYPE, (int)Overlay.GetType()->GetUNID());
	}

void CCreatePainterCtx::SetWeaponFireDesc (const CWeaponFireDesc *pDesc)

//	SetWeaponFireDesc
//
//	Associates a weapon fire descriptor.

	{
	m_pWeaponFireDesc = pDesc;

	//	While we're at it, set the API version.

	CItemType *pType;
	if (m_pWeaponFireDesc && (pType = m_pWeaponFireDesc->GetWeaponType()))
		m_dwAPIVersion = pType->GetAPIVersion();
	}

void CCreatePainterCtx::SetWeaponFireDescData (ICCItem *pTable, const CWeaponFireDesc &Desc) const

//	SetWeaponFireDescData
//
//	Sets the data from a weapon fire desc to the data block.

	{
	pTable->SetIntegerAt(FIELD_CHARGE_TIME, mathRound(Desc.GetChargeTime()));
	pTable->SetIntegerAt(FIELD_DAMAGE_HP, mathRound(Desc.GetAveDamage()));
	pTable->SetStringAt(FIELD_DAMAGE_TYPE, GetDamageShortName(Desc.GetDamageType()));
	pTable->SetIntegerAt(FIELD_FIRE_CHARGE, m_iFireCharge);
	pTable->SetIntegerAt(FIELD_FIRE_REPEAT, m_iFireRepeat);
	pTable->SetIntegerAt(FIELD_REPEATING, mathRound(Desc.GetContinuous()));
	pTable->SetIntegerAt(FIELD_SPEED, mathRound(100.0 * Desc.GetAveInitialSpeed() / LIGHT_SPEED));
	if (Desc.GetType() == CWeaponFireDesc::ftParticles)
		pTable->SetIntegerAt(FIELD_PARTICLE_COUNT, mathRound(Desc.GetAveParticleCount()));

	CItemType *pWeapon = Desc.GetWeaponType();
	DWORD dwWeaponUNID = (pWeapon ? pWeapon->GetUNID() : 0);
	pTable->SetIntegerAt(FIELD_WEAPON_UNID, dwWeaponUNID);
	}

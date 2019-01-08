//	CCreatePainterCtx.cpp
//
//	CCreatePainterCtx class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_ATTACKER					CONSTLIT("attacker")
#define FIELD_ARMOR_SEG					CONSTLIT("armorSeg")
#define FIELD_AVERAGE_DAMAGE_HP			CONSTLIT("averageDamageHP")
#define FIELD_CAUSE						CONSTLIT("cause")
#define FIELD_DAMAGE_HP					CONSTLIT("damageHP")
#define FIELD_DAMAGE_PER_PROJECTILE		CONSTLIT("damagePerProjectile")
#define FIELD_DAMAGE_TYPE				CONSTLIT("damageType")
#define FIELD_HIT_DIR					CONSTLIT("hitDir")
#define FIELD_HIT_POS					CONSTLIT("hitPos")
#define FIELD_OBJ_HIT					CONSTLIT("objHit")
#define FIELD_ORDER_GIVER				CONSTLIT("orderGiver")
#define FIELD_PARTICLE_COUNT			CONSTLIT("particleCount")
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

	CCodeChain &CC = g_pUniverse->GetCC();
	m_pData = ICCItemPtr(CC.CreateSymbolTable());

	//	Add data

	for (i = 0; i < m_Data.GetCount(); i++)
		m_pData->SetIntegerAt(CC, m_Data[i].sField, m_Data[i].iValue);

	//	Set values depending on what we have in context

	if (m_pDamageCtx)
		SetDamageCtxData(CC, m_pData, *m_pDamageCtx);

	//	NOTE: If we have a damage context, we don't set weaponfire data because
	//	it might overwrite it.

	else if (m_pWeaponFireDesc)
		SetWeaponFireDescData(CC, m_pData, m_pWeaponFireDesc);

	//	Done

	return m_pData;
	}

void CCreatePainterCtx::SetDamageCtxData (CCodeChain &CC, ICCItem *pTable, SDamageCtx &DamageCtx) const

//	SetDamageCtxData
//
//	Sets the data from a damage context to the data block

	{
	pTable->SetIntegerAt(CC, FIELD_OBJ_HIT, (int)DamageCtx.pObj);
	pTable->SetIntegerAt(CC, FIELD_ARMOR_SEG, DamageCtx.iSectHit);
	if (DamageCtx.pCause)
		pTable->SetIntegerAt(CC, FIELD_CAUSE, (int)DamageCtx.pCause);

	CSpaceObject *pAttacker = DamageCtx.Attacker.GetObj();
	if (pAttacker)
		pTable->SetIntegerAt(CC, FIELD_ATTACKER, (int)pAttacker);

	CSpaceObject *pOrderGiver = DamageCtx.GetOrderGiver();
	if (pOrderGiver)
		pTable->SetIntegerAt(CC, FIELD_ORDER_GIVER, (int)pAttacker);

	ICCItemPtr pHitPos(CreateListFromVector(CC, DamageCtx.vHitPos));
	pTable->SetAt(CC, FIELD_HIT_POS, pHitPos);

	pTable->SetIntegerAt(CC, FIELD_HIT_DIR, DamageCtx.iDirection);
	pTable->SetIntegerAt(CC, FIELD_DAMAGE_HP, DamageCtx.iDamage);
	pTable->SetIntegerAt(CC, FIELD_AVERAGE_DAMAGE_HP, (int)(DamageCtx.pDesc->GetAveDamage() + 0.5));
	pTable->SetStringAt(CC, FIELD_DAMAGE_TYPE, GetDamageShortName(DamageCtx.Damage.GetDamageType()));

	CItemType *pWeapon = DamageCtx.pDesc->GetWeaponType();
	DWORD dwWeaponUNID = (pWeapon ? pWeapon->GetUNID() : 0);
	pTable->SetIntegerAt(CC, FIELD_WEAPON_UNID, dwWeaponUNID);
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

void CCreatePainterCtx::SetWeaponFireDesc (CWeaponFireDesc *pDesc)

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

void CCreatePainterCtx::SetWeaponFireDescData (CCodeChain &CC, ICCItem *pTable, CWeaponFireDesc *pDesc) const

//	SetWeaponFireDescData
//
//	Sets the data from a weapon fire desc to the data block.

	{
	pTable->SetIntegerAt(CC, FIELD_DAMAGE_HP, (int)(pDesc->GetAveDamage() + 0.5));
	pTable->SetStringAt(CC, FIELD_DAMAGE_TYPE, GetDamageShortName(pDesc->GetDamageType()));
	pTable->SetIntegerAt(CC, FIELD_SPEED, (int)((100.0 * pDesc->GetAveInitialSpeed() / LIGHT_SPEED) + 0.5));
	if (pDesc->GetType() == CWeaponFireDesc::ftParticles)
		pTable->SetIntegerAt(CC, FIELD_PARTICLE_COUNT, (int)(pDesc->GetAveParticleCount() + 0.5));

	CItemType *pWeapon = pDesc->GetWeaponType();
	DWORD dwWeaponUNID = (pWeapon ? pWeapon->GetUNID() : 0);
	pTable->SetIntegerAt(CC, FIELD_WEAPON_UNID, dwWeaponUNID);
	}

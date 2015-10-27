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
#define FIELD_DAMAGE_TYPE				CONSTLIT("damageType")
#define FIELD_HIT_DIR					CONSTLIT("hitDir")
#define FIELD_HIT_POS					CONSTLIT("hitPos")
#define FIELD_OBJ_HIT					CONSTLIT("objHit")
#define FIELD_ORDER_GIVER				CONSTLIT("orderGiver")
#define FIELD_SPEED						CONSTLIT("speed")
#define FIELD_WEAPON_UNID				CONSTLIT("weaponUNID")

CCreatePainterCtx::~CCreatePainterCtx (void)

//	CCreatePainterCtx destructor

	{
	if (m_pData)
		m_pData->Discard(&g_pUniverse->GetCC());

	if (m_pDefaultParams)
		delete m_pDefaultParams;
	}

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
	m_pData = CC.CreateSymbolTable();
	CCSymbolTable *pTable = (CCSymbolTable *)m_pData;

	//	Add data

	for (i = 0; i < m_Data.GetCount(); i++)
		pTable->SetIntegerValue(CC, m_Data[i].sField, m_Data[i].iValue);

	//	Set values depending on what we have in context

	if (m_pDamageCtx)
		SetDamageCtxData(CC, pTable, *m_pDamageCtx);

	//	NOTE: If we have a damage context, we don't set weaponfire data because
	//	it might overwrite it.

	else if (m_pWeaponFireDesc)
		SetWeaponFireDescData(CC, pTable, m_pWeaponFireDesc);

	//	Done

	return m_pData;
	}

void CCreatePainterCtx::SetDamageCtxData (CCodeChain &CC, CCSymbolTable *pTable, SDamageCtx &DamageCtx)

//	SetDamageCtxData
//
//	Sets the data from a damage context to the data block

	{
	pTable->SetIntegerValue(CC, FIELD_OBJ_HIT, (int)DamageCtx.pObj);
	pTable->SetIntegerValue(CC, FIELD_ARMOR_SEG, DamageCtx.iSectHit);
	if (DamageCtx.pCause)
		pTable->SetIntegerValue(CC, FIELD_CAUSE, (int)DamageCtx.pCause);

	CSpaceObject *pAttacker = DamageCtx.Attacker.GetObj();
	if (pAttacker)
		pTable->SetIntegerValue(CC, FIELD_ATTACKER, (int)pAttacker);

	CSpaceObject *pOrderGiver = (pAttacker ? pAttacker->GetOrderGiver(DamageCtx.Attacker.GetCause()) : NULL);
	if (pOrderGiver)
		pTable->SetIntegerValue(CC, FIELD_ORDER_GIVER, (int)pAttacker);

	ICCItem *pHitPos = CreateListFromVector(CC, DamageCtx.vHitPos);
	pTable->SetValue(CC, FIELD_HIT_POS, pHitPos);
	pHitPos->Discard(&CC);

	pTable->SetIntegerValue(CC, FIELD_HIT_DIR, DamageCtx.iDirection);
	pTable->SetIntegerValue(CC, FIELD_DAMAGE_HP, DamageCtx.iDamage);
	pTable->SetIntegerValue(CC, FIELD_AVERAGE_DAMAGE_HP, (int)(DamageCtx.pDesc->GetAveDamage() + 0.5));
	pTable->SetStringValue(CC, FIELD_DAMAGE_TYPE, GetDamageShortName(DamageCtx.Damage.GetDamageType()));

	CItemType *pWeapon = DamageCtx.pDesc->GetWeaponType();
	DWORD dwWeaponUNID = (pWeapon ? pWeapon->GetUNID() : 0);
	pTable->SetIntegerValue(CC, FIELD_WEAPON_UNID, dwWeaponUNID);
	}

void CCreatePainterCtx::SetDefaultParam (const CString &sParam, const CEffectParamDesc &Value)

//	SetDefaultParam
//
//	Sets a default parameter for the painter

	{
	if (m_pDefaultParams == NULL)
		m_pDefaultParams = new CEffectParamSet;

	m_pDefaultParams->AddParam(sParam, Value);
	}

void CCreatePainterCtx::SetWeaponFireDescData (CCodeChain &CC, CCSymbolTable *pTable, CWeaponFireDesc *pDesc)

//	SetWeaponFireDescData
//
//	Sets the data from a weapon fire desc to the data block.

	{
	pTable->SetIntegerValue(CC, FIELD_DAMAGE_HP, (int)(pDesc->GetAveDamage() + 0.5));
	pTable->SetIntegerValue(CC, FIELD_SPEED, (int)((100.0 * pDesc->GetAveInitialSpeed() / LIGHT_SPEED) + 0.5));

	CItemType *pWeapon = pDesc->GetWeaponType();
	DWORD dwWeaponUNID = (pWeapon ? pWeapon->GetUNID() : 0);
	pTable->SetIntegerValue(CC, FIELD_WEAPON_UNID, dwWeaponUNID);
	}

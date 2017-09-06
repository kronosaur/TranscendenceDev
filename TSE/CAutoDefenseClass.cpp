//	CAutoDefenseClass.cpp
//
//	CAutoDefenseClass class
//	Copyright (c) 2004 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

#define RECHARGE_TIME_ATTRIB					CONSTLIT("rechargeTime")
#define FIRE_RATE_ATTRIB						CONSTLIT("fireRate")
#define INTERCEPT_RANGE_ATTRIB					CONSTLIT("interceptRange")
#define TARGET_ATTRIB							CONSTLIT("target")
#define TARGET_CRITERIA_ATTRIB					CONSTLIT("targetCriteria")
#define WEAPON_ATTRIB							CONSTLIT("weapon")

#define MISSILES_TARGET							CONSTLIT("missiles")

#define PROPERTY_FIRE_DELAY						CONSTLIT("fireDelay")
#define PROPERTY_FIRE_RATE						CONSTLIT("fireRate")

const int DEFAULT_INTERCEPT_RANGE =				10;

CAutoDefenseClass::CAutoDefenseClass (void)

//	CAutoDefenseClass constructor

	{
	}

int CAutoDefenseClass::GetActivateDelay (CInstalledDevice *pDevice, CSpaceObject *pSource) const

//	GetActivateDelay
//
//	Returns the activation delay

	{
	return m_iRechargeTicks;
	}

int CAutoDefenseClass::CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource)

//	CalcPowerUsed
//
//	Return power used by device

	{
	CDeviceClass *pWeapon = GetWeapon();

	if (pWeapon == NULL || !pDevice->IsEnabled())
		return 0;

	return pWeapon->CalcPowerUsed(Ctx, pDevice, pSource);
	}

DamageTypes CAutoDefenseClass::GetDamageType (CItemCtx &Ctx, const CItem &Ammo) const

//	GetDamageType
//
//	Returns the type of damage done by this device

	{
	CDeviceClass *pWeapon = GetWeapon();

	if (pWeapon)
		return pWeapon->GetDamageType(Ctx, Ammo);
	else
		return damageGeneric;
	}

ICCItem *CAutoDefenseClass::FindItemProperty (CItemCtx &Ctx, const CString &sProperty)

//	FindItemProperty
//
//	Returns the item property. Subclasses should call this if they do not
//	understand the property.

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	CDeviceClass *pWeapon;

	//	Get the property

	if (strEquals(sProperty, PROPERTY_FIRE_DELAY))
		return CC.CreateInteger(m_iRechargeTicks);

	else if (strEquals(sProperty, PROPERTY_FIRE_RATE))
		{
		Metric rDelay = m_iRechargeTicks;
		if (rDelay <= 0.0)
			return CC.CreateNil();

		return CC.CreateInteger((int)(1000.0 / rDelay));
		}

	//	Otherwise, just get the property from the weapon we're using

	else if (pWeapon = GetWeapon())
		{
		CItem Weapon(pWeapon->GetItemType(), 1);
		CItemCtx WeaponCtx(Weapon);

		return pWeapon->FindItemProperty(WeaponCtx, sProperty);
		}
	else
		return CDeviceClass::FindItemProperty(Ctx, sProperty);
	}

int CAutoDefenseClass::GetPowerRating (CItemCtx &Ctx) const

//	GetPowerRating
//
//	Returns the minimum reactor power needed for this device

	{
	CDeviceClass *pWeapon = GetWeapon();

	if (pWeapon)
		return pWeapon->GetPowerRating(Ctx);
	else
		return 0;
	}

bool CAutoDefenseClass::GetReferenceDamageType (CItemCtx &Ctx, const CItem &Ammo, DamageTypes *retiDamage, CString *retsReference) const

//	GetReferenceDamageType
//
//	Returns the damage type done by the weapon

	{
	CDeviceClass *pWeapon = GetWeapon();
	
	if (pWeapon)
		return pWeapon->GetReferenceDamageType(Ctx, Ammo, retiDamage, retsReference);
	else
		return false;
	}

void CAutoDefenseClass::OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	OnAddTypesUsed
//
//	Adds types used by this class

	{
	retTypesUsed->SetAt(m_pWeapon.GetUNID(), true);
	}

CString CAutoDefenseClass::OnGetReference (CItemCtx &Ctx, const CItem &Ammo, DWORD dwFlags)

//	OnGetReference
//
//	Returns a string that describes the basic attributes
//	of this weapon.

	{
	CDeviceClass *pWeapon = GetWeapon();
	
	if (pWeapon)
		return pWeapon->OnGetReference(Ctx, Ammo);
	else
		return NULL_STR;
	}

void CAutoDefenseClass::Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx)

//	Update
//
//	Update device

	{
	DEBUG_TRY

	CDeviceClass *pWeapon = GetWeapon();
	if (pWeapon == NULL)
		return;

	//	Update the weapon

	pWeapon->Update(pDevice, pSource, Ctx);

	//	Look for a target and fire

	if (pDevice->IsReady() && pDevice->IsEnabled())
		{
		int i;

		//	Look for a target 

		CSpaceObject *pBestTarget = NULL;
		CSystem *pSystem = pSource->GetSystem();
		CVector vSourcePos = pDevice->GetPos(pSource);

		//	Use the appropriate targeting method

		switch (m_iTargeting)
			{
			//	Hard-code search for the nearest missile

			case trgMissiles:
				{
				Metric rBestDist2 = m_rInterceptRange * m_rInterceptRange;

				for (i = 0; i < pSystem->GetObjectCount(); i++)
					{
					CSpaceObject *pObj = pSystem->GetObject(i);

					if (pObj
							&& pObj->GetCategory() == CSpaceObject::catMissile
							&& !pObj->GetDamageSource().IsEqual(pSource)
							&& !pObj->IsIntangible()
							&& pSource->IsEnemy(pObj->GetDamageSource()))
						{
						CVector vRange = pObj->GetPos() - vSourcePos;
						Metric rDistance2 = vRange.Dot(vRange);

						if (rDistance2 < rBestDist2)
							{
							pBestTarget = pObj;
							rBestDist2 = rDistance2;
							}
						}
					}

				break;
				}

			//	Look for an object by criteria

			case trgCriteria:
				{
				//	First we set the source

				CSpaceObject::SetCriteriaSource(m_TargetCriteria, pSource);

				//	Compute the range

				Metric rBestDist2;
				if (m_TargetCriteria.rMaxRadius < g_InfiniteDistance)
					rBestDist2 = (m_TargetCriteria.rMaxRadius * m_TargetCriteria.rMaxRadius);
				else
					rBestDist2 = m_rInterceptRange * m_rInterceptRange;

				//	Now look for the nearest object

				CSpaceObject::SCriteriaMatchCtx Ctx(m_TargetCriteria);
				for (i = 0; i < pSystem->GetObjectCount(); i++)
					{
					CSpaceObject *pObj = pSystem->GetObject(i);
					Metric rDistance2;

					if (pObj
							&& (m_TargetCriteria.dwCategories & pObj->GetCategory())
							&& ((rDistance2 = (pObj->GetPos() - vSourcePos).Length2()) < rBestDist2)
							&& pObj->MatchesCriteria(Ctx, m_TargetCriteria)
							&& !pObj->IsIntangible()
							&& pObj != pSource)
						{
						pBestTarget = pObj;
						rBestDist2 = rDistance2;
						}
					}

				break;
				}
			}

		//	If we found a target, try to shoot at it

		if (pBestTarget)
			{
			int iFireAngle = pWeapon->CalcFireSolution(pDevice, pSource, pBestTarget);
			if (iFireAngle != -1)
				{
				//	Since we're using this as a target, set the destroy notify flag
				//	(Normally beams don't notify, so we need to override this).

				pBestTarget->SetDestructionNotify();

				//	Fire

				pDevice->SetFireAngle(iFireAngle);
				pWeapon->Activate(pDevice, pSource, pBestTarget, &Ctx.bSourceDestroyed, &Ctx.bConsumedItems);
				pDevice->SetTimeUntilReady(m_iRechargeTicks);

				//	Identify

				if (pSource->IsPlayer())
					GetItemType()->SetKnown();
				}
			}
		}

	DEBUG_CATCH
	}

ALERROR CAutoDefenseClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice)

//	CreateFromXML
//
//	Load the class

	{
	ALERROR error;
	CAutoDefenseClass *pDevice;

	pDevice = new CAutoDefenseClass;
	if (pDevice == NULL)
		return ERR_MEMORY;

	if (error = pDevice->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

	int iFireRate = pDesc->GetAttributeInteger(FIRE_RATE_ATTRIB);
	if (iFireRate == 0)
		iFireRate = pDesc->GetAttributeInteger(RECHARGE_TIME_ATTRIB);
	if (iFireRate == 0)
		iFireRate = 15;

	pDevice->m_iRechargeTicks = (int)((iFireRate / STD_SECONDS_PER_UPDATE) + 0.5);
	if (error = pDevice->m_pWeapon.LoadUNID(Ctx, pDesc->GetAttribute(WEAPON_ATTRIB)))
		return error;

	//	Targeting. If we have a targetCriteria attribute then we use
	//	this to pick a target

	CString sCriteria;
	if (pDesc->FindAttribute(TARGET_CRITERIA_ATTRIB, &sCriteria))
		{
		pDevice->m_iTargeting = trgCriteria;

		//	We need to parse the criteria. But note that we have to parse
		//	it in an object-generic way, since one class could be used by
		//	multiple objects.

		CSpaceObject::ParseCriteria(NULL, sCriteria, &pDevice->m_TargetCriteria);
		}

	//	Otherwise, we use a hard-coded method

	else
		{
		CString sTarget = pDesc->GetAttribute(TARGET_ATTRIB);
		if (strEquals(sTarget, MISSILES_TARGET))
			pDevice->m_iTargeting = trgMissiles;
		else
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid target type: %s. Use target=\"missiles\" or targetCriteria=\"{criteria}\"."), sTarget);
			return ERR_FAIL;
			}
		}

	//	Intercept range

	int iInterceptRange = pDesc->GetAttributeIntegerBounded(INTERCEPT_RANGE_ATTRIB, 0, -1, DEFAULT_INTERCEPT_RANGE);
	pDevice->m_rInterceptRange = (Metric)(iInterceptRange * LIGHT_SECOND);

	//	Done

	*retpDevice = pDevice;

	return NOERROR;
	}

ALERROR CAutoDefenseClass::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	All design elements loaded

	{
	ALERROR error;

	if (error = m_pWeapon.Bind(Ctx))
		return error;

	return NOERROR;
	}

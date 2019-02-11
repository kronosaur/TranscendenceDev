//	CAutoDefenseClass.cpp
//
//	CAutoDefenseClass class
//	Copyright (c) 2004 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

#define CHECK_LINE_OF_FIRE_ATTRIB				CONSTLIT("checkLineOfFire")
#define RECHARGE_TIME_ATTRIB					CONSTLIT("rechargeTime")
#define FIRE_RATE_ATTRIB						CONSTLIT("fireRate")
#define INTERCEPT_RANGE_ATTRIB					CONSTLIT("interceptRange")
#define TARGET_ATTRIB							CONSTLIT("target")
#define TARGET_CRITERIA_ATTRIB					CONSTLIT("targetCriteria")
#define WEAPON_ATTRIB							CONSTLIT("weapon")
#define MAX_FIRE_ARC_ATTRIB						CONSTLIT("maxFireArc")
#define MIN_FIRE_ARC_ATTRIB						CONSTLIT("minFireArc")
#define OMNIDIRECTIONAL_ATTRIB					CONSTLIT("omnidirectional")

#define MISSILES_TARGET							CONSTLIT("missiles")

#define PROPERTY_FIRE_DELAY						CONSTLIT("fireDelay")
#define PROPERTY_FIRE_RATE						CONSTLIT("fireRate")
#define PROPERTY_ENABLED						CONSTLIT("enabled")
#define PROPERTY_EXTERNAL						CONSTLIT("external")
#define PROPERTY_FIRE_ARC						CONSTLIT("fireArc")
#define PROPERTY_OMNIDIRECTIONAL				CONSTLIT("omnidirectional")


const int DEFAULT_INTERCEPT_RANGE =				10;

CAutoDefenseClass::CAutoDefenseClass (void)

//	CAutoDefenseClass constructor

	{
	}

CSpaceObject *CAutoDefenseClass::FindTarget (CInstalledDevice *pDevice, CSpaceObject *pSource)

//	FindTarget
//
//	Returns an appropriate target (or NULL).

	{
	//	Look for a target

	bool isOmniDirectional = false;
	int iMinFireArc, iMaxFireArc;

	//	Look for a target 

	CSpaceObject *pBestTarget = NULL;
	CSystem *pSystem = pSource->GetSystem();
	CVector vSourcePos = pDevice->GetPos(pSource);

	//  Find out whether our autoDefenseDevice is omnidirectional,
	//  directional or has a fixed angle

	if (IsOmniDirectional(pDevice))
		isOmniDirectional = true;

	//  If not omnidirectional, check directional. If not, then
	//  our device points in one direction

	else if (!IsDirectional(pDevice, &iMinFireArc, &iMaxFireArc)) 
		{
		iMinFireArc = AngleMod((pDevice ? pDevice->GetRotation() : 0)
			+ AngleMiddle(m_iMinFireArc, m_iMaxFireArc));
		iMaxFireArc = iMinFireArc;
		}

	//  Calculate min/max fire arcs given the object's rotation

	iMinFireArc = (pSource->GetRotation() + iMinFireArc) % 360;
	iMaxFireArc = (pSource->GetRotation() + iMaxFireArc) % 360;

	//	Use the appropriate targeting method

	switch (m_iTargeting)
		{
		//	Hard-code search for the nearest missile

		case trgMissiles:
			{
			Metric rBestDist2 = m_rInterceptRange * m_rInterceptRange;

			for (int i = 0; i < pSystem->GetObjectCount(); i++)
				{
				CSpaceObject *pObj = pSystem->GetObject(i);

				if (pObj
						&& pObj->GetCategory() == CSpaceObject::catMissile
						&& !pObj->GetDamageSource().IsEqual(pSource)
						&& !pObj->IsIntangible()
						&& pSource->IsAngryAt(pObj->GetDamageSource())
						&& (AngleInArc(VectorToPolar((pObj->GetPos() - vSourcePos)), iMinFireArc, iMaxFireArc) ||
							isOmniDirectional))
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

			return pBestTarget;
			}

		//	Look for an object by criteria

		case trgCriteria:
			{
			//	First we set the source

			m_TargetCriteria.SetSource(pSource);

			//	Compute the range

			Metric rBestDist2;
			if (m_TargetCriteria.MatchesMaxRadius() < g_InfiniteDistance)
				rBestDist2 = (m_TargetCriteria.MatchesMaxRadius() * m_TargetCriteria.MatchesMaxRadius());
			else
				rBestDist2 = m_rInterceptRange * m_rInterceptRange;

			//	Now look for the nearest object

			CSpaceObjectCriteria::SCtx Ctx(m_TargetCriteria);
			for (int i = 0; i < pSystem->GetObjectCount(); i++)
				{
				CSpaceObject *pObj = pSystem->GetObject(i);
				Metric rDistance2;
				if (pObj
						&& (m_TargetCriteria.MatchesCategory(pObj->GetCategory()))
						&& ((rDistance2 = (pObj->GetPos() - vSourcePos).Length2()) < rBestDist2)
						&& pObj->MatchesCriteria(Ctx, m_TargetCriteria)
						&& !pObj->IsIntangible()
						&& pObj != pSource
						&& (AngleInArc(VectorToPolar((pObj->GetPos() - vSourcePos)), iMinFireArc, iMaxFireArc) ||
							isOmniDirectional))
					{
					pBestTarget = pObj;
					rBestDist2 = rDistance2;
					}
				}

			return pBestTarget;
			}

		default:
			ASSERT(false);
			return NULL;
		}
	}

int CAutoDefenseClass::GetActivateDelay (CItemCtx &ItemCtx) const

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
	CCodeChain &CC = GetUniverse().GetCC();
	CDeviceClass *pWeapon;
	CInstalledDevice *pDevice = Ctx.GetDevice();

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

	else if (strEquals(sProperty, PROPERTY_ENABLED))
		return (pDevice ? CC.CreateBool(pDevice->IsEnabled()) : CC.CreateNil());

	else if (strEquals(sProperty, PROPERTY_EXTERNAL))
		return CC.CreateBool(pDevice ? pDevice->IsExternal() : IsExternal());

	else if (strEquals(sProperty, PROPERTY_OMNIDIRECTIONAL))
		return CC.CreateBool(IsOmniDirectional(pDevice));

	else if (strEquals(sProperty, PROPERTY_FIRE_ARC))
		{
		int iMinFireArc;
		int iMaxFireArc;

		//	Omnidirectional

		if (IsOmniDirectional(pDevice))
			return CC.CreateString(PROPERTY_OMNIDIRECTIONAL);

		//	Fire arc

		else if (IsDirectional(pDevice, &iMinFireArc, &iMaxFireArc))
			{
			//	Create a list

			ICCItem *pResult = CC.CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			pList->AppendInteger(iMinFireArc);
			pList->AppendInteger(iMaxFireArc);

			return pResult;
			}

		//	Otherwise, see if we are pointing in a particular direction

		else
			{
			int iFacingAngle = AngleMod((pDevice ? pDevice->GetRotation() : 0) + AngleMiddle(m_iMinFireArc, m_iMaxFireArc));
			if (iFacingAngle == 0)
				return CC.CreateNil();
			else
				return CC.CreateInteger(iFacingAngle);
			}
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

int CAutoDefenseClass::GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse) const

//	GetPowerRating
//
//	Returns the minimum reactor power needed for this device

	{
	CDeviceClass *pWeapon = GetWeapon();

	if (pWeapon)
		return pWeapon->GetPowerRating(Ctx, retiIdlePowerUse);
	else
		{
		if (retiIdlePowerUse)
			*retiIdlePowerUse;

		return 0;
		}
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

Metric CAutoDefenseClass::GetShotSpeed (CItemCtx &Ctx) const

//	GetShotSpeed
//
//	Returns the speed of the shot.

	{
	CDeviceClass *pWeapon = GetWeapon();
	if (pWeapon == NULL)
		return 0.0;

	return pWeapon->GetShotSpeed(Ctx);
	}

bool CAutoDefenseClass::IsAreaWeapon (CSpaceObject *pSource, CInstalledDevice *pDevice)

//	IsAreaWeapon
//
//	Returns TRUE if this is an area weapon.

	{
	CDeviceClass *pWeapon = GetWeapon();
	if (pWeapon == NULL)
		return false;

	return pWeapon->IsAreaWeapon(pSource, pDevice);
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

bool CAutoDefenseClass::IsDirectional(CInstalledDevice *pDevice, int *retiMinFireArc, int *retiMaxFireArc)

//	IsDirectional
//
//	Returns TRUE if the autodefensedevice can turn but is not omni
//  Copied from CWeaponClass.cpp

	{
	//	If the weapon is omnidirectional then we don't need directional
	//	calculations.

	if (m_bOmnidirectional || (pDevice && pDevice->IsOmniDirectional()))
		return false;

	//	If we have a device, combine the fire arcs of device slot and 
	//  autodefensedevice

	if (pDevice)
		{
		//	If the device is directional then we always take the fire arc from
		//	the device slot.

		if (pDevice->IsDirectional())
			{
			if (retiMinFireArc)
				*retiMinFireArc = pDevice->GetMinFireArc();
			if (retiMaxFireArc)
				*retiMaxFireArc = pDevice->GetMaxFireArc();

			return true;
			}

		//	Otherwise, see if the autodefensedevice is directional.

		else if (m_iMinFireArc != m_iMaxFireArc)
			{
			//	If the device points in a specific direction then we offset the
			//	autodefensedevice's fire arc.

			int iDeviceSlotOffset = pDevice->GetMinFireArc();

			if (retiMinFireArc)
				*retiMinFireArc = (m_iMinFireArc + iDeviceSlotOffset) % 360;
			if (retiMaxFireArc)
				*retiMaxFireArc = (m_iMaxFireArc + iDeviceSlotOffset) % 360;

			return true;
			}

		//	Otherwise, we are not directional

		else
			return false;
		}
	else
		{
		//	Otherwise, just check the autodefensedevice

		if (retiMinFireArc)
			*retiMinFireArc = m_iMinFireArc;
		if (retiMaxFireArc)
			*retiMaxFireArc = m_iMaxFireArc;

		return (m_iMinFireArc != m_iMaxFireArc);
		}
	}

bool CAutoDefenseClass::IsOmniDirectional(CInstalledDevice *pDevice)

//	IsOmniDirectional
//
//	Returns TRUE if the autodefensedevice is omnidirectional (not limited)
//  Copied from CWeaponClass.cpp

	{
	//	The device slot improves the autodefensedevice. If the device slot is
	//  directional, then the autodefensedevice is directional. If the device
	//  slot is omni directional, then the autodefensedevice is
	//  omnidirectional.

	if (pDevice && pDevice->IsOmniDirectional())
		return true;

	return m_bOmnidirectional;
	}

void CAutoDefenseClass::Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx)

//	Update
//
//	Update device

	{
	DEBUG_TRY

	CDeviceClass *pWeapon = GetWeapon();
	if (pWeapon == NULL || pDevice == NULL || pSource == NULL)
		return;

	//	Update the weapon

	pWeapon->Update(pDevice, pSource, Ctx);

	//	Skip if we're not ready or disabled
	//
	//	NOTE: If pDevice is damaged or disrupted we handle this as a potential 
	//	misfire inside of pWeapon->Activate.

	if (!pDevice->IsReady() || !pDevice->IsEnabled())
		return;

	//	If the ship is disarmed or paralyzed, then we do not fire.

	if (pSource->GetCondition(CConditionSet::cndParalyzed) 
			|| pSource->GetCondition(CConditionSet::cndDisarmed))
		return;

	//	If we're docked with a station, then we do not fire.

	if (m_bCheckLineOfFire && pSource->GetDockedObj())
		return;

	//	Look for a target; if none, then skip.

	CSpaceObject *pTarget = FindTarget(pDevice, pSource);
	if (pTarget == NULL)
		return;

	//	Shoot at target

	int iFireAngle = pWeapon->CalcFireSolution(pDevice, pSource, pTarget);
	if (iFireAngle == -1)
		return;

	//	If friendlies are in the way, don't shoot

	if (m_bCheckLineOfFire
			&& !pSource->IsLineOfFireClear(pDevice, pTarget, iFireAngle, pSource->GetDistance(pTarget)))
		return;

	//	Since we're using this as a target, set the destroy notify flag
	//	(Normally beams don't notify, so we need to override this).

	pTarget->SetDestructionNotify();

	//	Fire

	pDevice->SetFireAngle(iFireAngle);
	pWeapon->Activate(pDevice, pSource, pTarget, &Ctx.bSourceDestroyed, &Ctx.bConsumedItems);
	pDevice->SetTimeUntilReady(m_iRechargeTicks);

	//	Identify

	if (pSource->IsPlayer())
		GetItemType()->SetKnown();

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

	pDevice->m_iMinFireArc = AngleMod(pDesc->GetAttributeInteger(MIN_FIRE_ARC_ATTRIB));
	pDevice->m_iMaxFireArc = AngleMod(pDesc->GetAttributeInteger(MAX_FIRE_ARC_ATTRIB));

	//  Set omnidirectional if either the "omnidirectional" attribute is True, OR
	//  if minfirearc = maxfirearc = 0.
	pDevice->m_bOmnidirectional = (pDesc->GetAttributeBool(OMNIDIRECTIONAL_ATTRIB) ||
		((pDevice->m_iMaxFireArc == pDevice->m_iMinFireArc) && pDevice->m_iMaxFireArc == 0));

	pDevice->m_iRechargeTicks = mathRound(iFireRate / STD_SECONDS_PER_UPDATE);
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

		pDevice->m_TargetCriteria.Init(sCriteria);
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

	//	Options

	pDevice->m_bCheckLineOfFire = pDesc->GetAttributeBool(CHECK_LINE_OF_FIRE_ATTRIB);

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

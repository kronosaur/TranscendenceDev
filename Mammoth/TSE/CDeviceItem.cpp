//	CDeviceItem.cpp
//
//	CDeviceItem class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CDeviceItem::AccumulateAttributes (const CItem &Ammo, TArray<SDisplayAttribute> *retList) const

//	AccumulateAttributes
//
//	Accumulate display attributes.

	{
	GetDeviceClass().AccumulateAttributes(*this, Ammo, retList);
	}

CDeviceItem::ECalcTargetTypes CDeviceItem::CalcTargetType (void) const

//	CalcTargetType
//
//	Helper function to figure out the kind of target to compute.

	{
	const CInstalledDevice &Device = *GetInstalledDevice();
	CSpaceObject *pSource = GetSource();
	if (pSource == NULL)
		return calcNoTarget;

	//	For primary weapons, the target is the controller target.
	//	
	//	NOTE: Selectable means that the weapon is not a secondary weapon
	//	and not a linked-fire weapon. We specifically exclude "fire if selected"
	//  linked-fire weapons, which normally count as "selectable", from this definition.

	DWORD dwLinkedFireSelected = CDeviceClass::lkfSelected | CDeviceClass::lkfSelectedVariant;

	if (Device.IsSelectable() && !(Device.GetSlotLinkedFireOptions() & dwLinkedFireSelected))
		{
		return calcControllerTarget;
		}

	//	Otherwise this is a linked fire weapon or a secondary weapon.

	else
		{
		const CDeviceClass &Weapon = GetDeviceClass();

		//	Get the actual options.

		DWORD dwLinkedFireOptions = GetLinkedFireOptions();

		CInstalledDevice *pPrimaryWeapon = pSource->GetNamedDevice(devPrimaryWeapon);
		CInstalledDevice *pSelectedLauncher = pSource->GetNamedDevice(devMissileWeapon);

		DWORD dwLinkedFireSelected = CDeviceClass::lkfSelected | CDeviceClass::lkfSelectedVariant;

		if ((dwLinkedFireOptions & CDeviceClass::lkfNever))
			{
			return calcNoTarget;
			}

		//	If our options is "fire always" or "fire if selected" then our target is always the same
		//	as the primary target. Same if we have auto fire enabled

		else if ((dwLinkedFireOptions & CDeviceClass::lkfAlways) || (dwLinkedFireOptions & dwLinkedFireSelected) || (pSource->IsPlayer() && Device.IsAutomatedWeapon()))
			{
			return calcControllerTarget;
			}

		//	Otherwise, we need to let our controller find a target for this weapon.

		else
			{
			return calcWeaponTarget;
			}
		}
	}

TSharedPtr<CItemEnhancementStack> CDeviceItem::GetEnhancementStack (void) const

//	GetEnhancementStack
//
//	Returns the enhancement stack for this item. If there are no enhancements, 
//	we return NULL.

	{
	//	If we have installed armor, then get the enhancement stack from it.

	if (const CInstalledDevice *pInstalled = m_Item.GetInstalledDevice())
		return pInstalled->GetEnhancementStack();

	//	Otherwise, see if we've got a cached enhancement stack

	if (m_pEnhancements)
		return m_pEnhancements;

	//	Otherwise, we need to create one from mods

	const CItemEnhancement &Mods = m_Item.GetMods();
	if (Mods.IsEmpty())
		return NULL;

	m_pEnhancements.TakeHandoff(new CItemEnhancementStack);
	m_pEnhancements->Insert(Mods);
	return m_pEnhancements;
	}

int CDeviceItem::GetFireArc (void) const

//	GetFireArc
//
//	Returns the fire arc for swivel weapons and turrets.

	{
	int iMinArc, iMaxArc;
	switch (GetType().GetDeviceClass()->GetRotationType(*this, &iMinArc, &iMaxArc))
		{
		case CDeviceRotationDesc::rotOmnidirectional:
			return 360;

		case CDeviceRotationDesc::rotSwivel:
			return AngleRange(iMinArc, iMaxArc);

		default:
			return 0;
		}
	}

int CDeviceItem::GetHP (int *retiMaxHP, bool bUninstalled) const

//	GetHP
//
//	Returns current hit point level.

	{
	const CInstalledDevice *pInstalled = GetInstalledDevice();
	if (!bUninstalled && pInstalled)
		{
		CItemCtx ItemCtx(GetSource(), pInstalled);
		return GetDeviceClass().GetHitPoints(ItemCtx, retiMaxHP);
		}
	else
		{
		CItemCtx ItemCtx(*this);
		return GetDeviceClass().GetHitPoints(ItemCtx, retiMaxHP);
		}
	}

DWORD CDeviceItem::GetLinkedFireOptions (void) const

//	GetLinkedFireOptions
//
//	Returns linked fire options.

	{
	//	Options from enhancements

	DWORD dwOptions = GetEnhancements().GetLinkedFireOptions();

	//	Options from the device slot

	if (const CInstalledDevice *pInstalled = m_Item.GetInstalledDevice())
		dwOptions = CDeviceClass::CombineLinkedFireOptions(dwOptions, pInstalled->GetSlotLinkedFireOptions());

	//	Options from device class

	dwOptions = CDeviceClass::CombineLinkedFireOptions(dwOptions, GetDeviceClass().GetLinkedFireOptions());
	
	//	Done

	return dwOptions;
	}

Metric CDeviceItem::GetMaxEffectiveRange (CSpaceObject *pTarget) const

//	GetMaxEffectiveRange
//
//	Returns the maximum effective range.

	{
	if (const CInstalledDevice *pInstalled = GetInstalledDevice())
		{
		return GetDeviceClass().GetMaxEffectiveRange(pInstalled->GetSource(), pInstalled, pTarget);
		}
	else
		{
		return GetDeviceClass().GetMaxEffectiveRange(NULL, NULL, pTarget);
		}
	}

int CDeviceItem::GetMaxHP (void) const

//	GetMaxHP
//
//	Return max hit points.

	{
	int iMaxHP;

	const CInstalledDevice *pInstalled = GetInstalledDevice();
	if (pInstalled)
		{
		CItemCtx ItemCtx(GetSource(), pInstalled);
		GetDeviceClass().GetHitPoints(ItemCtx, &iMaxHP);
		}
	else
		{
		CItemCtx ItemCtx(*this);
		GetDeviceClass().GetHitPoints(ItemCtx, &iMaxHP);
		}

	return iMaxHP;
	}

DamageTypes CDeviceItem::GetWeaponDamageType (void) const

//	GetWeaponDamageType
//
//	Returns the damage type for the weapon.

	{
	const CWeaponFireDesc *pShot = GetWeaponFireDesc();

	//	OK if we don't find shot--could be a launcher with no ammo

	if (pShot == NULL)
		return damageGeneric;

	//	Get the damage type

	return pShot->GetDamageType();
	}

bool CDeviceItem::IsWeaponAligned (CSpaceObject *pTarget, int *retiAimAngle, int *retiFireAngle) const

//	IsWeaponAligned
//
//	Returns TRUE if we're aligned.

	{
	if (const CInstalledDevice *pInstalled = GetInstalledDevice())
		{
		return GetDeviceClass().IsWeaponAligned(pInstalled->GetSource(), pInstalled, pTarget, retiAimAngle, retiFireAngle);
		}
	else
		return false;
	}

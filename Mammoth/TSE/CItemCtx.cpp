//	CItemCtx.cpp
//
//	CItemCtx class
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TSharedPtr<CItemEnhancementStack> CItemCtx::m_pNullEnhancements(new CItemEnhancementStack);

void CItemCtx::ClearItemCache (void)

//	ClearItemCache
//
//	Clear m_pItem, forcing us to get it from the source again.

	{
	if (m_pSource
			&& (m_pDevice || m_pArmor))
		m_pItem = NULL;
	}

void CItemCtx::Copy (const CItemCtx &Src)

//	Copy
//
//	Copy from source

	{
	//	Sometimes m_pItem points to a local CItem. In that case, we need to
	//	change it to point to us.

	m_Item = Src.m_Item;
	if (Src.m_pItem == &Src.m_Item)
		{
		m_pItem = &m_Item;
		}

	//	Otherwise, we assume the pointer is valid.

	else
		{
		m_pItem = Src.m_pItem;
		}

	//	Other fields

	m_pSource = Src.m_pSource;
	m_pSourceShipClass = Src.m_pSourceShipClass;
	m_pArmor = Src.m_pArmor;
	m_pDevice = Src.m_pDevice;
	m_pEnhancements = Src.m_pEnhancements;
	m_pWeapon = Src.m_pWeapon;
	m_iVariant = Src.m_iVariant;
	m_Weapon = Src.m_Weapon;
	}

ICCItem *CItemCtx::CreateItemVariable (CCodeChain &CC)

//	CreateItemVariable
//
//	Creates a variable for the item (or Nil)

	{
	const CItem *pItem = GetItemPointer();
	if (pItem == NULL)
		return CC.CreateNil();

	return CreateListFromItem(*pItem);
	}

CInstalledArmor *CItemCtx::GetArmor (void) const

//	GetArmor
//
//	Returns the installed armor struct

	{
	//	If we've got it, done

	if (m_pArmor)
		return m_pArmor;

	//	Otherwise, try to get it from the item and source

	CShip *pShip;
	if (m_pSource 
			&& m_pItem 
			&& m_pItem->IsInstalled() 
			&& m_pItem->GetType()
			&& m_pItem->GetType()->GetCategory() == itemcatArmor
			&& (pShip = m_pSource->AsShip()))
		{
		//	Cache it in case someone asks later
		m_pArmor = pShip->GetArmorSection(m_pItem->GetInstalled());
		return m_pArmor;
		}

	//	Couldn't get it

	return NULL;
	}

CArmorClass *CItemCtx::GetArmorClass (void) const

//	GetArmorClass
//
//	Returns the armor class

	{
	//	Get it from the installed device

	if (m_pArmor)
		return m_pArmor->GetClass();

	//	Otherwise, get it from the item

	if (m_pItem)
		{
		CItemType *pType = m_pItem->GetType();
		if (pType)
			return pType->GetArmorClass();
		}

	//	Couldn't get it

	return NULL;
	}

CInstalledDevice *CItemCtx::GetDevice (void)

//	GetDevice
//
//	Returns the installed device struct

	{
	//	If we've got it, done

	if (m_pDevice)
		return m_pDevice;

	//	Otherwise, try to get it from the item and source

	if (m_pSource 
			&& m_pItem 
			&& m_pItem->IsInstalled()
			&& m_pItem->GetType()
			&& m_pItem->GetType()->GetDeviceClass())
		{
		//	Cache it in case someone asks later
		m_pDevice = m_pSource->FindDevice(*m_pItem);
		return m_pDevice;
		}

	//	Couldn't get it

	return NULL;
	}

CDeviceClass *CItemCtx::GetDeviceClass (void)

//	GetDeviceClass
//
//	Returns the device class

	{
	//	Get it from the installed device

	if (m_pDevice)
		return m_pDevice->GetClass();

	//	Otherwise, get it from the item

	if (m_pItem)
		{
		CItemType *pType = m_pItem->GetType();
		if (pType)
			return pType->GetDeviceClass();
		}

	//	Couldn't get it

	return NULL;
	}

CDeviceItem CItemCtx::GetDeviceItem (void)
	{
	if (m_pDevice)
		return m_pDevice->GetDeviceItem();
	else
		{
		const CItem &Item = GetItem();
		if (CDeviceItem DeviceItem = Item.AsDeviceItem())
			return DeviceItem;
		else if (Item.IsMissile() && ResolveVariant())
			return m_Weapon.AsDeviceItemOrThrow();
		else
			return CItem::NullItem().AsDeviceItem();
		}
	}

TSharedPtr<CItemEnhancementStack> CItemCtx::GetEnhancementStack (void)

//	GetEnhancementStack
//
//	Returns the enhancement stack for the given item. May return NULL.

	{
	//	If we have installed armor, then get the enhancement stack from it.

	CInstalledArmor *pArmor = GetArmor();
	if (pArmor)
		return pArmor->GetEnhancementStack();

	//	If we have an installed device, then get the enhancement stack from it.

	CInstalledDevice *pDevice = GetDevice();
	if (pDevice)
		return pDevice->GetEnhancementStack();

	//	Otherwise, see if we've got a cached enhancement stack

	if (m_pEnhancements)
		return m_pEnhancements;

	//	Otherwise, we need to create one from mods

	const CItemEnhancement &Mods = GetMods();
	if (Mods.IsEmpty())
		return NULL;

	m_pEnhancements.TakeHandoff(new CItemEnhancementStack);
	m_pEnhancements->Insert(Mods);
	return m_pEnhancements;
	}

const CItem &CItemCtx::GetItem (void) const

//	GetItem
//
//	Returns the item struct

	{
	//	If we've got an item, then return a reference to it
	//	Whoever set this is responsible to guaranteeing its lifespan

	if (m_pItem)
		return *m_pItem;

	//	Otherwise, we need to cons one up

	const CItem *pItem = GetItemPointer();
	if (pItem == NULL)
		return CItem::GetNullItem();

	//	We cache it for later

	m_Item = *pItem;
	m_pItem = &m_Item;

	//	Done

	return *m_pItem;
	}

int CItemCtx::GetItemCharges (void)

//	GetDeviceCharges
//
//	Returns the number of charges for the device (or 0).

	{
	const CItem &Item = GetItem();
	if (Item.IsEmpty())
		return 0;

	return Item.GetCharges();
	}

const CItem *CItemCtx::GetItemPointer(void) const

//	GetItemPointer
//
//	Returns a pointer to an item or NULL.
//	Note: We can't guarantee that the pointer will be valid across calls
//	to script (because an ItemTable might be moved around) so this
//	pointer should never be returned to callers.

	{
	//	If we've got it, done

	if (m_pItem)
		return m_pItem;

	//	If we have a source and device, find the item

	if (m_pDevice && m_pSource)
		return m_pDevice->GetItem();

	//	If we have a source and armor, find the item

	if (m_pArmor && m_pSource)
		return m_pArmor->GetItem();

	//	Couldn't get it

	return NULL;
	}

int CItemCtx::GetItemVariantNumber(void)

//	GetItemVariantNumber
//
//	Returns the variant number for the item (or 0).

	{
	const CItem &Item = GetItem();
	if (Item.IsEmpty())
		return 0;

	return Item.GetVariantNumber();
	}

const CItemEnhancement &CItemCtx::GetMods(void)

//	GetMods
//
//	Returns mods for the item

	{
	//	If we've got an item, get mods from there

	if (m_pItem)
		return m_pItem->GetMods();

	//	Else, we have to get an item

	return GetItem().GetMods();
	}

const CShipClass *CItemCtx::GetSourceShipClass (void) const

//	GetSourceShipClass
//
//	If the source is a ship, we return the ship class. NULL otherwise.

	{
	if (m_pSource == NULL)
		return m_pSourceShipClass;

	CShip *pShip = m_pSource->AsShip();
	if (pShip == NULL)
		return NULL;

	return pShip->GetClass();
	}

bool CItemCtx::IsDeviceDamaged (void)

//	IsDeviceDamaged
//
//	Returns TRUE if the device is damaged.

	{
	return (GetDevice() != NULL && GetDevice()->IsDamaged());
	}

bool CItemCtx::IsDeviceDisrupted (void)

//	IsDeviceDisrupted
//
//	Returns TRUE if the device is disrupted.

	{
	return (GetDevice() != NULL && GetDevice()->IsDisrupted());
	}

bool CItemCtx::IsDeviceEnabled (void)

//  IsDeviceEnabled
//
//  Returns TRUE if the device is enabled.
//  NOTE: If we don't have a CInstalledDevice, we alway treat as enabled.
	
	{
	return (GetDevice() == NULL || GetDevice()->IsEnabled());
	}

bool CItemCtx::IsDeviceWorking (void)

//  IsDeviceWorking
//
//  Returns TRUE if the device is working.
//  NOTE: If we don't have a CInstalledDevice, we alway treat as working.
	
	{
	return (GetDevice() == NULL || GetDevice()->IsWorking());
	}

bool CItemCtx::ResolveVariant (void)

//	ResolveVariant
//
//	If m_pItem is a missile, we look for the weapon that can launch it and 
//	cache it in m_pWeapon and m_iVariant. If successful, we return TRUE.

	{
	if (m_pItem == NULL)
		return false;

	//	Look through all weapons that can launch this ammo. We pick the first
	//	weapon (arbitrarily).

	if (!CDeviceClass::FindWeaponFor(m_pItem->GetType(), &m_pWeapon, &m_iVariant))
		return false;

	m_Weapon = CItem(m_pWeapon->GetItemType(), 1);
	return true;
	}

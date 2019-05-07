//	CDeviceSystem.cpp
//
//	CDeviceSystem class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CDeviceSystem::CDeviceSystem (void)

//	CDeviceSystem constructor

	{
	int i;

	for (i = 0; i < devNamesCount; i++)
		m_NamedDevices[i] = -1;
	}

void CDeviceSystem::AccumulateEnhancementsToArmor (CSpaceObject *pObj, CInstalledArmor *pArmor, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements)

//	AccumulateDeviceEnhancementsToArmor
//
//	Adds enhancements to the given armor segment.

	{
	int i;

	for (i = 0; i < m_Devices.GetCount(); i++)
		if (!m_Devices[i].IsEmpty())
			{
			//	See if this device enhances us

			if (m_Devices[i].AccumulateEnhancements(pObj, pArmor, EnhancementIDs, pEnhancements))
				{
				//	If the device affected something, then we now know what it is

				if (pObj->IsPlayer())
					m_Devices[i].GetItem()->SetKnown();
				}
			}
	}

void CDeviceSystem::AccumulatePerformance (SShipPerformanceCtx &Ctx) const

//	AccumulatePerformance
//
//	Accumulate performance metrics.

	{
	int i;

    for (i = 0; i < m_Devices.GetCount(); i++)
        if (!m_Devices[i].IsEmpty())
            {
            CItemCtx ItemCtx(Ctx.pShip, &m_Devices[i]);
            ItemCtx.GetDevice()->AccumulatePerformance(ItemCtx, Ctx);
            }
	}

void CDeviceSystem::AccumulatePowerUsed (SUpdateCtx &Ctx, CSpaceObject *pObj, int &iPowerUsed, int &iPowerGenerated)

//	AccumulatePowerUsed
//
//	Increments power used and power generated for all devices.

	{
	int i;

	for (i = 0; i < m_Devices.GetCount(); i++)
		{
		int iDevicePower = m_Devices[i].CalcPowerUsed(Ctx, pObj);
		if (iDevicePower >= 0)
			iPowerUsed += iDevicePower;
		else
			iPowerGenerated += -iDevicePower;
		}
	}

int CDeviceSystem::CalcSlotsInUse (int *retiWeaponSlots, int *retiNonWeapon) const

//	CalcSlotsInUse
//
//	Returns the number of device slots being used

	{
	int i;
	int iAll = 0;
	int iWeapons = 0;
	int iNonWeapons = 0;

	//	Count the number of slots being used up currently

	for (i = 0; i < m_Devices.GetCount(); i++)
		{
		const CInstalledDevice &Device = m_Devices[i];
		if (!Device.IsEmpty())
			{
			int iSlots = Device.GetClass()->GetSlotsRequired();
			iAll += iSlots;

			if (Device.GetCategory() == itemcatWeapon 
					|| Device.GetCategory() == itemcatLauncher)
				iWeapons += iSlots;
			else
				iNonWeapons += iSlots;
			}
		}

	if (retiWeaponSlots)
		*retiWeaponSlots = iWeapons;

	if (retiNonWeapon)
		*retiNonWeapon = iNonWeapons;

	return iAll;
	}

void CDeviceSystem::CleanUp (void)

//	CleanUp
//
//	Reset

	{
	int i;

	m_Devices.DeleteAll();

	for (i = 0; i < devNamesCount; i++)
		m_NamedDevices[i] = -1;
	}

CInstalledDevice *CDeviceSystem::FindDevice (const CItem &Item)

//	FindDevice
//
//	Finds the given device.

	{
	if (Item.IsInstalled() && Item.GetType()->IsDevice())
		{
		int iDevSlot = Item.GetInstalled();
		return &m_Devices[iDevSlot];
		}
	else
		return NULL;
	}

int CDeviceSystem::FindDeviceIndex (const CItem &Item, DWORD dwFlags) const

//	FindDeviceIndex
//
//	Finds the given device (or -1).

	{
	//	Must have a type

	CItemType *pType;
	if ((pType = Item.GetType()) == NULL)
		return -1;

	//	If the item is installed, then look for this exact item.

	else if (Item.IsInstalled())
		{
		if (!pType->IsDevice())
			return -1;

		int iDevSlot = Item.GetInstalled();

		//	Validate if necessary

		if (dwFlags & FLAG_VALIDATE_ITEM)
			{
			if (iDevSlot < 0 || iDevSlot >= m_Devices.GetCount() || m_Devices[iDevSlot].IsEmpty() || m_Devices[iDevSlot].GetItem() == NULL)
				return -1;

			if (!Item.IsEqual(*m_Devices[iDevSlot].GetItem()))
				return -1;
			}

		//	Found

		return iDevSlot;
		}

	//	Look for a device of the given type

	else if (dwFlags & FLAG_MATCH_BY_TYPE)
		{
		for (int i = 0; i < GetCount(); i++)
			{
			const CInstalledDevice &Device = GetDevice(i);
			if (!Device.IsEmpty() 
					&& Device.GetItem()
					&& Device.GetItem()->GetType() == pType)
				{
				return i;
				}
			}

		return -1;
		}

	//	Not found

	else
		return -1;
	}

int CDeviceSystem::FindFreeSlot (void)

//	FindFreeSlot
//
//	Returns the index of a free device slot; -1 if none left

	{
	int i;

	for (i = 0; i < m_Devices.GetCount(); i++)
		if (m_Devices[i].IsEmpty())
			return i;

	//	We need to allocate a new slot

	int iNewSlot = m_Devices.GetCount();
	m_Devices.InsertEmpty();
	return iNewSlot;
	}

int CDeviceSystem::FindNamedIndex (const CItem &Item) const

//	FindNamedIndex
//
//	Returns the DeviceName of the given item. We assume that the
//	item is installed. Returns -1 if not installed.

	{
	if (Item.IsInstalled())
		{
		//	The device slot that this item is installed in is
		//	stored in the data field.

		int iSlot = Item.GetInstalled();

		//	Look to see if this slot has a name

		for (int i = devFirstName; i < devNamesCount; i++)
			if (m_NamedDevices[i] == iSlot)
				return i;
		}

	//	Not named or not installed

	return -1;
	}

int CDeviceSystem::FindNextIndex (CSpaceObject *pObj, int iStart, ItemCategories Category, int iDir, bool switchWeapons) const

//	FindNextIndex
//
//	Finds the next device of the given category

	{
	int iStartingSlot;

	//	iStartingSlot is always >= GetCount() so that we can iterate
	//	backwards if necessary.

	if (iStart == -1)
		iStartingSlot = (iDir == 1 ? GetCount() : GetCount() - 1);
	else
		iStartingSlot = GetCount() + (iStart + iDir);

	//	Loop until we find an appropriate device

	for (int i = 0; i < GetCount(); i++)
		{
		int iDevice = ((iDir * i) + iStartingSlot) % GetCount();
		if (!m_Devices[iDevice].IsEmpty() 
				&& m_Devices[iDevice].GetCategory() == Category
				&& m_Devices[iDevice].IsSelectable(CItemCtx(pObj, &m_Devices[iDevice])))
			return iDevice;
		}

	return -1;
	}

int CDeviceSystem::FindRandomIndex (bool bEnabledOnly) const

//	FindRandomIndex
//
//	Returns a random device

	{
	int i;

	//	Count the number of valid devices

	int iCount = 0;
	for (i = 0; i < GetCount(); i++)
		if (!m_Devices[i].IsEmpty() 
				&& (!bEnabledOnly || m_Devices[i].IsEnabled()))
			iCount++;

	if (iCount == 0)
		return -1;

	int iDev = mathRandom(1, iCount);

	//	Return the device

	for (i = 0; i < GetCount(); i++)
		if (!m_Devices[i].IsEmpty()
				&& (!bEnabledOnly || m_Devices[i].IsEnabled()))
			{
			if (--iDev == 0)
				return i;
			}

	return -1;
	}

bool CDeviceSystem::FindWeaponByItem (const CItem &Item, int *retiIndex, int *retiVariant) const

//	FindWeaponByItem
//
//	Finds a weapon and variant.

	{
	CItemType *pType = Item.GetType();
	if (pType == NULL)
		return false;

	CDeviceClass *pClass = pType->GetDeviceClass();

	//	If the item is not a device, then see if it is ammo.

	if (pClass == NULL)
		{
		for (int i = 0; i < GetCount(); i++)
			{
			const CInstalledDevice &Device = GetDevice(i);
			if (Device.IsEmpty())
				continue;

			int iVariant = Device.GetClass()->GetAmmoVariant(pType);
			if (iVariant == -1)
				continue;

			if (retiIndex) *retiIndex = i;
			if (retiVariant) *retiVariant = iVariant;
			return true;
			}

		return false;
		}

	//	Otherwise, if this is a weapon or launcher, then look for it.

	else if (pClass->GetCategory() == itemcatWeapon || pClass->GetCategory() == itemcatLauncher)
		{
		int iDevSlot = FindDeviceIndex(Item, FLAG_VALIDATE_ITEM | FLAG_MATCH_BY_TYPE);
		if (iDevSlot == -1)
			return false;

		if (retiIndex) *retiIndex = iDevSlot;
		if (retiVariant) *retiVariant = 0;
		return true;
		}

	//	Otherwise, not found

	else
		return false;
	}

int CDeviceSystem::GetCountByID (const CString &sID) const

//	GetCountByID
//
//	Returns the number of installed devices with the given ID.

	{
	int i;

	int iCount = 0;
	for (i = 0; i < m_Devices.GetCount(); i++)
		{
		if (!m_Devices[i].IsEmpty() 
				&& strEquals(sID, m_Devices[i].GetID()))
			iCount++;
		}

	return iCount;
	}

DeviceNames CDeviceSystem::GetNamedFromDeviceIndex (int iIndex) const

//	GetIndexFromDeviceIndex
//
//	Returns the device name from the index.

	{
	int i;

	for (i = devFirstName; i < devNamesCount; i++)
		if (m_NamedDevices[i] == iIndex)
			return (DeviceNames)i;

	return devNone;
	}

bool CDeviceSystem::Init (CSpaceObject *pObj, const CDeviceDescList &Devices, int iMaxDevices)

//	Init
//
//	Initialize based on a list of device descriptor.

	{
	int i;


	//	Allocate devices

	CleanUp();
	m_Devices.InsertEmpty(Max(Devices.GetCount(), iMaxDevices));

	//	Add items to the object, as specified.

	CItemListManipulator ObjItems(pObj->GetItemList());

	//	Install devices.

	for (i = 0; i < Devices.GetCount(); i++)
		{
		const SDeviceDesc &NewDevice = Devices.GetDeviceDesc(i);

		//	Add item

		ObjItems.AddItem(NewDevice.Item);

		//	Install the device

		m_Devices[i].InitFromDesc(NewDevice);
		m_Devices[i].Install(pObj, ObjItems, i, true);

		//	Assign to named devices

		switch (m_Devices[i].GetCategory())
			{
			case itemcatWeapon:
				if (m_NamedDevices[devPrimaryWeapon] == -1
						&& m_Devices[i].IsSelectable(CItemCtx(pObj, &m_Devices[i])))
					m_NamedDevices[devPrimaryWeapon] = i;
				break;

			case itemcatLauncher:
				if (m_NamedDevices[devMissileWeapon] == -1
						&& m_Devices[i].IsSelectable(CItemCtx(pObj, &m_Devices[i])))
					m_NamedDevices[devMissileWeapon] = i;
				break;

			case itemcatShields:
				if (m_NamedDevices[devShields] == -1)
					m_NamedDevices[devShields] = i;
				break;

			case itemcatDrive:
				if (m_NamedDevices[devDrive] == -1)
					m_NamedDevices[devDrive] = i;
				break;

			case itemcatCargoHold:
				if (m_NamedDevices[devCargo] == -1)
					m_NamedDevices[devCargo] = i;
				break;

			case itemcatReactor:
				if (m_NamedDevices[devReactor] == -1)
					m_NamedDevices[devReactor] = i;
				break;
			}

		//	Add extra items (we do this at the end because this will
		//	modify the cursor)

		ObjItems.AddItems(NewDevice.ExtraItems);
		}

	//	Success

	return true;
	}

bool CDeviceSystem::Install (CSpaceObject *pObj, CItemListManipulator &ItemList, int iDeviceSlot, int iSlotPosIndex, bool bInCreate, int *retiDeviceSlot)

//	Install
//
//	Installs a new item.

	{
	//	Look for a free slot to install to

	if (iDeviceSlot == -1)
		{
		iDeviceSlot = FindFreeSlot();
		if (iDeviceSlot == -1)
			{
			ASSERT(false);
			return false;
			}
		}

	CInstalledDevice &Device = m_Devices[iDeviceSlot];
    CItemCtx ItemCtx(pObj, &Device);

	//	Update the structure

	Device.Install(pObj, ItemList, iDeviceSlot);

	//	If we have a slot positing index, set it now

	if (iSlotPosIndex != -1)
		Device.SetSlotPosIndex(iSlotPosIndex);

	//	Adjust the named devices

	switch (Device.GetCategory())
		{
		case itemcatWeapon:
			if (Device.IsSelectable(ItemCtx))
				m_NamedDevices[devPrimaryWeapon] = iDeviceSlot;
			break;

		case itemcatLauncher:
			if (Device.IsSelectable(ItemCtx))
				m_NamedDevices[devMissileWeapon] = iDeviceSlot;
			break;

		case itemcatShields:
			m_NamedDevices[devShields] = iDeviceSlot;
			//	If we just installed a shield generator, start a 0 energy
			Device.Reset(pObj);
			break;

		case itemcatDrive:
			m_NamedDevices[devDrive] = iDeviceSlot;
			break;

		case itemcatCargoHold:
			m_NamedDevices[devCargo] = iDeviceSlot;
			break;

		case itemcatReactor:
			m_NamedDevices[devReactor] = iDeviceSlot;
			break;
		}

	//	Done

	if (retiDeviceSlot)
		*retiDeviceSlot = iDeviceSlot;

	return true;
	}

bool CDeviceSystem::IsSlotAvailable (ItemCategories iItemCat, int *retiSlot) const

//	IsSlotAvailable
//
//	Returns TRUE if it is possible to install a new device. If the slot is full
//	then we return the device slot that we need to replace.

	{
	int i;

	switch (iItemCat)
		{
		case itemcatLauncher:
		case itemcatReactor:
		case itemcatShields:
		case itemcatCargoHold:
		case itemcatDrive:
			{
			//	Look for a device of the given slot category.

			for (i = 0; i < m_Devices.GetCount(); i++)
				if (!m_Devices[i].IsEmpty()
						&& m_Devices[i].GetCategory() == iItemCat)
					{
					if (retiSlot)
						*retiSlot = i;

					return false;
					}

			return true;
			}

		default:
			ASSERT(retiSlot == NULL);
			return true;
		}
	}

void CDeviceSystem::MarkImages (void)

//	MarkImages
//
//	Marks images

	{
	int i;

	for (i = 0; i < m_Devices.GetCount(); i++)
		if (!m_Devices[i].IsEmpty())
			m_Devices[i].GetClass()->MarkImages();
	}

bool CDeviceSystem::OnDestroyCheck (CSpaceObject *pObj, DestructionTypes iCause, const CDamageSource &Attacker)

//	OnDestroyCheck
//
//	Checks to see if any devices/powers prevent us from being destroyed.
//	Returns TRUE if the ship is destroyed; FALSE otherwise

	{
	int i;

	//	Check to see if any devices can prevent the destruction

	for (i = 0; i < m_Devices.GetCount(); i++)
		if (!m_Devices[i].IsEmpty())
			{
			if (!m_Devices[i].OnDestroyCheck(pObj, iCause, Attacker))
				return false;
			}

	return true;
	}

void CDeviceSystem::ReadFromStream (SLoadCtx &Ctx, CSpaceObject *pObj)

//	ReadFromStream
//
//	Reads all the devices.

	{
	int i;
	DWORD dwLoad;

	//	Load count

	int iCount;
	Ctx.pStream->Read(iCount);

	//	Load the devices

	m_Devices.InsertEmpty(iCount);
	for (i = 0; i < iCount; i++)
		{
		m_Devices[i].ReadFromStream(pObj, Ctx);

		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		if (dwLoad != 0xffffffff)
			m_NamedDevices[dwLoad] = i;

		if (Ctx.dwVersion < 29)
			m_Devices[i].SetDeviceSlot(i);
		}
	}

void CDeviceSystem::ReadyFirstMissile (CSpaceObject *pObj)

//	ReadyFirstMissile
//
//	Selects the first missile

	{
	int i;

	CInstalledDevice *pDevice = GetNamedDevice(devMissileWeapon);
	if (pDevice)
		{
		pDevice->SelectFirstVariant(pObj);

		//	If we have any linked missile launchers, then select them also.

		for (i = 0; i < m_Devices.GetCount(); i++)
			{
			CInstalledDevice &LinkedDevice = GetDevice(i);
			CItemCtx Ctx(pObj, &LinkedDevice);

			if (!LinkedDevice.IsEmpty()
					&& &LinkedDevice != pDevice
					&& LinkedDevice.GetClass() == pDevice->GetClass()
					&& LinkedDevice.IsLinkedFire(Ctx, itemcatLauncher))
				LinkedDevice.SelectFirstVariant(pObj);
			}
		}
	}

void CDeviceSystem::ReadyFirstWeapon (CSpaceObject *pObj)

//	ReadyFirstWeapon
//
//	Selects the first weapon.

	{
	int iNextWeapon = FindNextIndex(pObj, -1, itemcatWeapon);
	if (iNextWeapon != -1)
		{
		m_NamedDevices[devPrimaryWeapon] = iNextWeapon;

		CInstalledDevice *pDevice = GetNamedDevice(devPrimaryWeapon);
		CDeviceClass *pClass = pDevice->GetClass();
		pClass->ValidateSelectedVariant(pObj, pDevice);
		}
	}

void CDeviceSystem::ReadyNextLauncher(CSpaceObject *pObj, int iDir)

//	ReadyNextLauncher
//
//	Select the next launcher.

	{
	int iNextWeapon = FindNextIndex(pObj, m_NamedDevices[devMissileWeapon], itemcatLauncher, iDir, true);
	if (iNextWeapon != -1)
		{
		m_NamedDevices[devMissileWeapon] = iNextWeapon;

		CInstalledDevice *pDevice = GetNamedDevice(devMissileWeapon);
		CDeviceClass *pClass = pDevice->GetClass();
		pClass->ValidateSelectedVariant(pObj, pDevice);
		}
	}

void CDeviceSystem::ReadyNextMissile (CSpaceObject *pObj, int iDir, bool bUsedLastAmmo)

//	ReadyNextMissile
//
//	Selects the next missile

	{
	bool lastSelected;
	bool firstSelected;
	CInstalledDevice *pDevice = GetNamedDevice(devMissileWeapon);
	if (pDevice)
		{
		pDevice->SelectNextVariant(pObj, iDir);
		lastSelected = pDevice->IsLastVariantSelected(pObj);
		firstSelected = pDevice->IsFirstVariantSelected(pObj);

		//	If we have any linked missile launchers, then select them also
		//  TODO: May need to change for SelectedFire launchers, as well as the fire command for SelectedFire launchers...

		for (int i = 0; i < m_Devices.GetCount(); i++)
			{
			CInstalledDevice &LinkedDevice = GetDevice(i);
			CItemCtx Ctx(pObj, &LinkedDevice);

			if (!LinkedDevice.IsEmpty()
					&& &LinkedDevice != pDevice
					&& LinkedDevice.GetClass() == pDevice->GetClass()
					&& LinkedDevice.IsLinkedFire(Ctx, itemcatLauncher))
				LinkedDevice.SelectNextVariant(pObj, iDir);
			}
		}
	
	//  If the last variant is selected, then select the previous missile launcher.
	//  Don't forget to also select the first (or last) missile, too.
	//  Note, we select the next missile launcher if the FIRST variant is selected, because we
	//  were on the last variant before running this function (which moved us to the first one).

	if (bUsedLastAmmo ? pDevice->GetValidVariantCount(pObj) == 0 : true)
		{
		bool bselectPrevLauncher = (lastSelected && (iDir == 0));
		bool bselectNextLauncher = ((firstSelected && (iDir == 1)) || (pDevice == NULL));
		
		if ((bselectPrevLauncher || bselectNextLauncher) && (pObj->GetCategory() == CSpaceObject::catShip) && (pObj->IsPlayer()))
			pObj->AsShip()->SetWeaponTriggered(devMissileWeapon, false);

		if (bselectNextLauncher)
			ReadyNextLauncher(pObj, 1);
		else if (bselectPrevLauncher)
			ReadyNextLauncher(pObj, 0);

		}

	}

void CDeviceSystem::ReadyNextWeapon (CSpaceObject *pObj, int iDir)

//	ReadyNextWeapon
//
//	Select the next weapon.

	{
	int iNextWeapon = FindNextIndex(pObj, m_NamedDevices[devPrimaryWeapon], itemcatWeapon, iDir);
	if (iNextWeapon != -1)
		{
		m_NamedDevices[devPrimaryWeapon] = iNextWeapon;

		CInstalledDevice *pDevice = GetNamedDevice(devPrimaryWeapon);
		CDeviceClass *pClass = pDevice->GetClass();
		pClass->ValidateSelectedVariant(pObj, pDevice);
		}
	}

DeviceNames CDeviceSystem::SelectWeapon (CSpaceObject *pObj, int iIndex, int iVariant)

//	SelectWeapon
//
//	Selects the given weapon to fire. Returns whether the named
//	class for this weapon

	{
	CInstalledDevice &Weapon = GetDevice(iIndex);

	if (Weapon.GetCategory() == itemcatWeapon)
		{
		m_NamedDevices[devPrimaryWeapon] = iIndex;
		return devPrimaryWeapon;
		}
	else if (Weapon.GetCategory() == itemcatLauncher)
		{
		m_NamedDevices[devMissileWeapon] = iIndex;
		ReadyFirstMissile(pObj);
		while (iVariant > 0)
			{
			ReadyNextMissile(pObj);
			iVariant--;
			}

		return devMissileWeapon;
		}
	else
		return devNone;
	}

void CDeviceSystem::SetCursorAtDevice (CItemListManipulator &ItemList, int iIndex) const

//	SetCursorAtDevice
//
//	Set the item list cursor to the item for the given device

	{
	ItemList.ResetCursor();

	const CInstalledDevice &Device = GetDevice(iIndex);
	if (Device.IsEmpty())
		return;

	while (ItemList.MoveCursorForward())
		{
		const CItem &Item = ItemList.GetItemAtCursor();
		if (Item.IsInstalled()
				&& Item.GetType() == Device.GetClass()->GetItemType()
				&& Item.GetInstalled() == iIndex)
			{
			ASSERT(Item.GetCount() == 1);
			break;
			}
		}
	}

void CDeviceSystem::SetCursorAtNamedDevice (CItemListManipulator &ItemList, DeviceNames iDev) const

//	SetCursorAtNamedDevice
//
//	Set the item list cursor to the given named device

	{
	if (m_NamedDevices[iDev] != -1)
		SetCursorAtDevice(ItemList, m_NamedDevices[iDev]);
	}

bool CDeviceSystem::Uninstall (CSpaceObject *pObj, CItemListManipulator &ItemList, ItemCategories *retiCat)

//	Uninstall
//
//	Uninstall the selected item

	{
	//	Get the item at the cursor

	const CItem &Item = ItemList.GetItemAtCursor();
	CItemType *pType = Item.GetType();

	//	Validate it

	if (pType == NULL)
		return false;

	if (!Item.IsInstalled())
		return false;

	if (pType->GetDeviceClass() == NULL)
		return false;

	//	Get the device slot that this item is at

	int iDevSlot = Item.GetInstalled();
	CInstalledDevice *pDevice = &m_Devices[iDevSlot];
	ItemCategories DevCat = pDevice->GetCategory();

	//	Clear the device

	pDevice->Uninstall(pObj, ItemList);

	//	Adjust named devices list

	switch (DevCat)
		{
		case itemcatWeapon:
			if (m_NamedDevices[devPrimaryWeapon] == iDevSlot)
				m_NamedDevices[devPrimaryWeapon] = FindNextIndex(pObj, iDevSlot, itemcatWeapon);
			break;

		case itemcatLauncher:
			if (m_NamedDevices[devMissileWeapon] == iDevSlot)
				m_NamedDevices[devMissileWeapon] = FindNextIndex(pObj, iDevSlot, itemcatLauncher);
			break;

		case itemcatShields:
			m_NamedDevices[devShields] = -1;
			break;

		case itemcatDrive:
			m_NamedDevices[devDrive] = -1;
			break;

		case itemcatCargoHold:
			m_NamedDevices[devCargo] = -1;
			break;

		case itemcatReactor:
			m_NamedDevices[devReactor] = -1;
			break;
		}

	if (retiCat)
		*retiCat = DevCat;

	//	Success!

	return true;
	}

void CDeviceSystem::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write the list of devices to the stream.

	{
	int i;

	DWORD dwSave = m_Devices.GetCount();
	pStream->Write(dwSave);

	for (i = 0; i < m_Devices.GetCount(); i++)
		{
		m_Devices[i].WriteToStream(pStream);

		DeviceNames iDev = GetNamedFromDeviceIndex(i);
		dwSave = (DWORD)((iDev == devNone) ? 0xffffffff : iDev);
		pStream->Write(dwSave);
		}
	}

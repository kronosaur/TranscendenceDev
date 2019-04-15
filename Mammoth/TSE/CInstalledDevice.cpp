//	CInstalledDevice.cpp
//
//	CInstalledDevice class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define DEVICE_ID_ATTRIB						CONSTLIT("deviceID")
#define ITEM_ATTRIB								CONSTLIT("item")

#define PROPERTY_CAPACITOR      				CONSTLIT("capacitor")
#define PROPERTY_ENABLED						CONSTLIT("enabled")
#define PROPERTY_EXTERNAL						CONSTLIT("external")
#define PROPERTY_EXTRA_POWER_USE				CONSTLIT("extraPowerUse")
#define PROPERTY_FIRE_ARC						CONSTLIT("fireArc")
#define PROPERTY_LINKED_FIRE_OPTIONS			CONSTLIT("linkedFireOptions")
#define PROPERTY_OMNIDIRECTIONAL				CONSTLIT("omnidirectional")
#define PROPERTY_POS							CONSTLIT("pos")
#define PROPERTY_SECONDARY						CONSTLIT("secondary")
#define PROPERTY_TEMPERATURE      				CONSTLIT("temperature")
#define PROPERTY_SHOT_SEPARATION_SCALE			CONSTLIT("shotSeparationScale")

//	CInstalledDevice class

CInstalledDevice::CInstalledDevice (void) : 
		m_pItem(NULL),
		m_pOverlay(NULL),
		m_dwTargetID(0),
		m_iDeviceSlot(-1),
		m_iPosAngle(0),
		m_iPosRadius(0),
		m_iPosZ(0),
		m_iMinFireArc(0),
		m_iMaxFireArc(0),
		m_iShotSeparationScale(32767),

		m_iTimeUntilReady(0),
		m_iFireAngle(0),
		m_iTemperature(0),
		m_iActivateDelay(0),
		m_iExtraPowerUse(0),
		m_iSlotPosIndex(-1),

		m_fOmniDirectional(false),
		m_fSecondaryWeapon(false),
		m_fTriggered(false),
		m_fLastActivateSuccessful(false),
		m_f3DPosition(false),

		m_fLinkedFireAlways(false),
		m_fLinkedFireTarget(false),
		m_fLinkedFireEnemy(false),
		m_fDuplicate(false),
		m_fCannotBeEmpty(false),
		m_fFateDamaged(false),
		m_fFateDestroyed(false),
		m_fFateSurvives(false),
		m_fFateComponetized(false)
	{
	}

bool CInstalledDevice::AccumulateSlotEnhancements (CSpaceObject *pSource, TArray<CString> &EnhancementIDs, CItemEnhancementStack *pEnhancements) const

//	AccumulateSlotEnhancements
//
//	Accumulates enhancements confered by the slot itself.

	{
	bool bEnhanced = false;

	//	Slot enhancements

	if (!m_SlotEnhancements.IsEmpty())
		bEnhanced = m_SlotEnhancements.Accumulate(CItemCtx(pSource, const_cast<CInstalledDevice *>(this)), *GetItem(), EnhancementIDs, pEnhancements);

	return bEnhanced;
	}

int CInstalledDevice::CalcPowerUsed (SUpdateCtx &Ctx, CSpaceObject *pSource)

//	CalcPowerUsed
//
//	Calculates how much power this device used this tick. Positive numbers are
//	consumption; negative numbers are power generation.

	{
	if (IsEmpty()) 
		return 0;

	//	Let the device compute power used. 

	int iPower = m_pClass->CalcPowerUsed(Ctx, this, pSource);

	//	Add extra power used

	iPower += m_iExtraPowerUse;

	//	Done

	return iPower;
	}

void CInstalledDevice::FinishInstall (CSpaceObject *pSource)

//	FinishInstall
//
//	Finishes the parts of the install that fire events
//	(At object creation time these events fire after the entire object
//	is created).

	{
	DEBUG_TRY

	m_pItem->FireOnInstall(pSource);
	m_pItem->FireOnEnabled(pSource);

	CShip *pShip = pSource->AsShip();
	if (pShip)
		pShip->GetController()->OnItemInstalled(*m_pItem);

	//	If necessary create an overlay for this device

	COverlayType *pOverlayType;
	pOverlayType = m_pClass->FireGetOverlayType(CItemCtx(pSource, this));

	//	Add it

	if (pOverlayType)
		{
		DWORD dwID;
		pSource->AddOverlay(pOverlayType, GetPosAngle(), GetPosRadius(), 0, 0, -1, &dwID);

		COverlay *pOverlay = pSource->GetOverlay(dwID);
		if (pOverlay)
			{
			pOverlay->SetDevice(GetDeviceSlot());
			SetOverlay(pOverlay);
			}
		}

	DEBUG_CATCH
	}

int CInstalledDevice::GetActivateDelay (CSpaceObject *pSource) const

//	GetActivateDelay
//
//	Returns the number of ticks to wait for activation
	
	{
	return m_iActivateDelay;
	}

CString CInstalledDevice::GetEnhancedDesc (CSpaceObject *pSource, const CItem *pItem)

//	GetEnhancedDesc
//
//	Returns description of the enhancement

	{
	CItemCtx ItemCtx(pSource, this);

	TArray<SDisplayAttribute> Attribs;
	if (!ItemCtx.GetEnhancementDisplayAttributes(&Attribs))
		return NULL_STR;

	CString sResult = Attribs[0].sText;
	for (int i = 1; i < Attribs.GetCount(); i++)
		{
		sResult.Append(CONSTLIT(" "));
		sResult.Append(Attribs[i].sText);
		}

	return sResult;
	}

ItemFates CInstalledDevice::GetFate (void) const

//	GetFate
//
//	Returns the current fate option for the device slot.

	{
	if (m_fFateDamaged)
		return fateDamaged;
	else if (m_fFateDestroyed)
		return fateDestroyed;
	else if (m_fFateSurvives)
		return fateSurvives;
	else if (m_fFateComponetized)
		return fateComponetized;
	else
		return fateNone;
	}

int CInstalledDevice::GetHitPointsPercent (CSpaceObject *pSource)

//	GetHitPointsPercent
//
//	Returns the integrity of the device (usually a shield) as a percent of max hp.

	{
	int iHP;
	int iMaxHP;
	GetStatus(pSource, &iHP, &iMaxHP);

	if (iMaxHP <= 0)
		return -1;
	else if (iMaxHP <= iHP)
		return 100;

	return ((1000 * iHP / iMaxHP) + 5) / 10;
	}

CSpaceObject *CInstalledDevice::GetLastShot (CSpaceObject *pSource, int iIndex) const

//	GetLastShot
//
//	Returns the last projectile object for this index.

	{
	if (pSource 
			&& iIndex < m_LastShotIDs.GetCount()
			&& m_LastShotIDs[iIndex] != 0)
		return pSource->GetSystem()->FindObject(m_LastShotIDs[iIndex]);
	else
		return NULL;
	}

DWORD CInstalledDevice::GetLinkedFireOptions (void) const

//	GetLinkedFireOptions
//
//	Returns linked-fire options for the device slot.

	{
	if (m_fLinkedFireAlways)
		return CDeviceClass::lkfAlways;
	else if (m_fLinkedFireTarget)
		return CDeviceClass::lkfTargetInRange;
	else if (m_fLinkedFireEnemy)
		return CDeviceClass::lkfEnemyInRange;
	else if (m_fSecondaryWeapon)
		return CDeviceClass::lkfEnemyInRange;
	else
		return 0;
	}

CVector CInstalledDevice::GetPos (CSpaceObject *pSource)

//	GetPos
//
//	Returns the position of the device

	{
	if (m_f3DPosition)
		{
		int iScale = pSource->GetImageScale();

		CVector vOffset;
		C3DConversion::CalcCoord(iScale, pSource->GetRotation() + m_iPosAngle, m_iPosRadius, m_iPosZ, &vOffset);

		return pSource->GetPos() + vOffset;
		}
	else if (m_iPosRadius)
		{
		return pSource->GetPos() 
				+ PolarToVector((m_iPosAngle + pSource->GetRotation()) % 360,
					m_iPosRadius * g_KlicksPerPixel);
		}
	else
		return pSource->GetPos();
	}

CVector CInstalledDevice::GetPosOffset (CSpaceObject *pSource)

//	GetPosOffset
//
//	Returns the position of the device

	{
	if (m_f3DPosition)
		{
		int iScale = pSource->GetImageScale();

		CVector vOffset;
		C3DConversion::CalcCoord(iScale, 90 + m_iPosAngle, m_iPosRadius, m_iPosZ, &vOffset);

		return vOffset;
		}
	else if (m_iPosRadius)
		{
		return PolarToVector((m_iPosAngle + 90) % 360,
					m_iPosRadius * g_KlicksPerPixel);
		}
	else
		return CVector();
	}

void CInstalledDevice::InitFromDesc (const SDeviceDesc &Desc)

//	InitFromDesc
//
//	Initializes from a desc

	{
	m_sID = Desc.sID;

	m_fOmniDirectional = Desc.bOmnidirectional;
	m_iMinFireArc = Desc.iMinFireArc;
	m_iMaxFireArc = Desc.iMaxFireArc;

	m_iPosAngle = Desc.iPosAngle;
	m_iPosRadius = Desc.iPosRadius;
	m_f3DPosition = Desc.b3DPosition;
	m_iPosZ = Desc.iPosZ;
	m_fExternal = Desc.bExternal;
	m_fCannotBeEmpty = Desc.bCannotBeEmpty;
	m_iShotSeparationScale = (unsigned int)(Desc.rShotSeparationScale * 32767);

	SetLinkedFireOptions(Desc.dwLinkedFireOptions);
	SetFate(Desc.iFate);

	m_fSecondaryWeapon = Desc.bSecondary;

	m_SlotEnhancements = Desc.Enhancements;
	if (Desc.iSlotBonus != 0)
		m_SlotEnhancements.InsertHPBonus(Desc.iSlotBonus);

	m_iSlotPosIndex = -1;
	}

ALERROR CInstalledDevice::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initializes each device slot from an XML structure

	{
	ALERROR error;

	CString sUNID = pDesc->GetAttribute(DEVICE_ID_ATTRIB);
	if (sUNID.IsBlank())
		sUNID = pDesc->GetAttribute(ITEM_ATTRIB);

	if (error = m_pClass.LoadUNID(Ctx, sUNID))
		return error;

	SDeviceDesc DeviceDesc;
	if (error = IDeviceGenerator::InitDeviceDescFromXML(Ctx, pDesc, &DeviceDesc))
		return error;

	InitFromDesc(DeviceDesc);

	return NOERROR;
	}

void CInstalledDevice::Install (CSpaceObject *pObj, CItemListManipulator &ItemList, int iDeviceSlot, bool bInCreate)

//	Install
//
//	Installs a new device of the given class

	{
	DEBUG_TRY

	const CItem &Item = ItemList.GetItemAtCursor();

	m_pClass.Set(Item.GetType()->GetDeviceClass());
	m_iDeviceSlot = iDeviceSlot;
	m_iSlotPosIndex = -1;
	m_pOverlay = NULL;
	m_dwData = 0;
	m_iExtraPowerUse = 0;
	m_iTemperature = 0;
	m_fWaiting = false;
	m_fEnabled = true;
	m_fTriggered = false;
	m_fRegenerating = false;
	m_fLastActivateSuccessful = false;
	m_fDuplicate = false;

	//	Call the class

	m_pClass->OnInstall(this, pObj, ItemList);

	//	Mark the item as installed

	ItemList.SetInstalledAtCursor(iDeviceSlot);

	//	We remember the item after it is installed;
	//	otherwise, we get a pointer to the wrong item

	m_pItem = ItemList.GetItemPointerAtCursor();
	ASSERT(m_pItem);

	//	Select the variant. We need to do this AFTER m_pItem is set because
	//	we need to check things like charges.

	m_pClass->SelectFirstVariant(pObj, this);

	//	Default to basic fire delay. Callers must set the appropriate delay
	//	based on enhancements later.

	m_iActivateDelay = m_pClass->GetActivateDelay(CItemCtx(pObj, this));

	//	If we're installing a device after creation then we
	//	zero-out the device position, etc. If necessary the
	//	caller or the device can set these fields later.
	//
	//	Note: This will overwrite whatever values were set
	//	at creation time.

	if (!bInCreate)
		{
		//	Desc is initialized to defaults even if FindDeviceSlotDesc fails.

		SDeviceDesc Desc;
		pObj->FindDeviceSlotDesc(Item, &Desc);

		//	Set the device slot properties

		m_sID = Desc.sID;
		m_iPosAngle = Desc.iPosAngle;
		m_iPosRadius = Desc.iPosRadius;
		m_iPosZ = Desc.iPosZ;
		m_f3DPosition = Desc.b3DPosition;
		m_fCannotBeEmpty = Desc.bCannotBeEmpty;
		m_iShotSeparationScale = (unsigned int)(Desc.rShotSeparationScale * 32767);

		SetFate(Desc.iFate);

		m_fExternal = (Desc.bExternal || m_pClass->IsExternal());

		m_fOmniDirectional = Desc.bOmnidirectional;
		m_iMinFireArc = Desc.iMinFireArc;
		m_iMaxFireArc = Desc.iMaxFireArc;

		SetLinkedFireOptions(Desc.dwLinkedFireOptions);
		m_fSecondaryWeapon = Desc.bSecondary;

		m_SlotEnhancements = Desc.Enhancements;
		if (Desc.iSlotBonus != 0)
			m_SlotEnhancements.InsertHPBonus(Desc.iSlotBonus);
		}

	//	Event (when creating a ship we wait until the
	//	whole ship is created before firing the event)

	if (!bInCreate)
		FinishInstall(pObj);

	DEBUG_CATCH
	}

bool CInstalledDevice::IsLinkedFire (CItemCtx &Ctx, ItemCategories iTriggerCat) const

//	IsLinkedFire
//
//	Returns TRUE if we're linked to weapon trigger

	{
	DWORD dwOptions = GetClass()->GetLinkedFireOptions(Ctx);
	if (dwOptions == 0)
		return false;
	else if (iTriggerCat == itemcatNone)
		return true;
	else
		return (GetClass()->GetCategory() == iTriggerCat);
	}

bool CInstalledDevice::IsSelectable (CItemCtx &Ctx) const

//	IsSelectable
//
//	Returns TRUE if device can be selected as a primary weapon or launcher.

	{
	return (!IsSecondaryWeapon()
			&& GetClass()->GetLinkedFireOptions(Ctx) == 0);
	}

ALERROR CInstalledDevice::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Done loading all design elements

	{
	ALERROR error;

	if (error = m_pClass.Bind(Ctx))
		return error;

	return NOERROR;
	}

void CInstalledDevice::PaintDevicePos (const SDeviceDesc &Device, CG32bitImage &Dest, int x, int y, int iScale, int iRotation)

//	PaintDevicePos
//
//	Paints the position of the device.

	{
	const int ARC_RADIUS = 30;
	CG32bitPixel RGB_LINE = CG32bitPixel(255, 255, 0);
	CG32bitPixel RGB_ARC = CG32bitPixel(RGB_LINE, 128);

	CDeviceClass *pDeviceClass = Device.Item.GetType()->GetDeviceClass();
	if (pDeviceClass == NULL)
		return;

	//	If this is a weapon, then we can take some settings from the weapon.

	bool bWeaponIsOmnidirectional = false;
	int iWeaponMinFireArc = -1;
	int iWeaponMaxFireArc = -1;
	CWeaponClass *pWeapon = pDeviceClass->AsWeaponClass();
	if (pWeapon)
		{
		switch (pWeapon->GetRotationType(CItemCtx(), &iWeaponMinFireArc, &iWeaponMaxFireArc))
			{
			case CDeviceClass::rotOmnidirectional:
				bWeaponIsOmnidirectional = true;
				break;

			case CDeviceClass::rotSwivel:
				break;

			default:
				iWeaponMinFireArc = -1;
				iWeaponMaxFireArc = -1;
				break;
			}
		}

	//	Compute the position of the device, accounting for rotation.

	int xPos;
	int yPos;
	if (Device.b3DPosition)
		C3DConversion::CalcCoord(iScale, iRotation + Device.iPosAngle, Device.iPosRadius, Device.iPosZ, &xPos, &yPos);
	else
		C3DConversion::CalcCoordCompatible(iRotation + Device.iPosAngle, Device.iPosRadius, &xPos, &yPos);

	//	Offset from where we want the center of the source to paint.

	xPos += x;
	yPos += y;

	//	Draw the center

	Dest.DrawDot(xPos, yPos, RGB_LINE, markerMediumCross);

	//	Draw fire arc

	if (!Device.bOmnidirectional && !bWeaponIsOmnidirectional)
		{
		int iMinFireArc;
		int iMaxFireArc;

		if (iWeaponMinFireArc != -1)
			{
			iMinFireArc = AngleMod(iRotation + iWeaponMinFireArc);
			iMaxFireArc = AngleMod(iRotation + iWeaponMaxFireArc);
			}
		else
			{
			iMinFireArc = AngleMod(iRotation + Device.iMinFireArc);
			iMaxFireArc = AngleMod(iRotation + Device.iMaxFireArc);
			}

		if (iMinFireArc != iMaxFireArc)
			CGDraw::Arc(Dest, CVector(xPos, yPos), ARC_RADIUS, mathDegreesToRadians(iMinFireArc), mathDegreesToRadians(iMaxFireArc), ARC_RADIUS / 2, RGB_ARC);

		int iFireAngle = AngleMiddle(iMinFireArc, iMaxFireArc);
		CVector vDir = PolarToVector(iFireAngle, ARC_RADIUS);
		CGDraw::Line(Dest, xPos, yPos, xPos + (int)vDir.GetX(), yPos - (int)vDir.GetY(), 1, RGB_LINE);
		}
	}

void CInstalledDevice::ReadFromStream (CSpaceObject *pSource, SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read object from stream
//
//	CString		m_sID
//	DWORD		device: class UNID (0xffffffff if not installed)
//	DWORD		device: m_dwTargetID
//	DWORD		device: m_dwData
//
//	DWORD		device: DWORD m_LastShotIDs count
//	DWORD		device: DWORD ID
//
//	DWORD		device: low = m_iPosAngle; hi = m_iPosRadius
//	DWORD		device: low = m_iMinFireArc; hi = m_iMaxFireArc
//	DWORD		device: low = m_iTimeUntilReady; hi = m_iFireAngle
//	DWORD		device: low = m_iSlotPosIndex; hi = m_iTemperature
//	DWORD		device: low = m_iSlotBonus; hi = m_iDeviceSlot
//	DWORD		device: low = m_iActivateDelay; hi = m_iPosZ
//	DWORD		device: low = m_iShotSeparationScale; hi = UNUSED
//	DWORD		device: flags
//
//	CItemEnhancementStack

	{
	int i;
	DWORD dwLoad;

	//	ID

	if (Ctx.dwVersion >= 157)
		m_sID.ReadFromStream(Ctx.pStream);

	//	Class

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	if (dwLoad == 0xffffffff)
		return;

	m_pClass.Set(g_pUniverse->FindDeviceClass(dwLoad));

	//	Other data

	if (Ctx.dwVersion >= 66)
		Ctx.pStream->Read((char *)&m_dwTargetID, sizeof(DWORD));

	Ctx.pStream->Read((char *)&m_dwData, sizeof(DWORD));

	//	Last shots

	if (Ctx.dwVersion >= 139)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_LastShotIDs.InsertEmpty(dwLoad);
		for (i = 0; i < (int)dwLoad; i++)
			Ctx.pStream->Read((char *)&m_LastShotIDs[i], sizeof(DWORD));
		}

	//	In 1.08 we changed how we store alternating and repeating counters.

	if (Ctx.dwVersion < 75 && m_pClass && m_pClass->AsWeaponClass())
		m_dwData = (m_dwData & 0xFFFF0000);

	//	Flags

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iPosAngle = (int)LOWORD(dwLoad);
	m_iPosRadius = (int)HIWORD(dwLoad);

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iMinFireArc = (int)LOWORD(dwLoad);
	m_iMaxFireArc = (int)HIWORD(dwLoad);

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iTimeUntilReady = (int)LOWORD(dwLoad);
	m_iFireAngle = (int)HIWORD(dwLoad);

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iTemperature = (int)HIWORD(dwLoad);

	if (Ctx.dwVersion < 92)
		{
		int iBonus = (int)LOWORD(dwLoad);
		if (iBonus != 0)
			{
			m_pEnhancements.TakeHandoff(new CItemEnhancementStack);
			m_pEnhancements->InsertHPBonus(iBonus);
			}
		m_iSlotPosIndex = -1;
		}
	else if (Ctx.dwVersion < 106)
		m_iSlotPosIndex = -1;
	else
		m_iSlotPosIndex = (int)LOWORD(dwLoad);

	//	In version 159, we replace slot bonus with extra power variable

	int iSlotBonus = 0;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

	if (Ctx.dwVersion >= 159)
		m_iExtraPowerUse = (int)LOWORD(dwLoad);
	else
		{
		m_iExtraPowerUse = 0;
		iSlotBonus = (int)LOWORD(dwLoad);
		}

	if (Ctx.dwVersion >= 29)
		m_iDeviceSlot = (int)HIWORD(dwLoad);
	else
		m_iDeviceSlot = -1;

	if (Ctx.dwVersion >= 44)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_iActivateDelay = (int)LOWORD(dwLoad);
		m_iPosZ = (int)HIWORD(dwLoad);
		}
	else
		{
		m_iActivateDelay = 8;
		m_iPosZ = 0;
		}

	//	In newer versions we store an activation delay instead of an adjustment

	if (Ctx.dwVersion < 93)
		m_iActivateDelay = m_iActivateDelay * m_pClass->GetActivateDelay(CItemCtx(pSource, this)) / 100;

	//	We no longer store mods in the device structure

	if (Ctx.dwVersion < 58)
		{
		CItemEnhancement Dummy;
		Dummy.ReadFromStream(Ctx);
		}

	if (Ctx.dwVersion >= 172)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_iShotSeparationScale = (int)LOWORD(dwLoad);
		}
	else
		{
		m_iShotSeparationScale = 32767;
		}

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_fOmniDirectional =	((dwLoad & 0x00000001) ? true : false);
	m_f3DPosition =			(((dwLoad & 0x00000002) ? true : false) && (Ctx.dwVersion >= 73));
	m_fFateSurvives =		(((dwLoad & 0x00000004) ? true : false) && (Ctx.dwVersion >= 58));
	m_fOverdrive =			((dwLoad & 0x00000008) ? true : false);
	m_fOptimized =			((dwLoad & 0x00000010) ? true : false);
	m_fSecondaryWeapon =	((dwLoad & 0x00000020) ? true : false);
	m_fFateDamaged =		(((dwLoad & 0x00000040) ? true : false) && (Ctx.dwVersion >= 58));
	m_fEnabled =			((dwLoad & 0x00000080) ? true : false);
	m_fWaiting =			((dwLoad & 0x00000100) ? true : false);
	m_fTriggered =			((dwLoad & 0x00000200) ? true : false);
	m_fRegenerating =		((dwLoad & 0x00000400) ? true : false);
	m_fLastActivateSuccessful = ((dwLoad & 0x00000800) ? true : false);

	m_fLinkedFireAlways =	((dwLoad & 0x00001000) ? true : false);
	m_fLinkedFireTarget =	((dwLoad & 0x00002000) ? true : false);
	m_fLinkedFireEnemy =	((dwLoad & 0x00004000) ? true : false);
	m_fExternal =			((dwLoad & 0x00008000) ? true : false);
	m_fDuplicate =			((dwLoad & 0x00010000) ? true : false);
	m_fCannotBeEmpty =		((dwLoad & 0x00020000) ? true : false);
	m_fFateDestroyed =		((dwLoad & 0x00040000) ? true : false);
	m_fFateComponetized =	((dwLoad & 0x00080000) ? true : false);
	bool bSlotEnhancements =((dwLoad & 0x00100000) ? true : false);

	//	Previous versions did not save this flag

	if (Ctx.dwVersion < 107)
		m_fExternal = m_pClass->IsExternal();

	//	Fix up the item pointer (but only if it is installed)

	m_pItem = NULL;
	if (m_pClass != NULL && m_iDeviceSlot != -1)
		{
		CItemListManipulator ItemList(pSource->GetItemList());
		pSource->SetCursorAtDevice(ItemList, this);
		if (ItemList.IsCursorValid())
			m_pItem = ItemList.GetItemPointerAtCursor();

		//	In previous versions we automatically offset weapon positions.
		//	In later versions we explicitly set the position, so we have
		//	to do so here.

		if (Ctx.dwVersion < 62)
			{
			if (m_iPosRadius == 0
					&& (m_pClass->GetCategory() == itemcatWeapon || m_pClass->GetCategory() == itemcatLauncher))
				{
				m_iPosAngle = 0;
				m_iPosRadius = 20;
				m_iPosZ = 0;
				m_f3DPosition = false;
				}
			}
		}

	//	Enhancement stack

	if (Ctx.dwVersion >= 92)
		m_pEnhancements = CItemEnhancementStack::ReadFromStream(Ctx);

	if (bSlotEnhancements)
		m_SlotEnhancements.ReadFromStream(Ctx);

	if (iSlotBonus != 0)
		m_SlotEnhancements.InsertHPBonus(iSlotBonus);
	}

int CInstalledDevice::IncCharges (CSpaceObject *pSource, int iChange)

//	IncCharges
//
//	Increments charges

	{
	CShip *pShip = pSource->AsShip();
	if (pShip == NULL)
		return -1;

	CItemListManipulator ItemList(pSource->GetItemList());
	pShip->SetCursorAtDevice(ItemList, m_iDeviceSlot);
	pShip->RechargeItem(ItemList, iChange);

	return ItemList.GetItemAtCursor().GetCharges();
	}

void CInstalledDevice::SetCharges (CSpaceObject *pSource, int iCharges)

//	SetCharges
//
//	Sets charges to this value.

	{
	CShip *pShip = pSource->AsShip();
	if (pShip == NULL)
		return;

	CItemListManipulator ItemList(pSource->GetItemList());
	pShip->SetCursorAtDevice(ItemList, m_iDeviceSlot);
	pShip->RechargeItem(ItemList, iCharges - ItemList.GetItemAtCursor().GetCharges());
	}

bool CInstalledDevice::SetEnabled (CSpaceObject *pSource, bool bEnabled)

//	SetEnabled
//
//	Enable/disable device. Returns TRUE if the enabled status changed.
	
	{
	DEBUG_TRY

	if (m_fEnabled == bEnabled)
		return false;

	m_fEnabled = bEnabled;

	//	Fire event

	CItem *pItem = GetItem();
	if (pItem)
		{
		if (bEnabled)
			pItem->FireOnEnabled(pSource);
		else
			pItem->FireOnDisabled(pSource);
		}

	//	Done

	return true;

	DEBUG_CATCH
	}

void CInstalledDevice::SetEnhancements (const TSharedPtr<CItemEnhancementStack> &pStack)

//	SetEnhancements
//
//	Sets the enhancements stack. NOTE: We take ownership of the stack.

	{
	if (pStack && !pStack->IsEmpty())
		m_pEnhancements = pStack;
	else
		m_pEnhancements.Delete();
	}

void CInstalledDevice::SetFate (ItemFates iFate)

//	SetFate
//
//	Sets the fate of the device when the ship is destroyed.

	{
	//	Clear out all the flats

	m_fFateDamaged = false;
	m_fFateDestroyed = false;
	m_fFateSurvives = false;
	m_fFateComponetized = false;

	//	Now set the flags appropriately

	switch (iFate)
		{
		case fateComponetized:
			m_fFateComponetized = true;
			break;

		case fateDamaged:
			m_fFateDamaged = true;
			break;

		case fateDestroyed:
			m_fFateDestroyed = true;
			break;

		case fateSurvives:
			m_fFateSurvives = true;
			break;

		default:
			//	Default.
			break;
		}
	}

void CInstalledDevice::SetLastShot (CSpaceObject *pObj, int iIndex)

//	SetLastShot
//
//	Remembers the last projectile object for this index.

	{
	if (iIndex < m_LastShotIDs.GetCount())
		m_LastShotIDs[iIndex] = (pObj ? pObj->GetID() : 0);
	}

void CInstalledDevice::SetLastShotCount (int iCount)

//	SetLastShotCount
//
//	Sets the number of shots

	{
	int i;

	m_LastShotIDs.DeleteAll();

	m_LastShotIDs.InsertEmpty(iCount);
	for (i = 0; i < iCount; i++)
		m_LastShotIDs[i] = 0;
	}

void CInstalledDevice::SetLinkedFireOptions (DWORD dwOptions)

//	SetLinkedFireOptions
//
//	Sets linked fire options

	{
	m_fLinkedFireAlways = false;
	m_fLinkedFireTarget = false;
	m_fLinkedFireEnemy = false;
	if (dwOptions & CDeviceClass::lkfAlways)
		m_fLinkedFireAlways = true;
	else if (dwOptions & CDeviceClass::lkfTargetInRange)
		m_fLinkedFireTarget = true;
	else if (dwOptions & CDeviceClass::lkfEnemyInRange)
		m_fLinkedFireEnemy = true;
	}

bool CInstalledDevice::SetProperty (CItemCtx &Ctx, const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets a property for an installed device. Returns FALSE if we could not set
//	the property for some reason.

	{
	if (IsEmpty())
		{
		if (retsError) *retsError = CONSTLIT("No device installed.");
		return false;
		}

	//	Figure out what to set

    if (strEquals(sName, PROPERTY_CAPACITOR))
        {
        CSpaceObject *pSource = Ctx.GetSource();
        if (!m_pClass->SetCounter(this, pSource, CDeviceClass::cntCapacitor, pValue->GetIntegerValue()))
            {
            if (retsError) *retsError = CONSTLIT("Unable to set capacitor value.");
            return false;
            }
        }
	else if (strEquals(sName, PROPERTY_EXTERNAL))
		{
		bool bSetExternal = (pValue && !pValue->IsNil());
		if (IsExternal() != bSetExternal)
			{
			//	If the class is external and we don't want it to be external, then
			//	we cannot comply.

			if (m_pClass->IsExternal() && !bSetExternal)
				{
				if (retsError) *retsError = CONSTLIT("Device is natively external and cannot be made internal.");
				return false;
				}

			SetExternal(bSetExternal);
			}
		}

	else if (strEquals(sName, PROPERTY_EXTRA_POWER_USE))
		{
		m_iExtraPowerUse = pValue->GetIntegerValue();
		}

	else if (strEquals(sName, PROPERTY_FIRE_ARC))
		{
		//	A value of nil means no fire arc (and no omni)

		if (pValue == NULL || pValue->IsNil())
			{
			SetOmniDirectional(false);
			SetFireArc(0, 0);
			}

		//	A value of "omnidirectional" counts

		else if (strEquals(pValue->GetStringValue(), PROPERTY_OMNIDIRECTIONAL))
			{
			SetOmniDirectional(true);
			SetFireArc(0, 0);
			}

		//	A single value means that we just point in a direction

		else if (pValue->GetCount() == 1)
			{
			int iMinFireArc = AngleMod(pValue->GetElement(0)->GetIntegerValue());
			SetOmniDirectional(false);
			SetFireArc(iMinFireArc, iMinFireArc);
			}

		//	Otherwise we expect a list with two elements

		else if (pValue->GetCount() >= 2)
			{
			int iMinFireArc = AngleMod(pValue->GetElement(0)->GetIntegerValue());
			int iMaxFireArc = AngleMod(pValue->GetElement(1)->GetIntegerValue());
			SetOmniDirectional(false);
			SetFireArc(iMinFireArc, iMaxFireArc);
			}

		//	Invalid

		else
			{
			if (retsError) *retsError = CONSTLIT("Invalid fireArc parameter.");
			return false;
			}
		}

	else if (strEquals(sName, PROPERTY_LINKED_FIRE_OPTIONS))
		{
		//	Parse the options

		DWORD dwOptions;
		if (!::GetLinkedFireOptions(pValue, &dwOptions, retsError))
			{
			if (retsError) *retsError = CONSTLIT("Invalid linked-fire option.");
			return false;
			}

		//	Set

		SetLinkedFireOptions(dwOptions);
		}

	else if (strEquals(sName, PROPERTY_POS))
		{
		//	Get the parameters. We accept a single list parameter with angle/radius/z.
		//	(The latter is compatible with the return of objGetDevicePos.)

		int iPosAngle;
		int iPosRadius;
		int iZ;
		if (pValue == NULL || pValue->IsNil())
			{
			iPosAngle = 0;
			iPosRadius = 0;
			iZ = 0;
			}
		else if (pValue->GetCount() >= 2)
			{
			iPosAngle = pValue->GetElement(0)->GetIntegerValue();
			iPosRadius = pValue->GetElement(1)->GetIntegerValue();

			if (pValue->GetCount() >= 3)
				iZ = pValue->GetElement(2)->GetIntegerValue();
			else
				iZ = 0;
			}
		else
			{
			if (retsError) *retsError = CONSTLIT("Invalid angle and radius");
			return false;
			}

		//	Set it

		SetPosAngle(iPosAngle);
		SetPosRadius(iPosRadius);
		SetPosZ(iZ);
		}

	else if (strEquals(sName, PROPERTY_SECONDARY))
		{
		if (pValue == NULL || !pValue->IsNil())
			SetSecondary(true);
		else
			SetSecondary(false);
		}

    else if (strEquals(sName, PROPERTY_TEMPERATURE))
        {
        CSpaceObject *pSource = Ctx.GetSource();
        if (!m_pClass->SetCounter(this, pSource, CDeviceClass::cntTemperature, pValue->GetIntegerValue()))
            {
            if (retsError) *retsError = CONSTLIT("Unable to set temperature value.");
            return false;
            }
        }
	else if (strEquals(sName, PROPERTY_SHOT_SEPARATION_SCALE))
		{
		double rShotSeparationScale = Clamp(pValue->GetDoubleValue(), -1.0, 1.0);
		SetShotSeparationScale(rShotSeparationScale);
		}

	//	Otherwise, let any sub-classes handle it.

	else
		return m_pClass->SetItemProperty(Ctx, sName, pValue, retsError);

	return true;
	}

void CInstalledDevice::Uninstall (CSpaceObject *pObj, CItemListManipulator &ItemList)

//	Uninstall
//
//	Uninstalls the device

	{
	CItem theItem = ItemList.GetItemAtCursor();
	int iDevSlot = theItem.GetInstalled();
	if (iDevSlot == -1)
		{
		ASSERT(false);
		return;
		}

	//	Destroy the overlay, if necessary

	if (m_pOverlay)
		{
		pObj->RemoveOverlay(m_pOverlay->GetID());
		m_pOverlay = NULL;
		}

	//	Event (we fire the event before we uninstall because once we uninstall
	//	the item might get combined with others)

	ItemList.GetItemAtCursor().FireOnDisabled(pObj);
	ItemList.GetItemAtCursor().FireOnUninstall(pObj);
	CShip *pShip = pObj->AsShip();
	if (pShip)
		pShip->GetController()->OnItemUninstalled(theItem);

	//	We need to refresh the cursor because OnUninstall might
	//	have done something

	if (!pObj->SetCursorAtDevice(ItemList, iDevSlot))
		{
		m_sID = NULL_STR;
		m_pItem = NULL;
		m_pClass.Set(NULL);
		return;
		}

	//	Mark the item as uninstalled

	ItemList.SetInstalledAtCursor(-1);

	//	Let the class clean up also

	m_pClass->OnUninstall(this, pObj, ItemList);

	//	We need to clear the ID or else it might get counted when we re-install.

	m_sID = NULL_STR;

	//	Done

	m_pItem = NULL;
	m_pClass.Set(NULL);
	m_fTriggered = false;
	m_fLastActivateSuccessful = false;
	}

void CInstalledDevice::Update (CSpaceObject *pSource, CDeviceClass::SDeviceUpdateCtx &Ctx)
	{
	DEBUG_TRY

	if (!IsEmpty()) 
		{
		m_pClass->Update(this, pSource, Ctx);

		//	Counters

		if (m_iTimeUntilReady > 0 && IsEnabled())
			{
			m_iTimeUntilReady--;

			if (m_pClass->ShowActivationDelayCounter(pSource, this))
				pSource->OnComponentChanged(comDeviceCounter);
			}

		//	Disruption

		bool bRepaired;
		if (GetDisruptedStatus(NULL, &bRepaired))
			{
			Ctx.bDisrupted = true;
			}

		//	If disruption was repaired

		if (bRepaired)
			{
			Ctx.bRepaired = true;
			pSource->OnDeviceStatus(this, CDeviceClass::statusDisruptionRepaired);
			}
		}

	DEBUG_CATCH
	}

void CInstalledDevice::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write object to stream
//
//	CString		m_sID
//	DWORD		device: class UNID (0xffffffff if not installed)
//	DWORD		device: m_dwTargetID
//	DWORD		device: m_dwLastShotID
//	DWORD		device: m_dwData
//
//	DWORD		device: DWORD m_LastShotIDs count
//	DWORD		device: DWORD ID
//
//	DWORD		device: low = m_iPosAngle; hi = m_iPosRadius
//	DWORD		device: low = m_iMinFireArc; hi = m_iMaxFireArc
//	DWORD		device: low = m_iTimeUntilReady; hi = m_iFireAngle
//	DWORD		device: low = m_iSlotIndex; hi = m_iTemperature
//	DWORD		device: low = m_iSlotBonus; hi = m_iDeviceSlot
//	DWORD		device: low = m_iActivateDelay; hi = m_iPosZ
//	DWORD		device: low = m_iShotSeparationScale; hi = UNUSED
//	DWORD		device: flags
//
//	CItemEnhancementStack
//	CEnhancementDesc

	{
	int i;
	DWORD dwSave;

	m_sID.WriteToStream(pStream);

	dwSave = (m_pClass ? m_pClass->GetUNID() : 0xffffffff);
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	if (m_pClass == NULL)
		return;

	pStream->Write((char *)&m_dwTargetID, sizeof(DWORD));
	pStream->Write((char *)&m_dwData, sizeof(DWORD));

	dwSave = m_LastShotIDs.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	for (i = 0; i < m_LastShotIDs.GetCount(); i++)
		pStream->Write((char *)&m_LastShotIDs[i], sizeof(DWORD));

	dwSave = MAKELONG(m_iPosAngle, m_iPosRadius);
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	
	dwSave = MAKELONG(m_iMinFireArc, m_iMaxFireArc);
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	
	dwSave = MAKELONG(m_iTimeUntilReady, m_iFireAngle);
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	
	dwSave = MAKELONG(m_iSlotPosIndex, m_iTemperature);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = MAKELONG(m_iExtraPowerUse, m_iDeviceSlot);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = MAKELONG(m_iActivateDelay, m_iPosZ);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = MAKELONG(m_iShotSeparationScale, 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = 0;
	dwSave |= (m_fOmniDirectional ?		0x00000001 : 0);
	dwSave |= (m_f3DPosition ?			0x00000002 : 0);
	dwSave |= (m_fFateSurvives ?		0x00000004 : 0);
	dwSave |= (m_fOverdrive ?			0x00000008 : 0);
	dwSave |= (m_fOptimized ?			0x00000010 : 0);
	dwSave |= (m_fSecondaryWeapon ?		0x00000020 : 0);
	dwSave |= (m_fFateDamaged ?			0x00000040 : 0);
	dwSave |= (m_fEnabled ?				0x00000080 : 0);
	dwSave |= (m_fWaiting ?				0x00000100 : 0);
	dwSave |= (m_fTriggered ?			0x00000200 : 0);
	dwSave |= (m_fRegenerating ?		0x00000400 : 0);
	dwSave |= (m_fLastActivateSuccessful ? 0x00000800 : 0);
	dwSave |= (m_fLinkedFireAlways ?	0x00001000 : 0);
	dwSave |= (m_fLinkedFireTarget ?	0x00002000 : 0);
	dwSave |= (m_fLinkedFireEnemy ?		0x00004000 : 0);
	dwSave |= (m_fExternal ?			0x00008000 : 0);
	dwSave |= (m_fDuplicate ?			0x00010000 : 0);
	dwSave |= (m_fCannotBeEmpty ?		0x00020000 : 0);
	dwSave |= (m_fFateDestroyed ?		0x00040000 : 0);
	dwSave |= (m_fFateComponetized ?	0x00080000 : 0);
	dwSave |= (!m_SlotEnhancements.IsEmpty() ? 0x00100000 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	CItemEnhancementStack::WriteToStream(m_pEnhancements, pStream);

	if (!m_SlotEnhancements.IsEmpty())
		m_SlotEnhancements.WriteToStream(*pStream);
	}

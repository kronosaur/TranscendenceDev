//	CRepairerClass.cpp
//
//	CRepairerClass class

#include "PreComp.h"

#define COMPARTMENT_REGEN_ATTRIB	CONSTLIT("compartmentRegen")
#define LEVEL_ATTRIB				CONSTLIT("level")
#define POWER_USE_ATTRIB			CONSTLIT("powerUse")
#define REPAIR_CYCLE_ATTRIB			CONSTLIT("repairCycle")
#define REGEN_ATTRIB				CONSTLIT("regen")

#define SCALING_TAG                 CONSTLIT("Scaling")

const CRepairerClass::SRepairerDesc CRepairerClass::m_NullDesc;

static const char *CACHED_EVENTS[CRepairerClass::evtCount] =
	{
		"GetArmorRegen",
	};

int CRepairerClass::CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource)

//	CalcPowerUsed
//
//	Computes the amount of power used by this device each tick

	{
	//	Doesn't work if not enabled

	if (!pDevice->IsEnabled())
		return 0;

	//	We've stored power consumption on the device itself

	return (int)pDevice->GetData();
	}

void CRepairerClass::CalcArmorRegen (const CDeviceItem &RepairerItem, int iSegment, int iTick, int *retiHP, int *retiPower) const

//	CalcArmorRegen
//
//	Returns the HP repaired and power consumed to repair the given segment

	{
	const SRepairerDesc &Desc = GetDesc(RepairerItem);

	//	Only works with installed devices.

	const CSpaceObject *pSource = RepairerItem.GetSource();
	if (!pSource)
		throw CException(ERR_FAIL);

	int iRegenHP = 0;
	int iPower = 0;

	//	Get the armor class for this segment

	const CArmorSystem &Armor = pSource->GetArmorSystem();
	const CInstalledArmor &Segment = Armor.GetSegment(iSegment);
	const CArmorItem ArmorItem = Segment.AsArmorItem();

	//	Compute the default regen and power consumption based on descriptor

	const CRegenDesc &ArmorRegen = GetArmorRegen(Desc, ArmorItem);
	iRegenHP = ArmorRegen.GetRegen(iTick, REPAIR_CYCLE_TIME);
	iPower = (ArmorRegen.GetHPPerEra() > 0 ? Desc.iPowerUse : 0);

	//	If we have an event to compute regen, then use it.

	SEventHandlerDesc Event;
	if (FindEventHandlerRepairerClass(EEventCache::GetArmorRegen, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());

		Ctx.DefineContainingType(GetItemType());
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineItemVar(RepairerItem);
		Ctx.DefineInteger(CONSTLIT("aArmorSeg"), iSegment);
		Ctx.DefineItem(CONSTLIT("aArmorItem"), ArmorItem);
		Ctx.DefineItemType(CONSTLIT("aArmorType"), &ArmorItem.GetType());

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			{
			iRegenHP = 0;
			iPower = 0;
			pSource->ReportEventError(CONSTLIT("GetArmorRegen"), pResult);
			}
		else if (pResult->IsInteger())
			iRegenHP = pResult->GetIntegerValue();
		else if (!pResult->IsNil())
			{
			//	We expect a struct

			ICCItem *pRegen = pResult->GetElement(CONSTLIT("regen"));
			if (pRegen)
				iRegenHP = pRegen->GetIntegerValue();

			ICCItem *pPowerUse = pResult->GetElement(CONSTLIT("powerUse"));
			if (pPowerUse)
				iPower = pPowerUse->GetIntegerValue();
			}

		Ctx.Discard(pResult);

		//	Convert to regen HP using a CRegenDesc structure

		CRegenDesc Regen;
		Regen.InitFromRegen((double)iRegenHP, REPAIR_CYCLE_TIME);
		iRegenHP = Regen.GetRegen(iTick, REPAIR_CYCLE_TIME);
		}

	//	Done

	*retiHP = iRegenHP;
	*retiPower = iPower;
	}

ALERROR CRepairerClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice)

//	CreateFromXML
//
//	Creates from an XML element

	{
	if (!pDesc || !pType)
		return ERR_FAIL;

	CRepairerClass *pDevice = new CRepairerClass;
	if (!pDevice)
		return ERR_MEMORY;

	if (ALERROR error = pDevice->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

	//  Figure out how many levels we need to allocate

	const int iBaseLevel = pType->GetLevel();
	const int iLevels = (pType->GetMaxLevel() - iBaseLevel) + 1;
	pDevice->m_Desc.InsertEmpty(iLevels);

	//	Loop over all levels that we need to initialize.

	const CXMLElement *pScaling = pDesc->GetContentElementByTag(SCALING_TAG);
	for (int i = 0; i < pDevice->m_Desc.GetCount(); i++)
		{
		int iLevel = iBaseLevel + i;

		//	Look for this level in the scaling element, if we have it.

		const CXMLElement *pLevelDesc;
		if (pScaling 
				&& (pLevelDesc = FindLevelDesc(Ctx, *pScaling, iLevel)))
			{ }

		//	Otherwise, we use the root.

		else
			pLevelDesc = pDesc;

		//	Initialize this level.

		if (!InitDescFromXML(Ctx, iLevel, *pLevelDesc, pDevice->m_Desc[i]))
			return ERR_FAIL;
		}

	//	Done

	*retpDevice = pDevice;

	return NOERROR;
	}

bool CRepairerClass::FindEventHandlerRepairerClass (EEventCache iEvent, SEventHandlerDesc *retEvent) const

//	FindEventHandlerRepairerClass
//
//	Finds the given event handler.

	{
	if (!m_CachedEvents[(int)iEvent].pCode)
		return false;

	if (retEvent)
		*retEvent = m_CachedEvents[(int)iEvent];

	return true;
	}

const CXMLElement *CRepairerClass::FindLevelDesc (SDesignLoadCtx &Ctx, const CXMLElement &Scaling, int iLevel)

//	FindLevelDesc
//
//	Looks through the scaling XML element for a child element of the given 
//	level. If not found, we return NULL.

	{
	for (int i = 0; i < Scaling.GetContentElementCount(); i++)
		{
		const CXMLElement *pChild = Scaling.GetContentElement(i);
		if (iLevel == pChild->GetAttributeInteger(LEVEL_ATTRIB))
			return pChild;
		}

	return NULL;
	}

const CRegenDesc &CRepairerClass::GetArmorRegen (const SRepairerDesc &Desc, const CArmorItem &ArmorItem) const

//	GetArmorRegen
//
//	Returns the regen stats for this particular armor segment.

	{
	//	If we only have a single entry in our array, then it means we apply it
	//	to all armor levels.

	if (Desc.ArmorRepair.GetCount() == 1)
		return Desc.ArmorRepair[0];

	//	Otherwise, it is indexed by target armor level

	else
		{
		int iArmorTech = ArmorItem.GetRepairLevel();
		if (iArmorTech >= 1 && iArmorTech - 1 < Desc.ArmorRepair.GetCount())
			return Desc.ArmorRepair[iArmorTech - 1];
		else
			return CRegenDesc::Null;
		}
	}

const CRepairerClass::SRepairerDesc &CRepairerClass::GetDesc (const CDeviceItem &RepairerItem) const

//	GetDesc
//
//	Returns the appropriate descriptor for the device.

	{
	if (m_Desc.GetCount() == 0)
		return m_NullDesc;

	int iBaseLevel = m_Desc[0].iLevel;

	//  Figure out if we want a scaled item

	int iIndex = Max(0, Min(RepairerItem.GetLevel() - iBaseLevel, m_Desc.GetCount() - 1));
	return m_Desc[iIndex];
	}

int CRepairerClass::GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse) const

//	GetPowerRating
//
//	Returns the power used by this device.

	{
	const CDeviceItem RepairerItem = Ctx.GetDeviceItem();
	const SRepairerDesc &Desc = GetDesc(RepairerItem);

	//	Idle power is always 0

	if (retiIdlePowerUse)
		*retiIdlePowerUse = 0;

	//	We make a list of all armor class types for the ship.

	TArray<CItem> Item;
	TArray<const CArmorItem> ArmorItem;
	const CSpaceObject *pSource = RepairerItem.GetSource();

	//	If we have a real ship, get the armor from it.

	if (pSource)
		{
		const auto &Armor = pSource->GetArmorSystem();
		ArmorItem.GrowToFit(Armor.GetSegmentCount());
		for (int i = 0; i < Armor.GetSegmentCount(); i++)
			ArmorItem.Insert(Armor.GetSegment(i).AsArmorItem());
		}

	//	Otherwise, see if we have a ship class

	else if (const CShipClass *pClass = Ctx.GetSourceShipClass())
		{
		const auto &Armor = pClass->GetArmorDesc();
		Item.InsertEmpty(Armor.GetCount());
		ArmorItem.GrowToFit(Armor.GetCount());
		for (int i = 0; i < Armor.GetCount(); i++)
			{
			Item[i] = Armor.GetSegment(i).GetArmorItem();
			ArmorItem.Insert(Item[i].AsArmorItem());
			}
		}

	//	Otherwise, standard rating

	else
		{
		return 2 * Desc.iPowerUse;
		}

	//	Loop over all armor segments adding up power consumption

	int iTotalPowerUse = 0;

	for (int i = 0; i < ArmorItem.GetCount(); i++)
		{
		int iPowerUse = 0;

		if (!ArmorItem[i])
			continue;

		//	First compute the standard power

		const CRegenDesc &ArmorRegen = GetArmorRegen(Desc, ArmorItem[i]);
		iPowerUse = (ArmorRegen.GetHPPerEra() > 0 ? Desc.iPowerUse : 0);

		//	Next, if we have an event, we call it to compute power

		SEventHandlerDesc Event;
		if (FindEventHandlerRepairerClass(EEventCache::GetArmorRegen, &Event))
			{
			CCodeChainCtx CCCtx(GetUniverse());

			CCCtx.DefineContainingType(GetItemType());
			CCCtx.SaveAndDefineSourceVar(pSource);
			CCCtx.SaveAndDefineItemVar(Ctx.GetItem());
			CCCtx.DefineInteger(CONSTLIT("aArmorSeg"), i);
			CCCtx.DefineItemType(CONSTLIT("aArmorType"), &ArmorItem[i].GetType());

			ICCItemPtr pResult = CCCtx.RunCode(Event);
			if (pResult->IsError())
				NULL;

			else if (pResult->IsInteger())
				NULL;

			else if (!pResult->IsNil())
				{
				//	We expect a struct

				ICCItem *pPowerUse = pResult->GetElement(CONSTLIT("powerUse"));
				if (pPowerUse)
					iPowerUse = pPowerUse->GetIntegerValue();
				}
			}

		//	Add it to the total

		iTotalPowerUse += iPowerUse;
		}

	//	Done

	return iTotalPowerUse;
	}

bool CRepairerClass::InitDescFromXML (SDesignLoadCtx &Ctx, int iLevel, const CXMLElement &DescXML, SRepairerDesc &retDesc)

//	InitDescFromXML
//
//	Initializes a descriptor from XML.

	{
	retDesc.iLevel = iLevel;
	retDesc.iPowerUse = DescXML.GetAttributeIntegerBounded(POWER_USE_ATTRIB, 0, -1, 0);

	//	Load repair attribute
	//
	//	NOTE: It's OK if we don't specify this; it probably means that we have
	//	an event to calculate it.

	CString sList;
	bool bRegen;
	if (DescXML.FindAttribute(REGEN_ATTRIB, &sList))
		bRegen = true;
	else if (DescXML.FindAttribute(REPAIR_CYCLE_ATTRIB, &sList))
		bRegen = false;
	else
		bRegen = false;

	//	Parse the list

	TArray<CString> List;
	ParseStringList(sList, 0, &List);
	for (int i = 0; i < List.GetCount(); i++)
		{
		CRegenDesc *pRegen = retDesc.ArmorRepair.Insert();
		if (bRegen)
			pRegen->InitFromRegenString(Ctx, List[i], REPAIR_CYCLE_TIME);
		else
			pRegen->InitFromRepairRateString(Ctx, List[i], REPAIR_CYCLE_TIME);
		}

	//	Compartment repair

	CString sAttrib;
	if (DescXML.FindAttribute(COMPARTMENT_REGEN_ATTRIB, &sAttrib))
		{
		if (ALERROR error = retDesc.CompartmentRepair.InitFromRegenString(Ctx, sAttrib, REPAIR_CYCLE_TIME))
			return false;
		}

	//	Success!

	return true;
	}

bool CRepairerClass::RepairShipArmor (CDeviceItem &RepairerItem, SDeviceUpdateCtx &Ctx)

//	RepairShipArmor
//
//	Repairs armor for the ship and sets power consumption. We return TRUE if armor
//	is damaged (whether or not it was repaired this tick).

	{
	CSpaceObject *pSource = RepairerItem.GetSource();
	if (!pSource)
		throw CException(ERR_FAIL);

	CShip *pShip = pSource->AsShip();
	if (!pShip)
		return false;

	auto pArmor = pSource->GetArmorSystem();
	if (!pArmor)
		return false;

	bool bNeedsRepair = false;
	int iTotalPower = 0;

	for (int i = 0; i < pArmor->GetSegmentCount(); i++)
		{
		auto &Segment = pArmor->GetSegment(i);
		if (Segment.IsDamaged())
			{
			int iHP;
			int iPowerPerSegment;

			CalcArmorRegen(RepairerItem, i, Ctx.iTick, &iHP, &iPowerPerSegment);

			//	Repair armor

			if (iHP)
				pShip->RepairArmor(i, iHP);

			//	Add to consumption

			iTotalPower += iPowerPerSegment;
			bNeedsRepair = true;
			}
		}

	//	Store the power consumption on the device itself. We will keep on
	//	consuming this amount of power per tick until we update again.

	RepairerItem.SetData(iTotalPower);

	//	Done

	return bNeedsRepair;
	}

bool CRepairerClass::RepairShipAttachedSections (CDeviceItem &RepairerItem, SDeviceUpdateCtx &Ctx)

//	RepairShipAttachedSections
//
//	Repairs attached sections. Returns TRUE if at least one section needs repairs.

	{
	CSpaceObject *pSource = RepairerItem.GetSource();
	if (!pSource)
		throw CException(ERR_FAIL);

	CShip *pShip = pSource->AsShip();
	if (!pShip)
		return false;

	TArray<CShip::SAttachedSectionInfo> Sections;
	pShip->GetAttachedSectionInfo(Sections);

	for (int i = 0; i < Sections.GetCount(); i++)
		{
		//	The first section is the ship itself

		if (Sections[i].pObj == pShip)
			continue;

		//	Repair this section; if we repaired it, then we're done (we only
		//	repair one section at a time).

		CShip *pSection = Sections[i].pObj->AsShip();
		if (pSection == NULL)
			continue;

		if (RepairShipArmor(RepairerItem, Ctx))
			return true;
		}

	//	Nothing repaired

	return false;
	}

bool CRepairerClass::RepairShipInterior (CDeviceItem &RepairerItem, SDeviceUpdateCtx &Ctx)

//	RepairShipInterior
//
//	Repair the ship interior. Returns TRUE if we repaired at least one hp.

	{
	CSpaceObject *pSource = RepairerItem.GetSource();
	if (!pSource)
		throw CException(ERR_FAIL);

	CShip *pShip = pSource->AsShip();
	if (!pShip)
		return false;

	const SRepairerDesc &Desc = GetDesc(RepairerItem);

	bool bRepairsNeeded = pShip->RepairInterior(Desc.CompartmentRepair.GetRegen(Ctx.iTick, REPAIR_CYCLE_TIME));
	int iPowerUsed = (bRepairsNeeded ? Desc.iPowerUse : 0);
	RepairerItem.SetData(iPowerUsed);

	return bRepairsNeeded;
	}

void CRepairerClass::Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx)

//	Update
//
//	Updates the device

	{
	DEBUG_TRY

	//	Short-circuit various cases

	CShip *pShip = pSource->AsShip();
	if (!pShip
			|| !pDevice
			|| (Ctx.iTick % REPAIR_CYCLE_TIME) != 0
			|| !pDevice->IsWorking())
		return;

	CDeviceItem RepairerItem = pDevice->GetDeviceItem();
	const SRepairerDesc &Desc = GetDesc(RepairerItem);

	//	Repair interior compartments first

	if (!Desc.CompartmentRepair.IsEmpty() && pShip->IsMultiHull() && RepairShipInterior(RepairerItem, Ctx))
		return;

	//	Armor is repaired next, if necessary

	if (Desc.ArmorRepair.GetCount() > 0 
			|| FindEventHandlerRepairerClass(EEventCache::GetArmorRegen))
		{
		if (RepairShipArmor(RepairerItem, Ctx))
			return;

		//	Otherwise, if we have attached objects, try to repair those.

		if (pShip->HasAttachedSections() && RepairShipAttachedSections(RepairerItem, Ctx))
			return;
		}

	DEBUG_CATCH
	}

ALERROR CRepairerClass::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Bind design

	{
	//	Load events

	CItemType *pType = GetItemType();
	pType->InitCachedEvents(evtCount, CACHED_EVENTS, m_CachedEvents);

	return NOERROR;
	}

void CRepairerClass::OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList)

//	OnInstall
//
//	Device installed

	{
	//	Identified when installed

	if (pSource->IsPlayer())
		ItemList.GetItemAtCursor().SetKnown();
	}

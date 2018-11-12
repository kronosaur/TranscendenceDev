//	CRepairerClass.cpp
//
//	CRepairerClass class

#include "PreComp.h"

#define COMPARTMENT_REGEN_ATTRIB	CONSTLIT("compartmentRegen")
#define POWER_USE_ATTRIB			CONSTLIT("powerUse")
#define REPAIR_CYCLE_ATTRIB			CONSTLIT("repairCycle")
#define REGEN_ATTRIB				CONSTLIT("regen")

#define REPAIR_CYCLE_TIME			10

static char *CACHED_EVENTS[CRepairerClass::evtCount] =
	{
		"GetArmorRegen",
	};

CRepairerClass::CRepairerClass (void)
	{
	}

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

void CRepairerClass::CalcRegen (CInstalledDevice *pDevice, CShip *pShip, int iSegment, int iTick, int *retiHP, int *retiPower)

//	CalcRegen
//
//	Returns the HP repaired and power consumed to repair the given segment

	{
	int iRegenHP = 0;
	int iPower = 0;

	//	Get the armor class for this segment

	CArmorClass *pArmorClass = pShip->GetArmorSection(iSegment)->GetClass();
	if (pArmorClass == NULL)
		{
		*retiHP = iRegenHP;
		*retiPower = iPower;
		return;
		}

	//	Compute the default regen and power consumption based on descriptor

	int iArmorTech = pArmorClass->GetRepairTech();
	if (iArmorTech - 1 < m_Repair.GetCount())
		{
		iRegenHP = m_Repair[iArmorTech - 1].GetRegen(iTick, REPAIR_CYCLE_TIME);
		iPower = (m_Repair[iArmorTech - 1].GetHPPerEra() > 0 ? m_iPowerUse : 0);
		}

	//	If we have an event to compute regen, then use it.

	SEventHandlerDesc Event;
	if (FindEventHandlerRepairerClass(evtGetArmorRegen, &Event))
		{
		CCodeChainCtx Ctx;

		Ctx.DefineContainingType(GetItemType());
		Ctx.SaveAndDefineSourceVar(pShip);
		Ctx.SaveAndDefineItemVar(pShip->GetItemForDevice(pDevice));
		Ctx.DefineInteger(CONSTLIT("aArmorSeg"), iSegment);
		Ctx.DefineItemType(CONSTLIT("aArmorType"), pArmorClass->GetItemType());

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			{
			iRegenHP = 0;
			iPower = 0;
			pShip->ReportEventError(CONSTLIT("GetArmorRegen"), pResult);
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
	ALERROR error;
	int i;
	CRepairerClass *pDevice;

	pDevice = new CRepairerClass;
	if (pDevice == NULL)
		return ERR_MEMORY;

	if (error = pDevice->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

	pDevice->m_iPowerUse = pDesc->GetAttributeInteger(POWER_USE_ATTRIB);

	//	Load repair attribute
	//
	//	NOTE: It's OK if we don't specify this; it probably means that we have
	//	an event to calculate it.

	CString sList;
	bool bRegen;
	if (pDesc->FindAttribute(REGEN_ATTRIB, &sList))
		bRegen = true;
	else if (pDesc->FindAttribute(REPAIR_CYCLE_ATTRIB, &sList))
		bRegen = false;
	else
		bRegen = false;

	//	Parse the list

	TArray<CString> List;
	ParseStringList(sList, 0, &List);
	for (i = 0; i < List.GetCount(); i++)
		{
		CRegenDesc *pRegen = pDevice->m_Repair.Insert();
		if (bRegen)
			pRegen->InitFromRegenString(Ctx, List[i], REPAIR_CYCLE_TIME);
		else
			pRegen->InitFromRepairRateString(Ctx, List[i], REPAIR_CYCLE_TIME);
		}

	//	Compartment repair

	CString sAttrib;
	if (pDesc->FindAttribute(COMPARTMENT_REGEN_ATTRIB, &sAttrib))
		{
		if (error = pDevice->m_CompartmentRepair.InitFromRegenString(Ctx, sAttrib, REPAIR_CYCLE_TIME))
			return error;
		}

	//	Done

	*retpDevice = pDevice;

	return NOERROR;
	}

int CRepairerClass::GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse) const

//	GetPowerRating
//
//	Returns the power used by this device.

	{
	//	Idle power is always 0

	if (retiIdlePowerUse) *retiIdlePowerUse = 0;

	//	We make a list of all armor class types for the ship.

	TArray<const CArmorClass *> Armor;
	CSpaceObject *pSource = Ctx.GetSource();
	CShip *pShip = (pSource ? pSource->AsShip() : NULL);
	const CShipClass *pClass;

	//	If we have a real ship, get the armor from it.

	if (pShip)
		{
		Armor.InsertEmpty(pShip->GetArmorSectionCount());
		for (int i = 0; i < pShip->GetArmorSectionCount(); i++)
			Armor[i] = pShip->GetArmorSection(i)->GetClass();
		}

	//	Otherwise, see if we have a ship class

	else if (pClass = Ctx.GetSourceShipClass())
		{
		Armor.InsertEmpty(pClass->GetArmorDesc().GetCount());
		for (int i = 0; i < pClass->GetArmorDesc().GetCount(); i++)
			Armor[i] = pClass->GetArmorDesc().GetSegment(i).GetArmorClass();
		}

	//	Otherwise, standard rating

	else
		{
		return 2 * m_iPowerUse;
		}

	//	Loop over all armor segments adding up power consumption

	int iTotalPowerUse = 0;

	for (int i = 0; i < Armor.GetCount(); i++)
		{
		int iPowerUse = 0;

		if (Armor[i] == NULL)
			continue;

		//	First compute the standard power

		int iArmorTech = Armor[i]->GetRepairTech();
		if (iArmorTech - 1 < m_Repair.GetCount())
			{
			iPowerUse = (m_Repair[iArmorTech - 1].GetHPPerEra() > 0 ? m_iPowerUse : 0);
			}

		//	Next, if we have an event, we call it to compute power

		SEventHandlerDesc Event;
		if (FindEventHandlerRepairerClass(evtGetArmorRegen, &Event))
			{
			CCodeChainCtx CCCtx;

			CCCtx.DefineContainingType(GetItemType());
			CCCtx.SaveAndDefineSourceVar(pShip);
			CCCtx.SaveAndDefineItemVar(Ctx.GetItem());
			CCCtx.DefineInteger(CONSTLIT("aArmorSeg"), i);
			CCCtx.DefineItemType(CONSTLIT("aArmorType"), Armor[i]->GetItemType());

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

bool CRepairerClass::RepairShipArmor (CInstalledDevice *pDevice, CShip *pShip, SDeviceUpdateCtx &Ctx)

//	RepairShipArmor
//
//	Repairs armor for the ship and sets power consumption. We return TRUE if armor
//	is damaged (whether or not it was repaired this tick).

	{
	bool bNeedsRepair = false;
	int iTotalPower = 0;

	for (int i = 0; i < pShip->GetArmorSectionCount(); i++)
		{
		if (pShip->IsArmorDamaged(i))
			{
			int iHP;
			int iPowerPerSegment;

			CalcRegen(pDevice, pShip, i, Ctx.iTick, &iHP, &iPowerPerSegment);

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

	pDevice->SetData(iTotalPower);

	//	Done

	return bNeedsRepair;
	}

bool CRepairerClass::RepairShipAttachedSections (CInstalledDevice *pDevice, CShip *pShip, SDeviceUpdateCtx &Ctx)

//	RepairShipAttachedSections
//
//	Repairs attached sections. Returns TRUE if at least one section needs repairs.

	{
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

		if (RepairShipArmor(pDevice, pSection, Ctx))
			return true;
		}

	//	Nothing repaired

	return false;
	}

bool CRepairerClass::RepairShipInterior (CInstalledDevice *pDevice, CShip *pShip, SDeviceUpdateCtx &Ctx)

//	RepairShipInterior
//
//	Repair the ship interior. Returns TRUE if we repaired at least one hp.

	{
	bool bRepairsNeeded = pShip->RepairInterior(m_CompartmentRepair.GetRegen(Ctx.iTick, REPAIR_CYCLE_TIME));
	int iPowerUsed = (bRepairsNeeded ? m_iPowerUse : 0);
	pDevice->SetData(iPowerUsed);

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
	if (pShip == NULL
			|| (Ctx.iTick % REPAIR_CYCLE_TIME) != 0
			|| !pDevice->IsEnabled() 
			|| pDevice->IsDamaged()
			|| pDevice->IsDisrupted())
		return;

	//	Repair interior compartments first

	if (!m_CompartmentRepair.IsEmpty() && pShip->IsMultiHull() && RepairShipInterior(pDevice, pShip, Ctx))
		return;

	//	Armor is repaired next, if necessary

	if (RepairShipArmor(pDevice, pShip, Ctx))
		return;

	//	Otherwise, if we have attached objects, try to repair those.

	if (pShip->HasAttachedSections() && RepairShipAttachedSections(pDevice, pShip, Ctx))
		return;

	//	Finally, see if we repair interior compartments

		
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
		GetItemType()->SetKnown();
	}

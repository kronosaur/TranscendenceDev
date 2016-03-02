//	CReactorClass.cpp
//
//	CReactorClass class

#include "PreComp.h"

#define FUEL_CAPACITY_ATTRIB					CONSTLIT("fuelCapacity")
#define FUEL_CRITERIA_ATTRIB					CONSTLIT("fuelCriteria")
#define FUEL_EFFICIENCY_ATTRIB					CONSTLIT("fuelEfficiency")
#define MAX_FUEL_ATTRIB							CONSTLIT("maxFuel")
#define MAX_FUEL_TECH_ATTRIB					CONSTLIT("maxFuelTech")
#define MIN_FUEL_TECH_ATTRIB					CONSTLIT("minFuelTech")
#define MAX_POWER_ATTRIB						CONSTLIT("maxPower")
#define REACTOR_EFFICIENCY_ATTRIB				CONSTLIT("reactorEfficiency")
#define REACTOR_POWER_ATTRIB					CONSTLIT("reactorPower")

#define PROPERTY_FUEL_CAPACITY					CONSTLIT("fuelCapacity")
#define PROPERTY_FUEL_CRITERIA					CONSTLIT("fuelCriteria")
#define PROPERTY_FUEL_EFFICIENCY				CONSTLIT("fuelEfficiency")
#define PROPERTY_FUEL_EFFICIENCY_BONUS			CONSTLIT("fuelEfficiencyBonus")
#define PROPERTY_POWER							CONSTLIT("power")

CReactorClass::CReactorClass (void) : CDeviceClass(NULL)
	{
	}

ALERROR CReactorClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice)

//	CreateFromXML
//
//	Creates from an XML element

	{
	ALERROR error;
	CReactorClass *pDevice;

	pDevice = new CReactorClass;
	if (pDevice == NULL)
		return ERR_MEMORY;

	if (error = pDevice->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

	//	Load the normal descriptor

	if (error = InitReactorDesc(Ctx, pDesc, &pDevice->m_Desc))
		return error;

	//	Compute descriptor when damaged

	pDevice->m_DamagedDesc.iMaxPower = 80 * pDevice->m_Desc.iMaxPower / 100;
	pDevice->m_DamagedDesc.rMaxFuel = pDevice->m_Desc.rMaxFuel;
	pDevice->m_DamagedDesc.iMinFuelLevel = pDevice->m_Desc.iMinFuelLevel;
	pDevice->m_DamagedDesc.iMaxFuelLevel = pDevice->m_Desc.iMaxFuelLevel;
	pDevice->m_DamagedDesc.rPowerPerFuelUnit = 0.8 * pDevice->m_Desc.rPowerPerFuelUnit;
	pDevice->m_DamagedDesc.pFuelCriteria = pDevice->m_Desc.pFuelCriteria;
	pDevice->m_DamagedDesc.fFreeFuelCriteria = false;
	pDevice->m_DamagedDesc.fDamaged = true;
	pDevice->m_DamagedDesc.fEnhanced = false;

	//	Compute descriptor when enhanced

	pDevice->m_EnhancedDesc.iMaxPower = 120 * pDevice->m_Desc.iMaxPower / 100;
	pDevice->m_EnhancedDesc.rMaxFuel = pDevice->m_Desc.rMaxFuel;
	pDevice->m_EnhancedDesc.iMinFuelLevel = pDevice->m_Desc.iMinFuelLevel;
	pDevice->m_EnhancedDesc.iMaxFuelLevel = pDevice->m_Desc.iMaxFuelLevel;
	pDevice->m_EnhancedDesc.rPowerPerFuelUnit = 1.5 * pDevice->m_Desc.rPowerPerFuelUnit;
	pDevice->m_EnhancedDesc.pFuelCriteria = pDevice->m_Desc.pFuelCriteria;
	pDevice->m_EnhancedDesc.fFreeFuelCriteria = false;
	pDevice->m_EnhancedDesc.fDamaged = false;
	pDevice->m_EnhancedDesc.fEnhanced = true;

	//	Done

	*retpDevice = pDevice;

	return NOERROR;
	}

bool CReactorClass::FindDataField (const ReactorDesc &Desc, const CString &sField, CString *retsValue)

//	FindDataField
//
//	Finds a data field for the reactor desc.

	{
	if (strEquals(sField, PROPERTY_POWER))
		*retsValue = strFromInt(Desc.iMaxPower * 100);
	else if (strEquals(sField, PROPERTY_FUEL_CRITERIA))
		{
		if (Desc.pFuelCriteria)
			*retsValue = CItem::GenerateCriteria(*Desc.pFuelCriteria);
		else
			*retsValue = strPatternSubst(CONSTLIT("f L:%d-%d;"), Desc.iMinFuelLevel, Desc.iMaxFuelLevel);
		}
	else if (strEquals(sField, PROPERTY_FUEL_EFFICIENCY))
		*retsValue = strFromInt((int)Desc.rPowerPerFuelUnit);
	else if (strEquals(sField, PROPERTY_FUEL_CAPACITY))
		*retsValue = strFromInt((int)(Desc.rMaxFuel / FUEL_UNITS_PER_STD_ROD));
	else
		return false;

	return true;
	}

bool CReactorClass::FindDataField (const CString &sField, CString *retsValue)

//	FindDataField
//
//	Returns meta-data

	{
	return FindDataField(m_Desc, sField, retsValue);
	}

ICCItem *CReactorClass::GetItemProperty (CItemCtx &Ctx, const CString &sName)

//	GetItemProperty
//
//	Returns the item property. Subclasses should call this if they do not
//	understand the property.

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	const ReactorDesc &Desc = *GetReactorDesc(Ctx.GetDevice(), Ctx.GetSource());

	if (strEquals(sName, PROPERTY_POWER))
		return CC.CreateInteger(Desc.iMaxPower * 100);

	else if (strEquals(sName, PROPERTY_FUEL_CRITERIA))
		{
		if (Desc.pFuelCriteria)
			return CC.CreateString(CItem::GenerateCriteria(*Desc.pFuelCriteria));
		else
			return CC.CreateString(strPatternSubst(CONSTLIT("f L:%d-%d;"), Desc.iMinFuelLevel, Desc.iMaxFuelLevel));
		}
	else if (strEquals(sName, PROPERTY_FUEL_EFFICIENCY))
		return CC.CreateInteger((int)Desc.rPowerPerFuelUnit);

	else if (strEquals(sName, PROPERTY_FUEL_EFFICIENCY_BONUS))
		return CC.CreateInteger(GetEfficiencyBonus(Desc));

	else if (strEquals(sName, PROPERTY_FUEL_CAPACITY))
		return CC.CreateInteger((int)(Desc.rMaxFuel / FUEL_UNITS_PER_STD_ROD));

	//	Otherwise, just get the property from the base class

	else
		return CDeviceClass::GetItemProperty(Ctx, sName);
	}

const ReactorDesc *CReactorClass::GetReactorDesc (CInstalledDevice *pDevice, CSpaceObject *pSource)

//	GetReactorDesc
//
//	Returns the reactor descriptor

	{
	if (pDevice == NULL)
		return &m_Desc;
	else if (pDevice->IsDamaged() || pDevice->IsDisrupted())
		return &m_DamagedDesc;
	else if (pDevice->IsEnhanced())
		return &m_EnhancedDesc;
	else
		return &m_Desc;
	}

ICCItem *CReactorClass::GetReactorProperty (const ReactorDesc &Desc, const CString &sProperty)

//	GetReactorProperty
//
//	Returns property for a built-in reactor. NOTE: We've mostly deprecated this
//	case. All player ships should have actual reactor items.

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sProperty, PROPERTY_POWER))
		return CC.CreateInteger(Desc.iMaxPower * 100);

	else if (strEquals(sProperty, PROPERTY_FUEL_CRITERIA))
		{
		if (Desc.pFuelCriteria)
			return CC.CreateString(CItem::GenerateCriteria(*Desc.pFuelCriteria));
		else
			return CC.CreateString(strPatternSubst(CONSTLIT("f L:%d-%d;"), Desc.iMinFuelLevel, Desc.iMaxFuelLevel));
		}
	else if (strEquals(sProperty, PROPERTY_FUEL_EFFICIENCY))
		return CC.CreateInteger((int)Desc.rPowerPerFuelUnit);

	else if (strEquals(sProperty, PROPERTY_FUEL_EFFICIENCY_BONUS))
		return CC.CreateInteger(GetEfficiencyBonus(Desc));

	else if (strEquals(sProperty, PROPERTY_FUEL_CAPACITY))
		return CC.CreateInteger((int)(Desc.rMaxFuel / FUEL_UNITS_PER_STD_ROD));
	else
		return CC.CreateNil();
	}

ALERROR CReactorClass::InitReactorDesc (SDesignLoadCtx &Ctx, CXMLElement *pDesc, ReactorDesc *retDesc, bool bShipClass)

//	InitReactorDesc
//
//	Initialize reactor desc structure from XML.

	{
	//	Initialize

	if (retDesc->pFuelCriteria)
		{
		delete retDesc->pFuelCriteria;
		retDesc->pFuelCriteria = NULL;
		retDesc->fFreeFuelCriteria = false;
		}

	//	Load some values

	if (bShipClass)
		{
		retDesc->iMaxPower = pDesc->GetAttributeIntegerBounded(REACTOR_POWER_ATTRIB, 0, -1, 0);
		retDesc->rMaxFuel = pDesc->GetAttributeDoubleBounded(FUEL_CAPACITY_ATTRIB, 0.0, -1.0, retDesc->iMaxPower * 250.0);
		retDesc->rPowerPerFuelUnit = pDesc->GetAttributeDoubleBounded(REACTOR_EFFICIENCY_ATTRIB, 0.0, -1.0, g_MWPerFuelUnit);
		}
	else
		{
		retDesc->iMaxPower = pDesc->GetAttributeIntegerBounded(MAX_POWER_ATTRIB, 0, -1, 100);
		retDesc->rMaxFuel = pDesc->GetAttributeDoubleBounded(MAX_FUEL_ATTRIB, 0.0, -1.0, retDesc->iMaxPower * 250.0);
		retDesc->rPowerPerFuelUnit = pDesc->GetAttributeDoubleBounded(FUEL_EFFICIENCY_ATTRIB, 0.0, -1.0, g_MWPerFuelUnit);
		}

	retDesc->fDamaged = false;
	retDesc->fEnhanced = false;

	//	Load the fuel criteria

	CString sCriteria;
	if (pDesc->FindAttribute(FUEL_CRITERIA_ATTRIB, &sCriteria))
		{
		retDesc->pFuelCriteria = new CItemCriteria;
		retDesc->fFreeFuelCriteria = true;

		CItem::ParseCriteria(sCriteria, retDesc->pFuelCriteria);

		retDesc->iMinFuelLevel = -1;
		retDesc->iMaxFuelLevel = -1;

		//	Warn if older method is used

		if (pDesc->AttributeExists(MIN_FUEL_TECH_ATTRIB) || pDesc->AttributeExists(MAX_FUEL_TECH_ATTRIB))
			kernelDebugLogMessage("Warning: minFuelTech and maxFuelTech ignored if fuelCriteria specified.");
		}

	//	If we have no fuel criteria, then use the older method

	else
		{
		retDesc->iMinFuelLevel = pDesc->GetAttributeIntegerBounded(MIN_FUEL_TECH_ATTRIB, 1, MAX_ITEM_LEVEL, 1);
		retDesc->iMaxFuelLevel = pDesc->GetAttributeIntegerBounded(MAX_FUEL_TECH_ATTRIB, retDesc->iMinFuelLevel, MAX_ITEM_LEVEL, (bShipClass ? 3 : MAX_ITEM_LEVEL));
		}

	return NOERROR;
	}

bool CReactorClass::IsFuelCompatible (const ReactorDesc &Desc, const CItem &FuelItem)

//	IsFuelCompatible
//
//	Returns TRUE if compatible with the given fuel.

	{
	if (Desc.pFuelCriteria)
		return FuelItem.MatchesCriteria(*Desc.pFuelCriteria);
	else
		{
		//	Must be fuel

		if (!FuelItem.GetType()->IsFuel())
			return false;

		//	Make sure we're the correct level

		int iLevel = FuelItem.GetLevel();
		return (iLevel >= Desc.iMinFuelLevel 
				&& iLevel <= Desc.iMaxFuelLevel);
		}
	}

CString CReactorClass::OnGetReference (CItemCtx &Ctx, int iVariant, DWORD dwFlags)

//	OnGetReference
//
//	Returns a reference string.

	{
	CString sReference;

	//	Get reactor stats

	const ReactorDesc &Desc = *GetReactorDesc(Ctx.GetDevice(), Ctx.GetSource());

	//	Power output

	sReference = strPatternSubst(CONSTLIT("%s max output"), ReactorPower2String(Desc.iMaxPower));

	//	Fuel level

	int iMinLevel;
	int iMaxLevel;
	if (Desc.pFuelCriteria)
		Desc.pFuelCriteria->GetExplicitLevelMatched(&iMinLevel, &iMaxLevel);
	else
		{
		iMinLevel = Desc.iMinFuelLevel;
		iMaxLevel = Desc.iMaxFuelLevel;
		}

	if (iMinLevel == -1 && iMaxLevel == -1)
		;
	else if (iMinLevel == iMaxLevel)
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("fuel level %d"), iMinLevel));
	else if (iMaxLevel == -1)
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("fuel level %d-25"), iMinLevel));
	else if (iMinLevel == -1)
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("fuel level 1-%d"), iMaxLevel));
	else
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("fuel level %d-%d"), iMinLevel, iMaxLevel));

	//	Efficiency

	int iBonus = GetEfficiencyBonus(Desc);
	if (iBonus > 0)
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("+%d%% efficiency"), iBonus));
	else if (iBonus < 0)
		AppendReferenceString(&sReference, strPatternSubst(CONSTLIT("%d%% efficiency"), iBonus));

	//	Done

	return sReference;
	}

int CReactorClass::GetEfficiencyBonus (const ReactorDesc &Desc)

//	GetEfficiencyBonus
//
//	Returns the efficiency of the reactor relative to the standard in percent
//	terms. We round to multiple of 5.

	{
	if (Desc.rPowerPerFuelUnit != g_MWPerFuelUnit)
		{
		int iBonus = (int)(100.0 * ((Desc.rPowerPerFuelUnit / g_MWPerFuelUnit) - 1.0));
		if (iBonus > 0)
			return 5 * ((iBonus + 2) / 5);
		else
			return 5 * ((iBonus - 2) / 5);
		}
	else
		return 0;
	}

void CReactorClass::OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList)

//	OnInstall
//
//	Device installed

	{
	//	Identified when installed

	if (pSource->IsPlayer())
		GetItemType()->SetKnown();
	}

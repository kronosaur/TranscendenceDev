//	CSolarDeviceClass.cpp
//
//	CSolarDeviceClass class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define POWER_GEN_ATTRIB							CONSTLIT("powerGen")
#define REFUEL_ATTRIB								CONSTLIT("refuel")
#define HEAT_GENERATION_ATTRIB						CONSTLIT("heatGeneration")
#define SOLAR_HEAT_GENERATION_ATTRIB				CONSTLIT("solarHeatGeneration")

#define CYCLE_TIME									10

CSolarDeviceClass::CSolarDeviceClass (void)

//	CSolarDeviceClass constructor

	{
	}

int CSolarDeviceClass::CalcHeatDelta (const SUpdateCtx& Ctx, const CInstalledDevice* pDevice, CSpaceObject* pSource)

//	CalcHeatDelta
//
//	Returns the amount of heat generated per tick

	{
	int iHeat = 0;

	//	Only if enabled

	if (!pDevice->IsEnabled())
		return 0;

	//	Add constant heat generation

	iHeat += m_iHeatGeneration;

	//	Add solar heat generation

	int iIntensity = Ctx.GetLightIntensity(pSource);
	if (pDevice->IsDamaged() || pDevice->IsDisrupted())
		iIntensity = iIntensity / 2;

	iHeat += (m_iSolarHeatGeneration * iIntensity) / 100;

	//	Done

	return iHeat;
	}

int CSolarDeviceClass::GetHeatRating (CItemCtx& Ctx, int* retiIdleHeatGeneration) const
{
	return max(m_iHeatGeneration, m_iHeatGeneration + m_iSolarHeatGeneration);
}

int CSolarDeviceClass::CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource)

//	CalcPowerUsed
//
//	Computes the amount of power used this tick (negative numbers mean power is generated).

	{
	if (m_iPowerGen == 0 || !pDevice->IsEnabled())
		return 0;

	int iIntensity = Ctx.GetLightIntensity(pSource);
	if (pDevice->IsDamaged() || pDevice->IsDisrupted())
		iIntensity = iIntensity / 2;

	//	Negative means power generation.

	return -(m_iPowerGen * iIntensity / 100);
	}

ALERROR CSolarDeviceClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice)

//	CreateFromXML
//
//	Load device data from XML

	{
	ALERROR error;
	CSolarDeviceClass *pDevice;

	pDevice = new CSolarDeviceClass;
	if (pDevice == NULL)
		return ERR_MEMORY;

	if (error = pDevice->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

	pDevice->m_iPowerGen = pDesc->GetAttributeIntegerBounded(POWER_GEN_ATTRIB, 0, -1, 0);
	pDevice->m_iRefuel = pDesc->GetAttributeIntegerBounded(REFUEL_ATTRIB, 0, -1, 0);
	pDevice->m_iSolarHeatGeneration = pDesc->GetAttributeIntegerBounded(SOLAR_HEAT_GENERATION_ATTRIB, 0, -1, 0);

	//	Done

	*retpDevice = pDevice;

	return NOERROR;
	}

void CSolarDeviceClass::Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx)

//	Update
//
//	Update device

	{
	DEBUG_TRY

	if (((Ctx.iTick + 7) % CYCLE_TIME) == 0)
		{
		if (m_iRefuel > 0)
			{
			int iIntensity = pSource->GetSystem()->CalculateLightIntensity(pSource->GetPos());
			if (pDevice->IsDamaged() || pDevice->IsDisrupted())
				iIntensity = iIntensity / 2;
			pSource->Refuel(iIntensity * (Metric)m_iRefuel / 100.0);
			}
		}

	DEBUG_CATCH
	}

void CSolarDeviceClass::OnInstall (CInstalledDevice *pDevice, CSpaceObject *pSource, CItemListManipulator &ItemList)

//	OnInstall
//
//	Device installed

	{
	//	Identified when installed

	if (pSource->IsPlayer())
		ItemList.GetItemAtCursor().SetKnown();
	}

//	CMiscellaneousClass.cpp
//
//	CMiscellaneousClass class

#include "PreComp.h"

#define POWER_USE_ATTRIB						(CONSTLIT("powerUse"))
#define POWER_TO_ACTIVATE_ATTRIB				(CONSTLIT("powerToActivate"))
#define CAPACITOR_POWER_USE_ATTRIB				(CONSTLIT("capacitorPowerUse"))
#define POWER_RATING_ATTRIB						(CONSTLIT("powerRating"))

#define HEAT_GENERATION_ATTRIB					(CONSTLIT("heatGeneration"))
#define CAPACITOR_HEAT_GENERATION_ATTRIB		(CONSTLIT("capacitorHeatGeneration"))

#define TICKS_PER_UPDATE						30

CMiscellaneousClass::CMiscellaneousClass (void)

//	CAutoDefenseClass constructor

	{
	}

int CMiscellaneousClass::CalcPowerUsed (SUpdateCtx &Ctx, CInstalledDevice *pDevice, CSpaceObject *pSource)

//	CalcPowerUsed
//
//	Returns the amount of power used per tick

	{
	int iPower = 0;

	//	Only if enabled

	if (!pDevice->IsEnabled())
		return 0;

	//	Add constant power use

	iPower += m_iPowerUse;

	//	If we're not ready, then we use up power to charge the capacitors

	if (!pDevice->IsReady())
		iPower += GetCapacitorPowerUse(pDevice, pSource);

	//	Done

	return iPower;
	}

int CMiscellaneousClass::CalcHeatDelta (const SUpdateCtx& Ctx, const CInstalledDevice* pDevice, CSpaceObject* pSource)

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

	//	If we're not ready, then we generate additional heat when charging the capacitors

	if (!pDevice->IsReady())
		iHeat += m_iCapacitorHeatGeneration;

	//	Done

	return iHeat;
	}

int CMiscellaneousClass::GetHeatRating (CItemCtx& Ctx, int* retiIdleHeatGeneration) const
	{
	return max(m_iHeatGeneration, m_iHeatGeneration + m_iCapacitorHeatGeneration);
	}

ALERROR CMiscellaneousClass::CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CItemType *pType, CDeviceClass **retpDevice)

//	CreateFromXML
//
//	Load the class

	{
	ALERROR error;
	CMiscellaneousClass *pDevice;

	pDevice = new CMiscellaneousClass;
	if (pDevice == NULL)
		return ERR_MEMORY;

	if (error = pDevice->InitDeviceFromXML(Ctx, pDesc, pType))
		return error;

	pDevice->m_iPowerRating = pDesc->GetAttributeInteger(POWER_RATING_ATTRIB);
	pDevice->m_iPowerUse = pDesc->GetAttributeInteger(POWER_USE_ATTRIB);
	pDevice->m_iPowerToActivate = pDesc->GetAttributeInteger(POWER_TO_ACTIVATE_ATTRIB);
	pDevice->m_iPowerForCapacitor = pDesc->GetAttributeInteger(CAPACITOR_POWER_USE_ATTRIB);

	pDevice->m_iHeatGeneration = pDesc->GetAttributeInteger(HEAT_GENERATION_ATTRIB);
	pDevice->m_iCapacitorHeatGeneration = pDesc->GetAttributeInteger(CAPACITOR_HEAT_GENERATION_ATTRIB);

	//	Done

	*retpDevice = pDevice;

	return NOERROR;
	}

int CMiscellaneousClass::GetActivateDelay (CItemCtx &ItemCtx) const

//	GetActivateDelay
//
//	Compute activation delay based on power usage

	{
	if (m_iPowerToActivate == 0)
		return 0;

	int iPowerPerCycle = GetCapacitorPowerUse(ItemCtx.GetDevice(), ItemCtx.GetSource());
	if (iPowerPerCycle == 0)
		return 0;

	return (m_iPowerToActivate / iPowerPerCycle);
	}

int CMiscellaneousClass::GetCapacitorPowerUse (CInstalledDevice *pDevice, CSpaceObject *pSource) const

//	GetCapacitorPowerUse
//
//	Returns the amount of power required to charge capacitors
//	for each tick

	{
	if (m_iPowerForCapacitor >= 0)
		return m_iPowerForCapacitor;
	else if (pSource)
		{
		//	m_iPowerForCapacitor is a % of max power

		return pSource->GetMaxPower() * (-m_iPowerForCapacitor) / 100;
		}
	else
		return 0;
	}

int CMiscellaneousClass::GetCounter (const CInstalledDevice *pDevice, const CSpaceObject *pSource, EDeviceCounterType *retiType, int *retiLevel) const

//	GetCounter
//
//	Returns the charge level

	{
	//  If we don't need activation power, then we're done

	if (m_iPowerToActivate == 0)
		{
		if (retiType)
			*retiType = EDeviceCounterType::None;

		if (retiLevel)
			*retiLevel = 0;

		return 0;
		}

	//  We're always a capacitor

	if (retiType)
		*retiType = EDeviceCounterType::Capacitor;

	//  If no device or source, we don't know the level

	if (pDevice == NULL || pSource == NULL)
		{
		if (retiLevel)
			*retiLevel = 0;

		return 0;
		}

	//	If we're full, no counter

	if (pDevice->IsReady())
		{
		if (retiLevel)
			*retiLevel = 100;

		//  Return 0 so we don't show a counter

		return 0;
		}

	//	Figure out how long before we're ready

	CItemCtx ItemCtx(pSource, pDevice);
	int iActivateDelay = GetActivateDelay(ItemCtx);
	int iLevel = (iActivateDelay > 0 ? 100 - (pDevice->GetTimeUntilReady() * 100 / iActivateDelay) : 0);

	if (retiLevel)
		*retiLevel = iLevel;

	return iLevel;
	}

int CMiscellaneousClass::GetPowerRating (CItemCtx &Ctx, int *retiIdlePowerUse) const

//	GetPowerRating
//
//	Returns the minimum reactor size needed for this device

	{
	if (retiIdlePowerUse)
		*retiIdlePowerUse = 0;

	//	If we have an explicit power rating, then use that

	if (m_iPowerRating > 0)
		return m_iPowerRating;

	//	If we can calculate max power usage, then return that

	else if (m_iPowerForCapacitor >= 0)
		return m_iPowerUse + m_iPowerForCapacitor;

	//	If power usage is relative to reactor output, then 
	//	assume 100MW reactor

	else
		return m_iPowerUse + (1000 * (-m_iPowerForCapacitor) / 100);
	}

bool CMiscellaneousClass::SetCounter (CInstalledDevice *pDevice, CSpaceObject *pSource, EDeviceCounterType iCounter, int iLevel)

//  SetCounter
//
//  Sets the counter to the given level. Returns FALSE if we cannot set it.

	{
	if (iCounter != EDeviceCounterType::Capacitor || m_iPowerToActivate == 0 || pDevice == NULL || pSource == NULL)
		return false;

	CItemCtx ItemCtx(pSource, pDevice);
	int iActivateDelay = GetActivateDelay(ItemCtx);
	int iTimeLeft = Max(0, Min((100 - iLevel) * iActivateDelay / 100, iActivateDelay));

	pDevice->SetTimeUntilReady(iTimeLeft);
	pSource->OnComponentChanged(comDeviceCounter);

	return true;
	}

bool CMiscellaneousClass::ShowActivationDelayCounter (CSpaceObject *pSource, CInstalledDevice *pDevice)

//	ShowActivationDelayCounter
//
//	Returns TRUE if we show a counter while charging

	{
	return (m_iPowerToActivate != 0);
	}

void CMiscellaneousClass::Update (CInstalledDevice *pDevice, CSpaceObject *pSource, SDeviceUpdateCtx &Ctx)

//	Update
//
//	Update device

	{
	}

//	CPowerConsumption.cpp
//
//	CPowerConsumption class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

Metric CPowerConsumption::ConsumeFuel (Metric rFuel, CReactorDesc::EFuelUseTypes iUse)

//	ConsumeFuel
//
//	Consumes some fuel, making sure we don't underflow. We return the actual
//	amount of fuel consumed.

	{
	if (rFuel < m_rFuelLeft)
		{
		m_rFuelLeft -= rFuel;
		return rFuel;
		}
	else
		{
		Metric rConsumed = m_rFuelLeft;
		m_rFuelLeft = 0.0;
		return rConsumed;
		}
	}

void CPowerConsumption::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Metric			m_rFuelLeft
//	DWORD			m_iPowerDrain
//	DWORD			m_iPowerGenerated
//	DWORD			m_iReactorGraceTimer; unused
//	DWORD			flags

	{
	Ctx.pStream->Read(m_rFuelLeft);
	Ctx.pStream->Read(m_iPowerDrain);
	Ctx.pStream->Read(m_iPowerGenerated);

	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);
	m_iReactorGraceTimer = (int)LOWORD(dwLoad);

	DWORD dwFlags;
	Ctx.pStream->Read(dwFlags);
	m_fOutOfFuel = ((dwFlags &	0x00000001) ? true : false);
	m_fOutOfPower = ((dwFlags &	0x00000002) ? true : false);
	}

void CPowerConsumption::Refuel (Metric rFuel, Metric rMaxFuel)

//	Refuel
//
//	Adds the given amount of fuel.

	{
	if (rFuel < 0.0)
		return;

	if (m_fOutOfFuel)
		{
		m_rFuelLeft = 0.0;
		m_fOutOfFuel = false;
		}

	m_rFuelLeft = Min(rMaxFuel, m_rFuelLeft + rFuel);
	}

void CPowerConsumption::SetFuelLeft (Metric rFuel)

//	SetFuelLeft
//
//	Sets the amount of fuel left.

	{
	m_rFuelLeft = rFuel;

	//	Clear out of fuel if necessary

	if (m_fOutOfFuel && rFuel > 0.0)
		m_fOutOfFuel = false;
	}

void CPowerConsumption::SetMaxFuel (Metric rMaxFuel)

//	SetMaxFuel
//
//	This is called when we change reactors to make sure we don't overflow fuel.

	{
	if (!m_fOutOfFuel)
		m_rFuelLeft = Min(m_rFuelLeft, rMaxFuel);
	}

bool CPowerConsumption::UpdateGraceTimer (void)

//	UpdateGraceTimer
//
//	Decrements the grace timer and returns TRUE if we're at 0.

	{
	if (m_iReactorGraceTimer > 0)
		m_iReactorGraceTimer--;

	return (m_iReactorGraceTimer <= 0);
	}

void CPowerConsumption::UpdatePowerUse (int iPowerDrained, int iPowerGenerated, Metric rEfficiency)

//	UpdatePowerUse
//
//	Sets the power stats

	{
	ASSERT(rEfficiency > 0.0);
	ASSERT(iPowerDrained >= 0);
	ASSERT(iPowerGenerated >= 0);

	m_iPowerDrain = iPowerDrained;
	m_iPowerGenerated = iPowerGenerated;

	ConsumeFuel(GetPowerNeeded() / rEfficiency, CReactorDesc::fuelConsume);
	}

void CPowerConsumption::WriteToStream (CSpaceObject *pObj, IWriteStream &Stream) const

//	WriteToStream
//
//	Metric			m_rFuelLeft
//	DWORD			m_iPowerDrain
//	DWORD			m_iPowerGenerated
//	DWORD			m_iReactorGraceTimer; unused
//	DWORD			flags

	{
	Stream.Write(m_rFuelLeft);
	Stream.Write(m_iPowerDrain);
	Stream.Write(m_iPowerGenerated);

	DWORD dwSave = MAKELONG(m_iReactorGraceTimer, 0);
	Stream.Write(dwSave);

	DWORD dwFlags = 0;
	dwFlags |= (m_fOutOfFuel ?	0x00000001 : 0);
	dwFlags |= (m_fOutOfPower ? 0x00000002 : 0);
	Stream.Write(dwFlags);
	}


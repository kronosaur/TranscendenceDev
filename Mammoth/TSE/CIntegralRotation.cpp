//	CIntegralRotation.cpp
//
//	CIntegralRotation class
//	Copyright (c) 2014 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

#define FIELD_THRUSTER_POWER					CONSTLIT("thrusterPower")

const Metric MANEUVER_MASS_FACTOR =				1.0;
const Metric MAX_INERTIA_RATIO =				9.0;

int CIntegralRotation::CalcFinalRotationFrame (const CIntegralRotationDesc &Desc, EManeuver iManeuver) const

//	CalcFinalRotationFrame
//
//	Computes the final rotation frame if the next maneuver is given.

	{
	switch (iManeuver)
		{
		case EManeuver::RotateLeft:
		case EManeuver::RotateRight:
			{
			//	Calculate our new state next tick.

			int iRotationFrame = m_iRotationFrame;
			int iRotationSpeed = m_iRotationSpeed;

			if (iManeuver == EManeuver::RotateLeft)
				UpdateRotateLeft(iRotationSpeed, Desc);
			else
				UpdateRotateRight(iRotationSpeed, Desc);

			iRotationFrame = UpdateRotationFrame(iRotationFrame, iRotationSpeed, Desc);
			return Desc.CalcFinalRotationFrame(iRotationFrame, iRotationSpeed);
			}

		default:
			return CalcFinalRotationFrame(Desc);
		}
	}

ICCItemPtr CIntegralRotation::Diagnostics (int iFrameCount, Metric rMaxRotationSpeed, Metric rAccel, Metric rAccelStop)

//	Diagnostics
//
//	Returns diagnostics on implementation.

	{
	try
		{
		static constexpr int TURN_COUNT = 10;

		ICCItemPtr pResult(ICCItem::List);

		CIntegralRotationDesc Desc;
		Desc.Init(iFrameCount, rMaxRotationSpeed, rAccel, rAccelStop);

		CIntegralRotation Rotation;
		Rotation.Init(Desc, 0);

		pResult->Append(Rotation.GetStatus(Desc));

		int iStartFrame = Rotation.m_iRotationFrame;
		int iStartSpeed = Rotation.m_iRotationSpeed;

		//	Turn 10 ticks clockwise

		for (int i = 0; i < TURN_COUNT; i++)
			{
			Rotation.Update(Desc, EManeuver::RotateRight);
			pResult->Append(Rotation.GetStatus(Desc));
			}

		Rotation.SetRotationSpeedDegrees(Desc, 0.0);
		pResult->Append(Rotation.GetStatus(Desc));

		//	Turn 10 ticks counter-clockwise

		for (int i = 0; i < TURN_COUNT; i++)
			{
			Rotation.Update(Desc, EManeuver::RotateLeft);
			pResult->Append(Rotation.GetStatus(Desc));
			}

		if (Rotation.m_iRotationFrame != iStartFrame)
			pResult->Append(ICCItemPtr(CONSTLIT("Asymmetric rotation detected.")));

		Rotation.Init(Desc, 0);
		pResult->Append(Rotation.GetStatus(Desc));

		//	Turn 10 ticks counter-clockwise.

		for (int i = 0; i < TURN_COUNT; i++)
			{
			Rotation.Update(Desc, EManeuver::RotateLeft);
			pResult->Append(Rotation.GetStatus(Desc));
			}

		Rotation.SetRotationSpeedDegrees(Desc, 0.0);
		pResult->Append(Rotation.GetStatus(Desc));

		//	Turn 10 ticks clockwise

		for (int i = 0; i < TURN_COUNT; i++)
			{
			Rotation.Update(Desc, EManeuver::RotateRight);
			pResult->Append(Rotation.GetStatus(Desc));
			}

		if (Rotation.m_iRotationFrame != iStartFrame)
			pResult->Append(ICCItemPtr(CONSTLIT("Asymmetric rotation detected.")));

		//	Done

		return pResult;
		}
	catch (...)
		{
		return ICCItemPtr::Error(CONSTLIT("Crash in CIntegralRotation::Diagnostics."));
		}
	}

int CIntegralRotation::GetFrameAlignedRotationAngle (const CIntegralRotationDesc &Desc) const

//	GetFrameAlignedRotationAngle
//
//	Returns the angle of the frame.

	{
	return Desc.GetRotationAngle(GetFrameIndex(m_iRotationFrame));
	}

EManeuver CIntegralRotation::GetManeuverToFace (const CIntegralRotationDesc &Desc, int iAngle) const

//	GetManeuverToFace
//
//	Returns the maneuver required to face the given angle (or NoRotation if we're
//	already facing in that (rough) direction).

	{
	//	Figure out where we will point if we stop turning right now.

	int iFinalAngle = Desc.GetRotationAngleExact(CalcFinalRotationFrame(Desc));
	int iDesiredAngle = iAngle;

	//	If we're going to be in the right spot by doing nothing, then just stop
	//	rotating.

	if (iFinalAngle == iDesiredAngle)
		return EManeuver::None;

	//	Otherwise, figure out how many frames we need to turn (and the 
	//	direction).

	int iDegreeDiff = ClockDiff(iDesiredAngle, iFinalAngle, 360);

	//	Are we turning left?

	const Metric rMaxRotationSpeed = Desc.GetMaxRotationSpeedDegrees();
	int iNewRotationFrame;
	if (iDegreeDiff > 0)
		{
		//	If we have a ways to go, then just turn

		if ((Metric)iDegreeDiff > rMaxRotationSpeed)
			return EManeuver::RotateLeft;

		//	Otherwise we need to calculate better. Figure out what our new
		//	rotation speed will be if we turn left.

		iNewRotationFrame = CalcFinalRotationFrame(Desc, EManeuver::RotateLeft);
		}

	//	Or right

	else
		{
		//	If we have a ways to go, then just turn

		if (-(Metric)iDegreeDiff > rMaxRotationSpeed)
			return EManeuver::RotateRight;

		//	Otherwise we need a better calculation. Figure out what our new
		//	rotation speed will be if we turn right.

		iNewRotationFrame = CalcFinalRotationFrame(Desc, EManeuver::RotateRight);
		}

	//	Figure out where we will end up next tick given our new rotation speed.

	int iNewAngle = Desc.GetRotationAngleExact(iNewRotationFrame);
	int iNewDegreeDiff = ClockDiff(iDesiredAngle, iNewAngle, 360);

	//	If we're closer to the target, then do it.

	if (iDegreeDiff > 0)
		{
		if (iNewDegreeDiff >= 0 && iNewDegreeDiff < iDegreeDiff)
			return EManeuver::RotateLeft;
		else
			return EManeuver::None;
		}
	else
		{
		if (iNewDegreeDiff <= 0 && iNewDegreeDiff > iDegreeDiff)
			return EManeuver::RotateRight;
		else
			return EManeuver::None;
		}
	}

int CIntegralRotation::GetRotationAngle (const CIntegralRotationDesc &Desc) const

//	GetRotationAngle
//
//	Converts from our rotation frame to an angle

	{
	return Desc.GetRotationAngleExact(m_iRotationFrame);
	}

Metric CIntegralRotation::GetRotationSpeedDegrees (const CIntegralRotationDesc &Desc) const

//	GetRotationSpeed
//
//	Returns the current rotation speed in degrees per tick.
//	Negative = counterclockwise.

	{
	if (Desc.GetFrameCount() == 0)
		return 0.0;

	int iMaxRotateFrameCount = Desc.GetFrameCount() * CIntegralRotationDesc::ROTATION_FRACTION;
	return (360.0 * m_iRotationSpeed) / iMaxRotateFrameCount;
	}

ICCItemPtr CIntegralRotation::GetStatus (const CIntegralRotationDesc &Desc) const

//	GetStatus
//
//	Returns the current rotation state.

	{
	ICCItemPtr pResult(ICCItem::SymbolTable);

	pResult->SetIntegerAt(CONSTLIT("angle"), GetRotationAngle(Desc));
	pResult->SetIntegerAt(CONSTLIT("frame"), GetFrameIndex());
	pResult->SetDoubleAt(CONSTLIT("speed"), GetRotationSpeedDegrees(Desc));

	pResult->SetIntegerAt(CONSTLIT("currentFrameVar"), m_iRotationFrame);
	pResult->SetIntegerAt(CONSTLIT("currentSpeedVar"), m_iRotationSpeed);
	
	if (m_iLastManeuver == EManeuver::None)
		pResult->SetStringAt(CONSTLIT("lastManeuver"), CONSTLIT("none"));
	else if (m_iLastManeuver == EManeuver::RotateLeft)
		pResult->SetStringAt(CONSTLIT("lastManeuver"), CONSTLIT("left"));
	else if (m_iLastManeuver == EManeuver::RotateRight)
		pResult->SetStringAt(CONSTLIT("lastManeuver"), CONSTLIT("right"));
	else
		pResult->SetStringAt(CONSTLIT("lastManeuver"), CONSTLIT("unknown"));

	return pResult;
	}

void CIntegralRotation::Init (const CIntegralRotationDesc &Desc, int iRotationAngle)

//	Init
//
//	Initialize

	{
	//	Defaults

	if (iRotationAngle != -1)
		SetRotationAngle(Desc, iRotationAngle);
	else
		SetRotationAngle(Desc, 0);

	m_iRotationSpeed = 0;
	m_iLastManeuver = EManeuver::None;
	}

void CIntegralRotation::ReadFromStream (SLoadCtx &Ctx, const CIntegralRotationDesc &Desc)

//	ReadFromStream
//
//	Reads data

	{
	DWORD dwLoad;

	Init(Desc);

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iRotationFrame = (int)dwLoad;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_iRotationSpeed = (int)dwLoad;

	//	Convert from older constants

	if (Ctx.dwVersion < 200)
		{
		m_iRotationFrame = mathRound((Metric)CIntegralRotationDesc::ROTATION_FRACTION * (Metric)m_iRotationFrame / (Metric)ROTATION_FRACTION_OLD);
		m_iRotationSpeed = mathRound((Metric)CIntegralRotationDesc::ROTATION_FRACTION * (Metric)m_iRotationSpeed / (Metric)ROTATION_FRACTION_OLD);
		}

	//	Make sure our frame is within bounds; this can change if the ship's
	//	rotation count is altered in the XML.

	if (GetFrameIndex() >= Desc.GetFrameCount())
		SetRotationAngle(Desc, 0);
	}

void CIntegralRotation::SetRotationAngle (const CIntegralRotationDesc &Desc, int iAngle)

//	SetRotationAngle
//
//	Sets the rotation angle

	{
	m_iRotationFrame = Desc.GetRotationFrameExact(iAngle);
	}

void CIntegralRotation::SetRotationSpeedDegrees (const CIntegralRotationDesc &Desc, Metric rDegreesPerTick)

//	SetRotationSpeedDegree
//
//	Sets the current rotation speed.

	{
	m_iRotationSpeed = mathRound((rDegreesPerTick * Desc.GetFrameCount() * CIntegralRotationDesc::ROTATION_FRACTION) / 360.0);
	}

void CIntegralRotation::Update (const CIntegralRotationDesc &Desc, EManeuver iManeuver)

//	Update
//
//	Updates once per tick

	{
	DEBUG_TRY

	//	Change the rotation velocity

	switch (iManeuver)
		{
		case EManeuver::None:
			if (m_iRotationSpeed != 0)
				{
				//	Slow down rotation

				if (m_iRotationSpeed > 0)
					{
					m_iRotationSpeed = Max(0, m_iRotationSpeed - Desc.GetRotationAccelStop());
					m_iLastManeuver = EManeuver::RotateLeft;
					}
				else
					{
					m_iRotationSpeed = Min(0, m_iRotationSpeed + Desc.GetRotationAccelStop());
					m_iLastManeuver = EManeuver::RotateRight;
					}

				//	If we've stopped rotating, align to center of degree

				if (m_iRotationSpeed == 0)
					m_iRotationFrame = Desc.GetRotationFrameExact(Desc.GetRotationAngleExact(m_iRotationFrame));
				}
			else
				m_iLastManeuver = EManeuver::None;
			break;

		case EManeuver::RotateRight:
			if (UpdateRotateRight(m_iRotationSpeed, Desc))
				m_iLastManeuver = EManeuver::RotateRight;
			else
				m_iLastManeuver = EManeuver::None;
			break;

		case EManeuver::RotateLeft:
			if (UpdateRotateLeft(m_iRotationSpeed, Desc))
				m_iLastManeuver = EManeuver::RotateLeft;
			else
				m_iLastManeuver = EManeuver::None;
			break;
		}

	//	Now rotate

	m_iRotationFrame = UpdateRotationFrame(m_iRotationFrame, m_iRotationSpeed, Desc);

	DEBUG_CATCH
	}

int CIntegralRotation::UpdateRotationFrame (int iRotationFrame, int iRotationSpeed, const CIntegralRotationDesc &Desc)

//	UpdateRotationFrame
//
//	Returns the new rotation frame based on the current speed.

	{
	if (iRotationSpeed != 0 && Desc.GetFrameCount() > 0)
		{
		const int iFrameMax = Desc.GetFrameCount() * CIntegralRotationDesc::ROTATION_FRACTION;

		iRotationFrame = (iRotationFrame + iRotationSpeed) % iFrameMax;
		if (iRotationFrame < 0)
			iRotationFrame += iFrameMax;
		}

	return iRotationFrame;
	}

void CIntegralRotation::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Writes data

	{
	DWORD dwSave = m_iRotationFrame;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = m_iRotationSpeed;
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}

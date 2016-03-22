//	CIntegralRotation.cpp
//
//	CIntegralRotation class
//	Copyright (c) 2014 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

#define FIELD_THRUSTER_POWER					CONSTLIT("thrusterPower")

const Metric MANEUVER_MASS_FACTOR =				1.0;
const Metric MAX_INERTIA_RATIO =				9.0;

CIntegralRotation::~CIntegralRotation (void)

//	CIntegralRotation destructor

	{
	}

EManeuverTypes CIntegralRotation::GetManeuverToFace (const CIntegralRotationDesc &Desc, int iAngle) const

//	GetManeuverToFace
//
//	Returns the maneuver required to face the given angle (or NoRotation if we're
//	already facing in that (rough) direction).

	{
	//	Convert to a frame index. NOTE: We figure out what our rotation would be
	//	if we stopped thrusting right now.

	int iCurrentFrameIndex = GetFrameIndex(CalcFinalRotationFrame(Desc));
	int iDesiredFrameIndex = Desc.GetFrameIndex(iAngle);

	//	If we're going to be in the right spot by doing nothing, then just stop
	//	rotating.

	if (iCurrentFrameIndex == iDesiredFrameIndex)
		return NoRotation;

	//	Otherwise, figure out how many frames we need to turn (and the 
	//	direction).

	int iFrameDiff = ClockDiff(iDesiredFrameIndex, iCurrentFrameIndex, Desc.GetFrameCount());

	//	Are we turning right?

	int iNewRotationSpeed;
	int iMaxFrameRot = (Desc.GetMaxRotationSpeed() / CIntegralRotationDesc::ROTATION_FRACTION);
	if (iFrameDiff > 0)
		{
		//	If we have a ways to go, then just turn

		if (iFrameDiff > iMaxFrameRot)
			return RotateRight;

		//	Otherwise we need to calculate better. Figure out what our new
		//	rotation speed will be if we turn right.

		iNewRotationSpeed = m_iRotationSpeed;
		if (iNewRotationSpeed < Desc.GetMaxRotationSpeed())
			{
			if (iNewRotationSpeed < 0)
				iNewRotationSpeed = Min(Desc.GetMaxRotationSpeed(), iNewRotationSpeed + Desc.GetRotationAccelStop());
			else
				iNewRotationSpeed = Min(Desc.GetMaxRotationSpeed(), iNewRotationSpeed + Desc.GetRotationAccel());
			}
		}

	//	Or left

	else
		{
		//	If we have a ways to go, then just turn

		if (-iFrameDiff > iMaxFrameRot)
			return RotateLeft;

		//	Otherwise we need a better calculation. Figure out what our new
		//	rotation speed will be if we turn left.

		iNewRotationSpeed = m_iRotationSpeed;
		if (iNewRotationSpeed > -Desc.GetMaxRotationSpeed())
			{
			if (iNewRotationSpeed > 0)
				iNewRotationSpeed = Max(-Desc.GetMaxRotationSpeed(), iNewRotationSpeed - Desc.GetRotationAccelStop());
			else
				iNewRotationSpeed = Max(-Desc.GetMaxRotationSpeed(), iNewRotationSpeed - Desc.GetRotationAccel());
			}
		}

	//	Figure out where we will end up next tick given our new rotation speed.

	int iNewRotationFrame = m_iRotationFrame;
	int iFrameMax = Desc.GetFrameCount() * CIntegralRotationDesc::ROTATION_FRACTION;
	iNewRotationFrame = (iNewRotationFrame + iNewRotationSpeed) % iFrameMax;
	if (iNewRotationFrame < 0)
		iNewRotationFrame += iFrameMax;

	int iNewFrameIndex = GetFrameIndex(Desc.CalcFinalRotationFrame(iNewRotationFrame, iNewRotationSpeed));
	int iNewFrameDiff = ClockDiff(iDesiredFrameIndex, iNewFrameIndex, Desc.GetFrameCount());

	//	If we're closer to the target, then do it.

	if (Absolute(iNewFrameDiff) < Absolute(iFrameDiff))
		return (iFrameDiff < 0 ? RotateLeft : RotateRight);
	else
		return NoRotation;
	}

int CIntegralRotation::GetRotationAngle (const CIntegralRotationDesc &Desc) const

//	GetRotationAngle
//
//	Converts from our rotation frame to an angle

	{
	return Desc.GetRotationAngle(GetFrameIndex(m_iRotationFrame));
	}

void CIntegralRotation::Init (const CIntegralRotationDesc &Desc, int iRotationAngle)

//	Init
//
//	Initialize

	{
	//	Defaults

	if (iRotationAngle != -1)
		SetRotationAngle(Desc, iRotationAngle);
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
	m_iRotationFrame = CIntegralRotationDesc::ROTATION_FRACTION * Desc.GetFrameIndex(iAngle) + (CIntegralRotationDesc::ROTATION_FRACTION / 2);
	}

void CIntegralRotation::Update (const CIntegralRotationDesc &Desc, EManeuverTypes iManeuver)

//	Update
//
//	Updates once per tick

	{
	//	Change the rotation velocity

	switch (iManeuver)
		{
		case NoRotation:
			if (m_iRotationSpeed != 0)
				{
				//	Slow down rotation

				if (m_iRotationSpeed > 0)
					{
					m_iRotationSpeed = Max(0, m_iRotationSpeed - Desc.GetRotationAccelStop());
					m_iLastManeuver = RotateLeft;
					}
				else
					{
					m_iRotationSpeed = Min(0, m_iRotationSpeed + Desc.GetRotationAccelStop());
					m_iLastManeuver = RotateRight;
					}

				//	If we've stopped rotating, align to center of frame

				if (m_iRotationSpeed == 0)
					m_iRotationFrame = ((m_iRotationFrame / CIntegralRotationDesc::ROTATION_FRACTION) * CIntegralRotationDesc::ROTATION_FRACTION) + (CIntegralRotationDesc::ROTATION_FRACTION / 2);
				}
			else
				m_iLastManeuver = NoRotation;
			break;

		case RotateRight:
			if (m_iRotationSpeed < Desc.GetMaxRotationSpeed())
				{
				if (m_iRotationSpeed < 0)
					m_iRotationSpeed = Min(Desc.GetMaxRotationSpeed(), m_iRotationSpeed + Desc.GetRotationAccelStop());
				else
					m_iRotationSpeed = Min(Desc.GetMaxRotationSpeed(), m_iRotationSpeed + Desc.GetRotationAccel());
				m_iLastManeuver = RotateRight;
				}
			else
				m_iLastManeuver = NoRotation;
			break;

		case RotateLeft:
			if (m_iRotationSpeed > -Desc.GetMaxRotationSpeed())
				{
				if (m_iRotationSpeed > 0)
					m_iRotationSpeed = Max(-Desc.GetMaxRotationSpeed(), m_iRotationSpeed - Desc.GetRotationAccelStop());
				else
					m_iRotationSpeed = Max(-Desc.GetMaxRotationSpeed(), m_iRotationSpeed - Desc.GetRotationAccel());
				m_iLastManeuver = RotateLeft;
				}
			else
				m_iLastManeuver = NoRotation;
			break;
		}

	//	Now rotate

	if (m_iRotationSpeed != 0)
		{
		int iFrameMax = Desc.GetFrameCount() * CIntegralRotationDesc::ROTATION_FRACTION;

		m_iRotationFrame = (m_iRotationFrame + m_iRotationSpeed) % iFrameMax;
		if (m_iRotationFrame < 0)
			m_iRotationFrame += iFrameMax;
		}
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

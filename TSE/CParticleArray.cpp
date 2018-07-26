//	CParticleArray.cpp
//
//	CParticleArray object

#include "PreComp.h"

const int FIXED_POINT =							256;
const CG32bitPixel FLAME_CORE_COLOR =			CG32bitPixel(255,241,230);
const CG32bitPixel FLAME_MIDDLE_COLOR =			CG32bitPixel(255,208,0);
const CG32bitPixel FLAME_OUTER_COLOR =			CG32bitPixel(255,59,27);
//const WORD FLAME_MIDDLE_COLOR =					CG32bitPixel(248,200,12);
//const WORD FLAME_OUTER_COLOR =					CG32bitPixel(189,30,0);

const int DEFAULT_LINE_LENGTH =					30;

#define PAINT_GASEOUS_PARTICLE(Dest,x,y,iWidth,rgbColor,iFade,iFade2)	\
	{	\
	switch (iWidth)	\
		{	\
		case 0:	\
			Dest.SetPixelTrans((x), (y), (rgbColor), (BYTE)(iFade));	\
			break;	\
\
		case 1:	\
			Dest.SetPixelTrans((x), (y), (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x) + 1, (y), (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x), (y) + 1, (rgbColor), (BYTE)(iFade2));	\
			break;	\
\
		case 2:	\
			Dest.SetPixelTrans((x), (y), (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x) + 1, (y), (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x), (y) + 1, (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x) - 1, (y), (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x), (y) - 1, (rgbColor), (BYTE)(iFade2));	\
			break;	\
\
		case 3:	\
			Dest.SetPixelTrans((x), (y), (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x) + 1, (y), (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x), (y) + 1, (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x) - 1, (y), (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x), (y) - 1, (rgbColor), (BYTE)(iFade));	\
			Dest.SetPixelTrans((x) + 1, (y) + 1, (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x) + 1, (y) - 1, (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x) - 1, (y) + 1, (rgbColor), (BYTE)(iFade2));	\
			Dest.SetPixelTrans((x) - 1, (y) - 1, (rgbColor), (BYTE)(iFade2));	\
			break;	\
\
		default:	\
			CGDraw::Circle(Dest, (x), (y), ((iWidth) + 1) / 2, CG32bitPixel((rgbColor), (BYTE)(iFade)));	\
			break;	\
		}	\
	}

CParticleArray::CParticleArray (void) :
		m_iCount(0),
		m_pArray(NULL),
		m_iLastAdded(-1),
		m_bUseRealCoords(false)

//	CParticleArray constructor

	{
	m_rcBounds.left = 0;
	m_rcBounds.top = 0;
	m_rcBounds.right = 0;
	m_rcBounds.bottom = 0;
	}

CParticleArray::~CParticleArray (void)

//	CParticleArray destructor

	{
	if (m_pArray)
		delete [] m_pArray;
	}

void CParticleArray::AddParticle (const CVector &vPos, const CVector &vVel, int iLifeLeft, int iRotation, int iDestiny, int iGeneration, Metric rData)

//	AddParticle
//
//	Adds a new particle to the array

	{
	ASSERT(iDestiny == -1 || (iDestiny >= 0 && iDestiny < g_DestinyRange));

	if (m_iCount == 0)
		return;

	//	Infinite loop without this.

	if (m_iLastAdded == -1)
		m_iLastAdded = m_iCount - 1;

	//	Look for an open slot

	int iSlot = (m_iLastAdded + 1) % m_iCount;
	while (iSlot != m_iLastAdded && m_pArray[iSlot].fAlive)
		iSlot = (iSlot + 1) % m_iCount;

	//	If we're out of room, can't add

	if (iSlot == m_iLastAdded)
		return;

	//	Add the particle at the slot

	SParticle *pParticle = &m_pArray[iSlot];

	if (m_bUseRealCoords)
		{
		pParticle->Pos = vPos;
		pParticle->Vel = (vVel * g_SecondsPerUpdate);
		PosToXY(vPos, &pParticle->x, &pParticle->y);
		//	xVel and yVel are ignored if using real coords
		}
	else
		{
		PosToXY(vPos, &pParticle->x, &pParticle->y);
		PosToXY(vVel * g_SecondsPerUpdate, &pParticle->xVel, &pParticle->yVel);
		}

	pParticle->iGeneration = iGeneration;
	pParticle->iLifeLeft = iLifeLeft;
	pParticle->iDestiny = (iDestiny == -1 ? mathRandom(0, g_DestinyRange - 1) : iDestiny);
	pParticle->iRotation = iRotation;
	pParticle->rData = rData;

	pParticle->fAlive = true;

	m_iLastAdded = iSlot;
	}

void CParticleArray::CleanUp (void)

//	CleanUp
//
//	Deletes the array

	{
	if (m_pArray)
		{
		delete [] m_pArray;
		m_pArray = NULL;
		}

	m_iCount = 0;
	}

void CParticleArray::CreateInterpolatedParticles (const CParticleSystemDesc &Desc, CSpaceObject *pObj, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick)

//	CreateInterpolatedParticles
//
//	Creates particles interpolated between two directions.

	{
	int i;

	ASSERT(iDirection != -1);

	//	Compute some basic stuff

	const Metric rJitterFactor = LIGHT_SPEED / 100000.0;
	Metric rLastRotation = mathDegreesToRadians(180 + Desc.GetXformRotation() + GetLastEmitDirection());
	Metric rCurRotation = mathDegreesToRadians(180 + Desc.GetXformRotation() + iDirection);

	//	Compute the spread angle, in radians

	Metric rSpread = mathDegreesToRadians(Max(0, Desc.GetSpreadAngle().Roll()));
	Metric rHalfSpread = 0.5 * rSpread;

	//	Calculate where last tick's particles would be based on the last rotation.

	Metric rAveSpeed = Desc.GetEmitSpeed().GetAveValue() * LIGHT_SPEED / 100.0;
	CVector vLastStart = (GetLastEmitPos() + (Desc.GetXformTime() * ::PolarToVectorRadians(rLastRotation, rAveSpeed * g_SecondsPerUpdate))) - vSource;

	//	Calculate where last tick's particles would be IF we have used the current
	//	rotation. This allows us to interpolate a turn.

	CVector vCurStart = (GetLastEmitPos() + (Desc.GetXformTime() * ::PolarToVectorRadians(rCurRotation, rAveSpeed * g_SecondsPerUpdate))) - vSource;

	//	Create particles

	for (i = 0; i < iCount; i++)
		{
		Metric rSlide = mathRandom(0, 9999) / 10000.0;

		//	Compute two points along the two slide vectors (last and current)

		CVector vSlide1 = rSlide * vLastStart;
		CVector vSlide2 = rSlide * vCurStart;
		CVector vAdj = (rSlide * vSlide1) + ((1.0 - rSlide) * vSlide2);

		//	We place the particle along the line betwen the current
		//	and last emit positions

		CVector vPos = vSource + vAdj;

		//	We blend the rotation as well

		if (Absolute(rCurRotation - rLastRotation) > PI)
			{
			if (rLastRotation < rCurRotation)
				rLastRotation += PI * 2.0;
			else
				rCurRotation += PI * 2.0;
			}

		Metric rSlideRotation = (rSlide * rLastRotation) + ((1.0 - rSlide) * rCurRotation);

		//	Generate a random velocity backwards

		Metric rRotation = rSlideRotation + (rHalfSpread * mathRandom(-1000, 1000) / 1000.0);
		Metric rSpeed = Desc.GetEmitSpeed().Roll() * LIGHT_SPEED / 100.0;
		CVector vVel = Desc.GetXformTime() * (vSourceVel + ::PolarToVectorRadians(rRotation, rSpeed + rJitterFactor * mathRandom(-500, 500)));

		//	Lifetime

		int iLifeLeft = Desc.GetParticleLifetime().Roll();

		//	Add the particle

		AddParticle(vPos, vVel, iLifeLeft, AngleToDegrees(rRotation), -1, iTick);
		}
	}

void CParticleArray::CreateLinearParticles (const CParticleSystemDesc &Desc, CSpaceObject *pObj, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick)

//	CreateLinearParticles
//
//	Creates new particles on a straight line

	{
	int i;

	ASSERT(iDirection != -1);

	//	Compute some basic stuff

	const Metric rJitterFactor = LIGHT_SPEED / 100000.0;
	Metric rCurRotation = mathDegreesToRadians(180 + Desc.GetXformRotation() + iDirection);

	//	Compute the spread angle, in radians

	Metric rSpread = mathDegreesToRadians(Max(0, Desc.GetSpreadAngle().Roll()));
	Metric rHalfSpread = 0.5 * rSpread;

	//	Calculate where last tick's particles would be based on the last rotation.

	Metric rAveSpeed = Desc.GetEmitSpeed().GetAveValue() * LIGHT_SPEED / 100.0;
	CVector vCurStart = (GetLastEmitPos() + (Desc.GetXformTime() * ::PolarToVectorRadians(rCurRotation, rAveSpeed * g_SecondsPerUpdate))) - vSource;

	//	Create particles

	for (i = 0; i < iCount; i++)
		{
		Metric rSlide = mathRandom(0, 9999) / 10000.0;

		//	We place the particle along the line betwen the current
		//	and last emit positions

		CVector vPos = vSource + rSlide * vCurStart;

		//	Generate a random velocity backwards

		Metric rRotation = rCurRotation + (rHalfSpread * mathRandom(-1000, 1000) / 1000.0);
		Metric rSpeed = Desc.GetEmitSpeed().Roll() * LIGHT_SPEED / 100.0;
		CVector vVel = Desc.GetXformTime() * (vSourceVel + ::PolarToVectorRadians(rRotation, rSpeed + rJitterFactor * mathRandom(-500, 500)));

		//	Lifetime

		int iLifeLeft = Desc.GetParticleLifetime().Roll();

		//	Add the particle

		AddParticle(vPos, vVel, iLifeLeft, AngleToDegrees(rRotation), -1, iTick);
		}
	}

void CParticleArray::Emit (const CParticleSystemDesc &Desc, CSpaceObject *pObj, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick, int *retiEmitted)

//	Emit
//
//	Emit particles based on the descriptor.
//
//	vSource is relative to the particle array origin.
//
//	vSourceVel is the velocity of the source relative to the array origin.
//		For some effects, in which the array origin moves with the effect, this
//		should be Null. But for particle damage, which has a fixed array origin,
//		this should be the motion of the ship/station that fired.

	{
	//	Do we emit this tick?

	if (Desc.GetEmitChance() < 100
			&& mathRandom(1, 100) > Desc.GetEmitChance())
		return;

	//	Figure out how many particles to emit

	int iCount = Desc.GetEmitRate().Roll();
	if (iCount <= 0)
		return;

	//	Initialize the particle array, if necessary.

	if (GetCount() == 0)
		{
		//	Compute the maximum number of particles that we could ever have

		int iNewParticleRate = Desc.GetEmitRate().GetMaxValue();
		int iParticleLifetime = Desc.GetParticleLifetime().GetMaxValue();

		int iMaxParticleCount = Max(0, iParticleLifetime * iNewParticleRate);

		//	Initialize the array

		Init(iMaxParticleCount, vSource);
		}

	//	The last source position is not always the same as the input source 
	//	position. Depending on the style, we might store something different.

	CVector vLastSource = vSource;

	//	Emit particles

	switch (Desc.GetStyle())
		{
		case CParticleSystemDesc::styleAmorphous:
			EmitAmorphous(Desc, iCount, vSource, vSourceVel, iDirection, iTick);
			break;

		case CParticleSystemDesc::styleBrownian:
			EmitCloud(Desc, iCount, vSource, vSourceVel, iDirection, iTick);
			break;

		case CParticleSystemDesc::styleComet:
			EmitComet(Desc, iCount, vSource, vSourceVel, iDirection, iTick);
			break;

		case CParticleSystemDesc::styleExhaust:
			EmitExhaust(Desc, pObj, iCount, vSource, vSourceVel, iDirection, iTick, &vLastSource);
			break;

		case CParticleSystemDesc::styleJet:
			if (Desc.IsFixedPos())
				EmitExhaust(Desc, pObj, iCount, vSource, vSourceVel, iDirection, iTick, &vLastSource);
			else
				EmitJet(Desc, pObj, iCount, vSource, vSourceVel, iDirection, iTick);
			break;

		case CParticleSystemDesc::styleRadiate:
		case CParticleSystemDesc::styleWrithe:
			EmitRadiate(Desc, iCount, vSource, vSourceVel, iDirection, iTick);
			break;

		case CParticleSystemDesc::styleSpray:
			EmitSpray(Desc, iCount, vSource, vSourceVel, iDirection, iTick);
			break;
		}

	//	Remember

	m_vLastEmitSource = vLastSource;
	m_vLastEmitSourceVel = vSourceVel;
	m_iLastEmitDirection = iDirection;

	//	Done

	if (retiEmitted)
		*retiEmitted = iCount;
	}

void CParticleArray::EmitAmorphous (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick)

//	EmitAmorphous
//
//	Emits a directional stream of particles with a random shape. The head
//	of the particles will travel at maximum speed, while the tail will travel at
//	minimum speed.

	{
	const Metric GAUSSIAN_SCALE = 0.5;

	int i;

	//	Compute some basic stuff

	Metric rCurRotation = mathDegreesToRadians(Desc.GetXformRotation() + iDirection);

	//	Compute the range in speed

	Metric rMaxSpeed = Desc.GetEmitSpeed().GetMaxValue() * LIGHT_SPEED / 100.0;
	Metric rMinSpeed = Desc.GetEmitSpeed().GetMinValue() * LIGHT_SPEED / 100.0;
	Metric rSpeedRange = Max(0.01 * LIGHT_SPEED, rMaxSpeed - rMinSpeed);

	//	Compute the spread angle, in radians

	Metric rSpread = mathDegreesToRadians(Max(0, Desc.GetSpreadAngle().Roll()));
	Metric rHalfSpread = 0.5 * rSpread;

	//	If the emissions come from a line, calculate that (emitWidth)

	Metric rSpreadRange = Desc.GetEmitWidth().Roll() * g_KlicksPerPixel;
	CVector vTangent = ::PolarToVectorRadians(rCurRotation + (0.5 * PI), rSpreadRange);

	//	Create the particles

	for (i = 0; i < iCount; i++)
		{
		//	Pick the position of the particle along the main axis, with a 
		//	concentration towards the head.

		Metric rLengthPos = GAUSSIAN_SCALE * Absolute(::mathRandomGaussian());
		Metric rSpeed = Max(0.01 * LIGHT_SPEED, rMaxSpeed - (rSpeedRange * rLengthPos));

		//	Pick a position along the width axis. Again, we concentrate on the
		//	center.

		Metric rWidthPos = GAUSSIAN_SCALE * ::mathRandomGaussian();
		Metric rRotation = rCurRotation + (rHalfSpread * rWidthPos);

		//	All particle start near the emission point

		CVector vPos = vSource;
		CVector vVel =  vSourceVel + ::PolarToVectorRadians(rRotation, rSpeed);

		//	Adjust for spread

		if (rSpreadRange > 0.0)
			{
			Metric rTangentPlace = ((mathRandom(0, 25) + mathRandom(0, 25) + mathRandom(0, 25) + mathRandom(0, 25)) - 50.0) / 100.0;
			vPos = vPos + (vTangent * rTangentPlace);
			}

		//	Add the particle

		AddParticle(vPos, vVel, Desc.GetParticleLifetime().Roll(), AngleToDegrees(rRotation), -1, iTick);
		}
	}

void CParticleArray::EmitCloud (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick)

//	EmitCloud
//
//	Creates randomly in a circle (of a given radius) and with random velocity.

	{
	int i;

	//	Compute some basic stuff

	const Metric rJitterFactor = LIGHT_SPEED / 100000.0;
	int iRadius = Desc.GetRadius().Roll();

	//	Create particles

	for (i = 0; i < iCount; i++)
		{
		//	Choose a position

		CVector vPos = vSource + ::PolarToVector(mathRandom(0, 359), mathRandom(0, iRadius) * g_KlicksPerPixel);

		//	Choose a random angle and velocity

		Metric rAngle = 2.0 * PI * (mathRandom(0, 9999) / 10000.0);
		Metric rSpeed = (Desc.GetEmitSpeed().Roll() * LIGHT_SPEED / 100.0) + rJitterFactor * mathRandom(-500, 500);
		CVector vVel = Desc.GetXformTime() * (vSourceVel + ::PolarToVectorRadians(rAngle, rSpeed));

		//	Lifetime

		int iLifeLeft = Desc.GetParticleLifetime().Roll();

		//	Add the particle

		AddParticle(vPos, vVel, iLifeLeft, AngleToDegrees(rAngle), -1, iTick);
		}
	}

void CParticleArray::EmitComet (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick)

//	EmitComet
//
//	Emits a directional stream of particles in the shape of a comet. The head
//	of the comet will travel at maximum speed, while the tail will travel at
//	minimum speed.

	{
	const TArray<CVector> &SplinePoints = CParticlePatternEffectCreator::GetSplinePoints();
	const int SPLINE_CHOOSE_POINTS = 1000;

	int i;

	//	Compute some basic stuff

	int iCurRotation = Desc.GetXformRotation() + iDirection;
	Metric rCurRotation = mathDegreesToRadians(iCurRotation);

	//	Compute the range in speed

	Metric rMaxSpeed = Desc.GetEmitSpeed().GetMaxValue() * LIGHT_SPEED / 100.0;
	Metric rMinSpeed = Desc.GetEmitSpeed().GetMinValue() * LIGHT_SPEED / 100.0;
	Metric rSpeedRange = Max(0.01 * LIGHT_SPEED, rMaxSpeed - rMinSpeed);

	//	Compute the size of the comet (at the time we emit)

	Metric rLength = g_SecondsPerUpdate * Desc.GetEmitSpeed().GetAveValueFloat() * LIGHT_SPEED / 100.0;
	Metric rWidth = Max(0.5 * rLength, Desc.GetEmitWidth().GetMaxValue() * g_KlicksPerPixel);

	//	Use a stepper to cluster the points towards the head.

	CStepIncrementor SplinePos(CStepIncrementor::styleSquare, 0.0, SplinePoints.GetCount() - 1, SPLINE_CHOOSE_POINTS);
	CStepIncrementor WidthPos(CStepIncrementor::styleLinear, -1.0, 1.0, SPLINE_CHOOSE_POINTS);

	//	Create the particles

	for (i = 0; i < iCount; i++)
		{
		//	Pick a random position on the comet line

		const CVector &vSplinePos = SplinePoints[(int)SplinePos.GetAt(mathRandom(0, SPLINE_CHOOSE_POINTS))];

		//	Pick a random position on the width and adjust the spline

		Metric rWidthAdj = WidthPos.GetAt(mathRandom(0, SPLINE_CHOOSE_POINTS));
		CVector vSplineAdj(vSplinePos.GetX() * rLength, vSplinePos.GetY() * rWidthAdj * rWidth);

		//	Rotate the spline

		CVector vSplinePosRot = vSplineAdj.Rotate(iCurRotation + 180);
		CVector vPos = vSource + vSplinePosRot;

		//	Pick a velocity, proportional to the place on the spline

		Metric rVelAdj = vSplinePos.GetX();
		CVector vVel =  vSourceVel + ::PolarToVectorRadians(rCurRotation, rMaxSpeed - (rSpeedRange * rVelAdj));

		//	Add the particle

		AddParticle(vPos, vVel, Desc.GetParticleLifetime().Roll(), iCurRotation, -1, iTick, rVelAdj);
		}
	}

void CParticleArray::EmitExhaust (const CParticleSystemDesc &Desc, CSpaceObject *pObj, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick, CVector *retvLastSource)

//	EmitExhaust
//
//	Emits missile exhaust.

	{
	int i;

	//	Calculate a vector to our previous position
	//
	//	NOTE: In this mode m_vLastEmitPos is the last position of the object.

	CVector vCurPos = (pObj ? pObj->GetPos() : CVector());

	//	No matter what we initialize the last source position, so we're ready.

	if (retvLastSource)
		*retvLastSource = vCurPos + vSource;

	//	If we haven't yet figured out which direction to emit, then we wait.
	//	(This doesn't happen until paint time).

	if (iDirection == -1)
		return;

	//	Calc positions

	CVector vToOldPos;
	if (Desc.IsTrackingObject())
		{
		Metric rAveSpeed = Desc.GetXformTime() * Desc.GetEmitSpeed().GetAveValue() * LIGHT_SPEED / 100.0;
		vToOldPos = GetLastEmitPos() - (vCurPos + vSource) + ::PolarToVector(180 + GetLastEmitDirection(), rAveSpeed * g_SecondsPerUpdate);
		}
	else
		{
		Metric rSpeed = (pObj ? pObj->GetVel().Length() : 0.0);
		vToOldPos = ::PolarToVector(180 + GetLastEmitDirection(), rSpeed * g_SecondsPerUpdate);
		}

	//	Compute two orthogonal coordinates

	CVector vAxis = ::PolarToVector(iDirection + 180, 1.0);
	CVector vTangent = ::PolarToVector(iDirection + 90, 1.0);

	//	Width of emission

	Metric rHalfWidth = 0.5 * Desc.GetEmitWidth().Roll() * g_KlicksPerPixel;

	//	Create particles

	for (i = 0; i < iCount; i++)
		{
		Metric rSlide = mathRandom(0, 9999) / 10000.0;
		Metric rTangentSlide = mathRandom(-9999, 9999) / 10000.0;

		//	Compute a position randomly along the line between the current and
		//	last emit positions.

		CVector vPos = vSource + rSlide * vToOldPos;

		//	If we have an emit width, adjust position

		if (rHalfWidth > 0.0)
			vPos = vPos + (vTangent * (rTangentSlide * rHalfWidth));

		//	Generate a random velocity along the tangent

		Metric rAxisJitter = mathRandom(-50, 50) / 100.0;
		CVector vVel = (vTangent * rTangentSlide * Desc.GetXformTime() * Desc.GetTangentSpeed().Roll() * LIGHT_SPEED / 100.0)
				+ (vAxis * (Desc.GetEmitSpeed().Roll() + rAxisJitter) * LIGHT_SPEED / 100.0);

		//	Lifetime

		int iLifeLeft = Desc.GetParticleLifetime().Roll();

		//	Add the particle

		AddParticle(vPos, vVel, iLifeLeft, iDirection, -1, iTick);
		}
	}

void CParticleArray::EmitJet (const CParticleSystemDesc &Desc, CSpaceObject *pObj, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick)

//	EmitJet
//
//	Emits a jet of particles

	{
	//	If we haven't yet figured out which direction to emit, then we wait.
	//	(This doesn't happen until paint time).

	if (iDirection == -1)
		return;

	//	If our emit direction has changed then we need to interpolate between the two

	if (iDirection != GetLastEmitDirection())
		CreateInterpolatedParticles(Desc, pObj, iCount, vSource, vSourceVel, iDirection, iTick);

	//	Otherwise, just linear creation

	else
		CreateLinearParticles(Desc, pObj, iCount, vSource, vSourceVel, iDirection, iTick);
	}

void CParticleArray::EmitRadiate (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick)

//	EmitRadiate
//
//	Emits in a circular shell.

	{
	int i;

	//	Compute some basic stuff

	const Metric rJitterFactor = LIGHT_SPEED / 100000.0;

	//	Calculate where last tick's particles would be based on the last rotation.

	Metric rAveSpeed = Desc.GetEmitSpeed().GetAveValue() * LIGHT_SPEED / 100.0;

	//	Create particles

	for (i = 0; i < iCount; i++)
		{
		//	Choose a random angle and velocity

		Metric rAngle = 2.0 * PI * (mathRandom(0, 9999) / 10000.0);
		Metric rSpeed = (Desc.GetEmitSpeed().Roll() * LIGHT_SPEED / 100.0) + rJitterFactor * mathRandom(-500, 500);
		CVector vVel = Desc.GetXformTime() * (vSourceVel + ::PolarToVectorRadians(rAngle, rSpeed));

		//	Lifetime

		int iLifeLeft = Desc.GetParticleLifetime().Roll();

		//	Add the particle

		AddParticle(vSource, vVel, iLifeLeft, AngleToDegrees(rAngle), -1, iTick);
		}
	}

void CParticleArray::EmitSpray (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick)

//	EmitSpray
//
//	Emits a directional spray (compatible with particle weapon effects).

	{
	int i;

	//	Calculate a few temporaries

	Metric rRatedSpeed = Desc.GetEmitSpeed().GetAveValueFloat() * LIGHT_SPEED / 100.0;
	Metric rRadius = (6.0 * rRatedSpeed);

	int iSpreadAngle = Desc.GetSpreadAngle().Roll();
	if (iSpreadAngle > 0)
		iSpreadAngle = (iSpreadAngle / 2) + 1;
	bool bSpreadAngle = (iSpreadAngle > 0);

	CVector vTemp = PolarToVector(iSpreadAngle, rRatedSpeed);
	Metric rTangentV = (3.0 * vTemp.GetY());
	int iTangentAngle = (iDirection + 90) % 360;

	int iSpreadWidth = Desc.GetEmitWidth().Roll();
	Metric rSpreadWidth = iSpreadWidth * g_KlicksPerPixel;
	bool bSpreadWidth = (iSpreadWidth > 0);

	//	Create the particles with appropriate velocity

	for (i = 0; i < iCount; i++)
		{
		Metric rPlace = ((mathRandom(0, 25) + mathRandom(0, 25) + mathRandom(0, 25) + mathRandom(0, 25)) - 50.0) / 100.0;
		Metric rTangentPlace = ((mathRandom(0, 25) + mathRandom(0, 25) + mathRandom(0, 25) + mathRandom(0, 25)) - 50.0) / 100.0;
	
		CVector vPlace = PolarToVector(iDirection, rRadius * rPlace);
		CVector vVel = PolarToVector(iDirection, rRatedSpeed)
				+ (0.05 * vPlace)
				+ PolarToVector(iTangentAngle, rTangentV * rTangentPlace);

		//	Compute the spread width

		CVector vPos = vSource + vPlace;
		if (bSpreadWidth)
			vPos = vPos + PolarToVector(iTangentAngle, rSpreadWidth * rTangentPlace);

		//	Compute the travel rotation for these particles

		int iRotation = (bSpreadAngle ? VectorToPolar(vVel) : iDirection);

		//	Create the particle

		AddParticle(vPos, vSourceVel + vVel, Desc.GetParticleLifetime().Roll(), iRotation, -1, iTick);
		}
	}

void CParticleArray::GetBounds (CVector *retvUR, CVector *retvLL)

//	GetBounds
//
//	Returns the bounds of the particles

	{
	if (m_bUseRealCoords)
		{
		*retvUR = m_vOrigin + m_vUR;
		*retvLL = m_vOrigin + m_vLL;
		}
	else
		{
		*retvUR = m_vOrigin + CVector(m_rcBounds.right * g_KlicksPerPixel, -m_rcBounds.top * g_KlicksPerPixel);
		*retvLL = m_vOrigin + CVector(m_rcBounds.left * g_KlicksPerPixel, -m_rcBounds.bottom * g_KlicksPerPixel);
		}
	}

void CParticleArray::Init (int iMaxCount, const CVector &vOrigin)

//	Init
//
//	Initializes the particle array

	{
	CleanUp();

	if (iMaxCount > 0)
		{
		m_pArray = new SParticle [iMaxCount];
		utlMemSet(m_pArray, sizeof(SParticle) * iMaxCount, 0);

		m_iCount = iMaxCount;
		}

	m_vOrigin = vOrigin;
	}

void CParticleArray::Move (const CVector &vMove)

//	Move
//
//	Moves all particles by the given vector

	{
	UseRealCoords();

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;
	int iParticleCount = 0;
	CVector vTotalPos;

	//	Keep track of bounds

	Metric xLeft = g_InfiniteDistance;
	Metric xRight = -g_InfiniteDistance;
	Metric yTop = -g_InfiniteDistance;
	Metric yBottom = g_InfiniteDistance;

	//	Loop

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			iParticleCount++;

			//	Update position

			pParticle->Pos = pParticle->Pos + vMove;

			//	Convert to integer
			//	NOTE: If we're using real coords we always ignore integer
			//	velocity.

			PosToXY(pParticle->Pos, &pParticle->x, &pParticle->y);

			//	Update the bounding box

			if (pParticle->Pos.GetX() > xRight)
				xRight = pParticle->Pos.GetX();
			if (pParticle->Pos.GetX() < xLeft)
				xLeft = pParticle->Pos.GetX();

			if (pParticle->Pos.GetY() < yBottom)
				yBottom = pParticle->Pos.GetY();
			if (pParticle->Pos.GetY() > yTop)
				yTop = pParticle->Pos.GetY();

			//	Update center of mass

			vTotalPos = vTotalPos + pParticle->Pos;
			}

		//	Next

		pParticle++;
		}

	//	Set bounds

	m_vUR = CVector(xRight, yTop);
	m_vLL = CVector(xLeft, yBottom);

	int xiRight, xiLeft, yiTop, yiBottom;
	PosToXY(m_vUR, &xiRight, &yiTop);
	PosToXY(m_vLL, &xiLeft, &yiBottom);

	m_rcBounds.left = xiLeft / FIXED_POINT;
	m_rcBounds.top = yiTop / FIXED_POINT;
	m_rcBounds.right = xiRight / FIXED_POINT;
	m_rcBounds.bottom = yiBottom / FIXED_POINT;

	//	Center of mass

	m_vCenterOfMass = (iParticleCount > 0 ? vTotalPos / (Metric)iParticleCount : NullVector);
	}

void CParticleArray::Paint (const CParticleSystemDesc &Desc, CG32bitImage &Dest, int xPos, int yPos, IEffectPainter *pPainter, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint all particles

	{
	if (pPainter == NULL)
		return;

	//	See if the painter has a paint descriptor (which is faster for us).

	SParticlePaintDesc PaintDesc;
	if (pPainter->GetParticlePaintDesc(&PaintDesc))
		{
		PaintDesc.iMaxLifetime = Desc.GetParticleLifetime().GetMaxValue();
		Paint(Dest, xPos, yPos, Ctx, PaintDesc);
		}

	//	Otherwise, we use the painter for each particle

	else
		Paint(Dest, xPos, yPos, Ctx, pPainter, Desc.GetEmitSpeed().GetAveValueFloat() * LIGHT_SECOND / 100.0);

#ifdef DEBUG_PARTICLE_BOUNDS
	CVector vUR;
	CVector vLL;

	GetBounds(&vUR, &vLL);
	int xUR, yUR;
	Ctx.XForm.Transform(vUR, &xUR, &yUR);

	int xLL, yLL;
	Ctx.XForm.Transform(vLL, &xLL, &yLL);

	Dest.DrawLine(xUR, yUR, xUR, yLL, 1, CG32bitPixel(255, 255, 0));
	Dest.DrawLine(xUR, yLL, xLL, yLL, 1, CG32bitPixel(255, 255, 0));
	Dest.DrawLine(xLL, yLL, xLL, yUR, 1, CG32bitPixel(255, 255, 0));
	Dest.DrawLine(xLL, yUR, xUR, yUR, 1, CG32bitPixel(255, 255, 0));
#endif
	}

void CParticleArray::Paint (CG32bitImage &Dest,
							int xPos,
							int yPos,
							SViewportPaintCtx &Ctx,
							SParticlePaintDesc &Desc)

//	Paint
//
//	Paint all particles

	{
	//	Paint based on style

	switch (Desc.iStyle)
		{
		case paintFlame:
			{
			int iCore = Desc.iMaxLifetime / 6;
			int iFlame = Desc.iMaxLifetime / 2;
			int iSmoke = 3 * Desc.iMaxLifetime / 4;
			int iSmokeBrightness = 80;

			PaintFireAndSmoke(Dest, 
					xPos, yPos, 
					Ctx, 
					Desc.iMaxLifetime,
					Desc.iMinWidth,
					Desc.iMaxWidth, 
					iCore, 
					iFlame, 
					iSmoke, 
					iSmokeBrightness);
			break;
			}

		case paintGlitter:
			{
			PaintGlitter(Dest, xPos, yPos, Ctx, Desc.iMaxWidth, Desc.rgbPrimaryColor, Desc.rgbSecondaryColor);
			break;
			}

		case paintImage:
			PaintImage(Dest, xPos, yPos, Ctx, Desc);
			break;

		case paintLine:
			PaintLine(Dest, xPos, yPos, Ctx, DEFAULT_LINE_LENGTH, Desc.rgbPrimaryColor, Desc.rgbSecondaryColor);
			break;

		case paintSmoke:
			{
			int iCore = 1;
			int iFlame = Desc.iMaxLifetime / 6;
			int iSmoke = Desc.iMaxLifetime / 4;
			int iSmokeBrightness = 60;

			PaintFireAndSmoke(Dest, 
					xPos, yPos, 
					Ctx, 
					Desc.iMaxLifetime,
					Desc.iMinWidth,
					Desc.iMaxWidth, 
					iCore, 
					iFlame, 
					iSmoke, 
					iSmokeBrightness);
			break;
			}

		default:
			PaintGaseous(Dest,
					xPos, yPos,
					Ctx,
					Desc.iMaxLifetime,
					Desc.iMinWidth,
					Desc.iMaxWidth,
					Desc.rgbPrimaryColor,
					Desc.rgbSecondaryColor);
			break;
		}
	}

void CParticleArray::Paint (CG32bitImage &Dest,
							int xPos,
							int yPos,
							SViewportPaintCtx &Ctx,
							IEffectPainter *pPainter,
							Metric rRatedSpeed)

//	Paint
//
//	Paint using a painter for each particle

	{
	int iSavedTick = Ctx.iTick;
	int iSavedDestiny = Ctx.iDestiny;
	int iSavedRotation = Ctx.iRotation;
	int iSavedMaxLength = Ctx.iMaxLength;

	//	If necessary set the max length based on the rated speed at the current
	//	tick count.

	Metric rMaxLengthFactor = 0.0;
	bool bNeedMaxLength = (rRatedSpeed > 0.0);
	if (bNeedMaxLength)
		rMaxLengthFactor = g_SecondsPerUpdate * rRatedSpeed / g_KlicksPerPixel;

	//	Loop

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			//	Compute the position of the particle

			int x = xPos + pParticle->x / FIXED_POINT;
			int y = yPos + pParticle->y / FIXED_POINT;

			//	Paint the particle

			Ctx.iTick = Max(0, iSavedTick - pParticle->iGeneration);
			Ctx.iDestiny = pParticle->iDestiny;
			Ctx.iRotation = pParticle->iRotation;
			if (bNeedMaxLength)
				Ctx.iMaxLength = (int)(Max(1, Ctx.iTick) * rMaxLengthFactor);

			pPainter->Paint(Dest, x, y, Ctx);
			}

		//	Next

		pParticle++;
		}

	Ctx.iTick = iSavedTick;
	Ctx.iDestiny = iSavedDestiny;
	Ctx.iRotation = iSavedRotation;
	Ctx.iMaxLength = iSavedMaxLength;
	}

void CParticleArray::PaintFireAndSmoke (CG32bitImage &Dest, 
										int xPos, 
										int yPos, 
										SViewportPaintCtx &Ctx, 
										int iLifetime,
										int iMinWidth,
										int iMaxWidth,
										int iCore,
										int iFlame,
										int iSmoke,
										int iSmokeBrightness)

//	PaintFireAndSmoke
//
//	Paints each particle as fire/smoke particle. 
//
//	iLifetime is the point at which the particle fades completely
//	iWidth is the maximum width of the particle

	{
	//	We don't support infinite lifetime here
	ASSERT(iLifetime >= 0);

	iCore = Max(iCore, 1);
	iFlame = Max(iFlame, iCore + 1);
	iSmoke = Max(iSmoke, iFlame + 1);

	iMinWidth = Max(1, iMinWidth);
	iMaxWidth = Max(iMinWidth, iMaxWidth);
	int iWidthRange = (iMaxWidth - iMinWidth) + 1;

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			int iLifeLeft = (pParticle->iLifeLeft == -1 ? iLifetime : Min(pParticle->iLifeLeft, iLifetime));
			int iAge = iLifetime - iLifeLeft;

			//	Compute properties of the particle based on its life

			CG32bitPixel rgbColor = 0;
			int iFade = 0;
			int iFade2 = 0;
			int iWidth = 0;

			if (iLifetime > 0)
				{
				//	Particle fades out over time

				iFade = Max(20, Min(255, (255 * iLifeLeft / iLifetime)));
				iFade2 = iFade / 2;

				//	Particle grows over time

				iWidth = iMinWidth + (iWidthRange * iAge / iLifetime);

				//	Smoke color

				int iDarkness = Min(255, iSmokeBrightness + (2 * (pParticle->iDestiny % 25)));
				CG32bitPixel rgbSmokeColor = CG32bitPixel::FromGrayscale(iDarkness);

				//	Some particles are gray

				CG32bitPixel rgbFadeColor;
				if ((pParticle->iDestiny % 4) != 0)
					rgbFadeColor = FLAME_OUTER_COLOR;
				else
					rgbFadeColor = rgbSmokeColor;

				//	Particle color changes over time

				if (iAge <= iCore)
					rgbColor = CG32bitPixel::Fade(FLAME_CORE_COLOR,
							FLAME_MIDDLE_COLOR,
							100 * iAge / iCore);
				else if (iAge <= iFlame)
					rgbColor = CG32bitPixel::Fade(FLAME_MIDDLE_COLOR,
							rgbFadeColor,
							100 * (iAge - iCore) / (iFlame - iCore));
				else if (iAge <= iSmoke)
					rgbColor = CG32bitPixel::Fade(rgbFadeColor,
							rgbSmokeColor,
							100 * (iAge - iFlame) / (iSmoke - iFlame));
				else
					rgbColor = rgbSmokeColor;
				}

			//	Compute the position of the particle

			int x = xPos + pParticle->x / FIXED_POINT;
			int y = yPos + pParticle->y / FIXED_POINT;

			//	Paint the particle

			PAINT_GASEOUS_PARTICLE(Dest, x, y, iWidth, rgbColor, iFade, iFade2);
			}

		//	Next

		pParticle++;
		}
	}

void CParticleArray::PaintGaseous (CG32bitImage &Dest,
								   int xPos,
								   int yPos,
								   SViewportPaintCtx &Ctx,
								   int iMaxLifetime,
								   int iMinWidth,
								   int iMaxWidth,
								   CG32bitPixel rgbPrimaryColor,
								   CG32bitPixel rgbSecondaryColor)

//	PaintGaseous
//
//	Paints gaseous particles that fade from primary color to secondary color

	{
	ASSERT(iMaxLifetime >= 0);

	iMinWidth = Max(1, iMinWidth);
	iMaxWidth = Max(iMinWidth, iMaxWidth);
	int iWidthRange = (iMaxWidth - iMinWidth) + 1;

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			int iLifeLeft = (pParticle->iLifeLeft == -1 ? iMaxLifetime : Min(pParticle->iLifeLeft, iMaxLifetime));
			int iAge = iMaxLifetime - iLifeLeft;

			//	Compute properties of the particle based on its life

			CG32bitPixel rgbColor = 0;
			int iFade = 0;
			int iFade2 = 0;
			int iWidth = 0;

			if (iMaxLifetime > 0)
				{
				//	Particle fades out over time

				iFade = Max(20, Min(255, (255 * iLifeLeft / iMaxLifetime)));
				iFade2 = iFade / 2;

				//	Particle grows over time

				iWidth = iMinWidth + (iWidthRange * iAge / iMaxLifetime);

				//	Particle color fades from primary to secondary

				rgbColor = CG32bitPixel::Fade(rgbPrimaryColor, rgbSecondaryColor, 100 * iAge / iMaxLifetime);
				}

			//	Compute the position of the particle

			int x = xPos + pParticle->x / FIXED_POINT;
			int y = yPos + pParticle->y / FIXED_POINT;

			//	Paint the particle

			PAINT_GASEOUS_PARTICLE(Dest, x, y, iWidth, rgbColor, iFade, iFade2);
			}

		//	Next

		pParticle++;
		}
	}

void CParticleArray::PaintGlitter (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, int iWidth, CG32bitPixel rgbPrimaryColor, CG32bitPixel rgbSecondaryColor)

//	PaintGlitter
//
//	Paints particles as glitter

	{
	const int HIGHLIGHT_ANGLE = 135;
	const int HIGHLIGHT_RANGE = 30;

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			//	Color flips to secondary color at certain angles

			int iBearing = Absolute(AngleBearing(HIGHLIGHT_ANGLE, pParticle->iRotation));
			CG32bitPixel rgbColor = (iBearing < HIGHLIGHT_RANGE ? CG32bitPixel::Fade(rgbSecondaryColor, rgbPrimaryColor, iBearing * 100 / HIGHLIGHT_RANGE) : rgbPrimaryColor);

			//	Compute the position of the particle

			int x = xPos + pParticle->x / FIXED_POINT;
			int y = yPos + pParticle->y / FIXED_POINT;

			//	Draw at appropriate size

			switch (iWidth)
				{
				case 0:
					Dest.SetPixel(x, y, rgbColor);
					break;

				case 1:
					Dest.SetPixel(x, y, rgbColor);
					Dest.SetPixelTrans(x + 1, y, rgbColor, 0x80);
					Dest.SetPixelTrans(x, y + 1, rgbColor, 0x80);
					break;

				case 2:
					Dest.SetPixel(x, y, rgbColor);
					Dest.SetPixelTrans(x + 1, y, rgbColor, 0x80);
					Dest.SetPixelTrans(x, y + 1, rgbColor, 0x80);
					Dest.SetPixelTrans(x - 1, y, rgbColor, 0x80);
					Dest.SetPixelTrans(x, y - 1, rgbColor, 0x80);
					break;

				case 3:
					Dest.SetPixel(x, y, rgbColor);
					Dest.SetPixel(x + 1, y, rgbColor);
					Dest.SetPixel(x, y + 1, rgbColor);
					Dest.SetPixel(x - 1, y, rgbColor);
					Dest.SetPixel(x, y - 1, rgbColor);
					Dest.SetPixelTrans(x + 1, y + 1, rgbColor, 0x80);
					Dest.SetPixelTrans(x + 1, y - 1, rgbColor, 0x80);
					Dest.SetPixelTrans(x - 1, y + 1, rgbColor, 0x80);
					Dest.SetPixelTrans(x - 1, y - 1, rgbColor, 0x80);
					break;

				default:
					CGDraw::Circle(Dest, x, y, (iWidth + 1) / 2, rgbColor);
					break;
				}
			}

		//	Next

		pParticle++;
		}
	}

void CParticleArray::PaintImage (CG32bitImage &Dest, int xPos, int yPos, SViewportPaintCtx &Ctx, SParticlePaintDesc &Desc)

//	Paint
//
//	Paints particle as an image

	{
	int iRotationFrame = 0;
	if (Desc.bDirectional)
		iRotationFrame = Angle2Direction(Ctx.iRotation, Desc.iVariants);

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			//	Figure out the animation frame to paint

			int iTick;
			if (Desc.bRandomStartFrame)
				iTick = Ctx.iTick + pParticle->iDestiny;
			else
				iTick = Ctx.iTick;

			//	Figure out the rotation or variant to paint

			//int iFrame = (Desc.bDirectional ? iRotationFrame : (pParticle->iDestiny % Desc.iVariants));
			int iFrame = (Desc.bDirectional ? Angle2Direction(AngleMod(Ctx.iRotation + pParticle->iRotation), Desc.iVariants) : (pParticle->iDestiny % Desc.iVariants));

			//	Compute the position of the particle

			int x = xPos + pParticle->x / FIXED_POINT;
			int y = yPos + pParticle->y / FIXED_POINT;

			//	Paint the particle

			Desc.pImage->PaintImage(Dest, x, y, iTick, iFrame);
			}

		//	Next

		pParticle++;
		}
	}

void CParticleArray::PaintLine (CG32bitImage &Dest,
								int xPos,
								int yPos,
								SViewportPaintCtx &Ctx,
								int iLength,
								CG32bitPixel rgbPrimaryColor,
								CG32bitPixel rgbSecondaryColor)

//	PaintLine
//
//	Paints particle as a line

	{
	//	We want the tail to be transparent and the head to be full.
	//	NOTE: We paint from head to tail.

	CG32bitPixel rgbFrom = rgbPrimaryColor;
	CG32bitPixel rgbTo = CG32bitPixel(rgbSecondaryColor, 0);

	//	Paint all the particles

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			//	Compute the position of the particle

			int xFrom = xPos + pParticle->x / FIXED_POINT;
			int yFrom = yPos + pParticle->y / FIXED_POINT;

			CVector vTail = PolarToVector(pParticle->iRotation, iLength);

			int xTo = xFrom - (int)vTail.GetX();
			int yTo = yFrom + (int)vTail.GetY();

			//	Paint the particle

			CGDraw::LineGradient(Dest, xFrom, yFrom, xTo, yTo, 1, rgbFrom, rgbTo);
			}

		//	Next

		pParticle++;
		}
	}

void CParticleArray::PosToXY (const CVector &xy, int *retx, int *rety)

//	PosToXY
//
//	Convert from system coordinates to particle coordinates

	{
	*retx = (int)(FIXED_POINT * xy.GetX() / g_KlicksPerPixel);
	*rety = -(int)(FIXED_POINT * xy.GetY() / g_KlicksPerPixel);
	}

void CParticleArray::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream

	{
	int i;
	DWORD dwLoad;

	CleanUp();

	//	Load the count

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	if (dwLoad > 0x00100000)
		return;

	m_iCount = dwLoad;

	//	Origin

	if (Ctx.dwVersion >= 67)
		{
		Ctx.pStream->Read((char *)&m_vOrigin, sizeof(CVector));
		Ctx.pStream->Read((char *)&m_vCenterOfMass, sizeof(CVector));
		Ctx.pStream->Read((char *)&m_vUR, sizeof(CVector));
		Ctx.pStream->Read((char *)&m_vLL, sizeof(CVector));
		}

	//	Last emit info

	if (Ctx.dwVersion >= 120)
		{
		Ctx.pStream->Read((char *)&m_vLastEmitSource, sizeof(CVector));
		Ctx.pStream->Read((char *)&m_vLastEmitSourceVel, sizeof(CVector));
		Ctx.pStream->Read((char *)&m_iLastEmitDirection, sizeof(DWORD));
		}

	//	Load flags

	if (Ctx.dwVersion >= 64)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_bUseRealCoords =			((dwLoad & 0x00000001) ? true : false);
		}
	else
		{
		m_bUseRealCoords = false;
		}

	//	If no particles, then we're done

	if (m_iCount == 0)
		{
		m_pArray = NULL;
		return;
		}

	//	Load the particles

	m_pArray = new SParticle [m_iCount];
	
	//	Previous version didn't have everything

	if (Ctx.dwVersion >= 120)
		Ctx.pStream->Read((char *)m_pArray, sizeof(SParticle) * m_iCount);

	else if (Ctx.dwVersion >= 119)
		{
		SParticle119 *pOldArray = new SParticle119[m_iCount];
		Ctx.pStream->Read((char *)pOldArray, sizeof(SParticle119) * m_iCount);

		for (i = 0; i < m_iCount; i++)
			{
			m_pArray[i].Pos = pOldArray[i].Pos;
			m_pArray[i].Vel = pOldArray[i].Vel;
			m_pArray[i].x = pOldArray[i].x;
			m_pArray[i].y = pOldArray[i].y;
			m_pArray[i].xVel = pOldArray[i].xVel;
			m_pArray[i].yVel = pOldArray[i].yVel;
			m_pArray[i].iGeneration = pOldArray[i].iGeneration;
			m_pArray[i].iLifeLeft = pOldArray[i].iLifeLeft;
			m_pArray[i].iDestiny = pOldArray[i].iDestiny;
			m_pArray[i].iRotation = pOldArray[i].iRotation;
			m_pArray[i].rData = pOldArray[i].dwData;
			m_pArray[i].fAlive = pOldArray[i].fAlive;
			}
		}
	else if (Ctx.dwObjClassID >= 64)
		{
		SParticle64 *pOldArray = new SParticle64[m_iCount];
		Ctx.pStream->Read((char *)pOldArray, sizeof(SParticle64) * m_iCount);

		for (i = 0; i < m_iCount; i++)
			{
			m_pArray[i].Pos = pOldArray[i].Pos;
			m_pArray[i].Vel = pOldArray[i].Vel;
			m_pArray[i].x = pOldArray[i].x;
			m_pArray[i].y = pOldArray[i].y;
			m_pArray[i].xVel = pOldArray[i].xVel;
			m_pArray[i].yVel = pOldArray[i].yVel;
			m_pArray[i].iGeneration = 0;
			m_pArray[i].iLifeLeft = pOldArray[i].iLifeLeft;
			m_pArray[i].iDestiny = pOldArray[i].iDestiny;
			m_pArray[i].iRotation = pOldArray[i].iRotation;
			m_pArray[i].rData = pOldArray[i].dwData;
			m_pArray[i].fAlive = pOldArray[i].fAlive;
			}
		}
	else
		{
		for (i = 0; i < m_iCount; i++)
			{
			Ctx.pStream->Read((char *)&m_pArray[i].x, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].y, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].xVel, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].yVel, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].iLifeLeft, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].iDestiny, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].iRotation, sizeof(DWORD));

			DWORD dwData;
			Ctx.pStream->Read((char *)&dwData, sizeof(DWORD));
			m_pArray[i].rData = (Metric)dwData;

			Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
			m_pArray[i].fAlive = ((dwLoad & 0x00000001) ? true : false);

			//	See if we need to compute real coords

			if (m_bUseRealCoords)
				{
				m_pArray[i].Pos = XYToPos(m_pArray[i].x, m_pArray[i].y);
				m_pArray[i].Vel = XYToPos(m_pArray[i].xVel, m_pArray[i].yVel);
				}
			}
		}
	}

void CParticleArray::ResetLastEmit (int iLastDirection, const CVector &vLastEmitPos, const CVector &vLastEmitVel)

//	ResetLastEmit
//
//	Reset the last emit variables. This is called when there is a discontinuity in emission
//	(e.g., thruster turned on/off) and we don't want to interpolate from the previous
//	emit position/direction.

	{
	m_iLastEmitDirection = iLastDirection;
	m_vLastEmitSource = vLastEmitPos;

	if (!vLastEmitVel.IsNull())
		m_vLastEmitSourceVel = vLastEmitVel;
	}

void CParticleArray::Update (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx)

//	Update
//
//	Updates the array based on the context

	{
	if ((Ctx.pDamageDesc || Desc.HasWakeFactor()) && Ctx.pSystem)
		UpdateCollisions(Desc, Ctx);

	//	If we're tracking, change velocity to follow target

	if (Ctx.pTarget && Ctx.pDamageDesc && Ctx.pDamageDesc->IsTrackingTime(Ctx.iTick))
		UpdateTrackTarget(Desc, Ctx);

	//	Adjust based on style

	switch (Desc.GetStyle())
		{
		case CParticleSystemDesc::styleBrownian:
			UpdateBrownian(Desc, Ctx);
			break;

		case CParticleSystemDesc::styleComet:
			UpdateComet(Desc, Ctx);
			break;

		case CParticleSystemDesc::styleWrithe:
			UpdateWrithe(Ctx);
			break;
		}
	}

void CParticleArray::UpdateBrownian (const CParticleSystemDesc &Desc, SEffectUpdateCtx &UpdateCtx)

//	UpdateBrownian
//
//	Brownian (random) motion within a radius.

	{
	//	We only update each particle 1/30 ticks.

	const int UPDATE_CYCLE = 30;
	const Metric IMPULSE_FACTOR = 0.5;
	const Metric MAX_SPEED_FACTOR = 3.0;

	//	We need to use real coordinates instead of fixed point

	UseRealCoords();

	//	Compute some temporaries

	int iCycleTick = (UpdateCtx.iTick % UPDATE_CYCLE);

	Metric rMaxRadius = Desc.GetRadius().GetMaxValue() * g_KlicksPerPixel;
	Metric rMaxRadius2 = rMaxRadius * rMaxRadius;
	bool bCheckRadius = (rMaxRadius > 0.0);

	Metric rCohesionSpeed = g_SecondsPerUpdate * Desc.GetXformTime() * Desc.GetCohesionFactor() * Desc.GetEmitSpeed().GetAveValue() * LIGHT_SPEED / 100.0;
	Metric rImpulseSpeed = g_SecondsPerUpdate * Desc.GetXformTime() * IMPULSE_FACTOR * Desc.GetEmitSpeed().GetAveValue() * LIGHT_SPEED / 100.0;
	Metric rMaxSpeed = MAX_SPEED_FACTOR * g_SecondsPerUpdate * Desc.GetXformTime() * Desc.GetEmitSpeed().GetMaxValue() * LIGHT_SPEED / 100.0;

	//	Loop over all particles

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	CVector vToCenter;
	Metric rDist2;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			//	We only update each particle 1/30 ticks.

			if ((pParticle->iDestiny % UPDATE_CYCLE) != iCycleTick)
				{ }

			//	If we're outside the maximum radius, then we need to push the
			//	particle back towards the center.

			else if (bCheckRadius
					&& (rDist2 = (vToCenter = (m_vLastEmitSource - pParticle->Pos)).Length2()) > rMaxRadius2)
				{
				Metric rDist = sqrt(rDist2);
				CVector vImpulse = (rCohesionSpeed / rDist) * vToCenter;

				pParticle->Vel = pParticle->Vel + vImpulse;
				pParticle->Vel.Clip(rMaxSpeed);
				pParticle->iRotation = VectorToPolar(pParticle->Vel);
				}

			//	Otherwise we give the particle a random impulse

			else
				{
				Metric rAngle = 2.0 * PI * (mathRandom(0, 9999) / 10000.0);
				pParticle->Vel = pParticle->Vel + ::PolarToVectorRadians(rAngle, rImpulseSpeed);
				pParticle->iRotation = VectorToPolar(pParticle->Vel);
				}
			}

		//	Next

		pParticle++;
		}
	}

void CParticleArray::UpdateCollisions (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx)

//	UpdateCollisions
//
//	Update particle collisions, including damage

	{
	int i;

	//	We need real coordinates for this

	UseRealCoords();

	//	Compute some values

	int iSplashChance = Desc.GetSplashChance();
	int iGhostChance = Desc.GetMissChance();

	//	Compute the velocity of the effect object in Km/tick

	CVector vEffectVel;
	if (Desc.HasWakeFactor())
		vEffectVel = Ctx.pObj->GetVel() * g_SecondsPerUpdate;

	//	Compute bounds

	CVector vUR;
	CVector vLL;
	GetBounds(&vUR, &vLL);

	//	Loop over all objects in the system

	for (i = 0; i < Ctx.pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = Ctx.pSystem->GetObject(i);

		//	If the object is in the bounding box then remember
		//	it so that we can do a more accurate calculation.

		if (pObj 
				&& pObj->InBox(vUR, vLL)
				&& Ctx.pObj->CanHit(pObj) 
				&& pObj->CanBeHit() 
				&& !pObj->IsDestroyed()
				&& pObj != Ctx.pObj)
			{

			//	See if this object can be hit by the damage

			bool bDoDamage = (Ctx.pDamageDesc && pObj->CanBeHitBy(Ctx.pDamageDesc->GetDamage()));

			//	Keep track of some values if we're doing damage

			int iHitCount = 0;
			CVector vTotalHitPos;

			//	Compute some values if we're affecting particles

			CVector vVelN;
			CVector vVelT;
			Metric rObjVel;
			int iObjMotionAngle;
			bool bDoWake = false;
			if (Desc.HasWakeFactor())
				{
				Metric rSpeed = pObj->GetVel().Length();
				if (rSpeed > 0.0)
					{
					//	Compute the velocity components of the object that will
					//	affect the particles

					vVelN = pObj->GetVel() / rSpeed;
					vVelT = vVelN.Perpendicular();

					//	Object speed in Km/tick

					rObjVel = rSpeed * g_SecondsPerUpdate;

					//	Compute the objects direction

					iObjMotionAngle = VectorToPolar(vVelN);

					bDoWake = true;
					}
				}

			//	If we're not doing damage or doing a wake, then we can skip

			if (!bDoDamage && !bDoWake)
				continue;

			//	Initialize context for hit testing

			SPointInObjectCtx PIOCtx;
			pObj->PointInObjectInit(PIOCtx);

			//	Loop over all particles

			SParticle *pParticle = m_pArray;
			SParticle *pEnd = pParticle + m_iCount;
			bool bNoParticlesLeft = true;

			while (pParticle < pEnd)
				{
				if (pParticle->fAlive)
					{
					bool bHit = false;

					//	There is at least one particle alive

					bNoParticlesLeft = false;

					//	If we're doing a true hit test then we need a pretty accurate test

					if (bDoDamage)
						{
						//	Compute the current position of the particle and the
						//	half-way position of the particle

						CVector vCurPos = m_vOrigin + pParticle->Pos;
						CVector vHalfPos = vCurPos - (pParticle->Vel / 2.0f);

						//	First check to see if the new position hit the object

						if (pObj->PointInObject(PIOCtx, pObj->GetPos(), vCurPos))
							{
							bHit = true;
							vTotalHitPos = vTotalHitPos + vCurPos;
							iHitCount++;
							}

						//	Otherwise, check the half-way point

						else if (pObj->PointInObject(PIOCtx, pObj->GetPos(), vHalfPos))
							{
							bHit = true;
							vTotalHitPos = vTotalHitPos + vHalfPos;
							iHitCount++;
							}
						}

					//	Otherwise we just check bounds.

					else
						{
						if (pObj->PointInObject(PIOCtx, pObj->GetPos(), m_vOrigin + pParticle->Pos))
							bHit = true;
						}


					//	If we hit, deal with it

					if (bHit)
						{
						//	Some particles hit the object

						if (bDoDamage && (iGhostChance == 0 || mathRandom(1, 100) <= iGhostChance))
							{
							//	Some of the particles bounce

							if (iSplashChance && mathRandom(1, 100) <= iSplashChance)
								{
								Metric rSpeed;
								int iDir = VectorToPolar(pParticle->Vel, &rSpeed);

								iDir = (iDir + 180 + mathRandom(-60, 60) + mathRandom(-60, 60)) % 360;
								CVector vNewVel = PolarToVector(iDir, mathRandom(5, 30) * rSpeed / 100.0);
								pParticle->Vel = vNewVel;

								if (pParticle->iLifeLeft != -1)
									pParticle->iLifeLeft = Max(1, pParticle->iLifeLeft - mathRandom(2, 5));
								}
							else
								pParticle->fAlive = false;
							}

						//	Surviving particles make be influenced

						else if (bDoWake)
							{
							//	Compute whether the particle is to the left or right of the object
							//	along the object's line of motion.
							//
							//	We start by computing the particle's position relative to object

							CVector vRelPos = (m_vOrigin + pParticle->Pos) - pObj->GetPos();
							int iRelAngle = VectorToPolar(vRelPos);

							//	Compute the bearing (>0 is left; <0 is right)

							int iBearing = AngleBearing(iObjMotionAngle, iRelAngle);

							//	Decompose the particle's velocity along the object's motion

							CVector vParticleVel = vEffectVel + pParticle->Vel;
							Metric rParticleVelLine = vParticleVel.Dot(vVelN);
							Metric rParticleVelPerp = vParticleVel.Dot(vVelT);

							//	Compute the maximum speed of the particle

							Metric rMaxSpeed = Max(Desc.GetWakeFactor() * rObjVel, pParticle->Vel.Length());

							//	Figure out how we affect the particle speed along the object's motion

							Metric rNewVelLine = rParticleVelLine + (Desc.GetWakeFactor() * rObjVel);
							Metric rNewVelPerp = rParticleVelPerp + (Desc.GetWakeFactor() * rObjVel);
							if (iBearing > 0)
								rNewVelPerp = -rNewVelPerp;

							//	Generate the new velocity

							CVector vNewVel = (rNewVelLine * vVelN) + (rNewVelPerp * vVelT);
							vNewVel.Clip(rMaxSpeed);

							//	Set the particle velocity

							pParticle->Vel = vNewVel - vEffectVel;
							}
						}
					}

				//	Next

				pParticle++;
				}

			//	If we hit the object, then add to the list

			if (bDoDamage && iHitCount > 0 && Ctx.iTotalParticleCount > 0)
				{
				//	Add to the list

				SEffectHitDesc *pHit = Ctx.Hits.Insert();
				pHit->pObjHit = pObj;
				pHit->vHitPos = (vTotalHitPos / iHitCount);
				pHit->iHitStrength = iHitCount;

				//	Compute damage

				int iTotal = Ctx.pDamageDesc->GetDamage().RollDamage() * pHit->iHitStrength;
				int iDamage = iTotal / Ctx.iTotalParticleCount;
				if (mathRandom(1, 100) <= (iTotal % Ctx.iTotalParticleCount) * 100 / Ctx.iTotalParticleCount)
					iDamage++;

				//	Some weapons always do minimum damage.

				iDamage = Max(Ctx.pDamageDesc->GetMinDamage(), iDamage);

				//	Do damage

				SDamageCtx DamageCtx(pObj,
						Ctx.pDamageDesc,
						Ctx.pEnhancements,
						Ctx.Attacker,
						Ctx.pObj,
						VectorToPolar(pHit->vHitPos - pObj->GetPos()),
						pHit->vHitPos,
						iDamage);

				pObj->Damage(DamageCtx);
				}

			//	If there are no particles left, then nothing more will happen
			
			if (bNoParticlesLeft)
				{
				Ctx.bDestroy = true;
				return;
				}
			}
		}
	}

void CParticleArray::UpdateComet (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx)

//	UpdateComet
//
//	Update comet velocity

	{
	const Metric SPEED_CONVERGE_FACTOR = 0.95;

	//	Calculate some basic metrics

	UseRealCoords();
	Metric rMaxSpeed = Desc.GetEmitSpeed().GetMaxValue() * LIGHT_SPEED / 100.0;
	Metric rMinSpeed = Desc.GetEmitSpeed().GetMinValue() * LIGHT_SPEED / 100.0;
	Metric rSpeedRange = Max(0.01 * LIGHT_SPEED, rMaxSpeed - rMinSpeed);

	//	Loop over all particles and adjust their velocity so
	//	that they all match max speed.

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive
				&& pParticle->rData > 0.0)
			{
			Metric rVelAdj = SPEED_CONVERGE_FACTOR * pParticle->rData;
			if (rVelAdj < 0.01)
				rVelAdj = 0.0;

			pParticle->Vel = g_SecondsPerUpdate * (m_vLastEmitSourceVel + ::PolarToVector(pParticle->iRotation, rMaxSpeed - (rSpeedRange * rVelAdj)));
			pParticle->rData = rVelAdj;
			}

		//	Next

		pParticle++;
		}
	}

void CParticleArray::UpdateMotionLinear (bool *retbAlive, CVector *retvAveragePos)

//	UpdateMotionLinear
//
//	Updates the position of all particles

	{
	//	If we've been asked for the average position, then we
	//	need to use real coordinates

	if (retvAveragePos)
		UseRealCoords();

	//	See if all particles are dead

	bool bAllParticlesDead = true;				//	If true, all dead after processing

	//	Keep track of center of mass

	bool bCalcCenterOfMass = m_bUseRealCoords;
	int iParticleCount = 0;
	CVector vTotalPos;

	//	Loop

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	//	Different loops depending on whether we are using
	//	real coordinates or not.

	if (m_bUseRealCoords)
		{
		//	Keep track of bounds

		Metric xLeft = g_InfiniteDistance;
		Metric xRight = -g_InfiniteDistance;
		Metric yTop = -g_InfiniteDistance;
		Metric yBottom = g_InfiniteDistance;

		//	Loop

		while (pParticle < pEnd)
			{
			if (pParticle->fAlive)
				{
				//	Update lifetime. If -1, then we're immortal

				if (pParticle->iLifeLeft == -1
						|| (--pParticle->iLifeLeft > 0))
					{
					iParticleCount++;

					//	Update position

					pParticle->Pos = pParticle->Pos + pParticle->Vel;

					//	Convert to integer
					//	NOTE: If we're using real coords we always ignore integer
					//	velocity.

					PosToXY(pParticle->Pos, &pParticle->x, &pParticle->y);

					//	Update the bounding box

					if (pParticle->Pos.GetX() > xRight)
						xRight = pParticle->Pos.GetX();
					if (pParticle->Pos.GetX() < xLeft)
						xLeft = pParticle->Pos.GetX();

					if (pParticle->Pos.GetY() < yBottom)
						yBottom = pParticle->Pos.GetY();
					if (pParticle->Pos.GetY() > yTop)
						yTop = pParticle->Pos.GetY();

					//	Update center of mass

					if (bCalcCenterOfMass)
						vTotalPos = vTotalPos + pParticle->Pos;

					bAllParticlesDead = false;
					}

				//	If we hit 0, then we're dead

				else
					pParticle->fAlive = false;
				}

			//	Next

			pParticle++;
			}

		//	Set bounds

		m_vUR = (bAllParticlesDead ? CVector() : CVector(xRight, yTop));
		m_vLL = (bAllParticlesDead ? CVector() : CVector(xLeft, yBottom));

		int xiRight, xiLeft, yiTop, yiBottom;
		PosToXY(m_vUR, &xiRight, &yiTop);
		PosToXY(m_vLL, &xiLeft, &yiBottom);

		m_rcBounds.left = xiLeft / FIXED_POINT;
		m_rcBounds.top = yiTop / FIXED_POINT;
		m_rcBounds.right = xiRight / FIXED_POINT;
		m_rcBounds.bottom = yiBottom / FIXED_POINT;

		//	Center of mass

		m_vCenterOfMass = (iParticleCount > 0 ? vTotalPos / (Metric)iParticleCount : NullVector);
		if (retvAveragePos)
			*retvAveragePos = m_vCenterOfMass;
		}
	else
		{
		//	Keep track of our bounds

		int xLeft = 0;
		int xRight = 0;
		int yTop = 0;
		int yBottom = 0;

		//	Loop

		while (pParticle < pEnd)
			{
			if (pParticle->fAlive)
				{
				if (pParticle->iLifeLeft == -1
						|| (--pParticle->iLifeLeft > 0))
					{
					bAllParticlesDead = false;

					//	Update position

					pParticle->x += pParticle->xVel;
					pParticle->y += pParticle->yVel;

					//	Update the bounding box

					if (pParticle->x > xRight)
						xRight = pParticle->x;
					if (pParticle->x < xLeft)
						xLeft = pParticle->x;

					if (pParticle->y > yBottom)
						yBottom = pParticle->y;
					if (pParticle->y < yTop)
						yTop = pParticle->y;
					}
				else
					pParticle->fAlive = false;
				}

			//	Next

			pParticle++;
			}

		//	Set the bounding rect

		m_rcBounds.left = xLeft / FIXED_POINT;
		m_rcBounds.top = yTop / FIXED_POINT;
		m_rcBounds.right = xRight / FIXED_POINT;
		m_rcBounds.bottom = yBottom / FIXED_POINT;
		}

	//	Any particles left?

	if (retbAlive)
		*retbAlive = !bAllParticlesDead;
	}

void CParticleArray::UpdateRingCohesion (Metric rRadius, Metric rMinRadius, Metric rMaxRadius, int iCohesion, int iResistance)

//	UpdateRingCohesion
//
//	Accelerates all particles so that they stay formed along a ring of the
//	given dimensions

	{
	//	We need to use real coordinates instead of fixed point

	UseRealCoords();

	//	Compute some constants

	Metric rOuterRange = rMaxRadius - rRadius;
	Metric rInnerRange = rRadius - rMinRadius;
	Metric *pNormalDist = GetDestinyToBellCurveArray();

	//	Loop over all particles

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			//	Compute this particle's distance from the center

			Metric rParticleRadius = pParticle->Pos.Length();
			
			//	See if the particle is outside of its bounds
			//	(we calculate a different boundary for each individual
			//	particle [based on destiny] so that we can have a "normal"
			//	distribution)

			bool bOutside;
			if (rParticleRadius > rRadius)
				{
				Metric rMax = rRadius + rOuterRange * Absolute(pNormalDist[pParticle->iDestiny]);
				bOutside = (rParticleRadius > rMax);
				}
			else
				{
				Metric rMin = rRadius - rInnerRange * Absolute(pNormalDist[pParticle->iDestiny]);
				bOutside = (rParticleRadius < rMin);
				}

			//	If we're outside our bounds, then we need to accelerate
			//	towards the center

			if (bOutside)
				{
				//	Compute some constants

				Metric rAccelerationFactor = (iCohesion * iCohesion) * g_KlicksPerPixel / 10000.0;

				//	Compute a normal vector pointing away from the center

				CVector vNormal = pParticle->Pos / rParticleRadius;

				//	Accelerate towards the center

				if (rParticleRadius > rRadius)
					pParticle->Vel = pParticle->Vel - (vNormal * rAccelerationFactor);
				else
					pParticle->Vel = pParticle->Vel + (vNormal * rAccelerationFactor);
				}

			//	Otherwise, internal resistance slows us down

			else
				{
				if (iResistance > 0)
					{
					Metric rDragFactor = (10000 - iResistance * iResistance) / 10000.0f;
					pParticle->Vel = pParticle->Vel * rDragFactor;
					}
				}
			}

		//	Next

		pParticle++;
		}
	}

void CParticleArray::UpdateTrackTarget (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx)

//	UpdateTrackTarget
//
//	Change particle velocities to track the given target

	{
	CSpaceObject *pTarget = Ctx.pTarget;
	int iManeuverRate = Ctx.pDamageDesc->GetManeuverRate();
	Metric rMaxSpeed = Ctx.pDamageDesc->GetRatedSpeed();

	//	We need to use real coordinates instead of fixed point

	UseRealCoords();

	//	Skip if we don't yet have a center of mass calculated

	if (m_vCenterOfMass.IsNull())
		return;

	//	Compute the intercept position of the target

	CVector vTarget = pTarget->GetPos() - (m_vOrigin + m_vCenterOfMass);
	CVector vTargetVel = pTarget->GetVel();
	Metric rTimeToIntercept = CalcInterceptTime(vTarget, vTargetVel, rMaxSpeed);
	
	CVector vAimPos;
#if 0
	if (rTimeToIntercept > 0.0)
		vAimPos = (pTarget->GetPos() + pTarget->GetVel() * rTimeToIntercept) - m_vOrigin;
	else
#endif
		vAimPos = pTarget->GetPos() - m_vOrigin;

	//	Loop over all particles

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			//	Compute the particles current direction of motion

			Metric rCurSpeed;
			int iCurDir = VectorToPolar(pParticle->Vel, &rCurSpeed);

			//	Compute desired direction

			int iTargetDir = VectorToPolar(vAimPos - pParticle->Pos);

			//	Turn to desired direction.

			if (!AreAnglesAligned(iTargetDir, iCurDir, 1))
				{
				int iTurn = (iTargetDir + 360 - iCurDir) % 360;

				int iNewDir;
				if (iTurn >= 180)
					{
					int iTurnAngle = Min((360 - iTurn), iManeuverRate);
					iNewDir = (iCurDir + 360 - iTurnAngle) % 360;
					}
				else
					{
					int iTurnAngle = Min(iTurn, iManeuverRate);
					iNewDir = (iCurDir + iTurnAngle) % 360;
					}

				//	Turn

				pParticle->Vel = PolarToVector(iNewDir, rCurSpeed);
				pParticle->iRotation = iNewDir;
				}
			}

		//	Next

		pParticle++;
		}
	}

void CParticleArray::UpdateWrithe (SEffectUpdateCtx &UpdateCtx)

//	UpdateWrithe
//
//	Updates the particle velocity and direction for a writhe animation.

	{
	//	We need to use real coordinates instead of fixed point

	UseRealCoords();

	//	Loop over all particles

	SParticle *pParticle = m_pArray;
	SParticle *pEnd = pParticle + m_iCount;

	while (pParticle < pEnd)
		{
		if (pParticle->fAlive)
			{
			int iTurn = (4 + (pParticle->iDestiny % 6)) * ((((pParticle->iDestiny + UpdateCtx.iTick) / 60) % 2) == 0 ? -1 : 1);
			pParticle->iRotation = AngleMod(pParticle->iRotation + iTurn);
			pParticle->Vel = pParticle->Vel.Rotate(mathDegreesToRadians(iTurn));
			}

		//	Next

		pParticle++;
		}
	}

void CParticleArray::UseRealCoords (void)

//	UseRealCoords
//
//	Switches to using real coordinates (instead of int)

	{
	if (!m_bUseRealCoords)
		{
		SParticle *pParticle = m_pArray;
		SParticle *pEnd = pParticle + m_iCount;

		while (pParticle < pEnd)
			{
			if (pParticle->fAlive)
				{
				pParticle->Pos = XYToPos(pParticle->x, pParticle->y);
				pParticle->Vel = XYToPos(pParticle->xVel, pParticle->yVel);
				}

			//	Next

			pParticle++;
			}

		m_bUseRealCoords = true;
		}
	}

void CParticleArray::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Writes the array to a stream
//
//	DWORD			count of particles
//	CVector			m_vOrigin
//	CVector			m_vCenterOfMass
//	CVector			m_vUR
//	CVector			m_vLL
//	CVector			m_vLastEmitSource
//	CVector			m_vLastEmitSourceVel
//	DWORD			m_iLastEmitDirection
//	DWORD			flags
//	SParticle[]		array of particles

	{
	DWORD dwSave;

	pStream->Write((char *)&m_iCount, sizeof(DWORD));
	pStream->Write((char *)&m_vOrigin, sizeof(CVector));
	pStream->Write((char *)&m_vCenterOfMass, sizeof(CVector));
	pStream->Write((char *)&m_vUR, sizeof(CVector));
	pStream->Write((char *)&m_vLL, sizeof(CVector));

	pStream->Write((char *)&m_vLastEmitSource, sizeof(CVector));
	pStream->Write((char *)&m_vLastEmitSourceVel, sizeof(CVector));
	pStream->Write((char *)&m_iLastEmitDirection, sizeof(DWORD));

	//	Flags

	dwSave = 0;
	dwSave |= (m_bUseRealCoords ?		0x00000001 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Array

	pStream->Write((char *)m_pArray, sizeof(SParticle) * m_iCount);
	}

CVector CParticleArray::XYToPos (int x, int y)

//	XYToPos
//
//	Converts from particle coordinates to screen coordinates

	{
	return CVector((x * g_KlicksPerPixel / FIXED_POINT), -(y * g_KlicksPerPixel / FIXED_POINT));
	}

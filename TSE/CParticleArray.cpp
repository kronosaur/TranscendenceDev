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

void CParticleArray::AddParticle (const CVector &vPos, const CVector &vVel, int iLifeLeft, int iRotation, int iDestiny, int iGeneration, DWORD dwData)

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
	pParticle->dwData = dwData;

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

void CParticleArray::Emit (const CParticleSystemDesc &Desc, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick, int *retiEmitted)

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
	int iCount = Desc.GetEmitRate().Roll();
	if (iCount <= 0)
		return;

	switch (Desc.GetStyle())
		{
		case CParticleSystemDesc::styleAmorphous:
			EmitAmorphous(Desc, iCount, vSource, vSourceVel, iDirection, iTick);
			break;

		case CParticleSystemDesc::styleComet:
			EmitComet(Desc, iCount, vSource, vSourceVel, iDirection, iTick);
			break;

		case CParticleSystemDesc::styleExhaust:
			//	LATER: Same as CParticleSystemEffectPainter::CreateFixedParticles...
			break;

		case CParticleSystemDesc::styleJet:
			//	LATER: Same as CParticleSystemEffectPainter
			break;

		case CParticleSystemDesc::styleRadiate:
			//	LATER: Same as CParticleSystemEffectPainter
			break;

		case CParticleSystemDesc::styleSpray:
			EmitSpray(Desc, iCount, vSource, vSourceVel, iDirection, iTick);
			break;
		}

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

	Metric rCurRotation = AngleToRadians(Desc.GetXformRotation() + iDirection);

	//	Compute the range in speed

	Metric rMaxSpeed = Desc.GetEmitSpeed().GetMaxValue() * LIGHT_SPEED / 100.0;
	Metric rMinSpeed = Desc.GetEmitSpeed().GetMinValue() * LIGHT_SPEED / 100.0;
	Metric rSpeedRange = Max(0.01 * LIGHT_SPEED, rMaxSpeed - rMinSpeed);

	//	Compute the spread angle, in radians

	Metric rSpread = AngleToRadians(Max(0, Desc.GetSpreadAngle().Roll()));
	Metric rHalfSpread = 0.5 * rSpread;

	//	If the emissions come from a line, calculate that (emitWidth)

	Metric rSpreadRange = Desc.GetEmitWidth().Roll() * g_KlicksPerPixel;
	CVector vTangent = ::PolarToVectorRadians(rCurRotation + (0.5 * g_Pi), rSpreadRange);

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

		AddParticle(vPos, vVel, Desc.GetParticleLifetime().Roll(), AngleToDegrees(rRotation));
		}
	}

void CParticleArray::EmitComet (const CParticleSystemDesc &Desc, int iCount, const CVector &vSource, const CVector &vSourceVel, int iDirection, int iTick)

//	EmitComet
//
//	Emits a directional stream of particles in the shape of a comet. The head
//	of the comet will travel at maximum speed, while the tail will travel at
//	minimum speed.

	{
	const TArray<CVector> &SplinePoints = CParticleCometEffectCreator::GetSplinePoints();
	const int SPLINE_CHOOSE_POINTS = 1000;

	int i;

	//	Compute some basic stuff

	int iCurRotation = Desc.GetXformRotation() + iDirection;
	Metric rCurRotation = AngleToRadians(iCurRotation);

	//	Compute the range in speed

	Metric rMaxSpeed = Desc.GetEmitSpeed().GetMaxValue() * LIGHT_SPEED / 100.0;
	Metric rMinSpeed = Desc.GetEmitSpeed().GetMinValue() * LIGHT_SPEED / 100.0;
	Metric rSpeedRange = Max(0.01 * LIGHT_SPEED, rMaxSpeed - rMinSpeed);

	//	Compute the size of the comet (at the time we emit)

	Metric rLength = g_SecondsPerUpdate * Desc.GetEmitSpeed().GetAveValueFloat() * LIGHT_SPEED / 100.0;
	Metric rWidth = Max(0.5 * rLength, Desc.GetEmitWidth().GetMaxValue() * g_KlicksPerPixel);

	//	Use a stepper to cluster the points towards the head.

	CStepIncrementor SplinePos(CStepIncrementor::styleSquare, 0.0, SplinePoints.GetCount() - 1, SPLINE_CHOOSE_POINTS);
	CStepIncrementor WidthPos(CStepIncrementor::styleSquare, 0.0, 1.0, SPLINE_CHOOSE_POINTS);

	//	Create the particles

	for (i = 0; i < iCount; i++)
		{
		//	Pick a random position on the comet line

		const CVector &vSplinePos = SplinePoints[(int)SplinePos.GetAt(mathRandom(0, SPLINE_CHOOSE_POINTS))];

		//	Pick a random position on the width and adjust the spline

		Metric rWidthAdj = WidthPos.GetAt(mathRandom(0, SPLINE_CHOOSE_POINTS));
		if ((i % 2) == 1)
			rWidthAdj = -rWidthAdj;

		CVector vSplineAdj(vSplinePos.GetX() * rLength, vSplinePos.GetY() * rWidthAdj * rWidth);

		//	Rotate the spline

		CVector vSplinePosRot = vSplineAdj.Rotate(iCurRotation + 180);
		CVector vPos = vSource + vSplinePosRot;

		//	Pick a velocity, proportional to the place on the spline

		CVector vVel =  vSourceVel + ::PolarToVectorRadians(rCurRotation, rMaxSpeed - (rSpeedRange * vSplinePos.GetX()));

		//	Add the particle

		AddParticle(vPos, vVel, Desc.GetParticleLifetime().Roll(), AngleToDegrees(rCurRotation));
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

		case paintImage:
			PaintImage(Dest, xPos, yPos, Ctx, Desc);
			break;

		case paintLine:
			PaintLine(Dest, xPos, yPos, Ctx, Desc.rgbPrimaryColor, Desc.rgbSecondaryColor);
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

			Ctx.iTick = iSavedTick - pParticle->iGeneration;
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

			int iFrame = (Desc.bDirectional ? iRotationFrame : (pParticle->iDestiny % Desc.iVariants));

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
								CG32bitPixel rgbPrimaryColor,
								CG32bitPixel rgbSecondaryColor)

//	PaintLine
//
//	Paints particle as a line

	{
	//	Figure out the velocity of our object

	int xVel = 0;
	int yVel = 0;
	if (Ctx.pObj)
		PosToXY(Ctx.pObj->GetVel(), &xVel, &yVel);

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

			int xTo = xFrom - (xVel + pParticle->xVel) / FIXED_POINT;
			int yTo = yFrom - (yVel + pParticle->yVel) / FIXED_POINT;

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
//
//	DWORD		Particle count
//	CVector		m_vOrigin
//	CVector		m_vCenterOfMass
//	CVector		m_vUR
//	CVector		m_vLL
//	DWORD		Flags
//	Array of particles

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

	if (Ctx.dwVersion < 64)
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
			Ctx.pStream->Read((char *)&m_pArray[i].dwData, sizeof(DWORD));

			Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
			m_pArray[i].fAlive = ((dwLoad & 0x00000001) ? true : false);
			m_pArray[i].dwSpare = 0;

			//	See if we need to compute real coords

			if (m_bUseRealCoords)
				{
				m_pArray[i].Pos = XYToPos(m_pArray[i].x, m_pArray[i].y);
				m_pArray[i].Vel = XYToPos(m_pArray[i].xVel, m_pArray[i].yVel);
				}
			}
		}
	else if (Ctx.dwVersion < 119)
		{
		for (i = 0; i < m_iCount; i++)
			{
			Ctx.pStream->Read((char *)&m_pArray[i].Pos, sizeof(CVector));
			Ctx.pStream->Read((char *)&m_pArray[i].Vel, sizeof(CVector));
			Ctx.pStream->Read((char *)&m_pArray[i].x, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].y, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].xVel, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].yVel, sizeof(DWORD));
			m_pArray[i].iGeneration = 0;
			Ctx.pStream->Read((char *)&m_pArray[i].iLifeLeft, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].iDestiny, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].iRotation, sizeof(DWORD));
			Ctx.pStream->Read((char *)&m_pArray[i].dwData, sizeof(DWORD));

			Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
			m_pArray[i].fAlive = ((dwLoad & 0x00000001) ? true : false);
			m_pArray[i].dwSpare = 0;
			}
		}
	else
		Ctx.pStream->Read((char *)m_pArray, sizeof(SParticle) * m_iCount);
	}

void CParticleArray::Update (const CParticleSystemDesc &Desc, SEffectUpdateCtx &Ctx)

//	Update
//
//	Updates the array based on the context

	{
	if ((Ctx.pDamageDesc || Ctx.iWakePotential > 0) && Ctx.pSystem)
		UpdateCollisions(Desc, Ctx);

	//	If we're tracking, change velocity to follow target

	if (Ctx.pTarget && Ctx.pDamageDesc && Ctx.pDamageDesc->IsTrackingTime(Ctx.iTick))
		UpdateTrackTarget(Ctx.pTarget, Ctx.pDamageDesc->GetManeuverRate(), Ctx.pDamageDesc->GetRatedSpeed());
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
	if (Ctx.iWakePotential)
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
				&& Ctx.pObj->CanHit(pObj) 
				&& pObj->CanBeHit() 
				&& pObj->InBox(vUR, vLL)
				&& !pObj->IsDestroyed()
				&& pObj != Ctx.pObj)
			{

			//	See if this object can be hit by the damage

			bool bDoDamage = (Ctx.pDamageDesc && pObj->CanBeHitBy(Ctx.pDamageDesc->m_Damage));

			//	Keep track of some values if we're doing damage

			int iHitCount = 0;
			CVector vTotalHitPos;

			//	Compute some values if we're affecting particles

			CVector vVelN;
			CVector vVelT;
			Metric rObjVel;
			Metric rWakeFactor;
			int iObjMotionAngle;
			if (Ctx.iWakePotential)
				{
				//	Compute the velocity components of the object that will
				//	affect the particles

				Metric rSpeed = pObj->GetVel().Length();
				vVelN = (rSpeed == 0.0 ? CVector(1.0f, 0) : pObj->GetVel() / rSpeed);
				vVelT = vVelN.Perpendicular();

				//	Object speed in Km/tick

				rObjVel = rSpeed * g_SecondsPerUpdate;

				//	How much does velocity of object affect particle

				rWakeFactor = (Metric)Ctx.iWakePotential / 100.0f;

				//	Compute the objects direction

				iObjMotionAngle = VectorToPolar(vVelN);
				}

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

						else if (Ctx.iWakePotential)
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

							Metric rMaxSpeed = Max(rWakeFactor * rObjVel, pParticle->Vel.Length());

							//	Figure out how we affect the particle speed along the object's motion

							Metric rNewVelLine = rParticleVelLine + (rWakeFactor * rObjVel);
							Metric rNewVelPerp = rParticleVelPerp + (rWakeFactor * rObjVel);
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

				int iTotal = Ctx.pDamageDesc->m_Damage.RollDamage() * pHit->iHitStrength;
				int iDamage = iTotal / Ctx.iTotalParticleCount;
				if (mathRandom(1, 100) <= (iTotal % Ctx.iTotalParticleCount) * 100 / Ctx.iTotalParticleCount)
					iDamage++;

				//	Do damage

				SDamageCtx DamageCtx;
				DamageCtx.pObj = pObj;
				DamageCtx.pDesc = Ctx.pDamageDesc;
				DamageCtx.Damage = Ctx.pDamageDesc->m_Damage;
				DamageCtx.Damage.SetDamage(iDamage);
				DamageCtx.Damage.AddEnhancements(Ctx.pEnhancements);
				DamageCtx.Damage.SetCause(Ctx.iCause);
				if (Ctx.bAutomatedWeapon)
					DamageCtx.Damage.SetAutomatedWeapon();
				DamageCtx.vHitPos = pHit->vHitPos;
				DamageCtx.iDirection = VectorToPolar(DamageCtx.vHitPos - pObj->GetPos());
				DamageCtx.pCause = Ctx.pObj;
				DamageCtx.Attacker = Ctx.Attacker;

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

void CParticleArray::UpdateTrackTarget (CSpaceObject *pTarget, int iManeuverRate, Metric rMaxSpeed)

//	UpdateTrackTarget
//
//	Change particle velocities to track the given target

	{
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
	if (rTimeToIntercept > 0.0)
		vAimPos = (pTarget->GetPos() + pTarget->GetVel() * rTimeToIntercept) - m_vOrigin;
	else
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
				}
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
//	DWORD			flags
//	SParticle[]		array of particles

	{
	DWORD dwSave;

	pStream->Write((char *)&m_iCount, sizeof(DWORD));
	pStream->Write((char *)&m_vOrigin, sizeof(CVector));
	pStream->Write((char *)&m_vCenterOfMass, sizeof(CVector));
	pStream->Write((char *)&m_vUR, sizeof(CVector));
	pStream->Write((char *)&m_vLL, sizeof(CVector));

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

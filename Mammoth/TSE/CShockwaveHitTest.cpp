//	CShockwaveHitTest.cpp
//
//	CShockwaveHitTest object

#include "PreComp.h"

struct SHitData
	{
	bool bHit;
	int iAngle;
	CVector vHitPos;
	};

const Metric GRID_SIZE = 8.0;

void CShockwaveHitTest::CalcObjBounds (CSpaceObject *pObj, const CVector &vPos, Metric *retrStartAngle, Metric *retrSizeAngle, Metric *retrStartRadius, Metric *retrEndRadius)

//	CalcObjBounds
//
//	Computes the bounds of the object in polar coordinates centered on vPos

	{
	//	Get the bounds of the object

	CVector vUR;
	CVector vLL;
	pObj->GetBoundingRect(&vUR, &vLL);
	
	//	Relative to vPos

	vUR = vUR - vPos;
	vLL = vLL - vPos;

	//	Get the other two corners

	CVector vUL = CVector(vLL.GetX(), vUR.GetY());
	CVector vLR = CVector(vUR.GetX(), vLL.GetY());

	//	Convert to polar coordinates

	Metric rULRadius;
	Metric rULAngle = ::VectorToPolarRadians(vUL, &rULRadius);

	Metric rURRadius;
	Metric rURAngle = ::VectorToPolarRadians(vUR, &rURRadius);

	Metric rLLRadius;
	Metric rLLAngle = ::VectorToPolarRadians(vLL, &rLLRadius);

	Metric rLRRadius;
	Metric rLRAngle = ::VectorToPolarRadians(vLR, &rLRRadius);

	//	Figure out the min and max radius

	*retrStartRadius = Min(Min(Min(rULRadius, rURRadius), rLLRadius), rLRRadius);
	*retrEndRadius = Max(Max(Max(rULRadius, rURRadius), rLLRadius), rLRRadius);

	//	If the box may straddles 0 angle, then we need to do special calculations

	if (vUL.GetX() >= 0.0 && vUR.GetX() >= 0.0 && vLL.GetX() >= 0.0 && vLR.GetX() >= 0.0)
		{
		//	If we're either all above the x-axis or all below it, then we're OK.

		if ((vUL.GetY() >= 0.0 && vUR.GetY() >= 0.0 && vLL.GetY() >= 0.0 && vLR.GetY() >= 0.0)
				|| (vUL.GetY() < 0.0 && vUR.GetY() < 0.0 && vLL.GetY() < 0.0 && vLR.GetY() < 0.0))
			{
			Metric rStartAngle = Min(Min(Min(rULAngle, rURAngle), rLLAngle), rLRAngle);
			Metric rEndAngle = Max(Max(Max(rULAngle, rURAngle), rLLAngle), rLRAngle);

			*retrStartAngle = 360.0 * rStartAngle / (PI * 2.0);
			*retrSizeAngle = 360.0 * (rEndAngle - rStartAngle) / (PI * 2.0);
			}

		//	Otherwise, we're straddling the x-axis

		else
			{
			Metric rStartAngle = rLLAngle;
			Metric rEndAngle = rULAngle;

			*retrStartAngle = 360.0 * rStartAngle / (PI * 2.0);
			*retrSizeAngle = 360.0 * (rEndAngle + (PI * 2.0) - rStartAngle) / (PI * 2.0);
			}
		}

	//	Otherwise, as long as we're not straddling the origin, we're OK

	else if ((vUL.GetY() >= 0.0 && vUR.GetY() >= 0.0 && vLL.GetY() >= 0.0 && vLR.GetY() >= 0.0)
			|| (vUL.GetY() < 0.0 && vUR.GetY() < 0.0 && vLL.GetY() < 0.0 && vLR.GetY() < 0.0)
			|| (vUL.GetX() < 0.0 && vUR.GetX() < 0.0 && vLL.GetX() < 0.0 && vLR.GetX() < 0.0))
		{
		Metric rStartAngle = Min(Min(Min(rULAngle, rURAngle), rLLAngle), rLRAngle);
		Metric rEndAngle = Max(Max(Max(rULAngle, rURAngle), rLLAngle), rLRAngle);

		*retrStartAngle = 360.0 * rStartAngle / (PI * 2.0);
		*retrSizeAngle = 360.0 * (rEndAngle - rStartAngle) / (PI * 2.0);
		}

	//	Otherwise, we have to check everything

	else
		{
		*retrStartAngle = 0.0;
		*retrSizeAngle = 359.9;
		*retrStartRadius = 0.0;
		*retrEndRadius = g_InfiniteDistance;
		}
	}

void CShockwaveHitTest::Init (int iSegments, int iLives)

//	Init
//
//	Initialize points

	{
	ASSERT(iSegments > 0);
	ASSERT(iLives >= 0);

	m_Segments.DeleteAll();
	m_Segments.InsertEmpty(iSegments);

	for (int i = 0; i < m_Segments.GetCount(); i++)
		m_Segments[i].iLives = iLives;
	}

void CShockwaveHitTest::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Load
//
//	DWORD		Count
//
//	DWORD		Life left
//	DWORD		dwLastHitID

	{
	DWORD dwLoad;

	Ctx.pStream->Read(dwLoad);
	m_Segments.DeleteAll();
	m_Segments.InsertEmpty(dwLoad);

	if (dwLoad > 0)
		{
		for (int i = 0; i < m_Segments.GetCount(); i++)
			{
			Ctx.pStream->Read(m_Segments[i].iLives);

			if (Ctx.dwVersion >= 211)
				{
				Ctx.pStream->Read(m_Segments[i].dwLastHitID);
				}
			}
		}
	}

void CShockwaveHitTest::Update (SEffectUpdateCtx &Ctx, const CVector &vPos, Metric rMinRadius, Metric rMaxRadius)

//	Update
//
//	Hit test and update (doing damage, if necessary)

	{
	int j;

	//	Compute some stuff

	Metric rSegmentAngle = 360.0 / m_Segments.GetCount();

	//	Generate a bounding box for the circle

	CVector vDiag = CVector(rMaxRadius, rMaxRadius);
	CVector vUR = vPos + vDiag;
	CVector vLL = vPos - vDiag;

	//	Compute the arc size of the grid at maximum radius

	Metric rGridAngle = 360.0 * (GRID_SIZE * g_KlicksPerPixel) / (2.0 * PI * rMaxRadius);
	Metric rGridRadius = GRID_SIZE * g_KlicksPerPixel;

	//	Segments are rotated by a random amount each time

	Metric rRandomOffset = mathRandom(0, (int)(rSegmentAngle * 3600.0) - 1) / 3600.0;

	//	Allocate a temporary array to keep track of which segments we hit

	TArray<SHitData> SegHit;
	SegHit.InsertEmpty(m_Segments.GetCount());

	const CSpaceObjectGrid &Grid = Ctx.pSystem->GetObjectGrid();
	SSpaceObjectGridEnumerator i;
	Grid.EnumStart(i, vUR, vLL, 0);

	//	Loop over all objects in and near the grid rectangle

	while (Grid.EnumHasMore(i))
		{

		//	EnumGetNext checks if the object is within bound and not destroyed

		CSpaceObject* pObj = Grid.EnumGetNext(i);
		if (pObj 
				&& Ctx.pObj->CanHit(pObj)
				&& pObj->CanBeHit()
				&& pObj->CanBeHitBy(Ctx.pDamageDesc->GetDamage())
				&& pObj != Ctx.pObj)
			{
			//	Figure out the bounds of the object in polar coordinates relative
			//	to the center of the shockwave.
			//
			//	rStartRadius and rEndRadius are guaranteed to be within the ring
			//	(or else CalcObjBounds will return FALSE).

			Metric rStartAngle;
			Metric rSizeAngle;
			Metric rStartRadius;
			Metric rEndRadius;
			CalcObjBounds(pObj, vPos, &rStartAngle, &rSizeAngle, &rStartRadius, &rEndRadius);

			//	Intersect with ring radii

			if (rStartRadius > rMaxRadius)
				continue;
			else if (rStartRadius < rMinRadius)
				rStartRadius = rMinRadius;

			if (rEndRadius < rMinRadius)
				continue;
			else if (rEndRadius > rMaxRadius)
				rEndRadius = rMaxRadius;

			//	Initialize the SegHit array to false.

			int iHitCount = 0;
			utlMemSet(&SegHit[0], sizeof(SHitData) * SegHit.GetCount(), 0);

			//	Loop through the grid to see if we hit anything

			Metric rTestRadius = rStartRadius;
			while (rTestRadius < rEndRadius)
				{
				Metric rTestAngle = 0.0;

				while (rTestAngle < rSizeAngle)
					{
					Metric rTheAngle = rStartAngle + rTestAngle;
					if (rTheAngle > 360.0)
						rTheAngle -= 360.0;

					int iSegment = (int)(rTheAngle / rSegmentAngle);

					//	Safety check just in case

					if (iSegment < 0)
						{
						ASSERT(false);
						iSegment = 0;
						}
					else if (iSegment >= SegHit.GetCount())
						{
						ASSERT(false);
						iSegment = iSegment % SegHit.GetCount();
						}

					//	If this segment has already been hit by this object or if we've
					//	already exceeded the segment's lives, then we skip

					if (!SegHit[iSegment].bHit 
							&& m_Segments[iSegment].iLives > 0
							&& m_Segments[iSegment].dwLastHitID != pObj->GetID())
						{
						CVector vHitTest = vPos + PolarToVector((int)(rTheAngle + rRandomOffset), rTestRadius);
						if (pObj->PointInObject(pObj->GetPos(), vHitTest))
							{
							SegHit[iSegment].bHit = true;
							SegHit[iSegment].iAngle = (int)rTheAngle;
							SegHit[iSegment].vHitPos = vHitTest;

							m_Segments[iSegment].dwLastHitID = pObj->GetID();
							iHitCount++;
							}
						}

					rTestAngle += rGridAngle;
					}

				rTestRadius += rGridRadius;
				}

			//	If we hit the object, then do damage

			if (iHitCount > 0)
				{
				//	There's a maximum amount of damage that we can do to a single 
				//	object.

				Metric rMaxHitCount = m_Segments.GetCount() * MAX_HITS_PER_OBJ_FRACTION;
				int iHitChance = 100;
				if ((Metric)iHitCount > rMaxHitCount)
					{
					iHitChance = mathRound(100.0 * rMaxHitCount / (Metric)iHitCount);
					}

				//	Do damage.

				for (j = 0; j < SegHit.GetCount(); j++)
					{
					if (!SegHit[j].bHit)
						continue;

					if (iHitChance < 100 && mathRandom(1, 100) > iHitChance)
						continue;

					//	Do damage to the object
					//	We check again if the object is destroyed because it could have been
					//	destroyed by one segment hit.

					if (!pObj->IsDestroyed())
						{
						SDamageCtx DamageCtx(pObj,
								*Ctx.pDamageDesc,
								Ctx.pEnhancements,
								Ctx.Attacker,
								Ctx.pObj,
								0.0,
								AngleMod(SegHit[j].iAngle + 180),
								SegHit[j].vHitPos);

						pObj->Damage(DamageCtx);
						}

					//	Remember that this segment hit something
					//	Taking passthrough into account

					if (mathRandom(1, 100) > Ctx.pDamageDesc->GetPassthrough())
						m_Segments[j].iLives = m_Segments[j].iLives - 1;
					}
				}
			}
		}
	}

void CShockwaveHitTest::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Save

	{
	DWORD dwSave;

	dwSave = m_Segments.GetCount();
	pStream->Write(dwSave);

	if (m_Segments.GetCount() > 0)
		{
		for (int i = 0; i < m_Segments.GetCount(); i++)
			{
			pStream->Write(m_Segments[i].iLives);
			pStream->Write(m_Segments[i].dwLastHitID);
			}
		}
	}

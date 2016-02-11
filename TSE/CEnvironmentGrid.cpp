//	CEnvironmentGrid.cpp
//
//	CEnvironmentGrid class
//	Copyright (c) 2013 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#ifdef DEBUG
//#define DEBUG_PAINT_TIMINGS
#endif

struct SEdgeDesc
	{
	DWORD dwFlag;
	int xOffset;
	int yOffset;
	DWORD dwOppositeFlag;
	};

const int EDGE_COUNT =						4;

static SEdgeDesc EDGE_DATA[EDGE_COUNT] =
	{
		{	CEnvironmentGrid::edgeRight,	+1,	 0,	CEnvironmentGrid::edgeLeft		},
		{	CEnvironmentGrid::edgeTop,		 0,	-1,	CEnvironmentGrid::edgeBottom	},
		{	CEnvironmentGrid::edgeLeft,		-1,	 0,	CEnvironmentGrid::edgeRight		},
		{	CEnvironmentGrid::edgeBottom,	 0,	+1,	CEnvironmentGrid::edgeTop		},
	};

CEnvironmentGrid::SConfig CEnvironmentGrid::CONFIG_DATA[CSystemType::sizeCount] =
	{
		{	2,	12,	128		},
		{	2,	8,	256		},
		{	1,	16,	512		},
		{	1,	12,	1024	},
	};

#ifdef DEBUG_PAINT_TIMINGS
static int g_iTimingCount = 0;
static DWORD g_dwTotalTime = 0;
#endif

class CTilePainter : public IThreadPoolTask
	{
	public:
		CTilePainter (SViewportPaintCtx &Ctx, CG32bitImage &Dest, CEnvironmentGrid *pGrid, int x1, int y1, int x2, int y2) :
				m_Ctx(Ctx),
				m_Dest(Dest),
				m_pGrid(pGrid),
				m_x1(x1),
				m_y1(y1),
				m_x2(x2),
				m_y2(y2)
			{ }

		virtual void Run (void)
			{
			int x, y;

			for (x = m_x1; x < m_x2; x++)
				for (y = m_y1; y < m_y2; y++)
					{
					DWORD dwEdgeMask;
					CSpaceEnvironmentType *pEnv = m_pGrid->GetTileType(x, y, &dwEdgeMask);
					if (pEnv)
						{
						int xCenter, yCenter;
						CVector vCenter = m_pGrid->TileToVector(x, y);
						m_Ctx.XForm.Transform(vCenter, &xCenter, &yCenter);

						pEnv->Paint(m_Dest, xCenter, yCenter, x, y, dwEdgeMask);
						}
					}
			};

	private:
		SViewportPaintCtx &m_Ctx;
		CG32bitImage &m_Dest;
		CEnvironmentGrid *m_pGrid;
		int m_x1;
		int m_y1;
		int m_x2;
		int m_y2;
	};

CEnvironmentGrid::CEnvironmentGrid (CSystemType::ETileSize iTileSize) : 
		m_Map((iTileSize == CSystemType::sizeUnknown ? CONFIG_DATA[CSystemType::sizeHuge].iLevelSize : CONFIG_DATA[iTileSize].iLevelSize), (iTileSize == CSystemType::sizeUnknown ? CONFIG_DATA[CSystemType::sizeHuge].iScale : CONFIG_DATA[iTileSize].iScale))

//	CEnvironmentGrid constructor

	{
	m_iTileCount = m_Map.GetTotalSize();
	m_iTileSize = (iTileSize == CSystemType::sizeUnknown ? CONFIG_DATA[CSystemType::sizeHuge].iTileSize : CONFIG_DATA[iTileSize].iTileSize);
//	m_iTileSize = (dwAPIVersion >= 14 ? defaultTileSize : tileSizeCompatible);
	}

DWORD CEnvironmentGrid::AddEdgeFlag (DWORD dwTile, DWORD dwEdgeFlag) const

//	AddEdgeFlag
//
//	Add the given edge flag to the tile DWORD

	{
	DWORD dwOriginalFlags = (dwTile & 0xF0000);
	DWORD dwNewFlags = dwOriginalFlags | (dwEdgeFlag << 16);
	return dwNewFlags | (dwTile & 0xFFFF);
	}

void CEnvironmentGrid::ConvertSpaceEnvironmentToPointers (CTileMap &UNIDs)

//	ConvertSpaceEnvironmentToPointers
//
//	Converts m_pEnvironment to pointers (restore from save file)

	{
	m_Map.Init(UNIDs.GetSize(), UNIDs.GetScale());

	STileMapEnumerator k;
	while (UNIDs.HasMore(k))
		{
		DWORD dwTile;
		int xTile;
		int yTile;

		UNIDs.GetNext(k, &xTile, &yTile, &dwTile);
		CSpaceEnvironmentType *pEnv = g_pUniverse->FindSpaceEnvironment(dwTile);
		if (pEnv)
			SetTileType(xTile, yTile, pEnv);
		}

	m_iTileCount = m_Map.GetTotalSize();
	}

void AdjustBounds (CVector *pUL, CVector *pLR, const CVector &vPos);
void ChangeVariation (Metric *pVariation, Metric rMaxVariation);

void CEnvironmentGrid::CreateArcNebula (SCreateCtx &Ctx, TArray<STileDesc> *retTiles)

//	CreateArcNebula
//
//	Creates an arc of nebula

	{
	if (Ctx.pEnv == NULL || Ctx.pOrbitDesc == NULL || Ctx.rWidth == 0.0)
		return;

	//	Compute some constants

	Metric rHalfWidth = Ctx.rWidth / 2.0;
	Metric rMaxVariation = Ctx.iWidthVariation * rHalfWidth / 100.0;
	Metric rHalfVariation = rMaxVariation / 2.0;
	Metric rHalfSpan = PI * Ctx.iSpan / 360.0;

	Metric rErode = Max(0, Min(Ctx.iErode, 100)) / 100.0;

	//	Shape of the arc

	CVector vCenter = Ctx.pOrbitDesc->GetFocus();
	Metric rArcCenterAngle = Ctx.pOrbitDesc->GetObjectAngle();
	Metric rMaxRadius = Ctx.pOrbitDesc->GetSemiMajorAxis() + rHalfWidth;
	Metric rMinRadius = Ctx.pOrbitDesc->GetSemiMajorAxis() - rHalfWidth;

	//	Shape parameters

	CWaveGenerator2 OuterWave(
			0.75,	//	Wave 0 amplitude
			5.0,	//	Wave 0 cycles
			0.25,	//	Wave 1 amplitude
			11.0	//	Wave 1 cycles
			);

	CWaveGenerator2 InnerWave(
			0.5,
			6.0,
			0.5,
			15.0
			);

	//	Compute the bounds of the nebula

	Metric rDiag = rMaxRadius + (GetTileSize() * g_KlicksPerPixel);
	CVector vUL = vCenter + CVector(-rDiag, rDiag);
	CVector vLR = vCenter + CVector(rDiag, -rDiag);

	//	Now iterate over every tile in bounds and see if it is within
	//	the band that we have defined.

	int xTileStart, yTileStart, xTileEnd, yTileEnd;
	VectorToTile(vUL, &xTileStart, &yTileStart);
	VectorToTile(vLR, &xTileEnd, &yTileEnd);

	//	Set the granularity of the result array (so that we don't keep on
	//	reallocating).

	if (retTiles)
		{
		retTiles->DeleteAll();
		int iGranularity = Max(DEFAULT_ARRAY_GRANULARITY, (xTileEnd - xTileStart + 1) * (yTileEnd - yTileStart + 1) / 2);
		retTiles->SetGranularity(iGranularity);
		}

	//	Set all tiles

	int x, y;
	for (x = xTileStart; x <= xTileEnd; x++)
		for (y = yTileStart; y <= yTileEnd; y++)
			{
			CVector vTile = TileToVector(x, y);
			CVector vRadius = vTile - vCenter;

			//	Convert to polar coordinates

			Metric rRadius;
			Metric rAngle = VectorToPolarRadians(vRadius, &rRadius);

			//	Skip if outside bounds

			if (rRadius > rMaxRadius || rRadius < rMinRadius)
				continue;

			//	Figure out our distance from the center angle

			Metric rDistAngle = rAngle - rArcCenterAngle;
			if (rDistAngle > PI)
				rDistAngle -= 2.0 * PI;
			else if (rDistAngle < -PI)
				rDistAngle += 2.0 * PI;

			Metric rDistAngleAbs = Absolute(rDistAngle);
			if (rDistAngleAbs > rHalfSpan)
				continue;

			//	Compute the shape at this angle

			Metric rMidRadius;
			Ctx.pOrbitDesc->GetPointAndRadius(rAngle, &rMidRadius);

			Metric rOuterWave = rHalfVariation * OuterWave.GetValue(rAngle);
			Metric rInnerWave = rHalfVariation * InnerWave.GetValue(rAngle);

			//	Taper the shape

			Metric rTaper = 1.0 - (rDistAngleAbs / rHalfSpan);

			//	Calc radius

			Metric rOuterRadius = rMidRadius + rTaper * (rHalfWidth - rOuterWave);
			Metric rInnerRadius = rMidRadius - rTaper * (rHalfWidth + rInnerWave);

			//	If we're inside the ring, then put a nebula down

			if (rRadius > rInnerRadius && rRadius < rOuterRadius)
				{
				//	Check erode parameters

				if (rErode > 0.0 && ErodeCheck(rErode, rRadius, rInnerRadius, rMidRadius, rOuterRadius))
					continue;

				//	Set the tile

				SetTileType(x, y, Ctx.pEnv);

				//	Add to list of tiles

				if (retTiles)
					{
					STileDesc *pNewTile = retTiles->Insert();
					pNewTile->x = x;
					pNewTile->y = y;
					pNewTile->pEnv = Ctx.pEnv;
					pNewTile->dwEdgeMask = 0;
					}
				}
			}
	}

void CEnvironmentGrid::CreateCircularNebula (SCreateCtx &Ctx, TArray<STileDesc> *retTiles)

//	CreateCircularNebula
//
//	The nebula is a ring:
//
//	radius: Ctx.pOrbitDesc radius
//	width of ring: Ctx.rWidth

	{
	if (Ctx.pEnv == NULL || Ctx.pOrbitDesc == NULL || Ctx.rWidth == 0.0)
		return;

	Metric rErode = Max(0, Min(Ctx.iErode, 100)) / 100.0;

	//	Compute some constants

	Metric rHalfWidth = Ctx.rWidth / 2.0;
	Metric rMaxVariation = Ctx.iWidthVariation * rHalfWidth / 100.0;
	Metric rHalfVariation = rMaxVariation / 2.0;

	//	Shape parameters

	CWaveGenerator2 OuterWave(
			0.75,	//	Wave 0 amplitude
			5.0,	//	Wave 0 cycles
			0.25,	//	Wave 1 amplitude
			11.0	//	Wave 1 cycles
			);

	CWaveGenerator2 InnerWave(
			0.5,
			6.0,
			0.5,
			15.0
			);

	//	The maximum diameter of the ring is the orbit diameter plus half the
	//	width.

	Metric rMaxRadius = Ctx.pOrbitDesc->GetSemiMajorAxis() + rHalfWidth;
	CVector vCenter = Ctx.pOrbitDesc->GetFocus();

	//	Compute the bounds of the nebula

	Metric rDiag = rMaxRadius + (GetTileSize() * g_KlicksPerPixel);
	CVector vUL = vCenter + CVector(-rDiag, rDiag);
	CVector vLR = vCenter + CVector(rDiag, -rDiag);

	//	Now iterate over every tile in bounds and see if it is within
	//	the band that we have defined.

	int xTileStart, yTileStart, xTileEnd, yTileEnd;
	VectorToTile(vUL, &xTileStart, &yTileStart);
	VectorToTile(vLR, &xTileEnd, &yTileEnd);

	//	Set the granularity of the result array (so that we don't keep on
	//	reallocating).

	if (retTiles)
		{
		retTiles->DeleteAll();
		int iGranularity = Max(DEFAULT_ARRAY_GRANULARITY, (xTileEnd - xTileStart + 1) * (yTileEnd - yTileStart + 1) / 2);
		retTiles->SetGranularity(iGranularity);
		}

	//	Set all tiles

	int x, y;
	for (x = xTileStart; x <= xTileEnd; x++)
		for (y = yTileStart; y <= yTileEnd; y++)
			{
			CVector vTile = TileToVector(x, y);
			CVector vRadius = vTile - vCenter;

			//	Convert to polar coordinates

			Metric rRadius;
			Metric rAngle = VectorToPolarRadians(vRadius, &rRadius);

			//	Compute the shape at this angle

			Metric rMidRadius;
			Ctx.pOrbitDesc->GetPointAndRadius(rAngle, &rMidRadius);

			Metric rOuterWave = rHalfVariation * OuterWave.GetValue(rAngle);
			Metric rInnerWave = rHalfVariation * InnerWave.GetValue(rAngle);

			Metric rOuterRadius = rMidRadius + rHalfWidth - rOuterWave;
			Metric rInnerRadius = rMidRadius - rHalfWidth + rInnerWave;

			//	If we're inside the ring, then put a nebula down

			if (rRadius > rInnerRadius && rRadius < rOuterRadius)
				{
				//	Check erode parameters

				if (rErode > 0.0 && ErodeCheck(rErode, rRadius, rInnerRadius, rMidRadius, rOuterRadius))
					continue;

				//	Set the tile

				SetTileType(x, y, Ctx.pEnv);

				//	Add to list of tiles

				if (retTiles)
					{
					STileDesc *pNewTile = retTiles->Insert();
					pNewTile->x = x;
					pNewTile->y = y;
					pNewTile->pEnv = Ctx.pEnv;
					pNewTile->dwEdgeMask = 0;
					}
				}
			}
	}

void CEnvironmentGrid::CreateSquareNebula (SCreateCtx &Ctx, TArray<STileDesc> *retTiles)

//	CreateSquareNebula
//
//	The nebula is a square:
//
//	center: Ctx.pOrbitDesc position
//	height: Ctx.rHeight
//	width: Ctx.rWidth

	{
	if (Ctx.pEnv == NULL || Ctx.pOrbitDesc == NULL || Ctx.rWidth == 0.0 || Ctx.rHeight == 0.0)
		return;

	CVector vCenter = Ctx.pOrbitDesc->GetObjectPos();

	//	Compute the bounds of the nebula

	CVector vUL = vCenter + CVector(-0.5 * Ctx.rWidth, -0.5 * Ctx.rHeight);
	CVector vLR = vCenter + CVector(0.5 * Ctx.rWidth, 0.5 * Ctx.rHeight);

	//	Now iterate over every tile in bounds and see if it is within
	//	the band that we have defined.

	int xTileStart, yTileStart, xTileEnd, yTileEnd;
	VectorToTile(vUL, &xTileStart, &yTileStart);
	VectorToTile(vLR, &xTileEnd, &yTileEnd);

	//	Set the granularity of the result array (so that we don't keep on
	//	reallocating).

	if (retTiles)
		{
		retTiles->DeleteAll();
		int iGranularity = Max(DEFAULT_ARRAY_GRANULARITY, (xTileEnd - xTileStart + 1) * (yTileEnd - yTileStart + 1) / 2);
		retTiles->SetGranularity(iGranularity);
		}

	//	Set all tiles

	int x, y;
	for (x = xTileStart; x <= xTileEnd; x++)
		for (y = yTileStart; y <= yTileEnd; y++)
			{
			SetTileType(x, y, Ctx.pEnv);

			//	Add to list of tiles

			if (retTiles)
				{
				STileDesc *pNewTile = retTiles->Insert();
				pNewTile->x = x;
				pNewTile->y = y;
				pNewTile->pEnv = Ctx.pEnv;
				pNewTile->dwEdgeMask = 0;
				}
			}
	}

bool CEnvironmentGrid::ErodeCheck (Metric rErode, Metric rRadius, Metric rInnerRadius, Metric rMidRadius, Metric rOuterRadius)

//	ErodeCheck
//
//	Returns TRUE if we skip creating a tile at this position.

	{
	Metric rChance;

	if (rRadius > rMidRadius && rOuterRadius > rMidRadius)
		rChance = rErode * (rRadius - rMidRadius) / (rOuterRadius - rMidRadius);

	else if (rInnerRadius < rMidRadius)
		rChance = rErode * (rMidRadius - rRadius) / (rMidRadius - rInnerRadius);

	else
		return false;

	//	If eroded, skip

	return (rChance > 0 && mathRandom(1, 100) <= (100.0 * rChance));
	}

void CEnvironmentGrid::GetNextTileType (STileMapEnumerator &i, int *retx, int *rety, CSpaceEnvironmentType **retpEnv, DWORD *retdwEdgeMask) const

//	GetNextTileType
//
//	Returns the next tile type

	{
	DWORD dwTile;
	m_Map.GetNext(i, retx, rety, &dwTile);

	if (retpEnv)
		*retpEnv = GetSpaceEnvironmentFromTileDWORD(dwTile);

	if (retdwEdgeMask)
		*retdwEdgeMask = (dwTile & 0xF0000) >> 16;
	}

CSpaceEnvironmentType *CEnvironmentGrid::GetSpaceEnvironmentFromTileDWORD (DWORD dwTile) const

//	GetSpaceEnvironmentFromTileDWORD
//
//	Returns the space environment.

	{
	DWORD dwAtom = (dwTile & 0xffff);
	if (dwAtom == 0)
		return NULL;

	return m_EnvList.atom_GetKey(dwAtom - 1);
	}

CSpaceEnvironmentType *CEnvironmentGrid::GetTileType (int xTile, int yTile, DWORD *retdwEdgeMask)

//	GetTileType
//
//	Returns the space environment type at the given tile.

	{
	DWORD dwTile = m_Map.GetTile(xTile, yTile);
	if (retdwEdgeMask)
		*retdwEdgeMask = (dwTile & 0xF0000) >> 16;

	return GetSpaceEnvironmentFromTileDWORD(dwTile);
	}

DWORD CEnvironmentGrid::MakeTileDWORD (CSpaceEnvironmentType *pEnv, DWORD dwEdgeMask)

//	MakeTileDWORD
//
//	Encodes into a DWORD for the tile map
//
//	The encoding is as follows:
//
//	33222222222211111111110000000000
//	10987654321098765432109876543210
//	            |  ||    16-bits   |
//				|EE||      SEA     |
//
//	EE = Edge bitmask
//
//	SEA = Space Environment Atom. NOTE: We need to bias by 1 because an atom of
//			0 is valid, but a DWORD of 0 means no tile.

	{
	//	If NULL environment, then 0 atom

	if (pEnv == NULL)
		return (dwEdgeMask << 16);

	//	Otherwise, get atom for environment

	else
		{
		//	Get the atom for this environment

		DWORD dwAtom = m_EnvList.atom_Find(pEnv);
		if (dwAtom == NULL_ATOM)
			m_EnvList.atom_Insert(pEnv, &dwAtom);

		//	Combine

		return (dwEdgeMask << 16) | (dwAtom + 1);
		}
	}

void CEnvironmentGrid::MarkImages (void)

//	MarkImages
//
//	Mark images in use.

	{
	int i;

	for (i = 0; i < m_EnvList.GetCount(); i++)
		{
		CSpaceEnvironmentType *pEnv = m_EnvList.GetKey(i);
		pEnv->MarkImages();
		}
	}

void CEnvironmentGrid::Paint (SViewportPaintCtx &Ctx, CG32bitImage &Dest)

//	Paint
//
//	Paint the environment

	{
	DEBUG_TRY

#ifdef DEBUG_PAINT_TIMINGS
	DWORD dwStart = ::GetTickCount();
#endif

	int x1, y1, x2, y2;

	VectorToTile(Ctx.vUR, &x2, &y1);
	VectorToTile(Ctx.vLL, &x1, &y2);
		
	//	Increase bounds (so we can paint the edges)

	int xStart = x1 - 1;
	int yStart = y1 - 1;
	int xEnd = x2 + 2;	//	+1 to expand and +1 because this is AFTER last tile.
	int yEnd = y2 + 2;

	//	Figure out how high each horizontal chunk is

	int cyTotal = yEnd - yStart;
	int cyChunk = Max(1, cyTotal / Ctx.pThreadPool->GetThreadCount());

	//	Start async tasks

	int y = yStart;
	while (y < yEnd)
		{
		int yChunkEnd = Min(yEnd, y + cyChunk);
		Ctx.pThreadPool->AddTask(new CTilePainter(Ctx, Dest, this, xStart, y, xEnd, yChunkEnd));

		y = yChunkEnd;
		}

	Ctx.pThreadPool->Run();

#ifdef DEBUG_PAINT_TIMINGS
	g_dwTotalTime += ::GetTickCount() - dwStart;
	g_iTimingCount++;

	if ((g_iTimingCount % 100) == 0)
		::kernelDebugLogMessage("Nebula paint time: %d.%02d", g_dwTotalTime / g_iTimingCount, (100 * g_dwTotalTime / g_iTimingCount) % 100);
#endif

	DEBUG_CATCH
	}

void CEnvironmentGrid::PaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest)

//	PaintMap
//
//	Paint the environment on the map

	{
	int cxHalfTileCount = m_iTileCount / 2;
	int cyHalfTileCount = m_iTileCount / 2;

	//	Compute the offset from center of tile to various corners

	CVector vULOffset = CVector(-(m_iTileSize / 2) * g_KlicksPerPixel, (m_iTileSize / 2) * g_KlicksPerPixel);
	CVector vLROffset = CVector((m_iTileSize / 2) * g_KlicksPerPixel, -(m_iTileSize / 2) * g_KlicksPerPixel);
	CVector vUROffset = CVector((m_iTileSize / 2) * g_KlicksPerPixel, (m_iTileSize / 2) * g_KlicksPerPixel);

	//	Paint all tiles

	STileMapEnumerator k;
	while (HasMoreTiles(k))
		{
		//	Get the tile

		int xTile;
		int yTile;
		CSpaceEnvironmentType *pEnv;
		DWORD dwEdgeMask;

		GetNextTileType(k, &xTile, &yTile, &pEnv, &dwEdgeMask);
		if (pEnv == NULL)
			continue;

		//	Get the corners of the tile in global coordinates

		CVector vCenter = TileToVector(xTile, yTile);
		CVector vUL = vCenter + vULOffset;
		CVector vLR = vCenter + vLROffset;
		CVector vUR = vCenter + vUROffset;

		//	Compute the position and size of the tile in map coordinates.
		//	Since the map uses a projection, the size of the tile changes size 
		//	depending on the position.

		int x;
		int y;
		Ctx.Transform(vUL, &x, &y);

		int x2;
		int y2;
		Ctx.Transform(vLR, &x2, &y2);

		int x3;
		int y3;
		Ctx.Transform(vUR, &x3, &y3);

		int cxTile = Max(x2 - x, x3 - x);
		int cyTile = y2 - y;

		//	Fade out based on distance

		int iDist = Max(Absolute(xTile - cxHalfTileCount), Absolute(yTile - cyHalfTileCount));
		DWORD dwFade = (iDist > 20 ? Min(2 * (iDist - 20), 0x80) : 0);

		//	Paint the tile

		pEnv->PaintMap(Dest, x, y, cxTile, cyTile, dwFade, xTile, yTile, dwEdgeMask);
		}
	}

void CEnvironmentGrid::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read
//
//	DWORD			m_iTileSize
//	DWORD			No. of atom entries
//	For each entry
//		DWORD		UNID of space environment
//
//	CTileMap		Tile Map

	{
	ALERROR error;
	int i;

	if (Ctx.dwVersion >= 82)
		{
		//	Read the size of each tile

		if (Ctx.dwVersion >= 88)
			Ctx.pStream->Read((char *)&m_iTileSize, sizeof(DWORD));
		else
			m_iTileSize = tileSizeCompatible;

		//	Read the environment list

		m_EnvList.DeleteAll();

		//	Since we saved the list in atom-order, we can reload and expect to
		//	add them and get back the same atom. We rely on the fact that the
		//	atom is the same as the order in which they are added.

		int iCount;
		Ctx.pStream->Read((char *)&iCount, sizeof(DWORD));
		for (i = 0; i < iCount; i++)
			{
			DWORD dwUNID;
			Ctx.pStream->Read((char *)&dwUNID, sizeof(DWORD));

			CSpaceEnvironmentType *pEnv = g_pUniverse->FindSpaceEnvironment(dwUNID);
			DWORD dwAtom;
			m_EnvList.atom_Insert(pEnv, &dwAtom);
			ASSERT(dwAtom == i);
			}

		//	Now load the tile map.

		if (error = m_Map.ReadFromStream(Ctx.pStream))
			throw CException(error);

		m_iTileCount = m_Map.GetTotalSize();
		}

	//	In old versions we just save the UNID of the environment type for
	//	each tile.

	else
		{
		//	Load into a separate map

		CTileMap Env;
		if (error = Env.ReadFromStream(Ctx.pStream))
			throw CException(error);

		//	Convert to the new storage method

		ConvertSpaceEnvironmentToPointers(Env);

		m_iTileSize = tileSizeCompatible;
		}
	}

void CEnvironmentGrid::SetTileType (int xTile, int yTile, CSpaceEnvironmentType *pEnv)

//	SetTileType
//
//	Sets a tile.

	{
	int i;

	//	We cannot overwrite a previous tile because the edges won't look right.

	if (m_Map.GetTile(xTile, yTile) != 0)
		return;

	//	For now there is no way to delete a tile.

	ASSERT(pEnv);

	if (InBounds(xTile, yTile))
		{
		//	Loop over all edges to see if there is a tile there and to tell the
		//	tile that a new tile has appeared on THEIR edge.

		DWORD dwEdgeMask = 0;
		for (i = 0; i < EDGE_COUNT; i++)
			{
			int xEdgeTile = xTile + EDGE_DATA[i].xOffset;
			int yEdgeTile = yTile + EDGE_DATA[i].yOffset;

			DWORD *pTile = m_Map.GetTilePointer(xEdgeTile, yEdgeTile);
			if (pTile && GetSpaceEnvironmentFromTileDWORD(*pTile) == pEnv)
				{
				//	This is an edge for the new tile.

				dwEdgeMask |= EDGE_DATA[i].dwFlag;

				//	Add the new tile as an edge for this tile.

				*pTile = AddEdgeFlag(*pTile, EDGE_DATA[i].dwOppositeFlag);
				}
			}

		//	Set the new tile

		m_Map.SetTile(xTile, yTile, MakeTileDWORD(pEnv, dwEdgeMask));
		}
	}

CVector CEnvironmentGrid::TileToVector (int x, int y) const

//	TileToVector
//
//	Returns the coordinates of the center of the given tile

	{
	int xyHalfSize = m_iTileCount / 2;
	return CVector(
			(x - xyHalfSize + 0.5) * m_iTileSize * g_KlicksPerPixel,
			-((y - xyHalfSize + 0.5) * m_iTileSize * g_KlicksPerPixel)
			);
	}

void CEnvironmentGrid::VectorToTile (const CVector &vPos, int *retx, int *rety) const

//	VectorToTile
//
//	Converts from a vector to a tile coordinate

	{
	*retx = (int)((vPos.GetX() / (m_iTileSize * g_KlicksPerPixel)) + (m_iTileCount / 2));
	*rety = (int)((-vPos.GetY() / (m_iTileSize * g_KlicksPerPixel)) + (m_iTileCount / 2));
	}

void CEnvironmentGrid::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write
//
//	DWORD			m_iTileSize
//	DWORD			No. of atom entries
//	For each entry
//		DWORD		UNID of space environment
//
//	CTileMap		Tile Map

	{
	int i;

	pStream->Write((char *)&m_iTileSize, sizeof(DWORD));

	//	Write the entry list. We write out in atom-order. Since we never delete
	//	atoms, we can guarantee that this is a contiguous list.

	DWORD dwSave = m_EnvList.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < m_EnvList.GetCount(); i++)
		{
		CSpaceEnvironmentType *pEnv = m_EnvList.atom_GetKey((DWORD)i);

		dwSave = pEnv->GetUNID();
		pStream->Write((char *)&dwSave, sizeof(DWORD));
		}

	//	Now write the tile map

	m_Map.WriteToStream(pStream);
	}

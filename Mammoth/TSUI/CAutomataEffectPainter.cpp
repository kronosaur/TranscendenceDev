//	CAutomataEffectPainter.cpp
//
//	CAutomataEffectPainter class
//	Copyright (c) 2015 by George Moromisato. All Rights Reserved.

#include "stdafx.h"

const CG32bitPixel RGB_BACKGROUND =			CG32bitPixel(0x0e, 0x0c, 0x0d);
const CG32bitPixel RGB_GREEN =				CG32bitPixel(0x89, 0xd1, 0x92);

CAutomataEffectPainter::CAutomataEffectPainter (int width, int height) : 
		m_iTicks(0),
		m_iInitialUpdates(2),
		m_iWidth(width),
		m_iHeight(height),
		m_iOpacityInc(51)
//	CAutomataEffectPainter constructor

	{
	int y, x, i;
	
	InitGradientColorTable();

	m_Grid = TArray< TArray<SCell> >();
	for (y = 0; y < m_iHeight; y++)
		{
		TArray<SCell> row = TArray<SCell>();
		for (x = 0; x < m_iWidth; x++)
			{
			//	Each cell has a 20% chance of starting alive
			row.Insert(SCell(mathRandom(0, 4) == 0));
			}
		m_Grid.Insert(row);
		}
	
	for (i = 0; i < m_iInitialUpdates; i++)
		Update();
	}
int CAutomataEffectPainter::CountLiveNeighbors (int x, int y)
	{
	int iResult = 0;
	if (IsAlive(x - 1, y - 1))
		iResult++;
	if (IsAlive(x - 1, y))
		iResult++;
	if (IsAlive(x - 1, y + 1))
		iResult++;

	if (IsAlive(x, y - 1))
		iResult++;
	if (IsAlive(x, y + 1))
		iResult++;

	if (IsAlive(x + 1, y - 1))
		iResult++;
	if (IsAlive(x + 1, y))
		iResult++;
	if (IsAlive(x + 1, y + 1))
		iResult++;
	return iResult;
	}
bool CAutomataEffectPainter::IsAlive(int x, int y)
//	IsAlive
//
//	Returns if the cell at the specified position is alive. Note: Wraps around if the coordinates are out of bounds
	{
	int xWrapped = x;
	while (xWrapped < 0)
		{
		xWrapped += m_iWidth;
		}
	xWrapped %= m_iWidth;

	int yWrapped = y;
	while (yWrapped < 0)
		{
		yWrapped += m_iHeight;
		}
	yWrapped %= m_iHeight;
	return m_Grid.GetAt(yWrapped).GetAt(xWrapped).bAlive;
	}
void CAutomataEffectPainter::InitGradientColorTable (void)

//	InitGradientColorTable
//
//	Initialize the color table used for the gradient

	{
	int i;
	//int MAX_STEPS = 255 / m_iOpacityInc;

	m_GradientColorTable.DeleteAll();
	m_GradientColorTable.InsertEmpty(256);

	for(i = 0; i < 256; i++)
		{
		BYTE iOpacity = i;
		m_GradientColorTable[i] = CG32bitPixel::Blend(RGB_BACKGROUND, RGB_GREEN, iOpacity);
		}
	}

void CAutomataEffectPainter::Paint (CG32bitImage &Dest, const RECT &rcRect)

//	Paint
//
//	Paint the current frame

	{
	int y, x;

	//	Some metrics

	SPaintCtx Ctx;
	Ctx.pDest = &Dest;
	Ctx.cxHalfWidth = (RectWidth(rcRect) / 2);
	Ctx.cyHalfHeight = (RectHeight(rcRect) / 2);
	Ctx.xCenter = rcRect.left + Ctx.cxHalfWidth;
	Ctx.yCenter = rcRect.top + Ctx.cyHalfHeight;

	//	Erase

	Dest.Fill(rcRect.left, rcRect.top, RectWidth(rcRect), RectHeight(rcRect), RGB_BACKGROUND);

	int xCenterGrid = m_iWidth / 2;
	int yCenterGrid = m_iHeight / 2;

	int TILE_SIZE = 16;
	
	for (y = 0; y < m_iHeight; y++)
		{
		for(x = 0; x < m_iWidth; x++)
			{
			SCell &cell = m_Grid.GetAt(y).GetAt(x);
			if (cell.bAlive)
				cell.iOpacity = Min(255, cell.iOpacity + m_iOpacityInc);
			else
				cell.iOpacity = Max(0, cell.iOpacity - m_iOpacityInc);
			int xOffset = (x - xCenterGrid) * TILE_SIZE;
			int yOffset = (y - yCenterGrid) * TILE_SIZE;
			
			Dest.Fill((int) Ctx.xCenter + xOffset, (int) Ctx.yCenter + yOffset, TILE_SIZE, TILE_SIZE, m_GradientColorTable.GetAt(cell.iOpacity));
			}
		}
	}

void CAutomataEffectPainter::Update (void)

//	Update
//
//	Update one frame

	{
	m_iTicks++;

	if (m_iTicks % 6 > 0)
		return;
	int y, x;

	/*
	//	This is a flawed implementation of Conway's Game of Life that fills the screen with a brain-like pattern
	for (y = 0; y < m_iHeight; y++)
		{
		for (x = 0; x < m_iWidth; x++)
			{
			int iLiveNeighbors = CountLiveNeighbors(x, y);
			SCell &cell = m_Grid.GetAt(y).GetAt(x);
			if(IsAlive(x, y))
				{
				if (iLiveNeighbors < 2 || iLiveNeighbors > 3)
					cell.bAlive = false;
				}
			else
				{
				if (iLiveNeighbors == 3)
					cell.bAlive = true;
				}
			}
		}
	*/
	//	This is a proper implementation of Conway's Game of Life
	//	First pass updates bAliveNext for each cell (directly setting bAlive has side effects on the next generation)
	for (y = 0; y < m_iHeight; y++)
		{
		for (x = 0; x < m_iWidth; x++)
			{
			int iLiveNeighbors = CountLiveNeighbors(x, y);
			SCell &cell = m_Grid.GetAt(y).GetAt(x);
			if(IsAlive(x, y))
				{
				if (iLiveNeighbors < 2 || iLiveNeighbors > 3)
					cell.bAliveNext = false;
				}
			else
				{
				if (iLiveNeighbors == 3)
					cell.bAliveNext = true;
				}
			}
		}

	//	Second pass updates bAlive from bAliveNext
	for (y = 0; y < m_iHeight; y++)
		{
		for (x = 0; x < m_iWidth; x++)
			{
			SCell &cell = m_Grid.GetAt(y).GetAt(x);
			cell.bAlive = cell.bAliveNext;
			}
		}
	}
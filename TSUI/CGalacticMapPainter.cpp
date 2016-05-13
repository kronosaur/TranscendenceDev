//	CGalacticMapPainter.cpp
//
//	CGalacticMapPainter class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

const int NODE_RADIUS =						6;
const int STARGATE_LINE_WIDTH =				3;
const int MIN_NODE_MARGIN =					64;

CGalacticMapPainter::CGalacticMapPainter (const CVisualPalette &VI, CSystemMap *pMap) : m_VI(VI),
		m_pMap(pMap),
		m_cxMap(-1),
		m_cyMap(-1),
		m_bFreeImage(false),
		m_pImage(NULL)

//	CGalacticMapPainter constructor

	{
	}

CGalacticMapPainter::~CGalacticMapPainter (void)

//	CGalacticMapPainter destructor

	{
	if (m_pImage && m_bFreeImage)
		delete m_pImage;
	}

void CGalacticMapPainter::AdjustCenter (const RECT &rcView, int xCenter, int yCenter, int iScale, int *retxCenter, int *retyCenter)

//	AdjustCenter
//
//	Adjusts the center coordinates so that the map is always fully visible

	{
	Init();

	//	Compute the dimensions of the map to paint

	int cxWidth = m_cxMap;
	int cyHeight = m_cyMap;
	int cxMap = (100 * RectWidth(rcView) / iScale);
	int cyMap = (100 * RectHeight(rcView) / iScale);

	//	Compute the given center in map image coordinates

	int xMapCenter = Max(cxMap / 2, Min((cxWidth / 2) + xCenter, cxWidth - (cxMap / 2)));
	int yMapCenter = Max(cyMap / 2, Min((cyHeight / 2) - yCenter, cyHeight - (cyMap / 2)));

	//	Convert back after adjustment

	if ((iScale * cxWidth / 100) < RectWidth(rcView))
		*retxCenter = 0;
	else
		*retxCenter = xMapCenter - (cxWidth / 2);

	if ((iScale * cyHeight / 100) < RectHeight(rcView))
		*retyCenter = 0;
	else
		*retyCenter = (cyHeight / 2) - yMapCenter;
	}

void CGalacticMapPainter::DrawNode (CG32bitImage &Dest, CTopologyNode *pNode, int x, int y, CG32bitPixel rgbColor)

//	DrawNode
//
//	Draws a topology node

	{
	CGDraw::CircleGradient(Dest, x + 2, y + 2, NODE_RADIUS + 1, 0);
	CGDraw::Circle(Dest, x, y, NODE_RADIUS, rgbColor);
	CGDraw::CircleGradient(Dest, x - 2, y - 2, NODE_RADIUS - 3, CG32bitPixel(0xff, 0xff, 0xff));

	m_VI.GetFont(fontMediumBold).DrawText(Dest,
			x, y + NODE_RADIUS + 2,
			CG32bitPixel(255, 255, 255),
			pNode->GetSystemName(),
			CG16bitFont::AlignCenter);

	//	Debug info

	if (g_pUniverse->InDebugMode() && m_pMap->DebugShowAttributes())
		{
		CString sLine = pNode->GetAttributes();
#if 0
		int iPos = strFind(sLine, CONSTLIT("level"));
		if (iPos != -1)
			sLine = strSubString(sLine, iPos, 7);
#endif

		m_VI.GetFont(fontMedium).DrawText(Dest,
				x,
				y + NODE_RADIUS + 2 + m_VI.GetFont(fontMediumBold).GetHeight(),
				CG32bitPixel(128, 128, 128),
				sLine,
				CG16bitFont::AlignCenter);
		}
	}

void CGalacticMapPainter::GalacticToView (int x, int y, const RECT &rcView, int xCenter, int yCenter, int iScale, int *retx, int *rety) const

//	GalacticToView
//
//	Converts from galactic (map) coordinates to view coordinates (relative to
//  the entire session).

	{
	int xViewCenter = rcView.left + (RectWidth(rcView) / 2);
	int yViewCenter = rcView.top + (RectHeight(rcView) / 2);

	//	Convert to view coordinates

	*retx = xViewCenter + iScale * (x - xCenter) / 100;
	*rety = yViewCenter + iScale * (yCenter - y) / 100;
	}

void CGalacticMapPainter::Init (void)

//	Init
//
//	Initialize what we need to paint.

	{
	int i;

	if (m_cxMap == -1)
		{
		//	Allocate a new image and initialize it with the background

		m_pImage = m_pMap->CreateBackgroundImage();
		m_bFreeImage = (m_pImage != NULL);

		//	Compute the size of the map based on whatever is greater, then background
		//	image or the position of the nodes

		int xMin = 0;
		int xMax = 0;
		int yMin = 0;
		int yMax = 0;

		for (i = 0; i < g_pUniverse->GetTopologyNodeCount(); i++)
			{
			CTopologyNode *pNode = g_pUniverse->GetTopologyNode(i);
			
			int xPos, yPos;
			if (pNode->GetDisplayPos(&xPos, &yPos) == m_pMap 
					&& pNode->IsKnown() 
					&& !pNode->IsEndGame())
				{
				if (xPos < xMin)
					xMin = xPos;
				if (xPos > xMax)
					xMax = xPos;
				if (yPos < yMin)
					yMin = yPos;
				if (yPos > yMax)
					yMax = yPos;
				}
			}

		int cxExtent = 2 * Max(xMax, -xMin) + MIN_NODE_MARGIN;
		int cyExtent = 2 * Max(yMax, -yMin) + MIN_NODE_MARGIN;
		m_cxMap = Max(cxExtent, (m_pImage ? m_pImage->GetWidth() : 0));
		m_cyMap = Max(cyExtent, (m_pImage ? m_pImage->GetHeight() : 0));
		}
	}

void CGalacticMapPainter::Paint (CG32bitImage &Dest, const RECT &rcView, int xCenter, int yCenter, int iScale)

//	Paint
//
//	Paints the galactic map to the given destination, centered at the
//	given coordinates.
//
//	iScale 100 = normal
//	iScale 200 = x2
//	iScale 400 = x4

	{
	int i, j, k;

	ASSERT(iScale > 0);

	//	Initialize

	Init();

	//	Paint the image, if we have it

	if (m_cxMap > 0 && m_cyMap > 0)
		{
		//	Compute some metrics

		m_iScale = iScale;
		m_xCenter = xCenter;
		m_yCenter = yCenter;

		CG32bitPixel rgbNodeColor = CG32bitPixel(255, 200, 128);
		CG32bitPixel rgbStargateColor = CG32bitPixel(160, 255, 128);

		int cxWidth = (m_pImage ? m_pImage->GetWidth() : 0);
		int cyHeight = (m_pImage ? m_pImage->GetHeight() : 0);

		//	Compute the dimensions of the map to paint

		int cxMap = (100 * RectWidth(rcView) / iScale);
		int cyMap = (100 * RectHeight(rcView) / iScale);
		m_xViewCenter = rcView.left + (RectWidth(rcView) / 2);
		m_yViewCenter = rcView.top + (RectHeight(rcView) / 2);

		//	Compute the given center in map image coordinates

		int xMapCenter = (cxWidth / 2) + xCenter;
		int yMapCenter = (cyHeight / 2) - yCenter;

		//	Compute the upper-left corner of the map

		int xMap = xMapCenter - (cxMap / 2);
		int yMap = yMapCenter - (cyMap / 2);

		//	Fill the borders, in case we the image won't fit

		if (xMap < 0)
			Dest.Fill(rcView.left, rcView.top, -(xMap * iScale) / 100, RectHeight(rcView), 0);

		if (yMap < 0)
			Dest.Fill(rcView.left, rcView.top, RectWidth(rcView), (-yMap * iScale) / 100, 0);

		if (xMap + cxMap > cxWidth)
			{
			int cx = iScale * ((xMap + cxMap) - cxWidth) / 100;
			Dest.Fill(rcView.right - cx, rcView.top, cx, RectHeight(rcView), 0);
			}

		if (yMap + cyMap > cyHeight)
			{
			int cy = iScale * ((yMap + cyMap) - cyHeight) / 100;
			Dest.Fill(rcView.left, rcView.bottom - cy, RectWidth(rcView), cy, 0);
			}

		//	Blt

		if (m_pImage)
			CGDraw::BltScaled(Dest, rcView.left, rcView.top, RectWidth(rcView), RectHeight(rcView),	*m_pImage, xMap, yMap, cxMap, cyMap);

		//	Loop over all nodes and clear marks on the ones that we need to draw

		for (i = 0; i < g_pUniverse->GetTopologyNodeCount(); i++)
			{
			CTopologyNode *pNode = g_pUniverse->GetTopologyNode(i);
			
			int xPos, yPos;
			pNode->SetMarked(pNode->GetDisplayPos(&xPos, &yPos) != m_pMap 
					|| !pNode->IsKnown() 
					|| pNode->IsEndGame());
			}

		//	Paint the nodes

		for (i = 0; i < g_pUniverse->GetTopologyNodeCount(); i++)
			{
			CTopologyNode *pNode = g_pUniverse->GetTopologyNode(i);
			if (!pNode->IsMarked())
				{
				SPoint Start;
				pNode->GetDisplayPos(&Start.x, &Start.y);
				Start = Xform(Start);

				//	Draw gate connections

				for (j = 0; j < pNode->GetStargateCount(); j++)
					{
					CTopologyNode::SStargateRouteDesc RouteDesc;
					pNode->GetStargateRouteDesc(j, &RouteDesc);

					if (RouteDesc.pToNode && !RouteDesc.pToNode->IsMarked())
						{
						SPoint End;
						RouteDesc.pToNode->GetDisplayPos(&End.x, &End.y);
						End = Xform(End);

						//	If this is a curved path, the draw it

						if (RouteDesc.MidPoints.GetCount() > 0)
							{
							int iCurMid = 0;
							while (iCurMid <= RouteDesc.MidPoints.GetCount())
								{
								//	If we're at the end and we don't have enough points 
								//	for a curve, then just draw a line.
								//
								//	LATER: We try to do a curved line.

								if (iCurMid == RouteDesc.MidPoints.GetCount())
									{
									SPoint ptFrom = Xform(RouteDesc.MidPoints[iCurMid - 1]);

									Dest.DrawLine(ptFrom.x, ptFrom.y, End.x, End.y, STARGATE_LINE_WIDTH, rgbStargateColor);
									}

								//	Otherwise, we draw a curve

								else
									{
									SPoint ptMid = Xform(RouteDesc.MidPoints[iCurMid]);
									SPoint ptFrom = (iCurMid > 0 ? Xform(RouteDesc.MidPoints[iCurMid - 1]) : Start);
									SPoint ptTo = (iCurMid + 1 < RouteDesc.MidPoints.GetCount() ? Xform(RouteDesc.MidPoints[iCurMid + 1]) : End);

									CGDraw::QuadCurve(Dest, ptFrom.x, ptFrom.y, ptTo.x, ptTo.y, ptMid.x, ptMid.y, STARGATE_LINE_WIDTH, rgbStargateColor);
									}

								iCurMid += 2;
								}

							//	If in debug mode, paint all the intermediate nodes

							if (g_pUniverse->InDebugMode())
								{
								for (k = 0; k < RouteDesc.MidPoints.GetCount(); k++)
									{
									SPoint ptPos = Xform(RouteDesc.MidPoints[k]);
									Dest.DrawDot(ptPos.x, ptPos.y, CG32bitPixel(255, 128, 0), markerMediumCross);
									}
								}
							}

						//	Otherwise, straight line

						else
							Dest.DrawLine(Start.x, Start.y, End.x, End.y, STARGATE_LINE_WIDTH, rgbStargateColor);
						}
					}

				//	Draw star system

				if (Start.x >= rcView.left && Start.x < rcView.right && Start.y >= rcView.top && Start.y < rcView.bottom)
					DrawNode(Dest, pNode, Start.x, Start.y, rgbNodeColor);

				pNode->SetMarked();
				}
			}
		}
	else
		Dest.Fill(rcView.left, rcView.top, RectWidth(rcView), RectHeight(rcView), 0);
	}

void CGalacticMapPainter::ViewToGalactic (int x, int y, const RECT &rcView, int xCenter, int yCenter, int iScale, int *retx, int *rety) const

//  ViewToGalactic
//
//  Converts from session/screen coordinates to galactic (map) coordinates.

    {
	int xViewCenter = rcView.left + (RectWidth(rcView) / 2);
	int yViewCenter = rcView.top + (RectHeight(rcView) / 2);

	//	Convert to galactic coordinates

    *retx = (100 * (x - xViewCenter) / iScale) + xCenter;
    *rety = yCenter - (100 * (y - yViewCenter) / iScale);
    }


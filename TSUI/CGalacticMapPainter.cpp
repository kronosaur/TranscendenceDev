//	CGalacticMapPainter.cpp
//
//	CGalacticMapPainter class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

const int NODE_RADIUS =						6;
const int HIT_TEST_RADIUS =                 8;
const int SELECTION_RADIUS =                24;
const int SELECTION_WIDTH =                 4;
const int STARGATE_LINE_WIDTH =				1;
const int MIN_NODE_MARGIN =					64;

const int NAME_PADDING_X =                  4;
const int NAME_SPACING_Y =                  4;
const int CORNER_RADIUS =                   2;

const Metric MIN_SYSTEM_THUMB_SCALE =       0.5;

const CG32bitPixel RGB_STARGATE =		    CG32bitPixel(178, 217, 255);    //  H:210 S:30 B:100
const CG32bitPixel RGB_SYSTEM_NAME =		CG32bitPixel(128, 191, 255);    //  H:210 S:50 B:100
//const CG32bitPixel RGB_SYSTEM_NAME =	    CG32bitPixel(204, 184, 163);    //  H:30  S:20 B:80
//const CG32bitPixel RGB_SYSTEM_NAME =	    CG32bitPixel(143, 159, 179);    //  H:210 S:20 B:70
//const CG32bitPixel RGB_SYSTEM_NAME =        CG32bitPixel(115, 121, 128);    //  H:210 S:10 B:50
const CG32bitPixel RGB_SYSTEM_NAME_BACK =   CG32bitPixel(23, 24, 26, 96);  //  H:210 S:10 B:20

CGalacticMapPainter::CGalacticMapPainter (const CVisualPalette &VI, CSystemMap *pMap, CSystemMapThumbnails &SystemMapThumbnails) : m_VI(VI),
		m_pMap(pMap),
        m_SystemMapThumbnails(SystemMapThumbnails),
		m_cxMap(-1),
		m_cyMap(-1),
		m_bFreeImage(false),
		m_rImageScale(1.0),
		m_pImage(NULL),
        m_pSelected(NULL),
        m_iSelectAngle(0),
        m_iScale(100),
        m_xCenter(0),
        m_yCenter(0)

//	CGalacticMapPainter constructor

	{
    RECT rcRect;
    rcRect.left = 0;
    rcRect.top = 0;
    rcRect.right = 100;
    rcRect.bottom = 100;

    SetViewport(rcRect);

	Init();
	}

CGalacticMapPainter::~CGalacticMapPainter (void)

//	CGalacticMapPainter destructor

	{
	if (m_pImage && m_bFreeImage)
		delete m_pImage;
	}

void CGalacticMapPainter::AdjustCenter (int xCenter, int yCenter, int iScale, int *retxCenter, int *retyCenter) const

//	AdjustCenter
//
//	Adjusts the center coordinates so that the map is always fully visible. The 
//  viewport must be set, but scale and position are inputs to this.

	{
	//	Compute the dimensions of the map to paint

	int cxWidth = m_cxMap;
	int cyHeight = m_cyMap;
	int cxMap = (100 * RectWidth(m_rcView) / iScale);
	int cyMap = (100 * RectHeight(m_rcView) / iScale);

	//	Compute the given center in map image coordinates

	int xMapCenter = Max(cxMap / 2, Min((cxWidth / 2) + xCenter, cxWidth - (cxMap / 2)));
	int yMapCenter = Max(cyMap / 2, Min((cyHeight / 2) - yCenter, cyHeight - (cyMap / 2)));

	//	Convert back after adjustment

	if ((iScale * cxWidth / 100) < RectWidth(m_rcView))
		*retxCenter = 0;
	else
		*retxCenter = xMapCenter - (cxWidth / 2);

	if ((iScale * cyHeight / 100) < RectHeight(m_rcView))
		*retyCenter = 0;
	else
		*retyCenter = (cyHeight / 2) - yMapCenter;
	}

void CGalacticMapPainter::DrawNode (CG32bitImage &Dest, CTopologyNode *pNode, int x, int y, Metric rScale, CG32bitPixel rgbColor) const

//	DrawNode
//
//	Draws a topology node

	{
    const CG16bitFont &NameFont = m_VI.GetFont(fontMedium);

    //  Draw selection, if necessary

    if (pNode == m_pSelected)
        DrawSelection(Dest, x, y, m_VI.GetColor(colorAreaDialogHighlight));

    //  Draw the node (either as a full system thumbnail or just the stars)

#ifdef FULL_SYSTEM_THUMBNAILS
    bool bFullSystem = ((pNode->GetLastVisitedTime() != 0xffffffff) && rScale > MIN_SYSTEM_THUMB_SCALE);
#else
    bool bFullSystem = false;
#endif

    m_SystemMapThumbnails.DrawThumbnail(pNode, Dest, x, y, bFullSystem, rScale);

    //  Draw the name label

    int cxName = NameFont.MeasureText(pNode->GetSystemName());
    int cxNameBack = cxName + 2 * NAME_PADDING_X;
    int xNameBack = x - (cxNameBack / 2);
    int yName = y + NODE_RADIUS + NAME_SPACING_Y;

    CGDraw::RoundedRect(Dest, xNameBack, yName, cxNameBack, NameFont.GetHeight(), CORNER_RADIUS, RGB_SYSTEM_NAME_BACK);

	NameFont.DrawText(Dest,
			xNameBack + NAME_PADDING_X, yName,
			RGB_SYSTEM_NAME,
			pNode->GetSystemName(),
			0);

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

void CGalacticMapPainter::DrawNodeConnections (CG32bitImage &Dest, CTopologyNode *pNode, int x, int y) const

//	DrawNodeConnections
//
//	Draws gate connections from this node to all other known systems.

	{
	int i, j;

	//	Compute some visual attributes.

	CG32bitPixel rgbStargateColor = (m_pMap ? m_pMap->GetStargateLineColor() : RGB_STARGATE);
    int iStargateLine = Max(1, mathRound(STARGATE_LINE_WIDTH * (Metric)m_iScale / 100.0));

	//	Paint each gate line

	for (i = 0; i < pNode->GetStargateCount(); i++)
		{
		CTopologyNode::SStargateRouteDesc RouteDesc;
		pNode->GetStargateRouteDesc(i, &RouteDesc);

		if (RouteDesc.pToNode 
				&& !RouteDesc.pToNode->IsMarked()
				&& RouteDesc.pToNode->IsKnown())
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

						Dest.DrawLine(ptFrom.x, ptFrom.y, End.x, End.y, iStargateLine, rgbStargateColor);
						}

					//	Otherwise, we draw a curve

					else
						{
						SPoint ptStart = {x, y};
						SPoint ptMid = Xform(RouteDesc.MidPoints[iCurMid]);
						SPoint ptFrom = (iCurMid > 0 ? Xform(RouteDesc.MidPoints[iCurMid - 1]) : ptStart);
						SPoint ptTo = (iCurMid + 1 < RouteDesc.MidPoints.GetCount() ? Xform(RouteDesc.MidPoints[iCurMid + 1]) : End);

						CGDraw::QuadCurve(Dest, ptFrom.x, ptFrom.y, ptTo.x, ptTo.y, ptMid.x, ptMid.y, iStargateLine, rgbStargateColor);
						}

					iCurMid += 2;
					}

				//	If in debug mode, paint all the intermediate nodes

				if (g_pUniverse->InDebugMode())
					{
					for (j = 0; j < RouteDesc.MidPoints.GetCount(); j++)
						{
						SPoint ptPos = Xform(RouteDesc.MidPoints[j]);
						Dest.DrawDot(ptPos.x, ptPos.y, CG32bitPixel(255, 128, 0), markerMediumCross);
						}
					}
				}

			//	Otherwise, straight line

			else
				Dest.DrawLine(x, y, End.x, End.y, iStargateLine, rgbStargateColor);
			}
		}
	}

void CGalacticMapPainter::DrawSelection (CG32bitImage &Dest, int x, int y, CG32bitPixel rgbColor) const

//  DrawSelection
//
//  Draws a selection

    {
    int i;
    const int ANGLE_INC = 90;

    //  Draw four arcs

    int iAngle = m_iSelectAngle;
    for (i = 0; i < 4; i++)
        {
        CGDraw::Arc(Dest, x, y, SELECTION_RADIUS, iAngle, iAngle + ANGLE_INC, SELECTION_WIDTH, rgbColor, CGDraw::blendNormal, SELECTION_WIDTH, 0);
        iAngle += ANGLE_INC;
        }

    m_iSelectAngle = AngleMod(m_iSelectAngle + 1);
    }

void CGalacticMapPainter::DrawUnknownNode (CG32bitImage &Dest, CTopologyNode *pNode, int x, int y, Metric rScale, CG32bitPixel rgbColor) const

//	DrawUnknownNode
//
//	Draws a topology node

	{
    //  Draw selection, if necessary

    if (pNode == m_pSelected)
        DrawSelection(Dest, x, y, m_VI.GetColor(colorAreaDialogHighlight));

	//	Draw a star

	int RADIUS = 24;
	int INTENSITY = 25;
	CG32bitPixel RGB_PRIMARY = CG32bitPixel(255, 255, 255);
	CG32bitPixel RGB_SECONDARY = CG32bitPixel(255, 255, 255);
	BYTE OPACITY = 255;

	//	Radius changes based on scale

	int iRadius = mathRound(rScale * RADIUS);

	//	Create a color table

	TArray<CG32bitPixel> ColorTable;
	CPaintHelper::CalcSmoothColorTable(iRadius, INTENSITY, RGB_PRIMARY, RGB_SECONDARY, OPACITY, &ColorTable);

	//	Paint

	CGDraw::Circle(Dest, x, y, iRadius, ColorTable, CGDraw::blendScreen);
	}

void CGalacticMapPainter::GalacticToView (int x, int y, int xCenter, int yCenter, int iScale, int *retx, int *rety) const

//	GalacticToView
//
//	Converts from galactic (map) coordinates to view coordinates (relative to
//  the entire session).

	{
	*retx = m_xViewCenter + iScale * (x - xCenter) / 100;
	*rety = m_yViewCenter + iScale * (yCenter - y) / 100;
	}

bool CGalacticMapPainter::HitTest (int x, int y, SSelectResult &Result) const

//  HitTest
//
//  Returns TRUE if we clicked on a node

    {
    int i;

    //  Convert to galactic coordinates

    int xPos, yPos;
    ViewToGalactic(x, y, m_xCenter, m_yCenter, m_iScale, &xPos, &yPos);

    //  See if we hit a node

    for (i = 0; i < g_pUniverse->GetTopologyNodeCount(); i++)
        {
        CTopologyNode *pNode = g_pUniverse->GetTopologyNode(i);
        int xNode, yNode;
        if (!pNode->IsPositionKnown()
                || pNode->IsEndGame()
                || pNode->GetDisplayPos(&xNode, &yNode) != m_pMap)
            continue;

        int xDiff = Absolute(xNode - xPos);
        int yDiff = Absolute(yNode - yPos);
        if (xDiff < HIT_TEST_RADIUS && yDiff < HIT_TEST_RADIUS)
            {
            Result.pNode = pNode;
            return true;
            }
        }

    //  If we get this far, we did not find a node.

    return false;
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

		m_pImage = m_pMap->CreateBackgroundImage(&m_rImageScale);
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
					&& pNode->IsPositionKnown() 
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
		m_cxMap = Max(cxExtent, GetImageGalacticWidth());
		m_cyMap = Max(cyExtent, GetImageGalacticHeight());
		}
	}

void CGalacticMapPainter::Paint (CG32bitImage &Dest) const

//	Paint
//
//	Paints the galactic map to the given destination, centered at the
//	given coordinates.
//
//	iScale 100 = normal
//	iScale 200 = x2
//	iScale 400 = x4

	{
	int i;

	ASSERT(m_iScale > 0);

    //  System thumbnails are full-res at x4

    Metric rScale = m_iScale / 400.0;

    //  Compute the stargate line width based on scale

    int iStargateLine = Max(1, mathRound(STARGATE_LINE_WIDTH * (Metric)m_iScale / 100.0));

	//	Paint the image, if we have it

	if (m_cxMap > 0 && m_cyMap > 0)
		{
		//	Compute some metrics

		CG32bitPixel rgbNodeColor = CG32bitPixel(255, 200, 128);
		CG32bitPixel rgbStargateColor = (m_pMap ? m_pMap->GetStargateLineColor() : RGB_STARGATE);

		int cxWidth = (m_pImage ? m_pImage->GetWidth() : 0);
		int cyHeight = (m_pImage ? m_pImage->GetHeight() : 0);

		//	Compute the dimensions of the map to paint

		int cxMap = (int)((100 * RectWidth(m_rcView) / m_iScale) * m_rImageScale);
		int cyMap = (int)((100 * RectHeight(m_rcView) / m_iScale) * m_rImageScale);

		//	Compute the given center in map image coordinates

		int xMapCenter = (cxWidth / 2) + (int)(m_xCenter * m_rImageScale);
		int yMapCenter = (cyHeight / 2) - (int)(m_yCenter * m_rImageScale);

		//	Compute the upper-left corner of the map

		int xMap = xMapCenter - (cxMap / 2);
		int yMap = yMapCenter - (cyMap / 2);

		//	Fill the borders, in case we the image won't fit

		if (xMap < 0)
			{
			int cx = -(int)(xMap * m_iScale / m_rImageScale) / 100;
			Dest.Fill(m_rcView.left, m_rcView.top, cx, RectHeight(m_rcView), 0);
			}

		if (yMap < 0)
			{
			int cy = -(int)(yMap * m_iScale / m_rImageScale) / 100;
			Dest.Fill(m_rcView.left, m_rcView.top, RectWidth(m_rcView), cy, 0);
			}

		if (xMap + cxMap > cxWidth)
			{
			int cx = (int)((m_iScale * ((xMap + cxMap) - cxWidth)) / m_rImageScale) / 100;
			Dest.Fill(m_rcView.right - cx, m_rcView.top, cx, RectHeight(m_rcView), 0);
			}

		if (yMap + cyMap > cyHeight)
			{
			int cy = (int)((m_iScale * ((yMap + cyMap) - cyHeight)) / m_rImageScale) / 100;
			Dest.Fill(m_rcView.left, m_rcView.bottom - cy, RectWidth(m_rcView), cy, 0);
			}

		//	Blt

		if (m_pImage)
			CGDraw::BltScaled(Dest, m_rcView.left, m_rcView.top, RectWidth(m_rcView), RectHeight(m_rcView), *m_pImage, xMap, yMap, cxMap, cyMap);

		//	Loop over all nodes and clear marks on the ones that we need to draw

		for (i = 0; i < g_pUniverse->GetTopologyNodeCount(); i++)
			{
			CTopologyNode *pNode = g_pUniverse->GetTopologyNode(i);
			
			int xPos, yPos;
			pNode->SetMarked(pNode->GetDisplayPos(&xPos, &yPos) != m_pMap 
					|| !pNode->IsPositionKnown() 
					|| pNode->IsEndGame());
			}

		//	Paint the nodes

		for (i = 0; i < g_pUniverse->GetTopologyNodeCount(); i++)
			{
			CTopologyNode *pNode = g_pUniverse->GetTopologyNode(i);
			if (pNode->IsMarked())
				continue;

			SPoint Start;
			pNode->GetDisplayPos(&Start.x, &Start.y);
			Start = Xform(Start);

			//	Draw gate connections

			if (pNode->IsKnown())
				DrawNodeConnections(Dest, pNode, Start.x, Start.y);

			//	Draw star system

			if (Start.x >= m_rcView.left && Start.x < m_rcView.right && Start.y >= m_rcView.top && Start.y < m_rcView.bottom)
				{
				if (pNode->IsKnown())
					DrawNode(Dest, pNode, Start.x, Start.y, rScale, rgbNodeColor);
				else
					DrawUnknownNode(Dest, pNode, Start.x, Start.y, rScale, rgbNodeColor);
				}

			pNode->SetMarked();
			}
		}
	else
		Dest.Fill(m_rcView.left, m_rcView.top, RectWidth(m_rcView), RectHeight(m_rcView), 0);
	}

void CGalacticMapPainter::ViewToGalactic (int x, int y, int xCenter, int yCenter, int iScale, int *retx, int *rety) const

//  ViewToGalactic
//
//  Converts from session/screen coordinates to galactic (map) coordinates.

    {
    *retx = (100 * (x - m_xViewCenter) / iScale) + xCenter;
    *rety = yCenter - (100 * (y - m_yViewCenter) / iScale);
    }


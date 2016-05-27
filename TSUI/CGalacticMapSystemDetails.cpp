//	CGalacticMapSystemDetails.h
//
//	CGalacticMapSystemDetails class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ID_STATION_LIST                         CONSTLIT("idStationList")

#define PROP_COLOR								CONSTLIT("color")
#define PROP_ENABLED							CONSTLIT("enabled")
#define PROP_FADE_EDGE_HEIGHT					CONSTLIT("fadeEdgeHeight")
#define PROP_FILL_TYPE							CONSTLIT("fillType")
#define PROP_FONT								CONSTLIT("font")
#define PROP_LINE_COLOR							CONSTLIT("lineColor")
#define PROP_LINE_PADDING						CONSTLIT("linePadding")
#define PROP_LINE_TYPE							CONSTLIT("lineType")
#define PROP_LINE_WIDTH							CONSTLIT("lineWidth")
#define PROP_LL_RADIUS							CONSTLIT("llRadius")
#define PROP_LR_RADIUS							CONSTLIT("lrRadius")
#define PROP_OPACITY							CONSTLIT("opacity")
#define PROP_PADDING_BOTTOM						CONSTLIT("paddingBottom")
#define PROP_POSITION							CONSTLIT("position")
#define PROP_SCALE								CONSTLIT("scale")
#define PROP_TEXT								CONSTLIT("text")
#define PROP_TEXT_ALIGN_HORZ					CONSTLIT("textAlignHorz")
#define PROP_UL_RADIUS							CONSTLIT("ulRadius")
#define PROP_UR_RADIUS							CONSTLIT("urRadius")

const int MAJOR_PADDING_TOP =					20;
const int INTER_LINE_SPACING =					20;
const int MAX_ICON_SIZE =                       100;
const int MIN_ICON_SIZE =                       32;
const Metric ICON_SCALE =                       0.25;
const int ICON_SPACING_HORZ =                   16;

CGalacticMapSystemDetails::CGalacticMapSystemDetails (const CVisualPalette &VI, CReanimator &Reanimator, const RECT &rcPane) :
        m_VI(VI),
        m_Reanimator(Reanimator),
        m_rcPane(rcPane)

//  CGalacticMapSystemDetails constructor

    {
    }

bool CGalacticMapSystemDetails::CreateDetailsPane (CTopologyNode *pNode, IAnimatron **retpAni)

//  CreateDetailsPane
//
//  Creates an animation showing the details for a given system. Returns FALSE 
//  if we fail to create the pane.

    {
    int i;

    //  Get the list of objects at this node

    TArray<CObjectTracker::SObjEntry> Objs;
    g_pUniverse->GetGlobalObjects().GetGalacticMapObjects(pNode, Objs);

    //  Create a sequencer which will be the root pane.

    CAniSequencer *pRoot = new CAniSequencer;
	pRoot->SetPropertyVector(PROP_POSITION, CVector(m_rcPane.left, m_rcPane.top));

    //  Create a listbox which will hold all stations in the system.

    RECT rcList = m_rcPane;

    CAniListBox *pList;
    m_VI.CreateListBox(pRoot, ID_STATION_LIST, 0, 0, RectWidth(rcList), RectHeight(rcList), 0, &pList);

    //  Add all the stations in the node

    int y = MAJOR_PADDING_TOP;
    for (i = 0; i < Objs.GetCount(); i++)
        {
		//	Generate a record for the object

		IAnimatron *pEntry;
		int cyHeight;
		CreateObjEntry(Objs[i], y, RectWidth(rcList), &pEntry, &cyHeight);

		pList->AddEntry(strFromInt(Objs[i].dwObjID), pEntry);
		y += cyHeight + INTER_LINE_SPACING;
        }

    //  Done

    *retpAni = pRoot;

    return true;
    }

void CGalacticMapSystemDetails::CreateObjEntry (const CObjectTracker::SObjEntry &Obj, int yPos, int cxWidth, IAnimatron **retpAni, int *retcyHeight)

//  CreateObjEntry
//
//  Creates an entry for the station.

    {
	const CG16bitFont &HeaderFont = m_VI.GetFont(fontHeader);
	const CG16bitFont &TextFont = m_VI.GetFont(fontMedium);

	//	Start with a sequencer

	CAniSequencer *pRoot = new CAniSequencer;
	pRoot->SetPropertyVector(PROP_POSITION, CVector(0, yPos));
    int x = 0;
    int y = 0;

    //  Create the icon for the object

    CG32bitImage *pIcon;
    if (CreateObjIcon(Obj, &pIcon))
        {
		int xOffset = (MAX_ICON_SIZE - pIcon->GetWidth()) / 2;
		IAnimatron *pIconAni = new CAniRect;
		pIconAni->SetPropertyVector(PROP_POSITION, CVector(x + xOffset, 0));
		pIconAni->SetPropertyVector(PROP_SCALE, CVector(pIcon->GetWidth(), pIcon->GetHeight()));
		pIconAni->SetFillMethod(new CAniImageFill(pIcon, true));

		pRoot->AddTrack(pIconAni, 0);
        }

    //  Add the object name

    CString sHeading = Obj.sName;
	int xText = x + MAX_ICON_SIZE + ICON_SPACING_HORZ;
    int cxText = cxWidth - (MAX_ICON_SIZE + ICON_SPACING_HORZ);

	IAnimatron *pName = new CAniText;
	pName->SetPropertyVector(PROP_POSITION, CVector(xText, y));
	pName->SetPropertyVector(PROP_SCALE, CVector(10000, 1000));
	pName->SetPropertyColor(PROP_COLOR, m_VI.GetColor(colorTextHighlight));
	pName->SetPropertyFont(PROP_FONT, &HeaderFont);
	pName->SetPropertyString(PROP_TEXT, sHeading);

	pRoot->AddTrack(pName, 0);
	y += HeaderFont.GetHeight();

    //  Add description

    CString sDesc = CONSTLIT("This is a cool station that will do some stuff and maybe it will be good or maybe it will not be. Either way, you should visit it. Maybe.");

	IAnimatron *pDesc = new CAniText;
	pDesc->SetPropertyVector(PROP_POSITION, CVector(xText, y));
	pDesc->SetPropertyVector(PROP_SCALE, CVector(cxText, 1000));
	pDesc->SetPropertyColor(PROP_COLOR, m_VI.GetColor(colorTextNormal));
	pDesc->SetPropertyFont(PROP_FONT, &TextFont);
	pDesc->SetPropertyString(PROP_TEXT, sDesc);

	RECT rcLine;
	pDesc->GetSpacingRect(&rcLine);

	pRoot->AddTrack(pDesc, 0);
	y += RectHeight(rcLine);

    //  Done

    *retpAni = pRoot;

	if (retcyHeight)
		*retcyHeight = Max((pIcon ? pIcon->GetHeight() : 0), y);
    }

bool CGalacticMapSystemDetails::CreateObjIcon (const CObjectTracker::SObjEntry &Obj, CG32bitImage **retpIcon)

//  CreateObjIcon
//
//  Creates an icon for the object. Returns FALSE if we could not create the 
//  icon.

    {
	CCompositeImageModifiers Modifiers;
    if (Obj.fShowDestroyed)
        Modifiers.SetStationDamage(true);

    int iRotation;
    const CObjectImageArray &FullImage = Obj.pType->GetTypeImage().GetImage(Obj.ImageSel, Modifiers, &iRotation);
    if (!FullImage.IsLoaded())
        return false;

	CG32bitImage *pBmpImage = &FullImage.GetImage(strFromInt(Obj.pType->GetUNID()));
    if (pBmpImage == NULL)
        return false;

	RECT rcBmpImage = FullImage.GetImageRect(0, iRotation);
    int iSize = Max(RectWidth(rcBmpImage), RectHeight(rcBmpImage));
    if (iSize <= 0)
        return false;

    //  Compute scale

    int iScaleSize = Max(MIN_ICON_SIZE, Min((int)(ICON_SCALE * iSize), MAX_ICON_SIZE));
    Metric rScale = (Metric)iScaleSize / (Metric)iSize;

    //  Create a small version

    CG32bitImage *pIcon = new CG32bitImage;
	if (!pIcon->CreateFromImageTransformed(*pBmpImage,
			rcBmpImage.left,
			rcBmpImage.top,
			RectWidth(rcBmpImage),
			RectHeight(rcBmpImage),
			rScale,
			rScale,
			0.0))
        {
        delete pIcon;
        return false;
        }

    //  Done

    *retpIcon = pIcon;
    return true;
    }

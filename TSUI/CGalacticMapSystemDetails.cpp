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

const int LIST_PADDING_X =                      20;
const int HEADER_PADDING_X =                    20;
const int HEADER_PADDING_Y =                    20;
const int ATTRIB_SPACING_Y =					2;

static const CG32bitPixel RGB_MODIFIER_NORMAL_BACKGROUND =		CG32bitPixel(101,101,101);	//	H:0   S:0   B:40
static const CG32bitPixel RGB_MODIFIER_NORMAL_TEXT =			CG32bitPixel(220,220,220);	//	H:0   S:0   B:86

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

    TSortMap<CString, SObjDesc> Objs;
    GetObjList(pNode, Objs);

    //  Create a sequencer which will be the root pane.

    CAniSequencer *pRoot = new CAniSequencer;
	pRoot->SetPropertyVector(PROP_POSITION, CVector(m_rcPane.left, m_rcPane.top));

    //  Create a background for the whole pane

	IAnimatron *pFrame;
    m_VI.CreateFrame(pRoot, NULL_STR, 0, 0, RectWidth(m_rcPane), RectHeight(m_rcPane), CVisualPalette::OPTION_FRAME_TRANS, &pFrame);

	IAnimatron *pHeaderBack;
    m_VI.CreateFrameHeader(pRoot, NULL_STR, 0, 0, 0, 0, 0, &pHeaderBack);

    //  Add system information

    int cyHeader;
    CreateSystemHeader(pRoot, pNode, &cyHeader);

	//	Now that we know the size of the text, resize

	pHeaderBack->SetPropertyVector(PROP_SCALE, CVector(RectWidth(m_rcPane) - 2, cyHeader - 1));

	//	Compute the size of the list area

	RECT rcList;
	rcList.top = cyHeader;
	rcList.bottom = rcList.top + RectHeight(m_rcPane) - cyHeader;
	rcList.left = LIST_PADDING_X;
	rcList.right = RectWidth(m_rcPane) - LIST_PADDING_X;

    //  Create a listbox which will hold all stations in the system.

	if (Objs.GetCount() > 0 && pNode->IsKnown())
		{
		CAniListBox *pList;
		m_VI.CreateListBox(pRoot, ID_STATION_LIST, rcList.left, rcList.top, RectWidth(rcList), RectHeight(rcList), 0, &pList);

		//  Add all the stations in the node

		int y = MAJOR_PADDING_TOP;
		for (i = 0; i < Objs.GetCount(); i++)
			{
			//	Generate a record for the object

			IAnimatron *pEntry;
			int cyHeight;
			CreateObjEntry(Objs[i], y, RectWidth(rcList), &pEntry, &cyHeight);

			pList->AddEntry(strFromInt(Objs[i].ObjData.dwObjID), pEntry);
			y += cyHeight + INTER_LINE_SPACING;
			}
		}

	//	If no objects, then shrink the pane 

	else
		{
		pFrame->SetPropertyVector(PROP_SCALE, CVector(RectWidth(m_rcPane), cyHeader + 1));
		}

    //  Done

    *retpAni = pRoot;

    return true;
    }

void CGalacticMapSystemDetails::CreateObjEntry (const SObjDesc &Obj, int yPos, int cxWidth, IAnimatron **retpAni, int *retcyHeight)

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

    IAnimatron *pIconAni = NULL;
    CG32bitImage *pIcon = NULL;
    int xIcon = 0;
    if (CreateObjIcon(Obj.ObjData, &pIcon))
        {
		xIcon = x + ((MAX_ICON_SIZE - pIcon->GetWidth()) / 2);
		pIconAni = new CAniRect;
		pIconAni->SetPropertyVector(PROP_SCALE, CVector(pIcon->GetWidth(), pIcon->GetHeight()));
		pIconAni->SetFillMethod(new CAniImageFill(pIcon, true));

		pRoot->AddTrack(pIconAni, 0);
        }

    //  Keep track of the total height of the text part so we can vertically
    //  center it.

    int cyText = 0;

    //  Add the object name

    CString sHeading = CLanguage::ComposeNounPhrase(Obj.ObjData.sName, Obj.iCount, NULL_STR, Obj.ObjData.dwNameFlags, nounCountOnly | nounTitleCapitalize);
	int xText = x + MAX_ICON_SIZE + ICON_SPACING_HORZ;
    int cxText = cxWidth - (MAX_ICON_SIZE + ICON_SPACING_HORZ);

	IAnimatron *pName = new CAniText;
	pName->SetPropertyVector(PROP_SCALE, CVector(10000, 1000));
	pName->SetPropertyColor(PROP_COLOR, m_VI.GetColor(colorTextHighlight));
	pName->SetPropertyFont(PROP_FONT, &HeaderFont);
	pName->SetPropertyString(PROP_TEXT, sHeading);

	pRoot->AddTrack(pName, 0);
    cyText += HeaderFont.GetHeight();

	//	Add attributes, if necessary

	IAnimatron *pAttribs = NULL;
	int cyAttribs = 0;
	if (Obj.Attribs.GetCount() > 0)
		{
		m_VI.CreateCartoucheArea(pRoot, NULL_STR, 0, 0, cxText, Obj.Attribs, TextFont, &pAttribs, &cyAttribs);
		cyText += cyAttribs + ATTRIB_SPACING_Y;
		}

    //  Add description

    IAnimatron *pDesc = NULL;
    if (!Obj.ObjData.sNotes.IsBlank())
        {
        pDesc = new CAniText;
        pDesc->SetPropertyVector(PROP_SCALE, CVector(cxText, 1000));
		if (Obj.ObjData.fEnemy)
			pDesc->SetPropertyColor(PROP_COLOR, m_VI.GetColor(colorTextDisadvantage));
		else
			pDesc->SetPropertyColor(PROP_COLOR, m_VI.GetColor(colorTextNormal));
        pDesc->SetPropertyFont(PROP_FONT, &TextFont);
        pDesc->SetPropertyString(PROP_TEXT, Obj.ObjData.sNotes);

        RECT rcLine;
        pDesc->GetSpacingRect(&rcLine);

        pRoot->AddTrack(pDesc, 0);
        cyText += RectHeight(rcLine);
        }

    //  Center the text

    int cyEntry = Max((pIcon ? pIcon->GetHeight() : 0), cyText);
    int yText = (cyEntry - cyText) / 2;
	pName->SetPropertyVector(PROP_POSITION, CVector(xText, yText));
	yText += HeaderFont.GetHeight();

	if (pAttribs)
		{
		pAttribs->SetPropertyVector(PROP_POSITION, CVector(xText, yText));
		yText += cyAttribs + ATTRIB_SPACING_Y;
		}

    if (pDesc)
	    pDesc->SetPropertyVector(PROP_POSITION, CVector(xText, yText));

    //  Center the icon

    if (pIconAni)
        {
        int yIcon = (cyEntry - pIcon->GetHeight()) / 2;
		pIconAni->SetPropertyVector(PROP_POSITION, CVector(xIcon, yIcon));
        }

    //  Done

    *retpAni = pRoot;

    if (retcyHeight)
        *retcyHeight = cyEntry;
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

    int iVariant;
    const CObjectImageArray &FullImage = Obj.pType->GetTypeImage().GetImage(Obj.ImageSel, Modifiers, &iVariant);
    CG32bitImage *pBmpImage = (FullImage.IsLoaded() ? &FullImage.GetImage(CONSTLIT("Galactic map")) : NULL);
	RECT rcBmpImage = FullImage.GetImageRect(0, iVariant);
    if (pBmpImage == NULL)
        {
        CStationType *pStationType = CStationType::AsType(Obj.pType);
        if (pStationType == NULL)
            return false;

        //  If we can't find the standard image, see if we have a hero image.

        const CObjectImageArray &HeroImage = pStationType->GetHeroImage(CCompositeImageSelector(), Modifiers, &iVariant);
        pBmpImage = (HeroImage.IsLoaded() ? &HeroImage.GetImage(CONSTLIT("Galactic map")) : NULL);
        if (pBmpImage == NULL)
            return false;

        rcBmpImage = HeroImage.GetImageRect(0, iVariant);
        }

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

void CGalacticMapSystemDetails::CreateSystemHeader (CAniSequencer *pContainer, CTopologyNode *pTopology, int *retcyHeight) const

//  CreateSystemHeader
//
//  Creates info about the system.

    {
	SSystemHeader Header;
	GetSystemHeaderData(pTopology, Header);

    const CG16bitFont &TitleFont = m_VI.GetFont(fontHeader);
    const CG16bitFont &DescFont = m_VI.GetFont(fontMedium);

    int x = HEADER_PADDING_X;
    int y = HEADER_PADDING_Y;
    int cxWidth = RectWidth(m_rcPane) - (2 * HEADER_PADDING_X);

    //  System name

    int cyText;
    m_VI.CreateTextArea(pContainer, 
            NULL_STR,
            x,
            y,
            cxWidth,
            1000,
            Header.sTitle,
            m_VI.GetColor(colorTextHighlight),
            TitleFont,
            NULL,
            &cyText);
    y += cyText;

	//	Attributes

	if (Header.Attribs.GetCount() > 0)
		{
		m_VI.CreateCartoucheArea(pContainer,
				NULL_STR,
				x,
				y,
				cxWidth,
				Header.Attribs,
				DescFont,
				NULL,
				&cyText);
		y += cyText + ATTRIB_SPACING_Y;
		}

	//	Details

    m_VI.CreateTextArea(pContainer, 
            NULL_STR,
            x,
            y,
            cxWidth,
            1000,
            Header.sDetails,
            m_VI.GetColor(colorTextNormal),
            DescFont,
            NULL,
            &cyText);
    y += cyText;

	y += HEADER_PADDING_Y;

	if (retcyHeight)
		*retcyHeight = y;
    }

void CGalacticMapSystemDetails::GetObjAttribs (const CObjectTracker::SObjEntry &Obj, TArray<CCartoucheBlock::SCartoucheDesc> &retAttribs) const

//	GetObjAttribs
//
//	Returns attributes for the object.

	{
	//	Add currency

	CTradingDesc *pTrade = Obj.pType->GetTradingDesc();
	if (pTrade && !Obj.fEnemy && pTrade->HasConsumerService())
		{
		CEconomyType *pCurrencyType = pTrade->GetEconomyType();
		if (pCurrencyType && pCurrencyType != g_pUniverse->GetDefaultCurrency())
			{
			CCartoucheBlock::SCartoucheDesc *pEntry = retAttribs.Insert();
			pEntry->sText = pCurrencyType->GetCurrencyNamePlural();
			pEntry->rgbBack = RGB_MODIFIER_NORMAL_BACKGROUND;
			pEntry->rgbColor = RGB_MODIFIER_NORMAL_TEXT;
			}
		}
	}

bool CGalacticMapSystemDetails::GetObjList (CTopologyNode *pNode, TSortMap<CString, SObjDesc> &Results) const

//  GetObjList
//
//  Returns an ordered list of objects to show in the details pane for this 
//  node. We return FALSE if there are no objects in the list.

    {
    int i;

    //  Initialize

    Results.DeleteAll();

    //  Get the list of objects at this node

    TArray<CObjectTracker::SObjEntry> Objs;
    g_pUniverse->GetGlobalObjects().GetGalacticMapObjects(pNode, Objs);
    if (Objs.GetCount() == 0)
        return false;

    //  Now add them to our result sorted and collated

    for (i = 0; i < Objs.GetCount(); i++)
        {
        //  Sort stations into groups

        int iDispSort;

		//	Abandoned enemy stations always show last

		if (Objs[i].fEnemy && Objs[i].fShowDestroyed)
			iDispSort = 4;

		//	Friendly stations show first

		else if (Objs[i].fFriendly)
			iDispSort = 1;

		//	Next are neutral stations

		else if (!Objs[i].fEnemy)
			iDispSort = 2;

		//	And then enemy stations

		else
			iDispSort = 3;

        //  Higher level stations go first

        int iLevelSort = (MAX_ITEM_LEVEL + 1 - Objs[i].pType->GetLevel());

		//	Size is next

		int iSizeSort = 10000 - Min(10000, Objs[i].pType->GetPropertyInteger(CONSTLIT("size")));

        //  Abandoned stations are separate

        CString sAbandoned;
        if (Objs[i].fShowDestroyed)
            sAbandoned = CONSTLIT("-A");

        //  Generate a sort string. We want stations with the same type and name
        //  to be collapsed into a single entry.

        CString sSort = strPatternSubst(CONSTLIT("%d-%02d-%05d-%s-%08x-%s"), iDispSort, iLevelSort, iSizeSort, Objs[i].sName, Objs[i].pType->GetUNID(), sAbandoned);

        //  Add to our result list

        SObjDesc *pEntry = Results.SetAt(sSort);
        pEntry->iCount++;
		if (pEntry->iCount == 1)
			{
			pEntry->ObjData = Objs[i];
			GetObjAttribs(Objs[i], pEntry->Attribs);
			}
        }

    //  Done

    return true;
    }

void CGalacticMapSystemDetails::GetSystemHeaderData (CTopologyNode *pNode, SSystemHeader &Header) const

//	GetSystemHeaderData
//
//	Returns data to show in the system header.

	{
	//	Title is either the system name or "Unknown" if we don't know about this
	//	system.

	if (pNode->IsKnown())
		Header.sTitle = pNode->GetSystemName();
	else
		Header.sTitle = CONSTLIT("Unknown");

	//	Compuse import/export data

	if (pNode->IsKnown())
		Header.sDetails = pNode->GetTradingEconomy().GetDescription();

    //  Compose a string indicating when we visited.

    CString sVisit;
    DWORD dwLastVisit = pNode->GetLastVisitedTime();
    if (dwLastVisit == 0xffffffff)
        sVisit = CONSTLIT("You've never visited this system.");
    else if (dwLastVisit == (DWORD)g_pUniverse->GetTicks())
        sVisit = CONSTLIT("You are currently in this system.");
    else
        {
		CTimeSpan Span = g_pUniverse->GetElapsedGameTimeAt(g_pUniverse->GetTicks()) - g_pUniverse->GetElapsedGameTimeAt(dwLastVisit);
        sVisit = strPatternSubst(CONSTLIT("Last visited %s ago."), Span.Format(NULL_STR));
        }

	if (!Header.sDetails.IsBlank())
		Header.sDetails = strPatternSubst(CONSTLIT("%s\n%s"), Header.sDetails, sVisit);
	else
		Header.sDetails = sVisit;

	//	Add debug information, if necessary

	if (g_pUniverse->InDebugMode())
		Header.sDetails = strPatternSubst(CONSTLIT("%s\n%s: %s"), Header.sDetails, pNode->GetID(), pNode->GetAttributes());

#if 0

	//	Attributes

	CCartoucheBlock::SCartoucheDesc *pEntry = Header.Attribs.Insert();
	pEntry->sText = CONSTLIT("Test");
	pEntry->rgbBack = RGB_MODIFIER_NORMAL_BACKGROUND;
	pEntry->rgbColor = RGB_MODIFIER_NORMAL_TEXT;

#endif
	}


//	CUIHelper.cpp
//
//	CUIHelper class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

const int TITLE_BAR_HEIGHT =					128;

const int ATTRIB_PADDING_X =					4;
const int ATTRIB_PADDING_Y =					1;
const int ATTRIB_SPACING_X =					2;
const int ATTRIB_SPACING_Y =					2;

const int BUTTON_HEIGHT =						96;
const int BUTTON_WIDTH =						96;
const int BUTTON_SPACING_X =					10;

const int DAMAGE_ADJ_ICON_WIDTH =				16;
const int DAMAGE_ADJ_ICON_HEIGHT =				16;
const int DAMAGE_ADJ_ICON_SPACING_X =			2;
const int DAMAGE_ADJ_SPACING_X =				6;

const int DAMAGE_TYPE_ICON_WIDTH =				16;
const int DAMAGE_TYPE_ICON_HEIGHT =				16;

const int ICON_CORNER_RADIUS =					8;
const int ICON_HEIGHT =							96;
const int ICON_WIDTH =							96;
const int SMALL_ICON_HEIGHT =					64;
const int SMALL_ICON_WIDTH =					64;

const int INPUT_ERROR_CORNER_RADIUS =			8;
const int INPUT_ERROR_PADDING_BOTTOM =			10;
const int INPUT_ERROR_PADDING_LEFT =			10;
const int INPUT_ERROR_PADDING_RIGHT =			10;
const int INPUT_ERROR_PADDING_TOP =				10;
const int INPUT_ERROR_TIME =					(30 * 10);

const int ITEM_TEXT_MARGIN_Y =					4;
const int ITEM_TEXT_MARGIN_X =					4;
const int ITEM_TEXT_MARGIN_BOTTOM =				10;
const int ITEM_DEFAULT_HEIGHT =					96;
const int ITEM_TITLE_EXTRA_MARGIN =				4;
const int ITEM_LEFT_PADDING =					8;
const int ITEM_RIGHT_PADDING =					8;

const int PADDING_LEFT =						20;

const int RING_SIZE =							8;
const int RING_MIN_RADIUS =						10;
const int RING_COUNT =							4;

const CG32bitPixel RGB_MODIFIER_NORMAL_BACKGROUND =		CG32bitPixel(101,101,101);	//	H:0   S:0   B:40
const CG32bitPixel RGB_MODIFIER_NORMAL_TEXT =			CG32bitPixel(220,220,220);	//	H:0   S:0   B:86

#define ALIGN_RIGHT								CONSTLIT("right")

#define CMD_CLOSE_SESSION						CONSTLIT("cmdCloseSession")
#define CMD_OK_SESSION							CONSTLIT("cmdOKSession")

#define EVENT_ON_CLICK							CONSTLIT("onClick")

#define ID_DLG_INPUT_ERROR						CONSTLIT("uiHelperInputErrorMsg")

#define PROP_COLOR								CONSTLIT("color")
#define PROP_ENABLED							CONSTLIT("enabled")
#define PROP_FILL_TYPE							CONSTLIT("fillType")
#define PROP_FONT								CONSTLIT("font")
#define PROP_LINE_COLOR							CONSTLIT("lineColor")
#define PROP_LINE_PADDING						CONSTLIT("linePadding")
#define PROP_LINE_TYPE							CONSTLIT("lineType")
#define PROP_LINE_WIDTH							CONSTLIT("lineWidth")
#define PROP_LL_RADIUS							CONSTLIT("llRadius")
#define PROP_LR_RADIUS							CONSTLIT("lrRadius")
#define PROP_OPACITY							CONSTLIT("opacity")
#define PROP_POSITION							CONSTLIT("position")
#define PROP_SCALE								CONSTLIT("scale")
#define PROP_TEXT								CONSTLIT("text")
#define PROP_TEXT_ALIGN_HORZ					CONSTLIT("textAlignHorz")
#define PROP_TEXT_ALIGN_VERT					CONSTLIT("textAlignVert")
#define PROP_UL_RADIUS							CONSTLIT("ulRadius")
#define PROP_UR_RADIUS							CONSTLIT("urRadius")

#define STYLE_CHECK								CONSTLIT("check")
#define STYLE_DISABLED							CONSTLIT("disabled")
#define STYLE_DOWN								CONSTLIT("down")
#define STYLE_FRAME								CONSTLIT("frame")
#define STYLE_FRAME_FOCUS						CONSTLIT("frameFocus")
#define STYLE_FRAME_DISABLED					CONSTLIT("frameDisabled")
#define STYLE_HOVER								CONSTLIT("hover")
#define STYLE_IMAGE								CONSTLIT("image")
#define STYLE_NORMAL							CONSTLIT("normal")
#define STYLE_TEXT								CONSTLIT("text")

int CUIHelper::CalcItemEntryHeight (CSpaceObject *pSource, const CItem &Item, const RECT &rcRect, DWORD dwOptions) const

//	CalcItemEntryHeight
//
//	Computes the height necessary to paint the item entry.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &LargeBold = VI.GetFont(fontLargeBold);
	const CG16bitFont &Medium = VI.GetFont(fontMedium);

	bool bNoIcon = ((dwOptions & OPTION_NO_ICON) == OPTION_NO_ICON);
	bool bTitle = ((dwOptions & OPTION_TITLE) == OPTION_TITLE);
	bool bNoPadding = ((dwOptions & OPTION_NO_PADDING) == OPTION_NO_PADDING);
	bool bActual = ((dwOptions & OPTION_KNOWN) == OPTION_KNOWN);

	//	Icons size

	int cxIcon = ((dwOptions & OPTION_SMALL_ICON) ? SMALL_ICON_WIDTH : ICON_WIDTH);
	int cyIcon = ((dwOptions & OPTION_SMALL_ICON) ? SMALL_ICON_HEIGHT : ICON_HEIGHT);

	//	Get the item

	CItemCtx Ctx(&Item, pSource);
	CItemType *pType = Item.GetType();
	if (pType == NULL)
		return ITEM_DEFAULT_HEIGHT;

	//	Compute the rect where the reference text will paint

	RECT rcDrawRect = rcRect;
	if (!bNoPadding)
		{
		rcDrawRect.left += ITEM_TEXT_MARGIN_X;
		rcDrawRect.right -= ITEM_TEXT_MARGIN_X + ITEM_RIGHT_PADDING;
		}

	if (!bNoIcon)
		{
		if (bNoPadding)
			rcDrawRect.left += cxIcon + ITEM_TEXT_MARGIN_X;
		else
			rcDrawRect.left += ITEM_LEFT_PADDING + cxIcon;
		}

	int iLevel = (bActual ? pType->GetLevel(Ctx) : pType->GetApparentLevel(Ctx));

	//	Compute the height of the row

	int cyHeight = 0;

	//	Account for margin

	cyHeight += (bNoPadding ? 0 : ITEM_TEXT_MARGIN_Y);

	//	Item title

	cyHeight += LargeBold.GetHeight();
	if (bTitle)
		cyHeight += ITEM_TITLE_EXTRA_MARGIN;

	//	Attributes

	TArray<SDisplayAttribute> Attribs;
	if (Item.GetDisplayAttributes(Ctx, &Attribs, NULL, bActual))
		{
		int cyAttribs;
		CCartoucheBlock AttribBlock;
		FormatDisplayAttributes(Attribs, rcDrawRect, AttribBlock, &cyAttribs);
		cyHeight += cyAttribs + ATTRIB_SPACING_Y;
		}

	//	Reference

	DWORD dwRefFlags = (bActual ? CItemType::FLAG_ACTUAL_ITEM : 0);
	CString sReference = Item.GetReference(Ctx, CItem(), dwRefFlags);

	//	If this is a weapon, then add room for the weapon damage

	if (Item.GetReferenceDamageType(Ctx, CItem(), dwRefFlags, NULL, NULL))
		cyHeight += Medium.GetHeight();

	//	If this is armor or a shield, then add room for damage resistance

	else if (Item.GetReferenceDamageAdj(pSource, dwRefFlags, NULL, NULL))
		cyHeight += Medium.GetHeight();

	//	Measure the reference text

	int iLines;
	if (!sReference.IsBlank())
		{
		iLines = Medium.BreakText(sReference, RectWidth(rcDrawRect), NULL, 0);
		cyHeight += iLines * Medium.GetHeight();
		}

	//	Measure the description

	CString sDesc = Item.GetDesc(Ctx, bActual);
	iLines = Medium.BreakText(sDesc, RectWidth(rcDrawRect), NULL, 0);
	cyHeight += iLines * Medium.GetHeight();

	//	Margin

	cyHeight += (bNoPadding ? 0 : ITEM_TEXT_MARGIN_BOTTOM);

	if (!bNoIcon)
		cyHeight = Max(cyIcon, cyHeight);

	//	Done

	return cyHeight;
	}

void CUIHelper::CreateBarButtons (CAniSequencer *pSeq, const RECT &rcRect, IHISession *pSession, const TArray<SMenuEntry> *pMenu, DWORD dwOptions) const

//	CreateBarButtons
//
//	Creates buttons for a top or bottom bar. rcRect is the rect where the 
//	buttons should go (relative to pSeq).

	{
	int i;

	//	Nothing to do if no menu

	if (pMenu == NULL)
		return;

	const CVisualPalette &VI = m_HI.GetVisuals();

	//	Count the number of buttons in each part of the menu.

	int iLeftButtons = 0;
	int iCenterButtons = 0;
	int iRightButtons = 0;
	int iMenuLines = 0;

	for (i = 0; i < pMenu->GetCount(); i++)
		{
		const SMenuEntry &Entry = pMenu->GetAt(i);
		if (Entry.dwFlags & MENU_TEXT)
			iMenuLines++;
		else if (Entry.dwFlags & MENU_ALIGN_LEFT)
			iLeftButtons++;
		else if (Entry.dwFlags & MENU_ALIGN_CENTER)
			iCenterButtons++;
		else if (Entry.dwFlags & MENU_ALIGN_RIGHT)
			iRightButtons++;
		else
			iCenterButtons++;
		}

	//	Create text menu

	if (iMenuLines > 0)
		{
		//	Menu is to the left.

		int xMenu = rcRect.left;
		int yMenu = rcRect.top;

		for (i = 0; i < pMenu->GetCount(); i++)
			{
			const SMenuEntry &Entry = pMenu->GetAt(i);
			if (Entry.dwFlags & MENU_TEXT)
				{
				IAnimatron *pButton;
				int cyHeight;

				VI.CreateLink(pSeq, 
						Entry.sCommand, 
						xMenu, 
						yMenu, 
						Entry.sLabel, 
						0, 
						&pButton, 
						NULL, 
						&cyHeight);

				pSession->RegisterPerformanceEvent(pButton, EVENT_ON_CLICK, Entry.sCommand);

				yMenu += cyHeight;
				}
			}
		}

	//	Create left buttons

	else if (iLeftButtons > 0)
		{
		//	Compute the starting position (relative to the bar)

		int xPos = rcRect.left;
		int yPos = rcRect.top;

		//	Create the buttons

		for (i = 0; i < pMenu->GetCount(); i++)
			{
			const SMenuEntry &Entry = pMenu->GetAt(i);
			if (Entry.dwFlags & MENU_ALIGN_LEFT)
				{
				IAnimatron *pButton;
				VI.CreateImageButton(pSeq,
						Entry.sCommand,
						xPos,
						yPos,
						Entry.pIcon, 
						Entry.sLabel,
						0,
						&pButton);

				pSession->RegisterPerformanceEvent(pButton, EVENT_ON_CLICK, Entry.sCommand);

				xPos += BUTTON_WIDTH + BUTTON_SPACING_X;
				}
			}
		}

	//	Create center buttons

	if (iCenterButtons > 0)
		{
		//	Compute the starting position (relative to the bar)

		int cxButtons = (iCenterButtons * BUTTON_WIDTH) + ((iCenterButtons - 1) * BUTTON_SPACING_X);
		int xPos = rcRect.left + (RectWidth(rcRect) - cxButtons) / 2;
		int yPos = rcRect.top;

		//	Create the buttons

		for (i = 0; i < pMenu->GetCount(); i++)
			{
			const SMenuEntry &Entry = pMenu->GetAt(i);
			if ((Entry.dwFlags & MENU_ALIGN_CENTER)
					|| (Entry.dwFlags & (MENU_ALIGN_LEFT | MENU_ALIGN_RIGHT | MENU_TEXT)) == 0)
				{
				IAnimatron *pButton;
				VI.CreateImageButton(pSeq,
						Entry.sCommand,
						xPos,
						yPos,
						Entry.pIcon, 
						Entry.sLabel,
						0,
						&pButton);

				pSession->RegisterPerformanceEvent(pButton, EVENT_ON_CLICK, Entry.sCommand);

				xPos += BUTTON_WIDTH + BUTTON_SPACING_X;
				}
			}
		}

	//	Create right buttons

	if (iRightButtons > 0)
		{
		//	Compute the starting position (relative to the bar)

		int cxButtons = (iCenterButtons * BUTTON_WIDTH) + ((iCenterButtons - 1) * BUTTON_SPACING_X);
		int xPos = rcRect.right - cxButtons;
		int yPos = rcRect.top;

		//	Create the buttons

		for (i = 0; i < pMenu->GetCount(); i++)
			{
			const SMenuEntry &Entry = pMenu->GetAt(i);
			if (Entry.dwFlags & MENU_ALIGN_RIGHT)
				{
				IAnimatron *pButton;
				VI.CreateImageButton(pSeq,
						Entry.sCommand,
						xPos,
						yPos,
						Entry.pIcon, 
						Entry.sLabel,
						0,
						&pButton);

				pSession->RegisterPerformanceEvent(pButton, EVENT_ON_CLICK, Entry.sCommand);

				xPos += BUTTON_WIDTH + BUTTON_SPACING_X;
				}
			}
		}

	}

void CUIHelper::CreateInputErrorMessage (IHISession *pSession, const RECT &rcRect, const CString &sTitle, CString &sDesc, IAnimatron **retpMsg) const

//	CreateInputErrorMessage
//
//	Creates an input error message box

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &TitleFont = VI.GetFont(fontLargeBold);
	const CG16bitFont &DescFont = VI.GetFont(fontMedium);

	//	Start with a sequencer as a parent of everything

	CAniSequencer *pMsg;
	CAniSequencer::Create(CVector(rcRect.left, rcRect.top), &pMsg);

	//	Create a controller to handle dismissing the message

	CInputErrorMessageController *pController = new CInputErrorMessageController(pSession);
	pMsg->AddTrack(pController, 0);

	//	Add a button to handle a click

	CAniButton *pButton = new CAniButton;
	pButton->SetPropertyVector(PROP_POSITION, CVector(0, 0));
	pButton->SetPropertyVector(PROP_SCALE, CVector(RectWidth(rcRect), RectHeight(rcRect)));
	pButton->SetStyle(STYLE_DOWN, NULL);
	pButton->SetStyle(STYLE_HOVER, NULL);
	pButton->SetStyle(STYLE_NORMAL, NULL);
	pButton->SetStyle(STYLE_DISABLED, NULL);
	pButton->SetStyle(STYLE_TEXT, NULL);
	pButton->AddListener(EVENT_ON_CLICK, pController);

	pMsg->AddTrack(pButton, 0);

	//	Figure out where the text goes

	int x = INPUT_ERROR_PADDING_LEFT;
	int cxWidth = RectWidth(rcRect) - (INPUT_ERROR_PADDING_LEFT + INPUT_ERROR_PADDING_RIGHT);
	int y = INPUT_ERROR_PADDING_TOP;
	int yEnd = RectHeight(rcRect) - INPUT_ERROR_PADDING_BOTTOM;
	int cxText = 0;

	//	Title text

	IAnimatron *pTitle = new CAniText;
	pTitle->SetPropertyVector(PROP_POSITION, CVector(x, y));
	pTitle->SetPropertyVector(PROP_SCALE, CVector(cxWidth, TitleFont.GetHeight()));
	((CAniText *)pTitle)->SetPropertyFont(PROP_FONT, &TitleFont);
	pTitle->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextWarningMsg));
	pTitle->SetPropertyString(PROP_TEXT, sTitle);

	y += TitleFont.GetHeight();
	cxText += TitleFont.GetHeight();

	//	Description

	IAnimatron *pDesc = new CAniText;
	pDesc->SetPropertyVector(PROP_POSITION, CVector(x, y));
	pDesc->SetPropertyVector(PROP_SCALE, CVector(cxWidth, 1000));
	((CAniText *)pDesc)->SetPropertyFont(PROP_FONT, &DescFont);
	pDesc->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextWarningMsg));
	pDesc->SetPropertyString(PROP_TEXT, sDesc);

	RECT rcDesc;
	pDesc->GetSpacingRect(&rcDesc);
	cxText += RectHeight(rcDesc);

	//	Now that we know the height of the text, add a rectangle as a background

	int cxFrame = Max(RectHeight(rcRect), cxText + INPUT_ERROR_PADDING_TOP + INPUT_ERROR_PADDING_BOTTOM);

	IAnimatron *pRect = new CAniRoundedRect;
	pRect->SetPropertyVector(PROP_POSITION, CVector());
	pRect->SetPropertyVector(PROP_SCALE, CVector(RectWidth(rcRect), cxFrame));
	pRect->SetPropertyColor(PROP_COLOR, VI.GetColor(colorAreaWarningMsg));
	pRect->SetPropertyOpacity(PROP_OPACITY, 255);
	pRect->SetPropertyInteger(PROP_UL_RADIUS, INPUT_ERROR_CORNER_RADIUS);
	pRect->SetPropertyInteger(PROP_UR_RADIUS, INPUT_ERROR_CORNER_RADIUS);
	pRect->SetPropertyInteger(PROP_LL_RADIUS, INPUT_ERROR_CORNER_RADIUS);
	pRect->SetPropertyInteger(PROP_LR_RADIUS, INPUT_ERROR_CORNER_RADIUS);

	pMsg->AddTrack(pRect, 0);

	//	Add title and desc

	pMsg->AddTrack(pTitle, 0);
	pMsg->AddTrack(pDesc, 0);

	//	Fade after some time

	pMsg->AnimateLinearFade(INPUT_ERROR_TIME, 5, 30);

	//	If we already have an input error box, delete it

	pSession->StopPerformance(ID_DLG_INPUT_ERROR);

	//	Start a new one

	pSession->StartPerformance(pMsg, ID_DLG_INPUT_ERROR, CReanimator::SPR_FLAG_DELETE_WHEN_DONE);

	//	Done

	if (retpMsg)
		*retpMsg = pMsg;
	}

void CUIHelper::CreateSessionFrameBar (IHISession *pSession, const TArray<SMenuEntry> *pMenu, DWORD dwOptions, IAnimatron **retpControl) const

//	CreateSessionFrameBar
//
//	Creates a standard menu bar at the top or bottom of the session.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();

	RECT rcCenter;
	VI.GetWidescreenRect(&rcCenter);

	//	Compute the rect of the entire bar

	RECT rcBar;
	rcBar.left = 0;
	rcBar.right = m_HI.GetScreenWidth();
	if (dwOptions & OPTION_FRAME_ALIGN_TOP)
		{
		rcBar.top = 0;
		rcBar.bottom = rcCenter.top;
		}
	else
		{
		rcBar.top = rcCenter.bottom;
		rcBar.bottom = m_HI.GetScreenHeight();
		}

	//	Create a sequencer to hold the bar

	CAniSequencer *pRoot;
	CAniSequencer::Create(CVector(rcBar.left, rcBar.top), &pRoot);

	//	Rectangle for the entire bar background

	IAnimatron *pItem = new CAniRect;
	pItem->SetPropertyVector(PROP_POSITION, CVector(0, 0));
	pItem->SetPropertyVector(PROP_SCALE, CVector(RectWidth(rcBar), RectHeight(rcBar)));
	pItem->SetPropertyColor(PROP_COLOR, VI.GetColor(colorAreaDeep));
	pItem->SetFillMethod(new CAniSolidFill);

	pRoot->AddTrack(pItem, 0);

	//	Rectangle for edge

	pItem = new CAniRect;
	if (dwOptions & OPTION_FRAME_ALIGN_TOP)
		pItem->SetPropertyVector(PROP_POSITION, CVector(0, RectHeight(rcBar) - 1));
	else
		pItem->SetPropertyVector(PROP_POSITION, CVector(0, 0));
	pItem->SetPropertyVector(PROP_SCALE, CVector(RectWidth(rcBar), 1));
	pItem->SetPropertyColor(PROP_COLOR, VI.GetColor(colorLineFrame));
	pItem->SetFillMethod(new CAniSolidFill);

	pRoot->AddTrack(pItem, 0);

	//	Figure out where the buttons should go

	RECT rcButtons;
	rcButtons.left = rcCenter.left;
	rcButtons.right = rcCenter.right;
	if (dwOptions & OPTION_FRAME_ALIGN_TOP)
		rcButtons.top = rcBar.bottom - TITLE_BAR_HEIGHT + ((TITLE_BAR_HEIGHT - BUTTON_HEIGHT) / 2);
	else
		rcButtons.top = (TITLE_BAR_HEIGHT - BUTTON_HEIGHT) / 2;
	rcButtons.bottom = rcBar.bottom;

	//	Create the buttons

	CreateBarButtons(pRoot, rcButtons, pSession, pMenu, dwOptions);

	//	Done

	*retpControl = pRoot;
	}

void CUIHelper::CreateSessionTitle (IHISession *pSession, 
									CCloudService &Service, 
									const CString &sTitle, 
									const TArray<SMenuEntry> *pMenu,
									DWORD dwOptions, 
									IAnimatron **retpControl) const

//	CreateSessionTitle
//
//	Creates a session title bar, including:
//
//	User icon
//	User name
//	Session title
//	Close button

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &SubTitleFont = VI.GetFont(fontSubTitle);

	RECT rcRect;
	VI.GetWidescreenRect(&rcRect);

	//	Create a sequencer to hold all the controls

	CAniSequencer *pRoot;
	CAniSequencer::Create(CVector(rcRect.left, rcRect.top - TITLE_BAR_HEIGHT), &pRoot);

	//	Add the header, unless excluded

	if (!(dwOptions & OPTION_SESSION_NO_HEADER))
		{
		//	The user icon is centered

		CAniRoundedRect *pIcon = new CAniRoundedRect;
		pIcon->SetPropertyVector(PROP_POSITION, CVector(0, (TITLE_BAR_HEIGHT - ICON_HEIGHT) / 2));
		pIcon->SetPropertyVector(PROP_SCALE, CVector(ICON_HEIGHT, ICON_WIDTH));
		pIcon->SetPropertyColor(PROP_COLOR, CG32bitPixel(128, 128, 128));
		pIcon->SetPropertyOpacity(PROP_OPACITY, 255);
		pIcon->SetPropertyInteger(PROP_UL_RADIUS, ICON_CORNER_RADIUS);
		pIcon->SetPropertyInteger(PROP_UR_RADIUS, ICON_CORNER_RADIUS);
		pIcon->SetPropertyInteger(PROP_LL_RADIUS, ICON_CORNER_RADIUS);
		pIcon->SetPropertyInteger(PROP_LR_RADIUS, ICON_CORNER_RADIUS);

		pRoot->AddTrack(pIcon, 0);

		//	The user name baseline is centered.

		CString sUsername;
		CG32bitPixel rgbUsernameColor;
		if (Service.HasCapability(ICIService::canGetUserProfile))
			{
			sUsername = Service.GetUsername();
			rgbUsernameColor = VI.GetColor(colorTextDialogInput);
			}
		else
			{
			sUsername = CONSTLIT("Offline");
			rgbUsernameColor = VI.GetColor(colorTextDialogLabel);
			}

		int y = (TITLE_BAR_HEIGHT / 2) - SubTitleFont.GetAscent();

		IAnimatron *pName = new CAniText;
		pName->SetPropertyVector(PROP_POSITION, CVector(ICON_WIDTH + PADDING_LEFT, y));
		pName->SetPropertyVector(PROP_SCALE, CVector(RectWidth(rcRect), RectHeight(rcRect)));
		pName->SetPropertyColor(PROP_COLOR, rgbUsernameColor);
		pName->SetPropertyFont(PROP_FONT, &SubTitleFont);
		pName->SetPropertyString(PROP_TEXT, sUsername);

		pRoot->AddTrack(pName, 0);
		y += SubTitleFont.GetHeight();

		//	Add the session title

		IAnimatron *pTitle = new CAniText;
		pTitle->SetPropertyVector(PROP_POSITION, CVector(ICON_WIDTH + PADDING_LEFT, y));
		pTitle->SetPropertyVector(PROP_SCALE, CVector(RectWidth(rcRect), RectHeight(rcRect)));
		pTitle->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextDialogTitle));
		pTitle->SetPropertyFont(PROP_FONT, &SubTitleFont);
		pTitle->SetPropertyString(PROP_TEXT, sTitle);

		pRoot->AddTrack(pTitle, 0);
		}

	//	Add command buttons at the bottom

	int yBottomBar = TITLE_BAR_HEIGHT + RectHeight(rcRect);

	//	Add a close button.

	if (!(dwOptions & OPTION_SESSION_NO_CANCEL_BUTTON))
		{
		//	If we have an OK button, then the label is Cancel.

		CString sCloseLabel = ((dwOptions & OPTION_SESSION_OK_BUTTON) ? CONSTLIT("Cancel") : CONSTLIT("Close"));
		const CG32bitImage &CloseIcon = VI.GetImage(imageCloseIcon);

		IAnimatron *pCloseButton;
		VI.CreateImageButton(pRoot, CMD_CLOSE_SESSION, RectWidth(rcRect) - BUTTON_WIDTH, yBottomBar + (TITLE_BAR_HEIGHT - BUTTON_HEIGHT) / 2, &CloseIcon, sCloseLabel, 0, &pCloseButton);

		pSession->RegisterPerformanceEvent(pCloseButton, EVENT_ON_CLICK, CMD_CLOSE_SESSION);
		}

	//	Add an OK button, if necessary

	if (dwOptions & OPTION_SESSION_OK_BUTTON)
		{
		const CG32bitImage &OKIcon = VI.GetImage(imageOKIcon);

		IAnimatron *pOKButton;
		VI.CreateImageButton(pRoot, CMD_OK_SESSION, (RectWidth(rcRect) - BUTTON_WIDTH) / 2, yBottomBar + (TITLE_BAR_HEIGHT - BUTTON_HEIGHT) / 2, &OKIcon, CONSTLIT("OK"), 0, &pOKButton);

		pSession->RegisterPerformanceEvent(pOKButton, EVENT_ON_CLICK, CMD_OK_SESSION);
		}

	//	Add extensions button

	else if (dwOptions & OPTION_SESSION_ADD_EXTENSION_BUTTON)
		{
		const CG32bitImage &OKIcon = VI.GetImage(imageSlotIcon);

		IAnimatron *pOKButton;
		VI.CreateImageButton(pRoot, CMD_OK_SESSION, (RectWidth(rcRect) - BUTTON_WIDTH) / 2, yBottomBar + (TITLE_BAR_HEIGHT - BUTTON_HEIGHT) / 2, &OKIcon, CONSTLIT("Add Extension"), 0, &pOKButton);

		pSession->RegisterPerformanceEvent(pOKButton, EVENT_ON_CLICK, CMD_OK_SESSION);
		}

	//	Add menu items, if necessary

	if (pMenu)
		RefreshMenu(pSession, pRoot, *pMenu);

	//	Done

	*retpControl = pRoot;
	}

void CUIHelper::CreateSessionWaitAnimation (const CString &sID, const CString &sText, IAnimatron **retpControl) const

//	CreateSessionWaitAnimation
//
//	Creates a small wait animation at the lower-left of a session screen.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();

	//	Compute some metrics

	RECT rcRect;
	VI.GetWidescreenRect(&rcRect);

	//	Figure out the position of the ring animation

	int xCenter = rcRect.left + (TITLE_BAR_HEIGHT / 2);
	int yCenter = rcRect.bottom + (TITLE_BAR_HEIGHT / 2);

	//	Create a sequencer to hold all the animations

	CAniSequencer *pRoot;
	CAniSequencer::Create(CVector(xCenter, yCenter), &pRoot);

	//	Create rings of increasing diameter

	VI.CreateRingAnimation(pRoot, RING_COUNT, RING_MIN_RADIUS, RING_SIZE);

	//	Add some text

	if (!sText.IsBlank())
		{
		const CG16bitFont &SubTitleFont = VI.GetFont(fontSubTitle);

		int xText = 8 + (TITLE_BAR_HEIGHT / 2);
		int yText = -(SubTitleFont.GetHeight() / 2);

		IAnimatron *pName = new CAniText;
		pName->SetPropertyVector(PROP_POSITION, CVector(xText, yText));
		pName->SetPropertyVector(PROP_SCALE, CVector(RectWidth(rcRect), SubTitleFont.GetHeight()));
		pName->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextFade));
		pName->SetPropertyFont(PROP_FONT, &SubTitleFont);
		pName->SetPropertyString(PROP_TEXT, sText);

		pRoot->AddTrack(pName, 0);
		}

	//	Done

	if (retpControl)
		*retpControl = pRoot;
	}

void CUIHelper::FormatDisplayAttributes (TArray<SDisplayAttribute> &Attribs, const RECT &rcRect, CCartoucheBlock &retBlock, int *retcyHeight) const

//	FormatDisplayAttributes
//
//	Initializes the rcRect structure in all attribute entries.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &Medium = VI.GetFont(fontMedium);

	for (int i = 0; i < Attribs.GetCount(); i++)
		{
		CG32bitPixel rgbBackColor;
		CG32bitPixel rgbTextColor;

		//	Figure out the colors

		switch (Attribs[i].iType)
			{
			case attribPositive:
			case attribEnhancement:
				rgbBackColor = VI.GetColor(colorAreaAdvantage);
				rgbTextColor = VI.GetColor(colorTextAdvantage);
				break;

			case attribNegative:
			case attribWeakness:
				rgbBackColor = VI.GetColor(colorAreaDisadvantage);
				rgbTextColor = VI.GetColor(colorTextDisadvantage);
				break;

			default:
				rgbBackColor = RGB_MODIFIER_NORMAL_BACKGROUND;
				rgbTextColor = RGB_MODIFIER_NORMAL_TEXT;
				break;
			}

		//	Add to block

		retBlock.AddCartouche(Attribs[i].sText, rgbTextColor, rgbBackColor);
		}

	//	Format

	retBlock.SetFont(&Medium);
	retBlock.Format(RectWidth(rcRect));

	//	Done

	if (retcyHeight)
		*retcyHeight = RectHeight(retBlock.GetBounds());
	}

void CUIHelper::GenerateDockScreenRTF (const CString &sText, CString *retsRTF) const

//	GenerateDockScreenRTF
//
//	Converts from plain text to RTF.

	{
	CRTFText::SAutoRTFOptions Options;
	Options.rgbQuoteText = g_pHI->GetVisuals().GetColor(colorTextQuote);

	*retsRTF = CRTFText::GenerateRTFText(sText, Options);
	}

void CUIHelper::PaintDisplayAttribs (CG32bitImage &Dest, int x, int y, const TArray<SDisplayAttribute> &Attribs, DWORD dwOptions) const

//	PaintDisplayAttribs
//
//	Paint item display attributes.

	{
	static constexpr int PADDING_X = 2;
	static constexpr int PADDING_Y = 1;

	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &Font = VI.GetFont(fontSmall);

	const int MARGIN_X = ((dwOptions & OPTION_NO_MARGIN) ? 0 : 2);
	const int MARGIN_Y = ((dwOptions & OPTION_NO_MARGIN) ? 0 : 1);
	const int BOX_HEIGHT = Font.GetHeight() + (2 * PADDING_Y);

	//	If we're aligned at the bottom, then the y position is the position of
	//	the bottom of the rect.

	int yOffset = 0;
	if (dwOptions & OPTION_ALIGN_BOTTOM)
		yOffset = -BOX_HEIGHT;

	//	Paint attributes vertically.

	if (dwOptions & OPTION_VERTICAL)
		{
		int yPos = y;

		for (int i = 0; i < Attribs.GetCount(); i++)
			{
			const CString &sMods = Attribs[i].sText;
			bool bDisadvantage = (Attribs[i].iType == attribWeakness);

			int cx = Font.MeasureText(sMods);
			int cxBox = cx + 2 * PADDING_X;
			int xPos = x;
			if (dwOptions & OPTION_ALIGN_RIGHT)
				xPos -= cxBox;

			Dest.Fill(xPos,
				yPos,
				cxBox,
				BOX_HEIGHT,
				(bDisadvantage ? VI.GetColor(colorAreaDisadvantage) : VI.GetColor(colorAreaAdvantage)));

			Font.DrawText(Dest,
				xPos + PADDING_X,
				yPos + PADDING_Y,
				(bDisadvantage ? VI.GetColor(colorTextDisadvantage) : VI.GetColor(colorTextAdvantage)),
				sMods);

			yPos += BOX_HEIGHT + MARGIN_Y;
			}
		}

	//	Paint attributes on a single line, aligned to the right.

	else if (dwOptions & OPTION_ALIGN_RIGHT)
		{
		int xPos = x;
		int yPos = y + yOffset;

		for (int i = Attribs.GetCount() - 1; i >= 0; i--)
			{
			const CString &sMods = Attribs[i].sText;
			bool bDisadvantage = (Attribs[i].iType == attribWeakness);

			int cx = Font.MeasureText(sMods);
			int cxBox = cx + 2 * PADDING_X;
			Dest.Fill(xPos - cxBox,
				yPos,
				cxBox,
				BOX_HEIGHT,
				(bDisadvantage ? VI.GetColor(colorAreaDisadvantage) : VI.GetColor(colorAreaAdvantage)));

			Font.DrawText(Dest,
				xPos - cxBox + PADDING_X,
				yPos + PADDING_Y,
				(bDisadvantage ? VI.GetColor(colorTextDisadvantage) : VI.GetColor(colorTextAdvantage)),
				sMods);

			xPos -= cxBox + (MARGIN_X);
			}
		}

	//	Otherwise, paint on a single line, aligned to the left or to the center

	else
		{
		//	Figure out where we start painting, depending on whether we are left-
		//	aligned or centered.

		int xPos;
		if (dwOptions & OPTION_ALIGN_CENTER)
			{
			//	Measure all attributes.

			int cxTotal = 0;
			TArray<int> cx;
			cx.InsertEmpty(Attribs.GetCount());
			for (int i = 0; i < Attribs.GetCount(); i++)
				{
				cx[i] = Font.MeasureText(Attribs[i].sText);
				cxTotal += cx[i];
				}

			cxTotal += MARGIN_X * (Attribs.GetCount() - 1);

			xPos = x - (cxTotal / 2);
			}
		else
			xPos = x;

		int yPos = y + yOffset;

		//	Paint all attributes on the line.

		for (int i = 0; i < Attribs.GetCount(); i++)
			{
			const CString &sMods = Attribs[i].sText;
			bool bDisadvantage = (Attribs[i].iType == attribWeakness);

			int cx = Font.MeasureText(sMods);
			int cxBox = cx + 2 * PADDING_X;
			Dest.Fill(xPos,
				yPos,
				cxBox,
				BOX_HEIGHT,
				(bDisadvantage ? VI.GetColor(colorAreaDisadvantage) : VI.GetColor(colorAreaAdvantage)));

			Font.DrawText(Dest,
				xPos + PADDING_X,
				yPos + PADDING_Y,
				(bDisadvantage ? VI.GetColor(colorTextDisadvantage) : VI.GetColor(colorTextAdvantage)),
				sMods);

			xPos += cxBox + (MARGIN_X);
			}
		}
	}

void CUIHelper::PaintItemEntry (CG32bitImage &Dest, CSpaceObject *pSource, const CItem &Item, const RECT &rcRect, CG32bitPixel rgbText, DWORD dwOptions) const

//	PaintItemEntry
//
//	Paints an item entry suitable for an item list.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &LargeBold = VI.GetFont(fontLargeBold);
	const CG16bitFont &Medium = VI.GetFont(fontMedium);
    CG32bitPixel rgbColorTitle = rgbText;
    CG32bitPixel rgbColorRef = rgbText;
	CG32bitPixel rgbColorDescSel = CG32bitPixel(200,200,200);
	CG32bitPixel rgbColorDesc = CG32bitPixel(128,128,128);
	CG32bitPixel rgbDisadvantage = VI.GetColor(colorTextDisadvantage);

	bool bSelected = ((dwOptions & OPTION_SELECTED) == OPTION_SELECTED);
	bool bNoIcon = ((dwOptions & OPTION_NO_ICON) == OPTION_NO_ICON);
	bool bTitle = ((dwOptions & OPTION_TITLE) == OPTION_TITLE);
	bool bNoPadding = ((dwOptions & OPTION_NO_PADDING) == OPTION_NO_PADDING);
	bool bNoArmorSpeedDisplay = ((dwOptions & OPTION_NO_ARMOR_SPEED_DISPLAY) == OPTION_NO_ARMOR_SPEED_DISPLAY);
	bool bActual = ((dwOptions & OPTION_KNOWN) == OPTION_KNOWN);

	//	Icons size

	int cxIcon = ((dwOptions & OPTION_SMALL_ICON) ? SMALL_ICON_WIDTH : ICON_WIDTH);
	int cyIcon = ((dwOptions & OPTION_SMALL_ICON) ? SMALL_ICON_HEIGHT : ICON_HEIGHT);

	//	Item context

	CItemCtx Ctx(&Item, pSource);
	CItemType *pItemType = Item.GetType();

	//	Calc the rect where we will draw

	RECT rcDrawRect = rcRect;
	if (!bNoPadding)
		{
		rcDrawRect.left += ITEM_TEXT_MARGIN_X;
		rcDrawRect.right -= ITEM_TEXT_MARGIN_X + ITEM_RIGHT_PADDING;
		rcDrawRect.top += ITEM_TEXT_MARGIN_Y;
		}

	//	Paint the image

	if (!bNoIcon)
		{
		int xIcon = (bNoPadding ? rcRect.left : rcRect.left + ITEM_LEFT_PADDING);
		DrawItemTypeIcon(Dest, xIcon, rcRect.top, pItemType, cxIcon, cyIcon);
		rcDrawRect.left = xIcon + cxIcon + ITEM_TEXT_MARGIN_X;
		}

	//	Paint the item name

	DWORD dwNounPhraseFlags = nounNoModifiers;
	if (bTitle)
		dwNounPhraseFlags |= nounTitleCapitalize | nounShort;
	else
		dwNounPhraseFlags |= nounCount;
	if (bActual)
		dwNounPhraseFlags |= nounActual;

	int cyHeight;
	RECT rcTitle = rcDrawRect;
	LargeBold.DrawText(Dest,
			rcTitle,
			rgbColorTitle,
			Item.GetNounPhrase(Ctx, dwNounPhraseFlags),
			0,
			CG16bitFont::SmartQuotes | CG16bitFont::TruncateLine,
			&cyHeight);

	rcDrawRect.top += cyHeight;
	if (bTitle)
		rcDrawRect.top += ITEM_TITLE_EXTRA_MARGIN;

	//	Paint the display attributes

	TArray<SDisplayAttribute> Attribs;
	if (Item.GetDisplayAttributes(Ctx, &Attribs, NULL, bActual))
		{
		CCartoucheBlock AttribBlock;
		FormatDisplayAttributes(Attribs, rcDrawRect, AttribBlock, &cyHeight);
		AttribBlock.Paint(Dest, rcDrawRect.left, rcDrawRect.top);
		rcDrawRect.top += cyHeight + ATTRIB_SPACING_Y;
		}

	//	Stats

	CString sStat;
	DWORD dwRefFlags = (bActual ? CItemType::FLAG_ACTUAL_ITEM : 0);

	int iLevel = (bActual ? pItemType->GetLevel(Ctx) : pItemType->GetApparentLevel(Ctx));
	CString sReference = Item.GetReference(Ctx, CItem(), dwRefFlags);
	DamageTypes iDamageType;
	CString sDamageRef;
	int iDamageAdj[damageCount];
	int iHP;

	if (Item.GetReferenceDamageType(Ctx, CItem(), dwRefFlags, &iDamageType, &sDamageRef))
		{
		//	Paint the damage type reference

		PaintReferenceDamageType(Dest,
				rcDrawRect.left,
				rcDrawRect.top,
				iDamageType,
				sDamageRef,
                rgbText);

		rcDrawRect.top += Medium.GetHeight();

		//	Paint additional reference in the line below

		if (!sReference.IsBlank())
			{
			Medium.DrawText(Dest, 
					rcDrawRect,
					rgbColorRef,
					sReference,
					0,
					0,
					&cyHeight);

			rcDrawRect.top += cyHeight;
			}
		}
	else if (Item.GetReferenceDamageAdj(pSource, dwRefFlags, &iHP, iDamageAdj))
		{
		//	Paint the initial text

		sStat = strPatternSubst("hp: %d ", iHP);
		int cxWidth = Medium.MeasureText(sStat, &cyHeight);
		Medium.DrawText(Dest, 
				rcDrawRect,
				rgbColorRef,
				sStat,
				0,
				0,
				&cyHeight);

		//	Paint the damage type array

		PaintReferenceDamageAdj(Dest,
				rcDrawRect.left + cxWidth + DAMAGE_ADJ_SPACING_X,
				rcDrawRect.top,
				iLevel,
				iHP,
				iDamageAdj,
                rgbText);

		rcDrawRect.top += cyHeight;

		//	If we have a speed bonus/penalty for armor, then show it.
		//	Usually we show penalty relative to the player.

		CSpaceObject *pBonusSource = g_pUniverse->GetPlayerShip();
		CItemCtx BonusCtx(&Item, pBonusSource);

		int iSpeedBonus;
		if (!bNoArmorSpeedDisplay && Item.GetReferenceSpeedBonus(BonusCtx, dwRefFlags, &iSpeedBonus))
			{
			int cx = rcDrawRect.left;
			sReference = strPatternSubst(CONSTLIT("%s — "), sReference);
			cx += Medium.MeasureText(sReference);

			Medium.DrawText(Dest,
					rcDrawRect,
					rgbColorRef,
					sReference,
					0,
					0,
					&cyHeight);

			if (iSpeedBonus == 0)
				Medium.DrawText(Dest, cx, rcDrawRect.top, rgbDisadvantage, CONSTLIT("too heavy"), iSpeedBonus);
			else if (iSpeedBonus > 0)
				Medium.DrawText(Dest, cx, rcDrawRect.top, rgbColorRef, strPatternSubst(CONSTLIT("+.%02dc bonus"), iSpeedBonus));
			else
				Medium.DrawText(Dest, cx, rcDrawRect.top, rgbDisadvantage, strPatternSubst(CONSTLIT("-.%02dc penalty"), -iSpeedBonus));

			rcDrawRect.top += cyHeight;
			}

		//	Paint additional reference in the line below

		else if (!sReference.IsBlank())
			{
			Medium.DrawText(Dest, 
					rcDrawRect,
					rgbColorRef,
					sReference,
					0,
					0,
					&cyHeight);

			rcDrawRect.top += cyHeight;
			}
		}
	else
		{
		Medium.DrawText(Dest, 
				rcDrawRect,
				rgbColorRef,
				sReference,
				0,
				0,
				&cyHeight);

		rcDrawRect.top += cyHeight;
		}

	//	Description

	CString sDesc = Item.GetDesc(Ctx, bActual);
	Medium.DrawText(Dest,
			rcDrawRect,
			(bSelected ? rgbColorDescSel : rgbColorDesc),
			sDesc,
			0,
			CG16bitFont::SmartQuotes,
			&cyHeight);
	rcDrawRect.top += cyHeight;
	}

void CUIHelper::PaintReferenceDamageAdj (CG32bitImage &Dest, int x, int y, int iLevel, int iHP, const int *iDamageAdj, CG32bitPixel rgbText) const

//	PaintReferenceDamageAdj
//
//	Takes an array of damage type adj values and displays them

	{
	struct SEntry
		{
		int iDamageType;
		int iDamageAdj;
		};

	int i;
	bool bSortByDamageType = true;
	bool bOptionShowDamageAdjAsHP = false;

	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &Small = VI.GetFont(fontSmall);
	const CG16bitFont &Medium = VI.GetFont(fontMedium);

	//	Must have positive HP

	if (iHP == 0)
		return;

	//	Sort damage types from highest to lowest

	TSortMap<CString, SEntry> Sorted;
	int iLengthEstimate = 0;
	int iImmuneCount = 0;
	for (i = 0; i < damageCount; i++)
		{
		//	Skip if this damage type is not appropriate to our level

		int iDamageLevel = GetDamageTypeLevel((DamageTypes)i);
		if (iDamageLevel < iLevel - 5 || iDamageLevel > iLevel + 3)
			continue;

		//	Skip if the damage adj is 100%

		if (iDamageAdj[i] == iHP)
			continue;

		//	Figure out the sort order

		CString sKey;
		if (bSortByDamageType)
			sKey = strPatternSubst(CONSTLIT("%02d"), i);
		else
			{
			DWORD dwHighToLow = (iDamageAdj[i] == -1 ? 0 : 1000000 - iDamageAdj[i]);
			sKey = strPatternSubst(CONSTLIT("%08x %02d"), dwHighToLow, i);
			}

		//	Add to list

		SEntry *pEntry = Sorted.SetAt(sKey);
		pEntry->iDamageType = i;
		pEntry->iDamageAdj = iDamageAdj[i];

		//	Estimate how many entries we will have (so we can decide the font size)
		//	We assume that immune entries get collapsed.

		if (iDamageAdj[i] != -1)
			iLengthEstimate++;
		else
			iImmuneCount++;
		}

	//	If we have six or more icons, then we need to paint smaller

	iLengthEstimate += Min(2, iImmuneCount);
	const CG16bitFont &TheFont = (iLengthEstimate >= 6 ? Small : Medium);
	int cyOffset = (Medium.GetHeight() - TheFont.GetHeight()) / 2;
	
	//	Paint the icons

	for (i = 0; i < Sorted.GetCount(); i++)
		{
		const SEntry &Entry = Sorted[i];
		int iDamageType = Entry.iDamageType;
		int iDamageAdj = Entry.iDamageAdj;
		int iPercentAdj = (100 * (iDamageAdj - iHP) / iHP);

		//	Prettify the % by rounding to a number divisible by 5

		int iPrettyPercent = 5 * ((iPercentAdj + 2 * Sign(iPercentAdj)) / 5);

		//	Skip if prettify results in 0%

		if (bOptionShowDamageAdjAsHP && iPrettyPercent == 0)
			continue;

		//	Draw icon

		g_pHI->GetVisuals().DrawDamageTypeIcon(Dest, x, y, (DamageTypes)iDamageType);
		x += DAMAGE_TYPE_ICON_WIDTH + DAMAGE_ADJ_ICON_SPACING_X;

		//	If we have a bunch of entries with "immune", then compress them

		if (i < (Sorted.GetCount() - 1)
				&& iDamageAdj == -1
				&& (iDamageAdj == Sorted[i + 1].iDamageAdj))
			continue;

		//	Figure out how to display damage adj

		CString sStat;
		if (iDamageAdj == -1)
			sStat = CONSTLIT("immune");
		else if (bOptionShowDamageAdjAsHP)
			sStat = strFromInt(iDamageAdj);
		else
			sStat = strPatternSubst(CONSTLIT("%s%d%%"), (iPrettyPercent > 0 ? CONSTLIT("+") : NULL_STR), iPrettyPercent);
		
		//	Draw

		Dest.DrawText(x,
				y + cyOffset,
				TheFont,
				rgbText,
				sStat,
				0,
				&x);

		x += DAMAGE_ADJ_SPACING_X;
		}
	}

void CUIHelper::PaintReferenceDamageType (CG32bitImage &Dest, int x, int y, int iDamageType, const CString &sRef, CG32bitPixel rgbText) const

//	PaintReferenceDamageType
//
//	Draws a single damage type icon (if not damageError) followed by reference text

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &Medium = VI.GetFont(fontMedium);

	//	Paint the icon first

	if (iDamageType != damageError
			&& iDamageType != damageGeneric)
		{
		//x += DAMAGE_ADJ_SPACING_X;
		g_pHI->GetVisuals().DrawDamageTypeIcon(Dest, x, y, (DamageTypes)iDamageType);
		x += DAMAGE_TYPE_ICON_WIDTH + DAMAGE_ADJ_ICON_SPACING_X;
		}

	//	Paint the reference text

	Dest.DrawText(x,
			y,
			Medium,
			rgbText,
			sRef,
			0);
	}

void CUIHelper::RefreshMenu (IHISession *pSession, IAnimatron *pRoot, const TArray<SMenuEntry> &Menu) const

//	RefreshMenu
//
//	Refreshes a menu created with CreateSessionTitle. NOTE: All menu entries 
//	be represented (though they may be hidden).

	{
	const CVisualPalette &VI = m_HI.GetVisuals();

	ASSERT(pRoot);
	if (pRoot == NULL)
		return;

	RECT rcRect;
	VI.GetWidescreenRect(&rcRect);

	//	Delete all existing elements (if any)

	for (int i = 0; i < Menu.GetCount(); i++)
		pSession->HIGetReanimator().DeleteElement(Menu[i].sCommand);

	//	Add command buttons at the bottom

	int yBottomBar = TITLE_BAR_HEIGHT + RectHeight(rcRect);

	//	Menu is to the left.

	int xMenu = 0;
	int yMenu = yBottomBar + (TITLE_BAR_HEIGHT - BUTTON_HEIGHT) / 2;

	//	Add menus

	for (int i = 0; i < Menu.GetCount(); i++)
		{
		const SMenuEntry &Entry = Menu.GetAt(i);

		//	If this entry is hidden, then skip

		if (Entry.dwFlags & MENU_HIDDEN)
			continue;

		//	Create

		IAnimatron *pButton;
		int cyHeight;
		VI.CreateLink(pRoot, Entry.sCommand, xMenu, yMenu, Entry.sLabel, 0, &pButton, NULL, &cyHeight);
		yMenu += cyHeight;

		//	Disable if necessary

		if (Entry.dwFlags & MENU_DISABLED)
			pButton->SetPropertyBool(PROP_ENABLED, false);

		//	Register for command

		pSession->RegisterPerformanceEvent(pButton, EVENT_ON_CLICK, Entry.sCommand);
		}
	}

int CUIHelper::ScrollAnimationDecay (int iOffset)

//	ScrollAnimationDecay
//
//	To implement smooth scrolling, we decay the offset.

	{
	int iDelta;
	if (iOffset > 0)
		{
		iDelta = Max(12, iOffset / 4);
		iOffset = Max(0, iOffset - iDelta);
		}
	else
		{
		iDelta = Min(-12, iOffset / 4);
		iOffset = Min(0, iOffset - iDelta);
		}

	return iOffset;
	}

//	PaintItemEntry
void CInputErrorMessageController::OnAniCommand (const CString &sID, const CString &sEvent, const CString &sCmd, DWORD dwData)

//	OnAniCommand
//
//	Handles a command

	{
	m_pSession->StopPerformance(ID_DLG_INPUT_ERROR);
	}

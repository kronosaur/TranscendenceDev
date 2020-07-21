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
#define PROP_IGNORE_DOUBLE_CLICK				CONSTLIT("ignoreDoubleClick")
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

CG32bitImage CUIHelper::CreateGlowBackground (void) const

//	CreateGlowBackground
//
//	Creates a standard background.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();

	RECT rcRect;
	VI.GetWidescreenRect(NULL, &rcRect);

	CG32bitPixel rgbBack = VI.GetColor(colorAreaDialog);
	CG32bitPixel rgbCenter = CG32bitPixel::Blend(rgbBack, CG32bitPixel(255, 255, 255), 0.1);

	return CreateGlowBackground(RectWidth(rcRect), RectHeight(rcRect), rgbCenter, rgbBack);
	}

CG32bitImage CUIHelper::CreateGlowBackground (int cxWidth, int cyHeight, CG32bitPixel rgbCenter, CG32bitPixel rgbEdge) const

//	CreateGlowBackground
//
//	Creates a background image with a circular gradient.

	{
	CG32bitImage Image;
	if (!Image.Create(cxWidth, cyHeight, CG32bitImage::alphaNone, rgbEdge))
		throw CException(ERR_MEMORY);

	int xCenter = cxWidth / 2;
	int yCenter = cyHeight / 2;
	int iRadius = cxWidth / 2;

	CGDraw::CircleGradient(Image, xCenter, yCenter, iRadius, rgbCenter);

	return Image;
	}

void CUIHelper::CreateInputErrorMessage (IHISession *pSession, const RECT &rcRect, const CString &sTitle, const CString &sDesc, IAnimatron **retpMsg) const

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

void CUIHelper::PaintRTFText (const CVisualPalette &VI, CG32bitImage &Dest, const RECT &rcRect, const CString &sRTFText, const CG16bitFont &DefaultFont, CG32bitPixel rgbDefaultColor, int *retcyHeight)

//	PaintRTFText
//
//	Paints a block of RTF text. This function is slightly slower then using 
//	CTextBlock directly, but sometimes this is more convenient.

	{
	//	Short-circuit.

	if (sRTFText.IsBlank())
		{ }

	//	If not RTF text, then we just use the normal painting

	else if (!CTextBlock::IsRTFText(sRTFText))
		{
		DefaultFont.DrawText(Dest,
				rcRect,
				rgbDefaultColor,
				sRTFText,
				0,
				0,
				retcyHeight
				);
		}

	//	Otherwise, paint RTF text

	else
		{
		CTextBlock RTF;

		//	Format the text

		SBlockFormatDesc BlockFormat;
		BlockFormat.cxWidth = RectWidth(rcRect);
		BlockFormat.cyHeight = -1;
		BlockFormat.iHorzAlign = alignLeft;
		BlockFormat.iVertAlign = alignTop;
		BlockFormat.iExtraLineSpacing = 0;

		BlockFormat.DefaultFormat.rgbColor = rgbDefaultColor;
		BlockFormat.DefaultFormat.pFont = &DefaultFont;

		CString sError;
		if (!RTF.InitFromRTF(sRTFText, VI, BlockFormat, &sError))
			{
			PaintRTFText(VI, Dest, rcRect, sError, DefaultFont, rgbDefaultColor, retcyHeight);
			return;
			}

		//	Paint

		RTF.Paint(Dest, rcRect.left, rcRect.top);

		//	Compute bounds

		if (retcyHeight)
			{
			RECT rcBounds;
			RTF.GetBounds(&rcBounds);
			*retcyHeight = RectHeight(rcBounds);
			}
		}
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

		if (Entry.dwFlags & MENU_IGNORE_DOUBLE_CLICK)
			pButton->SetPropertyBool(PROP_IGNORE_DOUBLE_CLICK, true);

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

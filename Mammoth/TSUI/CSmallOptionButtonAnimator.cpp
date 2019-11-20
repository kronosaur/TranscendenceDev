//	CSmallOptionButtonAnimator.cpp
//
//	CSmallOptionButtonAnimator class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ALIGN_RIGHT								CONSTLIT("right")

#define EVENT_ON_CLICK							CONSTLIT("onClick")

#define PROP_COLOR								CONSTLIT("color")
#define PROP_ENABLED							CONSTLIT("enabled")
#define PROP_FADE_EDGE_HEIGHT					CONSTLIT("fadeEdgeHeight")
#define PROP_FONT								CONSTLIT("font")
#define PROP_LL_RADIUS							CONSTLIT("llRadius")
#define PROP_LR_RADIUS							CONSTLIT("lrRadius")
#define PROP_PADDING_BOTTOM						CONSTLIT("paddingBottom")
#define PROP_POSITION							CONSTLIT("position")
#define PROP_SCALE								CONSTLIT("scale")
#define PROP_TEXT								CONSTLIT("text")
#define PROP_TEXT_ALIGN_HORZ					CONSTLIT("textAlignHorz")
#define PROP_UL_RADIUS							CONSTLIT("ulRadius")
#define PROP_UR_RADIUS							CONSTLIT("urRadius")
#define PROP_VIEWPORT_HEIGHT					CONSTLIT("viewportHeight")

#define STYLE_IMAGE								CONSTLIT("image")

void CSmallOptionButtonAnimator::Create (CAniVScroller &Root, const CString &sID, const CString &sLabel, int x, int y, int cxWidth, AlignmentStyles iAlign)

//	Create
//
//	Creates the button.

	{
	const CVisualPalette &VI = m_Session.GetHI().GetVisuals();
	const CG16bitFont &MediumBoldFont = VI.GetFont(fontMediumBold);

	m_pRoot = &Root;
	m_sID = sID;
	m_x = x;
	m_y = y;
	m_cxWidth = cxWidth;
	m_iAlign = iAlign;

	//	Label

	IAnimatron *pLabel = new CAniText;
	if (iAlign == alignRight)
		{
		pLabel->SetPropertyVector(PROP_POSITION, CVector(x, y));
		pLabel->SetPropertyVector(PROP_SCALE, CVector(cxWidth - SMALL_BUTTON_WIDTH - MAJOR_PADDING_HORZ, 100.0));
		pLabel->SetPropertyString(PROP_TEXT_ALIGN_HORZ, ALIGN_RIGHT);
		}
	else
		{
		pLabel->SetPropertyVector(PROP_POSITION, CVector(x + SMALL_BUTTON_WIDTH + MAJOR_PADDING_HORZ, y));
		pLabel->SetPropertyVector(PROP_SCALE, CVector(cxWidth, 100.0));
		}

	pLabel->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextDialogLabel));
	pLabel->SetPropertyFont(PROP_FONT, &MediumBoldFont);
	pLabel->SetPropertyString(PROP_TEXT, sLabel);

	Root.AddLine(pLabel);

	//	Create a button

	int xButton;
	if (iAlign == alignRight)
		xButton = x + cxWidth - SMALL_BUTTON_WIDTH;
	else
		xButton = x;
		
	IAnimatron *pButton;
	VI.CreateImageButtonSmall(NULL, 
			sID, 
			xButton, 
			y,
			&VI.GetImage(imageSmallEditIcon),
			0,
			&pButton);

	pButton->SetID(sID);

	m_Session.RegisterPerformanceEvent(pButton, EVENT_ON_CLICK, sID);
	Root.AddLine(pButton);
	}

void CSmallOptionButtonAnimator::SetImage (const CG32bitImage &Image, bool bFreeImage)

//	SetImage
//
//	Sets the image.

	{
	if (m_pRoot == NULL)
		return;

	IAnimatron *pButton;
	if (m_pRoot->FindElement(m_sID, &pButton))
		{
		IAnimatron *pStyle = pButton->GetStyle(STYLE_IMAGE);
		pStyle->SetFillMethod(new CAniImageFill(&Image, bFreeImage));
		}
	}

void CSmallOptionButtonAnimator::SetText (const CString &sText)

//	SetText
//
//	Sets the text.

	{
	if (m_pRoot == NULL)
		return;

	const CVisualPalette &VI = m_Session.GetHI().GetVisuals();
	const CG16bitFont &MediumBoldFont = VI.GetFont(fontMediumBold);
	const CG16bitFont &SubTitleFont = VI.GetFont(fontSubTitle);
	CString sTextID = strPatternSubst(CONSTLIT("%s_text"), m_sID);

	//	Delete the previous one

	m_Session.DeleteElement(sTextID);

	//	Player name

	IAnimatron *pText = new CAniText;
	pText->SetID(sTextID);
	if (m_iAlign == alignRight)
		{
		pText->SetPropertyVector(PROP_POSITION, CVector(m_x, m_y + MediumBoldFont.GetHeight()));
		pText->SetPropertyVector(PROP_SCALE, CVector(m_cxWidth - SMALL_BUTTON_WIDTH - MAJOR_PADDING_HORZ, 100.0));
		pText->SetPropertyString(PROP_TEXT_ALIGN_HORZ, ALIGN_RIGHT);
		}
	else
		{
		pText->SetPropertyVector(PROP_POSITION, CVector(m_x + SMALL_BUTTON_WIDTH + MAJOR_PADDING_HORZ, m_y + MediumBoldFont.GetHeight()));
		pText->SetPropertyVector(PROP_SCALE, CVector(m_cxWidth, 100.0));
		}

	pText->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextDialogInput));
	pText->SetPropertyFont(PROP_FONT, &SubTitleFont);
	pText->SetPropertyString(PROP_TEXT, sText);

	m_pRoot->AddLine(pText);
	}

void CSmallOptionButtonAnimator::StartEdit (int cxWidth, const CString &sValue)

//	StartEdit
//
//	Edit the value.

	{
	if (m_pRoot == NULL || m_bInEditMode)
		return;

	const CVisualPalette &VI = m_Session.GetHI().GetVisuals();
	CString sTextID = strPatternSubst(CONSTLIT("%s_text"), m_sID);
	CString sEditID = strPatternSubst(CONSTLIT("%s_edit"), m_sID);

	m_Session.DeleteElement(sTextID);

	//	Create edit control

	IAnimatron *pEdit;
	VI.CreateEditControl(NULL,
			sEditID,
			m_x + SMALL_BUTTON_WIDTH + MAJOR_PADDING_HORZ,
			m_y,
			cxWidth,
			0,
			NULL_STR,
			&pEdit,
			NULL);

	pEdit->SetPropertyString(PROP_TEXT, sValue);

	m_pRoot->AddLine(pEdit);

	m_Session.SetInputFocus(sEditID);
	m_bInEditMode = true;
	}

void CSmallOptionButtonAnimator::StopEdit (CString *retsValue)

//	StopEdit
//
//	Done editing. Return the value.

	{
	if (m_pRoot == NULL || !m_bInEditMode)
		return;

	const CVisualPalette &VI = m_Session.GetHI().GetVisuals();
	CString sEditID = strPatternSubst(CONSTLIT("%s_edit"), m_sID);

	CString sValue;
	IAnimatron *pEdit;
	if (m_pRoot->FindElement(sEditID, &pEdit))
		{
		sValue = pEdit->GetPropertyString(PROP_TEXT);

		m_Session.DeleteElement(sEditID);
		}

	m_bInEditMode = false;
	SetText(sValue);

	if (retsValue)
		*retsValue = sValue;
	}

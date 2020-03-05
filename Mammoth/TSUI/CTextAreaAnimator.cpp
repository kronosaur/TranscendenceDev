//	CTextAreaAnimator.cpp
//
//	CTextAreaAnimator class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define ALIGN_RIGHT								CONSTLIT("right")

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

void CTextAreaAnimator::Create (CAniVScroller &Root, const CString &sID, const CString &sText, int x, int y, int cxWidth, AlignmentStyles iAlign)

//	Create
//
//	Creates the button.

	{
	const CVisualPalette &VI = m_Session.GetHI().GetVisuals();
	const CG16bitFont &MediumFont = VI.GetFont(fontMedium);

	m_pRoot = &Root;
	m_sID = sID;
	m_x = x;
	m_y = y;
	m_cxWidth = cxWidth;
	m_iAlign = iAlign;

	//	Text

	IAnimatron *pText = new CAniText;
	pText->SetPropertyVector(PROP_POSITION, CVector(x, y));
	pText->SetPropertyVector(PROP_SCALE, CVector(cxWidth, 100.0));

	if (iAlign == alignRight)
		pText->SetPropertyString(PROP_TEXT_ALIGN_HORZ, ALIGN_RIGHT);

	pText->SetPropertyColor(PROP_COLOR, VI.GetColor(colorTextNormal));
	pText->SetPropertyFont(PROP_FONT, &MediumFont);
	pText->SetPropertyString(PROP_TEXT, sText);
	pText->SetID(sID);

	Root.AddLine(pText);
	}

void CTextAreaAnimator::SetText (const CString &sText)

//	SetText
//
//	Sets the text.

	{
	IAnimatron *pText;
	if (!m_pRoot || !m_pRoot->FindElement(m_sID, &pText))
		return;

	pText->SetPropertyString(PROP_TEXT, sText);
	}

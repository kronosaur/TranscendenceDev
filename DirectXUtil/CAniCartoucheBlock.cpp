//	CAniCartoucheBlock.cpp
//
//	CAniCartoucheBlock class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include <windows.h>
#include "Alchemy.h"
#include "DirectXUtil.h"
#include <math.h>
#include <stdio.h>

#include "Reanimator.h"

const int INDEX_VISIBLE =					0;
#define PROP_VISIBLE						CONSTLIT("visible")

const int INDEX_POSITION =					1;
#define PROP_POSITION						CONSTLIT("position")

const int INDEX_SCALE =						2;
#define PROP_SCALE							CONSTLIT("scale")

const int INDEX_ROTATION =					3;
#define PROP_ROTATION						CONSTLIT("rotation")

const int INDEX_OPACITY =					4;
#define PROP_OPACITY						CONSTLIT("opacity")

const int INDEX_FONT =						5;
#define PROP_FONT							CONSTLIT("font")

#define ALIGN_BOTTOM						CONSTLIT("bottom")
#define ALIGN_CENTER						CONSTLIT("center")
#define ALIGN_LEFT							CONSTLIT("left")
#define ALIGN_RIGHT							CONSTLIT("right")
#define ALIGN_TOP							CONSTLIT("top")

CAniCartoucheBlock::CAniCartoucheBlock (void)

//	CAniCartoucheBlock constructor

	{
	m_Properties.SetInteger(PROP_VISIBLE, 1);
	m_Properties.SetVector(PROP_POSITION, CVector());
	m_Properties.SetVector(PROP_SCALE, CVector(0.0, 0.0));
	m_Properties.SetInteger(PROP_ROTATION, 0);
	m_Properties.SetOpacity(PROP_OPACITY, 255);
	m_Properties.SetFont(PROP_FONT, NULL);
	}

void CAniCartoucheBlock::Create (const TArray<CCartoucheBlock::SCartoucheDesc> &Data,
								 const CVector &vPos,
								 int cxWidth,
								 const CG16bitFont *pFont,
								 IAnimatron **retpAni)

//	Create
//
//	Creates text with basic attributes

	{
	CAniCartoucheBlock *pText = new CAniCartoucheBlock;
	pText->m_Block.Add(Data);
	pText->SetPropertyVector(PROP_POSITION, vPos);
	pText->SetPropertyVector(PROP_SCALE, CVector(cxWidth, 100000));
	pText->SetPropertyFont(PROP_FONT, pFont);

	*retpAni = pText;
	}

void CAniCartoucheBlock::GetSpacingRect (RECT *retrcRect)

//	GetSpacingRect
//
//	Returns the spacing rect

	{
	const CG16bitFont *pFont = m_Properties[INDEX_FONT].GetFont();
	if (pFont == NULL)
		return IAnimatron::GetSpacingRect(retrcRect);

	CVector vSize = m_Properties[INDEX_SCALE].GetVector();
	int cxWidth = (int)vSize.GetX();

	//	If 0 width, we pick an infinity width

	if (cxWidth <= 0)
		cxWidth = 100000;

	//	Format the block

	m_Block.SetFont(pFont);
	m_Block.Format(cxWidth);
	*retrcRect = m_Block.GetBounds();
	}

void CAniCartoucheBlock::Paint (SAniPaintCtx &Ctx)

//	Paint
//
//	Paints

	{
	const CG16bitFont *pFont = m_Properties[INDEX_FONT].GetFont();
	if (pFont == NULL)
		pFont = &CReanimator::GetDefaultFont();

	//	Get the rect size

	CVector vPos = Ctx.ToDest.Transform(m_Properties[INDEX_POSITION].GetVector());
	CVector vSize = m_Properties[INDEX_SCALE].GetVector();

	RECT rcRect;
	rcRect.left = (int)vPos.GetX();
	rcRect.top = (int)vPos.GetY();
	rcRect.right = rcRect.left + (int)vSize.GetX();
	rcRect.bottom = rcRect.top + (int)vSize.GetY();

	int cxWidth = RectWidth(rcRect);
	if (cxWidth <= 0)
		cxWidth = 100000;

	//	Format

	m_Block.SetFont(pFont);
	m_Block.Format(cxWidth);

	//	Paint

	m_Block.Paint(Ctx.Dest, rcRect.left, rcRect.top);
	}

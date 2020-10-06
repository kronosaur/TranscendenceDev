//	CIconBarDisplay
//
//	CIconBarDisplay class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

int CIconBarDisplay::HitTest (int x, int y) const

//	HitTest
//
//	Returs the index of the icon (or -1).

	{
	if (m_Data.IsEmpty() || x < m_rcRect.left || x >= m_rcRect.right || y < m_rcRect.top || y >= m_rcRect.bottom)
		return -1;

	const int xRel = x - m_rcRect.left;
	const int yRel = y - m_rcRect.top;
	int iIndex = xRel / m_cxEntry;
	if (iIndex >= m_Data.GetCount())
		return -1;

	return iIndex;
	}

void CIconBarDisplay::Init (const RECT &rcScreen, const CMenuData &Data)

//	Init
//
//	Initialize the icon bar.

	{
	m_rcScreen = rcScreen;
	m_Data = Data;

	//	Compute some metrics

	m_cxEntry = DEFAULT_ICON_WIDTH;
	m_cyEntry = DEFAULT_ICON_HEIGHT;

	int cxBar = m_cxEntry * m_Data.GetCount();
	int cyBar = m_cyEntry;

	//	Position

	m_rcRect.left = m_rcScreen.left + (RectWidth(m_rcScreen) - cxBar) / 2;
	m_rcRect.right = m_rcRect.left + cxBar;
	m_rcRect.bottom = m_rcScreen.bottom;
	m_rcRect.top = m_rcScreen.bottom - cyBar;

	m_bInvalid = true;
	m_iSelected = -1;
	m_iHover = -1;
	m_bDown = false;
	}

bool CIconBarDisplay::OnLButtonDown (int x, int y, DWORD dwFlags)

//	OnLButtonDown
//
//	Handle WM_LBUTTONDOWN

	{
	int iIndex = HitTest(x, y);
	if (iIndex == -1)
		return false;

	m_bDown = true;
	m_iSelected = iIndex;
	m_iLastCommand = CGameKeys::keyNone;
	m_bInvalid = true;

	return true;
	}

bool CIconBarDisplay::OnLButtonUp (int x, int y, DWORD dwFlags)

//	OnLButtonUp
//
//	Handle WM_LBUTTONUP

	{
	int iIndex = HitTest(x, y);
	if (m_bDown && iIndex != -1 && iIndex == m_iSelected)
		m_iLastCommand = (CGameKeys::Keys)m_Data.GetItemData(m_iSelected);

	m_bDown = false;
	m_iSelected = -1;
	m_bInvalid = true;

	return iIndex != -1;
	}

bool CIconBarDisplay::OnMouseMove (int x, int y)

//	OnMouseMove
//
//	Handle WM_MOUSEMOVE

	{
	int iIndex = HitTest(x, y);

	if (m_iHover != iIndex)
		{
		m_iHover = iIndex;
		m_bInvalid = true;
		}

	return iIndex != -1;
	}

void CIconBarDisplay::Paint (CG32bitImage &Screen, int iTick) const

//	Paint
//
//	Paint the icon bar.

	{
	if (m_Data.IsEmpty())
		return;

	Realize();
	Screen.Blt(0, 0, m_Buffer.GetWidth(), m_Buffer.GetHeight(), m_Buffer, m_rcRect.left, m_rcRect.top);
	}

void CIconBarDisplay::Realize (void) const

//	Realize
//
//	Paint the buffer.

	{
	if (m_Data.IsEmpty() || !m_bInvalid)
		return;

	//	Allocate buffer

	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &KeyFont = VI.GetFont(fontHeader);

	if (m_Buffer.IsEmpty())
		m_Buffer.Create(RectWidth(m_rcRect), RectHeight(m_rcRect), CG32bitImage::alpha8, CG32bitPixel::Null());
	else
		m_Buffer.Set(CG32bitPixel::Null());

	//	Colors

	CG32bitPixel rgbBackgroundHover = VI.GetColor(colorAreaDialogInputFocus);

	//	Paint each entry

	int xEntry = 0;
	int yEntry = 0;
	for (int i = 0; i < m_Data.GetCount(); i++)
		{
		//	Background highlight

		if (m_iSelected == i && m_iHover == i)
			CGDraw::RoundedRect(m_Buffer, xEntry, yEntry, m_cxEntry, m_cyEntry, ENTRY_BORDER_RADIUS, VI.GetColor(colorAreaDialogTitle));
		else if (m_iHover == i && m_iSelected == -1)
			CGDraw::RoundedRect(m_Buffer, xEntry, yEntry, m_cxEntry, m_cyEntry, ENTRY_BORDER_RADIUS, rgbBackgroundHover);

		//	Paint the icon

		if (const CObjectImageArray *pImage = m_Data.GetItemImage(i))
			pImage->PaintImageUL(m_Buffer, xEntry, yEntry, 0, 0);

		//	Paint the accelerator

		if (const CString &sKey = m_Data.GetItemKey(i))
			{
			CInputKeyPainter KeyPainter;
			KeyPainter.Init(sKey, KeyFont.GetHeight(), VI.GetFont(fontLarge));
			KeyPainter.SetBackColor(VI.GetColor(colorTextNormal));
			KeyPainter.SetTextColor(VI.GetColor(colorAreaDeep));
			int xKey = xEntry;
			int yKey = yEntry + m_cyEntry - KeyPainter.GetHeight();
			KeyPainter.Paint(m_Buffer, xKey, yKey);
			}

		xEntry += m_cxEntry;
		}

	m_bInvalid = false;
	}

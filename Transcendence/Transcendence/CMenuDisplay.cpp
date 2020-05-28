//	CMenuDisplay
//
//	CMenuDisplay class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

void CMenuDisplay::DoCommand (int iIndex)

//	DoCommand
//
//	Invoke the given menu item.

	{
	IHISession *pSession = m_HI.GetSession();
	if (pSession == NULL)
		return;

	CString sID = m_Data.GetItemID(iIndex);
	if (sID.IsBlank())
		return;

	m_Model.GetUniverse().PlaySound(NULL, m_Model.GetUniverse().FindSound(UNID_DEFAULT_SELECT));
	pSession->HICommand(sID);
	}

int CMenuDisplay::HitTest (int x, int y) const

//	HitTest
//
//	Returns the menu entry corresponding to x,y or -1 if none.

	{
	if (m_Data.IsEmpty() || m_cyEntry <= 0)
		return -1;

	//	Convert to coordinates relative to upper-left corner of first entry.

	x -= m_rcRect.left + MENU_BORDER_RADIUS;
	y -= m_rcRect.top + MENU_BORDER_RADIUS;

	//	If we're too far left or right, then no hit

	if (x < 0 || x > m_cxEntry)
		return -1;

	//	Compute the menu entry

	int iEntry = y / m_cyEntry;
	if (iEntry < 0 || iEntry >= m_Data.GetCount())
		return -1;
	
	return iEntry;
	}

void CMenuDisplay::Init (const RECT &rcScreen)

//	Init
//
//	Initialize

	{
	m_rcScreen = rcScreen;
	m_bInvalid = true;
	}

bool CMenuDisplay::OnChar (char chChar, DWORD dwKeyData)

//	OnChar
//
//	Handle a character.

	{
	int iIndex = m_Data.FindItemByKey(CString(&chChar, 1));
	if (iIndex < 0)
		return false;

	DoCommand(iIndex);
	return true;
	}

bool CMenuDisplay::OnLButtonDown (int x, int y, DWORD dwFlags)

//	OnLButtonDown
//
//	Handle mouse button. Returns TRUE if we handled it.

	{
	int iHitTest = HitTest(x, y);
	m_bDown = iHitTest != -1;
	m_bInvalid = true;
	return m_bDown;
	}

bool CMenuDisplay::OnLButtonUp (int x, int y, DWORD dwFlags)

//	OnLButtonUp
//
//	Handle mouse button. Returns TRUE if we handled it.

	{
	bool bWasDown = m_bDown;
	int iHitTest = HitTest(x, y);
	m_bDown = false;
	m_bInvalid = true;

	if (bWasDown && iHitTest != -1)
		DoCommand(iHitTest);

	return iHitTest != -1;
	}

bool CMenuDisplay::OnMouseMove (int x, int y)

//	OnMouseMove
//
//	Handle mouse move. Coordinates are relative to the session.

	{
	int iHitTest = HitTest(x, y);
	if (iHitTest != m_iHover)
		{
		m_iHover = iHitTest;
		m_bInvalid = true;
		}

	return iHitTest != -1;
	}

void CMenuDisplay::Paint (CG32bitImage &Screen, int iTick) const

//	Paint
//
//	Paints the menu.

	{
	if (m_Data.IsEmpty())
		return;

	Realize();
	Screen.Blt(0, 0, m_Buffer.GetWidth(), m_Buffer.GetHeight(), m_Buffer, m_rcRect.left, m_rcRect.top);
	}

void CMenuDisplay::Realize (void) const

//	Realize
//
//	Paint the buffer.

	{
	if (m_Data.IsEmpty() || !m_bInvalid)
		return;

	//	Allocate buffer

	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &MenuFont = VI.GetFont(fontHeader);

	m_Buffer.Create(RectWidth(m_rcRect), RectHeight(m_rcRect), CG32bitImage::alpha8, CG32bitPixel::Null());

	//	Colors

	CG32bitPixel rgbBackground(VI.GetColor(colorAreaDialog), MENU_BACKGROUND_OPACITY);
	CG32bitPixel rgbBackgroundHover = VI.GetColor(colorAreaDialogInputFocus);
	CG32bitPixel rgbText = VI.GetColor(colorTextHighlight);
	CG32bitPixel rgbTextBright = CG32bitPixel(255, 255, 255);

	//	Paint background

	CGDraw::RoundedRect(m_Buffer, 0, 0, RectWidth(m_rcRect), RectHeight(m_rcRect), MENU_BORDER_RADIUS, rgbBackground, CGDraw::blendCompositeNormal);
	CGDraw::RoundedRectOutline(m_Buffer, 0, 0, RectWidth(m_rcRect), RectHeight(m_rcRect), MENU_BORDER_RADIUS, MENU_BORDER_WIDTH, rgbText);

	//	Paint each entry.

	int xEntry = MENU_BORDER_RADIUS;
	int yEntry = MENU_BORDER_RADIUS;
	for (int i = 0; i < m_Data.GetCount(); i++)
		{
		//	Highlight hover

		if (i == m_iHover)
			m_Buffer.Fill(xEntry, yEntry, m_cxEntry, m_cyEntry, rgbBackgroundHover);

		//	Center the text

		int cxText = MenuFont.MeasureText(m_Data.GetItemLabel(i));
		int cyText = MenuFont.GetHeight();
		int xText = xEntry + (m_cxEntry - cxText) / 2;
		int yText = yEntry + MENU_ITEM_VPADDING;

		MenuFont.DrawText(m_Buffer, 
				xText, 
				yText, 
				(m_bDown && m_iHover == i ? rgbTextBright : rgbText), 
				m_Data.GetItemLabel(i));

		//	Paint the key

		if (const CString &sKey = m_Data.GetItemKey(i))
			{
			CInputKeyPainter KeyPainter;
			KeyPainter.Init(sKey, MenuFont.GetHeight(), VI.GetFont(fontLarge));
			KeyPainter.SetBackColor(VI.GetColor(colorTextNormal));
			KeyPainter.SetTextColor(VI.GetColor(colorAreaDeep));
			int xKey = xText - (MENU_ITEM_HPADDING + KeyPainter.GetWidth());
			int yKey = yText;
			KeyPainter.Paint(m_Buffer, xKey, yKey);
			}

		yEntry += m_cyEntry;
		}

	//	Done

	m_bInvalid = false;
	}

void CMenuDisplay::Show (const CMenuData &Data, EPositions iPosition)

//	Show
//
//	Shows the menu.

	{
	const CVisualPalette &VI = m_HI.GetVisuals();
	const CG16bitFont &MenuFont = VI.GetFont(fontHeader);

	m_Data = Data;

	//	Compute some metrics

	m_cxEntry = MENU_ITEM_WIDTH;
	m_cyEntry = MenuFont.GetHeight() + 2 * MENU_ITEM_VPADDING;

	int cxMenu = m_cxEntry + 2 * MENU_BORDER_RADIUS;
	int cyMenu = (m_Data.GetCount() * m_cyEntry) + 2 * MENU_BORDER_RADIUS;
	m_rcRect.left = m_rcScreen.left + (RectWidth(m_rcScreen) - cxMenu) / 2;
	m_rcRect.right = m_rcRect.left + cxMenu;
	m_rcRect.top = m_rcScreen.top + (RectHeight(m_rcScreen) - cyMenu) / 2;
	m_rcRect.bottom = m_rcRect.top + cyMenu;

	m_bInvalid = true;
	m_iHover = -1;
	m_bDown = false;
	}

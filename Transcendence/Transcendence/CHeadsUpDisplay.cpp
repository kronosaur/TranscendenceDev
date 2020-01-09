//	CHeadsUpDisplay.cpp
//
//	CHeadsUpDisplay class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

void CHeadsUpDisplay::CleanUp (void)

//  CleanUp
//
//  Release resources

	{
	for (int i = 0; i < hudCount; i++)
		m_pHUD[i].Delete();
	}

void CHeadsUpDisplay::GetClearHorzRect (RECT *retrcRect) const

//  GetClearHorzRect
//
//  Returns a RECT that excludes all corner HUD elements.

	{
	RECT rcRect = m_rcScreen;

	for (int i = 0; i < hudCount; i++)
		if (m_pHUD[i])
			{
			RECT rcHUD;
			m_pHUD[i]->GetRect(&rcHUD);

			DWORD dwLoc = m_pHUD[i]->GetLocation();
			if (dwLoc & IHUDPainter::locAlignTop)
				{
				if (rcRect.top < rcHUD.bottom)
					rcRect.top = rcHUD.bottom;
				}
			else if (dwLoc & IHUDPainter::locAlignBottom)
				{
				if (rcRect.bottom > rcHUD.top)
					rcRect.bottom = rcHUD.top;
				}
			}

	if (retrcRect)
		*retrcRect = rcRect;
	}

bool CHeadsUpDisplay::CreateHUD (EHUDTypes iHUD, const CShipClass &SourceClass, const RECT &rcScreen, CString *retsError)

//	CreateHUD
//
//	Creates a new HUD object of the given type.

	{
	IHUDPainter *pHUD = IHUDPainter::Create(iHUD, m_Model.GetUniverse(), m_HI.GetVisuals(), SourceClass, rcScreen, retsError);
	if (pHUD == NULL)
		return false;

	m_pHUD[iHUD].Set(pHUD);
	return true;
	}

bool CHeadsUpDisplay::Init (const RECT &rcRect, CString *retsError)

//  Init
//
//  Initialize

	{
	CPlayerShipController *pPlayer = m_Model.GetPlayer();
	if (pPlayer == NULL)
		return false;

	CShip *pShip = pPlayer->GetShip();
	if (pShip == NULL)
		return false;

	CleanUp();
	m_rcScreen = rcRect;

	//	Create all HUD painters

	for (int i = 0; i < hudCount; i++)
		{
		if (!CreateHUD((EHUDTypes)i, *pShip->GetClass(), rcRect, retsError))
			return false;
		}

	//  Success!

	return true;
	}

void CHeadsUpDisplay::Invalidate (EHUDTypes iHUD)

//  Invalidate
//
//  Invalidates a specific HUD. If iHUD == hudNone, then we invalidate all HUDs.

	{
	switch (iHUD)
		{
		case hudNone:
			for (int i = 0; i < hudCount; i++)
				InvalidateHUD((EHUDTypes)i);
			break;

		case hudArmor:
		case hudShields:
			InvalidateHUD(hudArmor);
			InvalidateHUD(hudShields);
			break;

		default:
			InvalidateHUD(iHUD);
			break;
		}
	}

void CHeadsUpDisplay::Paint (CG32bitImage &Screen, int iTick, bool bInDockScreen)

//  Paint
//
//  Paint

	{
	DEBUG_TRY

	CPlayerShipController *pPlayer = m_Model.GetPlayer();
	if (pPlayer == NULL)
		return;

	CSpaceObject *pSource = pPlayer->GetShip();
	if (pSource == NULL)
		return;

	CUniverse &Universe = m_Model.GetUniverse();
	SHUDPaintCtx PaintCtx(Universe, m_HI.GetVisuals(), *pSource);
	PaintCtx.iTick = iTick;
	PaintCtx.byOpacity = Universe.GetSFXOptions().GetHUDOpacity();

	//  If we're in a dock screen, we don't always paint the HUD if we don't 
	//  have enough room.

	bool bPaintTop = true;
	bool bPaintBottom = true;
	if (bInDockScreen)
		{
		if (Screen.GetHeight() < 768)
			bPaintBottom = false;

		if (Screen.GetWidth() < 1920)
			bPaintTop = false;
		}

	//	Paint

	for (int i = 0; i < hudCount; i++)
		{
		if (m_pHUD[i])
			{
			DWORD dwLoc = m_pHUD[i]->GetLocation();
			if ((dwLoc & IHUDPainter::locAlignTop) && bPaintTop)
				{
				PaintHUD((EHUDTypes)i, Screen, PaintCtx);
				}
			else if ((dwLoc & IHUDPainter::locAlignBottom) && bPaintBottom)
				{
				PaintHUD((EHUDTypes)i, Screen, PaintCtx);
				}
			else if (!bInDockScreen)
				{
				PaintHUD((EHUDTypes)i, Screen, PaintCtx);
				}
			}
		}

	DEBUG_CATCH
	}

void CHeadsUpDisplay::PaintHUD (EHUDTypes iHUD, CG32bitImage &Screen, SHUDPaintCtx &PaintCtx) const

//	PaintHUD
//
//	Paint the given HUD.

	{
	if (!m_pHUD[iHUD])
		return;

	switch (iHUD)
		{
		//	Armor and shields paint together

		case hudArmor:
			PaintCtx.iSegmentSelected = m_iSelection;
			PaintCtx.pShieldsHUD = m_pHUD[hudShields];
			SetProgramState(psPaintingArmorDisplay);

			m_pHUD[iHUD]->Paint(Screen, PaintCtx);

			PaintCtx.iSegmentSelected = -1;
			PaintCtx.pShieldsHUD = NULL;
			SetProgramState(psAnimating);
			break;

		case hudShields:
			break;

		default:
			m_pHUD[iHUD]->Paint(Screen, PaintCtx);
			break;
		}
	}

void CHeadsUpDisplay::SetArmorSelection (int iSelection)

//  SetArmorSelection
//
//  Selects a given armor section.

	{
	if (m_iSelection != iSelection)
		{
		m_iSelection = iSelection;
		Invalidate(hudArmor);
		}
	}

void CHeadsUpDisplay::Update (int iTick)

//  Update
//
//  Update displays

	{
	DEBUG_TRY

	CPlayerShipController *pPlayer = m_Model.GetPlayer();
	if (pPlayer == NULL)
		return;

	CSpaceObject *pSource = pPlayer->GetShip();
	if (pSource == NULL)
		return;

	SHUDUpdateCtx UpdateCtx(m_Model.GetUniverse(), *pSource);
	UpdateCtx.iTick = iTick;

	for (int i = 0; i < hudCount; i++)
		if (m_pHUD[i])
			m_pHUD[i]->Update(UpdateCtx);

	DEBUG_CATCH
	}

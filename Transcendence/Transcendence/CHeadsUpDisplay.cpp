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

bool CHeadsUpDisplay::Init (const RECT &rcRect)

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

    //  Initialize armor

	SDesignLoadCtx Ctx;
	m_pHUD[hudArmor].Set(IHUDPainter::Create(Ctx, pShip->GetClass(), hudArmor));
	m_pHUD[hudShields].Set(IHUDPainter::Create(Ctx, pShip->GetClass(), hudShields));
	SetHUDLocation(hudArmor, rcRect, IHUDPainter::locAlignBottom | IHUDPainter::locAlignRight);

    //  Initialize reactor

	m_pHUD[hudReactor].Set(IHUDPainter::Create(Ctx, pShip->GetClass(), hudReactor));
	SetHUDLocation(hudReactor, rcRect, IHUDPainter::locAlignTop | IHUDPainter::locAlignLeft);

    //  Initialize weapons

	m_pHUD[hudTargeting].Set(IHUDPainter::Create(Ctx, pShip->GetClass(), hudTargeting));
	SetHUDLocation(hudTargeting, rcRect, IHUDPainter::locAlignBottom | IHUDPainter::locAlignLeft);

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

void CHeadsUpDisplay::Paint (CG32bitImage &Screen, bool bInDockScreen)

//  Paint
//
//  Paint

    {
	DEBUG_TRY

    CPlayerShipController *pPlayer = m_Model.GetPlayer();
    if (pPlayer == NULL)
        return;

	SHUDPaintCtx PaintCtx;
	PaintCtx.pSource = pPlayer->GetShip();
	PaintCtx.byOpacity = g_pUniverse->GetSFXOptions().GetHUDOpacity();

    //  If we're in a dock screen, we don't always paint the HUD if we don't 
    //  have enough room.

    bool bPaintTop = true;
    bool bPaintBottom = true;
    if (bInDockScreen)
        {
        if (Screen.GetHeight() < 768)
            bPaintBottom = false;

        if (Screen.GetHeight() < 960)
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

	SHUDUpdateCtx UpdateCtx;
	UpdateCtx.pSource = pPlayer->GetShip();
	UpdateCtx.iTick = iTick;

	for (int i = 0; i < hudCount; i++)
		if (m_pHUD[i])
			m_pHUD[i]->Update(UpdateCtx);

	DEBUG_CATCH
    }

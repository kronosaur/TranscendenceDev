//	GameSessionDisplays.h
//
//	Transcendence session classes
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CHeadsUpDisplay
    {
    public:
        CHeadsUpDisplay (CHumanInterface &HI, CTranscendenceModel &Model) :
                m_HI(HI),
                m_Model(Model)
            { }

		CHeadsUpDisplay (const CHeadsUpDisplay &Src) = delete;
		CHeadsUpDisplay (CHeadsUpDisplay &&Src) = delete;

        ~CHeadsUpDisplay (void) { CleanUp(); }

		CHeadsUpDisplay &operator= (const CHeadsUpDisplay &Src) = delete;
		CHeadsUpDisplay &operator= (CHeadsUpDisplay &&Src) = delete;

        void CleanUp (void);
        void GetClearHorzRect (RECT *retrcRect) const;
        bool Init (const RECT &rcRect);
        void Invalidate (EHUDTypes iHUD = hudNone);
        void Paint (CG32bitImage &Screen, int iTick, bool bInDockScreen = false);
        void SetArmorSelection (int iSelection);
        void Update (int iTick);

    private:
		void InvalidateHUD (EHUDTypes iHUD) { if (m_pHUD[iHUD]) m_pHUD[iHUD]->Invalidate(); }
		void PaintHUD (EHUDTypes iHUD, CG32bitImage &Screen, SHUDPaintCtx &PaintCtx) const;
		void SetHUDLocation (EHUDTypes iHUD, const RECT &rcRect, DWORD dwLocation)
			{ if (m_pHUD[iHUD]) m_pHUD[iHUD]->SetLocation(rcRect, dwLocation); }

        CHumanInterface &m_HI;
        CTranscendenceModel &m_Model;
        RECT m_rcScreen;

		//	HUDs

		TUniquePtr<IHUDPainter> m_pHUD[hudCount];
		int m_iSelection = -1;						//  Selected armor seg (or -1)
    };

class CSystemMapDisplay
    {
    public:
        CSystemMapDisplay (CHumanInterface &HI, CTranscendenceModel &Model, CHeadsUpDisplay &HUD);

        bool HandleKeyDown (int iVirtKey, DWORD dwKeyData);
        bool HandleMouseWheel (int iDelta, int x, int y, DWORD dwFlags);
        bool Init (const RECT &rcRect);
        void OnHideMap (void);
        void OnShowMap (void);
        void Paint (CG32bitImage &Screen);

    private:
        enum EConstants
            {
            MAP_SCALE_COUNT = 4,
            };

        Metric GetScaleKlicksPerPixel (int iScale) const;

        CHumanInterface &m_HI;
        CTranscendenceModel &m_Model;
        CHeadsUpDisplay &m_HUD;
        RECT m_rcScreen;

        CMapScaleCounter m_Scale;           //  Track current map scale
        CMapLegendPainter m_HelpPainter;
    };


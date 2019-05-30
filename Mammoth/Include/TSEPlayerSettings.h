//	TSEPlayerSettings.h
//
//	Defines classes and interfaces for player ship display, etc.
//	Copyright (c) 2015 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum EUITypes
	{
	uiNone =						-1,

	uiPilot =						0,	//	Traditional maneuver/fire UI
	uiCommand =						1,	//	RTS-style select/order UI
	};

enum EHUDTypes
	{
	hudNone =						-1,	//	Invalid HUD

	hudArmor =						0,
	hudShields =					1,
	hudReactor =					2,
	hudTargeting =					3,

	hudCount =						4,
	};

class CDockScreenVisuals
    {
    public:
		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const;
        ALERROR Bind (SDesignLoadCtx &Ctx);
        const CObjectImageArray &GetBackground (void) const { return m_Background; }
        const CObjectImageArray &GetContentMask (void) const { return m_ContentMask; }
		int GetTabHeight (void) const { return DEFAULT_TAB_HEIGHT; }
        CG32bitPixel GetTextBackgroundColor (void) const { return m_rgbTextBackground; }
        CG32bitPixel GetTextColor (void) const { return m_rgbText; }
        CG32bitPixel GetTitleBackgroundColor (void) const { return m_rgbTitleBackground; }
        CG32bitPixel GetTitleTextColor (void) const { return m_rgbTitleText; }
        CG32bitPixel GetWindowBackgroundColor (void) const { return CG32bitPixel::Darken(m_rgbTitleBackground, 30);  }
        ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
        void MarkImages (void) const;

        static CDockScreenVisuals &GetDefault (void);

    private:
		static constexpr int DEFAULT_TAB_HEIGHT = 24;

        CObjectImageArray m_Background;     //  Background image for dock screen.
        CObjectImageArray m_ContentMask;    //  Mask for content

        CG32bitPixel m_rgbTitleBackground;  //  Color of title area background
        CG32bitPixel m_rgbTitleText;        //  Color of title area text
        CG32bitPixel m_rgbTextBackground;   //  Color of normal text backgrounds
        CG32bitPixel m_rgbText;             //  Color of normal text

        static CDockScreenVisuals m_Default;
        static bool m_bDefaultInitialized;
    };

class CDockScreenPainter
	{
	public:
		CDockScreenPainter (const CDockScreenVisuals &Visuals) :
				m_Visuals(Visuals)
			{ }

		void PaintDisplayFrame (CG32bitImage &Dest, const RECT &rcRect) const;

	private:
		static constexpr int BORDER_RADIUS = 4;

		const CDockScreenVisuals &m_Visuals;
	};

class CPlayerSettings
	{
	public:
		CPlayerSettings (void);
        inline CPlayerSettings (const CPlayerSettings &Src) { Copy(Src); }
		inline ~CPlayerSettings (void) { CleanUp(); }

        inline CPlayerSettings &operator= (const CPlayerSettings &Src) { CleanUp(); Copy(Src); return *this; }

		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const;
		ALERROR Bind (SDesignLoadCtx &Ctx, CShipClass *pClass);
		CEffectCreator *FindEffectCreator (const CString &sUNID) const;
		inline EUITypes GetDefaultUI (void) const { return m_iDefaultUI; }
		inline const CString &GetDesc (void) const { return m_sDesc; }
		inline const CDockScreenTypeRef &GetDockServicesScreen (void) const { return m_pDockServicesScreen; }
        inline const CDockScreenVisuals &GetDockScreenVisuals (void) const { return (m_pDockScreenDesc ? *m_pDockScreenDesc : CDockScreenVisuals::GetDefault()); }
		inline CXMLElement *GetHUDDesc (EHUDTypes iType) const { ASSERT(iType >= 0 && iType < hudCount); return m_HUDDesc[iType].pDesc; }
		inline DWORD GetLargeImage (void) const { return m_dwLargeImage; }
		inline const CDockScreenTypeRef &GetShipConfigScreen (void) const { return m_pShipConfigScreen; }
		inline const CDockScreenTypeRef &GetShipScreen (void) const { return m_pShipScreen; }
		inline int GetSortOrder (void) const { return m_iSortOrder; }
		inline const CCurrencyAndRange &GetStartingCredits (void) const { return m_StartingCredits; }
		inline DWORD GetStartingMap (void) const { return m_dwStartMap; }
		inline const CString &GetStartingNode (void) const { return m_sStartNode; }
		inline const CString &GetStartingPos (void) const { return m_sStartPos; }
		void InitAsDefault (void);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		inline bool IsDebugOnly (void) const { return (m_fDebug ? true : false); }
		inline bool IsDefault (void) const { return (m_fIsDefault ? true : false); }
		inline bool IsIncludedInAllAdventures (void) const { return (m_fIncludeInAllAdventures ? true : false); }
		inline bool IsInitialClass (void) const { return (m_fInitialClass ? true : false); }
        inline bool IsResolved (void) const { return (m_fResolved ? true : false); }
        inline void MarkImages (void) const { GetDockScreenVisuals().MarkImages(); }
		void MergeFrom (const CPlayerSettings &Src);
        void Resolve (const CPlayerSettings *pSrc);

	private:
		struct SHUDDesc
			{
			SHUDDesc (void) :
					pDesc(NULL),
					bOwned(false),
					bFree(false)
				{ }

			CXMLElement *pDesc;						//	HUD descriptor
			bool bOwned;							//	If TRUE, we own this element
			bool bFree;								//	If TRUE, we need to free this element
			};

		void CleanUp (void);
		ALERROR ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError);
        void Copy (const CPlayerSettings &Src);

		CString m_sDesc;							//	Description
		DWORD m_dwLargeImage;						//	UNID of large image
		int m_iSortOrder;							//	For setting order of appearence

		//	Miscellaneous
		CCurrencyAndRange m_StartingCredits;		//	Starting credits
		DWORD m_dwStartMap;							//	Starting map UNID
		CString m_sStartNode;						//	Starting node (may be blank)
		CString m_sStartPos;						//	Label of starting position (may be blank)
		CDockScreenTypeRef m_pShipScreen;			//	Ship screen
		CDockScreenTypeRef m_pDockServicesScreen;	//	Screen used for ship upgrades (may be NULL)
		CDockScreenTypeRef m_pShipConfigScreen;		//	Screen used to show ship configuration (may be NULL)

		//	UI Elements
		EUITypes m_iDefaultUI;						//	Default UI for this ship class
        CDockScreenVisuals *m_pDockScreenDesc;
		SHUDDesc m_HUDDesc[hudCount];

		//	Flags
		DWORD m_fInitialClass:1;					//	Use ship class at game start
		DWORD m_fDebug:1;							//	Debug only
		DWORD m_fIncludeInAllAdventures:1;			//	TRUE if we should always include this ship
        DWORD m_fOwnDockScreenDesc:1;               //  TRUE if we own m_pDockScreenVisuals element
		DWORD m_fResolved:1;                        //  TRUE if we have resolved all inheritance
		DWORD m_fIsDefault:1;						//	TRUE if this is the default (generic) player settings
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;
	};

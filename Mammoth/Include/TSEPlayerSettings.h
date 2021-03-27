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

	hudAccelerate =					0,
	hudArmor =						1,
	hudLRS =						2,
	hudReactor =					3,
	hudShields =					4,
	hudTargeting =					5,

	hudCount =						6,
	};

class CDockScreenVisuals
	{
	public:
		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const;
		const CSoundResource *GetAmbient () const { return m_Ambient.GetSoundResource(); }
		ALERROR Bind (SDesignLoadCtx &Ctx);
		const CObjectImageArray &GetBackground (void) const { return m_Background; }
		int GetBorderRadius (void) const { return DEFAULT_BORDER_RADIUS; }
		const CObjectImageArray &GetContentMask (void) const { return m_ContentMask; }
		const CG16bitFont &GetStatusFont (void) const { return *m_pStatusFont; }
		int GetTabHeight (void) const { return DEFAULT_TAB_HEIGHT; }
		CG32bitPixel GetTextBackgroundColor (void) const { return m_rgbTextBackground; }
		CG32bitPixel GetTextColor (void) const { return m_rgbText; }
		CG32bitPixel GetTitleBackgroundColor (void) const { return m_rgbTitleBackground; }
		CG32bitPixel GetTitleTextColor (void) const { return m_rgbTitleText; }
		CG32bitPixel GetWindowBackgroundColor (void) const { return CG32bitPixel::Darken(m_rgbTitleBackground, 30);  }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc);
		void MarkImages (void) const;

		static CDockScreenVisuals &GetDefault (CUniverse &Universe);

	private:
		static constexpr int DEFAULT_BORDER_RADIUS = 4;
		static constexpr int DEFAULT_TAB_HEIGHT = 24;

		CObjectImageArray m_Background;     //  Background image for dock screen.
		CObjectImageArray m_ContentMask;    //  Mask for content

		CG32bitPixel m_rgbTitleBackground;  //  Color of title area background
		CG32bitPixel m_rgbTitleText;        //  Color of title area text
		CG32bitPixel m_rgbTextBackground;   //  Color of normal text backgrounds
		CG32bitPixel m_rgbText;             //  Color of normal text

		const CG16bitFont *m_pStatusFont = NULL;

		CSoundRef m_Ambient;				//	Ambient sound

		static CDockScreenVisuals m_Default;
		static bool m_bDefaultInitialized;
	};

class CDockScreenPainter
	{
	public:
		CDockScreenPainter (const CDockScreenVisuals &Theme, int cyTabRegion = 0) :
				m_Theme(Theme),
				m_cyTabRegion(cyTabRegion)
			{ }

		void PaintDisplayBackground (CG32bitImage &Dest, const RECT &rcRect) const;
		void PaintDisplayFrame (CG32bitImage &Dest, const RECT &rcRect) const;

	private:
		static constexpr int BORDER_RADIUS = 4;

		const CDockScreenVisuals &m_Theme;
		int m_cyTabRegion = 0;
	};

class CPlayerSettings
	{
	public:
		CPlayerSettings (void);
		CPlayerSettings (const CPlayerSettings &Src) { Copy(Src); }
		~CPlayerSettings (void) { CleanUp(); }

		CPlayerSettings &operator= (const CPlayerSettings &Src) { CleanUp(); Copy(Src); return *this; }

		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) const;
		ALERROR Bind (SDesignLoadCtx &Ctx, CShipClass *pClass);
		CEffectCreator *FindEffectCreator (const CString &sUNID) const;
		EUITypes GetDefaultUI (void) const { return m_iDefaultUI; }
		const CString &GetDesc (void) const { return m_sDesc; }
		const CDockScreenTypeRef &GetDockServicesScreen (void) const { return m_pDockServicesScreen; }
		const CDockScreenVisuals &GetDockScreenVisuals (CUniverse &Universe) const { return (m_pDockScreenDesc ? *m_pDockScreenDesc : CDockScreenVisuals::GetDefault(Universe)); }
		CXMLElement *GetHUDDesc (EHUDTypes iType) const { ASSERT(iType >= 0 && iType < hudCount); return m_HUDDesc[iType].pDesc; }
		DWORD GetLargeImage (void) const { return m_dwLargeImage; }
		const CDockScreenTypeRef &GetShipConfigScreen (void) const { return m_pShipConfigScreen; }
		const CDockScreenTypeRef &GetShipScreen (void) const { return m_pShipScreen; }
		int GetSortOrder (void) const { return m_iSortOrder; }
		const CCurrencyAndRange &GetStartingCredits (void) const { return m_StartingCredits; }
		DWORD GetStartingMap (void) const { return m_dwStartMap; }
		const CString &GetStartingNode (void) const { return m_sStartNode; }
		const CString &GetStartingPos (void) const { return m_sStartPos; }
		void InitAsDefault (void);
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		bool IsDebugOnly (void) const { return (m_fDebug ? true : false); }
		bool IsDefault (void) const { return (m_fIsDefault ? true : false); }
		bool IsIncludedInAllAdventures (void) const { return (m_fIncludeInAllAdventures ? true : false); }
		bool IsInitialClass (void) const { return (m_fInitialClass ? true : false); }
		bool IsResolved (void) const { return (m_fResolved ? true : false); }
		void MarkImages (CUniverse &Universe) const { GetDockScreenVisuals(Universe).MarkImages(); }
		void MergeFrom (const CPlayerSettings &Src);
		void Resolve (CUniverse &Universe, const CPlayerSettings *pSrc);

	private:
		struct SHUDDesc
			{
			CXMLElement *pDesc = NULL;				//	HUD descriptor
			bool bOwned = false;					//	If TRUE, we own this element
			bool bFree = false;						//	If TRUE, we need to free this element
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

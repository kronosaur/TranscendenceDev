//	TSEOverlayType.h
//
//	Classes and functions for overlays.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class COverlayCounterDesc
	{
	public:
		enum ETypes
			{
			counterNone,						//	Do not show a counter

			counterCommandBarProgress,			//	Show as progress bar in command bar
			counterProgress,					//	Show as progress bar on object
			counterRadius,						//	Show as circle of given radius (pixels)
			counterFlag,						//	Show a flag with counter and label
			counterTextFlag,					//	Show a flag with just text (no counter)
			};

		CG32bitPixel GetColor (void) const { return m_rgbPrimary; }
		const CString &GetLabel (void) const { return m_sLabel; }
		int GetMaxValue (void) const { return m_iMaxValue; }
		ETypes GetType (void) const { return m_iType; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);
		bool IsEmpty (void) const { return (m_iType == counterNone); }
		bool IsShownOnMap (void) const { return m_bShowOnMap; }

	private:
		ETypes m_iType = counterNone;			//	Type of counter to paint
		CString m_sLabel;						//	Label for counter
		int m_iMaxValue = 0;					//	Max value of counter (for progress bar)
		CG32bitPixel m_rgbPrimary;				//	Counter color

		bool m_bShowOnMap = false;				//	If TRUE, show counter on map
	};

class COverlayUndergroundDesc
	{
	public:
		enum ETypes
			{
			typeNone,

			typeVault,
			};

		int GetHitPoints (int iLevel = 1) const;
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc);
		bool IsEmpty (void) const { return m_iType == typeNone; }

	private:
		ETypes m_iType = typeNone;
		int m_iStrength = 0;					//	Hits points to dig up
	};

class COverlayType : public CDesignType
	{
	public:
		COverlayType(void);
		COverlayType (const COverlayType &Src) = delete;
		COverlayType (COverlayType &&Src) = delete;

		virtual ~COverlayType(void);

		COverlayType &operator= (const COverlayType &Src) = delete;
		COverlayType &operator= (COverlayType &&Src) = delete;

		bool AbsorbsWeaponFire (CInstalledDevice *pWeapon);
		bool Disarms (void) const { return m_fDisarmShip; }
		const COverlayCounterDesc &GetCounterDesc (void) const { return m_Counter; }
		int GetDamageAbsorbed (CSpaceObject *pSource, SDamageCtx &Ctx);
		Metric GetDrag (void) const { return m_rDrag; }
		CEffectCreator *GetEffectCreator (void) const { return m_pEffect; }
		const CEnhancementDesc &GetEnhancementsConferred (void) const { return m_Enhancements; }
		CEffectCreator *GetHitEffectCreator (void) const { return m_pHitEffect; }
		int GetMaxHitPoints (const CSpaceObject &Source) const;
		const COverlayUndergroundDesc &GetUndergroundDesc (void) const { return m_Underground; }
		int GetWeaponBonus (CInstalledDevice *pDevice, CSpaceObject *pSource);
		bool IsHitEffectAlt (void) { return m_fAltHitEffect; }
		bool IsShieldOverlay (void) { return m_fShieldOverlay; }
		bool IsShipScreenDisabled (void) { return m_fDisableShipScreen; }
		bool IsShownOnMap (void) { return m_Counter.IsShownOnMap(); }
		bool IsUnderground (void) const { return !m_Underground.IsEmpty(); }
		bool Paralyzes (void) const { return m_fParalyzeShip; }
		bool RotatesWithSource (const CSpaceObject &Source) const;
		bool ShowsInHUD () const { return m_fShowInHUD; }
		bool Spins (void) const { return m_fSpinShip; }
		bool StopsTime (void) const { return m_fTimeStop; }

		//	CDesignType overrides
		static COverlayType *AsType(CDesignType *pType) { return ((pType && pType->GetType() == designOverlayType) ? (COverlayType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual CString GetNamePattern (DWORD dwNounFormFlags = 0, DWORD *retdwFlags = NULL) const override { if (retdwFlags) *retdwFlags = 0; return m_sNamePattern; }
		virtual DesignTypes GetType (void) const override { return designOverlayType; }
		virtual const CObjectImageArray &GetTypeSimpleImage (void) const override { return m_Image; }

	protected:
		//	CDesignType overrides
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;
		virtual void OnMarkImages (void) override;

	private:
		CString m_sNamePattern;					//	Name of overlay (optional)
		CObjectImageArray m_Image;				//	Image to show on HUD (optional)

		CEnhancementDesc m_Enhancements;		//	Enhancements conferred
		CDamageAdjDesc m_AbsorbAdj;				//	Damage absorbed by the field
		DamageTypeSet m_WeaponSuppress;			//	Types of weapons suppressed
		CDamageAdjDesc m_BonusAdj;				//	Bonus to weapon damage (by damage type)
		Metric m_rDrag = 1.0;					//	Drag coefficient (1.0 = no drag)

		CEffectCreator *m_pEffect = NULL;		//	Effect for field
		CEffectCreator *m_pHitEffect = NULL;	//	Effect when field is hit by damage

		COverlayCounterDesc m_Counter;			//	Counter descriptor
		COverlayUndergroundDesc m_Underground;	//	Underground (mining) descriptor

		DWORD m_fAltHitEffect:1 = false;		//	If TRUE, hit effect replaces normal effect
		DWORD m_fRotateWithShip:1 = false;		//	If TRUE, we rotate along with source rotation
		DWORD m_fShieldOverlay:1 = false;		//	If TRUE, we are above hull/armor
		DWORD m_fParalyzeShip:1 = false;		//	If TRUE, ship is paralyzed
		DWORD m_fDisarmShip:1 = false;			//	If TRUE, ship is disarmed
		DWORD m_fDisableShipScreen:1 = false;	//	If TRUE, player cannot bring up ship screen
		DWORD m_fSpinShip:1 = false;			//	If TRUE, ship spins uncontrollably
		DWORD m_fTimeStop:1 = false;			//	If TRUE, ship is time-stopped

		DWORD m_fRotateWithSource:1 = false;	//	If TRUE, we rotate along with source
		DWORD m_fShowInHUD:1 = false;			//	If TRUE, show time remaining on HUD
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;
	};

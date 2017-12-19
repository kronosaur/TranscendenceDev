//	TSEOverlayType.h
//
//	Classes and functions for overlays.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class COverlayType : public CDesignType
	{
	public:
		enum ECounterDisplay
			{
			counterNone,						//	Do not show a counter

			counterCommandBarProgress,			//	Show as progress bar in command bar
			counterProgress,					//	Show as progress bar on object
			counterRadius,						//	Show as circle of given radius (pixels)
			counterFlag,						//	Show a flag with counter and label
			};

		COverlayType(void);
		virtual ~COverlayType(void);

		bool AbsorbsWeaponFire (CInstalledDevice *pWeapon);
		inline bool Disarms (void) const { return m_fDisarmShip; }
		inline CG32bitPixel GetCounterColor (void) const { return m_rgbCounterColor; }
		inline const CString &GetCounterLabel (void) const { return m_sCounterLabel; }
		inline int GetCounterMax (void) const { return m_iCounterMax; }
		inline ECounterDisplay GetCounterStyle (void) const { return m_iCounterType; }
		int GetDamageAbsorbed (CSpaceObject *pSource, SDamageCtx &Ctx);
		inline Metric GetDrag (void) const { return m_rDrag; }
		inline CEffectCreator *GetEffectCreator (void) const { return m_pEffect; }
		inline CEffectCreator *GetHitEffectCreator (void) const { return m_pHitEffect; }
		int GetWeaponBonus (CInstalledDevice *pDevice, CSpaceObject *pSource);
		inline bool IsHitEffectAlt (void) { return m_fAltHitEffect; }
		inline bool IsShieldOverlay (void) { return m_fShieldOverlay; }
		inline bool IsShipScreenDisabled (void) { return m_fDisableShipScreen; }
		inline bool IsShownOnMap (void) { return m_fShowOnMap; }
		inline bool Paralyzes (void) const { return m_fParalyzeShip; }
		inline bool RotatesWithShip (void) { return m_fRotateWithShip; }
		inline bool Spins (void) const { return m_fSpinShip; }

		//	CDesignType overrides
		static COverlayType *AsType(CDesignType *pType) { return ((pType && pType->GetType() == designOverlayType) ? (COverlayType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual DesignTypes GetType (void) const override { return designOverlayType; }

	protected:
		//	CDesignType overrides
		virtual void OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed) override;
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual CEffectCreator *OnFindEffectCreator (const CString &sUNID) override;

	private:
		CDamageAdjDesc m_AbsorbAdj;				//	Damage absorbed by the field
		DamageTypeSet m_WeaponSuppress;			//	Types of weapons suppressed
		CDamageAdjDesc m_BonusAdj;				//	Bonus to weapon damage (by damage type)
		Metric m_rDrag;							//	Drag coefficient (1.0 = no drag)

		CEffectCreator *m_pEffect;				//	Effect for field
		CEffectCreator *m_pHitEffect;			//	Effect when field is hit by damage

		ECounterDisplay m_iCounterType;			//	Type of counter to paint
		CString m_sCounterLabel;				//	Label for counter
		int m_iCounterMax;						//	Max value of counter (for progress bar)
		CG32bitPixel m_rgbCounterColor;					//	Counter color

		DWORD m_fAltHitEffect:1;				//	If TRUE, hit effect replaces normal effect
		DWORD m_fRotateWithShip:1;				//	If TRUE, we rotate along with source rotation
		DWORD m_fShieldOverlay:1;				//	If TRUE, we are above hull/armor
		DWORD m_fParalyzeShip:1;				//	If TRUE, ship is paralyzed
		DWORD m_fDisarmShip:1;					//	If TRUE, ship is disarmed
		DWORD m_fDisableShipScreen:1;			//	If TRUE, player cannot bring up ship screen
		DWORD m_fSpinShip:1;					//	If TRUE, ship spins uncontrollably
		DWORD m_fShowOnMap:1;					//	If TRUE, we show on the system map

		DWORD m_fSpare1:1;
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;
	};



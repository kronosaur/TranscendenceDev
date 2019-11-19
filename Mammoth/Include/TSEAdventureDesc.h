//	TSEAdventureDesc.h
//
//	Classes and functions for adventure descriptors.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CEngineOptions
	{
	public:
		CEngineOptions (void);

		const CDamageAdjDesc *GetArmorDamageAdj (int iLevel) const { if (iLevel < 1 || iLevel > MAX_ITEM_LEVEL) throw CException(ERR_FAIL); return &m_ArmorDamageAdj[iLevel - 1]; }
		int GetDefaultInteraction (void) const { return m_iDefaultInteraction; }
		int GetDefaultShotHP (void) const { return m_iDefaultShotHP; }
		const CDamageAdjDesc *GetShieldDamageAdj (int iLevel) const { if (iLevel < 1 || iLevel > MAX_ITEM_LEVEL) throw CException(ERR_FAIL); return &m_ShieldDamageAdj[iLevel - 1]; }
		bool InitArmorDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc) { return InitDamageAdjFromXML(Ctx, XMLDesc, m_ArmorDamageAdj); }
		bool InitFromProperties (SDesignLoadCtx &Ctx, const CDesignType &Type);
		bool InitShieldDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc) { return InitDamageAdjFromXML(Ctx, XMLDesc, m_ShieldDamageAdj); }

	private:
		bool InitDamageAdjFromXML (SDesignLoadCtx &Ctx, const CXMLElement &XMLDesc, CDamageAdjDesc *DestTable);

		static void InitDefaultDamageAdj (void);

		CDamageAdjDesc m_ArmorDamageAdj[MAX_ITEM_LEVEL];
		CDamageAdjDesc m_ShieldDamageAdj[MAX_ITEM_LEVEL];
		int m_iDefaultInteraction = -1;
		int m_iDefaultShotHP = -1;
	};

class CAdventureDesc : public CDesignType
	{
	public:
		CAdventureDesc (void);

		void FireOnGameEnd (const CGameRecord &Game, const SBasicGameStats &BasicStats);
		void FireOnGameStart (void);
		DWORD GetBackgroundUNID (void) const { return m_dwBackgroundUNID; }
		const CEconomyType &GetDefaultCurrency (void) const;
		CString GetDesc (void);
		const CXMLElement &GetEncounterOverrideXML (void) const { return m_EncounterOverridesXML; }
		const CEngineOptions &GetEngineOptions (void) const { return m_EngineOptions; }
		DWORD GetExtensionUNID (void) const { return m_dwExtensionUNID; }
		const CString &GetName (void) const { return m_sName; }
		DWORD GetStartingMapUNID (void) const { return m_dwStartingMap; }
		const CString &GetStartingNodeID (void) const { return m_sStartingNodeID; }
		const CString &GetStartingPos (void) const { return m_sStartingPos; }
		ALERROR GetStartingShipClasses (TSortMap<CString, CShipClass *> *retClasses, CString *retsError);
		const CString &GetWelcomeMessage (void) const { return m_sWelcomeMessage; }
		bool InitEngineOptions (SDesignLoadCtx &Ctx) { return m_EngineOptions.InitFromProperties(Ctx, *this); }
		bool IsCurrentAdventure (void) const { return (m_fIsCurrentAdventure ? true : false); }
		bool IsInDefaultResource (void) const { return (m_fInDefaultResource ? true : false); }
		bool IsValidStartingClass (CShipClass *pClass);
		void SetCurrentAdventure (bool bCurrent = true) { m_fIsCurrentAdventure = bCurrent; }

		//	CDesignType overrides

		static CAdventureDesc *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designAdventureDesc) ? (CAdventureDesc *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual DesignTypes GetType (void) const override { return designAdventureDesc; }

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnUnbindDesign (void) override { m_fIsCurrentAdventure = false; }

	private:

		DWORD m_dwExtensionUNID = 0;

		CString m_sName;						//	Name of adventure
		DWORD m_dwBackgroundUNID = 0;				//	Background image to use for choice screen
		CString m_sWelcomeMessage;				//	Equivalent of "Welcome to Transcendence!"

		CDesignTypeCriteria m_StartingShips;	//	Starting ship criteria
		DWORD m_dwStartingMap = 0;				//	Default system map to load
		CString m_sStartingNodeID;				//	NodeID where we start
		CString m_sStartingPos;					//	Named object at which we start

		CEngineOptions m_EngineOptions;			//	Options for engine behavior
		CXMLElement m_EncounterOverridesXML;

		CEconomyTypeRef m_pDefaultCurrency;		//	Default currency (mostly used for UI)

		DWORD m_fIsCurrentAdventure:1;			//	TRUE if this is the current adventure
		DWORD m_fInDefaultResource:1;			//	TRUE if adventure is a module in the default resource
		DWORD m_fIncludeOldShipClasses:1;		//	TRUE if we should include older extensions (even if 
												//		they don't match starting ship criteria).
	};


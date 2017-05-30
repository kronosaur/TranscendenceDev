//	TSEAdventureDesc.h
//
//	Classes and functions for adventure descriptors.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CAdventureDesc : public CDesignType
	{
	public:
		CAdventureDesc (void);

		void FireOnGameEnd (const CGameRecord &Game, const SBasicGameStats &BasicStats);
		void FireOnGameStart (void);
		inline const CDamageAdjDesc *GetArmorDamageAdj (int iLevel) const { return &m_ArmorDamageAdj[iLevel - 1]; }
		inline DWORD GetBackgroundUNID (void) { return m_dwBackgroundUNID; }
		CString GetDesc (void);
		const CStationEncounterDesc *GetEncounterDesc (DWORD dwUNID) const;
		inline DWORD GetExtensionUNID (void) { return m_dwExtensionUNID; }
		inline const CString &GetName (void) { return m_sName; }
		inline const CDamageAdjDesc *GetShieldDamageAdj (int iLevel) const { return &m_ShieldDamageAdj[iLevel - 1]; }
		inline DWORD GetStartingMapUNID (void) { return m_dwStartingMap; }
		inline const CString &GetStartingNodeID (void) { return m_sStartingNodeID; }
		inline const CString &GetStartingPos (void) { return m_sStartingPos; }
		ALERROR GetStartingShipClasses (TSortMap<CString, CShipClass *> *retClasses, CString *retsError);
		inline const CString &GetWelcomeMessage (void) { return m_sWelcomeMessage; }
		bool InitEncounterOverrides (CString *retsError = NULL);
		inline bool IsCurrentAdventure (void) { return (m_fIsCurrentAdventure ? true : false); }
		inline bool IsInDefaultResource (void) { return (m_fInDefaultResource ? true : false); }
		bool IsValidStartingClass (CShipClass *pClass);
		inline void SetCurrentAdventure (bool bCurrent = true) { m_fIsCurrentAdventure = bCurrent; }

		//	CDesignType overrides

		static CAdventureDesc *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designAdventureDesc) ? (CAdventureDesc *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual DesignTypes GetType (void) const override { return designAdventureDesc; }

		//	Helpers

		static const CDamageAdjDesc *GetDefaultArmorDamageAdj (int iLevel);
		static const CDamageAdjDesc *GetDefaultShieldDamageAdj (int iLevel);

	protected:
		//	CDesignType overrides
		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnUnbindDesign (void) override { m_fIsCurrentAdventure = false; }

	private:
		static void InitDefaultDamageAdj (void);

		DWORD m_dwExtensionUNID;

		CString m_sName;						//	Name of adventure
		DWORD m_dwBackgroundUNID;				//	Background image to use for choice screen
		CString m_sWelcomeMessage;				//	Equivalent of "Welcome to Transcendence!"

		CDesignTypeCriteria m_StartingShips;	//	Starting ship criteria
		DWORD m_dwStartingMap;					//	Default system map to load
		CString m_sStartingNodeID;				//	NodeID where we start
		CString m_sStartingPos;					//	Named object at which we start

		CDamageAdjDesc m_ArmorDamageAdj[MAX_ITEM_LEVEL];
		CDamageAdjDesc m_ShieldDamageAdj[MAX_ITEM_LEVEL];

		CXMLElement m_EncounterOverridesXML;
		TSortMap<DWORD, CStationEncounterDesc> m_Encounters;

		DWORD m_fIsCurrentAdventure:1;			//	TRUE if this is the current adventure
		DWORD m_fInDefaultResource:1;			//	TRUE if adventure is a module in the default resource
		DWORD m_fIncludeOldShipClasses:1;		//	TRUE if we should include older extensions (even if 
												//		they don't match starting ship criteria).
		DWORD m_fInInitEncounterOverrides:1;	//	TRUE if we're initializing this.
	};


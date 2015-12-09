//	TSEPlayerSettings.h
//
//	Defines classes and interfaces for player ship display, etc.
//	Copyright (c) 2015 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum EHUDTypes
	{
	hudNone =						-1,		//	Invalid HUD

	hudArmor =						0,
	hudShields =					1,
	hudReactor =					2,
	hudTargeting =					3,

	hudCount =						4,
	};

class CPlayerSettings
	{
	public:
		CPlayerSettings (void)
			{ }

		~CPlayerSettings (void) { CleanUp(); }

		CPlayerSettings &operator= (const CPlayerSettings &Source);

		void AddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed);
		ALERROR Bind (SDesignLoadCtx &Ctx, CShipClass *pClass);
		CEffectCreator *FindEffectCreator (const CString &sUNID);
		inline const CString &GetDesc (void) const { return m_sDesc; }
		inline const CDockScreenTypeRef &GetDockServicesScreen (void) const { return m_pDockServicesScreen; }
		inline CXMLElement *GetHUDDesc (EHUDTypes iType) const { ASSERT(iType >= 0 && iType < hudCount); return m_HUDDesc[iType].pDesc; }
		inline const CCurrencyAndValue &GetHullValue (void) const { return m_HullValue; }
		inline DWORD GetLargeImage (void) const { return m_dwLargeImage; }
		inline const CDockScreenTypeRef &GetShipConfigScreen (void) const { return m_pShipConfigScreen; }
		inline const CDockScreenTypeRef &GetShipScreen (void) const { return m_pShipScreen; }
		inline int GetSortOrder (void) const { return m_iSortOrder; }
		inline const CCurrencyAndRange &GetStartingCredits (void) const { return m_StartingCredits; }
		inline DWORD GetStartingMap (void) const { return m_dwStartMap; }
		inline const CString &GetStartingNode (void) const { return m_sStartNode; }
		inline const CString &GetStartingPos (void) const { return m_sStartPos; }
		ALERROR InitFromXML (SDesignLoadCtx &Ctx, CShipClass *pClass, CXMLElement *pDesc);
		inline bool IsDebugOnly (void) const { return (m_fDebug ? true : false); }
		inline bool IsHUDDescInherited (EHUDTypes iType) const { ASSERT(iType >= 0 && iType < hudCount); return m_HUDDesc[iType].bInherited;  }
		inline bool IsIncludedInAllAdventures (void) const { return (m_fIncludeInAllAdventures ? true : false); }
		inline bool IsInitialClass (void) const { return (m_fInitialClass ? true : false); }
		void MergeFrom (const CPlayerSettings &Src);

	private:
		struct SHUDDesc
			{
			SHUDDesc (void) :
					pDesc(NULL),
					bFree(false),
					bInherited(false)
				{ }

			CXMLElement *pDesc;						//	HUD descriptor
			bool bFree;								//	If TRUE, we need to free this on exit
			bool bInherited;						//	If TRUE, we inherited this from a base class
			};

		void CleanUp (void);
		ALERROR ComposeLoadError (SDesignLoadCtx &Ctx, const CString &sError);

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
		CCurrencyAndValue m_HullValue;				//	Value of hull alone (excluding any devices/armor)

		//	HUD descriptors
		SHUDDesc m_HUDDesc[hudCount];

		//	Flags
		DWORD m_fInitialClass:1;					//	Use ship class at game start
		DWORD m_fDebug:1;							//	Debug only
		DWORD m_fIncludeInAllAdventures:1;			//	TRUE if we should always include this ship
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:24;
	};

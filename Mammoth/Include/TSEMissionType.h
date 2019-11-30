//	TSEMissionType.h
//
//	Classes and functions for mission types.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CMissionList;

enum class EMissionAutoAccept
	{
	none,
	error,

	acceptAndExit,
	acceptAndContinue,
	};

class CMissionType : public CDesignType
	{
	public:
		enum ECachedHandlers
			{
			//	This list must match CACHED_EVENTS array in CMissionType.cpp

			evtCanCreate				= 0,

			evtCount					= 1,
			};

		bool CanBeCreated (const CMissionList &AllMissions, CSpaceObject *pOwner, ICCItem *pCreateData) const;
		bool CanBeDeclined (void) const { return (m_iAutoAccept == EMissionAutoAccept::none); }
		bool CanBeDeleted (void) const { return m_fAllowDelete; }
		bool CanBeEncountered (void) const { return (m_iMaxAppearing == -1 || m_iExisting < m_iMaxAppearing); }
		bool CleanNonPlayer(void) const { return !m_fRecordNonPlayer; }
		bool CloseIfOutOfSystem (void) const { return m_fCloseIfOutOfSystem; }
		bool FailureWhenOwnerDestroyed (void) const { return !m_fNoFailureOnOwnerDestroyed; }
		bool FailureWhenOutOfSystem (void) const { return (m_iFailIfOutOfSystem != -1); }
		bool FindCachedEventHandler (ECachedHandlers iEvent, SEventHandlerDesc *retEvent = NULL) const { return m_CachedEvents.FindEventHandler(iEvent, retEvent); }
		bool ForceUndockAfterDebrief (void) const { return m_fForceUndockAfterDebrief; }
		const CString &GetArc (void) const { return m_sArc; }
		int GetArcSequence (void) const { return m_iArcSequence; }
		EMissionAutoAccept GetAutoAccept (void) const { return m_iAutoAccept; }
		const CString &GetName (void) const { return m_sName; }
		int GetExpireTime (void) const { return m_iExpireTime; }
		DWORD GetLastAcceptedOn (void) const { return m_dwLastAcceptedOn; }
		int GetOutOfSystemTimeOut (void) const { return m_iFailIfOutOfSystem; }
		int GetPriority (void) const { return (m_pArcRoot ? m_pArcRoot->m_iPriority : m_iPriority); }
		DWORD GetShuffle (void) const { return (m_pArcRoot ? m_pArcRoot->m_dwShuffle : m_dwShuffle); }
		bool HasDebrief (void) const { return !m_fNoDebrief; }
		bool HasInProgress (void) const { return !m_fNoInProgress; }
		void IncAccepted (void);
		bool KeepsStats (void) const { return !m_fNoStats; }
		void OnMissionCreated (void) { m_iExisting++; }
		void OnMissionDestroyed (void) { m_iExisting--; }
		void SetShuffle (DWORD dwValue = mathRandom()) { m_dwShuffle = dwValue; }

		//	CDesignType overrides

		static CMissionType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designMissionType) ? (CMissionType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override { if (retiMinLevel) *retiMinLevel = m_iMinLevel; if (retiMaxLevel) *retiMaxLevel = m_iMaxLevel; return (m_iMinLevel + m_iMaxLevel) / 2; }
		virtual DesignTypes GetType (void) const override { return designMissionType; }

	protected:
		//	CDesignType overrides

		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual ICCItemPtr OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const override;
		virtual ALERROR OnPrepareBindDesign (SDesignLoadCtx &Ctx) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		bool FireCanCreate (CSpaceObject *pOwner, ICCItem *pCreateData) const;

		static ICCItemPtr AutoAcceptAsItem (EMissionAutoAccept iAutoAccept);
		static EMissionAutoAccept ParseAutoAccept (SDesignLoadCtx &Ctx, const CString &sValue);
		static bool ParseMissionArc (SDesignLoadCtx &Ctx, const CString &sValue, CString *retsArc = NULL, int *retiSequence = NULL, CString *retsError = NULL);

		//	Basic properties

		CString m_sName;					//	Internal name
		CMissionType *m_pArcRoot = NULL;	//	First mission in arc (NULL if not part of mission arc)
		CString m_sArc;						//	For related missions
		int m_iArcSequence = -1;			//	Missions assigned in this order (lower numbers first)
		int m_iPriority;					//	Relative priority (default = 1)
		EMissionAutoAccept m_iAutoAccept = EMissionAutoAccept::none;

		//	Mission creation

		int m_iMinLevel;					//	Minimum system level supported
		int m_iMaxLevel;					//	Maximum system level supported
		DiceRange m_MaxAppearing;			//	Limit to number of times mission has been accepted by player
											//		(NULL if no limit)
		int m_iExpireTime;					//	Mission expires after this amount
											//		of time if not accepted.
		int m_iFailIfOutOfSystem;			//	If player is out of mission system for this amount of time
											//		(in ticks) then mission fails (-1 means on timeout)

		//	Mission stats

		int m_iMaxAppearing;				//	Limit to number of times mission can appear (-1 = no limit)
		int m_iExisting;					//	How many missions of this type currently exist.
											//		NOTE: This number can go up and down. If we create a mission
											//		the number goes up, but if we later destroy the mission
											//		(perhaps because it expired) then the count drops.
		int m_iAccepted;					//	Number of times player has accepted this mission type
		DWORD m_dwLastAcceptedOn = 0;		//	Latest tick when we accepted this mission (0 = never)

		//	Events

		TEventHandlerCache<ECachedHandlers, evtCount> m_CachedEvents;

		DWORD m_fNoFailureOnOwnerDestroyed:1;	//	If TRUE, mission does not fail when owner destroyed
		DWORD m_fNoDebrief:1;				//	If TRUE, mission is closed on success
		DWORD m_fNoStats:1;					//	If TRUE, mission is not included in count of missions assigned/completed
		DWORD m_fCloseIfOutOfSystem:1;		//	If TRUE, mission is closed if player leaves system.
		DWORD m_fForceUndockAfterDebrief:1;	//	If TRUE, default mission screen undocks after debrief
		DWORD m_fAllowDelete:1;				//	If TRUE, player can delete mission
		DWORD m_fRecordNonPlayer:1;			//	If TRUE, non-player missions will not be deleted after completion
		DWORD m_fDestroyOnDecline:1;		//	If TRUE, destroy mission if player declines.

		DWORD m_fNoInProgress:1;			//	If TRUE, no in progress messages
		DWORD m_fSpare2:1;
		DWORD m_fSpare3:1;
		DWORD m_fSpare4:1;
		DWORD m_fSpare5:1;
		DWORD m_fSpare6:1;
		DWORD m_fSpare7:1;
		DWORD m_fSpare8:1;

		DWORD m_dwSpare:16;

		DWORD m_dwShuffle = 0;				//	Temp value used to shuffle
	};


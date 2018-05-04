//	TSEMissionType.h
//
//	Classes and functions for mission types.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CMissionType : public CDesignType
	{
	public:
		inline bool CanBeDeclined (void) const { return !m_fNoDecline; }
		inline bool CanBeDeleted (void) const { return m_fAllowDelete; }
		inline bool CanBeEncountered (void) const { return (m_iMaxAppearing == -1 || m_iAccepted < m_iMaxAppearing); }
		inline bool CleanNonPlayer(void) const { return !m_fRecordNonPlayer; }
		inline bool CloseIfOutOfSystem (void) const { return m_fCloseIfOutOfSystem; }
		inline bool FailureWhenOwnerDestroyed (void) const { return !m_fNoFailureOnOwnerDestroyed; }
		inline bool FailureWhenOutOfSystem (void) const { return (m_iFailIfOutOfSystem != -1); }
		inline bool ForceUndockAfterDebrief (void) const { return m_fForceUndockAfterDebrief; }
		inline const CString &GetName (void) const { return m_sName; }
		inline int GetExpireTime (void) const { return m_iExpireTime; }
		inline int GetOutOfSystemTimeOut (void) const { return m_iFailIfOutOfSystem; }
		inline int GetPriority (void) const { return m_iPriority; }
		inline bool HasDebrief (void) const { return !m_fNoDebrief; }
		inline void IncAccepted (void) { m_iAccepted++; }
		inline bool KeepsStats (void) const { return !m_fNoStats; }

		//	CDesignType overrides

		static CMissionType *AsType (CDesignType *pType) { return ((pType && pType->GetType() == designMissionType) ? (CMissionType *)pType : NULL); }
		virtual bool FindDataField (const CString &sField, CString *retsValue) const override;
		virtual int GetLevel (int *retiMinLevel = NULL, int *retiMaxLevel = NULL) const override { if (retiMinLevel) *retiMinLevel = m_iMinLevel; if (retiMaxLevel) *retiMaxLevel = m_iMaxLevel; return (m_iMinLevel + m_iMaxLevel) / 2; }
		virtual DesignTypes GetType (void) const override { return designMissionType; }

	protected:
		//	CDesignType overrides

		virtual ALERROR OnBindDesign (SDesignLoadCtx &Ctx) override;
		virtual ALERROR OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc) override;
		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) override;
		virtual void OnReinit (void) override;
		virtual void OnWriteToStream (IWriteStream *pStream) override;

	private:
		//	Basic properties

		CString m_sName;					//	Internal name
		int m_iPriority;					//	Relative priority (default = 1)

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
		int m_iAccepted;					//	Number of times player has accepted this mission type

		DWORD m_fNoFailureOnOwnerDestroyed:1;	//	If TRUE, mission does not fail when owner destroyed
		DWORD m_fNoDebrief:1;				//	If TRUE, mission is closed on success
		DWORD m_fNoStats:1;					//	If TRUE, mission is not included in count of missions assigned/completed
		DWORD m_fCloseIfOutOfSystem:1;		//	If TRUE, mission is closed if player leaves system.
		DWORD m_fForceUndockAfterDebrief:1;	//	If TRUE, default mission screen undocks after debrief
		DWORD m_fAllowDelete:1;				//	If TRUE, player can delete mission
		DWORD m_fNoDecline:1;				//	If TRUE, mission cannot be declined once offered.
		DWORD m_fRecordNonPlayer:1;			//	If TRUE, non-player missions will not be deleted after completion

		DWORD m_dwSpare:24;
	};


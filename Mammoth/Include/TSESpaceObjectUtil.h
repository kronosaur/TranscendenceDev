//	TSESpaceObjectUtil.h
//
//	Utilities used by the base CSpaceObject class.
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Item Event Handler

enum EItemEventDispatchTypes
	{
	dispatchFireEvent,
	dispatchCheckEnhancementLifetime,
	};

class CItemEventDispatcher
	{
	public:
		CItemEventDispatcher (void);
		~CItemEventDispatcher (void);

		void FireEvent (CSpaceObject *pSource, ECodeChainEvents iEvent)	{ if (m_pFirstEntry) FireEventFull(pSource, iEvent); }
		void FireOnDocked (CSpaceObject *pSource, CSpaceObject *pDockedAt) const;
		void FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const;
		void FireUpdateEvents (CSpaceObject *pSource) { if (m_pFirstEntry) FireUpdateEventsFull(pSource); }
		void Init (CSpaceObject *pSource);

	private:
		struct SEntry
			{
			EItemEventDispatchTypes iType;				//	Type of entry
			CItem *pItem;								//	Pointer to item

			ECodeChainEvents iEvent;					//	Event (if dispatchFireEvent)
			SEventHandlerDesc Event;					//	Code (if dispatchFireEvent)

			DWORD dwEnhancementID;						//	ID of enhancement (if relevant)

			SEntry *pNext;
			};

		void AddEntry (const CString &sEvent, EItemEventDispatchTypes iType, const SEventHandlerDesc &Event, CItem *pItem, DWORD dwEnhancementID);
		SEntry *AddEntry (void);
		void FireEventFull (CSpaceObject *pSource, ECodeChainEvents iEvent);
		void FireUpdateEventsFull (CSpaceObject *pSource);
		void Refresh (CSpaceObject *pSource, SEntry *pFirst);
		void RemoveAll (void);

		SEntry *m_pFirstEntry;
	};

class CAutoDockCalc
	{
	public:
		const CSpaceObject *GetDockObj (void) const;
		int GetDockingPortIndex (void) const { return m_iDockingPort; }
		const CVector &GetDockingPortPos (void) const { return m_vDockingPort; }
		void Update (const CSpaceObject &PlayerObj, const CSpaceObject &Source, const CVector &vPortPos, int iPortIndex, Metric rMaxDist2);

	private:
		const CSpaceObject *m_pDockingObj = NULL;	//	If non-null, nearest object to dock with
		int m_iDockingPort = -1;					//	Nearest docking port
		CVector m_vDockingPort;						//	Position of docking port (absolute)
		Metric m_rDockingPortDist2 = 0.0;			//	Distance from player to docking port
	};

class CAutoTargetCalc
	{
	public:
		static constexpr Metric MAX_DISTANCE =			30.0 * LIGHT_SECOND;

		const CSpaceObject *GetAutoTarget (void) const;
		void Init (const CSpaceObject &PlayerObj);
		bool IsAutoTargetNeeded (void) const { return (m_bNeedsAutoTarget && GetAutoTarget()); }
		bool IsPlayerTargetOutOfRange (void) const { return m_bPlayerTargetOutOfRange; }
		void Update (const CSpaceObject &PlayerObj, const CSpaceObject &Obj);

	private:
		bool m_bNeedsAutoTarget = false;				//	TRUE if player's weapon needs an autotarget
		bool m_bPlayerTargetOutOfRange = false;			//	TRUE if player's current target is unreachable
		int m_iPlayerPerception = 0;					//	Player's perception

		const CSpaceObject *m_pPlayerTarget = NULL;		//	Current player target (may be NULL)
		const CSpaceObject *m_pTargetObj = NULL;		//	If non-null, nearest possible target for player
		Metric m_rTargetDist2 = g_InfiniteDistance2;	//	Distance from player to target
		int m_iMinFireArc = 0;							//	Fire arc of primary weapon
		int m_iMaxFireArc = 0;
	};


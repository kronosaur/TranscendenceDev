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

class CAutoMiningCalc
	{
	public:
		static constexpr Metric MAX_DISTANCE =			30.0 * LIGHT_SECOND;
		static constexpr Metric MAX_DISTANCE2 =			(MAX_DISTANCE * MAX_DISTANCE);

		const CSpaceObject *GetAutoTarget (void) const;
		void Init (const CSpaceObject &PlayerObj);
		void Update (const CSpaceObject &PlayerObj, const CSpaceObject &Obj);

	private:
		void SetTargetIfBetter (const CSpaceObject &Obj, Metric rObjDist2);

		bool m_bNeedsAutoTarget = false;

		const CSpaceObject *m_pAsteroidTarget = NULL;	//	Current asteroid target (may be NULL)
		Metric m_rTargetDist2 = g_InfiniteDistance2;	//	Distance from player to target
		int m_iMinFireArc = 0;							//	Fire arc of primary weapon
		int m_iMaxFireArc = 0;
	};

class CAutoTargetCalc
	{
	public:
		static constexpr Metric MAX_DISTANCE =			30.0 * LIGHT_SECOND;
		static constexpr Metric MAX_DISTANCE2 =			(MAX_DISTANCE * MAX_DISTANCE);

		const CSpaceObject *GetAutoTarget (void) const;
		void Init (const CSpaceObject &PlayerObj);
		bool IsAutoTargetNeeded (void) const { return (m_bNeedsAutoTarget && GetAutoTarget()); }
		bool IsPlayerTargetOutOfRange (void) const { return m_bPlayerTargetOutOfRange; }
		void Update (const CSpaceObject &PlayerObj, const CSpaceObject &Obj);

	private:
		void SetTargetIfBetter (const CSpaceObject &Obj, Metric rObjDist2);

		bool m_bNeedsAutoTarget = false;				//	TRUE if player's weapon needs an autotarget
		bool m_bPlayerTargetOutOfRange = false;			//	TRUE if player's current target is unreachable
		int m_iPlayerPerception = 0;					//	Player's perception

		const CSpaceObject *m_pPlayerTarget = NULL;		//	Current player target (may be NULL)
		const CSpaceObject *m_pTargetObj = NULL;		//	If non-null, nearest possible target for player
		Metric m_rTargetDist2 = g_InfiniteDistance2;	//	Distance from player to target
		bool m_bTargetCanAttack = false;				//	TRUE if current target can attack
		int m_iMinFireArc = 0;							//	Fire arc of primary weapon
		int m_iMaxFireArc = 0;
	};

class CSpaceObjectTargetList
	{
	public:
		void CleanUp (void) { m_bValid = false; }
		void Delete (const CSpaceObject &Obj);

		static constexpr DWORD FLAG_NO_LINE_OF_FIRE_CHECK =			0x00000001;
		static constexpr DWORD FLAG_NO_RANGE_CHECK =				0x00000002;
		bool FindTargetAligned (CSpaceObject &SourceObj, const CDeviceItem &WeaponItem, CSpaceObject **retpTarget = NULL, int *retiFireAngle = NULL) const;
		bool FindTargetInRange (const CSpaceObject &SourceObj, const CDeviceItem &WeaponItem, DWORD dwFlags = 0, CSpaceObject **retpTarget = NULL, int *retiFireAngle = NULL, Metric *retrDist2 = NULL) const;

		const TArray<CSpaceObject *> &GetList (void) const { return m_List; }
		void InitEmpty (void) { m_List.DeleteAll(); m_bValid = true; }
		void InitWithNearestMissiles (CSpaceObject &SourceObj, int iMaxTargets, Metric rMaxDist, DWORD dwFlags);
		void InitWithNearestTargetableMissiles (CSpaceObject &SourceObj, int iMaxTargets, Metric rMaxDist, DWORD dwFlags);

		static constexpr DWORD FLAG_INCLUDE_NON_AGGRESSORS =		0x00000010;
		static constexpr DWORD FLAG_INCLUDE_STATIONS =				0x00000020;
		static constexpr DWORD FLAG_INCLUDE_PLAYER =				0x00000040;
		static constexpr DWORD FLAG_INCLUDE_SOURCE_TARGET =			0x00000080;
		void InitWithNearestVisibleEnemies (CSpaceObject &SourceObj, int iMaxTargets, Metric rMaxDist, CSpaceObject *pExcludeObj, DWORD dwFlags);

		bool IsEmpty (void) const { return (!m_bValid || m_List.GetCount() == 0); }
		bool IsValid (void) const { return m_bValid; }
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (CSystem &System, IWriteStream &Stream) const;

	private:
		static constexpr DWORD SPECIAL_INVALID_COUNT = 0xffffffff;

		TArray<CSpaceObject *> m_List;
		bool m_bValid = false;
	};

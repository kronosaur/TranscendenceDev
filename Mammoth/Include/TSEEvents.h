//	TSEEvents.h
//
//	Classes and functions for system events
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CSystemEvent
	{
	public:
		enum Classes
			{
			//	NOTE: These values are stored in the save file
			cTimedEncounterEvent =			0,
			cTimedCustomEvent =				1,
			cTimedRecurringEvent =			2,
			cTimedTypeEvent =				3,
			cTimedMissionEvent =			4,
			cRangeTypeEvent =				5,
			};

		CSystemEvent (DWORD dwTick) : m_dwTick(dwTick), m_bDestroyed(false) { }
		CSystemEvent (SLoadCtx &Ctx);
		virtual ~CSystemEvent (void) { }

		static void CreateFromStream (SLoadCtx &Ctx, CSystemEvent **retpEvent);

		DWORD GetTick (void) { return m_dwTick; }
		bool IsDestroyed (void) { return m_bDestroyed; }
		bool IsEqual (CSystemEvent &Src) const;
		void SetDestroyed (void) { m_bDestroyed = true; }
		void SetTick (DWORD dwTick) { m_dwTick = dwTick; }
		void WriteToStream (CSystem *pSystem, IWriteStream *pStream);

		virtual CString DebugCrashInfo (void) { return NULL_STR; }
		virtual void DoEvent (DWORD dwTick, CSystem &System) = 0;
		virtual CString GetEventHandlerName (void) { return NULL_STR; }
		virtual CSpaceObject *GetEventHandlerObj (void) { return NULL; }
		virtual CDesignType *GetEventHandlerType (void) { return NULL; }
		virtual bool OnObjChangedSystems (const CSpaceObject &Obj) const { return false; }
		virtual bool OnObjDestroyed (CSpaceObject *pObj) { return false; }
		virtual void OnPlayerChangedShips (CSpaceObject &OldShip, CSpaceObject &NewShip, SPlayerChangedShipsCtx &Options) { }
		virtual bool OnStationDestroyed (CSpaceObject *pObj) { return false; }

	protected:
		virtual Classes GetClass (void) const = 0;
		virtual bool OnIsEqual (CSystemEvent &Src) const = 0;
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) = 0;

	private:
		DWORD m_dwTick;
		bool m_bDestroyed;
	};

class CSystemEventList
	{
	public:
		~CSystemEventList (void);

		void AddEvent (CSystemEvent *pEvent);
		bool CancelEvent (CSpaceObject *pObj, bool bInDoEvent);
		bool CancelEvent (CSpaceObject *pObj, const CString &sEvent, bool bInDoEvent);
		bool CancelEvent (CDesignType *pType, const CString &sEvent, bool bInDoEvent);
		void DeleteAll (void);
		TArray<CSystemEvent *> FindEventsForObj (const CSpaceObject &Obj) const;
		int GetCount (void) const { return m_List.GetCount(); }
		CSystemEvent *GetEvent (int iIndex) const { return m_List[iIndex]; }
		void MoveEvent (int iIndex, CSystemEventList &Dest) { Dest.AddEvent(m_List[iIndex]); m_List.Delete(iIndex); }
		void MoveEventForObjTo (const CSpaceObject &Obj, CSystemEventList &Dest);
		void OnObjDestroyed (CSpaceObject *pObj);
		void OnPlayerChangedShips (CSpaceObject &OldShip, CSpaceObject &NewShip, SPlayerChangedShipsCtx &Options);
		void OnStationDestroyed (CSpaceObject *pObj);
		void ReadFromStream (SLoadCtx &Ctx);
		void RemoveEvent (int iIndex) { delete m_List[iIndex]; m_List.Delete(iIndex); }
		void Update (DWORD dwTick, CSystem &System);
		void WriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		bool FindEvent (CSystemEvent &Src, int *retiIndex = NULL) const;

		TArray<CSystemEvent *> m_List;
	};


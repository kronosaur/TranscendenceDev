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
			};

		CSystemEvent (DWORD dwTick) : m_dwTick(dwTick), m_bDestroyed(false) { }
		virtual ~CSystemEvent (void) { }
		static void CreateFromStream (SLoadCtx &Ctx, CSystemEvent **retpEvent);

		inline DWORD GetTick (void) { return m_dwTick; }
		inline bool IsDestroyed (void) { return m_bDestroyed; }
		inline void SetDestroyed (void) { m_bDestroyed = true; }
		inline void SetTick (DWORD dwTick) { m_dwTick = dwTick; }
		void WriteToStream (CSystem *pSystem, IWriteStream *pStream);

		virtual CString DebugCrashInfo (void) { return NULL_STR; }
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem) = 0;
		virtual CString GetEventHandlerName (void) { return NULL_STR; }
		virtual CSpaceObject *GetEventHandlerObj (void) { return NULL; }
		virtual CDesignType *GetEventHandlerType (void) { return NULL; }
		virtual bool OnObjChangedSystems (CSpaceObject *pObj) { return false; }
		virtual bool OnObjDestroyed (CSpaceObject *pObj) { return false; }

	protected:
		CSystemEvent (void) { }

		virtual void OnReadFromStream (SLoadCtx &Ctx) = 0;
		virtual void OnWriteClassToStream (IWriteStream *pStream) = 0;
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) = 0;

	private:
		DWORD m_dwTick;
		bool m_bDestroyed;
	};

class CSystemEventList
	{
	public:
		~CSystemEventList (void);

		inline void AddEvent (CSystemEvent *pEvent) { m_List.Insert(pEvent); }
		bool CancelEvent (CSpaceObject *pObj, bool bInDoEvent);
		bool CancelEvent (CSpaceObject *pObj, const CString &sEvent, bool bInDoEvent);
		bool CancelEvent (CDesignType *pType, const CString &sEvent, bool bInDoEvent);
		void DeleteAll (void);
		inline int GetCount (void) const { return m_List.GetCount(); }
		inline CSystemEvent *GetEvent (int iIndex) const { return m_List[iIndex]; }
		inline void MoveEvent (int iIndex, CSystemEventList &Dest) { Dest.AddEvent(m_List[iIndex]); m_List.Delete(iIndex); }
		void ReadFromStream (SLoadCtx &Ctx);
		inline void RemoveEvent (int iIndex) { delete m_List[iIndex]; m_List.Delete(iIndex); }
		void Update (DWORD dwTick, CSystem *pSystem);
		void WriteToStream (CSystem *pSystem, IWriteStream *pStream);

	private:
		TArray<CSystemEvent *> m_List;
	};

//	Event Implementations ------------------------------------------------------

class CRangeTypeEvent : public CSystemEvent
	{
	};

class CTimedEncounterEvent : public CSystemEvent
	{
	public:
		CTimedEncounterEvent (void) { } //	Used only for loading
		CTimedEncounterEvent (int iTick,
							  CSpaceObject *pTarget,
							  DWORD dwEncounterTableUNID,
							  CSpaceObject *pGate,
							  Metric rDistance);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem) override;
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteClassToStream (IWriteStream *pStream) override;
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		CSpaceObject *m_pTarget;
		DWORD m_dwEncounterTableUNID;
		CSpaceObject *m_pGate;
		Metric m_rDistance;
	};

class CTimedCustomEvent : public CSystemEvent
	{
	public:
		CTimedCustomEvent (void) { }	//	Used only for loading
		CTimedCustomEvent (int iTick,
						   CSpaceObject *pObj,
						   const CString &sEvent);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CSpaceObject *GetEventHandlerObj (void) override { return m_pObj; }
		virtual bool OnObjChangedSystems (CSpaceObject *pObj) override;
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteClassToStream (IWriteStream *pStream) override;
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		CSpaceObject *m_pObj;
		CString m_sEvent;
	};

class CTimedRecurringEvent : public CSystemEvent
	{
	public:
		CTimedRecurringEvent (void) { }	//	Used only for loading
		CTimedRecurringEvent (int iInterval,
							  CSpaceObject *pObj,
							  const CString &sEvent);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CSpaceObject *GetEventHandlerObj (void) override { return m_pObj; }
		virtual bool OnObjChangedSystems (CSpaceObject *pObj) override;
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteClassToStream (IWriteStream *pStream) override;
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		int m_iInterval;
		CSpaceObject *m_pObj;
		CString m_sEvent;
	};

class CTimedTypeEvent : public CSystemEvent
	{
	public:
		CTimedTypeEvent (void) { }	//	Used only for loading
		CTimedTypeEvent (int iTick,
						 int iInterval,
						 CDesignType *pType,
						 const CString &sEvent);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CDesignType *GetEventHandlerType (void) override { return m_pType; }

	protected:
		virtual void OnReadFromStream (SLoadCtx &Ctx) override;
		virtual void OnWriteClassToStream (IWriteStream *pStream) override;
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		int m_iInterval;			//	0 = not recurring
		CDesignType *m_pType;
		CString m_sEvent;
	};


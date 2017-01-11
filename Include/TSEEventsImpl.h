//	TSEEventsImpl.h
//
//	Classes and functions for system events
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CRangeTypeEvent : public CSystemEvent
	{
	public:
		CRangeTypeEvent (CDesignType *pType, const CString &sEvent, CSpaceObject *pCenter, Metric rRadius, const CString &sCriteria);
		CRangeTypeEvent (CDesignType *pType, const CString &sEvent, const CVector &vCenter, Metric rRadius, const CString &sCriteria);
		CRangeTypeEvent (SLoadCtx &Ctx);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CDesignType *GetEventHandlerType (void) override { return m_pType; }
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;

	protected:
		virtual Classes GetClass (void) const override { return cRangeTypeEvent; }
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		CDesignType *m_pType;
		CString m_sEvent;
		CSpaceObject *m_pCenter;			//	Center. If NULL, use m_vCenter instead.
		CVector m_vCenter;					//	Center. Only if m_pCenter is NULL.
		Metric m_rRadius;					//	Radius from center.
		CString m_sCriteria;				//	Match these objects (if empty, match player ship)
		
		Metric m_rRadius2;					//	Cached for efficiency.
		CSpaceObject::Criteria m_Criteria;	//	Parsed criteria (cached)
		bool m_bCriteriaInit;				//	TRUE if we've parsed the criteria
	};

class CTimedEncounterEvent : public CSystemEvent
	{
	public:
		CTimedEncounterEvent (int iTick,
							  CSpaceObject *pTarget,
							  DWORD dwEncounterTableUNID,
							  CSpaceObject *pGate,
							  Metric rDistance);
		CTimedEncounterEvent (SLoadCtx &Ctx);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem) override;
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;

	protected:
		virtual Classes GetClass (void) const override { return cTimedEncounterEvent; }
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
		CTimedCustomEvent (int iTick,
						   CSpaceObject *pObj,
						   const CString &sEvent);
		CTimedCustomEvent (SLoadCtx &Ctx);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CSpaceObject *GetEventHandlerObj (void) override { return m_pObj; }
		virtual bool OnObjChangedSystems (CSpaceObject *pObj) override;
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;

	protected:
		virtual Classes GetClass (void) const override { return cTimedCustomEvent; }
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		CSpaceObject *m_pObj;
		CString m_sEvent;
	};

class CTimedRecurringEvent : public CSystemEvent
	{
	public:
		CTimedRecurringEvent (int iInterval,
							  CSpaceObject *pObj,
							  const CString &sEvent);
		CTimedRecurringEvent (SLoadCtx &Ctx);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CSpaceObject *GetEventHandlerObj (void) override { return m_pObj; }
		virtual bool OnObjChangedSystems (CSpaceObject *pObj) override;
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;

	protected:
		virtual Classes GetClass (void) const override { return cTimedRecurringEvent; }
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		int m_iInterval;
		CSpaceObject *m_pObj;
		CString m_sEvent;
	};

class CTimedTypeEvent : public CSystemEvent
	{
	public:
		CTimedTypeEvent (int iTick,
						 int iInterval,
						 CDesignType *pType,
						 const CString &sEvent);
		CTimedTypeEvent (SLoadCtx &Ctx);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem *pSystem) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CDesignType *GetEventHandlerType (void) override { return m_pType; }

	protected:
		virtual Classes GetClass (void) const override { return cTimedTypeEvent; }
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		int m_iInterval;			//	0 = not recurring
		CDesignType *m_pType;
		CString m_sEvent;
	};

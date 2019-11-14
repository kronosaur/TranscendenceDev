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
		virtual void DoEvent (DWORD dwTick, CSystem &System) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CDesignType *GetEventHandlerType (void) override { return m_pType; }
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;
		virtual void OnPlayerChangedShips (CSpaceObject &OldShip, CSpaceObject &NewShip, SPlayerChangedShipsCtx &Options) override;

	protected:
		virtual Classes GetClass (void) const override { return cRangeTypeEvent; }
		virtual bool OnIsEqual (CSystemEvent &Src) const override;
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		CDesignType *m_pType;
		CString m_sEvent;
		CSpaceObject *m_pCenter;			//	Center. If NULL, use m_vCenter instead.
		CVector m_vCenter;					//	Center. Only if m_pCenter is NULL.
		Metric m_rRadius;					//	Radius from center.
		CString m_sCriteria;				//	Match these objects (if empty, match player ship)
		
		Metric m_rRadius2;					//	Cached for efficiency.
		CSpaceObjectCriteria m_Criteria;	//	Parsed criteria (cached)
		bool m_bCriteriaInit;				//	TRUE if we've parsed the criteria
	};

class CTimedEncounterEvent : public CSystemEvent
	{
	public:
		CTimedEncounterEvent (int iTick,
							  CSpaceObject *pTarget,
							  DWORD dwEncounterTableUNID,
							  CSpaceObject *pGate,
							  const CVector &vPos,
							  Metric rDistance);
		CTimedEncounterEvent (int iTick,
							  const CSpaceObjectList &Targets,
							  DWORD dwEncounterTableUNID,
							  CSpaceObject *pGate,
							  const CVector &vPos,
							  Metric rDistance);
		CTimedEncounterEvent (SLoadCtx &Ctx);

		virtual CString DebugCrashInfo (void) override;
		virtual void DoEvent (DWORD dwTick, CSystem &System) override;
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;
		virtual void OnPlayerChangedShips (CSpaceObject &OldShip, CSpaceObject &NewShip, SPlayerChangedShipsCtx &Options) override;
		virtual bool OnStationDestroyed (CSpaceObject *pObj) override;

	protected:
		virtual Classes GetClass (void) const override { return cTimedEncounterEvent; }
		virtual bool OnIsEqual (CSystemEvent &Src) const override { return false; }
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		CVector CalcEncounterPos (CSpaceObject *pTarget, Metric rDistance) const;

		CSpaceObjectList m_Targets;
		DWORD m_dwEncounterTableUNID;

		CSpaceObject *m_pGate;
		CVector m_vPos;
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
		virtual void DoEvent (DWORD dwTick, CSystem &System) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CSpaceObject *GetEventHandlerObj (void) override { return m_pObj; }
		virtual bool OnObjChangedSystems (CSpaceObject *pObj) override;
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;

	protected:
		virtual Classes GetClass (void) const override { return cTimedCustomEvent; }
		virtual bool OnIsEqual (CSystemEvent &Src) const override;
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
		virtual void DoEvent (DWORD dwTick, CSystem &System) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CSpaceObject *GetEventHandlerObj (void) override { return m_pObj; }
		virtual bool OnObjChangedSystems (CSpaceObject *pObj) override;
		virtual bool OnObjDestroyed (CSpaceObject *pObj) override;

	protected:
		virtual Classes GetClass (void) const override { return cTimedRecurringEvent; }
		virtual bool OnIsEqual (CSystemEvent &Src) const override;
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
		virtual void DoEvent (DWORD dwTick, CSystem &System) override;
		virtual CString GetEventHandlerName (void) override { return m_sEvent; }
		virtual CDesignType *GetEventHandlerType (void) override { return m_pType; }

	protected:
		virtual Classes GetClass (void) const override { return cTimedTypeEvent; }
		virtual bool OnIsEqual (CSystemEvent &Src) const override;
		virtual void OnWriteToStream (CSystem *pSystem, IWriteStream *pStream) override;

	private:
		int m_iInterval;			//	0 = not recurring
		CDesignType *m_pType;
		CString m_sEvent;
	};

//	CRangeTypeEvent.cpp
//
//	CRangeTypeEvent class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CRangeTypeEvent::CRangeTypeEvent (CDesignType *pType, const CString &sEvent, CSpaceObject *pCenter, Metric rRadius, const CString &sCriteria) : CSystemEvent(0)

//	CRangeTypeEvent constructor

	{
	}

CRangeTypeEvent::CRangeTypeEvent (CDesignType *pType, const CString &sEvent, const CVector &vCenter, Metric rRadius, const CString &sCriteria) : CSystemEvent(0)

//	CRangeTypeEvent constructor

	{
	}

CRangeTypeEvent::CRangeTypeEvent (SLoadCtx &Ctx) : CSystemEvent(Ctx)

//	CRangeTypeEvent constructor

	{
	}

CString CRangeTypeEvent::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Output debug info

	{
	CString sResult;

	sResult.Append(CONSTLIT("CRangeTypeEvent\r\n"));
	return sResult;
	}

void CRangeTypeEvent::DoEvent (DWORD dwTick, CSystem *pSystem)

//	DoEvent
//
//	Called on tick expiration

	{
	}

void CRangeTypeEvent::OnWriteToStream (CSystem *pSystem, IWriteStream *pStream)

//	OnWriteToStream
//
//	Write our data.

	{
	}

//	CCrewAI.cpp
//
//	CCrewAI class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CCrewAI::CCrewAI (void)

//	CCrewAI constructor

	{
	}

CString CCrewAI::DebugCrashInfo (void)

//	DebugCrashInfo
//
//	Output crash information

	{
	CString sResult;

	sResult.Append(CONSTLIT("CCrewAI\r\n"));

	return sResult;
	}

void CCrewAI::OnObjDestroyedNotify (const SDestroyCtx &Ctx)

//	OnObjDestroyedNotify
//
//	An object has been destroyed.

	{
	}

void CCrewAI::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read from stream

	{
	}

void CCrewAI::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write to stream

	{
	}

//	IPlayerController.cpp
//
//	IPlayerController class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

IPlayerController::IPlayerController (void) :
		m_pSovereign(NULL)

//	IPlayerController constructor

	{
	}

CSovereign *IPlayerController::GetSovereign (void) const

//	GetSovereign
//
//	Returns the player's sovereign

	{
	if (m_pSovereign == NULL)
		{
		m_pSovereign = g_pUniverse->FindSovereign(g_PlayerSovereignUNID);
		if (m_pSovereign == NULL)
			kernelDebugLogMessage("ERROR: Unable to find player sovereign");
		}

	return m_pSovereign;
	}

void IPlayerController::ReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from stream

	{
	OnReadFromStream(Ctx);
	}

void IPlayerController::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write to stream

	{
	OnWriteToStream(pStream);
	}

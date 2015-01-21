//	IPlayerController.cpp
//
//	IPlayerController class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

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

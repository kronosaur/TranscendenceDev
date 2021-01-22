//	CAIShipControls.cpp
//
//	CAIShipControls class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

CAIShipControls::CAIShipControls (void) : 
		m_iManeuver(NoRotation),
		m_iThrustDir(constNeverThrust)

//	CAIShipControls constructor

	{
	}

void CAIShipControls::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Load from stream

	{
	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);
	m_iManeuver = (EManeuverTypes)dwLoad;

	Ctx.pStream->Read(m_iThrustDir);
	}

void CAIShipControls::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write to stream

	{
	pStream->Write((DWORD)m_iManeuver);
	pStream->Write(m_iThrustDir);
	}

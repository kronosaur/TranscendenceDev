//	C3DObjectPos.cpp
//
//	C3DObjectPos class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void C3DObjectPos::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	DWORD		m_iPosAngle, m_iPosRadius
//	DWORD		m_iPosZ, unused

	{
	DWORD dwLoad;
	Ctx.pStream->Read(dwLoad);
	m_iPosAngle = (int)LOWORD(dwLoad);
	m_iPosRadius = (int)HIWORD(dwLoad);

	Ctx.pStream->Read(dwLoad);
	m_iPosZ = (int)LOWORD(dwLoad);
	}

void C3DObjectPos::WriteToStream (IWriteStream &Stream) const

//	WriteToStream
//
//	DWORD		m_iPosAngle, m_iPosRadius
//	DWORD		m_iPosZ, unused

	{
	DWORD dwSave = MAKELONG(m_iPosAngle, m_iPosRadius);
	Stream.Write(dwSave);

	dwSave = MAKELONG(m_iPosZ, 0);
	Stream.Write(dwSave);
	}

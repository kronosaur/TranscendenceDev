//	CSquadronID.cpp
//
//	CSquadronID class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const CSquadronID CSquadronID::Null;

void CSquadronID::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	DWORD			m_dwLeaderID
//	CString			m_sID

	{
	Ctx.pStream->Read(m_dwLeaderID);
	m_sID.ReadFromStream(Ctx.pStream);
	}

void CSquadronID::WriteToStream (IWriteStream &Stream)

//	WriteToStream
//
//	DWORD			m_dwLeaderID
//	CString			m_sID

	{
	Stream.Write(m_dwLeaderID);
	m_sID.WriteToStream(&Stream);
	}

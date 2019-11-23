//	CLocationDef.cpp
//
//	CLocationDef class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

bool CLocationDef::CanBeBlocked (void) const

//	CanBeBlocked
//
//	Returns TRUE if we can be blocked by other location that is too close to us.

	{
	//	Can't be blocked if we're already full or if we're a required location.

	if (!IsEmpty() || IsRequired())
		return false;

	//	Otherwise, we can be blocked.

	else
		return true;
	}

void CLocationDef::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	CString			m_sID
//	COrbit			m_OrbitDesc
//	CString			m_sAttributes
//	DWORD			m_dwObjID
//	DWORD			flags

	{
	DWORD dwLoad;

	m_sID.ReadFromStream(Ctx.pStream);
	m_OrbitDesc.ReadFromStream(Ctx);
	m_sAttributes.ReadFromStream(Ctx.pStream);

	Ctx.pStream->Read((char *)&m_dwObjID, sizeof(DWORD));

	//	Flags

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_bBlocked = ((dwLoad & 0x00000001) ? true : false);
	}

void CLocationDef::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	CString			m_sID
//	COrbit			m_OrbitDesc
//	CString			m_sAttributes
//	DWORD			m_dwObjID
//	DWORD			flags

	{
	DWORD dwSave;

	m_sID.WriteToStream(pStream);
	m_OrbitDesc.WriteToStream(*pStream);
	m_sAttributes.WriteToStream(pStream);

	pStream->Write((char *)&m_dwObjID, sizeof(DWORD));

	//	Flags

	dwSave = 0;
	dwSave |= (m_bBlocked ? 0x00000001 : 0);
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}

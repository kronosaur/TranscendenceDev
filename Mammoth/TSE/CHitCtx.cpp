//	CHitCtx.cpp
//
//	CHitCtx class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CHitCtx::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	CSpaceObject		m_pHit
//	CVector				m_vHitPos
//	DWORD				m_iHitDir

	{
	CSystem::ReadObjRefFromStream(Ctx, &m_pHit);
	Ctx.pStream->Read((char *)&m_vHitPos, sizeof(CVector));
	Ctx.pStream->Read((char *)&m_iHitDir, sizeof(DWORD));
	}

void CHitCtx::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	CSpaceObject		m_pHit
//	CVector				m_vHitPos
//	DWORD				m_iHitDir

	{
	CSystem::WriteObjRefToStream(*pStream, m_pHit);
	pStream->Write((char *)&m_vHitPos, sizeof(CVector));
	pStream->Write((char *)&m_iHitDir, sizeof(DWORD));
	}

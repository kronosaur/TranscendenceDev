//	CObjectJoint.cpp
//
//	CObjectJoint class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define TYPE_HINGE						CONSTLIT("hinge")
#define TYPE_ROD						CONSTLIT("rod")

CObjectJoint::CObjectJoint (ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo) :
		m_iType(iType),
		m_dwID(g_pUniverse->CreateGlobalID()),
		m_iTick(0),
		m_iLifetime(-1),
		m_iMaxLength(0)

//	CObjectJoint constructor

	{
	m_P1.pObj = pFrom;
	m_P2.pObj = pTo;
	}

void CObjectJoint::CreateFromStream (SLoadCtx &Ctx, CObjectJoint **retpJoint)

//	CreateFromStream
//
//	Creates a new joint from a stream.

	{
	CObjectJoint *pJoint = new CObjectJoint;

	int iType;
	Ctx.pStream->Read(iType);
	pJoint->m_iType = (ETypes)iType;

	Ctx.pStream->Read(pJoint->m_dwID);
	Ctx.pStream->Read(pJoint->m_iTick);
	Ctx.pStream->Read(pJoint->m_iLifetime);
	Ctx.pStream->Read(pJoint->m_iMaxLength);

	ReadFromStream(Ctx, pJoint->m_P1);
	ReadFromStream(Ctx, pJoint->m_P2);
	}

CObjectJoint::ETypes CObjectJoint::ParseType (const CString &sValue)

//	ParseType
//
//	Returns a type

	{
	if (strEquals(sValue, TYPE_HINGE))
		return jointHinge;
	else if (strEquals(sValue, TYPE_ROD))
		return jointRod;
	else
		return jointNone;
	}

void CObjectJoint::ReadFromStream (SLoadCtx &Ctx, SAttachPoint &Point)

//	ReadFromStream
//
//	Reads the attach point

	{
	DWORD dwLoad;

	Ctx.pSystem->ReadObjRefFromStream(Ctx, &Point.pObj);

	Point.Pos.ReadFromStream(Ctx);

	Ctx.pStream->Read(dwLoad);
	Point.iMinArc = (int)LOWORD(dwLoad);
	Point.iMaxArc = (int)HIWORD(dwLoad);
	}

void CObjectJoint::SetNextJoint (CSpaceObject *pObj, CObjectJoint *pNext)

//	SetNextJoint
//
//	Sets the next joint for the given object.

	{
	if (m_P1.pObj == pObj)
		m_P1.pNext = pNext;
	else if (m_P2.pObj == pObj)
		m_P2.pNext = pNext;
	}

void CObjectJoint::WriteToStream (CSystem *pSystem, IWriteStream &Stream)

//	WriteToStream
//
//	Writes out the joint to the stream.
//
//	DWORD			m_iType
//	DWORD			m_dwID
//	DWORD			m_iTick
//	DWORD			m_iLifetime
//	
//	DWORD			m_iMaxLength
//
//	DWORD			m_P1.pObj
//	C3DObjectPos	m_P1.Pos
//	DWORD			m_P1.iMinArc and m_P1.iMaxArc
//
//	DWORD			m_P2.pObj
//	C3DObjectPos	m_P2.Pos
//	DWORD			m_P2.iMinArc and m_P1.iMaxArc

	{
	Stream.Write((int)m_iType);
	Stream.Write(m_dwID);
	Stream.Write(m_iTick);
	Stream.Write(m_iLifetime);
	Stream.Write(m_iMaxLength);

	WriteToStream(pSystem, Stream, m_P1);
	WriteToStream(pSystem, Stream, m_P2);
	}

void CObjectJoint::WriteToStream (CSystem *pSystem, IWriteStream &Stream, const SAttachPoint &Point)

//	WriteToStream
//
//	Writes an attach point

	{
	pSystem->WriteObjRefToStream(Point.pObj, &Stream);

	Point.Pos.WriteToStream(Stream);

	DWORD dwSave = MAKELONG(Point.iMinArc, Point.iMaxArc);
	Stream.Write(dwSave);
	}

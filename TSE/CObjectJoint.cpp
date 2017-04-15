//	CObjectJoint.cpp
//
//	CObjectJoint class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define FIELD_LENGTH					CONSTLIT("length")
#define FIELD_LIMIT1					CONSTLIT("limit1")
#define FIELD_LIMIT2					CONSTLIT("limit2")
#define FIELD_POS1						CONSTLIT("pos1")
#define FIELD_POS2						CONSTLIT("pos2")

#define TYPE_HINGE						CONSTLIT("hinge")
#define TYPE_ROD						CONSTLIT("rod")

CObjectJoint::CObjectJoint (ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo) :
		m_iType(iType),
		m_dwID(g_pUniverse->CreateGlobalID()),
		m_iTick(0),
		m_iLifetime(-1),
		m_iMaxLength(0),
		m_fDestroyed(false),
		m_fPaintNeeded(false)

//	CObjectJoint constructor

	{
	m_P1.pObj = pFrom;
	m_P2.pObj = pTo;

	//	Some defaults

	switch (m_iType)
		{
		case jointRod:
			m_iMaxLength = mathRound(pFrom->GetDistance(pTo) / g_KlicksPerPixel);
			break;
		}
	}

void CObjectJoint::AddContacts (CPhysicsContactResolver &Resolver)

//	AddContacts
//
//	Adds contacts

	{
	switch (m_iType)
		{
		case jointHinge:
			Resolver.AddRod(m_P1.pObj, m_P2.pObj, 0.0);
			break;

		case jointRod:
			Resolver.AddRod(m_P1.pObj, m_P2.pObj, m_iMaxLength * g_KlicksPerPixel);
			break;
		}
	}

void CObjectJoint::ApplyOptions (ICCItem *pOptions)

//	ApplyOptions
//
//	We accept the following options:
//
//	length: length in pixels
//	pos1: vector or { angle:n radius:n z:n }
//	pos2: vector or { angle:n radius:n z:n }
//	limit1: { min:n max:n }
//	limit2: { min:n max:n }

	{
	CCodeChainCtx Ctx;

	if (pOptions == NULL)
		return;

	//	Length

	ICCItem *pLength = pOptions->GetElement(FIELD_LENGTH);
	if (pLength)
		m_iMaxLength = Max(0, pLength->GetIntegerValue());

	//	Position

	ICCItem *pPos = pOptions->GetElement(FIELD_POS1);
	if (pPos)
		m_P1.Pos = Ctx.As3DObjectPos(m_P1.pObj, pPos);

	pPos = pOptions->GetElement(FIELD_POS2);
	if (pPos)
		m_P2.Pos = Ctx.As3DObjectPos(m_P2.pObj, pPos);

	//	Limits

	ICCItem *pLimits = pOptions->GetElement(FIELD_LIMIT1);
	if (pLimits && !pLimits->IsNil())
		{
		int iMinArc, iMaxArc;
		Ctx.AsArc(pLimits, &iMinArc, &iMaxArc);

		m_P1.iMinArc = iMinArc;
		m_P1.iMaxArc = iMaxArc;
		}

	pLimits = pOptions->GetElement(FIELD_LIMIT2);
	if (pLimits && !pLimits->IsNil())
		{
		int iMinArc, iMaxArc;
		Ctx.AsArc(pLimits, &iMinArc, &iMaxArc);

		m_P2.iMinArc = iMinArc;
		m_P2.iMaxArc = iMaxArc;
		}
	}

void CObjectJoint::CreateFromStream (SLoadCtx &Ctx, CObjectJoint **retpJoint)

//	CreateFromStream
//
//	Creates a new joint from a stream.

	{
	CObjectJoint *pJoint = new CObjectJoint(jointNone, NULL, NULL);

	int iType;
	Ctx.pStream->Read(iType);
	pJoint->m_iType = (ETypes)iType;

	Ctx.pStream->Read(pJoint->m_dwID);
	Ctx.pStream->Read(pJoint->m_iTick);
	Ctx.pStream->Read(pJoint->m_iLifetime);
	Ctx.pStream->Read(pJoint->m_iMaxLength);

	ReadFromStream(Ctx, pJoint->m_P1);
	ReadFromStream(Ctx, pJoint->m_P2);

	*retpJoint = pJoint;
	}

void CObjectJoint::Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const

//	Paint
//
//	Paint the joint

	{
	switch (m_iType)
		{
		case jointRod:
			{
			int xFrom, yFrom;
			Ctx.XForm.Transform(m_P1.pObj->GetPos(), &xFrom, &yFrom);

			int xTo, yTo;
			Ctx.XForm.Transform(m_P2.pObj->GetPos(), &xTo, &yTo);

			Dest.DrawLine(xFrom, yFrom, xTo, yTo, 2, CG32bitPixel(255, 255, 128));
			break;
			}
		}
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

void CObjectJoint::SetObjListPaintNeeded (CSpaceObject *pObj, bool bValue)

//	SetObjListPaintNeeded
//
//	Sets painte needed flag for all joints for the given object.

	{
	CObjectJoint *pNext = this;
	while (pNext)
		{
		pNext->SetPaintNeeded(bValue);
		pNext = pNext->GetNextJoint(pObj);
		}
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

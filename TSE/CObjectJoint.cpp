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
#define TYPE_SPINE						CONSTLIT("spine")

const Metric SPINE_MAX_DIST2 =			(0.0 * g_KlicksPerPixel * g_KlicksPerPixel);
const Metric SPINE_SPRING_K =			0.05;

CObjectJoint::CObjectJoint (ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo) :
		m_iType(iType),
		m_dwID(g_pUniverse->CreateGlobalID()),
		m_iTick(0),
		m_iLifetime(-1),
		m_iMaxLength(0),
		m_fDestroyed(false),
		m_fPaintNeeded(false),
		m_fShipCompartment(false)

//	CObjectJoint constructor

	{
	m_P1.pObj = pFrom;
	m_P2.pObj = pTo;

	//	If no type, then we don't initialize anything else. This happens when
	//	loading from a stream.

	if (iType == jointNone)
		return;

	//	Set some flags

	ASSERT(pFrom && pTo);
	m_fShipCompartment = (pFrom->IsAttached() || pTo->IsAttached());

	//	Some defaults

	switch (m_iType)
		{
		case jointRod:
		case jointSpine:
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
		case jointSpine:
			Resolver.AddRod(m_P1.pObj, m_P2.pObj, m_iMaxLength * g_KlicksPerPixel);
			break;
		}
	}

void CObjectJoint::AddForces (void)

//	AddForces
//
//	Adds forces on the objects caused by the joint.

	{
	switch (m_iType)
		{
		case jointSpine:
			{
			//	For spine joints we try to align P2's rotation with P1's.

			int iRotation = m_P1.pObj->GetRotation();
			
			//	For now we assume that we want P2 to be behind P1 (with respect
			//	to P1's rotation). This can be configurable later.

			int iJointRotation = iRotation + 180;

			//	Figure out the desired position of P2's center, based on 
			//	the rod length and the rotation.

			CVector vDesiredPos = m_P1.pObj->GetPos() + PolarToVector(iJointRotation, m_iMaxLength * g_KlicksPerPixel);

			//	If we've close to the desired position, then we don't have any forces

			CVector vDiff = vDesiredPos - m_P2.pObj->GetPos();
			Metric rDist2 = vDiff.Length2();
			if (rDist2 < SPINE_MAX_DIST2)
				break;

			//	Otherwise, we accelerate P2 towards the desired point, proportionally to
			//	our distance.

			Metric rDist = sqrt(rDist2);
			CVector vAccelUnit = (vDesiredPos - m_P2.pObj->GetPos()).Normal(&rDist);
			CVector vAccel = vAccelUnit * rDist * SPINE_SPRING_K;

			//	NOTE: We change the velocity directly instead of proportionally to mass
			//	because we assume the spring is stronger for heavier objects (which
			//	makes sense for attached objects).

			m_P2.pObj->DeltaV(vAccel);

			break;
			}
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

	//	Flags

	DWORD dwFlags;
	if (Ctx.dwVersion >= 148)
		Ctx.pStream->Read(dwFlags);
	else
		dwFlags = 0;

	pJoint->m_fShipCompartment = (dwFlags & 0x00000001 ? true : false);

	//	Max length

	Ctx.pStream->Read(pJoint->m_iMaxLength);

	//	Attack points

	ReadFromStream(Ctx, pJoint->m_P1);
	ReadFromStream(Ctx, pJoint->m_P2);

	*retpJoint = pJoint;
	}

void CObjectJoint::Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const

//	Paint
//
//	Paint the joint

	{
	//	Ship compartments do their own painting

	if (m_fShipCompartment)
		return;

	//	Paint based on type

	switch (m_iType)
		{
		case jointRod:
		case jointSpine:
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
	else if (strEquals(sValue, TYPE_SPINE))
		return jointSpine;
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
//	Sets paint needed flag for all joints for the given object.

	{
	CObjectJoint *pNext = this;
	while (pNext)
		{
		pNext->SetPaintNeeded(bValue);
		pNext = pNext->GetNextJoint(pObj);
		}
	}

void CObjectJoint::Update (SUpdateCtx &Ctx)

//	Update
//
//	Updates the joint every tick

	{
	//	If this is a ship compartment, we rotation the ship to face the attach
	//	point.
	//
	//	NOTE: We assume that Obj2 always faces Obj1.

	if (m_fShipCompartment)
		{
		CShip *pAttachedShip = m_P2.pObj->AsShip();
		if (pAttachedShip)
			{
			CVector vToAttachPoint = m_P1.pObj->GetPos() - m_P2.pObj->GetPos();
			pAttachedShip->SetRotation(VectorToPolar(vToAttachPoint));
			}
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
//	DWORD			m_dwFlags
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

	DWORD dwFlags = 0;
	dwFlags |= (m_fShipCompartment ? 0x00000001 : 0);
	Stream.Write(dwFlags);

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

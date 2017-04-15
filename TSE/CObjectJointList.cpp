//	CObjectJointList.cpp
//
//	CObjectJointList class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CObjectJointList::AddContacts (CPhysicsContactResolver &Resolver)

//	AddContacts
//
//	Adds contacts to the resolver.

	{
	int i;

	for (i = 0; i < m_AllJoints.GetCount(); i++)
		{
		//	While we're here we delete any destroyed joints

		if (m_AllJoints[i]->IsDestroyed())
			{
			delete m_AllJoints[i];
			m_AllJoints.Delete(i);
			i--;
			continue;
			}

		//	Clear the paint needed flag in preparation for next paint cycle

		m_AllJoints[i]->SetPaintNeeded(false);

		//	Add contacts for this joint

		m_AllJoints[i]->AddContacts(Resolver);
		}
	}

void CObjectJointList::AddJointToObjList (CObjectJoint *pJoint, CSpaceObject *pObj)

//	AddJointToObjList
//
//	Adds this joint to the list of joints for the given object.

	{
	pJoint->SetNextJoint(pObj, pObj->GetFirstJoint());
	pObj->SetFirstJoint(pJoint);
	}

void CObjectJointList::CreateJoint (CObjectJoint::ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo, CObjectJoint **retpJoint)

//	CreateJoint
//
//	Creates a new joint of the given type between the two objects. Joint 
//	properties are set to default values.

	{
	ASSERT(pFrom);
	ASSERT(pTo);
	ASSERT(iType != CObjectJoint::jointNone);

	//	Create a new joint and add it to our list (at the end).

	CObjectJoint *pJoint = new CObjectJoint(iType, pFrom, pTo);
	m_AllJoints.Insert(pJoint);

	//	For each object we keep a linked list of joints to that object.

	AddJointToObjList(pJoint, pFrom);
	AddJointToObjList(pJoint, pTo);

	//	Done

	if (retpJoint)
		*retpJoint = pJoint;
	}

void CObjectJointList::DeleteAll (void)

//	DeleteAll
//
//	Delete all joints

	{
	int i;

	for (i = 0; i < m_AllJoints.GetCount(); i++)
		delete m_AllJoints[i];

	m_AllJoints.DeleteAll();
	}

void CObjectJointList::ObjDestroyed (CSpaceObject *pObj)

//	ObjDestroyed
//
//	Delete any joints for the given object.

	{
	CObjectJoint *pNext = pObj->GetFirstJoint();
	while (pNext)
		{
		//	Destroy this joint.

		pNext->Destroy();

		//	Remove it from the list of the other object.

		RemoveJointFromObjList(pNext, pNext->GetOtherObj(pObj));

		//	Next

		pNext = pNext->GetNextJoint(pObj);
		}

	//	This object has no joints (since it is destroyed).

	pObj->SetFirstJoint(NULL);
	}

void CObjectJointList::Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint all joints

	{
	int i;

	for (i = 0; i < m_AllJoints.GetCount(); i++)
		if (m_AllJoints[i]->IsPaintNeeded())
			m_AllJoints[i]->Paint(Dest, Ctx);
	}

void CObjectJointList::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads all joints.
//
//	NOTE: This must be called AFTER all objects have been loaded in the system,
//	so we expect object references to be resolved.

	{
	int i;

	DeleteAll();

	//	First variable is the number of joints saved.

	DWORD dwCount;
	Ctx.pStream->Read(dwCount);

	m_AllJoints.GrowToFit(dwCount);

	//	Loop over all joints

	for (i = 0; i < (int)dwCount; i++)
		{
		CObjectJoint *pJoint;
		CObjectJoint::CreateFromStream(Ctx, &pJoint);

		m_AllJoints.Insert(pJoint);

		AddJointToObjList(pJoint, pJoint->GetObj1());
		AddJointToObjList(pJoint, pJoint->GetObj2());
		}
	}

void CObjectJointList::RemoveJointFromObjList (CObjectJoint *pJoint, CSpaceObject *pObj)

//	RemoveJointFromObjList
//
//	Removes the joint from the list.

	{
	CObjectJoint *pPrev = NULL;
	CObjectJoint *pTest = pObj->GetFirstJoint();
	while (pTest)
		{
		CObjectJoint *pNext = pTest->GetNextJoint(pObj);

		if (pTest == pJoint)
			{
			if (pPrev == NULL)
				pObj->SetFirstJoint(pNext);
			else
				pPrev->SetNextJoint(pObj, pNext);

			break;
			}

		pPrev = pTest;
		pTest = pNext;
		}
	}

void CObjectJointList::WriteToStream (CSystem *pSystem, IWriteStream &Stream)

//	WriteToStream
//
//	Writes out the whole list to a stream.
//
//	DWORD			count of joints
//	CObjectJoint	list of joints

	{
	int i;

	//	Remove all destroyed joints first (no need to save them)

	for (i = 0; i < m_AllJoints.GetCount(); i++)
		{
		if (m_AllJoints[i]->IsDestroyed())
			{
			delete m_AllJoints[i];
			m_AllJoints.Delete(i);
			i--;
			}
		}

	//	Write it out

	DWORD dwSave = m_AllJoints.GetCount();
	Stream.Write(dwSave);

	for (i = 0; i < m_AllJoints.GetCount(); i++)
		m_AllJoints[i]->WriteToStream(pSystem, Stream);
	}

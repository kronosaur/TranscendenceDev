//	CObjectJointList.cpp
//
//	CObjectJointList class
//	Copyright (c) 2017 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CObjectJointList::AddJointToObjList (CObjectJoint *pJoint, CSpaceObject *pObj)

//	AddJointToObjList
//
//	Adds this joint to the list of joints for the given object.

	{
	bool bNew;
	CObjectJoint **ppFirstJoint = m_FirstJoint.SetAt(pObj->GetID(), &bNew);
	if (bNew)
		pJoint->SetNextJoint(pObj, NULL);
	else
		pJoint->SetNextJoint(pObj, *ppFirstJoint);

	*ppFirstJoint = pJoint;
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
	m_FirstJoint.DeleteAll();
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

void CObjectJointList::WriteToStream (CSystem *pSystem, IWriteStream &Stream)

//	WriteToStream
//
//	Writes out the whole list to a stream.
//
//	DWORD			count of joints
//	CObjectJoint	list of joints

	{
	int i;

	DWORD dwSave = m_AllJoints.GetCount();
	Stream.Write(dwSave);

	for (i = 0; i < m_AllJoints.GetCount(); i++)
		m_AllJoints[i]->WriteToStream(pSystem, Stream);
	}

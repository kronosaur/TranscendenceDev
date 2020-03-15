//	CShotArray.cpp
//
//	CShotArray class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void CShotArray::AdjustFireAngle (int iAngleAdj)

//	AdjustFireAngle
//
//	Adjusts all fire angles.

	{
	for (int i = 0; i < GetCount(); i++)
		m_Shots[i].iDir = AngleMod(m_Shots[i].iDir + iAngleAdj);
	}

void CShotArray::SetTarget (CSpaceObject *pTarget)

//	SetTarget
//
//	Sets the target on all shots

	{
	for (int i = 0; i < GetCount(); i++)
		m_Shots[i].pTarget = pTarget;
	}

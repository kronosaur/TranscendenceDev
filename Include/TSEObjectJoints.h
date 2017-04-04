//	TSEObjectJoints.h
//
//	Defines classes and interfaces for joining objects
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CObjectJoint
	{
	public:
		enum ETypes
			{
			jointNone,

			jointRod,						//	A straight, solid rod.
			};

		CObjectJoint (void) : m_iType(jointNone)
			{ }

		void InitRod (CSpaceObject *pFrom, CSpaceObject *pTo, int iLength);
		inline void SetLifetime (int iLifetime) { m_iLifetime = iLifetime; }
		void SetMotionArc (CSpaceObject *pObj, int iMinArc, int iMaxArc);

	private:
		struct SAttachPoint
			{
			CSpaceObject *pObj;				//	Object we're attached to

			int iMinArc:16;					//	Min angle of motion arc (degrees) -1 = unconstrained
			int iMaxArc:16;					//	Max angle of motion arc (degrees)
			DWORD dwOverlayID;				//	If non-zero, attached at the overlay (otherwise, at center)
			};

		ETypes m_iType;						//	Type of joint
		DWORD m_dwID;						//	Global ID
		int m_iTick;						//	tick
		int m_iLifetime;					//	Lifetime in ticks (-1 = permanent)

		//	Attachment points

		SAttachPoint m_P1;
		SAttachPoint m_P2;

		//	Joint properties

		int m_iMaxLength;					//	Max length of joint

		//	Cached

		Metric m_rMaxLength2;
	};

class CObjectJointList
	{
	public:
		void CreateRod (CSpaceObject *pFrom, CSpaceObject *pTo, int iLength, DWORD *retdwID = NULL);

	private:
		TLinkedList<CObjectJoint> m_List;
	};


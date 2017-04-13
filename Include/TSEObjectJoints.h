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

			jointHinge,						//	A connection at a single point
			jointRod,						//	A straight, solid rod.
			};

		CObjectJoint (ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo);
		static void CreateFromStream (SLoadCtx &Ctx, CObjectJoint **retpJoint);

		inline DWORD GetID (void) const { return m_dwID; }
		inline CSpaceObject *GetObj1 (void) const { return m_P1.pObj; }
		inline CSpaceObject *GetObj2 (void) const { return m_P2.pObj; }
		inline void SetLifetime (int iLifetime) { m_iLifetime = iLifetime; }
		inline void SetMaxLength (int iLength) { m_iMaxLength = iLength; }
		void SetMotionArc (int iMinArc1, int iMaxArc1, int iMinArc2 = -1, int iMaxArc2 = -1);
		void SetNextJoint (CSpaceObject *pObj, CObjectJoint *pNext);
		void SetPos (const C3DObjectPos &Pos1, const C3DObjectPos &Pos2);
		void WriteToStream (CSystem *pSystem, IWriteStream &Stream);

		static ETypes ParseType (const CString &sValue);

	private:
		struct SAttachPoint
			{
			SAttachPoint (void) :
					pObj(NULL),
					iMinArc(-1),
					iMaxArc(-1),
					pNext(NULL)
				{ }

			CSpaceObject *pObj;				//	Object we're attached to

			C3DObjectPos Pos;				//	Position relative to object center
			int iMinArc:16;					//	Min angle of motion arc (degrees) -1 = unconstrained
			int iMaxArc:16;					//	Max angle of motion arc (degrees)

			CObjectJoint *pNext;			//	Next joint for this object
			};

		CObjectJoint (void) { }
		void WriteToStream (CSystem *pSystem, IWriteStream &Stream, const SAttachPoint &Point);

		static void ReadFromStream (SLoadCtx &Ctx, SAttachPoint &Point);

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
		~CObjectJointList (void) { DeleteAll(); }

		void CreateJoint (CObjectJoint::ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo, CObjectJoint **retpJoint = NULL);
		void DeleteAll (void);
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (CSystem *pSystem, IWriteStream &Stream);

	private:
		void AddJointToObjList (CObjectJoint *pJoint, CSpaceObject *pObj);

		TArray<CObjectJoint *> m_AllJoints;
		TSortMap<DWORD, CObjectJoint *> m_FirstJoint;
	};


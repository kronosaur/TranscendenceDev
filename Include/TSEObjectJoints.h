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

		void AddContacts (CPhysicsContactResolver &Resolver);
		void ApplyOptions (ICCItem *pOptions);
		inline void Destroy (void) { m_fDestroyed = true; m_fPaintNeeded = false; }
		inline DWORD GetID (void) const { return m_dwID; }
		inline CObjectJoint *GetNextJoint (CSpaceObject *pObj) const { if (pObj == m_P1.pObj) return m_P1.pNext; else if (pObj == m_P2.pObj) return m_P2.pNext; else return NULL; }
		inline CSpaceObject *GetObj1 (void) const { return m_P1.pObj; }
		inline CSpaceObject *GetObj2 (void) const { return m_P2.pObj; }
		inline CSpaceObject *GetOtherObj (CSpaceObject *pObj) const { if (pObj == m_P1.pObj) return m_P2.pObj; else if (pObj == m_P2.pObj) return m_P1.pObj; else return NULL; }
		inline bool IsDestroyed (void) const { return m_fDestroyed; }
		inline bool IsPaintNeeded (void) const { return m_fPaintNeeded; }
		void Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx) const;
		inline void SetLifetime (int iLifetime) { m_iLifetime = iLifetime; }
		inline void SetMaxLength (int iLength) { m_iMaxLength = iLength; }
		void SetMotionArc (int iMinArc1, int iMaxArc1, int iMinArc2 = -1, int iMaxArc2 = -1);
		void SetNextJoint (CSpaceObject *pObj, CObjectJoint *pNext);
		void SetObjListPaintNeeded (CSpaceObject *pObj, bool bValue = true);
		void SetPaintNeeded (bool bValue = true) { m_fPaintNeeded = true; }
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

		//	Flags

		DWORD m_fDestroyed:1;				//	Joint has been destroyed
		DWORD m_fPaintNeeded:1;				//	TRUE if we need to paint this frame

		DWORD m_dwSpare:30;

		//	Cached

		Metric m_rMaxLength2;
	};

class CObjectJointList
	{
	public:
		~CObjectJointList (void) { DeleteAll(); }

		void AddContacts (CPhysicsContactResolver &Resolver);
		void CreateJoint (CObjectJoint::ETypes iType, CSpaceObject *pFrom, CSpaceObject *pTo, CObjectJoint **retpJoint = NULL);
		void DeleteAll (void);
		void ObjDestroyed (CSpaceObject *pObj);
		void Paint (CG32bitImage &Dest, SViewportPaintCtx &Ctx);
		void ReadFromStream (SLoadCtx &Ctx);
		void WriteToStream (CSystem *pSystem, IWriteStream &Stream);

	private:
		void AddJointToObjList (CObjectJoint *pJoint, CSpaceObject *pObj);
		void RemoveJointFromObjList (CObjectJoint *pJoint, CSpaceObject *pObj);

		TArray<CObjectJoint *> m_AllJoints;
	};


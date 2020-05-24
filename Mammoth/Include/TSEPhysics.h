//	TSEPhysics.h
//
//	Defines classes and interfaces for physics simulation
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.
//
//	REFERENCE
//
//	Game Physics Engine Development, 2nd Edition
//	Ian Millington, CRC Press, 2010.

#pragma once

class CPhysicsContact
	{
	public:
		enum ETypes
			{
			contactNone,

			contactCollision,
			contactBarrierCollision,
			contactSelfContact,
			};

		CPhysicsContact (void) : m_iType(contactNone) { }

		inline const CVector &GetMove1 (void) const { return m_vMove1; }
		inline const CVector &GetMove2 (void) const { return m_vMove2; }
		inline CSpaceObject *GetObj1 (void) const { return m_pObj; }
		inline CSpaceObject *GetObj2 (void) const { return m_pContactObj; }
		inline Metric GetPenetration (void) const { return m_rPenetration; }
		Metric GetSeparatingVel (void) const;
		void Init (ETypes iType, CSpaceObject *pObj, CSpaceObject *pContactObj, const CVector &vNormal, Metric rPenetration, Metric rRestitution);
		void InitCollision (CSpaceObject *pObj, CSpaceObject *pContactObj);
		void OnUpdateDone (void);
		void Recalc (const CVector &vMove);
		void Resolve (void);

	private:
		void InitDerivedValues (void);
		void ResolveInterpenetration (void);
		void ResolveVelocity (void);

		ETypes m_iType;						//	Contact type
		CSpaceObject *m_pObj;				//	Main object in contact
		CSpaceObject *m_pContactObj;		//	Object we contacted
		CVector m_vContactNormal;			//	Direction of impact from contact object
											//		This points AWAY from the contact object
		Metric m_rPenetration;				//	Distance along contact normal to move in order
											//		to not be penetrating.
		Metric m_rRestitution;				//	Restitution coefficient 0 to 1

		//	These values are derived from the above

		Metric m_rInvMass1;					//	Immobile objects have inverse mass of 0
		Metric m_rInvMass2;
		Metric m_rTotalInvMass;				//	Total inverse mass

		//	Cached calculations

		mutable bool m_bSeparatingVel;		//	TRUE if m_rSeparatingVel is valid
		mutable Metric m_rSeparatingVel;	//	Velocity along the contact normal

		//	Resolution values

		CVector m_vMove1;
		CVector m_vMove2;
	};

class CPhysicsContactResolver
	{
	public:
		void AddCollision (CSpaceObject *pObj, CSpaceObject *pContactObj);
		void AddRod (CPhysicsContact::ETypes iType, CSpaceObject *pObj1, CSpaceObject *pObj2, Metric rLength);
		void BeginUpdate (void);
		void Update (void);

	private:
		CPhysicsContact *FindContactToResolve (void);
		void RecalcContacts (CPhysicsContact *pChanged);

		TArray<CPhysicsContact> m_Contacts;
	};

class CPhysicsForceResolver
	{
	public:
		struct SForceDesc
			{
			CSpaceObject *pObj = NULL;
			CVector vForce;						//	Absolute acceleration
			CVector vLimitedForce;				//	Accelerate only if below max speed.
			Metric rDragFactor = 1.0;			//	Final velocity gets multiplied by this.
			};

		int AllocDesc (CSpaceObject &Obj);
		void BeginUpdate (void);
		const SForceDesc &GetConstDesc (int iIndex) const { return m_Forces[iIndex]; }
		SForceDesc &GetDesc (int iIndex) { return m_Forces[iIndex]; }
		void Update (CSystem &System, const Metric rSecondsPerUpdate);

	private:
		static constexpr int DEFAULT_ALLOC = 200;
		static constexpr Metric MIN_DRAG_SPEED2 = 1000.0;

		TArray<SForceDesc> m_Forces;
	};

class CPhysicsForceDesc
	{
	public:
		void AddDrag (CPhysicsForceResolver &ForceResolver, CSpaceObject &Obj, Metric rDragFactor);
		void AddForce (CPhysicsForceResolver &ForceResolver, CSpaceObject &Obj, const CVector &vForce);
		void AddForceLimited (CPhysicsForceResolver &ForceResolver, CSpaceObject &Obj, const CVector &vForce);
		void Clear (void) { m_iIndex = -1; }
		
	private:
		CPhysicsForceResolver::SForceDesc &GetDesc (CPhysicsForceResolver &ForceResolver, CSpaceObject &Obj);

		int m_iIndex = -1;
	};

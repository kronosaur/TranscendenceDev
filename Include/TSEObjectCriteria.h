//	TSEObjectCriteria.h
//
//	Defines classes and interfaces for selecting space objects
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CSpaceObject;

class CSpaceObjectCriteria
	{
	public:
		enum CriteriaPosCheckTypes
			{
			checkNone,
			checkPosIntersect,
			checkLineIntersect,
			};

		enum CriteriaSortTypes
			{
			sortNone,
			sortByDistance,
			};

		struct SCtx
			{
			SCtx (const CSpaceObjectCriteria &Crit);

			//	Computed from criteria
			Metric rMinRadius2;
			Metric rMaxRadius2;

			//	Nearest/farthest object
			CSpaceObject *pBestObj;
			Metric rBestDist2;

			//	Sorted results
			TSortMap<Metric, CSpaceObject *> DistSort;

			//	Temporaries
			bool bCalcPolar;
			bool bCalcDist2;
			};

		CSpaceObjectCriteria (void) { }
		CSpaceObjectCriteria (const CString &sCriteria);
		CSpaceObjectCriteria (CSpaceObject *pSourceArg, const CString &sCriteria);

		inline bool ExcludesPlayer (void) const { return m_bExcludePlayer; }
		inline bool ExcludesVirtual (void) const { return !m_bIncludeVirtual; }
		inline CriteriaSortTypes GetSort (void) const { return m_iSort; }
		inline ESortOptions GetSortOrder (void) const { return m_iSortOrder; }
		inline CSpaceObject *GetSource (void) const { return m_pSource; }
		inline void Init (const CString &sCriteria) { *this = CSpaceObjectCriteria(); Parse(NULL, sCriteria); }
		inline void Init (CSpaceObject *pSource, const CString &sCriteria) { *this = CSpaceObjectCriteria(); Parse(pSource, sCriteria); }
		inline bool IsEmpty (void) const { return (m_dwCategories == 0); }
		inline bool MatchesActiveOnly (void) const { return m_bActiveObjectsOnly; }
		inline bool MatchesAngryOnly (void) const { return m_bAngryObjectsOnly; }
		bool MatchesAttributes (const CSpaceObject &Obj) const;
		inline bool MatchesCategory (DWORD dwCat) const { return (m_dwCategories & dwCat) != 0; }
		inline const CString &MatchesData (void) const { return m_sData; }
		inline bool MatchesDockedWithSource (void) const { return m_bDockedWithSource; }
		inline bool MatchesEnemyOnly (void) const { return m_bEnemyObjectsOnly; }
		inline bool MatchesFartherThan (void) const { return m_bFartherThan; }
		inline bool MatchesFarthestOnly (void) const { return m_bFarthestOnly; }
		inline bool MatchesFriendlyOnly (void) const { return m_bFriendlyObjectsOnly; }
		inline bool MatchesHomeBaseIsSource (void) const { return m_bHomeBaseIsSource; }
		inline int MatchesIntersectAngle (void) const { return m_iIntersectAngle; }
		inline bool MatchesKilledOnly (void) const { return m_bKilledObjectsOnly; }
		bool MatchesLevel (int iLevel) const;
		inline bool MatchesManufacturedOnly (void) const { return m_bManufacturedObjectsOnly; }
		inline Metric MatchesMaxRadius (void) const { return m_rMaxRadius; }
		inline Metric MatchesMinRadius (void) const { return m_rMinRadius; }
		inline bool MatchesNearerThan (void) const { return m_bNearerThan; }
		inline bool MatchesNearestOnly (void) const { return m_bNearestOnly; }
		bool MatchesOrder (const CSpaceObject *pSource, const CSpaceObject &Obj) const;
		inline bool MatchesPerceivableOnly (void) const { return m_bPerceivableOnly; }
		inline int MatchesPerception (void) const { return m_iPerception; }
		inline bool MatchesPlayer (void) const { return m_bSelectPlayer; }
		bool MatchesPosition (const CSpaceObject &Obj) const;
		bool MatchesSovereign (CSovereign *pSovereign) const;
		inline bool MatchesStargate (const CString &sID) const { return (m_sStargateID.IsBlank() || strEquals(m_sStargateID, sID)); }
		inline bool MatchesStargatesOnly (void) const { return m_bStargatesOnly; }
		inline bool MatchesStructureScaleOnly (void) const { return m_bStructureScaleOnly; }
		inline bool MatchesTargetIsSource (void) const { return m_bTargetIsSource; }
		inline void SetLineIntersect (const CVector &vPos1, const CVector &vPos2) { m_iPosCheck = checkLineIntersect; m_vPos1 = vPos1; m_vPos2 = vPos2; }
		inline void SetPosIntersect (const CVector &vPos) { m_iPosCheck = checkPosIntersect; m_vPos1 = vPos; }
		void SetSource (CSpaceObject *pSource);

	private:
		void Parse (CSpaceObject *pSource, const CString &sCriteria);

		CSpaceObject *m_pSource = NULL;				//	Source

		DWORD m_dwCategories = 0;					//	Only these object categories
		bool m_bSelectPlayer = false;				//	Select the player
		bool m_bIncludeVirtual = false;				//	Include virtual objects
		bool m_bActiveObjectsOnly = false;			//	Only active object (e.g., objects that can attack)
		bool m_bKilledObjectsOnly = false;			//	Only objects that cannot attack
		bool m_bFriendlyObjectsOnly = false;		//	Only friendly to source
		bool m_bEnemyObjectsOnly = false;			//	Only enemy to source
		bool m_bAngryObjectsOnly = false;			//	Only enemy and angry objects
		bool m_bManufacturedObjectsOnly = false;	//	Exclude planets, stars, etc.
		bool m_bStructureScaleOnly = false;			//	Only structure-scale objects
		bool m_bStargatesOnly = false;				//	Only stargates
		bool m_bNearestOnly = false;				//	The nearest object to the source
		bool m_bFarthestOnly = false;				//	The fartest object to the source
		bool m_bNearerThan = false;					//	Only objects nearer than rMinRadius
		bool m_bFartherThan = false;				//	Only objects farther than rMaxRadius
		bool m_bHomeBaseIsSource = false;			//	Only objects whose home base is the source
		bool m_bDockedWithSource = false;			//	Only objects currently docked with source
		bool m_bExcludePlayer = false;				//	Exclude the player
		bool m_bTargetIsSource = false;				//	Only objects whose target is the source

		bool m_bPerceivableOnly = false;			//	Only objects that can be perceived by the source
		int m_iPerception = 0;						//	Cached perception of pSource

		bool m_bSourceSovereignOnly = false;		//	Only objects the same sovereign as source
		DWORD m_dwSovereignUNID = 0;				//	Only objects with this sovereign UNID

		CString m_sData;							//	Only objects with non-Nil data
		CString m_sStargateID;						//	Only objects with this stargate ID (if non blank)
		Metric m_rMinRadius = 0.0;					//	Only objects at or beyond the given radius
		Metric m_rMaxRadius = g_InfiniteDistance;	//	Only objects within the given radius
		int m_iIntersectAngle = -1;					//	Only objects that intersect line from source
		IShipController::OrderTypes m_iOrder = IShipController::orderNone;	//	Only objects with this order

		TArray<CString> m_AttribsRequired;			//	Required attributes
		TArray<CString> m_AttribsNotAllowed;		//	Exclude objects with these attributes
		TArray<CString> m_SpecialRequired;			//	Special required attributes
		TArray<CString> m_SpecialNotAllowed;		//	Special excluding attributes

		int m_iEqualToLevel = -1;					//	Objects of this level
		int m_iGreaterThanLevel = -1;
		int m_iLessThanLevel = -1;

		CriteriaPosCheckTypes m_iPosCheck = checkNone;
		CVector m_vPos1;
		CVector m_vPos2;

		CriteriaSortTypes m_iSort = sortNone;
		ESortOptions m_iSortOrder = AscendingSort;
	};


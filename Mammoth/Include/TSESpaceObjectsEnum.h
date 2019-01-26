//	TSESpaceObjectsEnum.h
//
//	Transcendence Space Engine
//	Copyright 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	RANGE FILTERS --------------------------------------------------------------

//	CNearestInRadiusRange
//
//	Matches objects within rRadius of vCenter.

class CNearestInRadiusRange
	{
	public:
		CNearestInRadiusRange (const CVector &vCenter, Metric rRadius) :
				m_vCenter(vCenter)
			{
			SetRadius(rRadius);
			}

		inline const CVector &GetLL (void) const { return m_vLL; }
		inline const CVector &GetUR (void) const { return m_vUR; }

		inline bool Matches (CSpaceObject *pObj, Metric *retrDist2 = NULL) const
			{
			CVector vDist = pObj->GetPos() - m_vCenter;
			Metric rDist2 = vDist.Length2();

			if (rDist2 >= m_rBestDist2)
				return false;

			if (retrDist2)
				*retrDist2 = rDist2;

			return true;
			}

		inline void ReduceRadius (Metric rRadius)
			{
			if (rRadius < m_rRadius)
				SetRadius(rRadius);
			}

		inline void SetBestDist2 (Metric rDist2) { m_rBestDist2 = rDist2; }

	private:
		inline void SetRadius (Metric rRadius)
			{
			m_rRadius = rRadius;
			m_rBestDist2 = rRadius * rRadius;

			CVector vRange = CVector(m_rRadius, m_rRadius);
			m_vUR = m_vCenter + vRange;
			m_vLL = m_vCenter - vRange;
			}

		CVector m_vCenter;
		Metric m_rRadius;

		CVector m_vUR;
		CVector m_vLL;

		Metric m_rBestDist2;
	};

//	CNearestInArcAndRadiusRange
//
//	Matches objects within rRadius and the given arc or vCenter.

class CNearestInArcAndRadiusRange
	{
	public:
		CNearestInArcAndRadiusRange (const CVector &vCenter, Metric rRadius, int iMinFireArc, int iMaxFireArc) :
				m_vCenter(vCenter),
				m_iMinFireArc(iMinFireArc),
				m_iMaxFireArc(iMaxFireArc)
			{
			SetRadius(rRadius);
			}

		inline const CVector &GetLL (void) const { return m_vLL; }
		inline const CVector &GetUR (void) const { return m_vUR; }

		inline bool Matches (CSpaceObject *pObj, Metric *retrDist2 = NULL) const
			{
			CVector vDist = pObj->GetPos() - m_vCenter;
			Metric rDist2 = vDist.Length2();

			if (rDist2 >= m_rBestDist2)
				return false;

			int iAngle = VectorToPolar(vDist);
			if (!AngleInArc(iAngle, m_iMinFireArc, m_iMaxFireArc))
				return false;

			if (retrDist2)
				*retrDist2 = rDist2;

			return true;
			}

		inline void ReduceRadius (Metric rRadius)
			{
			if (rRadius < m_rRadius)
				SetRadius(rRadius);
			}

		inline void SetBestDist2 (Metric rDist2) { m_rBestDist2 = rDist2; }

	private:
		inline void SetRadius (Metric rRadius)
			{
			m_rRadius = rRadius;
			m_rBestDist2 = rRadius * rRadius;

			CVector vRange = CVector(m_rRadius, m_rRadius);
			m_vUR = m_vCenter + vRange;
			m_vLL = m_vCenter - vRange;
			}

		CVector m_vCenter;
		Metric m_rRadius;
		int m_iMinFireArc;
		int m_iMaxFireArc;

		CVector m_vUR;
		CVector m_vLL;

		Metric m_rBestDist2;
	};

//	SELECTORS ------------------------------------------------------------------
//
//	CAnyObjSelector
//
//	Selects any object, including virtual ones, except those that are destroyed,
//	suspended, etc.

class CAnyObjSelector
	{
	public:
		inline Metric GetMaxRange (void) const
			{
			return g_InfiniteDistance;
			}

		inline bool Matches (CSpaceObject *pObj, Metric rDist2) const
			{
			return !pObj->IsUnreal();
			}

		inline bool MatchesCategory (CSpaceObject *pObj) const
			{
			return true;
			}
	};

//	CCriteriaObjSelector
//
//	Same as CAnyObjSelector, but objects must match a criteria.
//	NOTE: This can match virtual objects.

class CCriteriaObjSelector
	{
	public:
		CCriteriaObjSelector (const CSpaceObjectCriteria &Criteria) :
				m_Criteria(Criteria),
				m_Ctx(m_Criteria)
			{ }

		inline Metric GetMaxRange (void) const
			{
			return g_InfiniteDistance;
			}

		inline bool Matches (CSpaceObject *pObj, Metric rDist2)
			{
			return (!pObj->IsUnreal())
				&& pObj->MatchesCriteria(m_Ctx, m_Criteria);
			}

		inline bool MatchesCategory (CSpaceObject *pObj) const
			{
			return true;
			}

	private:
		const CSpaceObjectCriteria &m_Criteria;
		CSpaceObjectCriteria::SCtx m_Ctx;
	};

//	CVisibleEnemyObjSelector
//
//	Selects possible enemy targets for pSource that are visible to pSource. 
//	Includes otherwise friendly objects that are angry at pSource.

class CVisibleEnemyObjSelector
	{
	public:
		CVisibleEnemyObjSelector (CSpaceObject *pSource) :
				m_pSource(pSource),
				m_Perception(pSource->GetPerception())
			{ }

		inline Metric GetMaxRange (void) const
			{
			return m_Perception.GetRange(0);
			}

		inline bool Matches (CSpaceObject *pObj, Metric rDist2) const
			{
			return (pObj->CanAttack()
				&& m_pSource->IsAngryAt(pObj)
				&& m_Perception.CanBeTargeted(pObj, rDist2)
				&& !pObj->IsDestroyed()
				&& pObj != m_pSource
				&& pObj != m_pExcludeObj
				&& !pObj->IsEscortingFriendOf(m_pSource));
			}

		inline bool MatchesCategory (CSpaceObject *pObj) const
			{
			return (pObj->GetCategory() == CSpaceObject::catShip
						|| (m_bIncludeStations && pObj->GetCategory() == CSpaceObject::catStation));
			}

		inline void SetExcludeObj (CSpaceObject *pObj) { m_pExcludeObj = pObj; }
		inline void SetIncludeStations (bool bValue = true) { m_bIncludeStations = bValue; }

	private:
		CPerceptionCalc m_Perception;
		CSpaceObject *m_pSource = NULL;
		CSpaceObject *m_pExcludeObj = NULL;
		bool m_bIncludeStations = false;
	};

//	CVisibleAggressorObjSelector
//
//	Selects targetable objects that are visible to pSource and that have been
//	aggressive recently.

class CVisibleAggressorObjSelector
	{
	public:
		CVisibleAggressorObjSelector (CSpaceObject *pSource) :
				m_pSource(pSource),
				m_Perception(pSource->GetPerception()),
				m_iAggressorThreshold(g_pUniverse->GetTicks() - CSpaceObject::AGGRESSOR_THRESHOLD)
			{ }

		inline Metric GetMaxRange (void) const
			{
			return m_Perception.GetRange(0);
			}

		inline bool Matches (CSpaceObject *pObj, Metric rDist2) const
			{
			return (pObj->CanAttack()
				&& m_pSource->IsAngryAt(pObj)
				&& m_Perception.CanBeTargeted(pObj, rDist2)
				&& !pObj->IsDestroyed()
				&& pObj != m_pSource
				&& pObj != m_pExcludeObj
				&& pObj->GetLastFireTime() > m_iAggressorThreshold
				&& !pObj->IsEscortingFriendOf(m_pSource));
			}

		inline bool MatchesCategory (CSpaceObject *pObj) const
			{
			return (pObj->GetCategory() == CSpaceObject::catShip
						|| (m_bIncludeStations && pObj->GetCategory() == CSpaceObject::catStation));
			}

		inline void SetExcludeObj (CSpaceObject *pObj) { m_pExcludeObj = pObj; }
		inline void SetIncludeStations (bool bValue = true) { m_bIncludeStations = bValue; }

	private:
		CPerceptionCalc m_Perception;
		CSpaceObject *m_pSource = NULL;
		CSpaceObject *m_pExcludeObj = NULL;
		int m_iAggressorThreshold = -1;
		bool m_bIncludeStations = false;
	};

//	CVisibleObjSelector
//
//	Selects targetable objects that are visible to pSource.

class CVisibleObjSelector
	{
	public:
		CVisibleObjSelector (CSpaceObject *pSource) :
				m_pSource(pSource),
				m_Perception(pSource->GetPerception())
			{ }

		inline Metric GetMaxRange (void) const
			{
			return m_Perception.GetRange(0);
			}

		inline bool Matches (CSpaceObject *pObj, Metric rDist2) const
			{
			return (pObj->CanAttack()
				&& m_Perception.CanBeTargeted(pObj, rDist2)
				&& !pObj->IsDestroyed()
				&& pObj != m_pSource
				&& pObj != m_pExcludeObj
				&& !pObj->IsEscortingFriendOf(m_pSource));
			}

		inline bool MatchesCategory (CSpaceObject *pObj) const
			{
			return (pObj->GetCategory() == CSpaceObject::catShip
						|| (m_bIncludeStations && pObj->GetCategory() == CSpaceObject::catStation));
			}

		inline void SetExcludeObj (CSpaceObject *pObj) { m_pExcludeObj = pObj; }
		inline void SetIncludeStations (bool bValue = true) { m_bIncludeStations = bValue; }

	private:
		CPerceptionCalc m_Perception;
		CSpaceObject *m_pSource = NULL;
		CSpaceObject *m_pExcludeObj = NULL;
		bool m_bIncludeStations = false;
	};

class CSpaceObjectEnum
	{
	public:
		template <class RANGE, class SELECTOR> 
		static CSpaceObject *FindNearestEnemyObj (const CSystem &System, CSpaceObject &SourceObj, RANGE &Range, SELECTOR &Selector)
			{
			CSpaceObject *pBestObj = NULL;

			//	Get the sovereign

			CSovereign *pSovereign = SourceObj.GetSovereignToDefend();
			if (pSovereign == NULL)
				return NULL;

			//	Adjust range if Selector has a shorter range (e.g., because of
			//	perception).

			Range.ReduceRadius(Selector.GetMaxRange());

			//	If we're the player, then we can't use the sovereign enemy list
			//	because it doesn't handle angry objects.

			if (pSovereign->IsPlayer())
				{
				const CSpaceObjectGrid &Grid = System.GetObjectGrid();
				SSpaceObjectGridEnumerator i;
				Grid.EnumStart(i, Range.GetUR(), Range.GetLL(), gridNoBoxCheck);

				while (Grid.EnumHasMore(i))
					{
					CSpaceObject *pObj = Grid.EnumGetNextFast(i);

					Metric rDist2;
					if (Selector.MatchesCategory(pObj)
							&& Range.Matches(pObj, &rDist2)
							&& SourceObj.IsAngryAt(pObj)
							&& Selector.Matches(pObj, rDist2))
						{
						Range.SetBestDist2(rDist2);
						pBestObj = pObj;
						}
					}

				return pBestObj;
				}

			//	Otherwise, we use the list of enemies of the sovereign, because
			//	it is faster than the grid.

			else
				{
				Metric rDist2;

				//	Start with the player ship, if we're angry at it.

				CSpaceObject *pPlayer = System.GetPlayerShip();
				if (pPlayer 
						&& Selector.MatchesCategory(pPlayer)
						&& !SourceObj.IsEnemy(pPlayer) 
						&& SourceObj.IsAngryAt(pPlayer)
						&& Range.Matches(pPlayer, &rDist2)
						&& Selector.Matches(pPlayer, rDist2))
					{
					Range.SetBestDist2(rDist2);
					pBestObj = pPlayer;
					}

				//	Get the list of enemy objects

				const CSpaceObjectList &ObjList = pSovereign->GetEnemyObjectList(&System);

				for (int i = 0; i < ObjList.GetCount(); i++)
					{
					CSpaceObject *pObj = ObjList.GetObj(i);

					if (Selector.MatchesCategory(pObj)
							&& Range.Matches(pObj, &rDist2)
							&& Selector.Matches(pObj, rDist2))
						{
						Range.SetBestDist2(rDist2);
						pBestObj = pObj;
						}
					}

				return pBestObj;
				}
			}

		template <class RANGE, class SELECTOR>
		static int FindNearestEnemyObjs (const CSystem &System, CSpaceObject &SourceObj, RANGE &Range, SELECTOR &Selector, TArray<CSpaceObject *> &retObjs, int iMaxObjs = 10)

		//	FindNearestEnemyObjs
		//
		//	Returns an array of enemy objects that match the given range and 
		//	selector. We append to retObjs, in case callers have their own list
		//	of objects.

			{
			TSortMap<Metric, CSpaceObject *> Sorted;

			//	Get the sovereign

			CSovereign *pSovereign = SourceObj.GetSovereignToDefend();
			if (pSovereign == NULL)
				return NULL;

			//	Adjust range if Selector has a shorter range (e.g., because of
			//	perception).

			Range.ReduceRadius(Selector.GetMaxRange());

			//	If we're the player, then we can't use the sovereign enemy list
			//	because it doesn't handle angry objects.

			if (pSovereign->IsPlayer())
				{
				const CSpaceObjectGrid &Grid = System.GetObjectGrid();
				SSpaceObjectGridEnumerator i;
				Grid.EnumStart(i, Range.GetUR(), Range.GetLL(), gridNoBoxCheck);

				while (Grid.EnumHasMore(i))
					{
					CSpaceObject *pObj = Grid.EnumGetNextFast(i);

					Metric rDist2;
					if (Selector.MatchesCategory(pObj)
							&& Range.Matches(pObj, &rDist2)
							&& SourceObj.IsAngryAt(pObj)
							&& Selector.Matches(pObj, rDist2))
						{
						Sorted.Insert(rDist2, pObj);

						//	If we've already got enough objects, then set a new worst 
						//	distance.

						if (Sorted.GetCount() >= iMaxObjs)
							Range.SetBestDist2(Sorted.GetKey(iMaxObjs - 1));
						}
					}
				}

			//	Otherwise, we use the list of enemies of the sovereign, because
			//	it is faster than the grid.

			else
				{
				Metric rDist2;

				//	Start with the player ship, if we're angry at it.

				CSpaceObject *pPlayer = System.GetPlayerShip();
				if (pPlayer 
						&& Selector.MatchesCategory(pPlayer)
						&& !SourceObj.IsEnemy(pPlayer) 
						&& SourceObj.IsAngryAt(pPlayer)
						&& Range.Matches(pPlayer, &rDist2)
						&& Selector.Matches(pPlayer, rDist2))
					{
					Sorted.Insert(rDist2, pPlayer);

					//	NOTE: We don't bother setting the best distance, since
					//	we probably don't have enough objects to fill the list.
					}

				//	Get the list of enemy objects

				const CSpaceObjectList &ObjList = pSovereign->GetEnemyObjectList(&System);

				for (int i = 0; i < ObjList.GetCount(); i++)
					{
					CSpaceObject *pObj = ObjList.GetObj(i);

					if (Selector.MatchesCategory(pObj)
							&& Range.Matches(pObj, &rDist2)
							&& Selector.Matches(pObj, rDist2))
						{
						Sorted.Insert(rDist2, pObj);

						//	If we've already got enough objects, then set a new worst 
						//	distance.

						if (Sorted.GetCount() >= iMaxObjs)
							Range.SetBestDist2(Sorted.GetKey(iMaxObjs - 1));
						}
					}
				}

			//	Add each of the entries in the array to the	output.

			int iFound = Min(Sorted.GetCount(), iMaxObjs);
			retObjs.GrowToFit(iFound);
			for (int i = 0; i < iFound; i++)
				retObjs.Insert(Sorted[i]);

			//	Return the number of enemies found

			return iFound;
			}

		template <class RANGE, class SELECTOR>
		static CSpaceObject *FindNearestObj (const CSystem &System, RANGE &Range, SELECTOR &Selector)
			{
			const CSpaceObjectGrid &Grid = System.GetObjectGrid();

			Range.ReduceRadius(Selector.GetMaxRange());

			CSpaceObject *pBestObj = NULL;

			SSpaceObjectGridEnumerator i;
			Grid.EnumStart(i, Range.GetUR(), Range.GetLL(), gridNoBoxCheck);

			while (Grid.EnumHasMore(i))
				{
				CSpaceObject *pObj = Grid.EnumGetNextFast(i);

				Metric rDist2;
				if (Selector.MatchesCategory(pObj)
						&& Range.Matches(pObj, &rDist2)
						&& Selector.Matches(pObj, rDist2))
					{
					Range.SetBestDist2(rDist2);
					pBestObj = pObj;
					}
				}

			return pBestObj;
			}

		template <class RANGE, class SELECTOR>
		static CSpaceObject *FindObjInRange (const CSystem &System, RANGE &Range, SELECTOR &Selector)
			{
			const CSpaceObjectGrid &Grid = System.GetObjectGrid();

			Range.ReduceRadius(Selector.GetMaxRange());

			SSpaceObjectGridEnumerator i;
			Grid.EnumStart(i, Range.GetUR(), Range.GetLL(), gridNoBoxCheck);

			while (Grid.EnumHasMore(i))
				{
				CSpaceObject *pObj = Grid.EnumGetNextFast(i);

				Metric rDist2;
				if (Selector.MatchesCategory(pObj)
						&& Range.Matches(pObj, &rDist2)
						&& Selector.Matches(pObj, rDist2))
					{
					return pObj;
					}
				}

			return NULL;
			}

	};

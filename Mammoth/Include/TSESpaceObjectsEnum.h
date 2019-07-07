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

		const CVector &GetLL (void) const { return m_vLL; }
		const CVector &GetUR (void) const { return m_vUR; }

		bool Matches (CSpaceObject &Obj, Metric *retrDist2 = NULL) const
			{
			CVector vDist = Obj.GetPos() - m_vCenter;
			Metric rDist2 = vDist.Length2();

			if (rDist2 >= m_rBestDist2)
				return false;

			if (retrDist2)
				*retrDist2 = rDist2;

			return true;
			}

		void ReduceRadius (Metric rRadius)
			{
			if (rRadius < m_rRadius)
				SetRadius(rRadius);
			}

		void SetBestDist2 (Metric rDist2) { m_rBestDist2 = rDist2; }

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

		const CVector &GetLL (void) const { return m_vLL; }
		const CVector &GetUR (void) const { return m_vUR; }

		bool Matches (CSpaceObject &Obj, Metric *retrDist2 = NULL) const
			{
			CVector vDist = Obj.GetPos() - m_vCenter;
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

		void ReduceRadius (Metric rRadius)
			{
			if (rRadius < m_rRadius)
				SetRadius(rRadius);
			}

		void SetBestDist2 (Metric rDist2) { m_rBestDist2 = rDist2; }

	private:
		void SetRadius (Metric rRadius)
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

		inline bool Matches (CSpaceObject &Obj, Metric rDist2) const
			{
			return !Obj.IsUnreal();
			}

		inline bool MatchesCategory (CSpaceObject &Obj) const
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

		inline bool Matches (CSpaceObject &Obj, Metric rDist2)
			{
			return (!Obj.IsUnreal())
				&& Obj.MatchesCriteria(m_Ctx, m_Criteria);
			}

		inline bool MatchesCategory (CSpaceObject &Obj) const
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
		CVisibleEnemyObjSelector (CSpaceObject &Source) :
				m_Source(Source),
				m_Perception(Source.GetPerception())
			{ }

		Metric GetMaxRange (void) const
			{
			return m_Perception.GetRange(0);
			}

		bool Matches (CSpaceObject &Obj, Metric rDist2) const
			{
			return (Obj.CanAttack()
				&& m_Source.IsAngryAt(&Obj)
				&& m_Perception.CanBeTargeted(&Obj, rDist2)
				&& !Obj.IsUnreal()
				&& !Obj.IsDestroyed()
				&& &Obj != &m_Source
				&& &Obj != m_pExcludeObj
				&& !Obj.IsEscortingFriendOf(&m_Source));
			}

		bool MatchesCategory (CSpaceObject &Obj) const
			{
			return (Obj.GetCategory() == CSpaceObject::catShip
						|| (m_bIncludeStations && Obj.GetCategory() == CSpaceObject::catStation)
						|| (m_bIncludeMissiles && (Obj.GetCategory() == CSpaceObject::catMissile && Obj.IsTargetableProjectile())));
			}

		void SetExcludeObj (CSpaceObject *pObj) { m_pExcludeObj = pObj; }
		void SetIncludeStations (bool bValue = true) { m_bIncludeStations = bValue; }
		void SetIncludeMissiles(bool bValue = true) { m_bIncludeMissiles = bValue; }

	private:
		CPerceptionCalc m_Perception;
		CSpaceObject &m_Source;
		CSpaceObject *m_pExcludeObj = NULL;
		bool m_bIncludeStations = false;
		bool m_bIncludeMissiles = false;
	};

//	CVisibleAggressorObjSelector
//
//	Selects targetable objects that are visible to pSource and that have been
//	aggressive recently.

class CVisibleAggressorObjSelector
	{
	public:
		CVisibleAggressorObjSelector (CSpaceObject &Source) :
				m_Source(Source),
				m_Perception(Source.GetPerception()),
				m_iAggressorThreshold(Source.GetUniverse().GetTicks() - CSpaceObject::AGGRESSOR_THRESHOLD)
			{ }

		Metric GetMaxRange (void) const
			{
			return m_Perception.GetRange(0);
			}

		bool Matches (CSpaceObject &Obj, Metric rDist2) const
			{
			return (Obj.CanAttack()
				&& m_Source.IsAngryAt(&Obj)
				&& m_Perception.CanBeTargeted(&Obj, rDist2)
				&& !Obj.IsUnreal()
				&& !Obj.IsDestroyed()
				&& &Obj != &m_Source
				&& &Obj != m_pExcludeObj
				&& Obj.GetLastFireTime() > m_iAggressorThreshold
				&& !Obj.IsEscortingFriendOf(&m_Source));
			}

		bool MatchesCategory (CSpaceObject &Obj) const
			{
			return (Obj.GetCategory() == CSpaceObject::catShip
						|| (m_bIncludeStations && Obj.GetCategory() == CSpaceObject::catStation)
						|| (m_bIncludeMissiles && (Obj.GetCategory() == CSpaceObject::catMissile && Obj.IsTargetableProjectile())));
			}

		void SetExcludeObj (CSpaceObject *pObj) { m_pExcludeObj = pObj; }
		void SetIncludeStations (bool bValue = true) { m_bIncludeStations = bValue; }
		void SetIncludeMissiles(bool bValue = true) { m_bIncludeMissiles = bValue; }

	private:
		CPerceptionCalc m_Perception;
		CSpaceObject &m_Source;
		CSpaceObject *m_pExcludeObj = NULL;
		int m_iAggressorThreshold = -1;
		bool m_bIncludeStations = false;
		bool m_bIncludeMissiles = false;
	};

//	CVisibleObjSelector
//
//	Selects targetable objects that are visible to pSource.

class CVisibleObjSelector
	{
	public:
		CVisibleObjSelector (CSpaceObject &Source) :
				m_Source(Source),
				m_Perception(Source.GetPerception())
			{ }

		Metric GetMaxRange (void) const
			{
			return m_Perception.GetRange(0);
			}

		bool Matches (CSpaceObject &Obj, Metric rDist2) const
			{
			return (Obj.CanAttack()
				&& m_Perception.CanBeTargeted(&Obj, rDist2)
				&& !Obj.IsUnreal()
				&& !Obj.IsDestroyed()
				&& &Obj != &m_Source
				&& &Obj != m_pExcludeObj
				&& !Obj.IsEscortingFriendOf(&m_Source));
			}

		bool MatchesCategory (CSpaceObject &Obj) const
			{
			return (Obj.GetCategory() == CSpaceObject::catShip
						|| (m_bIncludeStations && Obj.GetCategory() == CSpaceObject::catStation)
						|| (m_bIncludeMissiles && (Obj.GetCategory() == CSpaceObject::catMissile && Obj.IsTargetableProjectile())));
			}

		void SetExcludeObj (CSpaceObject *pObj) { m_pExcludeObj = pObj; }
		void SetIncludeStations (bool bValue = true) { m_bIncludeStations = bValue; }
		void SetIncludeMissiles (bool bValue = true) { m_bIncludeMissiles = bValue; }

	private:
		CPerceptionCalc m_Perception;
		CSpaceObject &m_Source;
		CSpaceObject *m_pExcludeObj = NULL;
		bool m_bIncludeStations = false;
		bool m_bIncludeMissiles = false;
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
					if (Selector.MatchesCategory(*pObj)
							&& Range.Matches(*pObj, &rDist2)
							&& SourceObj.IsAngryAt(pObj)
							&& Selector.Matches(*pObj, rDist2))
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
						&& Selector.MatchesCategory(*pPlayer)
						&& !SourceObj.IsEnemy(pPlayer) 
						&& SourceObj.IsAngryAt(pPlayer)
						&& Range.Matches(*pPlayer, &rDist2)
						&& Selector.Matches(*pPlayer, rDist2))
					{
					Range.SetBestDist2(rDist2);
					pBestObj = pPlayer;
					}

				//	Get the list of enemy objects

				const CSpaceObjectList &ObjList = pSovereign->GetEnemyObjectList(&System);

				for (int i = 0; i < ObjList.GetCount(); i++)
					{
					CSpaceObject *pObj = ObjList.GetObj(i);

					if (Selector.MatchesCategory(*pObj)
							&& Range.Matches(*pObj, &rDist2)
							&& Selector.Matches(*pObj, rDist2))
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
					if (Selector.MatchesCategory(*pObj)
							&& Range.Matches(*pObj, &rDist2)
							&& SourceObj.IsAngryAt(pObj)
							&& Selector.Matches(*pObj, rDist2))
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
						&& Selector.MatchesCategory(*pPlayer)
						&& !SourceObj.IsEnemy(pPlayer) 
						&& SourceObj.IsAngryAt(pPlayer)
						&& Range.Matches(*pPlayer, &rDist2)
						&& Selector.Matches(*pPlayer, rDist2))
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

					if (Selector.MatchesCategory(*pObj)
							&& Range.Matches(*pObj, &rDist2)
							&& Selector.Matches(*pObj, rDist2))
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
				if (Selector.MatchesCategory(*pObj)
						&& Range.Matches(*pObj, &rDist2)
						&& Selector.Matches(*pObj, rDist2))
					{
					return pObj;
					}
				}

			return NULL;
			}

	};

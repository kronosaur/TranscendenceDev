//	TSESpaceObjectsEnum.h
//
//	Transcendence Space Engine
//	Copyright 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

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

class CAnyObjSelector
	{
	public:
		inline Metric GetMaxRange (void) const
			{
			return g_InfiniteDistance;
			}

		inline bool Matches (CSpaceObject *pObj, Metric rDist2) const
			{
			return (!pObj->IsIntangible() || pObj->IsVirtual());
			}

		inline bool MatchesCategory (CSpaceObject *pObj) const
			{
			return true;
			}
	};

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
			return (!pObj->IsIntangible() || pObj->IsVirtual())
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
		template <class RANGE, class SELECTOR> static CSpaceObject *FindNearestEnemyObj (const CSystem &System, CSpaceObject &SourceObj, RANGE &Range, SELECTOR &Selector)
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

		template <class RANGE, class SELECTOR> static CSpaceObject *FindNearestObj (const CSystem &System, RANGE &Range, SELECTOR &Selector)
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

		template <class RANGE, class SELECTOR> static CSpaceObject *FindObjInRange (const CSystem &System, RANGE &Range, SELECTOR &Selector)
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

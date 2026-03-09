//	SystemImpl.h
//
//	Helpers and utility classes
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CAStarPathFinder
	{
	public:
		CAStarPathFinder (void);
		~CAStarPathFinder (void);

		void AddObstacle (const CVector &vPos, const Metric &rSize);
		void AddBarrier (const CVector &vUR, const CVector &vLL);
		int FindPath (const CVector &vStart, const CVector &vEnd, CVector **retPathList, bool bTryReverse = true);
#ifdef DEBUG_ASTAR_PATH
		const TArray<CVector> &GetDebugPointsChecked (void) const { return m_PointsChecked; }
#endif

		static bool LineIntersectsRect (const CVector &vStart, const CVector &vEnd, const CVector &vUR, const CVector &vLL);

	private:
		struct SObstacle
			{
			CVector vPos;
			Metric rSize;
			};
		struct SBarrier
			{
			CVector vUR;
			CVector vLL;
			};

		struct SNode
			{
			int x;
			int y;

			CVector vPos;
			int iCostFromStart;
			int iHeuristic;
			int iTotalCost;

			SNode *pParent;
			SNode *pPrev;
			SNode *pNext;
			};

		typedef SNode *SNodeRoot;

		void AddToClosedList (SNode *pNew);
		void AddToOpenList (SNode *pNew);
		int CalcHeuristic (const CVector &vPos, const CVector &vDest);
		void CollapsePath (TArray<SNode *> &Path, int iStart, int iEnd);
		void CreateInOpenList (const CVector &vEnd, SNode *pCurrent, int xDir, int yDir);
		bool IsInClosedList (int x, int y);
		bool IsPathClear (const CVector &vStart, const CVector &vEnd);
		bool IsPointClear (const CVector &vPos);
		int OptimizePath (const CVector &vEnd, SNode *pFinal, CVector **retPathList);
		void Reset (void);

		void AddToList (SNodeRoot &pRoot, SNode *pNew, SNode *pAddAfter = NULL);
		void DeleteFromList (SNodeRoot &pRoot, SNode *pDelete);
		void DeleteList (SNodeRoot &pRoot);

		TArray<SObstacle> m_Obstacles;
		TArray<SBarrier> m_Barriers;
		SNodeRoot m_pOpenList;
		SNodeRoot m_pClosedList;
		CTileMap *m_pClosedMap;

#ifdef DEBUG_ASTAR_PATH
		TArray<CVector> m_PointsChecked;
#endif

#ifdef DEBUG_ASTAR_PERF
		int m_iCallsToIsPathClear;
		int m_iClosedListCount;
		int m_iOpenListCount;
#endif
	};


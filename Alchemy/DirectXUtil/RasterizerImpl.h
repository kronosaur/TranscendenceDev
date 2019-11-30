//	RasterizerImpl.h
//
//	Classes for rasterization
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SEdgeState
	{
	SEdgeState *pNextEdge;
	int x;
	int yStart;
	int xWholePixelMove;
	int xDirection;
	int iErrorTerm;
	int iErrorTermAdjUp;
	int iErrorTermAdjDown;
	int iCount;
	};

class CSortedEdgeList
	{
	public:
		CSortedEdgeList (void) :
				m_pFirst(NULL),
				m_pNext(NULL)
			{ }

		void Advance (void);
		SEdgeState *GetFirst (void) const { return m_pFirst; }
		CSortedEdgeList *GetNext (void) const { return m_pNext; }
		void Insert (SEdgeState *pEdge);
		void Insert (CSortedEdgeList &EdgeList);
		bool IsEmpty (void) const { return (m_pFirst == NULL); }
		void SetNext (CSortedEdgeList *pNext) { m_pNext = pNext; }
		void Sort (void);

	private:
		SEdgeState *m_pFirst;
		CSortedEdgeList *m_pNext;
	};

class CGlobalEdgeTable
	{
	public:
		CGlobalEdgeTable (void) :
				m_pFirst(NULL),
				m_pLast(NULL)
			{ }

		~CGlobalEdgeTable (void);

		CSortedEdgeList *GetFirst (void) const { return m_pFirst;  }
		CSortedEdgeList *GetLast (void) const { return m_pLast; }
		void Insert (SEdgeState *pEdge);

	private:
		CSortedEdgeList *m_pFirst;
		CSortedEdgeList *m_pLast;
	};

class CPolygonRasterizer
	{
	public:
		CPolygonRasterizer (const CGPath &Path, int iAntiAlias = 3);
		~CPolygonRasterizer (void);

		void Rasterize (CGRegion *retRegion);

	private:
		void AddPolygonToGET (const TArray<CVector> &Points);
		void RasterizeLine (CGRunList &Runs);

		TArray<SEdgeState *> m_Edges;		//	An array of allocated edge arrays
		CGlobalEdgeTable m_GET;				//	Global Edge Table
		CSortedEdgeList m_AET;				//	Active Edge Table

		int m_iAntiAlias;					//	Number of sub-pixel samples per dimension
	};

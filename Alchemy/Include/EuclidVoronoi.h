//	EuclidVoronoi.h
//
//	Voronoi classes
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CVoronoiTessellation
	{
	public:
		inline int GetNeighbor (int iSiteIndex, int iIndex) const { return m_Sites[iSiteIndex].Neighbors[iIndex]; }
		inline int GetNeighborCount (int iSiteIndex) const { return m_Sites[iSiteIndex].Neighbors.GetCount(); }
		inline const CLine &GetNeighborEdge (int iSiteIndex, int iIndex) const { return m_Edges[m_Sites[iSiteIndex].Edges[iIndex]].Edge; }
		inline int GetSiteCount (void) const { return m_Sites.GetCount(); }
		inline DWORD GetSiteData (int iIndex) const { return m_Sites[iIndex].dwData; }
		inline void GetSiteNeighbors (int iIndex, TArray<int> *retNeighbors) const { *retNeighbors = m_Sites[iIndex].Neighbors; }
		inline const CVector &GetSiteOrigin (int iIndex) const { return m_Sites[iIndex].Origin; }
		inline void GetSitePolygon (int iIndex, TArray<CVector> *retPoints) const { *retPoints = m_Sites[iIndex].Polygon; }
		void Init (const TArray<CVector> &Points, Metric rWidth = 0.0, Metric rHeight = 0.0);
		inline void SetSiteData (int iIndex, DWORD dwData) { m_Sites[iIndex].dwData = dwData; }

	private:
		struct SEdge
			{
			CLine Edge;
			int iSite1;
			int iSite2;
			};

		struct SSite
			{
			CVector Origin;					//	Original point
			TArray<CVector> Polygon;		//	Polygon points
			TArray<int> Edges;				//	Index into edges table
			TArray<int> Neighbors;			//	Index into sites table

			DWORD dwData;					//	User data

			TArray<int> LeftToMatch;
			};

		void InsertEdge (SSite &Site, int iEdge);
		bool InsertEdgeToPolygon (SSite &Site, int iEdge, bool bDefer = true);
		void ValidatePolygon (SSite &Site);

		TArray<SSite> m_Sites;
		TArray<SEdge> m_Edges;

		Metric m_rWidth;
		Metric m_rHeight;
	};

struct SVoronoiEdge
	{
	CLine Edge;
	int iSite1;
	int iSite2;
	};

void mathVoronoi (const TArray<CVector> &Points, TArray<CLine> *retSegments, Metric rWidth = 0.0, Metric rHeight = 0.0);
void mathVoronoiEx (const TArray<CVector> &Points, TArray<SVoronoiEdge> *retSegments, Metric rWidth = 0.0, Metric rHeight = 0.0);


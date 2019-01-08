//	CVoronoiTessellation.cpp
//
//	CVoronoiTessellation class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"

#include "Euclid.h"

const double POINT_IDENTITY_EPSILON =			1.0e-05;
const double POINT_IDENTITY_EPSILON2 =			POINT_IDENTITY_EPSILON * POINT_IDENTITY_EPSILON;

void CVoronoiTessellation::Init (const TArray<CVector> &Points, Metric rWidth, Metric rHeight)

//	Init
//
//	Initialize with the given set of points.

	{
	int i;

	m_Sites.DeleteAll();
	m_Edges.DeleteAll();

	m_rWidth = rWidth;
	m_rHeight = rHeight;

	if (Points.GetCount() > 0)
		{
		//	Build up all the sites

		m_Sites.InsertEmpty(Points.GetCount());
		for (i = 0; i < Points.GetCount(); i++)
			{
			m_Sites[i].Origin = Points[i];
			m_Sites[i].dwData = 0;
			}

		//	Execute Fortune's algorithm

		TArray<SVoronoiEdge> Edges;
		mathVoronoiEx(Points, &Edges, m_rWidth, m_rHeight);

		//	Add all of the edges to our edge table

		m_Edges.InsertEmpty(Edges.GetCount());
		for (i = 0; i < Edges.GetCount(); i++)
			{
			m_Edges[i].Edge = Edges[i].Edge;
			m_Edges[i].iSite1 = Edges[i].iSite1;
			m_Edges[i].iSite2 = Edges[i].iSite2;

			//	Now add this edge to both sites

			InsertEdge(m_Sites[Edges[i].iSite1], i);
			InsertEdge(m_Sites[Edges[i].iSite2], i);

			//	Add the site to each other's neighbors

			m_Sites[Edges[i].iSite1].Neighbors.Insert(Edges[i].iSite2);
			m_Sites[Edges[i].iSite2].Neighbors.Insert(Edges[i].iSite1);
			}

		//	Loop over all sites and make sure we've got valid polygons.

		for (i = 0; i < m_Sites.GetCount(); i++)
			ValidatePolygon(m_Sites[i]);
		}
	}

void CVoronoiTessellation::InsertEdge (SSite &Site, int iEdge)

//	InsertEdge
//
//	Inserts the given edge to the site in order.

	{
	int i;

	//	Add to the edges table

	Site.Edges.Insert(iEdge);

	//	Add each of the points to the polygon in the proper order.

	const SEdge &NewEdge = m_Edges[iEdge];
	if (Site.Polygon.GetCount() == 0)
		{
		Site.Polygon.Insert(NewEdge.Edge.From());
		Site.Polygon.Insert(NewEdge.Edge.To());
		}
	else
		{
		//	Try to insert the edge in the polygon list. If we succeed, then 
		//	see if any deferred edges can be inserted now.

		if (InsertEdgeToPolygon(Site, iEdge))
			{
			for (i = 0; i < Site.LeftToMatch.GetCount(); i++)
				{
				if (InsertEdgeToPolygon(Site, Site.LeftToMatch[i], false))
					{
					Site.LeftToMatch.Delete(i);
					i--;
					}
				}
			}
		}
	}

bool CVoronoiTessellation::InsertEdgeToPolygon (SSite &Site, int iEdge, bool bDefer)

//	InsertEdgeToPolygon
//
//	Inserts the given edge to the polygon list. We return TRUE if we added the
//	edge.

	{
	const SEdge &NewEdge = m_Edges[iEdge];

	//	We either add at the end or at the beginning

	if (Site.Polygon[0].Distance2(NewEdge.Edge.From()) < POINT_IDENTITY_EPSILON2)
		{
		Site.Polygon.Insert(NewEdge.Edge.To(), 0);
		return true;
		}

	if (Site.Polygon[0].Distance2(NewEdge.Edge.To()) < POINT_IDENTITY_EPSILON2)
		{
		Site.Polygon.Insert(NewEdge.Edge.From(), 0);
		return true;
		}

	if (Site.Polygon[Site.Polygon.GetCount() - 1].Distance2(NewEdge.Edge.From()) < POINT_IDENTITY_EPSILON2)
		{
		Site.Polygon.Insert(NewEdge.Edge.To());
		return true;
		}

	if (Site.Polygon[Site.Polygon.GetCount() - 1].Distance2(NewEdge.Edge.To()) < POINT_IDENTITY_EPSILON2)
		{
		Site.Polygon.Insert(NewEdge.Edge.From());
		return true;
		}

	//	If we did not match and we want to defer, add the edge

	if (bDefer)
		Site.LeftToMatch.Insert(iEdge);

	return false;
	}

void CVoronoiTessellation::ValidatePolygon (SSite &Site)

//	ValidatePolygon
//
//	Makes sure this is a valid polygon. If not, deletes it.

	{
	int i;

	//	If we could not complete this polygon then it means that it was
	//	on the edge and we don't want it.
	//
	//	LATER: We should intersect it with the borders.

	if (Site.LeftToMatch.GetCount() > 0)
		{
		Site.Polygon.DeleteAll();
		Site.LeftToMatch.DeleteAll();
		}

	//	Otherwise, check to see if any points are really close to each other
	//	(And delete the extra ones.)

	else if (Site.Polygon.GetCount() > 0)
		{
		for (i = 0; i < Site.Polygon.GetCount(); i++)
			{
			const CVector &vP1 = Site.Polygon[i];
			const CVector &vP2 = Site.Polygon[(i + 1) % Site.Polygon.GetCount()];

			if (vP1.Distance2(vP2) < POINT_IDENTITY_EPSILON2)
				{
				Site.Polygon.Delete(i);
				i--;
				}
			}

		//	Make sure we're left with a valid polygon

		if (Site.Polygon.GetCount() < 3)
			Site.Polygon.DeleteAll();
		}
	}


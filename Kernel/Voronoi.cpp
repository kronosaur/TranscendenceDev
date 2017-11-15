//	Voronoi.cpp
//
//	Voronoi functions
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"

#include "Euclid.h"

#include "VoronoiGenerator.h"

void mathVoronoiEx (const TArray<CVector> &Points, TArray<SVoronoiEdge> *retSegments, Metric rWidth, Metric rHeight)

//	mathVoronoiEx
//
//	Returns all the Voronoi segments for the given points.

	{
	int i;

	//	Short-circuit

	retSegments->DeleteAll();
	if (Points.GetCount() <= 0)
		return;

	//	Keep track of bounds

	Metric rMinX = -0.5 * rWidth;
	Metric rMaxX = 0.5 * rWidth;
	Metric rMinY = -0.5 * rHeight;
	Metric rMaxY = 0.5 * rHeight;

	//	The generator expects a sharded array of points.

	VFLOAT *pInputX = new VFLOAT [Points.GetCount()];
	VFLOAT *pInputY = new VFLOAT [Points.GetCount()];
	for (i = 0; i < Points.GetCount(); i++)
		{
		pInputX[i] = Points[i].X();
		pInputY[i] = Points[i].Y();

		if (Points[i].X() < rMinX) rMinX = Points[i].X();
		if (Points[i].X() > rMaxX) rMaxX = Points[i].X();
		if (Points[i].Y() < rMinY) rMinY = Points[i].Y();
		if (Points[i].Y() > rMaxY) rMaxY = Points[i].Y();
		}

	VoronoiDiagramGenerator Generator;
	Generator.generateVoronoi(pInputX, pInputY, Points.GetCount(), rMinX, rMaxX, rMinY, rMaxY, 0.0, false);

	//	Get the output

	int iEdgeCount = Generator.getEdgeCount();
	retSegments->InsertEmpty(iEdgeCount);
	
	Generator.resetIterator();

	VFLOAT x1, y1, x2, y2;
	int iSite1, iSite2;
	i = 0;
	while (Generator.getNext(x1, y1, x2, y2, &iSite1, &iSite2))
		{
		ASSERT(i < retSegments->GetCount());

		SVoronoiEdge &NewEdge = retSegments->GetAt(i);
		NewEdge.Edge.SetFrom(CVector(x1, y1));
		NewEdge.Edge.SetTo(CVector(x2, y2));
		NewEdge.iSite1 = iSite1;
		NewEdge.iSite2 = iSite2;

		i++;
		}

	//	Done

	delete [] pInputX;
	delete [] pInputY;
	}

void mathVoronoi (const TArray<CVector> &Points, TArray<CLine> *retSegments, Metric rWidth, Metric rHeight)

//	mathVoronoi
//
//	Returns all the Voronoi segments for the given points.

	{
	int i;

	//	Short-circuit

	retSegments->DeleteAll();
	if (Points.GetCount() <= 0)
		return;

	//	Keep track of bounds

	Metric rMinX = -0.5 * rWidth;
	Metric rMaxX = 0.5 * rWidth;
	Metric rMinY = -0.5 * rHeight;
	Metric rMaxY = 0.5 * rHeight;

	//	The generator expects a sharded array of points.

	VFLOAT *pInputX = new VFLOAT [Points.GetCount()];
	VFLOAT *pInputY = new VFLOAT [Points.GetCount()];
	for (i = 0; i < Points.GetCount(); i++)
		{
		pInputX[i] = Points[i].X();
		pInputY[i] = Points[i].Y();

		if (Points[i].X() < rMinX) rMinX = Points[i].X();
		if (Points[i].X() > rMaxX) rMaxX = Points[i].X();
		if (Points[i].Y() < rMinY) rMinY = Points[i].Y();
		if (Points[i].Y() > rMaxY) rMaxY = Points[i].Y();
		}

	VoronoiDiagramGenerator Generator;
	Generator.generateVoronoi(pInputX, pInputY, Points.GetCount(), rMinX, rMaxX, rMinY, rMaxY, 0.0, false);

	//	Get the output

	int iEdgeCount = Generator.getEdgeCount();
	retSegments->InsertEmpty(iEdgeCount);
	
	Generator.resetIterator();

	VFLOAT x1, y1, x2, y2;
	i = 0;
	while (Generator.getNext(x1, y1, x2, y2))
		{
		ASSERT(i < retSegments->GetCount());

		retSegments->GetAt(i) = CLine(CVector(x1, y1), CVector(x2, y2));
		i++;
		}

	//	Done

	delete [] pInputX;
	delete [] pInputY;
	}

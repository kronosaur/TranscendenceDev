//	CalcConvexHull.cpp
//
//	Calculates convex hull for a point cloud
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"

#include "Euclid.h"

#include "Kernel\quickhull\QuickHull.hpp"

using namespace quickhull;

void CGeometry3D::CalcConvexHull (const TArray<CVector3D> &Points, TArray<SIndexedLine> &Edges)

//	CalcConvexHull
//
//	Computes the convex hull of the given point cloud, using the Quickhull 
//	algorithm.

	{
	size_t i;

	Edges.DeleteAll();

	QuickHull<Metric> qh; // Could be double as well
	auto mesh = qh.getConvexHull((Metric *)&Points[0], Points.GetCount(), true, true);
	const auto &indices = mesh.getIndexBuffer();

	//	Allocate space

	Edges.GrowToFit(indices.size() / 4);

	//	The resulting index array is a set of triangle points, so we need to 
	//	remove duplicate lines (because each line will be part of two triangles).

	TSortMap<DWORDLONG, bool> Seen;
	Seen.GrowToFit(indices.size() / 2);

	for (i = 1; i < indices.size(); i++)
		{
		SIndexedLine NewLine{ (int)indices[i - 1], (int)indices[i] };
		if (NewLine.iEnd < NewLine.iStart)
			Swap(NewLine.iEnd, NewLine.iStart);

		if (Seen.Find(*(DWORDLONG *)&NewLine))
			continue;

		Edges.Insert(NewLine);
		Seen.Insert(*(DWORDLONG *)&NewLine, true);
		}
	}

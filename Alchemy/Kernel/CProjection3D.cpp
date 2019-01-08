//	CProjection3D.cpp
//
//	CProjection3D Class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"

#include "Euclid.h"

TArray<CVector> CProjection3D::Transform (const CXForm3D &Xform, const TArray<CVector3D> &Points) const

//	Transform
//
//	Transforms an array of points.

	{
	int i;

	TArray<CVector> Result;
	Result.InsertEmpty(Points.GetCount());

	for (i = 0; i < Points.GetCount(); i++)
		Result[i] = Transform(Xform, Points[i]);

	return Result;
	}

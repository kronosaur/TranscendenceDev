//	CSphericalCoordinates.cpp
//
//	CSphericalCoordinates Class
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"

#include "Euclid.h"

CVector3D CSphericalCoordinates::FromCartessian (const CVector3D &vPos)

//	FromCartessian
//
//	Returns a spherical coordinate vector:
//
//	x = radius
//	y = zenith angle (angle from straight up), in radians
//	z = azimuth, in radians

	{
	Metric rRadius = vPos.Length();
	if (rRadius == 0.0)
		return CVector3D();

	return CVector3D(rRadius, acos(vPos.GetZ() / rRadius), atan2(vPos.GetY(), vPos.GetX()));
	}

CVector3D CSphericalCoordinates::ToCartessian (const CVector3D &vPos)

//	ToCartessian
//
//	Returns cartessian coordinate from a spherical vector.

	{
	return CVector3D(vPos.GetX() * sin(vPos.GetY()) * cos(vPos.GetZ()), vPos.GetX() * sin(vPos.GetY()) * sin(vPos.GetZ()), vPos.GetX() * cos(vPos.GetY()));
	}

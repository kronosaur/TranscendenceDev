//	CXForm3D.cpp
//
//	CXForm3D Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Euclid.h"

CXForm3D::CXForm3D (XFormType type)
	{
	ASSERT(type == xformIdentity);

	m_Xform[0][0] = 1.0;
	m_Xform[0][1] = 0.0;
	m_Xform[0][2] = 0.0;
	m_Xform[0][3] = 0.0;

	m_Xform[1][0] = 0.0;
	m_Xform[1][1] = 1.0;
	m_Xform[1][2] = 0.0;
	m_Xform[1][3] = 0.0;

	m_Xform[2][0] = 0.0;
	m_Xform[2][1] = 0.0;
	m_Xform[2][2] = 1.0;
	m_Xform[2][3] = 0.0;

	m_Xform[3][0] = 0.0;
	m_Xform[3][1] = 0.0;
	m_Xform[3][2] = 0.0;
	m_Xform[3][3] = 1.0;
	}

CXForm3D::CXForm3D (XFormType type, Metric rX, Metric rY, Metric rZ)
	{
	switch (type)
		{
		case xformTranslate:
			m_Xform[0][0] = 1.0;
			m_Xform[0][1] = 0.0;
			m_Xform[0][2] = 0.0;
			m_Xform[0][3] = 0.0;

			m_Xform[1][0] = 0.0;
			m_Xform[1][1] = 1.0;
			m_Xform[1][2] = 0.0;
			m_Xform[1][3] = 0.0;

			m_Xform[2][0] = 0.0;
			m_Xform[2][1] = 0.0;
			m_Xform[2][2] = 1.0;
			m_Xform[2][3] = 0.0;

			m_Xform[3][0] = rX;
			m_Xform[3][1] = rY;
			m_Xform[3][2] = rZ;
			m_Xform[3][3] = 1.0;
			break;

		case xformScale:
			m_Xform[0][0] = rX;
			m_Xform[0][1] = 0.0;
			m_Xform[0][2] = 0.0;
			m_Xform[0][3] = 0.0;

			m_Xform[1][0] = 0.0;
			m_Xform[1][1] = rY;
			m_Xform[1][2] = 0.0;
			m_Xform[1][3] = 0.0;

			m_Xform[2][0] = 0.0;
			m_Xform[2][1] = 0.0;
			m_Xform[2][2] = rZ;
			m_Xform[2][3] = 0.0;

			m_Xform[3][0] = 0.0;
			m_Xform[3][1] = 0.0;
			m_Xform[3][2] = 0.0;
			m_Xform[3][3] = 1.0;
			break;

		case xformRotate:
			*this = CXForm3D(xformRotateX, rX) * CXForm3D(xformRotateY, rY) * CXForm3D(xformRotateZ, rZ);
			break;

		default:
			ASSERT(false);
		}
	}

CXForm3D::CXForm3D (XFormType type, const CVector3D &vVector)
	{
	*this = CXForm3D(type, vVector.GetX(), vVector.GetY(), vVector.GetZ());
	}

CXForm3D::CXForm3D (XFormType type, Metric rRadians)
	{
	switch (type)
		{
		case xformRotateX:
			m_Xform[0][0] = 1.0;
			m_Xform[0][1] = 0.0;
			m_Xform[0][2] = 0.0;
			m_Xform[0][3] = 0.0;

			m_Xform[1][0] = 0.0;
			m_Xform[1][1] = cos(rRadians);
			m_Xform[1][2] = -sin(rRadians);
			m_Xform[1][3] = 0.0;

			m_Xform[2][0] = 0.0;
			m_Xform[2][1] = sin(rRadians);
			m_Xform[2][2] = cos(rRadians);
			m_Xform[2][3] = 0.0;

			m_Xform[3][0] = 0.0;
			m_Xform[3][1] = 0.0;
			m_Xform[3][2] = 0.0;
			m_Xform[3][3] = 1.0;
			break;

		case xformRotateY:
			m_Xform[0][0] = cos(rRadians);
			m_Xform[0][1] = 0.0;
			m_Xform[0][2] = sin(rRadians);
			m_Xform[0][3] = 0.0;

			m_Xform[1][0] = 0.0;
			m_Xform[1][1] = 1.0;
			m_Xform[1][2] = 0.0;
			m_Xform[1][3] = 0.0;

			m_Xform[2][0] = -sin(rRadians);
			m_Xform[2][1] = 0.0;
			m_Xform[2][2] = cos(rRadians);
			m_Xform[2][3] = 0.0;

			m_Xform[3][0] = 0.0;
			m_Xform[3][1] = 0.0;
			m_Xform[3][2] = 0.0;
			m_Xform[3][3] = 1.0;
			break;

		case xformRotate:
		case xformRotateZ:
			m_Xform[0][0] = cos(rRadians);
			m_Xform[0][1] = -sin(rRadians);
			m_Xform[0][2] = 0.0;
			m_Xform[0][3] = 0.0;

			m_Xform[1][0] = sin(rRadians);
			m_Xform[1][1] = cos(rRadians);
			m_Xform[1][2] = 0.0;
			m_Xform[1][3] = 0.0;

			m_Xform[2][0] = 0.0;
			m_Xform[2][1] = 0.0;
			m_Xform[2][2] = 1.0;
			m_Xform[2][3] = 0.0;

			m_Xform[3][0] = 0.0;
			m_Xform[3][1] = 0.0;
			m_Xform[3][2] = 0.0;
			m_Xform[3][3] = 1.0;
			break;

		default:
			ASSERT(false);
		}
	}

CVector3D CXForm3D::Transform (const CVector3D &vVector) const
	{
	return CVector3D(
			vVector.GetX() * m_Xform[0][0] + vVector.GetY() * m_Xform[1][0] + vVector.GetZ() * m_Xform[2][0] + m_Xform[3][0],
			vVector.GetX() * m_Xform[0][1] + vVector.GetY() * m_Xform[1][1] + vVector.GetZ() * m_Xform[2][1] + m_Xform[3][1],
			vVector.GetX() * m_Xform[0][2] + vVector.GetY() * m_Xform[1][2] + vVector.GetZ() * m_Xform[2][2] + m_Xform[3][2]
			);
	}

CVector3D CXForm3D::Transform (const CVector3D &vVector, Metric *retrW) const
	{
	*retrW = vVector.GetX() * m_Xform[0][3] + vVector.GetY() * m_Xform[1][3] + vVector.GetZ() * m_Xform[2][3] + m_Xform[3][3];

	return CVector3D(
			vVector.GetX() * m_Xform[0][0] + vVector.GetY() * m_Xform[1][0] + vVector.GetZ() * m_Xform[2][0] + m_Xform[3][0],
			vVector.GetX() * m_Xform[0][1] + vVector.GetY() * m_Xform[1][1] + vVector.GetZ() * m_Xform[2][1] + m_Xform[3][1],
			vVector.GetX() * m_Xform[0][2] + vVector.GetY() * m_Xform[1][2] + vVector.GetZ() * m_Xform[2][2] + m_Xform[3][2]
			);
	}

const CXForm3D operator* (const CXForm3D &op1, const CXForm3D &op2)
	{
	CXForm3D Result;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Result.m_Xform[i][j] = 
					  op1.m_Xform[i][0] * op2.m_Xform[0][j]
					+ op1.m_Xform[i][1] * op2.m_Xform[1][j]
					+ op1.m_Xform[i][2] * op2.m_Xform[2][j]
					+ op1.m_Xform[i][3] * op2.m_Xform[3][j];

	return Result;
	}

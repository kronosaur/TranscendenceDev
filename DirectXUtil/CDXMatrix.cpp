//	CDXMatrix.cpp
//
//	CDXMatrix class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include <windows.h>
#include <dsound.h>
#include "Alchemy.h"
#include "DirectXUtil.h"
#include "DX3DImpl.h"

CDXMatrix::CDXMatrix (void)

//	CDXMatrix constructor

	{
	m_Matrix._11 = 0.0;
	m_Matrix._12 = 0.0;
	m_Matrix._13 = 0.0;
	m_Matrix._14 = 0.0;

	m_Matrix._21 = 0.0;
	m_Matrix._22 = 0.0;
	m_Matrix._23 = 0.0;
	m_Matrix._24 = 0.0;

	m_Matrix._31 = 0.0;
	m_Matrix._32 = 0.0;
	m_Matrix._33 = 0.0;
	m_Matrix._34 = 0.0;

	m_Matrix._41 = 0.0;
	m_Matrix._42 = 0.0;
	m_Matrix._43 = 0.0;
	m_Matrix._44 = 0.0;
	}

void CDXMatrix::SetIdentity (void)

//	SetIdentity
//
//	Initializes to identity matrix

	{
	m_Matrix._11 = 1.0f;
	m_Matrix._12 = 0.0;
	m_Matrix._13 = 0.0;
	m_Matrix._14 = 0.0;

	m_Matrix._21 = 0.0;
	m_Matrix._22 = 1.0f;
	m_Matrix._23 = 0.0;
	m_Matrix._24 = 0.0;

	m_Matrix._31 = 0.0;
	m_Matrix._32 = 0.0;
	m_Matrix._33 = 1.0f;
	m_Matrix._34 = 0.0;

	m_Matrix._41 = 0.0;
	m_Matrix._42 = 0.0;
	m_Matrix._43 = 0.0;
	m_Matrix._44 = 1.0f;
	}

void CDXMatrix::SetOrthoLH (FLOAT rWidth, FLOAT rHeight, FLOAT rNear, FLOAT rFar)

//	SetOrthoLH
//
//	Initializes to ortho projection

	{
	ASSERT(rWidth >= 0.0);
	ASSERT(rHeight >= 0.0);
	ASSERT(rNear != rFar);

	m_Matrix._11 = 2.0f / rWidth;
	m_Matrix._12 = 0.0;
	m_Matrix._13 = 0.0;
	m_Matrix._14 = 0.0;

	m_Matrix._21 = 0.0;
	m_Matrix._22 = 2.0f / rHeight;
	m_Matrix._23 = 0.0;
	m_Matrix._24 = 0.0;

	m_Matrix._31 = 0.0;
	m_Matrix._32 = 0.0;
	m_Matrix._33 = 1.0f / (rFar - rNear);
	m_Matrix._34 = 0.0;

	m_Matrix._41 = 0.0;
	m_Matrix._42 = 0.0;
	m_Matrix._43 = rNear / (rNear - rFar);
	m_Matrix._44 = 1.0f;
	}

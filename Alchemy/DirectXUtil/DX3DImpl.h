//	DX3DImpl.h
//
//	Some internal helpers for DirectX 3D
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SVertexFormatStd
{
	FLOAT x, y, z;
	DWORD color;
	FLOAT u, v;
};

#define D3DFVF_VERTEXFORMAT_STD (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

class CDXMatrix
	{
	public:
		CDXMatrix (void);

		inline operator D3DMATRIX () const { return m_Matrix; }
		inline operator const D3DMATRIX * () const { return &m_Matrix; }

		void SetIdentity (void);
		void SetOrthoLH (FLOAT rWidth, FLOAT rHeight, FLOAT rNear, FLOAT rFar);

	private:
		D3DMATRIX m_Matrix;
	};
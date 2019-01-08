//	EuclidMatrix.h
//
//	Matrix Classes
//	Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CMatrix
	{
	public:
		CMatrix (void);
		CMatrix (const CMatrix &Src);
		CMatrix (int iRows, int iCols);
		~CMatrix (void);

		CMatrix &operator= (const CMatrix &Src);

		void CleanUp (void);
		void Fill (Metric rValue);
		void Init (int iRows, int iCols, Metric rValue = 0.0);

	private:
		void Alloc (int iRows, int iCols);
		void Copy (const CMatrix &Src);

		int m_iRows;
		int m_iCols;
		Metric *m_Array;
	};

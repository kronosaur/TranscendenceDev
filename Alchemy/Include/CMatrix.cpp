//	CMatrix.cpp
//
//	CMatrix Class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"

#include "Euclid.h"

CMatrix::CMatrix (void) :
		m_iRows(0),
		m_iCols(0),
		m_Array(NULL)

//	CMatrix constructor

	{
	}

CMatrix::CMatrix (const CMatrix &Src)

//	CMatrix constructor

	{
	Copy(Src);
	}

CMatrix::CMatrix (int iRows, int iCols)

//	CMatrix constructor

	{
	Alloc(iRows, iCols);
	Fill(0.0);
	}

CMatrix::~CMatrix (void)

//	CMatrix destructor

	{
	CleanUp();
	}

CMatrix &CMatrix::operator= (const CMatrix &Src)

//	CMatrix operator =

	{
	CleanUp();
	Copy(Src);
	return *this;
	}

void CMatrix::Alloc (int iRows, int iCols)

//	Alloc
//
//	Allocates a new matrix of the given size. Assumes that we're empty.

	{
	if (iRows <= 0 || iCols <= 0)
		{
		m_iRows = 0;
		m_iCols = 0;
		m_Array = NULL;
		}

	m_iRows = iRows;
	m_iCols = iCols;
	m_Array = new Metric [m_iRows * m_iCols];
	}

void CMatrix::CleanUp (void)

//	CleanUp
//
//	Free resources

	{
	if (m_Array)
		{
		delete [] m_Array;
		m_Array = NULL;
		}
	}

void CMatrix::Copy (const CMatrix &Src)

//	Copy
//
//	Copy from source. Assumes that we are empty.

	{
	int i;
	int iCount = Src.m_iRows * Src.m_iCols;

	Alloc(Src.m_iRows, Src.m_iCols);
	for (i = 0; i < iCount; i++)
		m_Array[i] = Src.m_Array[i];
	}

void CMatrix::Fill (Metric rValue)

//	Fill
//
//	Set the value of all cells.

	{
	int i;
	int iCount = m_iRows * m_iCols;

	for (i = 0; i < iCount; i++)
		m_Array[i] = rValue;
	}

void CMatrix::Init (int iRows, int iCols, Metric rValue)

//	Init
//
//	Initializes to an empty array

	{
	CleanUp();
	Alloc(iRows, iCols);
	Fill(rValue);
	}

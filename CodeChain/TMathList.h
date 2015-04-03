//	TMathList.h
//
//	Helpers for CodeChain functions
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once;

template <class COMPUTATION> class TMathReduce
	{
	public:
		ICCItem *Run (CCodeChain &CC, ICCItem *pList)
			{
			ASSERT(pList->GetCount() > 0);

			ICCItem *pFirst = pList->GetElement(0);

			//	If the first value is an integer, reduce until we find a float

			int iPos = 1;
			int iResult;
			double rResult;
			if (pFirst->IsInteger())
				{
				iResult = pFirst->GetIntegerValue();
				while (iPos < pList->GetCount() && pList->GetElement(iPos)->IsInteger())
					{
					iResult = COMPUTE(iResult, pList->GetElement(iPos)->GetIntegerValue());
					iPos++;
					}

				if (iPos == pList->GetCount())
					return CC.CreateInteger(iResult);
				else
					rResult = iResult;
				}
			else
				rResult = pFirst->GetDoubleValue();

			while (iPos < pList->GetCount())
				{
				rResult = COMPUTE(rResult, pList->GetElement(iPos)->GetDoubleValue());
				iPos++;
				}

			return CC.CreateDouble(rResult);
			}

	private:
		inline int COMPUTE (int iX, int iY) const { return ((COMPUTATION *)this)->Compute(iX, iY); }
		inline double COMPUTE (double rX, double rY) const { return ((COMPUTATION *)this)->Compute(rX, rY); }
	};

class CAddition : public TMathReduce<CAddition>
	{
	public:
		int Compute (int iX, int iY) const { return iX + iY; }
		double Compute (double rX, double rY) const { return rX + rY; }
	};

class CSubtraction : public TMathReduce<CSubtraction>
	{
	public:
		int Compute (int iX, int iY) const { return iX - iY; }
		double Compute (double rX, double rY) const { return rX - rY; }
	};

class CMax : public TMathReduce<CMax>
	{
	public:
		int Compute (int iX, int iY) const { return Max(iX, iY); }
		double Compute (double rX, double rY) const { return Max(rX, rY); }
	};

class CMin : public TMathReduce<CMin>
	{
	public:
		int Compute (int iX, int iY) const { return Min(iX, iY); }
		double Compute (double rX, double rY) const { return Min(rX, rY); }
	};

class CMultiplication : public TMathReduce<CMultiplication>
	{
	public:
		int Compute (int iX, int iY) const { return iX * iY; }
		double Compute (double rX, double rY) const { return rX * rY; }
	};

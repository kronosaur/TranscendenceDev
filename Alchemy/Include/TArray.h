//	TArray.h
//
//	TArray Class
//	Copyright 2019 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum ESortOptions
	{
	DescendingSort = 1,
	AscendingSort = -1,
	};

#ifdef DEBUG_MEMORY_LEAKS
#undef new
#endif

static constexpr int DEFAULT_ARRAY_GRANULARITY = 10;

class CArrayBase
	{
	public:
		void SetGranularity (int iGranularity) { if (m_pBlock == NULL) AllocBlock(::GetProcessHeap(), iGranularity); else m_pBlock->m_iGranularity = iGranularity; }

		static CString DebugGetStats (void);

	protected:
		struct SHeader
			{
			HANDLE m_hHeap;				//	Heap on which block is allocated
			int m_iSize;				//	Size of data portion (as seen by callers)
			int m_iAllocSize;			//	Current size of block
			int m_iGranularity;			//	Used by descendants to resize block
			};

		CArrayBase (HANDLE hHeap, int iGranularity);
		CArrayBase (SHeader *pBlock) : m_pBlock(pBlock)
			{ }

		~CArrayBase (void);

		void AllocBlock (HANDLE hHeap, int iGranularity);
		void CleanUpBlock (void);
		void CopyOptions (const CArrayBase &Src);
		void DeleteBytes (int iOffset, int iLength);
		char *GetBytes (void) const { return (m_pBlock ? (char *)(&m_pBlock[1]) : NULL); }
		int GetGranularity (void) const { return (m_pBlock ? m_pBlock->m_iGranularity : Kernel::DEFAULT_ARRAY_GRANULARITY); }
		HANDLE GetHeap (void) const { return (m_pBlock ? m_pBlock->m_hHeap : ::GetProcessHeap()); }
		int GetSize (void) const { return (m_pBlock ? m_pBlock->m_iSize : 0); }
		void InsertBytes (int iOffset, void *pData, int iLength, int iAllocQuantum);
		ALERROR Resize (int iNewSize, bool bPreserve, int iAllocQuantum);
		void TakeHandoffBase (CArrayBase &Src);

		SHeader *m_pBlock;
	};

#pragma warning(disable:4291)			//	No need for a delete because we're placing object
template <class VALUE> class TRawArray : public CArrayBase
	{
	public:
		TRawArray (void) : CArrayBase(NULL, DEFAULT_ARRAY_GRANULARITY), m_iExtraBytes(0) { }
		TRawArray (int iExtraBytes) : CArrayBase(NULL, DEFAULT_ARRAY_GRANULARITY), m_iExtraBytes(iExtraBytes) { }

		const VALUE &operator [] (int iIndex) const { return GetAt(iIndex); }
		VALUE &operator [] (int iIndex) { return GetAt(iIndex); }

		void Delete (int iIndex)
			{
			VALUE *pElement = (VALUE *)(GetBytes() + iIndex * GetElementSize());
			ASSERT((char *)pElement - GetBytes() < GetSize());
			pElement->VALUE::~VALUE();
			DeleteBytes(iIndex * GetElementSize(), GetElementSize());
			}

		void DeleteAll (void)
			{
			VALUE *pElement = (VALUE *)GetBytes();
			for (int i = 0; i < GetCount(); i++, pElement++)
				pElement->VALUE::~VALUE();

			CleanUpBlock();
			}

		const VALUE &GetAt (int iIndex) const
			{
			const VALUE *pElement = (VALUE *)(GetBytes() + iIndex * GetElementSize());
			return *pElement;
			}

		VALUE &GetAt (int iIndex)
			{
			VALUE *pElement = (VALUE *)(GetBytes() + iIndex * GetElementSize());
			return *pElement;
			}

		int GetCount (void) const { return (GetSize() / GetElementSize()); }

		int GetElementSize (void) const { return sizeof(VALUE) + m_iExtraBytes; }

		int GetExtraBytes (void) const { return m_iExtraBytes; }

		void Insert (const VALUE &Value, int iIndex = -1)
			{
			int iOffset;
			if (iIndex == -1) iIndex = GetCount();
			iOffset = iIndex * GetElementSize();
			InsertBytes(iOffset, NULL, GetElementSize(), DEFAULT_ARRAY_GRANULARITY * GetElementSize());

			VALUE *pElement = new(placement_new, GetBytes() + iOffset) VALUE(Value);
			}

		VALUE *InsertEmpty (int iCount = 1, int iIndex = -1)
			{
			int iOffset;
			if (iIndex == -1) iIndex = GetCount();
			iOffset = iIndex * GetElementSize();
			InsertBytes(iOffset, NULL, iCount * GetElementSize(), iCount * DEFAULT_ARRAY_GRANULARITY * GetElementSize());

			for (int i = 0; i < iCount; i++)
				{
				VALUE *pElement = new(placement_new, GetBytes() + iOffset + (i * GetElementSize())) VALUE;
				}

			return &GetAt(iIndex);
			}

		void SetExtraBytes (int iExtraBytes)
			{
			ASSERT(GetCount() == 0);
			m_iExtraBytes = iExtraBytes;
			}

	private:
		int m_iExtraBytes;
	};

template <class VALUE> class TArray : public Kernel::CArrayBase
	{
	public:
		TArray (void) : CArrayBase(NULL, DEFAULT_ARRAY_GRANULARITY) { }
		explicit TArray (HANDLE hHeap) : CArrayBase(hHeap, DEFAULT_ARRAY_GRANULARITY) { }
		explicit TArray (int iGranularity) : CArrayBase(NULL, iGranularity) { }
		TArray (const TArray<VALUE> &Obj) : CArrayBase(Obj.GetHeap(), Obj.GetGranularity())
			{
			InsertBytes(0, NULL, Obj.GetCount() * sizeof(VALUE), GetGranularity() * sizeof(VALUE));

			for (int i = 0; i < Obj.GetCount(); i++)
				{
				VALUE *pElement = new(placement_new, GetBytes() + (i * sizeof(VALUE))) VALUE(Obj[i]);
				}
			}
		TArray (TArray<VALUE> &&Src) : CArrayBase(Src.m_pBlock)
			{
			Src.m_pBlock = NULL;
			}

		~TArray (void) { DeleteAll(); }

		TArray<VALUE> &operator= (const TArray<VALUE> &Obj)
			{
			DeleteAll();

			CopyOptions(Obj);
			InsertBytes(0, NULL, Obj.GetCount() * sizeof(VALUE), GetGranularity() * sizeof(VALUE));

			for (int i = 0; i < Obj.GetCount(); i++)
				{
				VALUE *pElement = new(placement_new, GetBytes() + (i * sizeof(VALUE))) VALUE(Obj[i]);
				}

			return *this;
			}

		TArray<VALUE> &operator= (TArray<VALUE> &&Src)
			{
			DeleteAll();
			m_pBlock = Src.m_pBlock;
			Src.m_pBlock = NULL;
			return *this;
			}

		const VALUE &operator [] (int iIndex) const { return GetAt(iIndex); }
		VALUE &operator [] (int iIndex) { return GetAt(iIndex); }

		void Delete (int iIndex)
			{
			VALUE *pElement = (VALUE *)(GetBytes() + iIndex * sizeof(VALUE));
			ASSERT((char *)pElement - GetBytes() < GetSize());
			pElement->VALUE::~VALUE();
			DeleteBytes(iIndex * sizeof(VALUE), sizeof(VALUE));
			}

		void Delete (int iIndex, int iCount)
			{
			if (iIndex < 0 || iCount <= 0)
				return;

			iCount = Min(iCount, GetCount() - iIndex);
			VALUE *pElement = (VALUE *)(GetBytes() + iIndex * sizeof(VALUE));
			for (int i = 0; i < iCount; i++, pElement++)
				pElement->VALUE::~VALUE();

			DeleteBytes(iIndex * sizeof(VALUE), iCount * sizeof(VALUE));
			}

		void DeleteAll (void)
			{
			if (m_pBlock)
				{
				VALUE *pElement = (VALUE *)GetBytes();
				for (int i = 0; i < GetCount(); i++, pElement++)
					pElement->VALUE::~VALUE();

				CleanUpBlock();
				}
			}

		void DeleteValue (const VALUE &ToDelete)
			{
			for (int i = 0; i < GetCount(); i++)
				if (GetAt(i) == ToDelete)
					{
					Delete(i);
					i--;
					}
			}

		bool Find (const VALUE &ToFind, int *retiIndex = NULL) const
			{
			int iCount = GetCount();

			for (int i = 0; i < iCount; i++)
				if (GetAt(i) == ToFind)
					{
					if (retiIndex)
						*retiIndex = i;
					return true;
					}

			return false;
			}

		const VALUE &GetAt (int iIndex) const
			{
			ASSERT(iIndex >= 0 && iIndex < GetCount());
			const VALUE *pElement = (VALUE *)(GetBytes() + iIndex * sizeof(VALUE));
			return *pElement;
			}

		VALUE &GetAt (int iIndex)
			{
			ASSERT(iIndex >= 0 && iIndex < GetCount());
			VALUE *pElement = (VALUE *)(GetBytes() + iIndex * sizeof(VALUE));
			return *pElement;
			}

		int GetCount (void) const
			{
			return GetSize() / sizeof(VALUE);
			}

		void GrowToFit (int iCount)
			{
			if (iCount > 0)
				Resize(GetSize() + iCount * sizeof(VALUE), true, GetGranularity() * sizeof(VALUE));
			}

		void Insert (const VALUE &Value, int iIndex = -1)
			{
			int iOffset;
			if (iIndex == -1) iIndex = GetCount();
			iOffset = iIndex * sizeof(VALUE);
			InsertBytes(iOffset, NULL, sizeof(VALUE), GetGranularity() * sizeof(VALUE));

			VALUE *pElement = new(placement_new, GetBytes() + iOffset) VALUE(Value);
			}

		void Insert (const TArray<VALUE> &Src, int iIndex = -1)
			{
			int i;

			int iOffset;
			if (iIndex == -1) iIndex = GetCount();
			iOffset = iIndex * sizeof(VALUE);
			InsertBytes(iOffset, NULL, Src.GetCount() * sizeof(VALUE), GetGranularity() * sizeof(VALUE));

			for (i = 0; i < Src.GetCount(); i++)
				{
				VALUE *pElement = new(placement_new, GetBytes() + iOffset + i * sizeof(VALUE)) VALUE(Src[i]);
				}
			}

		VALUE *Insert (void)
			{
			int iOffset = GetCount() * sizeof(VALUE);
			InsertBytes(iOffset, NULL, sizeof(VALUE), GetGranularity() * sizeof(VALUE));

			return new(placement_new, GetBytes() + iOffset) VALUE;
			}

		VALUE *InsertAt (int iIndex)
			{
			int iOffset;
			if (iIndex == -1) iIndex = GetCount();
			iOffset = iIndex * sizeof(VALUE);
			InsertBytes(iOffset, NULL, sizeof(VALUE), GetGranularity() * sizeof(VALUE));

			return new(placement_new, GetBytes() + iOffset) VALUE;
			}

		void InsertEmpty (int iCount = 1, int iIndex = -1)
			{
			if (iCount <= 0)
				return;

			int iOffset;
			if (iIndex == -1) iIndex = GetCount();
			iOffset = iIndex * sizeof(VALUE);
			InsertBytes(iOffset, NULL, iCount * sizeof(VALUE), GetGranularity() * sizeof(VALUE));

			for (int i = 0; i < iCount; i++)
				{
				VALUE *pElement = new(placement_new, GetBytes() + iOffset + (i * sizeof(VALUE))) VALUE;
				}
			}

		void InsertSorted (const VALUE &Value, Kernel::ESortOptions Order = AscendingSort)
			{
			int iCount = GetCount();
			int iMin = 0;
			int iMax = iCount;
			int iTry = iMax / 2;

			while (true)
				{
				if (iMax <= iMin)
					{
					Insert(Value, iMin);
					break;
					}

				int iCompare = Order * KeyCompare(Value, GetAt(iTry));
				if (iCompare == 0)
					{
					Insert(Value, iTry);
					break;
					}
				else if (iCompare == -1)
					{
					iMin = iTry + 1;
					iTry = iMin + (iMax - iMin) / 2;
					}
				else if (iCompare == 1)
					{
					iMax = iTry;
					iTry = iMin + (iMax - iMin) / 2;
					}
				}
			}

		VALUE Pop (void)
			{
			int iCount = GetCount();
			if (iCount == 0)
				return VALUE();

			VALUE Temp = GetAt(iCount - 1);
			Delete(iCount - 1);
			return Temp;
			}

		void Push (const VALUE &Value)
			{
			Insert(Value);
			}

		void SetCount (int iNewCount)
			{
			int iCurCount = GetCount();
			if (iNewCount > iCurCount)
				InsertEmpty(iNewCount - iCurCount);
			else if (iNewCount < iCurCount)
				{
				VALUE *pElement = (VALUE *)GetBytes();
				for (int i = iNewCount; i < iCurCount; i++, pElement++)
					pElement->VALUE::~VALUE();

				DeleteBytes(iNewCount * sizeof(VALUE), (iCurCount - iNewCount) * sizeof(VALUE));
				}
			}

		void Shuffle (void)
			{
			if (GetCount() < 2)
				return;

			//	Fisher-Yates algorithm

			int i = GetCount() - 1;
			while (i > 0)
				{
				int x = mathRandom(0, i);

				VALUE Temp = GetAt(x);
				GetAt(x) = GetAt(i);
				GetAt(i) = Temp;

				i--;
				}
			}

		void Sort (Kernel::ESortOptions Order = AscendingSort)
			{
			std::function<int(const VALUE &, const VALUE &)> fnCompare = KeyCompare<VALUE>;
			Sort(fnCompare, Order);
			}

		void Sort (std::function<int(const VALUE &, const VALUE &)> fnCompare, Kernel::ESortOptions Order = AscendingSort)
			{
			if (GetCount() < 2)
				return;

			TArray<int> Result;
			Result.GrowToFit(GetCount());

			//	Binary sort the contents into an indexed array

			SortRange(fnCompare, Order, 0, GetCount() - 1, Result);

			//	Create a new sorted array

			TArray<VALUE> SortedArray;
			SortedArray.InsertEmpty(GetCount());
			for (int i = 0; i < GetCount(); i++)
				SortedArray[i] = GetAt(Result[i]);

			TakeHandoff(SortedArray);
			}

		void TakeHandoff (TArray<VALUE> &Obj)
			{
			DeleteAll();
			TakeHandoffBase(Obj);
			}

	private:
		void SortRange (std::function<int(const VALUE &, const VALUE &)> fnCompare, Kernel::ESortOptions Order, int iLeft, int iRight, TArray<int> &Result)
			{
			if (iLeft == iRight)
				Result.Insert(iLeft);
			else if (iLeft + 1 == iRight)
				{
				int iCompare = Order * fnCompare(GetAt(iLeft), GetAt(iRight));
				if (iCompare == 1)
					{
					Result.Insert(iLeft);
					Result.Insert(iRight);
					}
				else
					{
					Result.Insert(iRight);
					Result.Insert(iLeft);
					}
				}
			else
				{
				int iMid = iLeft + ((iRight - iLeft) / 2);

				TArray<int> Buffer1;
				TArray<int> Buffer2;

				SortRange(fnCompare, Order, iLeft, iMid, Buffer1);
				SortRange(fnCompare, Order, iMid+1, iRight, Buffer2);

				//	Merge

				int iPos1 = 0;
				int iPos2 = 0;
				bool bDone = false;
				while (!bDone)
					{
					if (iPos1 < Buffer1.GetCount() && iPos2 < Buffer2.GetCount())
						{
						int iCompare = Order * fnCompare(GetAt(Buffer1[iPos1]), GetAt(Buffer2[iPos2]));
						if (iCompare == 1)
							Result.Insert(Buffer1[iPos1++]);
						else if (iCompare == -1)
							Result.Insert(Buffer2[iPos2++]);
						else
							{
							Result.Insert(Buffer1[iPos1++]);
							Result.Insert(Buffer2[iPos2++]);
							}
						}
					else if (iPos1 < Buffer1.GetCount())
						Result.Insert(Buffer1[iPos1++]);
					else if (iPos2 < Buffer2.GetCount())
						Result.Insert(Buffer2[iPos2++]);
					else
						bDone = true;
					}
				}
			}
	};
#pragma warning(default:4291)

//	TProbabilityTable ----------------------------------------------------------

template <class VALUE> class TProbabilityTable
	{
	public:
		TProbabilityTable (void) :
				m_iTotalChance(0)
			{ }

		const VALUE &operator [] (int iIndex) const { return GetAt(iIndex); }
		VALUE &operator [] (int iIndex) { return GetAt(iIndex); }

		void Delete (int iIndex)
			{
			m_iTotalChance -= m_Table[iIndex].iChance;
			m_Table.Delete(iIndex);
			}

		void DeleteAll (void)
			{
			m_Table.DeleteAll();
			m_iTotalChance = 0;
			}

		const VALUE &GetAt (int iIndex) const { return m_Table[iIndex].Value; }
		VALUE &GetAt (int iIndex) { return m_Table[iIndex].Value; }

		int GetChance (int iIndex) const { return m_Table[iIndex].iChance; }

		int GetChanceByValue (const VALUE &ToFind) const
			{
			for (int i = 0; i < m_Table.GetCount(); i++)
				if (m_Table[i].Value == ToFind)
					return m_Table[i].iChance;

			return 0;
			}

		int GetCount (void) const { return m_Table.GetCount(); }

		double GetScaledChance (int iIndex, int iScale = 100) const
			{
			if (m_iTotalChance == 0)
				return 0.0;

			return (double)iScale * (double)GetChance(iIndex) / (double)m_iTotalChance;
			}

		double GetScaledChanceByValue (const VALUE &ToFind, int iScale = 100) const
			{
			if (m_iTotalChance == 0)
				return 0.0;

			return (double)iScale * (double)GetChanceByValue(ToFind) / (double)m_iTotalChance;
			}

		int GetTotalChance (void) const { return m_iTotalChance; }

		void GrowToFit (int iCount)
			{
			m_Table.GrowToFit(iCount);
			}

		void Insert (const VALUE &NewValue, int iChance)
			{
			ASSERT(iChance >= 0);

			SEntry *pEntry = m_Table.Insert();
			pEntry->iChance = iChance;
			pEntry->Value = NewValue;
			m_iTotalChance += iChance;
			}

		bool IsEmpty (void) const { return (m_iTotalChance == 0); }

		int RollPos (void) const
			{
			if (IsEmpty())
				return -1;

			int iPos = 0;
			int iRoll = mathRandom(0, m_iTotalChance - 1);

			//	Get the position

			while (iRoll >= m_Table[iPos].iChance)
				iRoll -= m_Table[iPos++].iChance;

			return iPos;
			}

		void SetChance (int iIndex, int iChance)
			{
			ASSERT(iChance >= 0);

			m_iTotalChance += iChance - m_Table[iIndex].iChance;
			m_Table[iIndex].iChance = iChance;
			}

	private:
		struct SEntry
			{
			int iChance;
			VALUE Value;
			};

		TArray<SEntry> m_Table;
		int m_iTotalChance;
	};

//	Simple array classes

//class CStringArray : public TArray<CString> { };
//class CIntArray : public TArray<int> { };

#ifdef DEBUG_MEMORY_LEAKS
#define new DEBUG_NEW
#endif

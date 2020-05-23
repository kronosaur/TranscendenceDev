//	TMap.h
//
//	TMap class
//	Copyright (c) 2010 by George Moromisato. All Rights Reserved.

#ifndef INCL_TMAP
#define INCL_TMAP

class CMapIterator
	{
	public:
		CMapIterator (void) : m_iTableEntry(0), m_pPos(NULL) { }

	private:
		int m_iTableEntry;
		void *m_pPos;

	friend class CMapBase;
	};

class CMapBase
	{
	protected:
		struct EntryBase
			{
			EntryBase *pNext;
			};

		CMapBase (int iTableSize);
		~CMapBase (void);

		EntryBase *DeleteEntry (void *pVoidKey, void *pKey, int iKeyLen);
		EntryBase *FindEntry (void *pVoidKey, void *pKey, int iKeyLen, int *retiSlot = NULL, EntryBase **retpPrevEntry = NULL) const;
		int Hash (void *pKey, int iKeyLen) const;
		void InsertEntry (void *pKey, int iKeyLen, EntryBase *pEntry);
		void InsertEntry (int iSlot, EntryBase *pEntry);

		void Reset (CMapIterator &Iterator) const;
		EntryBase *GetNext (CMapIterator &Iterator) const;
		bool HasMore (CMapIterator &Iterator) const;

		virtual bool KeyEquals (void *pVoidKey, EntryBase *pEntry) const = 0;

		EntryBase **m_pTable;						//	An array of entries
		int m_iTableSize;							//	Size of table
		int m_iCount;
	};

//	Comparison functions

bool MapKeyEquals (const CString &sKey1, const CString &sKey2);
template<class KEY> bool MapKeyEquals (const KEY &Key1, const KEY &Key2) { return Key1 == Key2; }

void *MapKeyHashData (const CString &Key);
template<class KEY> void *MapKeyHashData (const KEY &Key) { return (void *)&Key; }
int MapKeyHashDataSize (const CString &Key);
template<class KEY> int MapKeyHashDataSize (const KEY &Key) { return sizeof(KEY); }

//	Template

template <class KEY, class VALUE> class TMap : public CMapBase
	{
	public:
		TMap (int iTableSize = 999) : CMapBase(iTableSize) { }
		TMap (const TMap<KEY, VALUE> &Src) : CMapBase(Src.m_iTableSize)
			{
			Copy(Src);
			}

		~TMap (void)
			{
			DeleteAll();
			}

		TMap<KEY, VALUE> &operator= (const TMap<KEY, VALUE> &Obj)
			{
			DeleteAll();

			delete [] m_pTable;
			m_iTableSize = Obj.m_iTableSize;
			m_pTable = new EntryBase * [Obj.m_iTableSize];

			Copy(Obj);

			return *this;
			}

		void Delete (const KEY &Key)
			{
			Entry *pDeletedEntry = (Entry *)DeleteEntry((void *)&Key, MapKeyHashData(Key), MapKeyHashDataSize(Key));
			if (pDeletedEntry)
				delete pDeletedEntry;
			}

		void DeleteAll (void)
			{
			for (int i = 0; i < m_iTableSize; i++)
				{
				Entry *pEntry = (Entry *)m_pTable[i];
				while (pEntry)
					{
					Entry *pNextEntry = pEntry->m_pNext;
					delete pEntry;
					pEntry = pNextEntry;
					}
				m_pTable[i] = NULL;
				}

			m_iCount = 0;
			}

		VALUE * const Find (const KEY &Key) const
			{
			Entry *pEntryFound = (Entry *)FindEntry((void *)&Key, MapKeyHashData(Key), MapKeyHashDataSize(Key));
			if (pEntryFound)
				return &pEntryFound->m_Value;
			else
				return NULL;
			}

		int GetCount (void) { return m_iCount; }

		const KEY &GetKey (VALUE *pValue) const
			{
			//	LATER: Is there some keyword for field offset?
			Entry *pEntry = (Entry *)(((char *)pValue) - (sizeof(KEY) + sizeof(Entry *)));
			return pEntry->m_Key;
			}

		const KEY &GetNext (CMapIterator &Iterator, VALUE **retpValue) const
			{
			Entry *pEntryFound = (Entry *)CMapBase::GetNext(Iterator);
			ASSERT(pEntryFound);
			if (retpValue)
				*retpValue = &pEntryFound->m_Value;
			return pEntryFound->m_Key;
			}

		VALUE * const GetNext (CMapIterator &Iterator) const
			{
			Entry *pEntryFound = (Entry *)CMapBase::GetNext(Iterator);
			ASSERT(pEntryFound);
			return &pEntryFound->m_Value;
			}

		bool HasMore (CMapIterator &Iterator) const
			{
			return CMapBase::HasMore(Iterator);
			}

		VALUE * const Insert (const KEY &Key)
			{
			Entry *pNewEntry = new Entry(Key);
			InsertEntry(MapKeyHashData(Key), MapKeyHashDataSize(Key), (EntryBase *)pNewEntry);
			return &pNewEntry->m_Value;
			}

		void Insert (const KEY &Key, const VALUE &Value)
			{
			Entry *pNewEntry = new Entry(Key, Value);
			InsertEntry(MapKeyHashData(Key), MapKeyHashDataSize(Key), (EntryBase *)pNewEntry);
			}

		void Reset (CMapIterator &Iterator) const
			{
			CMapBase::Reset(Iterator);
			}

		VALUE * const Set (const KEY &Key)
			{
			int iSlot;
			Entry *pEntryFound = (Entry *)FindEntry((void *)&Key, MapKeyHashData(Key), MapKeyHashDataSize(Key), &iSlot);
			if (pEntryFound)
				return &pEntryFound->m_Value;

			Entry *pNewEntry = new Entry(Key);
			InsertEntry(iSlot, (EntryBase *)pNewEntry);
			return &pNewEntry->m_Value;
			}

		void Set (const KEY &Key, const VALUE &Value)
			{
			Entry *pEntryFound = (Entry *)FindEntry((void *)&Key, MapKeyHashData(Key), MapKeyHashDataSize(Key));
			if (pEntryFound)
				pEntryFound->m_Value = Value;
			else
				Insert(Key, Value);
			}

	protected:
		virtual bool KeyEquals (void *pVoidKey, EntryBase *pEntry) const
			{
			return MapKeyEquals(*(KEY *)pVoidKey, ((Entry *)pEntry)->m_Key);
			}

	private:
		class Entry
			{
			public:
				Entry (const KEY &Key) :
						m_pNext(NULL),
						m_Key(Key)
					{ }

				Entry (const KEY &Key, const VALUE &Value) :
						m_pNext(NULL),
						m_Key(Key),
						m_Value(Value)
					{ }

				Entry *m_pNext;
				KEY m_Key;
				VALUE m_Value;
			};

		void Copy (const TMap<KEY, VALUE> &Src)
			{
			m_iCount = Src.m_iCount;

			for (int i = 0; i < m_iTableSize; i++)
				{
				Entry *pStartDest = NULL;
				Entry *pNextDest = NULL;
				Entry *pNext = (Entry *)Src.m_pTable[i];
				while (pNext)
					{
					Entry *pNew = new Entry(pNext->m_Key, pNext->m_Value);
					if (pNextDest)
						pNextDest->m_pNext = pNew;
					else
						pStartDest = pNew;

					pNextDest = pNew;
					pNext = pNext->m_pNext;
					}

				m_pTable[i] = (EntryBase *)pStartDest;
				}
			}
	};

const DWORD NULL_ATOM = 0xffffffff;

template <class KEY, class VALUE> class TSortMap
	{
	public:
		TSortMap (ESortOptions iOrder = AscendingSort) : m_iOrder(iOrder) { }

		const VALUE &operator [] (int iIndex) const { return GetValue(iIndex); }
		VALUE &operator [] (int iIndex) { return GetValue(iIndex); }

		void Delete (int iIndex)
			{
			ASSERT(iIndex >= 0 && iIndex < m_Index.GetCount());
			int iPos = m_Index[iIndex];
			m_Index.Delete(iIndex);

			//	Add the array slot to free list (we can't delete it
			//	because it would move the indices of other entries)

			m_Array[iPos].theValue = VALUE();
			m_Free.Insert(iPos);
			}

		void DeleteAll (void)
			{
			m_Index.DeleteAll();
			m_Array.DeleteAll();
			m_Free.DeleteAll();
			}

		void DeleteAt (const KEY &key)
			{
			int iPos;
			if (FindPos(key, &iPos))
				Delete(iPos);
			}

		bool Find (const KEY &key, VALUE *retpValue = NULL) const
			{
			int iPos;
			if (!FindPos(key, &iPos))
				return false;

			if (retpValue)
				*retpValue = GetValue(iPos);

			return true;
			}

		bool FindByValue (const VALUE &Value, int *retiPos = NULL) const
			{
			for (int i = 0; i < GetCount(); i++)
				if (GetValue(i) == Value)
					{
					if (retiPos)
						*retiPos = i;
					return true;
					}

			return false;
			}

		bool FindPos (const KEY &key, int *retiPos = NULL) const
			{
			int iCount = m_Index.GetCount();
			int iMin = 0;
			int iMax = iCount;
			int iTry = iMax / 2;

			while (true)
				{
				if (iMax <= iMin)
					{
					if (retiPos)
						*retiPos = iMin;
					return false;
					}

				int iCompare = m_iOrder * KeyCompare(key, GetKey(iTry));
				if (iCompare == 0)
					{
					if (retiPos)
						*retiPos = iTry;
					return true;
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

			return false;
			}

		const VALUE *GetAt (const KEY &key) const
			{
			int iPos;
			if (!FindPos(key, &iPos))
				return NULL;

			return &m_Array[m_Index[iPos]].theValue;
			}

		VALUE *GetAt (const KEY &key)
			{
			int iPos;
			if (!FindPos(key, &iPos))
				return NULL;

			return &m_Array[m_Index[iPos]].theValue;
			}

		int GetCount (void) const
			{
			return m_Index.GetCount();
			}

		const KEY &GetKey (int iIndex) const
			{
			return m_Array[m_Index[iIndex]].theKey;
			}

		const VALUE &GetValue (int iIndex) const
			{
			return m_Array[m_Index[iIndex]].theValue;
			}

		VALUE &GetValue (int iIndex)
			{
			return m_Array[m_Index[iIndex]].theValue;
			}

		void GrowToFit (int iCount)
			{
			int i;

			int iPos = m_Array.GetCount();
			m_Array.InsertEmpty(iCount);
			m_Free.GrowToFit(iCount);
			for (i = 0; i < iCount; i++)
				m_Free.Insert(iPos + i);

			m_Index.GrowToFit(iCount);
			}

		const VALUE &IncAt (const KEY &key, const VALUE &incValue)
			{
			int iIndex;

			if (FindPos(key, &iIndex))
				{
				m_Array[m_Index[iIndex]].theValue += incValue;
				return m_Array[m_Index[iIndex]].theValue;
				}
			else
				{
				int iPos;
				SEntry *pEntry = InsertEntry(&iPos);

				//	Do it

				m_Index.Insert(iPos, iIndex);
				pEntry->theKey = key;
				pEntry->theValue = incValue;

				return pEntry->theValue;
				}
			}

		VALUE *Insert (const KEY &newKey)
			{
			return atom_Insert(newKey);
			}

		void Insert (const KEY &newKey, const VALUE &newValue)
			{
			VALUE *pNewValue = atom_Insert(newKey);
			*pNewValue = newValue;
			}

		void InsertSorted (const KEY &newKey, const VALUE &newValue, int iPos = -1)
			{
			//	Find where to insert it in the array

			int iInsertPos;
			SEntry *pEntry = InsertEntry(&iInsertPos);

			//	Insert in the index

			m_Index.Insert(iInsertPos, iPos);
			pEntry->theKey = newKey;
			pEntry->theValue = newValue;
			}

		void Merge (const TSortMap<KEY, VALUE> &Src, TArray<VALUE> *retReplaced = NULL)
			{
			//	For now this only works if we have the same sort order

			if (m_iOrder != Src.m_iOrder)
				{
				ASSERT(false);
				return;
				}

			//	Set up

			int iSrcPos = 0;
			int iDestPos = 0;

			//	Merge

			while (iSrcPos < Src.m_Index.GetCount())
				{
				//	If we're at the end of the destination then just insert

				if (iDestPos == m_Index.GetCount())
					{
					InsertSorted(Src.GetKey(iSrcPos), Src.GetValue(iSrcPos));

					//	Advance

					iDestPos++;
					iSrcPos++;
					}

				//	Otherwise, see if we need to insert or replace

				else
					{
					int iCompare = m_iOrder * KeyCompare(Src.GetKey(iSrcPos), GetKey(iDestPos));

					//	If the same key then we replace

					if (iCompare == 0)
						{
						//	Return replaced values

						if (retReplaced)
							retReplaced->Insert(GetValue(iDestPos));

						GetValue(iDestPos) = Src.GetValue(iSrcPos);

						//	Advance

						iDestPos++;
						iSrcPos++;
						}

					//	If the source is less than dest then we insert at this
					//	position.

					else if (iCompare == 1)
						{
						InsertSorted(Src.GetKey(iSrcPos), Src.GetValue(iSrcPos), iDestPos);

						//	Advance

						iDestPos++;
						iSrcPos++;
						}

					//	Otherwise, go to the next destination slot

					else
						iDestPos++;
					}
				}
			}

		VALUE *SetAt (const KEY &key, bool *retbInserted = NULL)
			{
			int iIndex;

			if (FindPos(key, &iIndex))
				{
				if (retbInserted)
					*retbInserted = false;

				return &m_Array[m_Index[iIndex]].theValue;
				}
			else
				{
				int iPos;
				SEntry *pEntry = InsertEntry(&iPos);

				//	Do it

				m_Index.Insert(iPos, iIndex);
				pEntry->theKey = key;

				if (retbInserted)
					*retbInserted = true;

				return &pEntry->theValue;
				}
			}

		void SetAt (const KEY &key, const VALUE &value, bool *retbInserted = NULL)
			{
			VALUE *pValue = SetAt(key, retbInserted);
			*pValue = value;
			}

		void SetGranularity (int iGranularity)
			{
			m_Index.SetGranularity(iGranularity);
			m_Array.SetGranularity(iGranularity);
			}

		void TakeHandoff (TSortMap<KEY, VALUE> &Src)
			{
			m_iOrder = Src.m_iOrder;
			m_Array.TakeHandoff(Src.m_Array);
			m_Free.TakeHandoff(Src.m_Free);
			m_Index.TakeHandoff(Src.m_Index);
			}

		//	Atom helper functions

		void atom_Delete (DWORD dwAtom)
			{
			int i;

			//	Look for the array position in the index and delete it.

			for (i = 0; i < m_Index.GetCount(); i++)
				if (m_Index[i] == (int)dwAtom)
					{
					m_Index.Delete(i);
					break;
					}

			//	Add the array slot to the free list

			m_Array[dwAtom].theValue = VALUE();
			m_Free.Insert(dwAtom);
			}

		DWORD atom_Find (const KEY &key)
			{
			int iPos;
			if (!FindPos(key, &iPos))
				return NULL_ATOM;

			return (DWORD)m_Index[iPos];
			}

		DWORD atom_GetFromPos (int iPos) const
			{
			return m_Index[iPos];
			}

		const KEY &atom_GetKey (DWORD dwAtom) const
			{
			return m_Array[dwAtom].theKey;
			}

		VALUE &atom_GetValue (DWORD dwAtom) const
			{
			return m_Array[dwAtom].theValue;
			}

		VALUE *atom_Insert (const KEY &newKey, DWORD *retdwAtom = NULL)
			{
			//	Find index insertion position

			int iIndex;
			FindPos(newKey, &iIndex);

			//	Find where to insert it in the array

			int iPos;
			SEntry *pEntry = InsertEntry(&iPos);

			//	Do it

			if (retdwAtom)
				*retdwAtom = (DWORD)iPos;

			m_Index.Insert(iPos, iIndex);
			pEntry->theKey = newKey;
			return &pEntry->theValue;
			}

	private:
		struct SEntry
			{
			KEY theKey;
			VALUE theValue;
			};

		SEntry *InsertEntry (int *retiPos)
			{
			SEntry *pEntry;
			if (m_Free.GetCount() == 0)
				{
				*retiPos = m_Array.GetCount();
				pEntry = m_Array.Insert();
				}
			else
				{
				*retiPos = m_Free[m_Free.GetCount() - 1];
				pEntry = &m_Array[*retiPos];
				m_Free.Delete(m_Free.GetCount() - 1);
				}
			return pEntry;
			}

		ESortOptions m_iOrder;
		TArray<int> m_Index;
		TArray<SEntry> m_Array;
		TArray<int> m_Free;
	};

template <class ENTRY, size_t N> class TStaticStringTable
	{
	public:
		ENTRY &operator [] (int iIndex) { ASSERT(iIndex >= 0 && iIndex < N); return m_Array[iIndex]; }

		const ENTRY &operator [] (int iIndex) const { ASSERT(iIndex >= 0 && iIndex < N); return m_Array[iIndex]; }
		
		bool FindPos (const CString &sKey, int *retiPos = NULL) const
			{
			char *pKey = sKey.GetASCIIZPointer();
			char *pKeyEnd = pKey + sKey.GetLength();
			return FindPos(pKey, pKeyEnd, retiPos);
			}

		bool FindPos (const char *pKey, const char *pKeyEnd, int *retiPos = NULL) const
			{
			int iCount = N;
			int iMin = 0;
			int iMax = iCount;
			int iTry = iMax / 2;

			while (true)
				{
				if (iMax <= iMin)
					{
					if (retiPos)
						*retiPos = iMin;
					return false;
					}

				int iCompare = Compare(pKey, pKeyEnd, GetKey(iTry));
				if (iCompare == 0)
					{
					if (retiPos)
						*retiPos = iTry;
					return true;
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

			return false;
			}

		bool FindPosCase (const CString &sKey, int *retiPos = NULL) const
			{
			char *pKey = sKey.GetASCIIZPointer();
			char *pKeyEnd = pKey + sKey.GetLength();
			return FindPosCase(pKey, pKeyEnd, retiPos);
			}

		bool FindPosCase (const char *pKey, const char *pKeyEnd, int *retiPos = NULL) const
			{
			int iCount = N;
			int iMin = 0;
			int iMax = iCount;
			int iTry = iMax / 2;

			while (true)
				{
				if (iMax <= iMin)
					{
					if (retiPos)
						*retiPos = iMin;
					return false;
					}

				int iCompare = CompareCase(pKey, pKeyEnd, GetKey(iTry));
				if (iCompare == 0)
					{
					if (retiPos)
						*retiPos = iTry;
					return true;
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

			return false;
			}

		const ENTRY *GetAt (const CString &sKey) const
			{
			int iPos;
			if (!FindPos(sKey, &iPos))
				return NULL;

			return &m_Array[iPos];
			}

		const ENTRY *GetAt (const char *pKey, const char *pKeyEnd) const
			{
			int iPos;
			if (!FindPos(pKey, pKeyEnd, &iPos))
				return NULL;

			return &m_Array[iPos];
			}

		const ENTRY *GetAtCase (const CString &sKey) const
			{
			int iPos;
			if (!FindPosCase(sKey, &iPos))
				return NULL;

			return &m_Array[iPos];
			}

		const ENTRY *GetAtCase (const char *pKey, const char *pKeyEnd) const
			{
			int iPos;
			if (!FindPosCase(pKey, pKeyEnd, &iPos))
				return NULL;

			return &m_Array[iPos];
			}

		int GetCount (void) const { return N; }

		const char *GetKey (int iIndex) const { return m_Array[iIndex].pszKey; }

		ENTRY m_Array[N];

	private:
		int Compare (const char *pS1, const char *pS1End, const char *pS2) const
			{
			while (pS1 < pS1End && *pS2 != '\0')
				{
				char chChar1 = strLowerCaseAbsolute(*pS1++);
				char chChar2 = strLowerCaseAbsolute(*pS2++);

				if (chChar1 > chChar2)
					return -1;
				else if (chChar1 < chChar2)
					return 1;
				}

			//	If the strings match up to a point, check to see if we hit
			//	the end of both.

			if (*pS2 == '\0' && pS1 == pS1End)
				return 0;
			else if (*pS2 == '\0')
				return -1;
			else
				return 1;
			}

		int CompareCase (const char *pS1, const char *pS1End, const char *pS2) const
			{
			while (pS1 < pS1End && *pS2 != '\0')
				{
				char chChar1 = *pS1++;
				char chChar2 = *pS2++;

				if (chChar1 > chChar2)
					return -1;
				else if (chChar1 < chChar2)
					return 1;
				}

			//	If the strings match up to a point, check to see if we hit
			//	the end of both.

			if (*pS2 == '\0' && pS1 == pS1End)
				return 0;
			else if (*pS2 == '\0')
				return -1;
			else
				return 1;
			}
	};

template <class VALUE> struct TStaticStringEntry
	{
	const char *pszKey;
	VALUE Value;
	};

struct SSimpleStringEntry
	{
	const char *pszKey;
	};

template <class VALUE> class TProbabilityMap
	{
	public:
		VALUE &operator [] (int iIndex) const { return GetAt(iIndex); }

		void Delete (int iIndex)
			{
			m_iTotalChance -= m_Table[iIndex];
			m_Table.Delete(iIndex);
			}

		void DeleteAll (void)
			{
			m_Table.DeleteAll();
			m_iTotalChance = 0;
			}

		int GetChance (int iIndex) const { return m_Table[iIndex]; }

		int GetChanceByValue (const VALUE &ToFind) const
			{
			int const *pChance = m_Table.GetAt(ToFind);
			if (pChance == NULL)
				return 0;

			return *pChance;
			}

		int GetCount (void) const { return m_Table.GetCount(); }

		const VALUE &GetKey (int iIndex) const { return m_Table.GetKey(iIndex); }

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

		void Insert (const VALUE &NewValue, int iChance)
			{
			ASSERT(iChance >= 0);

			bool bInserted;
			int *pChance = m_Table.SetAt(NewValue, &bInserted);
			if (bInserted)
				*pChance = iChance;
			else
				*pChance += iChance;

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

			while (m_Table[iPos] <= iRoll)
				iRoll -= m_Table[iPos++];

			return iPos;
			}

	private:
		TSortMap<VALUE, int> m_Table;
		int m_iTotalChance = 0;
	};

//	Simple map classes

template <class VALUE> class TStringMap : public TMap<CString, VALUE> { };
template <class VALUE> class TIntMap : public TMap<int, VALUE> { };

#endif

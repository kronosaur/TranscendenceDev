//	TLinkedList.h
//
//	Linked list classes
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CListIterator
	{
	public:
		CListIterator (void) :
				m_pEntry(NULL)
			{ }

	private:
		void *m_pEntry;

	template <class VALUE> friend class TLinkedList;
	};

template <class VALUE> class TLinkedList
	{
	public:
		TLinkedList (void) : 
				m_pFirst(NULL),
				m_pLast(NULL)
			{ }

		~TLinkedList (void)
			{
			DeleteAll();
			}

		void DeleteAll (void)
			{
			SEntry *pNext = m_pFirst;
			while (pNext)
				{
				SEntry *pToDelete = pNext;
				pNext = pNext->pNext;
				delete pToDelete;
				}

			m_pFirst = NULL;
			m_pLast = NULL;
			}

		void Delete (CListIterator &Pos)
			{
			if (Pos.m_pEntry)
				{
				SEntry *pEntry = (SEntry *)Pos.m_pEntry;
				Pos.m_pEntry = pEntry->pNext;
				Delete(pEntry);
				}
			}

		const VALUE &GetNext (CListIterator &Pos) const
			{
			return ((SEntry *)Pos.m_pEntry)->Value;
			}

		VALUE &GetNext (CListIterator &Pos)
			{
			return ((SEntry *)Pos.m_pEntry)->Value;
			}

		void Insert (const VALUE &Value, CListIterator &Pos = CListIterator())
			{
			SEntry *pNewEntry = new SEntry;
			pNewEntry->Value = Value;
			Insert(pNewEntry, (SEntry *)Pos.m_pEntry);
			}

		VALUE *Insert (CListIterator &Pos = CListIterator())
			{
			SEntry *pNewEntry = new SEntry;
			Insert(pNewEntry, (SEntry *)Pos.m_pEntry);
			return &pNewEntry->Value;
			}

		bool HasMore (CListIterator &Pos) const
			{
			return (Pos.m_pEntry != NULL);
			}

		void Reset (CListIterator &Pos) const
			{
			Pos.m_pEntry = m_pFirst;
			}

	private:
		struct SEntry
			{
			VALUE Value;
			SEntry *pNext;
			SEntry *pPrev;
			};

		void Delete (SEntry *pEntry)
			{
			if (pEntry->pPrev == NULL)
				m_pFirst = pEntry->pNext;
			else
				pEntry->pPrev->pNext = pEntry->pNext;

			if (pEntry->pNext == NULL)
				m_pLast = pEntry->pPrev;
			else
				pEntry->pNext->pPrev = pEntry->pPrev;

			delete pEntry;
			}

		void Insert (SEntry *pNewEntry, SEntry *pPos)
			{
			if (pPos)
				{
				if (pPos->pPrev == NULL)
					m_pFirst = pNewEntry;
				else
					pPos->pPrev->pNext = pNewEntry;

				pNewEntry->pPrev = pPos->pPrev;
				pNewEntry->pNext = pPos;

				pPos->pPrev = pNewEntry;
				}
			else
				{
				if (m_pLast == NULL)
					{
					m_pFirst = pNewEntry;
					m_pLast = pNewEntry;
					pNewEntry->pPrev = NULL;
					pNewEntry->pNext = NULL;
					}
				else
					{
					m_pLast->pNext = pNewEntry;
					pNewEntry->pPrev = m_pLast;
					pNewEntry->pNext = NULL;
					m_pLast = pNewEntry;
					}
				}
			}

		SEntry *m_pFirst;
		SEntry *m_pLast;
	};

//	Old classes

#ifdef KERNEL_TLISTNODE

template <class TYPE> class TListNode
	{
	public:
		TListNode (void) : m_pNext(NULL) { }
		virtual ~TListNode (void) { }

		int GetCount (void)
			{
			int iCount = 0;
			TListNode<TYPE> *pNext = m_pNext;
			while (pNext)
				{
				iCount++;
				pNext = pNext->m_pNext;
				}
			return iCount;
			}

		TYPE *GetNext (void) { return (TYPE *)m_pNext; }

		void Insert (TListNode<TYPE> *pNewNode)
			{
			pNewNode->m_pNext = m_pNext;
			m_pNext = pNewNode;
			}

		void Remove (TListNode<TYPE> *pNodeToRemove)
			{
			TListNode<TYPE> *pNext = m_pNext;
			while (pNext)
				{
				if (pNext == pNodeToRemove)
					{
					RemoveNext();
					break;
					}
				pNext = pNext->m_pNext;
				}
			}

	private:
		void RemoveNext (void)
			{
			TListNode<TYPE> *pDelete = m_pNext;
			m_pNext = m_pNext->m_pNext;
			delete pDelete;
			}

		TListNode<TYPE> *m_pNext;
	};

#endif

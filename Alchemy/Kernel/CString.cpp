//	CString.cpp
//
//	Implementation of standard string class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include <float.h>

#define STORE_SIZE_INIT						256
#define STORE_SIZE_INCREMENT				256
#define STORE_ALLOC_MAX						(64 * 1024 * 1024)

CString::PSTORESTRUCT CString::g_pStore;
int CString::g_iStoreSize;
CString::PSTORESTRUCT CString::g_pFreeStore;
CRITICAL_SECTION g_csStore;
const CString Kernel::NULL_STR;

#ifdef DEBUG_STRING_LEAKS
int g_iStoreCount = 0;
#endif

bool Kernel::g_bLowerCaseAbsoluteTableInit = false;
char Kernel::g_LowerCaseAbsoluteTable[256];

//	List of English prepositions. The short list only includes prepositions that
//	might be found in noun phrases.
//
//	For the full list see: http://en.wikipedia.org/wiki/List_of_English_prepositions

static const char *TITLE_CAP_EXCEPTIONS_SHORT[] =
	{
	"a",
	"an",
	"and",
	"at",
	"by",
	"for",
	"in",
	"near",
	"not",
	"of",
	"on",
	"or",
	"the",
	"to",
	"under",
	"upon",
	};

static int TITLE_CAP_EXCEPTIONS_SHORT_COUNT = sizeof(TITLE_CAP_EXCEPTIONS_SHORT) / sizeof(TITLE_CAP_EXCEPTIONS_SHORT[0]);

CString::CString (void) :
		m_pStore(NULL)

//	CString constructor

	{
	}

CString::CString (const char *pString) :
		m_pStore(NULL)

//	CString constructor

	{
	if (pString)
		Transcribe(pString, -1);
	}

CString::CString (const char *pString, int iLength) :
		m_pStore(NULL)

//	CString constructor

	{
	if (pString)
		Transcribe(pString, iLength);
	}

CString::CString (CharacterSets iCharSet, const char *pString) :
		m_pStore(NULL)

//	CString constructor

	{
	switch (iCharSet)
		{
		case csUTF8:
			{
			int iUTF8Len = strlen(pString);

			//	Optimistically assume that we can fit each character into a Unicode
			//	character.

			int iUnicodeLen = iUTF8Len;
			WCHAR *szUnicode = new WCHAR [iUnicodeLen + 1];
			int iResult = ::MultiByteToWideChar(CP_UTF8, 0, pString, iUTF8Len, szUnicode, iUnicodeLen);

			//	Deal with failure

			if (iResult == 0)
				{
				if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
					{
					delete [] szUnicode;

					//	Figure out how big the buffer should be and allocate appropriately
					//	And redo the conversion.

					iUnicodeLen = ::MultiByteToWideChar(CP_UTF8, 0, pString, iUTF8Len, NULL, 0);
					if (iUnicodeLen == 0)
						return;

					szUnicode = new WCHAR [iUnicodeLen + 1];
					iResult = ::MultiByteToWideChar(CP_UTF8, 0, pString, iUTF8Len, szUnicode, iUnicodeLen);
					}
				else
					{
					delete [] szUnicode;
					return;
					}
				}

			iUnicodeLen = iResult;

			//	Now convert back to system code page

			Size(iUnicodeLen+1);
			iResult = ::WideCharToMultiByte(CP_ACP, 0, szUnicode, iUnicodeLen, m_pStore->pString, iUnicodeLen, NULL, NULL);

			//	Deal with failure

			if (iResult == 0)
				{
				if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
					{
					//	Figure out how big the buffer should be and allocate appropriately
					//	And redo the conversion.

					int iSystemLen = ::WideCharToMultiByte(CP_ACP, 0, szUnicode, iUnicodeLen, NULL, 0, NULL, NULL);
					if (iSystemLen == 0)
						{
						delete [] szUnicode;
						Truncate(0);
						return;
						}

					Size(iSystemLen+1);
					iResult = ::WideCharToMultiByte(CP_ACP, 0, szUnicode, iUnicodeLen, m_pStore->pString, iSystemLen, NULL, NULL);
					}
				else
					{
					delete [] szUnicode;
					Truncate(0);
					return;
					}
				}

			delete [] szUnicode;
			m_pStore->iLength = iResult;
			m_pStore->pString[m_pStore->iLength] = '\0';
			break;
			}

		default:
			{
			if (pString)
				Transcribe(pString, -1);
			}
		}
	}

CString::CString (const char *pString, int iLength, BOOL bExternal) :
		m_pStore(NULL)

//	CString constructor

	{
	if (pString)
		{
		if (bExternal)
			{
			if (iLength == -1)
				iLength = lstrlen(pString);

			//	Allocate a storage block (if necessary)

			if (iLength > 0)
				{
				m_pStore = AllocStore(0, FALSE);
				if (m_pStore)
					{
					//	A negative value means that this is an external
					//	read-only storage

					m_pStore->iAllocSize = -iLength;
					m_pStore->iLength = iLength;
					m_pStore->pString = const_cast<char *>(pString);
					}
				}
			}
		else
			Transcribe(pString, iLength);
		}
	}

CString::CString (const SConstString &String) :
		m_pStore(NULL)

//	CString constructor

	{
	if (String.pszString && String.iLen > 0)
		{
		m_pStore = AllocStore(0, FALSE);
		if (m_pStore)
			{
			//	A negative value means that this is an external
			//	read-only storage

			m_pStore->iAllocSize = -String.iLen;
			m_pStore->iLength = String.iLen;
			m_pStore->pString = const_cast<char *>(String.pszString);
			}
		}
	}

CString::~CString (void)

//	CString destructor

	{
	DecRefCount();
	}

CString::CString (void *pStore, bool bDummy) : m_pStore((PSTORESTRUCT)pStore)

//	CString private constructor

	{
	if (m_pStore)
		m_pStore->iRefCount++;
	}

CString::CString (const CString &pString) :
		m_pStore(NULL)

//	CString copy constructor

	{
	m_pStore = pString.m_pStore;

	//	Up the ref count

	if (m_pStore)
		m_pStore->iRefCount++;
	}

CString &CString::operator= (const CString &pString)

//	operator =
//
//	Overrides the assignment operator

	{
	//	First bump up the new string's refcount, in case it happens to be the
	//	exact same as ours.

	if (pString.m_pStore)
		pString.m_pStore->iRefCount++;

	//	Now decrement our own.

	DecRefCount();

	//	Take it.

	m_pStore = pString.m_pStore;

	return *this;
	}

bool CString::operator== (const CString &sValue) const

//	operator ==
//
//	NOTE: Case-insensitive compare

	{
	return strEquals(*this, sValue);
	}

bool CString::operator!= (const CString &sValue) const

//	operator !=
//
//	NOTE: Case-insensitive compare

	{
	return !strEquals(*this, sValue);
	}

void CString::AddToFreeList (PSTORESTRUCT pStore, int iSize)

//	AddToFreeList
//
//	Adds the array of storage locations to the free list. Note that
//	this routine assumes that the critical section has been locked

	{
	int i;

	for (i = 0; i < iSize; i++)
		{
		pStore->iRefCount = 0;
		pStore->pString = (char *)g_pFreeStore;
		g_pFreeStore = pStore;
		pStore++;
		}
	}

CString::PSTORESTRUCT CString::AllocStore (int iSize, BOOL bAllocString)

//	AllocStore
//
//	Allocates a new string store of at least the given size

	{
	PSTORESTRUCT pStore;

	//	Critical section

	EnterCriticalSection(&g_csStore);

	//	If we haven't yet created the store array, do it now

	if (g_pStore == NULL)
		{
		//	Reserve a megabyte of virtual memory

		g_pStore = (PSTORESTRUCT)VirtualAlloc(NULL, STORE_ALLOC_MAX, MEM_RESERVE, PAGE_NOACCESS);
		if (g_pStore == NULL)
			{
			LeaveCriticalSection(&g_csStore);
			ASSERT(false);
			return NULL;
			}

		//	Commit a little bit of it

		if (VirtualAlloc(g_pStore,
				STORE_SIZE_INIT * sizeof(STORESTRUCT),
				MEM_COMMIT,
				PAGE_READWRITE) == NULL)
			{
			LeaveCriticalSection(&g_csStore);
			ASSERT(false);
			return NULL;
			}

		//	Initialize the free list

		AddToFreeList(g_pStore, STORE_SIZE_INIT);

		g_iStoreSize = STORE_SIZE_INIT;
		}

	//	If there're no more free entries, re-alloc the store array

	if (g_pFreeStore == NULL)
		{
		if ((g_iStoreSize + STORE_SIZE_INCREMENT) * sizeof(STORESTRUCT) > STORE_ALLOC_MAX)
			{
			LeaveCriticalSection(&g_csStore);
			ASSERT(false);
			return NULL;
			}

		if (VirtualAlloc(g_pStore + g_iStoreSize,
				STORE_SIZE_INCREMENT * sizeof(STORESTRUCT),
				MEM_COMMIT,
				PAGE_READWRITE) == NULL)
			{
			LeaveCriticalSection(&g_csStore);
			ASSERT(false);
			return NULL;
			}

		//	Add the new storage blocks to the free list

		AddToFreeList(g_pStore + g_iStoreSize, STORE_SIZE_INCREMENT);
		if (!g_pFreeStore)
			throw CException(ERR_FAIL);

		g_iStoreSize += STORE_SIZE_INCREMENT;
		}

	//	Pick a block off the free list

	pStore = g_pFreeStore;
	g_pFreeStore = (PSTORESTRUCT)pStore->pString;

#ifdef DEBUG_STRING_LEAKS
	g_iStoreCount++;
#endif

	LeaveCriticalSection(&g_csStore);

	//	Initialize it

	if (bAllocString)
		{
		pStore->iRefCount = 1;
		pStore->iAllocSize = iSize;
		pStore->iLength = 0;
		pStore->pString = (char *)HeapAlloc(GetProcessHeap(), 0, iSize);
		}
	else
		{
		pStore->iRefCount = 1;
		pStore->iAllocSize = 0;
		pStore->iLength = 0;
		pStore->pString = NULL;
		}

#ifdef DEBUG_STRING_LEAKS
	pStore->iMark = 0;
	if (iSize == 1)
		::GetTickCount();
#endif

	return pStore;
	}

void CString::Append (LPCSTR pString, int iLength, DWORD dwFlags)

//	Append
//
//	Appends the given string.

	{
	if (iLength == -1)
		iLength = strlen(pString);

	//	Resize allocation

	DWORD dwSizeFlags = FLAG_PRESERVE_CONTENTS;
	if (dwFlags & FLAG_ALLOC_EXTRA)
		dwSizeFlags |= FLAG_GEOMETRIC_GROWTH;

	Size(GetLength() + iLength + 1, dwSizeFlags);

	//	Append

	int iStart = GetLength();
	for (int i = 0; i < iLength; i++)
		m_pStore->pString[iStart + i] = pString[i];

	m_pStore->iLength += iLength;

	//	NULL terminate

	m_pStore->pString[GetLength()] = '\0';
	}

void CString::Capitalize (CapitalizeOptions iOption)

//	Capitalize
//
//	Capitalizes the string in place

	{
	//	Can't deal with NULL strings

	if (GetLength() == 0)
		return;

	//	Make sure we have our own copy

	Size(GetLength()+1, FLAG_PRESERVE_CONTENTS);
	char *pPos = GetASCIIZPointer();

	//	Capitalize

	switch (iOption)
		{
		case capFirstLetter:
			{
			//	Capitalize first letter

			*pPos = (char)(DWORD)::CharUpper((LPTSTR)(int)*pPos);
			break;
			}

		default:
			ASSERT(false);
		}
	}

#ifndef INLINE_DECREF
void CString::DecRefCount (void)

//	DecRefCount
//
//	Decrements reference count on storage

	{
	//	If we've got a storage block, de-reference it

	if (m_pStore)
		{
		ASSERT(m_pStore->iRefCount > 0);
		m_pStore->iRefCount--;

		//	If we're done, free the block

		if (m_pStore->iRefCount == 0)
			{
			EnterCriticalSection(&g_csStore);
			if (!IsExternalStorage())
				HeapFree(GetProcessHeap(), 0, m_pStore->pString);
			AddToFreeList(m_pStore, 1);
			LeaveCriticalSection(&g_csStore);
			}
		}
	}
#endif

void CString::FreeStore (PSTORESTRUCT pStore)

//	FreeStore
//
//	Free the store

	{
	EnterCriticalSection(&g_csStore);
	if (pStore->iAllocSize >= 0)	//	!IsExternalStorage()
		HeapFree(GetProcessHeap(), 0, pStore->pString);
	AddToFreeList(pStore, 1);

#ifdef DEBUG_STRING_LEAKS
	g_iStoreCount--;
#endif

	LeaveCriticalSection(&g_csStore);
	}

char *CString::GetASCIIZPointer (void) const

//	GetASCIIZPointer
//
//	Returns a pointer to a NULL terminated array of characters

	{
	//	We always NULL terminate strings

	return GetPointer();
	}

int CString::GetLength (void) const

//	GetLength
//
//	Returns the number of characters in the string

	{
	if (m_pStore)
		return m_pStore->iLength;
	else
		return 0;
	}

int CString::GetMemoryUsage (void) const

//	GetMemoryUsage
//
//	Returns the total memory used.

	{
	if (m_pStore == NULL || m_pStore->iAllocSize <= 0)
		return 0;

	return (sizeof STORESTRUCT) + m_pStore->iAllocSize;
	}

char *CString::GetPointer (void) const

//	GetPointer
//
//	Returns a pointer to an array of characters. The caller must not
//	access elements beyond the length of the string

	{
	if (m_pStore)
		return m_pStore->pString;
	else
		return const_cast<char *>("");
	}

char *CString::GetWritePointer (int iLength)

//	GetWritePointer
//
//	Returns a pointer that allows the user to write to the string.
//	On return, the buffer is guaranteed to be at least iLength.

	{
	Size(iLength + 1, FLAG_PRESERVE_CONTENTS);

	m_pStore->iLength = iLength;
	m_pStore->pString[iLength] = '\0';

	return m_pStore->pString;
	}

void CString::GrowToFit (int iLength)

//	GrowToFit
//
//	Makes sure the string is allocated to at least the given length.

	{
	bool bInit = (m_pStore == NULL);
	Size(iLength + 1, FLAG_PRESERVE_CONTENTS);
	if (bInit)
		{
		m_pStore->iLength = 0;
		m_pStore->pString[0] = '\0';
		}
	}

void CString::InitLowerCaseAbsoluteTable (void)

//	InitLowerCaseAbsoluteTable
//
//	Initialize lowercase table. The table generates a lowercase map
//	for the Windows Western character set. This should be used only
//	for non-localized purposes, such as absolute sorting used by CSymbolTable

	{
	if (!g_bLowerCaseAbsoluteTableInit)
		{
		int chChar;

		for (chChar = 0; chChar < 256; chChar++)
			{
			if (chChar >= 'A' && chChar <= 'Z')
				g_LowerCaseAbsoluteTable[chChar] = chChar + 0x20;
			else if (chChar == 0x8A)
				g_LowerCaseAbsoluteTable[chChar] = (BYTE)0x9A;
			else if (chChar == 0x8C)
				g_LowerCaseAbsoluteTable[chChar] = (BYTE)0x9C;
			else if (chChar == 0x8E)
				g_LowerCaseAbsoluteTable[chChar] = (BYTE)0x9E;
			else if (chChar == 0x9F)
				g_LowerCaseAbsoluteTable[chChar] = (BYTE)0xFF;
			else if (chChar >= 0xC0 && chChar <= 0xD6)
				g_LowerCaseAbsoluteTable[chChar] = chChar + 0x20;
			else if (chChar >= 0xD8 && chChar <= 0xDE)
				g_LowerCaseAbsoluteTable[chChar] = chChar + 0x20;
			else
				g_LowerCaseAbsoluteTable[chChar] = chChar;
			}

		g_bLowerCaseAbsoluteTableInit = true;
		}
	}

ALERROR CString::LoadHandler (CUnarchiver *pUnarchiver)

//	Load
//
//	Load the string

	{
	ALERROR error;
	DWORD dwLength;

	//	Load the length of the string

	if (error = pUnarchiver->ReadData((char *)&dwLength, sizeof(DWORD)))
		return error;

	//	If we've got nothing, leave now

	if (dwLength == 0)
		return NOERROR;

	//	Size the string appropriately

	Size(dwLength+1);

	//	Load the string

	if (error = pUnarchiver->ReadData(m_pStore->pString, dwLength))
		return error;

	m_pStore->iLength = (int)dwLength;

	//	NULL terminate

	m_pStore->pString[dwLength] = '\0';

	//	Skip beyond to pad to DWORD boundary

	dwLength %= sizeof(DWORD);
	if (dwLength > 0)
		{
		if (error = pUnarchiver->ReadData((char *)&dwLength, sizeof(DWORD) - dwLength))
			return error;
		}

	return NOERROR;
	}

void CString::ReadFromStream (IReadStream *pStream)

//	ReadFromStream
//
//	Reads the string from a stream

	{
	DWORD dwLength;

	//	Load the length of the string and size the string

	pStream->Read((char *)&dwLength, sizeof(DWORD));
	if (dwLength == 0)
		{
		Truncate(0);
		return;
		}

	Size(dwLength+1);

	//	Load the string

	pStream->Read(m_pStore->pString, dwLength);
	m_pStore->iLength = (int)dwLength;

	//	NULL terminate

	m_pStore->pString[dwLength] = '\0';

	//	Skip beyond to pad to DWORD boundary

	dwLength %= sizeof(DWORD);
	if (dwLength > 0)
		pStream->Read((char *)&dwLength, sizeof(DWORD) - dwLength);
	}

ALERROR CString::SaveHandler (CArchiver *pArchiver)

//	Save
//
//	Override saving the string because we need to do special stuff
//	with the string store.

	{
	ALERROR error;
	DWORD dwLength;

	//	Write out the length of the string

	dwLength = (DWORD)GetLength();
	if (error = pArchiver->WriteData((char *)&dwLength, sizeof(DWORD)))
		return error;

	//	Write out the string itself

	if (dwLength > 0)
		{
		if (error = pArchiver->WriteData(GetPointer(), dwLength))
			return error;

		//	Pad the string so that we're always on a DWORD boundary

		dwLength %= sizeof(DWORD);
		if (dwLength > 0)
			{
			if (error = pArchiver->WriteData((char *)&dwLength, sizeof(DWORD)-dwLength))
				return error;
			}
		}

	return NOERROR;
	}

void CString::Size (int iLength, DWORD dwFlags)

//	Size
//
//	Resizes the storage so that it is at least as big as the
//	given length. This routine also makes sure that we are the only
//	object using this storage.
//	Return FALSE if failed

	{
	//	If we don't have storage yet, allocate a new one

	if (m_pStore == NULL)
		{
		m_pStore = AllocStore(iLength, TRUE);
		if (m_pStore == NULL)
			throw CException(ERR_MEMORY);
		}

	//	If we're sharing the store with someone else, make our own copy

	if (m_pStore->iRefCount > 1)
		{
		PSTORESTRUCT pNewStore;

		pNewStore = AllocStore(iLength, TRUE);
		if (pNewStore == NULL)
			throw CException(ERR_MEMORY);

		//	If we're supposed to preserve contents, copy the content over

		if (dwFlags & FLAG_PRESERVE_CONTENTS)
			{
			int i;
			int iCopyLen = Min(m_pStore->iLength+1, iLength);

			for (i = 0; i < iCopyLen; i++)
				pNewStore->pString[i] = m_pStore->pString[i];

			pNewStore->iLength = m_pStore->iLength;
			}

		m_pStore->iRefCount--;
		m_pStore = pNewStore;
		}

	//	If we're not big enough, re-allocate
	//	Note that when iAllocSize is negative (meaning that we have an
	//	external storage) we always reallocate

	if (IsExternalStorage() || m_pStore->iAllocSize < iLength)
		{
		int iNewAlloc;
		if (dwFlags & FLAG_GEOMETRIC_GROWTH)
			iNewAlloc = Max(iLength, m_pStore->iAllocSize * 2);
		else
			iNewAlloc = iLength;

		char *pNewString = (char *)HeapAlloc(GetProcessHeap(), 0, iNewAlloc);
		if (pNewString == NULL)
			throw CException(ERR_MEMORY);

		//	If we're supposed to preserve contents, copy the content over

		if (dwFlags & FLAG_PRESERVE_CONTENTS)
			utlMemCopy(m_pStore->pString, pNewString, Min(m_pStore->iLength, iLength));

		//	Only free if this is our storage

		if (!IsExternalStorage())
			HeapFree(GetProcessHeap(), 0, m_pStore->pString);

		m_pStore->pString = pNewString;
		m_pStore->iAllocSize = iNewAlloc;
		}

	//	Done

	ASSERT(m_pStore);
	ASSERT(m_pStore->iRefCount == 1);
	ASSERT(m_pStore->iAllocSize >= iLength);
	}

void CString::Transcribe (const char *pString, int iLen)

//	Transcribe
//
//	Copies the string for a char * buffer.
//	If iLen is -1, we assume ASCIIZ

	{
	int i;

	//	Handle NULL

	if (pString == NULL)
		{
		DecRefCount();
		m_pStore = NULL;
		return;
		}

	//	Handle ASCIIZ

	if (iLen == -1)
		iLen = lstrlen(pString);

	//	Handle 0 length

	if (iLen == 0)
		{
		DecRefCount();
		m_pStore = NULL;
		return;
		}

	//	Allocate size

	Size(iLen+1);

	for (i = 0; i < iLen; i++)
		m_pStore->pString[i] = pString[i];

	m_pStore->iLength = iLen;

	//	NULL terminate

	m_pStore->pString[iLen] = '\0';
	}

void CString::Truncate (int iLength)

//	Truncate
//
//	Truncate the string to the given length. The length cannot be greater
//	than the current length of the string

	{
	ASSERT(iLength >= 0 && iLength <= GetLength());

	if (iLength == 0)
		{
		DecRefCount();
		m_pStore = NULL;
		return;
		}

	//	Call this just to make sure that we have our own copy

	Size(iLength+1, FLAG_PRESERVE_CONTENTS);

	//	Set the new length

	m_pStore->iLength = iLength;
	m_pStore->pString[iLength] = '\0';
	}

void CString::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Write out to a stream

	{
	DWORD dwLength;

	//	Write out the length of the string

	dwLength = GetLength();
	pStream->Write((char *)&dwLength, sizeof(dwLength));

	//	Write out the string itself

	if (dwLength > 0)
		{
		pStream->Write(GetPointer(), dwLength);

		//	Pad the string so that we're always on a DWORD boundary

		dwLength %= sizeof(DWORD);
		if (dwLength > 0)
			pStream->Write((char *)&dwLength, sizeof(DWORD)-dwLength);
		}
	}

int Kernel::strCompare (const CString &sString1, const CString &sString2)

//	strCompare
//
//	Compares two strings are returns 1 if sString1 is > sString2; -1 if sString1
//	is < sString2; and 0 if both strings are equal

	{
	char *pPos1;
	char *pPos2;
	int i, iLen;

	//	Setup

	pPos1 = sString1.GetPointer();
	pPos2 = sString2.GetPointer();
	iLen = Min(sString1.GetLength(), sString2.GetLength());

	//	Compare by commonality

	for (i = 0; i < iLen; i++)
		{
		if (CharLower((LPTSTR)(BYTE)(*pPos1)) > CharLower((LPTSTR)(BYTE)(*pPos2)))
			return 1;
		else if (CharLower((LPTSTR)(BYTE)(*pPos1)) < CharLower((LPTSTR)(BYTE)(*pPos2)))
			return -1;

		pPos1++;
		pPos2++;
		}

	//	If the strings match up to a point, check to see which is 
	//	longest.

	if (sString1.GetLength() > sString2.GetLength())
		return 1;
	else if (sString1.GetLength() < sString2.GetLength())
		return -1;
	else
		return 0;
	}

int Kernel::strCompareAbsolute (const CString &sString1, const CString &sString2)

//	strCompareAbsolute
//
//	Compares two strings are returns 1 if sString1 is > sString2; -1 if sString1
//	is < sString2; and 0 if both strings are equal.
//
//	The resulting sort order does not change with locale. Use this only for
//	internal sorting (e.g., symbol tables).

	{
	char *pPos1;
	char *pPos2;
	int i, iLen;

	//	Setup

	pPos1 = sString1.GetPointer();
	pPos2 = sString2.GetPointer();
	iLen = Min(sString1.GetLength(), sString2.GetLength());

	//	Compare by commonality

	for (i = 0; i < iLen; i++)
		{
		char chChar1 = strLowerCaseAbsolute(*pPos1++);
		char chChar2 = strLowerCaseAbsolute(*pPos2++);

		if (chChar1 > chChar2)
			return 1;
		else if (chChar1 < chChar2)
			return -1;
		}

	//	If the strings match up to a point, check to see which is 
	//	longest.

	if (sString1.GetLength() > sString2.GetLength())
		return 1;
	else if (sString1.GetLength() < sString2.GetLength())
		return -1;
	else
		return 0;
	}

int Kernel::strCompareAbsolute (LPCSTR pS1, LPCSTR pS2)
	{
	while (*pS1 != '\0' && *pS2 != '\0')
		{
		char chChar1 = strLowerCaseAbsolute(*pS1++);
		char chChar2 = strLowerCaseAbsolute(*pS2++);

		if (chChar1 > chChar2)
			return 1;
		else if (chChar1 < chChar2)
			return -1;
		}

	//	If the strings match up to a point, check to see if we hit
	//	the end of both.

	if (*pS2 == '\0' && *pS1 == '\0')
		return 0;
	else if (*pS2 == '\0')
		return 1;
	else
		return -1;
	}

ALERROR Kernel::strDelimitEx (const CString &sString, 
					  char cDelim, 
					  DWORD dwFlags,
					  int iMinParts, 
					  TArray<CString> *retList)

//	strDelimitEx
//
//	Parses the given string into multiple strings using the given delimeter. 

	{
	char *pPos;
	char *pPartStart;
	int iPartLength;
	int iPartCount;

	ASSERT(cDelim != '\0');
	bool bDelimitComma = ((dwFlags & DELIMIT_COMMA) ? true : false);
	bool bDelimitSemi = ((dwFlags & DELIMIT_SEMI_COLON) ? true : false);
	bool bQuoteEscape = ((dwFlags & DELIMIT_QUOTE_ESCAPE) ? true : false);

	//	Auto comma means that we check to see if the delimiter is a comma 
	//	instead of the standard delimiter.

	if (dwFlags & DELIMIT_AUTO_COMMA)
		{
		//	See if we use the standard delimiter.

		pPos = sString.GetPointer();
		bool bDelimOK = false;
		while (*pPos != '\0')
			{
			if (*pPos == cDelim)
				{
				bDelimOK = true;
				break;
				}
			pPos++;
			}

		//	If not, then use comma as a delimiter

		if (!bDelimOK)
			bDelimitComma = true;
		}

	//	Initialize string list

	retList->DeleteAll();

	//	Get info about the string

	pPos = sString.GetPointer();

	//	Parse the string

	iPartCount = 0;
	pPartStart = pPos;
	iPartLength = 0;

	bool bInQuotes = false;

	while (*pPos != '\0')
		{
		//	Quote escape

		if (bInQuotes)
			{
			if (*pPos == '\"')
				{
				bInQuotes = false;
				}

			pPos++;
			iPartLength++;
			}

		//	Quotes?

		else if (bQuoteEscape && *pPos == '\"')
			{
			pPos++;
			iPartLength++;
			bInQuotes = true;
			}

		//	If we've found a delimeter, then flush the string up to now
		//	to the current part.

		else if (*pPos == cDelim
				|| (bDelimitComma && *pPos == ',')
				|| (bDelimitSemi && *pPos == ';'))
			{
			CString sPart(pPartStart, iPartLength);
			if (dwFlags & DELIMIT_TRIM_WHITESPACE)
				sPart = strTrimWhitespace(sPart);

			//	If we have quotes around the whole entry, then remove them

			if (bQuoteEscape && sPart.GetLength() > 2)
				{
				char *pPos = sPart.GetASCIIZPointer();
				if (pPos[0] == '\"' && pPos[sPart.GetLength() - 1] == '\"')
					sPart = strSubString(sPart, 1, sPart.GetLength() - 2);
				}

			//	Add

			if ((dwFlags & DELIMIT_ALLOW_BLANK_STRINGS) || !sPart.IsBlank())
				{
				retList->Insert(sPart);
				iPartCount++;
				}

			//	Skip to the next part

			pPos++;
			iPartLength = 0;

			pPartStart = pPos;
			}
		else
			{
			pPos++;
			iPartLength++;
			}
		}

	//	Transcribe the last part

	if (iPartLength > 0)
		{
		CString sPart(pPartStart, iPartLength);
		if (dwFlags & DELIMIT_TRIM_WHITESPACE)
			sPart = strTrimWhitespace(sPart);

		if ((dwFlags & DELIMIT_ALLOW_BLANK_STRINGS) || !sPart.IsBlank())
			{
			retList->Insert(sPart);
			iPartCount++;
			}
		}

	//	If we haven't yet created the minimum number of parts,
	//	create the rest

	if (iMinParts != -1 && iPartCount < iMinParts)
		{
		int i;

		for (i = 0; i < iMinParts - iPartCount; i++)
			retList->Insert(NULL_STR);
		}

	//	Done

	return NOERROR;
	}

int Kernel::strDelimitCount (const CString &sString, char cDelim, DWORD dwFlags)

//	strDelimitCount
//
//	Returns the number of delimited parts in the string

	{
	char *pPos = sString.GetASCIIZPointer();
	int iCount = 0;
	bool bInName = false;

	while (*pPos != '\0')
		{
		switch (*pPos)
			{
			case ';':
				if (bInName || (dwFlags & DELIMIT_ALLOW_BLANK_STRINGS))
					{
					iCount++;
					bInName = false;
					}
				break;

			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if (!(dwFlags & DELIMIT_TRIM_WHITESPACE))
					bInName = true;
				break;

			default:
				bInName = true;
			}

		pPos++;
		}

	if (bInName || (dwFlags & DELIMIT_ALLOW_BLANK_STRINGS))
		iCount++;

	return iCount;
	}

CString Kernel::strDelimitGet (const CString &sString, char cDelim, DWORD dwFlags, int iIndex)

//	strDelimitGet
//
//	Returns the delimited part by index

	{
	char *pPos = sString.GetASCIIZPointer();
	int iCount = 0;
	bool bInName = false;
	char *pStart = NULL;

	while (*pPos != '\0' && pStart == NULL)
		{
		switch (*pPos)
			{
			case ';':
				if (bInName || (dwFlags & DELIMIT_ALLOW_BLANK_STRINGS))
					{
					iCount++;
					bInName = false;
					}
				break;

			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if (!bInName && !(dwFlags & DELIMIT_TRIM_WHITESPACE))
					{
					if (iCount == iIndex)
						pStart = pPos;
					bInName = true;
					}
				break;

			default:
				if (!bInName)
					{
					if (iCount == iIndex)
						pStart = pPos;
					bInName = true;
					}
				break;
			}

		pPos++;
		}

	if (pStart)
		{
		pPos = pStart;

		//	Find the end

		while (*pPos != '\0' && *pPos != ';')
			pPos++;

		//	Trim trailing whitespace (if necessary)

		if (dwFlags & DELIMIT_TRIM_WHITESPACE)
			{
			while (pPos > pStart && strIsWhitespace(pPos-1))
				pPos--;
			}

		return CString(pStart, pPos - pStart);
		}
	else
		return NULL_STR;
	}

bool Kernel::strEquals (const CString &sString1, const CString &sString2)

//	strEquals
//
//	Returns TRUE if the strings are equal

	{
	char *pPos1;
	char *pPos2;
	int i, iLen;

	//	If the strings aren't the same length then don't bother

	iLen = sString1.GetLength();
	if (iLen != sString2.GetLength())
		return false;

	//	Setup

	pPos1 = sString1.GetPointer();
	pPos2 = sString2.GetPointer();

	//	Compare by commonality

	for (i = 0; i < iLen; i++)
		if (CharLower((LPTSTR)(BYTE)(pPos1[i])) != CharLower((LPTSTR)(BYTE)(pPos2[i])))
			return false;

	return true;
	}

bool Kernel::strEqualsCase (const CString &sString1, const CString &sString2)

//	strEqualsCase
//
//	Returns TRUE if the strings are exactly equal (case-sensitive).

	{
	//	If the strings aren't the same length then don't bother

	int iLen = sString1.GetLength();
	if (iLen != sString2.GetLength())
		return false;

	//	Setup

	char *pPos1 = sString1.GetPointer();
	char *pPos1End = pPos1 + iLen;
	char *pPos2 = sString2.GetPointer();

	//	Compare by character

	for (; pPos1 < pPos1End; pPos1++, pPos2++)
		if (*pPos1 != *pPos2)
			return false;

	return true;
	}

int Kernel::strFind (const CString &sString, const CString &sStringToFind)

//	strFind
//
//	Finds the target string in the given string and returns the
//	offset in sString at which the target starts. If the target is
//	not found anywhere in sString then we return -1
//
//	Find is case insensitive.

	{
	int iStringLen = sString.GetLength();
	int iTargetLen = sStringToFind.GetLength();
	char *pString = sString.GetPointer();
	int i;

	//	If the target is null, then we don't match

	if (sStringToFind.IsBlank())
		return -1;

	//	Search for a match at successive offsets of sString
	//	until we're past the point where sStringToFind would not
	//	fit.

	for (i = 0; i <= iStringLen - iTargetLen; i++)
		{
		CString sTest(pString + i, iTargetLen, TRUE);

		if (strCompareAbsolute(sTest, sStringToFind) == 0)
			return i;
		}

	//	Didn't find it

	return -1;
	}

CString Kernel::strFormatBytes (DWORD dwBytes)

//	strFormatBytes
//
//	Converts a number of bytes to a string, as follows:
//
//	1 KB - 999 KB
//	1 MB - 999 MB
//	1 GB

	{
	if (dwBytes >= 1000000000)
		{
		dwBytes += 50000000;
		DWORD dwMajor = dwBytes / 1000000000;
		DWORD dwMinor = (dwBytes % 1000000000) / 100000000;
		return strPatternSubst(CONSTLIT("%d.%d GB"), dwMajor, dwMinor);
		}
	else if (dwBytes >= 1000000)
		{
		dwBytes += 50000;
		DWORD dwMajor = dwBytes / 1000000;
		DWORD dwMinor = (dwBytes % 1000000) / 100000;
		return strPatternSubst(CONSTLIT("%d.%d MB"), dwMajor, dwMinor);
		}
	else
		{
		dwBytes += 52;
		DWORD dwMajor = dwBytes / 1024;
		DWORD dwMinor = (dwBytes % 1024) / 103;
		return strPatternSubst(CONSTLIT("%d.%d KB"), dwMajor, dwMinor);
		}
	}

CString Kernel::strFormatInteger (int iValue, int iMinFieldWidth, DWORD dwFlags)

//	strFormatInteger
//
//	Converts an integer to a string

	{
	int i;

	DWORD dwRadix = 10;
	bool bNegative = (iValue < 0) && !(dwFlags & FORMAT_UNSIGNED);
	DWORD dwValue = (bNegative ? (DWORD)(-iValue) : (DWORD)iValue);

	//	Convert to new base (we end up in reverse order)

	DWORD Result[100];
	int iDigitCount = 0;
	DWORD dwRemainder = dwValue;

	do
		{
		Result[iDigitCount] = (dwRemainder % dwRadix);

		iDigitCount++;
		dwRemainder = dwRemainder / dwRadix;
		}
	while (dwRemainder > 0);

	//	Compute the length of the result (not counting padding,
	//	but including thousands separators).

	int iResultLength = iDigitCount;
	if (dwFlags & FORMAT_THOUSAND_SEPARATOR)
		iResultLength += (iDigitCount - 1) / 3;

	if (bNegative)
		iResultLength += 1;

	//	Total length

	int iTotalLength = (iMinFieldWidth == -1 ? iResultLength : Max(iResultLength, iMinFieldWidth));

	//	Allocate result string

	CString sResult;
	char *pPos = sResult.GetWritePointer(iTotalLength);

	//	Padding

	char *pEndPos = pPos + (iTotalLength - iResultLength);
	while (pPos < pEndPos)
		*pPos++ = ((dwFlags & FORMAT_LEADING_ZERO) ? '0' : ' ');

	//	Sign

	if (bNegative)
		*pPos++ = '-';

	//	Write the result backwards

	char *pDigitPos = sResult.GetPointer() + (iTotalLength - 1);
	for (i = 0; i < iDigitCount; i++)
		{
		if ((dwFlags & FORMAT_THOUSAND_SEPARATOR) && i > 0 && (i % 3) == 0)
			*pDigitPos-- = ',';

		*pDigitPos-- = (char)('0' + Result[i]);
		}

	//	Done

	return sResult;
	}

CString Kernel::strFormatInteger (INT64 iValue, int iMinFieldWidth, DWORD dwFlags)

//	strFormatInteger
//
//	Converts an integer to a string

	{
	int i;

	DWORDLONG dwRadix = 10;
	bool bNegative = (iValue < 0) && !(dwFlags & FORMAT_UNSIGNED);
	DWORDLONG dwValue = (bNegative ? (DWORD)(-iValue) : (DWORD)iValue);

	//	Convert to new base (we end up in reverse order)

	DWORD Result[100];
	int iDigitCount = 0;
	DWORDLONG dwRemainder = dwValue;

	do
		{
		Result[iDigitCount] = (DWORD)(dwRemainder % dwRadix);

		iDigitCount++;
		dwRemainder = dwRemainder / dwRadix;
		}
	while (dwRemainder > 0);

	//	Compute the length of the result (not counting padding,
	//	but including thousands separators).

	int iResultLength = iDigitCount;
	if (dwFlags & FORMAT_THOUSAND_SEPARATOR)
		iResultLength += (iDigitCount - 1) / 3;

	if (bNegative)
		iResultLength += 1;

	//	Total length

	int iTotalLength = (iMinFieldWidth == -1 ? iResultLength : Max(iResultLength, iMinFieldWidth));

	//	Allocate result string

	CString sResult;
	char *pPos = sResult.GetWritePointer(iTotalLength);

	//	Padding

	char *pEndPos = pPos + (iTotalLength - iResultLength);
	while (pPos < pEndPos)
		*pPos++ = ((dwFlags & FORMAT_LEADING_ZERO) ? '0' : ' ');

	//	Sign

	if (bNegative)
		*pPos++ = '-';

	//	Write the result backwards

	char *pDigitPos = sResult.GetPointer() + (iTotalLength - 1);
	for (i = 0; i < iDigitCount; i++)
		{
		if ((dwFlags & FORMAT_THOUSAND_SEPARATOR) && i > 0 && (i % 3) == 0)
			*pDigitPos-- = ',';

		*pDigitPos-- = (char)('0' + Result[i]);
		}

	//	Done

	return sResult;
	}

CString Kernel::strFromDouble (double rValue, int iDecimals)

//	strFromDouble
//
//	Converts a double to a string

	{
	CString sResult;

	if (iDecimals == -1)
		{
		char *pPos = sResult.GetWritePointer(_CVTBUFSIZE);
		if (_gcvt_s(pPos, sResult.GetLength(), rValue, 17) != 0)
			return CONSTLIT("NaN");
		}
	else
		{
		CString sDigits;
		char *pPos = sDigits.GetWritePointer(_CVTBUFSIZE);

		int iDecimalPoint;
		int iSign;

		if (_fcvt_s(pPos, sDigits.GetLength(), rValue, iDecimals, &iDecimalPoint, &iSign) != 0)
			return CONSTLIT("NaN");

		char *pSrc = sDigits.GetASCIIZPointer();

		pPos = sResult.GetWritePointer(_CVTBUFSIZE);
		char *pPosEnd = pPos + sResult.GetLength();
		if (iSign != 0)
			*pPos++ = '-';

		if (iDecimalPoint <= 0)
			{
			*pPos++ = '0';
			*pPos++ = '.';

			if (*pSrc == '\0')
				*pPos++ = '0';
			else
				{
				while (iDecimalPoint < 0 && pPos < pPosEnd)
					{
					*pPos++ = '0';
					iDecimalPoint++;
					}
				}
			}
		else
			{
			while (iDecimalPoint > 0 && pPos < pPosEnd)
				{
				*pPos++ = *pSrc++;
				iDecimalPoint--;
				}

			if (pPos < pPosEnd)
				*pPos++ = '.';
			}

		while (*pSrc != '\0' && pPos < pPosEnd)
			*pPos++ = *pSrc++;

		if (pPos < pPosEnd)
			*pPos++ = '\0';
		else
			pPosEnd[-1] = '\0';
		}

	//	Figure out how much we wrote

	char *pPos = sResult.GetASCIIZPointer();
	int iLen = strlen(pPos);

	//	If we end in a '.' then add a terminating 0

	if (iLen == 0)
		return CONSTLIT("NaN");

	if (pPos[iLen - 1] == '.')
		{
		pPos[iLen++] = '0';
		pPos[iLen] = '\0';
		}

	sResult.Truncate(iLen);
	return sResult;
	}

CString Kernel::strFromInt (int iInteger, bool bSigned)

//	CStringFromInt
//
//	Converts an integer to a string

	{
	char szString[256];
	int iLen;

	if (bSigned)
		iLen = wsprintf(szString, "%d", iInteger);
	else
		iLen = wsprintf(szString, "%u", iInteger);

	CString sString(szString, iLen);
	return sString;
	}

void CString::INTStringCleanUp (void)

//	INTStringCleanUp
//
//	Cleans up the basic global data

	{
	//DeleteCriticalSection(&g_csStore);
	}

ALERROR CString::INTStringInit (void)

//	INTStringInit
//
//	Initializes the subsystem. This routine must be called before
//	any other call.

	{
	InitializeCriticalSection(&g_csStore);
	InitLowerCaseAbsoluteTable();
	return NOERROR;
	}

CString Kernel::strCapitalize (const CString &sString, int iOffset)

//	strCapitalize
//
//	Capitalizes the given letter

	{
	CString sUpper = sString;
	CharUpperBuff(sUpper.GetWritePointer(sUpper.GetLength()) + iOffset, 1);
	return sUpper;
	}

CString Kernel::strCapitalizeWords (const CString &sString)

//	strCapitalizeWords
//
//	Capitalizes the first letter in every word in the string

	{
	CString sUpper;
	char *pSrc = sString.GetASCIIZPointer();
	char *pDest = sUpper.GetWritePointer(sString.GetLength());
	bool bCapitalize = true;
	while (*pSrc != '\0')
		{
		*pDest = *pSrc;

		if (*pSrc == ' ' || *pSrc == '-')
			bCapitalize = true;
		else if (bCapitalize)
			{
			::CharUpperBuff(pDest, 1);
			bCapitalize = false;
			}

		pDest++;
		pSrc++;
		}

	return sUpper;
	}

CString Kernel::strCat (const CString &sString1, const CString &sString2)

//	strCat
//
//	Returns the concatenation of the two string

	{
	CString sCat;

	sCat = sString1;
	sCat.Append(sString2);
	return sCat;
	}

CString Kernel::strCEscapeCodes (const CString &sString)

//	strCEscapeCodes
//
//	Parses C Language escape codes and returns the string

	{
	char *pPos = sString.GetASCIIZPointer();
	enum States
		{
		stateStart,
		stateFoundBackslash,
		stateDone,
		};
	int iState = stateStart;
	CString sResult;
	char *pStart = pPos;

	while (iState != stateDone)
		{
		switch (iState)
			{
			case stateStart:
				if (*pPos == '\0')
					{
					if (pStart == sString.GetASCIIZPointer())
						return sString;
					else
						return strCat(sResult, CString(pStart, pPos - pStart));
					}
				else if (*pPos == '\\')
					iState = stateFoundBackslash;
				break;

			case stateFoundBackslash:
				if (*pPos == '\0')
					return strCat(sResult, CString(pStart, pPos - pStart));
				else
					{
					char *pPrevPos = pPos - 1;
					CString sChar;
					if (*pPos == 'n')
						sChar = CString("\n");
					else if (*pPos == 'r')
						sChar = CString("\r");
					else if (*pPos == 't')
						sChar = CString("\t");
					else if (*pPos == 'x' || *pPos == 'X')
						{
						pPos++;
						int iFirstDigit = (*pPos != '\0' ? strGetHexDigit(pPos++) : 0);
						int iSecondDigit = (*pPos != '\0' ? strGetHexDigit(pPos) : 0);

						char chChar = (char)(16 * iFirstDigit + iSecondDigit);
						sChar = CString(&chChar, 1);
						}
					else
						sChar = CString(pPos, 1);

					sResult.Append(CString(pStart, pPrevPos - pStart));
					sResult.Append(sChar);
					if (*pPos == '\0')
						return sResult;

					pStart = pPos + 1;
					}
				iState = stateStart;
				break;
			}

		pPos++;
		}

	return sResult;
	}

CString Kernel::strConvertToToken (const CString &sString, bool bLowercase)

//	strConvertToToken
//
//	Substitutes all non-alphanumeric characters with an underscore.

	{
	char *pSrc = sString.GetASCIIZPointer();
	char *pSrcEnd = pSrc + sString.GetLength();

	CString sResult;
	char *pDest = sResult.GetWritePointer(sString.GetLength());

	while (pSrc < pSrcEnd)
		{
		if (strIsAlphaNumeric(pSrc))
			{
			if (bLowercase)
				*pDest++ = (char)(DWORD)CharLower((LPTSTR)(BYTE)(*pSrc++));
			else
				*pDest++ = *pSrc++;
			}
		else
			{
			*pDest++ = '_';
			pSrc++;
			}
		}

	return sResult;
	}

bool Kernel::strEndsWith (const CString &sString, const CString &sStringToFind)

//	strEndsWith
//
//	Returns TRUE if sString ends with sStringToFind

	{
	int iStrLen = sString.GetLength();
	int iTargetLen = sStringToFind.GetLength();
	if (iTargetLen > iStrLen)
		return false;

	char *pPos = sString.GetASCIIZPointer() + (iStrLen - iTargetLen);
	char *pEndPos = pPos + iTargetLen;
	char *pTarget = sStringToFind.GetASCIIZPointer();

	while (pPos < pEndPos)
		{
		if (CharLower((LPTSTR)(BYTE)(*pPos++)) != CharLower((LPTSTR)(BYTE)(*pTarget++)))
			return false;
		}

	return true;
	}

CString Kernel::strFormatMicroseconds (DWORD dwMicroseconds)

//	strFormatMicroseconds
//
//	Formats to the form:
//
//	11.1 ms
//	133 us

	{
	if (dwMicroseconds > 1000000)
		return strPatternSubst("%d.%03d s", dwMicroseconds / 1000000, dwMicroseconds % 1000000);
	else if (dwMicroseconds > 1000)
		return strPatternSubst("%d.%03d ms", dwMicroseconds / 1000, dwMicroseconds % 1000);
	else
		return strPatternSubst("%d \xb5s", dwMicroseconds);
	}

CString Kernel::strFormatMilliseconds (DWORD dwMilliseconds)

//	strFormatMilliseconds
//
//	Formats to the form:
//
//	1.001 s
//	132 ms

	{
	if (dwMilliseconds > 1000)
		return strPatternSubst("%d.%03d s", dwMilliseconds / 1000, dwMilliseconds % 1000);
	else
		return strPatternSubst("%d ms", dwMilliseconds);
	}

int Kernel::strGetHexDigit (const char *pPos)

//	strGetHexDigit
//
//	Returns a number from 0-15

	{
	if (*pPos >= '0' && *pPos <= '9')
		return (*pPos) - '0';
	else if (*pPos >= 'a' && *pPos <= 'f')
		return 10 + ((*pPos) - 'a');
	else if (*pPos >= 'A' && *pPos <= 'F')
		return 10 + ((*pPos) - 'A');
	else
		return 0;
	}

char Kernel::strGetHexDigit (int iDigit)
	{
	if (iDigit >= 0 && iDigit < 10)
		return '0' + iDigit;
	else if (iDigit >= 10 && iDigit <= 15)
		return 'a' + (iDigit - 10);
	else
		return '0';
	}

bool Kernel::strIsASCIISymbol (const char *pPos)

//	strIsASCIISymbol
//
//	Returns TRUE if the character is an ASCII symbol

	{
	switch (*pPos)
		{
		case '!':
		case '\"':
		case '#':
		case '$':
		case '%':
		case '&':
		case '\'':
		case '(':
		case ')':
		case '*':
		case '+':
		case ',':
		case '-':
		case '.':
		case '/':
		case ':':
		case ';':
		case '<':
		case '=':
		case '>':
		case '?':
		case '@':
		case '[':
		case '\\':
		case ']':
		case '^':
		case '_':
		case '`':
		case '{':
		case '|':
		case '}':
		case '~':
			return true;

		default:
			return false;
		}
	}

bool Kernel::strIsInt (const CString &sValue, DWORD dwFlags, int *retiValue)

//	strIsInt
//
//	Returns true if this is an integer value.

	{
	const char *pPos = sValue.GetASCIIZPointer();
	const char *pPosEnd;
	bool bError;
	int iValue = strParseInt(pPos, 0, dwFlags, &pPosEnd, &bError);
	if (retiValue)
		*retiValue = iValue;

	return (!bError && (int)(pPosEnd - pPos) == sValue.GetLength());
	}

bool Kernel::strIsUpper (const char *pPos)

//	strIsUpper
//
//	Returns TRUE if this is an uppercase character.

	{
	char chLower = (char)(DWORD)::CharLowerA((LPSTR)(BYTE)*pPos);
	return (chLower != *pPos);
	}

CString Kernel::strJoin (const TArray<CString> &List, const CString &sConjunction)

//	strJoin
//
//	Joins the strings with the given conjunction. If the conjunction is
//	"oxfordComma", then we add a "and" between the last two.

	{
	int i;

	//	Must be at least one element

	if (List.GetCount() == 0)
		return NULL_STR;

	//	Oxford Comma rule

	else if (strEquals(sConjunction, CONSTLIT("oxfordComma")))
		{
		CString COMMA(", ");
		CString AND(" and ");
		CString COMMA_AND(", and ");

		//	Compute the size of the string

		int iNewSize = List[0].GetLength();
		for (i = 1; i < List.GetCount(); i++)
			{
			if (i == List.GetCount() - 1)
				{
				if (List.GetCount() >= 3)
					iNewSize += COMMA_AND.GetLength() + List[i].GetLength();
				else
					iNewSize += AND.GetLength() + List[i].GetLength();
				}
			else
				iNewSize += COMMA.GetLength() + List[i].GetLength();
			}

		//	Create result

		CString sResult;
		char *pDest = sResult.GetWritePointer(iNewSize);
		for (i = 0; i < List.GetCount(); i++)
			{
			if (i > 0)
				{
				if (i == List.GetCount() - 1)
					{
					if (List.GetCount() >= 3)
						{
						utlMemCopy(COMMA_AND.GetASCIIZPointer(), pDest, COMMA_AND.GetLength());
						pDest += COMMA_AND.GetLength();
						}
					else
						{
						utlMemCopy(AND.GetASCIIZPointer(), pDest, AND.GetLength());
						pDest += AND.GetLength();
						}
					}
				else
					{
					utlMemCopy(COMMA.GetASCIIZPointer(), pDest, COMMA.GetLength());
					pDest += COMMA.GetLength();
					}
				}

			utlMemCopy(List[i].GetASCIIZPointer(), pDest, List[i].GetLength());
			pDest += List[i].GetLength();
			}

		return sResult;
		}

	//	Normal

	else
		{
		//	Compute the size of the string

		int iNewSize = List[0].GetLength();
		for (i = 1; i < List.GetCount(); i++)
			{
			iNewSize += sConjunction.GetLength() + List[i].GetLength();
			}

		//	Create result

		CString sResult;
		char *pDest = sResult.GetWritePointer(iNewSize);
		for (i = 0; i < List.GetCount(); i++)
			{
			if (i > 0)
				{
				utlMemCopy(sConjunction.GetASCIIZPointer(), pDest, sConjunction.GetLength());
				pDest += sConjunction.GetLength();
				}

			utlMemCopy(List[i].GetASCIIZPointer(), pDest, List[i].GetLength());
			pDest += List[i].GetLength();
			}

		return sResult;
		}
	}

bool Kernel::strNeedsEscapeCodes (const CString &sString)
	{
	char *pPos = sString.GetPointer();
	char *pEndPos = pPos + sString.GetLength();
	while (pPos < pEndPos)
		{
		if (*pPos++ < ' ')
			return true;
		}

	return false;
	}

CString Kernel::strLoadFromRes (HINSTANCE hInst, int iResID)

//	strLoadFromRes
//
//	Loads the resource string

	{
	char szString[1024];
	int iLen;

	iLen = LoadString(hInst, iResID, szString, sizeof(szString)-1);

	CString sString(szString, iLen);
	return sString;
	}

double Kernel::strParseDouble (const char *pStart, double rNullResult, const char **retpEnd, bool *retbNullValue)

//  strParseDouble
//
//  Parses a double precision value.

    {
    const char *pPos = pStart;

    //  Skip any leading whitespace

    while (strIsWhitespace(pPos))
        pPos++;

    //  We copy what we've got to a buffer, because we're going to use atof.

    static const int MAX_SIZE = 64;
    const char *pSrc = pPos;
    char szBuffer[MAX_SIZE];
    char *pDest = szBuffer;
    char *pDestEnd = szBuffer + MAX_SIZE;

	//	Parse the integer part

	int iSign = 1;
	if (*pSrc == '-')
		{
		iSign = -1;
		*pDest++ = *pSrc++;
		}
	else if (*pSrc == '+')
		*pDest++ = *pSrc++;

	const char *pInt = pSrc;
	while (*pSrc >= '0' && *pSrc <= '9' && pDest < pDestEnd)
		*pDest++ = *pSrc++;

	const char *pIntEnd = pSrc;
	if (pInt == pIntEnd)
		{
        if (retbNullValue) *retbNullValue = true;
        return rNullResult;
		}

	//	Do we have a fractional part?

	const char *pFrac = NULL;
	const char *pFracEnd = pIntEnd;
	if (*pSrc == '.' && pDest < pDestEnd)
		{
		*pDest++ = *pSrc++;

		pFrac = pSrc;
		while (*pSrc >= '0' && *pSrc <= '9' && pDest < pDestEnd)
			*pDest++ = *pSrc++;

		pFracEnd = pSrc;
		if (pFrac == pFracEnd)
			{
			if (retbNullValue) *retbNullValue = true;
			return rNullResult;
			}
		}

	//	Do we have an exponential part?

	const char *pExp = NULL;
	const char *pExpEnd = pFracEnd;
	int iExpSign = 1;
	if ((*pSrc == 'e' || *pSrc == 'E') && pDest < pDestEnd)
		{
		*pDest++ = *pSrc++;

		if (pDest == pDestEnd)
			;
		else if (*pSrc == '+')
			*pDest++ = *pSrc++;
		else if (*pSrc == '-')
			{
			iExpSign = -1;
			*pDest++ = *pSrc++;
			}

		pExp = pSrc;
		while (*pSrc >= '0' && *pSrc <= '9' && pDest < pDestEnd)
			*pDest++ = *pSrc++;

		pExpEnd = pSrc;
		if (pExp == pExpEnd)
			{
			if (retbNullValue) *retbNullValue = true;
			return rNullResult;
			}
		}

	//	Done

    if (retpEnd) *retpEnd = pSrc;

    //  If we hit the end of the buffer, or we didn't find any valid characters,
    //  then we fail.

    if (pDest == pDestEnd
            || pDest == szBuffer)
        {
        if (retbNullValue) *retbNullValue = true;
        return rNullResult;
        }

    //  Null-terminate our buffer

    *pDest++ = '\0';

    //  Success 

    if (retbNullValue) *retbNullValue = false;
    return atof(szBuffer);
    }

int Kernel::strParseInt (const char *pStart, int iNullResult, DWORD dwFlags, const char **retpEnd, bool *retbNullValue)

//	strParseInt
//
//	pStart: Start parsing. Skips any leading whitespace
//	iNullResult: If there are no valid numbers, returns this value
//	retpEnd: Returns the character at which we stopped parsing
//	retbNullValue: Returns TRUE if there are no valid numbers.

	{
	const char *pPos;
	BOOL bNegative;
	BOOL bFoundNumber;
	BOOL bHex;
	int iInt;

	bool bExpectSeparators = ((dwFlags & PARSE_THOUSAND_SEPARATOR) ? true : false);

	//	Preset

	if (retbNullValue)
		*retbNullValue = false;

	pPos = pStart;
	bNegative = FALSE;
	bFoundNumber = FALSE;
	bHex = FALSE;
	iInt = 0;

	//	Skip whitespace

	while (*pPos == ' ' || *pPos == '\t' || *pPos == '\n' || *pPos == '\r')
		pPos++;

	//	If NULL, then we're done

	if (*pPos == '\0')
		{
		if (retbNullValue)
			*retbNullValue = true;

		if (retpEnd)
			*retpEnd = pPos;

		return iNullResult;
		}

	//	If negative, remember it

	if (*pPos == '-')
		{
		bNegative = TRUE;
		pPos++;
		}
	else if (*pPos == '+')
		pPos++;

	//	See if this is a hex number

	if (*pPos == '0')
		{
		pPos++;
		bFoundNumber = TRUE;

		//	If the next character is x (or X) then we've got
		//	a Hex number

		if (*pPos == 'x' || *pPos == 'X')
			{
			pPos++;
			bHex = TRUE;
			}
		}

	//	Keep parsing

	if (bHex)
		{
		DWORD dwInt = 0;

		while (*pPos != '\0' 
				&& ((*pPos >= '0' && *pPos <= '9') 
					|| (*pPos >= 'a' && *pPos <='f')
					|| (*pPos >= 'A' && *pPos <= 'F')))
			{
			if (*pPos >= '0' && *pPos <= '9')
				dwInt = 16 * dwInt + (*pPos - '0');
			else if (*pPos >= 'A' && *pPos <= 'F')
				dwInt = 16 * dwInt + (10 + (*pPos - 'A'));
			else
				dwInt = 16 * dwInt + (10 + (*pPos - 'a'));

			pPos++;
			}

		iInt = (int)dwInt;
		}
	else
		{
		while (*pPos != '\0' && *pPos >= '0' && *pPos <= '9')
			{
			iInt = 10 * iInt + (*pPos - '0');
			pPos++;
			bFoundNumber = TRUE;

			if (bExpectSeparators && *pPos == ',')
				pPos++;
			}
		}

	//	Done?

	if (!bFoundNumber)
		{
		if (retbNullValue)
			*retbNullValue = true;

		if (retpEnd)
			*retpEnd = pPos;

		return iNullResult;
		}

	//	Done!

	if (bNegative)
		iInt = -iInt;

	if (retpEnd)
		*retpEnd = pPos;

	return iInt;
	}

int Kernel::strParseIntOfBase (const char *pStart, int iBase, int iNullResult, const char **retpEnd, bool *retbNullValue)

//	strParseIntOfBase
//
//	Parses an integer of the given base

	{
	const char *pPos;
	BOOL bNegative;
	BOOL bFoundNumber;
	int iInt;

	//	Preset

	if (retbNullValue)
		*retbNullValue = false;

	pPos = pStart;
	bNegative = FALSE;
	bFoundNumber = FALSE;
	iInt = 0;

	//	Skip whitespace

	while (*pPos == ' ' || *pPos == '\t' || *pPos == '\n' || *pPos == '\r')
		pPos++;

	//	If NULL, then we're done

	if (*pPos == '\0')
		{
		if (retbNullValue)
			*retbNullValue = true;

		if (retpEnd)
			*retpEnd = pPos;

		return iNullResult;
		}

	//	If negative, remember it

	if (*pPos == '-')
		{
		bNegative = TRUE;
		pPos++;
		}
	else if (*pPos == '+')
		pPos++;

	//	See if this is a hex number

	if (*pPos == '0')
		{
		pPos++;
		bFoundNumber = TRUE;

		//	If the next character is x (or X) then we've got
		//	a Hex number

		if (*pPos == 'x' || *pPos == 'X')
			{
			pPos++;
			iBase = 16;
			}
		}

	//	Now parse for numbers

	DWORD dwInt = 0;

	while (*pPos != '\0' 
			&& ((*pPos >= '0' && *pPos <= '9') 
				|| (*pPos >= 'a' && *pPos <='f')
				|| (*pPos >= 'A' && *pPos <= 'F')))
		{
		if (*pPos >= '0' && *pPos <= '9')
			dwInt = (DWORD)iBase * dwInt + (*pPos - '0');
		else if (*pPos >= 'A' && *pPos <= 'F')
			dwInt = (DWORD)iBase * dwInt + (10 + (*pPos - 'A'));
		else
			dwInt = (DWORD)iBase * dwInt + (10 + (*pPos - 'a'));

		pPos++;
		bFoundNumber = TRUE;
		}

	iInt = (int)dwInt;

	//	Done?

	if (!bFoundNumber)
		{
		if (retbNullValue)
			*retbNullValue = true;

		if (retpEnd)
			*retpEnd = pPos;

		return iNullResult;
		}

	//	Done!

	if (bNegative)
		iInt = -iInt;

	if (retpEnd)
		*retpEnd = pPos;

	return iInt;
	}

void Kernel::strParseWhitespace (const char *pPos, const char **retpPos)

//	strParseWhitespace
//
//	Skips whitespace

	{
	while (strIsWhitespace(pPos))
		pPos++;

	if (retpPos)
		*retpPos = pPos;
	}

CString Kernel::strProcess (const CString &sValue, DWORD dwFlags)

//	strProcess
//
//	Filters the string in various ways based on flags.

	{
	char *pSrc = sValue.GetASCIIZPointer();
	char *pSrcEnd = pSrc + sValue.GetLength();

	CString sResult;
	char *pDest = sResult.GetWritePointer(sValue.GetLength() * 2);
	char *pDestStart = pDest;
	char *pDestEnd = pDest + sResult.GetLength();

	while (pSrc < pSrcEnd && pDest < pDestEnd)
		{
		switch (*pSrc)
			{
			case '\"':
				if (dwFlags & STRPROC_NO_DOUBLE_QUOTES)
					*pDest++ = '\'';
				else if (dwFlags & STRPROC_ESCAPE_DOUBLE_QUOTES)
					{
					*pDest++ = '\\';
					*pDest++ = '\"';
					}
				else
					*pDest++ = *pSrc;
				pSrc++;
				break;

			default:
				*pDest++ = *pSrc++;
				break;
			}
		}

	//	Done

	sResult.Truncate((int)(pDest - pDestStart));
	return sResult;
	}

CString Kernel::strRepeat (const CString &sString, int iCount)
	{
	ASSERT(iCount >= 0);

	int iLen = sString.GetLength();
	int iFinalCount = iCount * iLen;
	if (iFinalCount == 0)
		return NULL_STR;

	CString sResult;
	char *pDest = sResult.GetWritePointer(iFinalCount);
	char *pSource = sString.GetASCIIZPointer();

	for (int i = 0; i < iFinalCount; i++)
		*pDest++ = pSource[i % iLen];

	return sResult;
	}

bool Kernel::strStartsWith (const CString &sString, const CString &sStringToFind)

//	strStartsWith
//
//	Returns TRUE if sString starts with sStringToFind

	{
	int iStrLen = sString.GetLength();
	int iTargetLen = sStringToFind.GetLength();
	if (iTargetLen > iStrLen)
		return false;

	char *pPos = sString.GetASCIIZPointer();
	char *pEndPos = pPos + iTargetLen;
	char *pTarget = sStringToFind.GetASCIIZPointer();

	while (pPos < pEndPos)
		{
		if (CharLower((LPTSTR)(BYTE)(*pPos++)) != CharLower((LPTSTR)(BYTE)(*pTarget++)))
			return false;
		}

	return true;
	}

CString Kernel::strTitleCapitalize (const CString &sString, const char **pExceptions, int iExceptionsCount)

//	strTitleCapitalize
//
//	Title capitalization

	{
	int i, j;

	//	Edge conditions

	if (sString.IsBlank())
		return NULL_STR;

	//	Get the exception list

	const char **pExceptionList = (pExceptions ? pExceptions : TITLE_CAP_EXCEPTIONS_SHORT);
	int iExceptionListCount = (iExceptionsCount ? iExceptionsCount : TITLE_CAP_EXCEPTIONS_SHORT_COUNT);

	//	Generate a new string with title capitalization

	CString sResult = sString;
	char *pDest = sResult.GetASCIIZPointer();
	char *pDestEnd = pDest + sResult.GetLength();
	char *pPos = sString.GetASCIIZPointer();

	//	First we split the string into words

	TArray<char *> Words;
	Words.Insert(pDest);
	char *pStart = NULL;

	while (pDest < pDestEnd)
		{
		if (*pDest == ' ')
			{
			while (*pDest == ' ')
				pDest++;

			if (pDest < pDestEnd)
				Words.Insert(pDest);
			}

		pDest++;
		}

	//	The first word is capitalized

	*Words[0] = (char)(DWORD)::CharUpper((LPSTR)*Words[0]);

	//	The last word is capitalized

	if (Words.GetCount() > 1)
		*Words[Words.GetCount() - 1] = (char)(DWORD)::CharUpper((LPSTR)*Words[Words.GetCount() - 1]);

	//	All the words in between are capitalized if they are not on the
	//	exception list.

	for (i = 1; i < Words.GetCount() - 1; i++)
		{
		CString sWord(Words[i], (Words[i + 1] - 1) - Words[i], true);

		bool bException = false;
		for (j = 0; j < iExceptionListCount; j++)
			if (strEquals(sWord, CString(pExceptionList[j], -1, true)))
				{
				bException = true;
				break;
				}

		if (!bException)
			*Words[i] = (char)(DWORD)::CharUpper((LPSTR)*Words[i]);
		}

	//	Don

	return sResult;
	}

CString RomanNumeralBase (int iNumber, const char *szTens, const char *szFives, const char *szSingles)
	{
	char szBuffer[10];
	char *pPos = szBuffer;

	switch (iNumber)
		{
		case 1:
		case 2:
		case 3:
			{
			for (int i = 0; i < iNumber; i++)
				*pPos++ = *szSingles;

			return CString(szBuffer, iNumber);
			}

		case 4:
			{
			*pPos++ = *szSingles;
			*pPos++ = *szFives;
			return CString(szBuffer, 2);
			}

		case 5:
		case 6:
		case 7:
		case 8:
			{
			*pPos++ = *szFives;

			for (int i = 0; i < iNumber - 5; i++)
				*pPos++ = *szSingles;

			return CString(szBuffer, 1 + iNumber - 5);
			}

		case 9:
			{
			*pPos++ = *szSingles;
			*pPos++ = *szTens;
			return CString(szBuffer, 2);
			}

		default:
			return NULL_STR;
		}
	}

CString Kernel::strRomanNumeral (int i)

//	strRomanNumeral
//
//	Returns the roman numeral

	{
	if (i < 0)
		return CONSTLIT("(negative)");
	else if (i == 0)
		return CONSTLIT("(zero)");
	else
		{
		CString sResult = RomanNumeralBase((i % 10000) / 1000, " ", " ", "M");
		sResult.Append(RomanNumeralBase((i % 1000) / 100, "M", "D", "C"));
		sResult.Append(RomanNumeralBase((i % 100) / 10, "C", "L", "X"));
		sResult.Append(RomanNumeralBase((i % 10), "X", "V", "I"));

		return sResult;
		}
	}

double Kernel::strToDouble (const CString &sString, double rFailResult, bool *retbFailed)

//	strToDouble
//
//	Converts a string to a double

	{
	//	Check to see if this is a hex integer

	char *pPos = sString.GetASCIIZPointer();
	if (sString.GetLength() > 2
			&& pPos[0] == '0'
			&& (pPos[1] == 'x' || pPos[1] == 'X'))
		{
		bool bFailed;
		DWORD dwValue = strToInt(sString, 0, &bFailed);
		if (retbFailed) *retbFailed = bFailed;
		return (bFailed ? rFailResult : (double)dwValue);
		}

	//	Assume a float

	double rResult = ::atof(sString.GetASCIIZPointer());
	if (_isnan(rResult))
		{
		if (retbFailed)
			*retbFailed = true;
		return rFailResult;
		}

	if (retbFailed)
		*retbFailed = false;

	return rResult;
	}

CString Kernel::strToFilename (const CString &sString)

//	strToFilename
//
//	Replaces any characters in the given string that are not valid in a filename.
//
//	NOTE: We do not guarantee a 1-1 mapping between string and valid
//	filename; nor do we guarantee reverse mapping.

	{
	CString sResult;
	char *pSrc = sString.GetASCIIZPointer();
	char *pDest = sResult.GetWritePointer(sString.GetLength());

	while (*pSrc != '\0')
		{
		//	Make sure we only have valid characters

		if ((*pSrc >= 'A' && *pSrc <= 'Z')
				|| (*pSrc >= 'a' && *pSrc <= 'z')
				|| (*pSrc >= '0' && *pSrc <= '9'))
			*pDest = *pSrc;
		else if (*pSrc == ' ')
			*pDest = '_';
		else
			*pDest = '+';

		pSrc++;
		pDest++;
		}

	return sResult;
	}

int Kernel::strToInt (const CString &sString, int iFailResult, bool *retbFailed)

//	CStringToInt
//
//	Converts a string to an integer

	{
	return strParseInt(sString.GetASCIIZPointer(), iFailResult, NULL, retbFailed);
	}

CString Kernel::strToLower (const CString &sString)

//	strToLower
//
//	Converts string to all lower-case

	{
	CString sResult = sString;
	::CharLowerBuff(sResult.GetWritePointer(sResult.GetLength()), sResult.GetLength());
	return sResult;
	}

CString Kernel::strToUpper (const CString &sString)

//	strToUpper
//
//	Converts string to all upper-case

	{
	CString sUpper = sString;
	::CharUpperBuff(sUpper.GetWritePointer(sUpper.GetLength()), sUpper.GetLength());
	return sUpper;
	}

CString Kernel::strToXMLText (const CString &sText, bool bInBody)

//	strToXMLText
//
//	Returns a string that has been stripped of all characters
//	that cannot appear in an XML attribute or body

	{
	CString sResult;
	int iExtra = 0;

	//	Optimistically assume that the text has no bad
	//	characters, while also keeping track of the amount of space
	//	that we would need for escape codes.

	char *pDest = sResult.GetWritePointer(sText.GetLength());
	char *pPos = sText.GetASCIIZPointer();
	char *pPosEnd = pPos + sText.GetLength();
	while (pPos < pPosEnd)
		{
		switch (*pPos)
			{
			case '&':
				iExtra += 4;		//	&amp
				break;

			case '<':
				iExtra += 3;		//	&lt
				break;

			case '>':
				iExtra += 3;		//	&gt
				break;

			case '\"':
				iExtra += 5;		//	&quot
				break;

			case '\'':
				iExtra += 5;		//	apos
				break;

			default:
				if (bInBody
						&& (*pPos == '\n' || *pPos == '\r' || *pPos == '\t'))
					{ }
				else if (*pPos < ' ' || *pPos > '~')
					iExtra += 5;
				break;
			}

		*pDest++ = *pPos++;
		}

	//	Done?

	if (iExtra == 0)
		{
		*pDest = '\0';
		return sResult;
		}

	//	Need to escape

	pDest = sResult.GetWritePointer(sText.GetLength() + iExtra);
	pPos = sText.GetASCIIZPointer();
	pPosEnd = pPos + sText.GetLength();
	while (pPos < pPosEnd)
		{
		switch (*pPos)
			{
			case '&':
				*pDest++ = '&';
				*pDest++ = 'a';
				*pDest++ = 'm';
				*pDest++ = 'p';
				*pDest++ = ';';
				break;

			case '<':
				*pDest++ = '&';
				*pDest++ = 'l';
				*pDest++ = 't';
				*pDest++ = ';';
				break;

			case '>':
				*pDest++ = '&';
				*pDest++ = 'g';
				*pDest++ = 't';
				*pDest++ = ';';
				break;

			case '\"':
				*pDest++ = '&';
				*pDest++ = 'q';
				*pDest++ = 'u';
				*pDest++ = 'o';
				*pDest++ = 't';
				*pDest++ = ';';
				break;

			case '\'':
				*pDest++ = '&';
				*pDest++ = 'a';
				*pDest++ = 'p';
				*pDest++ = 'o';
				*pDest++ = 's';
				*pDest++ = ';';
				break;

			default:
				if (*pPos >= ' ' && *pPos <= '~')
					*pDest++ = *pPos;
				else if (bInBody
						&& (*pPos == '\n' || *pPos == '\r' || *pPos == '\t'))
					*pDest++ = *pPos;
				else
					{
					CString sEscape = strPatternSubst(CONSTLIT("%02x"), (DWORD)(BYTE)*pPos);
					char *pEscape = sEscape.GetASCIIZPointer();
					*pDest++ = '&';
					*pDest++ = '#';
					*pDest++ = 'x';
					*pDest++ = pEscape[0];
					*pDest++ = pEscape[1];
					*pDest++ = ';';
					}
				break;
			}

		pPos++;
		}

	//	Done!

	*pDest = '\0';
	return sResult;
	}

CString Kernel::strTrimWhitespace (const CString &sString, bool bLeading, bool bTrailing)

//	strTrimWhitespace
//
//	Removes leading and trailing whitespace

	{
	const char *pPos = sString.GetASCIIZPointer();
	const char *pStart;
	const char *pEnd;

	if (bLeading)
		strParseWhitespace(pPos, &pStart);
	else
		pStart = pPos;

	pEnd = pPos + sString.GetLength();

	if (bTrailing)
		{
		while (pEnd > pStart && strIsWhitespace(pEnd-1))
			pEnd--;
		}

	return CString(pStart, pEnd - pStart);
	}

CString Kernel::strSubString (const CString &sString, int iOffset, int iLength)

//	strSubString
//
//	Returns a substring of the given string

	{
	if (iOffset >= sString.GetLength())
		return LITERAL("");
	else
		{
		if (iLength == -1)
			iLength = sString.GetLength() - iOffset;
		else
			iLength = Min(iLength, sString.GetLength() - iOffset);

		CString sSub(sString.GetPointer() + iOffset, iLength);
		return sSub;
		}
	}

CString Kernel::strWord (const CString &sString, int iWordPos)

//	strWord
//
//	Returns the nth word (0-based) in the string. Words are
//	contiguous alphanumeric strings

	{
	enum States { stateNone, stateInWord };

	//	Find the beginning of the word

	int iState = stateNone;
	char *pStart = NULL;
	char *pPos = sString.GetASCIIZPointer();
	while (*pPos != '\0' && (iWordPos > 0 || iState != stateInWord))
		{
		if (iState == stateInWord)
			{
			if (!strIsAlphaNumeric(pPos))
				{
				iWordPos--;
				iState = stateNone;
				}

			pPos++;
			}
		else
			{
			if (strIsAlphaNumeric(pPos))
				iState = stateInWord;
			else
				pPos++;
			}
		}

	if (*pPos == '\0')
		return NULL_STR;

	//	Look for the end of the word

	pStart = pPos;
	while (*pPos != '\0' && strIsAlphaNumeric(pPos))
		pPos++;

	return CString(pStart, pPos - pStart);
	}

void *CString::INTCopyStorage (void *pvStore)
	{
	PSTORESTRUCT pStore = (PSTORESTRUCT)pvStore;

	if (pStore)
		pStore->iRefCount++;

	return pStore;
	}

void *CString::INTGetStorage (const CString &sString)
	{
	if (sString.m_pStore)
		sString.m_pStore->iRefCount++;

	return sString.m_pStore;
	}

void CString::INTFreeStorage (void *pvStore)
	{
	PSTORESTRUCT pStore = (PSTORESTRUCT)pvStore;

	if (pStore && (--pStore->iRefCount) == 0)
		FreeStore(pStore);
	}

CString CString::INTMakeString (void *pvStore)
	{
	return CString(pvStore, true);
	}

void CString::INTSetStorage (CString &sString, void *pvStore)
	{
	sString.DecRefCount();

	sString.m_pStore = (PSTORESTRUCT)pvStore;

	//	If we've got a storage, bump up the ref count

	if (sString.m_pStore)
		sString.m_pStore->iRefCount++;
	}

void CString::INTTakeStorage (void *pStore)
	{
	DecRefCount();
	m_pStore = (PSTORESTRUCT)pStore;
	}

#ifdef DEBUG_STRING_LEAKS

int CString::DebugGetStringCount (void)
	{
	return g_iStoreCount;
	}

void CString::DebugMark (void)
	{
	//	Iterate over the entire store and mark all currently
	//	allocated strings

	for (int i = 0; i < g_iStoreSize; i++)
		if (g_pStore[i].iRefCount > 0)
			g_pStore[i].iMark++;
	}

void CString::DebugOutputLeakedStrings (void)
	{
	//	All strings that have a mark == 0 are leaked

	int iLeaked = 0;
	for (int i = 0; i < g_iStoreSize; i++)
		if (g_pStore[i].iRefCount > 0)
			{
			if (g_pStore[i].iMark == 0)
				{
				char szBuffer[1024];
				wsprintf(szBuffer, "'%s'\n", (g_pStore[i].pString ? g_pStore[i].pString : "(null)"));
				::OutputDebugString(szBuffer);
				iLeaked++;
				}
			else
				g_pStore[i].iMark--;
			}

	if (iLeaked > 0)
		{
		char szBuffer[1024];
		wsprintf(szBuffer, "Leaked %d strings!\n", iLeaked);
		::OutputDebugString(szBuffer);
		}
	}

#endif

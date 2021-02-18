//	Kernel.h
//
//	Kernel definitions.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

#define _CRT_RAND_S
#include <cstddef>
#include <functional>

#ifndef _WINDOWS_

//	Support Windows 7 and above

#ifndef _WIN32_WINNT
#define _WIN32_WINNT	0x0601 
#define WINVER			0x0601
#endif						

#define NOMINMAX
#include <windows.h>

#endif

#include <mmsystem.h>

//	For some reason, <kernelspecs.h> defines HIGH_LEVEL, which ends up 
//	conflicting with a lot of other definitions.

#ifdef HIGH_LEVEL
#undef HIGH_LEVEL
#endif

//	Debugging defines

#ifdef DEBUG
//#define DEBUG_MEMORY_LEAKS
//#define DEBUG_STRING_LEAKS
//#define DEBUG_ARRAY_STATS
#endif

//	Undefine some common names

#undef DrawText
#undef PlaySound

//	Explicit placement operator
struct placement_new_class { };
extern placement_new_class placement_new;
inline void *operator new (size_t, ::placement_new_class, void *p) { return p; }

#ifdef DEBUG_MEMORY_LEAKS
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

//	HACK: Declare _alloca so that we don't have to include malloc.h
extern "C" void *          __cdecl _alloca(size_t);

namespace Kernel {

//	Define ASSERT macro, if necessary

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(exp)						\
			{							\
			if (!(exp))					\
				DebugBreak();			\
			}
#else
#define ASSERT(exp)
#endif
#endif

//	Call stack logging

#define DEBUG_TRY					try {
#define DEBUG_CATCH					} catch (...) { kernelDebugLogPattern("Crash in %s", CString(__FUNCTION__)); throw; }
#define DEBUG_CATCH_CONTINUE		} catch (...) { kernelDebugLogPattern("Crash in %s", CString(__FUNCTION__)); }
#define DEBUG_CATCH_MSG(msg)		} catch (...) { kernelDebugLogPattern((msg)); throw; }
#define DEBUG_CATCH_MSG1(msg,p1)	} catch (...) { kernelDebugLogPattern((msg),(p1)); throw; }

#define INLINE_DECREF				TRUE

//	Error definitions

typedef DWORD ALERROR;

#ifndef NOERROR
#define NOERROR									0
#endif

#define ERR_FAIL								1	//	Generic failure
#define ERR_MEMORY								2	//	Out of memory
#define ERR_ENDOFFILE							3	//	Read past end of file
#define ERR_CANCEL								4	//	User canceled operation
#define ERR_NOTFOUND							5	//	Entry not found
#define ERR_FILEOPEN							6	//	Unable to open file
#define ERR_CLASSNOTFOUND						7	//	Constructor for class not found
#define ERR_OUTOFDATE							8	//	Not latest version
#define ERR_MORE								9	//	More needed
#define ERR_WIN32_EXCEPTION						10	//	Win32 exception
#define ERR_OUTOFROOM							11	//	Unable to insert
#define ERR_FILE_IN_USE							12	//	File is in use

#define ERR_MODULE						0x00010000	//	First module error message (see GlobalErr.h)
#define ERR_APPL						0x01000000	//	First application error message
#define ERR_FLAG_DISPLAYED				0x80000000	//	Error message already displayed

inline BOOL ErrorWasDisplayed (ALERROR error) { return (error & ERR_FLAG_DISPLAYED) ? TRUE : FALSE; }
inline ALERROR ErrorSetDisplayed (ALERROR error) { return error | ERR_FLAG_DISPLAYED; }
inline ALERROR ErrorCode (ALERROR error) { return error & ~ERR_FLAG_DISPLAYED; }

//	Miscellaneous macros

inline int Absolute (int iValue) { return (iValue < 0 ? -iValue : iValue); }
inline double Absolute (double rValue) { return (rValue < 0.0 ? -rValue : rValue); }
inline int AlignDown (int iValue, int iGranularity) { return (iValue / iGranularity) * iGranularity; }
inline int AlignDownSigned (int iValue, int iGranularity) { return ((iValue + (iValue < 0 ? (1 - iGranularity) : 0)) / iGranularity) * iGranularity; }
inline int AlignUp (int iValue, int iGranularity) { return ((iValue + (iGranularity - 1)) / iGranularity) * iGranularity; }
inline int AlignUpSigned (int iValue, int iGranularity) { return ((iValue + (iValue < 0 ? 0 : (iGranularity - 1))) / iGranularity) * iGranularity; }
inline int ClockMod (int iValue, int iDivisor) { int iResult = (iValue % iDivisor); return (iResult < 0 ? iResult + iDivisor : iResult); }
inline int ClockDiff (int iValue, int iOrigin, int iDivisor)
	{
	int iDiff = ClockMod(iValue - iOrigin, iDivisor);
	int iHalfDiv = iDivisor / 2;
	if (iDiff <= iHalfDiv)
		return iDiff;
	else
		return iDiff - iDivisor;
	}
inline BOOL IsShiftDown (void) { return (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? TRUE : FALSE; }
inline BOOL IsControlDown (void) { return (GetAsyncKeyState(VK_CONTROL) & 0x8000) ? TRUE : FALSE; }
inline int Sign (int iValue) { return (iValue == 0 ? 0 : (iValue > 0 ? 1 : -1)); }
template <class VALUE> VALUE Clamp (VALUE x, VALUE a, VALUE b)
	{
	return (x < a ? a : (x > b ? b : x));
	}
template <class VALUE> VALUE Max (VALUE a, VALUE b)
	{
	return (a > b ? a : b);
	}
template <class VALUE> VALUE Min (VALUE a, VALUE b)
	{
	return (a < b ? a : b);
	}
template <class VALUE> void Swap (VALUE &a, VALUE &b)
	{
	VALUE temp = a;
	a = b;
	b = temp;
	}

#ifdef NOMINMAX
template <class VALUE> VALUE max (VALUE a, VALUE b)
	{
	return (a > b ? a : b);
	}
template <class VALUE> VALUE min (VALUE a, VALUE b)
	{
	return (a < b ? a : b);
	}

inline int max (int a, LONG b) { return (a > b ? a : b); }
inline int max (LONG a, int b) { return (a > b ? a : b); }
inline int max (int a, size_t b) { return (a > (int)b ? a : b); }
inline int max (size_t a, int b) { return ((int)a > b ? a : b); }
inline int min (int a, LONG b) { return (a < b ? a : b); }
inline int min (LONG a, int b) { return (a < b ? a : b); }
inline int min (int a, size_t b) { return (a < (int)b ? a : b); }
inline int min (size_t a, int b) { return ((int)a < b ? a : b); }
#endif

inline int RectHeight(RECT *pRect) { return pRect->bottom - pRect->top; }
inline int RectHeight(const RECT &Rect) { return Rect.bottom - Rect.top; }
inline int RectWidth(RECT *pRect) { return pRect->right - pRect->left; }
inline int RectWidth(const RECT &Rect) { return Rect.right - Rect.left; }
inline void RectCenter (const RECT &rcRect, int *retx, int *rety) { *retx = rcRect.left + (RectWidth(rcRect) / 2); *rety = rcRect.top + (RectHeight(rcRect) / 2); }
inline bool RectsIntersect(const RECT &R1, const RECT &R2)
	{
	return (R1.right >= R2.left)
			&& (R1.left < R2.right)
			&& (R1.bottom >= R2.top)
			&& (R1.top < R2.bottom);
	}
inline bool RectEncloses (RECT *pR1, RECT *pR2)
	{
	return (pR1->left <= pR2->left)
			&& (pR1->right >= pR2->right)
			&& (pR1->top <= pR2->top)
			&& (pR1->bottom >= pR2->bottom);
	}
inline bool RectEquals (const RECT &rc1, const RECT &rc2)
	{
	return (rc1.left == rc2.left
			&& rc1.top == rc2.top
			&& rc1.right == rc2.right
			&& rc1.bottom == rc2.bottom);
	}
inline void RectInit (RECT *pRect)
	{
	pRect->left = 0;
	pRect->top = 0;
	pRect->right = 0;
	pRect->bottom = 0;
	}

const int MAX_SHORT =					32767;

//	API flags

#define API_FLAG_MASKBLT				0x00000001	//	MaskBlt is available
#define API_FLAG_WINNT					0x00000002	//	Running on Windows NT
#define API_FLAG_DWM					0x00000004	//	Desktop Window Manager running (Vista or Win7)

//	Forward class definitions

class CArchiver;
class CObject;
class CUnarchiver;
class CIDTable;
class IReadStream;
class IWriteStream;

//	Templates and Structures

#include "KernelString.h"
#include "KernelExceptions.h"

#include "TSmartPtr.h"
#include "TArray.h"
#include "TLinkedList.h"
#include "TMap.h"
#include "TQueue.h"
#include "TStack.h"

//	TimeDate classes

#define SECONDS_PER_DAY					(60 * 60 * 24)

class CTimeDate
	{
	public:
		enum Constants
			{
			Now,
			Today,
			};

		CTimeDate (void);
		CTimeDate (Constants Init);
		CTimeDate (const SYSTEMTIME &Time);
		CTimeDate (int iDaysSince1AD, int iMillisecondsSinceMidnight);

		operator SYSTEMTIME () const { return m_Time; }

		int Year (void) const { return m_Time.wYear; }
		int Month (void) const { return m_Time.wMonth; }
		int Day (void) const { return m_Time.wDay; }
		int Hour (void) const { return m_Time.wHour; }
		int Minute (void) const { return m_Time.wMinute; }
		int Second (void) const { return m_Time.wSecond; }
		int Millisecond (void) const { return m_Time.wMilliseconds; }

		int Compare (const CTimeDate &Src) const;
		int DayOfWeek (void) const;
		int DaysSince1AD (void) const;
		CString Format (const CString &sFormat) const;
		int MillisecondsSinceMidnight (void) const;
		bool Parse (const CString &sFormat, const CString &sValue, CString *retsError = NULL);
		CTimeDate ToLocalTime (void) const;

	private:
		SYSTEMTIME m_Time;
	};

class CTimeSpan
	{
	public:
		CTimeSpan (void);
		CTimeSpan (int iMilliseconds);
		CTimeSpan (int iDays, int iMilliseconds);

		static bool Parse (const CString &sValue, CTimeSpan *retValue);

		int Days (void) const { return (int)m_Days; }
		int Seconds (void) const { return (SECONDS_PER_DAY * m_Days) + (m_Milliseconds / 1000); }
		int Milliseconds (void) const { return (SECONDS_PER_DAY * 1000 * m_Days) + m_Milliseconds; }
		int MillisecondsSinceMidnight (void) const { return (int)m_Milliseconds; }

		CString Encode (void) const;
		CString Format (const CString &sFormat) const;
		bool IsBlank (void) const { return (m_Days == 0 && m_Milliseconds == 0); }
		void ReadFromStream (IReadStream *pStream);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		static bool ParsePartial (const char *pPos, DWORD *retdwDays, DWORD *retdwMilliseconds, const char **retpPos);

		DWORD m_Days;
		DWORD m_Milliseconds;
	};

const CTimeSpan operator+ (const CTimeSpan &op1, const CTimeSpan &op2);
const CTimeSpan operator- (const CTimeSpan &op1, const CTimeSpan &op2);

CTimeDate timeAddTime (const CTimeDate &StartTime, const CTimeSpan &Addition);
CTimeSpan timeSpan (const CTimeDate &StartTime, const CTimeDate &EndTime);
CTimeDate timeSubtractTime (const CTimeDate &StartTime, const CTimeSpan &Subtraction);
bool timeIsLeapYear (int iYear);

//	High-performance timer classes --------------------------------------------

class CPeriodicWaiter
	{
	public:
		CPeriodicWaiter (DWORD dwPeriod);

		void Wait (void);

	private:
		DWORD m_dwPeriod;
		LONGLONG m_LastCounter;

		LONGLONG m_PCFreq;
		LONGLONG m_PCCountsPerPeriod;
	};

//	Object class ID definitions

typedef DWORD OBJCLASSID;

#define OBJCLASS_MODULE_MASK					0xFFF00000
#define OBJCLASS_MODULE_SHIFT					20

#define OBJCLASS_MODULE_KERNEL					0
#define OBJCLASS_MODULE_APPLICATION				1
#define OBJCLASS_MODULE_GLOBAL					2
#define OBJCLASS_MODULE_COUNT					3

inline constexpr OBJCLASSID MakeOBJCLASSIDExt (int iModule, int iID) { return (((DWORD)iModule) << OBJCLASS_MODULE_SHIFT) + (DWORD)iID; }
inline constexpr OBJCLASSID MakeOBJCLASSID (int iID) { return MakeOBJCLASSIDExt(OBJCLASS_MODULE_APPLICATION, iID); }
inline constexpr OBJCLASSID MakeGlobalOBJCLASSID (int iID) { return MakeOBJCLASSIDExt(OBJCLASS_MODULE_GLOBAL, iID); }
inline constexpr int OBJCLASSIDGetID (OBJCLASSID ObjID) { return (int)(ObjID & ~OBJCLASS_MODULE_MASK); }
inline constexpr int OBJCLASSIDGetModule (OBJCLASSID ObjID) { return (int)((ObjID & OBJCLASS_MODULE_MASK) >> OBJCLASS_MODULE_SHIFT); }

//	Object data description

#define DATADESC_OPCODE_STOP				0	//	No more entries
#define DATADESC_OPCODE_INT					1	//	32-bit integer (iCount valid)
#define DATADESC_OPCODE_REFERENCE			2	//	Reference to memory location or object (iCount valid)
#define DATADESC_OPCODE_ALLOC_OBJ			3	//	Pointer to owned object (derived from CObject)
#define DATADESC_OPCODE_EMBED_OBJ			4	//	Embedded object (derived from CObject)
#define DATADESC_OPCODE_ZERO				5	//	32-bit of zero-init data (iCount valid)
#define DATADESC_OPCODE_VTABLE				6	//	This is a vtable (which is initialized by new) (iCount is valid)
#define DATADESC_OPCODE_ALLOC_SIZE32		7	//	Number of 32-bit words allocated in the following memory block
#define DATADESC_OPCODE_ALLOC_MEMORY		8	//	Block of memory; previous must be ALLOC_SIZE

#define DATADESC_FLAG_CUSTOM		0x00000001	//	Object handles saving this part

typedef struct
	{
	int iOpCode:8;								//	Op-code
	int iCount:8;								//	Count
	DWORD dwFlags:16;							//	Miscellaneous flags
	} DATADESCSTRUCT, *PDATADESCSTRUCT;

//	Abstract object class

class IObjectClass
	{
	public:
		IObjectClass (OBJCLASSID ObjID, PDATADESCSTRUCT pDataDesc) : m_ObjID(ObjID), m_pDataDesc(pDataDesc) { }

		PDATADESCSTRUCT GetDataDesc (void) { return m_pDataDesc; }
		OBJCLASSID GetObjID (void) { return m_ObjID; }
		virtual CObject *Instantiate (void) = 0;
		virtual int GetObjSize (void) = 0;

	private:
		OBJCLASSID m_ObjID;
		PDATADESCSTRUCT m_pDataDesc;
	};

//	Base object class

class CObject
	{
	public:
		CObject (IObjectClass *pClass);
		virtual ~CObject (void);

		CObject *Copy (void);
		IObjectClass *GetClass (void) { return m_pClass; }
		static bool IsValidPointer (CObject *pObj);
		ALERROR Load (CUnarchiver *pUnarchiver);
		ALERROR LoadDone (void);
		ALERROR Save (CArchiver *pArchiver);

		static ALERROR Flatten (CObject *pObject, CString *retsData);
		static ALERROR Unflatten (CString sData, CObject **retpObject);

	protected:
		virtual void CopyHandler (CObject *pOriginal) { }
		virtual ALERROR LoadCustom (CUnarchiver *pUnarchiver, BYTE *pDest) { return NOERROR; }
		virtual ALERROR LoadDoneHandler (void) { return NOERROR; }
		virtual ALERROR LoadHandler (CUnarchiver *pUnarchiver);
		virtual LPVOID MemAlloc (int iSize) { return (BYTE *)HeapAlloc(GetProcessHeap(), 0, iSize); }
		virtual void MemFree (LPVOID pMem) { HeapFree(GetProcessHeap(), 0, pMem); }
		virtual ALERROR SaveCustom (CArchiver *pArchiver, BYTE *pSource) { return NOERROR; }
		virtual ALERROR SaveHandler (CArchiver *pArchiver);

	private:
		CObject *Clone (void) { return m_pClass->Instantiate(); }
		BOOL CopyData (PDATADESCSTRUCT pPos, BYTE **iopSource, BYTE **iopDest);
		PDATADESCSTRUCT DataDescNext (PDATADESCSTRUCT pPos);
		PDATADESCSTRUCT DataDescStart (void);
		BYTE *DataStart (void);
		void VerifyDataDesc (void);

		IObjectClass *m_pClass;
	};

//	Factory for creating objects

class CObjectClassFactory
	{
	public:
		static CObject *Create (OBJCLASSID ObjID);
		static IObjectClass *GetClass (OBJCLASSID ObjID);
		static void NewClass (IObjectClass *pClass);
	};

//	Template for object classes

template <class T>
class CObjectClass : public IObjectClass
	{
	public:
		CObjectClass (OBJCLASSID ObjID, PDATADESCSTRUCT pDataDesc = NULL)
				: IObjectClass(ObjID, pDataDesc)
				{ CObjectClassFactory::NewClass(this); }
 
		virtual CObject *Instantiate (void) { return new T; }
		virtual int GetObjSize (void) { return sizeof(T); }
	};

//	Synchronization -----------------------------------------------------------

class CCriticalSection
	{
	public:
		CCriticalSection (void) { ::InitializeCriticalSection(&m_cs); }
		~CCriticalSection (void) { ::DeleteCriticalSection(&m_cs); }

		void Lock (void) { ::EnterCriticalSection(&m_cs); }
		void Unlock (void) { ::LeaveCriticalSection(&m_cs); }

	private:
		CRITICAL_SECTION m_cs;
	};

class CSmartLock
	{
	public:
		CSmartLock(CCriticalSection &cs) : m_cs(cs) { m_cs.Lock(); }
		~CSmartLock (void) { m_cs.Unlock(); }

	private:
		CCriticalSection &m_cs;
	};

class COSObject
	{
	public:
		COSObject (void) : m_hHandle(INVALID_HANDLE_VALUE) { }
		~COSObject (void) { if (m_hHandle != INVALID_HANDLE_VALUE) ::CloseHandle(m_hHandle); }

		void Close (void) { if (m_hHandle != INVALID_HANDLE_VALUE) { ::CloseHandle(m_hHandle); m_hHandle = INVALID_HANDLE_VALUE; } }
		HANDLE GetWaitObject (void) const { return m_hHandle; }
		void TakeHandoff (COSObject &Obj) { Close(); m_hHandle = Obj.m_hHandle; Obj.m_hHandle = INVALID_HANDLE_VALUE; }
		bool Wait (DWORD dwTimeout = INFINITE) const { return (::WaitForSingleObject(m_hHandle, dwTimeout) != WAIT_TIMEOUT); }

	protected:
		HANDLE m_hHandle;
	};

class CManualEvent : public COSObject
	{
	public:
		void Create (void);
		void Create (const CString &sName, bool *retbExists = NULL);
		bool IsSet (void) { return (::WaitForSingleObject(m_hHandle, 0) == WAIT_OBJECT_0); }
		void Reset (void) { ::ResetEvent(m_hHandle); }
		void Set (void) { ::SetEvent(m_hHandle); }
	};

//	CINTDynamicArray. Implementation of a dynamic array.
//	(NOTE: To save space, this class does not have a virtual
//	destructor. Do not sub-class this class without taking that into account).

class CINTDynamicArray
	{
	public:
		CINTDynamicArray (void);
		CINTDynamicArray (HANDLE hHeap);
		~CINTDynamicArray (void);

		ALERROR Append (BYTE *pData, int iLength, int iAllocQuantum)
			{ return Insert(-1, pData, iLength, iAllocQuantum); }
		ALERROR Delete (int iOffset, int iLength);
		ALERROR DeleteAll (void) { return Delete(0, m_iLength); }
		int GetLength (void) const { return m_iLength; }
		void SetLength (int iLength) { m_iLength = iLength; }
		BYTE *GetPointer (int iOffset) const { return (m_pArray ? m_pArray + iOffset : NULL); }
		ALERROR Insert (int iOffset, BYTE *pData, int iLength, int iAllocQuantum);
		ALERROR Resize (int iNewSize, BOOL bPreserve, int iAllocQuantum);

	private:
		int m_iLength;							//	Length of the array in bytes
		int m_iAllocSize;						//	Allocated size of the array
		HANDLE m_hHeap;							//	Heap to use
		BYTE *m_pArray;							//	Array data
	};

//	CIntArray. Implementation of a dynamic array of integers

class CIntArray : public CObject
	{
	public:
		CIntArray (void);
		virtual ~CIntArray (void);

		CIntArray &operator= (const CIntArray &Obj);

		ALERROR AppendElement (int iElement, int *retiIndex = NULL);
		ALERROR CollapseArray (int iPos, int iCount) { return RemoveRange(iPos, iPos + iCount - 1); }
		ALERROR ExpandArray (int iPos, int iCount);
		int FindElement (int iElement) const;
		int GetCount (void) const;
		int GetElement (int iIndex) const;
		ALERROR InsertElement (int iElement, int iPos, int *retiIndex);
		ALERROR InsertRange (CIntArray *pList, int iStart, int iEnd, int iPos);
		ALERROR MoveRange (int iStart, int iEnd, int iPos);
		ALERROR Set (int iCount, int *pData);
		ALERROR RemoveAll (void);
		ALERROR RemoveElement (int iPos) { return RemoveRange(iPos, iPos); }
		ALERROR RemoveRange (int iStart, int iEnd);
		void ReplaceElement (int iPos, int iElement);
		void Shuffle (void);

	private:
		int m_iAllocSize;					//	Number of integers allocated
		int *m_pData;						//	Pointer to integer array
		int m_iLength;						//	Number of integers used
	};

//	CString. Implementation of a standard string class

static constexpr char CHAR_LEFT_DOUBLE_QUOTE =		'\x93';
static constexpr char CHAR_RIGHT_DOUBLE_QUOTE =		'\x94';

static constexpr SConstString CSTR_LEFT_DOUBLE_QUOTE =	CONSTDEFS("\x93");
static constexpr SConstString CSTR_RIGHT_DOUBLE_QUOTE =	CONSTDEFS("\x94");
static constexpr SConstString CSTR_BULLET =				CONSTDEFS("\x95");
static constexpr SConstString CSTR_MDASH =				CONSTDEFS("\x97");
static constexpr SConstString CSTR_TRADEMARK =			CONSTDEFS("\x99");
static constexpr SConstString CSTR_COPYRIGHT =			CONSTDEFS("\xA9");
static constexpr SConstString CSTR_REGISTERED =			CONSTDEFS("\xAE");
static constexpr SConstString CSTR_DEGREE =				CONSTDEFS("\xB0");
static constexpr SConstString CSTR_PLUS_MINUS =			CONSTDEFS("\xB1");

static constexpr SConstString CSTR_CAP_AACUTE =			CONSTDEFS("\xC1");
static constexpr SConstString CSTR_CAP_EACUTE =			CONSTDEFS("\xC9");
static constexpr SConstString CSTR_CAP_IACUTE =			CONSTDEFS("\xCD");
static constexpr SConstString CSTR_CAP_NTILDE =			CONSTDEFS("\xD1");
static constexpr SConstString CSTR_CAP_OACUTE =			CONSTDEFS("\xD3");
static constexpr SConstString CSTR_TIMES =				CONSTDEFS("\xD7");
static constexpr SConstString CSTR_CAP_UACUTE =			CONSTDEFS("\xDA");
static constexpr SConstString CSTR_CAP_UUML =			CONSTDEFS("\xDC");
static constexpr SConstString CSTR_AACUTE =				CONSTDEFS("\xE1");
static constexpr SConstString CSTR_EACUTE =				CONSTDEFS("\xE9");
static constexpr SConstString CSTR_IACUTE =				CONSTDEFS("\xED");
static constexpr SConstString CSTR_NTILDE =				CONSTDEFS("\xF1");
static constexpr SConstString CSTR_OACUTE =				CONSTDEFS("\xF3");
static constexpr SConstString CSTR_UACUTE =				CONSTDEFS("\xFA");
static constexpr SConstString CSTR_UUML =				CONSTDEFS("\xFC");

static constexpr SConstString CSTR_WINGDING_LEFT_ARROW =	CONSTDEFS("\xE7");
static constexpr SConstString CSTR_WINGDING_RIGHT_ARROW =	CONSTDEFS("\xE8");

//	CDictionary. Implementation of a dynamic array of entries

class CDictionary : public CObject
	{
	public:
		CDictionary (void);
		CDictionary (IObjectClass *pClass);
		virtual ~CDictionary (void);

		ALERROR AddEntry (int iKey, int iValue);
		ALERROR Find (int iKey, int *retiValue) const;
		ALERROR FindEx (int iKey, int *retiEntry) const;
		ALERROR FindOrAdd (int iKey, int iValue, bool *retbFound, int *retiValue);
		int GetCount (void) const { return m_Array.GetCount() / 2; }
		void GetEntry (int iEntry, int *retiKey, int *retiValue) const;
		ALERROR ReplaceEntry (int iKey, int iValue, bool bAdd, bool *retbAdded, int *retiOldValue);
		ALERROR RemoveAll (void) { return m_Array.RemoveAll(); }
		ALERROR RemoveEntryByOrdinal (int iEntry, int *retiOldValue = NULL);
		ALERROR RemoveEntry (int iKey, int *retiOldValue);

	protected:
		virtual int Compare (int iKey1, int iKey2) const;
		ALERROR ExpandArray (int iPos, int iCount) { return m_Array.ExpandArray(2 * iPos, 2 * iCount); }
		void SetEntry (int iEntry, int iKey, int iValue);

		bool FindSlot (int iKey, int *retiPos) const;

		CIntArray m_Array;
	};

//	CIDTable. Implementation of a table that matches IDs with objects

class CIDTable : public CDictionary
	{
	public:
		CIDTable (void);
		CIDTable (BOOL bOwned, BOOL bNoReference);
		virtual ~CIDTable (void);

		ALERROR AddEntry (int iKey, CObject *pValue) { return CDictionary::AddEntry(iKey, (int)pValue); }
		int GetKey (int iEntry) const;
		CObject *GetValue (int iEntry) const;
		ALERROR Lookup (int iKey, CObject **retpValue) const;
		ALERROR LookupEx (int iKey, int *retiEntry) const;
		ALERROR RemoveAll (void);
		ALERROR RemoveEntry (int iKey, CObject **retpOldValue);
		ALERROR ReplaceEntry (int iKey, CObject *pValue, bool bAdd, CObject **retpOldValue);
		void SetValue (int iEntry, CObject *pValue, CObject **retpOldValue);

	protected:
		virtual int Compare (int iKey1, int iKey2) const;
		virtual void CopyHandler (CObject *pOriginal);
		virtual ALERROR LoadHandler (CUnarchiver *pUnarchiver);
		virtual ALERROR SaveHandler (CArchiver *pArchiver);

	private:
		BOOL m_bOwned;
		BOOL m_bNoReference;
	};

//	CSymbolTable. Implementation of a symbol table

class CSymbolTable : public CDictionary
	{
	public:
		CSymbolTable (void);
		CSymbolTable (BOOL bOwned, BOOL bNoReference);
		virtual ~CSymbolTable (void);
		CSymbolTable &operator= (const CSymbolTable &Obj);

		ALERROR AddEntry (const CString &sKey, CObject *pValue);
		CString GetKey (int iEntry) const;
		CObject *GetValue (int iEntry) const;
		ALERROR Lookup (const CString &sKey, CObject **retpValue = NULL) const;
		ALERROR LookupEx (const CString &sKey, int *retiEntry) const;
		ALERROR RemoveAll (void);
		ALERROR RemoveEntry (int iEntry, CObject **retpOldValue = NULL);
		ALERROR RemoveEntry (const CString &sKey, CObject **retpOldValue);
		ALERROR ReplaceEntry (const CString &sKey, CObject *pValue, bool bAdd, CObject **retpOldValue);
		void SetValue (int iEntry, CObject *pValue, CObject **retpOldValue);

	protected:
		virtual int Compare (int iKey1, int iKey2) const;
		virtual void CopyHandler (CObject *pOriginal);
		virtual ALERROR LoadHandler (CUnarchiver *pUnarchiver);
		virtual ALERROR SaveHandler (CArchiver *pArchiver);

	private:
		BOOL m_bOwned;
		BOOL m_bNoReference;
	};

//	CAtomTable. Implementation of a string hash table

class CAtomTable : public CObject
	{
	public:
		CAtomTable (void);
		CAtomTable (int iHashSize);
		virtual ~CAtomTable (void);

		ALERROR AppendAtom (const CString &sString, int *retiAtom);
		int Atomize (const CString &sString);

	private:
		CSymbolTable *Hash (const CString &sString);

		int m_iHashSize;
		int m_iNextAtom;
		CSymbolTable *m_pBackbone;
	};

//	CLargeSet

class CLargeSet
	{
	public:
		enum EConstants
			{
			INVALID_VALUE = 0xFFFFFFFF,
			};

		CLargeSet (int iSize = -1);

		void Clear (DWORD dwValue);
		void ClearAll (void);
		DWORD GetNextValue (DWORD dwStart = 0) const;
		bool InitFromString (const CString &sValue, DWORD dwMaxValue = 0, CString *retsError = NULL);
		bool IsEmpty (void) const;
		bool IsSet (DWORD dwValue) const;
		void Set (DWORD dwValue);

	private:
		static DWORD GetBitFromMask (DWORD dwMask);

		TArray<DWORD> m_Set;
	};

//	Atomizer

class CAtomizer
	{
	public:
		CAtomizer (void);

		DWORD Atomize (const CString &sIdentifier);
		int GetCount (void) const { return m_StringToAtom.GetCount(); }
		const CString &GetIdentifier (DWORD dwAtom) const;
		int GetMemoryUsage (void) const;

	private:
		DWORD m_dwNextID;
		TSortMap<CString, DWORD> m_StringToAtom;
		TArray<CString> m_AtomToString;
	};

//	Memory Blocks

class IReadBlock
	{
	public:
		virtual ~IReadBlock (void) { }

		virtual ALERROR Close (void) = 0;
		virtual ALERROR Open (void) = 0;
		virtual int GetLength (void) = 0;
		virtual char *GetPointer (int iOffset, int iLength = -1) = 0;
	};

class CFileReadBlock : public CObject, public IReadBlock
	{
	public:
		CFileReadBlock (void);
		CFileReadBlock (const CString &sFilename);
		virtual ~CFileReadBlock (void);

		const CString &GetFilename (void) const { return m_sFilename; }

		//	IReadBlock virtuals

		virtual ALERROR Close (void);
		virtual ALERROR Open (void);
		virtual int GetLength (void) { return (int)m_dwFileSize; }
		virtual char *GetPointer (int iOffset, int iLength = -1) { return m_pFile + iOffset; }

	private:
		CString m_sFilename;
		HANDLE m_hFile;
		HANDLE m_hFileMap;
		char *m_pFile;
		DWORD m_dwFileSize;
	};

class CResourceReadBlock : public CObject, public IReadBlock
	{
	public:
		CResourceReadBlock (void);
		CResourceReadBlock (HMODULE hInst, const char *pszRes, const char *pszType = RT_RCDATA);
		virtual ~CResourceReadBlock (void);

		//	IReadBlock virtuals

		virtual ALERROR Close (void);
		virtual ALERROR Open (void);
		virtual int GetLength (void) { return m_dwLength; }
		virtual char *GetPointer (int iOffset, int iLength = -1) { return m_pData + iOffset; }

	private:
		HMODULE m_hModule;
		const char *m_pszRes;
		const char *m_pszType;

		char *m_pData;
		DWORD m_dwLength;
	};

class CBufferReadBlock : public CObject, public IReadBlock
	{
	public:
		CBufferReadBlock (void) : CObject(NULL) { }
		CBufferReadBlock (const CString &sData) : CObject(NULL), m_sData(sData) { }

		//	IReadBlock virtuals

		virtual ALERROR Close (void) { return NOERROR; }
		virtual ALERROR Open (void) { return NOERROR; }
		virtual int GetLength (void) { return m_sData.GetLength(); }
		virtual char *GetPointer (int iOffset, int iLength = -1) { return m_sData.GetPointer() + iOffset; }

	private:
		CString m_sData;
	};

//	File System

class IWriteStream
	{
	public:
		virtual ALERROR Close (void) = 0;
		virtual ALERROR Create (void) = 0;
		virtual ALERROR Write (const char *pData, int iLength, int *retiBytesWritten = NULL) = 0;

		ALERROR Write (char chChar, int iLength = 1) { return WriteChar(chChar, iLength); }
		ALERROR Write (int iValue) { return Write((char *)&iValue, sizeof(DWORD)); }
		ALERROR Write (DWORD dwValue) { return Write((char *)&dwValue, sizeof(DWORD)); }
		ALERROR Write (double rValue) { return Write((char *)&rValue, sizeof(double)); }
		ALERROR Write (LONGLONG iValue) { return Write((char *)&iValue, sizeof(LONGLONG)); }
		ALERROR Write (DWORDLONG iValue) { return Write((char *)&iValue, sizeof(DWORDLONG)); }
		ALERROR Write (const CString &sString) { return Write(sString.GetPointer(), sString.GetLength()); }

		ALERROR WriteChar (char chChar, int iLength = 1);
		ALERROR WriteChars (const CString &sString, int *retiBytesWritten = NULL) { return Write(sString.GetASCIIZPointer(), sString.GetLength(), retiBytesWritten); }
	};

class IReadStream
	{
	public:
		virtual ALERROR Close (void) = 0;
		virtual ALERROR Open (void) = 0;
		virtual ALERROR Read (char *pData, int iLength, int *retiBytesRead = NULL) = 0;

		ALERROR Read (int &iValue) { return Read((char *)&iValue, sizeof(DWORD)); }
		ALERROR Read (DWORD &dwValue) { return Read((char *)&dwValue, sizeof(DWORD)); }
		ALERROR Read (double &rValue) { return Read((char *)&rValue, sizeof(double)); }
		ALERROR Read (LONGLONG &iValue) { return Read((char *)&iValue, sizeof(LONGLONG)); }
		ALERROR Read (DWORDLONG &iValue) { return Read((char *)&iValue, sizeof(DWORDLONG)); }
	};

//	CMemoryWriteStream. This object is used to write variable length
//	data to a memory block.

class CMemoryWriteStream : public CObject, public IWriteStream
	{
	public:
		CMemoryWriteStream (int iMaxSize = DEFAULT_MAX_SIZE);
		virtual ~CMemoryWriteStream (void);

		char *GetPointer (void) { return m_pBlock; }
		int GetLength (void) { return m_iCurrentSize; }
		void Seek (int iPos);

		//	IWriteStream virtuals

		virtual ALERROR Close (void) override;
		virtual ALERROR Create (void) override;
		virtual ALERROR Write (const char *pData, int iLength, int *retiBytesWritten = NULL) override;

		//	We want to inherit all the overloaded versions of Write.

		using IWriteStream::Write;

	private:
		enum Constants
			{
			DEFAULT_MAX_SIZE = 			(1024 * 1024),
			};

		int m_iMaxSize;
		int m_iCommittedSize;
		int m_iCurrentSize;
		char *m_pBlock;
	};

class CMemoryReadBlockWrapper : public IReadBlock
	{
	public:
		CMemoryReadBlockWrapper (CMemoryWriteStream &Stream) : 
				m_pPointer(Stream.GetPointer()),
				m_iLength(Stream.GetLength())
			{ }

		virtual ALERROR Close (void) { return NOERROR; }
		virtual ALERROR Open (void) { return NOERROR; }
		virtual int GetLength (void) { return m_iLength; }
		virtual char *GetPointer (int iOffset, int iLength = -1) { return m_pPointer + iOffset; }

	private:
		char *m_pPointer;
		int m_iLength;
	};

//	CMemoryReadStream. This object is used to read variable length data

class CMemoryReadStream : public CObject, public IReadStream
	{
	public:
		CMemoryReadStream (void);
		CMemoryReadStream (char *pData, int iDataSize);
		virtual ~CMemoryReadStream (void);

		//	IReadStream virtuals

		virtual ALERROR Close (void) override { return NOERROR; }
		virtual ALERROR Open (void) override { m_iPos = 0; return NOERROR; }
		virtual ALERROR Read (char *pData, int iLength, int *retiBytesRead = NULL) override;

		//	We want to inherit all the overloaded versions of Read.

		using IReadStream::Read;

	private:
		char *m_pData;
		int m_iDataSize;
		int m_iPos;
	};

//	CFileWriteStream. This object is used to write a file out

class CFileWriteStream : public CObject, public IWriteStream
	{
	public:
		CFileWriteStream (void);
		CFileWriteStream (const CString &sFilename, BOOL bUnique = FALSE);
		virtual ~CFileWriteStream (void);

		ALERROR Open (void);

		//	IWriteStream virtuals

		virtual ALERROR Close (void) override;
		virtual ALERROR Create (void) override;
		virtual ALERROR Write (const char *pData, int iLength, int *retiBytesWritten = NULL) override;

		//	We want to inherit all the overloaded versions of Write.

		using IWriteStream::Write;

	private:
		CString m_sFilename;
		BOOL m_bUnique;
		HANDLE m_hFile;
	};

//	CFileReadStream. This object is used to read a file in

class CFileReadStream : public CObject, public IReadStream
	{
	public:
		CFileReadStream (void);
		CFileReadStream (const CString &sFilename);
		virtual ~CFileReadStream (void);

		DWORD GetFileSize (void) { return m_dwFileSize; }

		//	IReadStream virtuals

		virtual ALERROR Close (void) override;
		virtual ALERROR Open (void) override;
		virtual ALERROR Read (char *pData, int iLength, int *retiBytesRead = NULL) override;

		//	We want to inherit all the overloaded versions of Read.

		using IReadStream::Read;

	private:
		CString m_sFilename;
		HANDLE m_hFile;
		HANDLE m_hFileMap;
		char *m_pFile;
		char *m_pPos;
		DWORD m_dwFileSize;
	};

//	CArchive. This is an object that knows how to archive objects to a 
//	stream.

class CArchiver : public CObject
	{
	public:
		CArchiver (void);
		CArchiver (IWriteStream *pStream);
		virtual ~CArchiver (void);

		ALERROR AddExternalReference (CString sTag, void *pReference);
		ALERROR AddObject (CObject *pObject);
		ALERROR BeginArchive (void);
		ALERROR EndArchive (void);
		void SetVersion (DWORD dwVersion) { m_dwVersion = dwVersion; }

		//	These methods should only be called by objects
		//	that are being saved

		ALERROR Reference2ID (void *pReference, int *retiID);
		ALERROR SaveObject (CObject *pObject);
		ALERROR SaveObject (CString *pObject);
		ALERROR WriteData (char *pData, int iLength);

	private:
		IWriteStream *m_pStream;					//	Stream to save to
		TArray<CObject *> m_List;					//	List of objects to save
		CDictionary m_ReferenceList;				//	Pointer references
		CSymbolTable m_ExternalReferences;			//	List of external references
		int m_iNextID;								//	Next ID to use for references
		DWORD m_dwVersion;							//	User-defined version
	};

//	CUnarchiver. This is an object that knows how to load objects from
//	a stream.

class CUnarchiver : public CObject
	{
	public:
		CUnarchiver (void);
		CUnarchiver (IReadStream *pStream);
		virtual ~CUnarchiver (void);

		ALERROR BeginUnarchive (void);
		ALERROR EndUnarchive (void);
		TArray<CObject *> &GetList (void) { return m_List; }
		CObject *GetObject (int iIndex);
		DWORD GetVersion (void) { return m_dwVersion; }
		ALERROR ResolveExternalReference (CString sTag, void *pReference);
		void SetMinVersion (DWORD dwVersion) { m_dwMinVersion = dwVersion; }

		//	These methods should only be called by objects
		//	that are being loaded

		ALERROR LoadObject (CObject **retpObject);
		ALERROR LoadObject (CString **retpString);
		ALERROR ReadData (char *pData, int iLength);
		ALERROR ResolveReference (int iID, void **pReference);

	private:
		IReadStream *m_pStream;
		TArray<CObject *> m_List;
		CSymbolTable *m_pExternalReferences;
		CIntArray m_ReferenceList;
		CIntArray m_FixupTable;
		DWORD m_dwVersion;
		DWORD m_dwMinVersion;
	};

//	CDataFile. This is a file-based collection of variable-sized records.

#define DFOPEN_FLAG_READ_ONLY					0x00000001

class CDataFile : public CObject
	{
	public:
		struct SVersionInfo
			{
			DWORD dwVersion;
			int iEntry;
			};

		CDataFile (const CString &sFilename);
		virtual ~CDataFile (void);

		ALERROR AddEntry (const CString &sData, int *retiEntry);
		ALERROR Close (void);
		ALERROR DeleteEntry (int iEntry);
		ALERROR Flush (void);
		CString GetFilename (void) const { return m_sFilename; }
		int GetDefaultEntry (void);
		int GetEntryLength (int iEntry);
		BOOL IsOpen (void) { return (m_hFile != INVALID_HANDLE_VALUE || m_pFile); }
		ALERROR Open (DWORD dwFlags = 0) { return Open(NULL_STR, dwFlags); }
		ALERROR Open (const CString &sFilename, DWORD dwFlags = 0);
		ALERROR OpenFromResource (HMODULE hInst, char *pszRes, DWORD dwFlags = 0);
		ALERROR ReadEntry (int iEntry, CString *retsData);
		ALERROR ReadEntryPartial (int iEntry, int iPos, int iLength, CString *retsData);
		ALERROR ReadHistory (int iEntry, TArray<SVersionInfo> *retHistory);
		void SetDefaultEntry (int iEntry);
		ALERROR WriteEntry (int iEntry, const CString &sData);
		ALERROR WriteVersion (int iEntry, const CString &sData, DWORD *retdwVersion = NULL);

		static ALERROR Create (const CString &sFilename,
							   int iBlockSize,
							   int iInitialEntries);

	private:
		typedef struct
			{
			DWORD dwBlock;								//	Block Number (-1 = unused)
			DWORD dwBlockCount;							//	Number of blocks reserved for entry
			DWORD dwSize;								//	Size of entry
			DWORD dwVersion;							//	Version number
			DWORD dwPrevEntry;							//	Previous version
			DWORD dwLatestEntry;						//	Latest entry (-1 if this is latest)
			DWORD dwFlags;								//	Misc flags
			} ENTRYSTRUCT, *PENTRYSTRUCT;

		struct SEntryV1
			{
			DWORD dwBlock;								//	Block Number (-1 = unused)
			DWORD dwBlockCount;							//	Number of blocks reserved for entry
			DWORD dwSize;								//	Size of entry
			DWORD dwFlags;								//	Misc flags
			};

		ALERROR AllocBlockChain (DWORD dwBlockCount, DWORD *retdwStartingBlock);
		ALERROR FreeBlockChain (DWORD dwStartingBlock, DWORD dwBlockCount);
		ALERROR GrowEntryTable (int *retiEntry);
		ALERROR OpenInt (void);
		ALERROR ReadBuffer (DWORD dwFilePos, DWORD dwLen, void *pBuffer);
		ALERROR ResizeEntry (int iEntry, DWORD dwSize, DWORD *retdwBlockCount);
		ALERROR WriteBlockChain (DWORD dwStartingBlock, char *pData, DWORD dwSize);

		CString m_sFilename;							//	Filename of data file
		HANDLE m_hFile;									//	Open file handle
		IReadBlock *m_pFile;							//	Memory file

		int m_iBlockSize;								//	Size of each block
		int m_iBlockCount;								//	Number of blocks in file
		int m_iDefaultEntry;							//	Default entry

		int m_iEntryTableCount;							//	Number of entries
		PENTRYSTRUCT m_pEntryTable;						//	Entry table

		DWORD m_fHeaderModified:1;						//	TRUE if header has changed
		DWORD m_fEntryTableModified:1;					//	TRUE if entry table has changed
		DWORD m_fFlushing:1;							//	TRUE if we're inside ::Flush
		DWORD m_fReadOnly:1;							//	TRUE if we're open read-only
	};

//	Directory classes

struct SFileDesc
	{
	CString sFilename;

	bool bFolder;
	bool bReadOnly;
	bool bSystemFile;
	bool bHiddenFile;
	};

class CFileDirectory
	{
	public:
		CFileDirectory (const CString &sFilespec);
		~CFileDirectory (void);

		bool HasMore (void);
		CString GetNext (bool *retbIsFolder = NULL);
		void GetNextDesc (SFileDesc *retDesc);

	private:
		CString m_sFilespec;
		HANDLE m_hSearch;
		WIN32_FIND_DATA m_FindData;
	};

//	Logging classes

#define ILOG_FLAG_WARNING					0x00000001	//	Warning log entry
#define ILOG_FLAG_ERROR						0x00000002	//	Error log entry
#define ILOG_FLAG_FATAL						0x00000004	//	Fatal error log entry
#define ILOG_FLAG_TIMEDATE					0x00000008	//	Include time date

class ILog
	{
	public:
		void LogOutput (DWORD dwFlags, char *pszLine, ...) const;
		void LogOutput (DWORD dwFlags, const CString &sLine) const;
		virtual void Print (const CString &sLine) const = 0;
		virtual void Progress (const CString &sLine, int iPercent = -1) const { Print(sLine); }
	};

class CTextFileLog : public ILog
	{
	public:
		CTextFileLog (void);
		CTextFileLog (const CString &sFilename);

		ALERROR Close (void);
		ALERROR Create (bool bAppend);
		CString GetSessionLog (void);
		void SetFilename (const CString &sFilename);
		void SetSessionStart (void);
		virtual ~CTextFileLog (void);

		//	ILog virtuals

		virtual void Print (const CString &sLine) const override;

	private:
		HANDLE m_hFile = NULL;
		CString m_sFilename;

		DWORD m_dwSessionStart = 0;			//	Offset to file at start of session
	};

//	Registry classes

class CRegKey
	{
	public:
		CRegKey (void);
		~CRegKey (void);

		static ALERROR OpenUserAppKey (const CString &sCompany, 
									   const CString &sAppName,
									   CRegKey *retKey);

		operator HKEY() const { return m_hKey; }

		bool FindStringValue (const CString &sValue, CString *retsData);
		void SetStringValue (const CString &sValue, const CString &sData);

	private:
		void CleanUp (void);

		HKEY m_hKey;
	};

//	Thread pool

class IThreadPoolTask
	{
	public:
		virtual ~IThreadPoolTask (void) { }
		virtual void Run (void) { }
	};

class CThreadPool
	{
	public:
		~CThreadPool (void) { CleanUp(); }

		void AddTask (IThreadPoolTask *pTask);
		bool Boot (int iThreadCount);
		void CleanUp (void);
		int GetThreadCount (void) const { return m_Threads.GetCount() + 1; }
		void Run (void);

	private:
		struct SThreadDesc
			{
			HANDLE hThread;
			};

		IThreadPoolTask *GetTaskToRun (void);
		void RunTask (IThreadPoolTask *pTask);
		void WorkerThread (void);

		static DWORD WINAPI WorkerThreadStub (LPVOID pData) { ((CThreadPool *)pData)->WorkerThread(); return 0; }

		CCriticalSection m_cs;
		TArray<SThreadDesc> m_Threads;
		TQueue<IThreadPoolTask *> m_Tasks;
		int m_iTasksRemaining;
		TArray<IThreadPoolTask *> m_Completed;
		CManualEvent m_WorkAvail;
		CManualEvent m_WorkCompleted;
		CManualEvent m_Quit;
	};

//	Initialization functions (Kernel.cpp)

void kernelCleanUp (void);
void kernelClearDebugLog (void);
void kernelDebugLogPattern (const char *pszLine, ...);
void kernelDebugLogString (const CString &sLine);
CString kernelGetSessionDebugLog (void);

#define KERNEL_FLAG_INTERNETS					0x00000001
BOOL kernelInit (DWORD dwFlags = 0);
ALERROR kernelSetDebugLog (const CString &sFilespec, bool bAppend = true);
ALERROR kernelSetDebugLog (CTextFileLog *pLog, bool bAppend = true, bool bFreeLog = false);
HANDLE kernelCreateThread (LPTHREAD_START_ROUTINE pfStart, LPVOID pData);
bool kernelDispatchUntilEventSet (HANDLE hEvent, DWORD dwTimeout = INFINITE);

//	String and Arrays

static const DWORD DELIMIT_TRIM_WHITESPACE =		0x00000001;
static const DWORD DELIMIT_ALLOW_BLANK_STRINGS =	0x00000002;
static const DWORD DELIMIT_COMMA =					0x00000004;
static const DWORD DELIMIT_SEMI_COLON =				0x00000008;
static const DWORD DELIMIT_QUOTE_ESCAPE =			0x00000010;
static const DWORD DELIMIT_AUTO_COMMA =				0x00000020;
ALERROR strDelimitEx (const CString &sString, char cDelim, DWORD dwFlags, int iMinParts, TArray<CString> *retList);

inline ALERROR strDelimit (const CString &sString, char cDelim, int iMinParts, TArray<CString> *pStringList)
	{ return strDelimitEx(sString, cDelim, 0, iMinParts, pStringList); }

CString strJoin (const TArray<CString> &List, const CString &sConjunction);


//	Path functions (Path.cpp)

enum ESpecialFolders
	{
	folderAppData,
	folderDocuments,
	folderPictures,
	folderMusic,
	};

struct SFileVersionInfo
	{
	SFileVersionInfo (void) :
			dwFileVersion(0),
			dwProductVersion(0)
		{ }

	CString sProductName;
	CString sProductVersion;
	CString sCompanyName;
	CString sCopyright;

	ULONG64 dwFileVersion;
	ULONG64 dwProductVersion;
	};

bool fileCopy (const CString &sSourceFilespec, const CString &sDestFilespec);
bool fileDelete (const CString &sFilespec, bool bRecycle = false);

const DWORD FFL_FLAG_DIRECTORIES_ONLY =		0x00000001;
const DWORD FFL_FLAG_RELATIVE_FILESPEC =	0x00000002;
const DWORD FFL_FLAG_RECURSIVE =			0x00000004;
bool fileGetFileList (const CString &sRoot, const CString &sPath, const CString &sSearch, DWORD dwFlags, TArray<CString> *retFiles);

CTimeDate fileGetModifiedTime (const CString &sFilespec);
CString fileGetProductName (void);
DWORD fileGetProductVersion (void);
ALERROR fileGetVersionInfo (const CString &sFilename, SFileVersionInfo *retInfo);
bool fileMove (const CString &sSourceFilespec, const CString &sDestFilespec);
bool fileOpen (const CString &sFile, const CString &sParameters = NULL_STR, const CString &sCurrentFolder = NULL_STR, CString *retsError = NULL);

CString pathAddComponent (const CString &sPath, const CString &sComponent);
CString pathAddExtensionIfNecessary (const CString &sPath, const CString &sExtension);
bool pathCreate (const CString &sPath);
bool pathDeleteAll (const CString &sPath);
bool pathExists (const CString &sPath);
CString pathGetExecutablePath (HINSTANCE hInstance);
CString pathGetExtension (const CString &sPath);
CString pathGetFilename (const CString &sPath);
CString pathGetPath (const CString &sPath);
CString pathGetResourcePath (char *pszResID);
CString pathGetSpecialFolder (ESpecialFolders iFolder);
CString pathGetTempPath (void);
bool pathIsAbsolute (const CString &sPath);
bool pathIsFolder (const CString &sFilespec);
inline bool pathIsPathSeparator (char *pPos) { return (*pPos == '\\' || *pPos == '/'); }
bool pathIsResourcePath (const CString &sPath, char **retpszResID);
bool pathIsWritable (const CString &sFilespec);
CString pathMakeAbsolute (const CString &sPath, const CString &sRoot = NULL_STR);
CString pathMakeRelative (const CString &sFilespec, const CString &sRoot, bool bNoCheck = false);
CString pathStripExtension (const CString &sPath);
bool pathValidateFilename (const CString &sFilename, CString *retsValidFilename = NULL);

//	RegEx functions (RegEx.cpp)

struct SRegExMatch
	{
	char *pPos;
	CString sMatch;
	};

bool strRegEx (char *pStart, const CString &sPattern, TArray<SRegExMatch> *retMatches = NULL, char **retpEnd = NULL);

//	Math functions (Math.cpp)

int mathAdjust (int iValue, int iPercent);
int mathAdjustRound (int iValue, int iPercent);
DWORD mathGetSeed (void);
DWORD mathMakeSeed (DWORD dwValue);
int mathNearestPowerOf2 (int x);
int mathPower (int x, int n);
DWORD mathRandom (void);
inline double mathRandomDouble (void) { return (mathRandom() / 2147483648.0); }
int mathRandom (int iFrom, int iTo);
double mathRandomGaussian (void);
double mathRandomMinusOneToOne (void);
int mathRound (double x);
int mathRoundStochastic (double x);
int mathSeededRandom (int iSeed, int iFrom, int iTo);
void mathSetSeed (DWORD dwSeed);
int mathSqrt (int x);

#include "TMath.h"

//	Compression functions

void CompressRunLengthByte (IWriteStream *pOutput, IReadBlock *pInput);
void UncompressRunLengthByte (IWriteStream *pOutput, IReadBlock *pInput);

//	System functions

DWORD sysGetAPIFlags (void);
DWORD sysGetTicksElapsed (DWORD dwTick, DWORD *retdwNow = NULL);
int sysGetProcessorCount (void);
CString sysGetUserName (void);
bool sysIsBigEndian (void);
bool sysOpenURL (const CString &sURL);

//	Utility functions (Utilities.cpp)

DWORD utlHashFunctionCase (BYTE *pKey, int iKeyLen);
void utlMemSet (LPVOID pDest, DWORD Count, BYTE Value);
void utlMemCopy (const char *pSource, char *pDest, DWORD dwCount);
BOOL utlMemCompare (char *pSource, char *pDest, DWORD dwCount);
inline LPVOID MemAlloc (int iSize) { return (BYTE *)HeapAlloc(GetProcessHeap(), 0, iSize); }
inline void MemFree (LPVOID pMem) { HeapFree(GetProcessHeap(), 0, pMem); }

//	UI functions

ALERROR uiCopyTextToClipboard (HWND hWnd, const CString &sText);
void uiGetCenteredWindowRect (int cxWidth, 
							  int cyHeight, 
							  RECT *retrcRect,
							  bool bClip = true);
inline bool uiIsControlDown (void) { return (::GetAsyncKeyState(VK_CONTROL) & 0x8000) ? true : false; }
inline bool uiIsKeyDown (int iVirtKey) { return ((::GetAsyncKeyState(iVirtKey) & 0x8000) ? true : false); }
inline bool uiIsKeyRepeat (DWORD dwKeyData) { return ((dwKeyData & 0x40000000) ? true : false); }
inline bool uiIsNumLockOn (void) { return (::GetKeyState(VK_NUMLOCK) & 0x0001) ? true : false; }
inline bool uiIsShiftDown (void) { return (::GetAsyncKeyState(VK_SHIFT) & 0x8000) ? true : false; }
inline char uiGetCharFromKeyCode (int iVirtKey) { DWORD dwChar = ::MapVirtualKey((UINT)iVirtKey, MAPVK_VK_TO_CHAR); return (dwChar < 256 ? (char)(BYTE)dwChar : 0); }

//	Note: This cannot be an inline because it will fail if the inline is
//	ever compiled as a function call
#define MemStackAlloc(iSize) (_alloca(iSize))

//	Comparison functions

template<>
inline int KeyCompare<LPCSTR> (const LPCSTR &Key1, const LPCSTR &Key2)
	{
	return strCompareAbsolute(Key1, Key2);
	}

template<>
inline int KeyCompare<CString> (const CString &sKey1, const CString &sKey2)
	{
	return strCompareAbsolute(sKey1, sKey2);
	}

template<>
inline int KeyCompare<CTimeDate> (const CTimeDate &Key1, const CTimeDate &Key2)
	{
	return Key1.Compare(Key2);
	}

//	The following macro "NoEmptyFile()" can be put into a file 
//	in order suppress the MS Visual C++ Linker warning 4221 
// 
//	warning LNK4221: no public symbols found; archive member will be inaccessible 
// 
//	Thanks to: http://stackoverflow.com/users/14904/adisak
//	See: http://stackoverflow.com/questions/1822887/what-is-the-best-way-to-eliminate-ms-visual-c-linker-warning-warning-lnk422
 
#define NoEmptyFile()   namespace { char NoEmptyFileDummy##__LINE__; } 

};

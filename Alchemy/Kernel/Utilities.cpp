//	Utilities.cpp
//
//	Miscellaneous utility functions
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include <cstring>
#include <bit>

DWORD Kernel::sysGetTicksElapsed (DWORD dwTick, DWORD *retdwNow)

//	sysGetTicksElapsed
//
//	Returns the number of milliseconds since the given tick count

	{
	DWORD dwNow = ::GetTickCount();
	if (retdwNow)
		*retdwNow = dwNow;

	if (dwNow < dwTick)
		return (0xffffffff - dwTick) + dwNow + 1;
	else
		return dwNow - dwTick;
	}

//	sysGetProcessorsInMask
//
//	Gets the number of processors in a KAFFINITY mask
//	Note that this can only accurately count up to 32 processors
//	in a 32-bit app
//
DWORD Kernel::sysGetProcessorsInMask(KAFFINITY &AffinityMask)
	{
	return std::popcount(AffinityMask);
	}

//	sysGetProcessorInfo
// 
//	Returns an SProcessorInfo struct with high level details
//	about the number of cores and processor groups in the cpu
//	It can return core counts above 32 (32-bit) or 64 (64-bit)
//	Note that these may span multiple processor groups
//
SProcessorInfo Kernel::sysGetProcessorInfo(void)
	{
	SProcessorInfo sInfo = SProcessorInfo();
	DWORD dwLength = 0;
	DWORD dwWinError = 0;
	GetLogicalProcessorInformationEx(RelationAll, NULL, &dwLength);
	dwWinError = GetLastError();
	if (dwWinError != ERROR_INSUFFICIENT_BUFFER)
		{
		ASSERT(false);
		//	sInfo.fSuccess defaults to false
		return sInfo;
		}
	//	This buffer must be treated as a buffer of bytes, because the SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX structs
	//	have variable length, as demarkated in their size field.
	union
		{
		BYTE* pBuffer;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pSysInfo;
		};
	union
		{
		BYTE* pBufferPos;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pCurSysInfo;
		};
	pSysInfo = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)malloc(dwLength);
	DWORD dwMaxLength = dwLength;

	//	We call this differently in 32bit vs 64bit
	//  Calling with RelativeProcessorCore as the type has special behavior
	//  for getting the true count of cores on a 32bit system, because
	//	the KAFFINITY mask struct can only show 32 bits on a 32bit system
	// 
	//	Attempts to get an accurate number but will fall back to
	//	sysGetProcessorCountLegacy.
	//  The actual number of processors in the cpu is at least this high
	// 
	//	https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformationex

	if (GetLogicalProcessorInformationEx(RelationAll, pSysInfo, &dwLength))
		{
		SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX CurInfo;
		for (DWORD dwOffset = 0; dwOffset < dwLength; dwOffset += CurInfo.Size)
			{

			//	Intellisense thinks pBuffer is NULL because it doesnt
			//	handle the union correctly. Its actually non-NULL.

			pBufferPos = &(pBuffer[dwOffset]);
			CurInfo = *pCurSysInfo;

			//	If this has data about cpu cores

			if (CurInfo.Relationship == RelationProcessorCore)
				{

				//	A relation processor core always corresponds to 1 physical core 

				sInfo.dwNumPhysical++;

				//	if this has SMT, we need to count logical cores based on mask bits

				if (CurInfo.Processor.Flags & LTP_PC_SMT)
					{

					//	RelationProcessorCore only ever has 1 GroupMask.
					//	https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-processor_relationship
					// 
					//	Note: In 32bit apps KAFFINITY is only 32bits and the
					//	upper 32 processors are mirrored onto the lower 32 processors
					//	As a result it works up till 32-way SMT

					sInfo.dwNumLogical += sysGetProcessorsInMask(CurInfo.Processor.GroupMask[0].Mask);
					}

				//	otherwise it only has 1 logical core

				else
					sInfo.dwNumLogical++;
				}

			//	If this has data about processor groups

			else if (CurInfo.Relationship == RelationGroup)
				{
				sInfo.dwNumProcessorGroups += CurInfo.Group.ActiveGroupCount;
				if (CurInfo.Group.ActiveGroupCount != CurInfo.Group.MaximumGroupCount)
					sInfo.fCanAddProcessorGroups = 1;
				}
			}
		sInfo.fReliablePhysicalProcessorCount = 1;
		sInfo.fReliableLogicalProcessorCount = sInfo.dwNumLogical < 32 || RELIABLE_AFFINITY_MASK ? 1 : 0;
		sInfo.fReliableProcessorGroups = 1;
		}
	else
		{
			ASSERT(false);
			//	sInfo.fSuccess defaults to false
			return sInfo;
		}

	//	fallback implementation

	if (!(sInfo.dwNumPhysical && sInfo.dwNumLogical && sInfo.dwNumProcessorGroups))
		{
		//	If we know that our logical processor count is reliable, we can
		//	Interpolate the other values based on it, even though we cant
		//	know for sure if the physical processor count is accurate

		if (sInfo.fReliableLogicalProcessorCount)
			{
			sInfo.dwNumPhysical = sInfo.dwNumLogical;
			sInfo.dwNumProcessorGroups = (sInfo.dwNumLogical >> 6) + 1;
			}

		//	Otherwise we have to guess

		else
			{
			sInfo.dwNumLogical = sysGetProcessorCountLegacy();
			if (!sInfo.dwNumPhysical)
				sInfo.dwNumPhysical = sInfo.dwNumLogical;
			if (!sInfo.dwNumProcessorGroups)
				sInfo.dwNumProcessorGroups = 1;

			//	If we have fewer than 32 logical processors its accurate
			//	aside from some extremely weird system configurations where
			//	someone is manually using tiny processor groups, probably
			//	to box in processor group unaware apps deliberately.
			//
			//	Ex, on older multi-socket systems to keep things on
			//	a single socket without using affinity masks.
			//
			//	As a result we assume that this is accurate and just
			//	pretend to be a processor group unaware app by limiting
			//	ourselves to 1.

			if (sInfo.dwNumLogical < 32)
				sInfo.fReliableLogicalProcessorCount = true;
			sInfo.fReliableProcessorGroups = 1;
			sInfo.dwNumProcessorGroups = 1;
			}
		}
	else
		sInfo.fSuccess = true;

	free(pBuffer);

	return sInfo;
	}

//	sysGetProcessorCountLegacy
// 
//  Returns the number of processors in the current processor group.
// 	This is a max of 32 on 32-bit windows or 64 or 64-bit windows.
//  The actual number of processors in the cpu is at least this high
//
int Kernel::sysGetProcessorCountLegacy(void)
	{
	SYSTEM_INFO si;
	::GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
	}


//	sysGetProcessorCountLegacy
// 
//  Returns a number of processors in the cpu. Attempts to get an
//	accurate number but will fall back to sysGetProcessorCountLegacy.
//  The actual number of processors in the cpu is at least this high
//
int Kernel::sysGetProcessorCount(void)
	{
	SProcessorInfo sInfo = sysGetProcessorInfo();
	if (sInfo.fSuccess)
		return sInfo.dwNumLogical;
	else
		return sysGetProcessorCountLegacy();
	}

CString Kernel::sysGetUserName (void)

//	sysGetUserName
//
//	Returns the name of the currently logged-on user

	{
	CString sName;
	DWORD dwLen = 256;
	char *pPos = sName.GetWritePointer(dwLen);

	::GetUserName(pPos, &dwLen);
	sName.Truncate(lstrlen(pPos));

	return sName;
	}

bool Kernel::sysIsBigEndian (void)

//	sysIsBigEndian
//
//	Returns TRUE if we're running on a big-endian architecture

	{
	return ((*(unsigned short *) ("#S") >> 8) == '#');
	}

bool Kernel::sysOpenURL (const CString &sURL)

//	sysOpenURL
//
//	Launches the default browser to the given URL.

	{
	HINSTANCE hResult = ::ShellExecute(NULL, "open", sURL.GetASCIIZPointer(), NULL, NULL, SW_SHOWNORMAL);
	if ((DWORD)hResult <= 32)
		{
		//	Errors are <= 32.
		return false;
		}

	//	Done

	return true;
	}

#define mix(a,b,c) \
	{ \
	a -= b; a -= c; a ^= (c>>13); \
	b -= c; b -= a; b ^= (a<<8); \
	c -= a; c -= b; c ^= (b>>13); \
	a -= b; a -= c; a ^= (c>>12); \
	b -= c; b -= a; b ^= (a<<16); \
	c -= a; c -= b; c ^= (b>>5); \
	a -= b; a -= c; a ^= (c>>3); \
	b -= c; b -= a; b ^= (a<<10); \
	c -= a; c -= b; c ^= (b>>15); \
	}

DWORD Kernel::utlHashFunctionCase (BYTE *pKey, int iKeyLen)

//	utlHashFunction
//
//	Hash the data
//
//	Source: Bob Jenkins
//	http://burtleburtle.net/bob/hash/evahash.html

	{
	DWORD initval = 1013;
	BYTE *k = pKey;
	DWORD a,b,c,len;

	//	Set up the internal state
	len = iKeyLen;
	a = b = 0x9e3779b9;		//	the golden ratio; an arbitrary value
	c = initval;			//	the previous hash value (arbitrary)

	//	---------------------------------------- handle most of the key

	while (len >= 12)
		{
		a += (k[0] +((DWORD)k[1]<<8) +((DWORD)k[2]<<16) +((DWORD)k[3]<<24));
		b += (k[4] +((DWORD)k[5]<<8) +((DWORD)k[6]<<16) +((DWORD)k[7]<<24));
		c += (k[8] +((DWORD)k[9]<<8) +((DWORD)k[10]<<16)+((DWORD)k[11]<<24));
		mix(a,b,c);
		k += 12; len -= 12;
		}

	//	------------------------------------- handle the last 11 bytes
	c += iKeyLen;
	switch(len)				//	all the case statements fall through
		{
		case 11: c+=((DWORD)k[10]<<24); [[fallthrough]];
		case 10: c+=((DWORD)k[9]<<16); [[fallthrough]];
		case 9 : c+=((DWORD)k[8]<<8); [[fallthrough]];
		//	the first byte of c is reserved for the length */
		case 8 : b+=((DWORD)k[7]<<24); [[fallthrough]];
		case 7 : b+=((DWORD)k[6]<<16); [[fallthrough]];
		case 6 : b+=((DWORD)k[5]<<8); [[fallthrough]];
		case 5 : b+=k[4]; [[fallthrough]];
		case 4 : a+=((DWORD)k[3]<<24); [[fallthrough]];
		case 3 : a+=((DWORD)k[2]<<16); [[fallthrough]];
		case 2 : a+=((DWORD)k[1]<<8); [[fallthrough]];
		case 1 : a+=k[0];
		//	case 0: nothing left to add */
		}

	mix(a,b,c);

	//	-------------------------------------------- report the result
	return c;
	}

#undef mix

void Kernel::utlMemSet (LPVOID pDest, DWORD Count, BYTE Value)

//	utlMemSet
//
//	Initializes a block of memory to the given value.
//	
//	Inputs:
//		pDest: Pointer to block of memory to initialize
//		Count: Length of block in bytes
//		Value: Value to initialize to

	{
	std::memset(pDest, Value, Count);
	}

void Kernel::utlMemCopy (const char *pSource, char *pDest, DWORD dwCount)

//	utlMemCopy
//
//	Copies a block of memory of Count bytes from pSource to pDest.
//	
//	Inputs:
//		pSource: Pointer to source memory block
//		pDest: Pointer to destination memory block
//		dwCount: Number of bytes to copy

	{
	std::memcpy(pDest, pSource, dwCount);
	}

BOOL Kernel::utlMemCompare (char *pSource, char *pDest, DWORD dwCount)

//	utlMemCompare
//
//	Compare two blocks of memory for equality

	{
	for (; dwCount > 0; dwCount--)
		if (*pDest++ != *pSource++)
			return FALSE;

	return TRUE;
	}

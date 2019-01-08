//	CManualEvent.cpp
//
//	CManualEvent class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "Kernel.h"
#include "KernelObjID.h"

void CManualEvent::Create (void)

//	Create
//
//	Create the event

	{
	Create(NULL_STR); 
	}

void CManualEvent::Create (const CString &sName, bool *retbExists)

//	Create
//
//	Create the event

	{
	Close();

	//	Create the semaphore

	m_hHandle = ::CreateEvent(NULL, 
			TRUE, 
			FALSE, 
			(sName.IsBlank() ? NULL : (LPSTR)sName));
	if (m_hHandle == NULL)
		throw CException(ERR_MEMORY);

	//	See if the semaphore already exists

	if (retbExists)
		*retbExists = (::GetLastError() == ERROR_ALREADY_EXISTS);
	}

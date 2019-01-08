//	CDXBackgroundBlt.cpp
//
//	CDXBackgroundBlt class
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include <windows.h>
#include <dsound.h>
#include "Alchemy.h"
#include "DirectXUtil.h"

CDXBackgroundBlt::CDXBackgroundBlt (CDXScreen &DX) :
		m_DX(DX),
		m_hBackgroundThread(INVALID_HANDLE_VALUE),
		m_hWorkEvent(INVALID_HANDLE_VALUE),
		m_hQuitEvent(INVALID_HANDLE_VALUE)

//	CDXBackgroundBlt constructor

	{
	}

DWORD WINAPI CDXBackgroundBlt::BackgroundThread (LPVOID pData)

//	BackgroundThread
//
//	Background thread

	{
	CDXBackgroundBlt *pThis = (CDXBackgroundBlt *)pData;

	while (true)
		{
		HANDLE Handles[2];
		Handles[0] = pThis->m_hQuitEvent;
		const DWORD WAIT_QUIT_EVENT = (WAIT_OBJECT_0);
		Handles[1] = pThis->m_hWorkEvent;
		const DWORD WAIT_WORK_EVENT = (WAIT_OBJECT_0 + 1);

		//	Wait for work to do

		DWORD dwWait = ::WaitForMultipleObjects(2, Handles, FALSE, INFINITE);

		//	Do the work

		if (dwWait == WAIT_QUIT_EVENT)
			return 0;
		else if (dwWait == WAIT_WORK_EVENT)
			{
			CSmartLock Lock(pThis->m_cs);
			pThis->m_DX.Render();
			::ResetEvent(pThis->m_hWorkEvent);
			}
		}
	}

void CDXBackgroundBlt::Render (void)

//	Render
//
//	Blt to screen

	{
	CSmartLock Lock(m_cs);

	//	If background thread is enabled, then let it blt.

	if (IsEnabled())
		{
		m_DX.SwapBuffers();
		::SetEvent(m_hWorkEvent);
		}

	//	Otherwise, we paint on this thread

	else
		{
		m_DX.SwapBuffers();
		m_DX.Render();
		}
	}

void CDXBackgroundBlt::CleanUp (void)

//	CleanUp
//
//	Clean up class

	{
	if (IsEnabled())
		{
		::SetEvent(m_hQuitEvent);
		::WaitForSingleObject(m_hBackgroundThread, 5000);

		::CloseHandle(m_hBackgroundThread);
		m_hBackgroundThread = INVALID_HANDLE_VALUE;

		::CloseHandle(m_hWorkEvent);
		m_hWorkEvent = INVALID_HANDLE_VALUE;

		::CloseHandle(m_hQuitEvent);
		m_hQuitEvent = INVALID_HANDLE_VALUE;
		}
	}

bool CDXBackgroundBlt::Init (int cxWidth, int cyHeight, CString *retsError)

//	Init
//
//	Initialize

	{
	//	If we have multiple CPU cores, then we create a thread to paint in the
	//	background.

#ifndef DEBUG_NO_DX_BLT_THREAD
	if (::sysGetProcessorCount() > 1)
		{
		ASSERT(!IsEnabled());

		m_hQuitEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hWorkEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hBackgroundThread = ::kernelCreateThread(BackgroundThread, this);
		}
#endif

	return true;
	}

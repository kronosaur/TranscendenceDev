//	CTextFileLog.cpp
//
//	Implements CTextFileLog object
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CTextFileLog::CTextFileLog (void) : 
		m_hFile(NULL),
		m_dwSessionStart(0)

//	CTextFileLog constructor

	{
	}

CTextFileLog::CTextFileLog (const CString &sFilename) : 
		m_sFilename(sFilename),
		m_hFile(NULL)

//	CTextFileLog constructor

	{
	}

CTextFileLog::~CTextFileLog (void)

//	CTextFileLog destructor

	{
	Close();
	}

ALERROR CTextFileLog::Close (void)

//	Close
//
//	Close the log

	{
	if (m_hFile == NULL)
		return NOERROR;

	CloseHandle(m_hFile);
	m_hFile = NULL;

	return NOERROR;
	}

ALERROR CTextFileLog::Create (bool bAppend)

//	Create
//
//	Create a new log file.
//
//	NOTE: If the log file is open by another process, we return ERR_FILE_IN_USE.

	{
	ASSERT(m_hFile == NULL);

	m_hFile = CreateFile(m_sFilename.GetASCIIZPointer(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
		{
		m_hFile = NULL;

		//	If the file is open by another process, then return a different error.

		DWORD dwError = ::GetLastError();
		if (dwError == ERROR_SHARING_VIOLATION)
			return ERR_FILE_IN_USE;
		else
			return ERR_FAIL;
		}

	//	If we're appending to an existing log file, move the file pointer
	//	to the end of the file.

	if (bAppend)
		{
		LONG lFileHigh = 0;
		m_dwSessionStart = ::SetFilePointer(m_hFile, 0, &lFileHigh, FILE_END);
		}

	//	Otherwise, truncate the file

	else
		{
		SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
		SetEndOfFile(m_hFile);
		m_dwSessionStart = 0;
		}

	return NOERROR;
	}

CString CTextFileLog::GetSessionLog (void)

//	GetSessionLog
//
//	Returns all output since the start of the session.

	{
	if (!m_hFile)
		throw CException(ERR_FAIL);

	//	Figure out the current position of the file pointer

	DWORD dwCurPos = ::SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
	if (dwCurPos == INVALID_SET_FILE_POINTER)
		return CONSTLIT("ERROR: Unable to seek in kernel debug log.");

	//	Set the file pointer to the start of the session

	if (::SetFilePointer(m_hFile, m_dwSessionStart, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return CONSTLIT("ERROR: Unable to seek in kernel debug log.");

	//	Make sure the file log is not too big

	DWORD dwLogSize = dwCurPos - m_dwSessionStart;
	if (dwCurPos < m_dwSessionStart || dwLogSize > 1000000)
		return CONSTLIT("ERROR: Kernel debug log is too big.");

	//	Allocate an appropriate string

	CString sLog;
	char *pLog = sLog.GetWritePointer(dwLogSize);

	//	Read from the file

	DWORD dwRead;
	if (!::ReadFile(m_hFile, pLog, dwLogSize, &dwRead, NULL))
		return CONSTLIT("ERROR: Unable to read kernel debug log.");

	//	Restore position

	::SetFilePointer(m_hFile, 0, NULL, FILE_END);

	return sLog;
	}

void CTextFileLog::Print (const CString &sLine) const

//	Print
//
//	Outputs a line to the log

	{
	if (!m_hFile)
		throw CException(ERR_FAIL);

	DWORD dwWritten;
	WriteFile(m_hFile, sLine.GetASCIIZPointer(), sLine.GetLength(), &dwWritten, NULL);
	WriteFile(m_hFile, "\r\n", 2, &dwWritten, NULL);

	//	Flush now because we don't want to lose any info if we crash

	FlushFileBuffers(m_hFile);
	}

void CTextFileLog::SetFilename (const CString &sFilename)

//	SetFilename
//
//	Sets the filename of the log file

	{
	ASSERT(m_hFile == NULL);
	m_sFilename = sFilename;
	}

void CTextFileLog::SetSessionStart (void)

//	SetSessionStart
//
//	Sets the start of the session at the current position of the log.

	{
	if (!m_hFile)
		throw CException(ERR_FAIL);

	DWORD dwCurPos = ::SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
	if (dwCurPos == INVALID_SET_FILE_POINTER)
		return;

	m_dwSessionStart = dwCurPos;
	}

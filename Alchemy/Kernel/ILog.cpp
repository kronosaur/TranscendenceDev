//	ILog.cpp
//
//	ILog Class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

void ILog::LogOutput (DWORD dwFlags, const CString &sLine) const

//	LogOutput
//
//	Output a line to the log

	{
	//	Write the time date

	if (dwFlags & ILOG_FLAG_TIMEDATE)
		{
		SYSTEMTIME time;

		GetLocalTime(&time);
		Print(strPatternSubst(CONSTLIT("%04d-%02d-%02d %02d:%02d:%02d\t%s"),
				time.wYear,
				time.wMonth,
				time.wDay,
				time.wHour,
				time.wMinute,
				time.wSecond,
				sLine));
		}
	else
		Print(sLine);
	}

void ILog::LogOutput (DWORD dwFlags, char *pszLine, ...) const

//	LogOutput
//
//	Output a line to the log

	{
	CString sParsedLine;

	char *pArgs = (char *)&pszLine + sizeof(pszLine);
	sParsedLine = strPattern(CString(pszLine, ::strlen(pszLine), TRUE), (void **)pArgs);

	LogOutput(dwFlags, sParsedLine);
	}


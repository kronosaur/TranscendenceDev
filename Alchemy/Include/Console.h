//	Console.h
//
//	Console classes
//	Copyright 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	CConsoleFormat -------------------------------------------------------------

class CConsoleFormat
	{
	public:
		static CString CenterLine (const CString &sLine, int iCols = 80);
		static int WrapText (const CString &sText, TArray<CString> &retLines, int iCols = 80);
	};


//	IHIController.cpp
//
//	IHIController class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "stdafx.h"

CHumanInterface &IHIController::CreateHI (void)

//	CreateHI
//
//	Returns a valid HI

	{
	CHumanInterface::Create();
	return *g_pHI;
	}

ALERROR IHIController::OnBoot (char *pszCommandLine, SHIOptions *retOptions, CString *retsError)

//	OnBoot
//
//	This is the default booting behavior

	{
	return NOERROR;
	}

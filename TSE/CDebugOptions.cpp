//	CDebugOptions.cpp
//
//	CDebugOptions class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_DEBUG_MODE					CONSTLIT("debugMode")
#define PROPERTY_SHOW_NAV_PATHS				CONSTLIT("showNavPaths")

#define ERR_MUST_BE_IN_DEBUG_MODE			CONSTLIT("Must be in debug mode to set a debug property.")

ICCItemPtr CDebugOptions::GetProperty (const CString &sProperty) const

//	GetProperty
//
//	Returns a property

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sProperty, PROPERTY_DEBUG_MODE))
		return ICCItemPtr(CC.CreateBool(g_pUniverse->InDebugMode()));

	else if (strEquals(sProperty, PROPERTY_SHOW_NAV_PATHS))
		return ICCItemPtr(CC.CreateBool(m_bShowNavPaths));

	else
		return ICCItemPtr(CC.CreateNil());
	}

bool CDebugOptions::SetProperty (const CString &sProperty, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets a property. If the property is not found, we return FALSE with a blank
//	error. If we have an error, we returns FALSE with a valid error string.
//	Otherwise, we return TRUE.

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	//	NOTE: This only works in debug mode.

	if (!g_pUniverse->InDebugMode())
		{
		if (retsError) *retsError = ERR_MUST_BE_IN_DEBUG_MODE;
		return false;
		}

	//	Set a property

	if (strEquals(sProperty, PROPERTY_SHOW_NAV_PATHS))
		m_bShowNavPaths = !pValue->IsNil();

	else
		{
		if (retsError) *retsError = NULL_STR;
		return false;
		}

	return true;
	}

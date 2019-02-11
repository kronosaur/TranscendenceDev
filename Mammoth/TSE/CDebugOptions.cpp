//	CDebugOptions.cpp
//
//	CDebugOptions class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_DEBUG_MODE					CONSTLIT("debugMode")
#define PROPERTY_MEMORY_USE					CONSTLIT("memoryUse")
#define PROPERTY_SHOW_AI_DEBUG				CONSTLIT("showAIDebug")
#define PROPERTY_SHOW_BOUNDS				CONSTLIT("showBounds")
#define PROPERTY_SHOW_FACINGS_ANGLE			CONSTLIT("showFacingsAngle")
#define PROPERTY_SHOW_LINE_OF_FIRE			CONSTLIT("showLineOfFire")
#define PROPERTY_SHOW_NAV_PATHS				CONSTLIT("showNavPaths")
#define PROPERTY_SHOW_NODE_INFO				CONSTLIT("showNodeInfo")

#define ERR_MUST_BE_IN_DEBUG_MODE			CONSTLIT("Must be in debug mode to set a debug property.")

ICCItemPtr CDebugOptions::GetMemoryUse (void) const

//	GetMemoryUse
//
//	Returns current memory stats

	{
	ICCItemPtr pResult = ICCItemPtr(ICCItem::SymbolTable);

	//	Design collection stats

	CDesignCollection::SStats Stats;
	g_pUniverse->GetDesignCollection().GetStats(Stats);

	pResult->SetIntegerAt(CONSTLIT("graphicsTotal"), (int)(DWORD)Stats.dwGraphicsMemory);
	pResult->SetIntegerAt(CONSTLIT("graphicsWrecks"), (int)(DWORD)Stats.dwWreckGraphicsMemory);
	pResult->SetIntegerAt(CONSTLIT("XML"), (int)(DWORD)Stats.dwTotalXMLMemory);

	return pResult;
	}

ICCItemPtr CDebugOptions::GetProperty (const CString &sProperty) const

//	GetProperty
//
//	Returns a property

	{
	if (strEquals(sProperty, PROPERTY_MEMORY_USE))
		return GetMemoryUse();

	else if (strEquals(sProperty, PROPERTY_DEBUG_MODE))
		return ICCItemPtr(g_pUniverse->InDebugMode());

	else if (strEquals(sProperty, PROPERTY_SHOW_AI_DEBUG))
		return ICCItemPtr(m_bShowAIDebug);

	else if (strEquals(sProperty, PROPERTY_SHOW_BOUNDS))
		return ICCItemPtr(m_bShowBounds);

	else if (strEquals(sProperty, PROPERTY_SHOW_FACINGS_ANGLE))
		return ICCItemPtr(m_bShowFacingsAngle);

	else if (strEquals(sProperty, PROPERTY_SHOW_LINE_OF_FIRE))
		return ICCItemPtr(m_bShowLineOfFire);

	else if (strEquals(sProperty, PROPERTY_SHOW_NAV_PATHS))
		return ICCItemPtr(m_bShowNavPaths);

	else if (strEquals(sProperty, PROPERTY_SHOW_NODE_INFO))
		return ICCItemPtr(m_bShowNodeAttributes);

	else
		return ICCItemPtr(ICCItem::Nil);
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

	if (strEquals(sProperty, PROPERTY_SHOW_AI_DEBUG))
		m_bShowAIDebug = !pValue->IsNil();

	else if (strEquals(sProperty, PROPERTY_SHOW_BOUNDS))
		m_bShowBounds = !pValue->IsNil();

	else if (strEquals(sProperty, PROPERTY_SHOW_FACINGS_ANGLE))
		m_bShowFacingsAngle = !pValue->IsNil();

	else if (strEquals(sProperty, PROPERTY_SHOW_LINE_OF_FIRE))
		m_bShowLineOfFire = !pValue->IsNil();

	else if (strEquals(sProperty, PROPERTY_SHOW_NAV_PATHS))
		m_bShowNavPaths = !pValue->IsNil();

	else if (strEquals(sProperty, PROPERTY_SHOW_NODE_INFO))
		m_bShowNodeAttributes = !pValue->IsNil();

	else
		{
		if (retsError) *retsError = NULL_STR;
		return false;
		}

	return true;
	}

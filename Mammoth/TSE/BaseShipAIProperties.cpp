//	BaseShipAIProperties.cpp
//
//	CBaseShipAI class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TPropertyHandler<CBaseShipAI> CBaseShipAI::m_PropertyTable = std::array<TPropertyHandler<CBaseShipAI>::SPropertyDef, 1> {{
		{
		"waitingForShields",			"True/Nil is we're waiting for shields to regenerate",
		[](const CBaseShipAI &BaseShipAI, const CString &sProperty) 
			{
			return ICCItemPtr(BaseShipAI.m_AICtx.IsWaitingForShieldsToRegen());
			},
		[](CBaseShipAI &BaseShipAI, const CString &sProperty, const ICCItem &Value, CString *retsError)
			{
			BaseShipAI.m_AICtx.SetWaitingForShieldsToRegen(!Value.IsNil());
			return true;
			},
		},

	}};

ICCItem *CBaseShipAI::FindProperty (const CString &sProperty)

//	FindProperty
//
//	Returns the property (or NULL). If non-null, callers must free the pointer.

	{
	//	First look for a property in our table.

	ICCItemPtr pValue;
	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue->Reference();

	//	Otherwise, not found

	else
		return NULL;
	}

ESetPropertyResult CBaseShipAI::SetProperty (const CString &sProperty, const ICCItem &Value, CString *retsError)

//	SetProperty
//
//	Sets a property.

	{
	CString sError;
	if (!m_PropertyTable.SetProperty(*this, sProperty, Value, &sError))
		{
		if (sError.IsBlank())
			return ESetPropertyResult::notFound;
		else
			{
			if (retsError)
				*retsError = sError;
			return ESetPropertyResult::error;
			}
		}

	return ESetPropertyResult::set;
	}

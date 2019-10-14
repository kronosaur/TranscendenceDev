//	CDockSession.cpp
//
//	CDockSession class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TPropertyHandler<CDockSession> CDockSession::m_PropertyTable = std::array<TPropertyHandler<CDockSession>::SPropertyDef, 1> {
	{	"foo",
		"Test.",
		[](const CDockSession &DockSession, const CString &sProperty) { return ICCItemPtr(ICCItem::Nil); },
		[](CDockSession &DockSession, const CString &sProperty, const ICCItem &Value, CString *retsError) { return false; }
		},
	};

ICCItemPtr CDockSession::GetProperty (const CString &sProperty) const

//	GetProperty
//
//	Returns a property. We guarantee that the result is non-null.

	{
	ICCItemPtr pValue;

	//	First look for built-in properties

	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;

	//	Next check to see if the screen handles this property.

	else if (pValue = GetUI().GetProperty(sProperty))
		return pValue;

	//	Now look up user-defined properties

	return ICCItemPtr(ICCItem::Nil);
	}

bool CDockSession::SetProperty (const CString &sProperty, const ICCItem &Value, CString *retsError)

//	SetProperty
//
//	Sets a property. If we fail, we return FALSE and retsError is the error.

	{
	CString sError;

	//	First look for built-in properies

	if (m_PropertyTable.SetProperty(*this, sProperty, Value, &sError))
		return true;
	else if (!sError.IsBlank())
		{
		if (retsError) *retsError = sError;
		return false;
		}

	//	Next check to see if the screen handles the property.

	else if (GetUI().SetProperty(sProperty, Value))
		return true;

	//	Look up user-defined properties.

	//	Not found

	if (retsError)
		*retsError = strPatternSubst(CONSTLIT("Unknown property: %s"), sProperty);

	return false;
	}

//	SystemTypeProperties.cpp
//
//	CSystemType class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TPropertyHandler<CSystemType> CSystemType::m_PropertyTable = std::array<TPropertyHandler<CSystemType>::SPropertyDef, 1> {{
		{
		"name",					"name of template",
		[](const CSystemType &Type, const CString &sProperty) 
			{
			return ICCItemPtr(Type.m_sName);
			},
		NULL,
		},
	}};

ICCItemPtr CSystemType::OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	OnGetProperty
//
//	Returns a property.

	{
	ICCItemPtr pValue;
	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;

	//	Otherwise, not found

	else
		return NULL;
	}

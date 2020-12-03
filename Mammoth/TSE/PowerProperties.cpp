//	PowerProperties.cpp
//
//	CPower class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TPropertyHandler<CPower> CPower::m_PropertyTable = std::array<TPropertyHandler<CPower>::SPropertyDef, 2> {{
		{
		"imageDesc",		"imageDesc for the power",
		[](const CPower &Type, const CString &sProperty) 
			{
			return ICCItemPtr(CreateListFromImage(Type.GetUniverse().GetCC(), Type.GetImage()));
			},
		NULL,
		},

		{
		"name",				"Name of power",
		[](const CPower &Type, const CString &sProperty) 
			{
			return ICCItemPtr(Type.GetName());
			},
		NULL,
		},
	}};

ICCItemPtr CPower::OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

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


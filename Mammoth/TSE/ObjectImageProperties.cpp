//	CObjectImage.cpp
//
//	CObjectImage class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TPropertyHandler<CObjectImage> CObjectImage::m_PropertyTable = std::array<TPropertyHandler<CObjectImage>::SPropertyDef, 1> {{
		{
		"memoryUsageKB",		"Current memory usage, in KB",
		[](const CObjectImage &Type, const CString &sProperty) 
			{
			size_t dwUsage = Type.GetMemoryUsage();
			if (dwUsage == 0)
				return ICCItemPtr::Nil();
			else
				return ICCItemPtr(mathRound((DWORD)dwUsage / 1024.0));
			},
		NULL,
		},
	}};

ICCItemPtr CObjectImage::OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	OnGetProperty
//
//	Returns the property.

	{
	ICCItemPtr pValue;
	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;

	//	Otherwise, not found

	else
		return NULL;
	}

//	SystemProperties.cpp
//
//	CSystem class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TPropertyHandler<CSystem> CSystem::m_PropertyTable = std::array<TPropertyHandler<CSystem>::SPropertyDef, 3> {{
		{
		"randomEncounterObjTable",			"table of random encounters",
		[](const CSystem &System, const CString &sProperty) 
			{
			if (System.HasRandomEncounters())
				return System.GetRandomEncounterObjTable().AsCCItem(System); 
			else
				return ICCItemPtr::Nil();
			},
		NULL,
		},

		{
		"randomEncounterTypeTable",			"table of random encounters",
		[](const CSystem &System, const CString &sProperty) 
			{
			if (System.HasRandomEncounters())
				return System.GetRandomEncounterTypeTable().AsCCItem(System);
			else
				return ICCItemPtr::Nil();
			},
		NULL,
		},

		{
		"type",			"UNID",
		[](const CSystem &System, const CString &sProperty) 
			{
			if (const CSystemType *pType = System.GetType())
				return ICCItemPtr(pType->GetUNID());
			else
				return ICCItemPtr::Nil();
			},
		NULL,
		}
	}};

ICCItemPtr CSystem::GetProperty (CCodeChainCtx &CCX, const CString &sProperty) const

//	GetProperty
//
//	Returns the given property.

	{
	ICCItemPtr pValue;

	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;
	else if (m_pTopology)
		return m_pTopology->GetProperty(sProperty);
	else
		return ICCItemPtr::Nil();
	}

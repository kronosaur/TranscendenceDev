//	CStationEncounterOverrideTable.cpp
//
//	CStationEncounterOverrideTable class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define UNID_ATTRIB								CONSTLIT("UNID")

const CStationEncounterDesc &CStationEncounterOverrideTable::GetEncounterDesc (const CStationType &Type) const

//	GetEncounterDesc
//
//	Returns the encounter desc for the given station type.

	{
	//	Look for the descriptor in the table. If we have it, then we have an 
	//	override.

	auto *pDesc = m_Table.GetAt(Type.GetUNID());
	if (pDesc)
		return *pDesc;

	//	Otherwise, we return the original descriptor for the type.

	return Type.GetEncounterDesc();
	}

bool CStationEncounterOverrideTable::InitFromXML (CDesignCollection &Design, const CXMLElement &OverrideTableXML, CString *retsError)

//	InitFromXML
//
//	Initializes a table of encounter overrides.

	{
	SDesignLoadCtx LoadCtx;

	//	Loop over all overrides

	for (int i = 0; i < OverrideTableXML.GetContentElementCount(); i++)
		{
		const CXMLElement *pOverride = OverrideTableXML.GetContentElement(i);

		//	Get the UNID that we're overriding

		DWORD dwUNID;
		if (::LoadUNID(LoadCtx, pOverride->GetAttribute(UNID_ATTRIB), &dwUNID) != NOERROR)
			{
			if (retsError) *retsError = LoadCtx.sError;
			return false;
			}

		if (dwUNID == 0)
			{
			if (retsError) *retsError = CONSTLIT("Encounter override must specify UNID to override.");
			return false;
			}

		//	Get the station type. If we don't find the station, skip it. We 
		//	assume that this is an optional type.


		const CStationType *pType = CStationType::AsType(Design.FindEntry(dwUNID));
		if (pType == NULL)
			{
			if (Design.GetUniverse().InDebugMode())
				Design.GetUniverse().LogOutput(strPatternSubst("Skipping encounter override %08x because type is not found.", dwUNID));
			continue;
			}

		//	Add to table

		CStationEncounterDesc *pDesc = m_Table.SetAt(pType->GetUNID());
		if (!pDesc->InitAsOverride(pType->GetEncounterDesc(), *pOverride, retsError))
			return false;
		}

	return true;
	}

//	CExtensionListMap.cpp
//
//	CExtensionListMap class
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#include "stdafx.h"

#define DISABLED_TAG						CONSTLIT("Disabled")
#define ENABLED_TAG							CONSTLIT("Enabled")

#define DEBUG_MODE_ATTRIB					CONSTLIT("debugMode")
#define UNID_ATTRIB							CONSTLIT("unid")

void CExtensionListMap::GetList (DWORD dwAdventure, const TArray<CExtension *> &Available, bool bDebugMode, TArray<DWORD> *retList) const

//	GetList
//
//	Returns a list

	{
	int i;

	retList->DeleteAll();

	SEntry *pEntry = m_Map.GetAt(dwAdventure);
	if (pEntry)
		{
		TSortMap<DWORD, bool> *pSource = (bDebugMode ? &pEntry->DebugList : &pEntry->List);

		//	Look up each available extension in our list

		for (i = 0; i < Available.GetCount(); i++)
			{
			bool *pEnabled = pSource->GetAt(Available[i]->GetUNID());

			//	If we don't know about this extension, then it means that it is 
			//	a new extension, so we enabled it by default unless we're in 
			//	compatibility mode.

			if (pEnabled == NULL)
				{
				if (Available[i]->IsRegistered())
					{
					if (bDebugMode
							&& !pEntry->m_bDisabledIfNotInDebugList)
						retList->Insert(Available[i]->GetUNID());
					else if (!bDebugMode
							&& !pEntry->m_bDisabledIfNotInList)
						retList->Insert(Available[i]->GetUNID());
					}
				}

			//	Otherwise, we enabled this extension if that's the default

			else if (*pEnabled)
				retList->Insert(Available[i]->GetUNID());
			}
		}

	//	If we don't know about this adventure, then we enabled all registered
	//	extensions by default.

	else
		{
		for (i = 0; i < Available.GetCount(); i++)
			if (Available[i]->IsRegistered())
				retList->Insert(Available[i]->GetUNID());
		}
	}

ALERROR CExtensionListMap::ReadFromXML (CXMLElement *pDesc)

//	ReadFromXML
//
//	Loads it from XML.

	{
	int i;

	m_Map.DeleteAll();

	//	Loop over all lists

	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pEntry = pDesc->GetContentElement(i);
		DWORD dwAdventure = (DWORD)pEntry->GetAttributeInteger(UNID_ATTRIB);
		bool bDebugMode = pEntry->GetAttributeBool(DEBUG_MODE_ATTRIB);

		SEntry *pNewEntry = m_Map.SetAt(dwAdventure);
		TSortMap<DWORD, bool> *pDest = (bDebugMode ? &pNewEntry->DebugList : &pNewEntry->List);

		//	If we have sub-elements, then this is the new format

		if (pEntry->GetContentElementCount() > 0)
			{
			CXMLElement *pEnabled = pEntry->GetContentElementByTag(ENABLED_TAG);
			if (pEnabled)
				{
				if (ReadList(pEnabled->GetContentText(0), true, pDest) != NOERROR)
					return ERR_FAIL;
				}

			CXMLElement *pDisabled = pEntry->GetContentElementByTag(DISABLED_TAG);
			if (pDisabled)
				{
				if (ReadList(pDisabled->GetContentText(0), false, pDest, (bDebugMode ? &pNewEntry->m_bDisabledIfNotInDebugList : &pNewEntry->m_bDisabledIfNotInList)) != NOERROR)
					return ERR_FAIL;
				}
			}

		//	Otherwise, we load backwards compatibility mode

		else
			{
			if (ReadList(pEntry->GetContentText(0), true, pDest) != NOERROR)
				return ERR_FAIL;

			//	In backwards compatibility mode, any extension not in our list
			//	is disabled by default.

			if (bDebugMode)
				pNewEntry->m_bDisabledIfNotInDebugList = true;
			else
				pNewEntry->m_bDisabledIfNotInList = true;
			}
		}

	return NOERROR;
	}

ALERROR CExtensionListMap::ReadList (const CString &sList, bool bEnabled, TSortMap<DWORD, bool> *retpList, bool *retbDisabledIfNotInList)

//	ReadList
//
//	Reads a list of UNIDs.

	{
	int i;

	if (retbDisabledIfNotInList)
		{
		if (strEquals(sList, CONSTLIT("*")))
			{
			*retbDisabledIfNotInList = true;
			return NOERROR;
			}
		else
			*retbDisabledIfNotInList = false;
		}

	//	Read the UNIDs

	TArray<CString> Values;
	ParseStringList(sList, 0, &Values);

	for (i = 0; i < Values.GetCount(); i++)
		retpList->SetAt(strToInt(Values[i], 0), bEnabled);

	return NOERROR;
	}

void CExtensionListMap::SetList (DWORD dwAdventure, const TArray<CExtension *> &Available, bool bDebugMode, const TArray<DWORD> &List)

//	SetList
//
//	Sets a list

	{
	int i;

	SEntry *pNewEntry = m_Map.SetAt(dwAdventure);
	
	//	Since we're setting the list explicitly, any new extensions are ones that
	//	we haven't yet seen, so we can enabled those by default.
	//
	//	[These flags are only for compatibility with a previous version which did
	//	not store unselected extensions.]

	if (bDebugMode)
		pNewEntry->m_bDisabledIfNotInDebugList = false;
	else
		pNewEntry->m_bDisabledIfNotInList = false;

	//	Get the list to used

	TSortMap<DWORD, bool> *pDest = (bDebugMode ? &pNewEntry->DebugList : &pNewEntry->List);
	pDest->DeleteAll();

	//	Set all the extensions as disabled

	for (i = 0; i < Available.GetCount(); i++)
		pDest->SetAt(Available[i]->GetUNID(), false);

	//	Set the enabled extension

	for (i = 0; i < List.GetCount(); i++)
		pDest->SetAt(List[i], true);
	}

ALERROR CExtensionListMap::WriteAsXML (IWriteStream *pOutput)

//	WriteAsXML
//
//	Writes to XML

	{
	ALERROR error;
	int i;

	//	Open tag

	CString sData;
	sData = CONSTLIT("\t<Extensions>\r\n");
	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength(), NULL))
		return error;

	//	Entries

	for (i = 0; i < m_Map.GetCount(); i++)
		{
		DWORD dwAdventure = m_Map.GetKey(i);
		const SEntry &Entry = m_Map[i];

		//	Write the non-debug

		if (Entry.List.GetCount() > 0)
			{
			if (error = WriteList(pOutput, dwAdventure, false, Entry.List, Entry.m_bDisabledIfNotInList))
				return error;
			}

		//	Write the debug list

		if (Entry.DebugList.GetCount() > 0)
			{
			if (error = WriteList(pOutput, dwAdventure, true, Entry.DebugList, Entry.m_bDisabledIfNotInDebugList))
				return error;
			}
		}

	//	Close tag

	sData = CONSTLIT("\t</Extensions>\r\n");
	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength(), NULL))
		return error;

	return NOERROR;
	}

ALERROR CExtensionListMap::WriteList (IWriteStream *pOutput, DWORD dwAdventure, bool bDebugMode, const TSortMap<DWORD, bool> &List, bool bDisabledIfNotInList)

//	WriteList
//
//	Writes a single list

	{
	ALERROR error;
	int i;

	CString sData;
	if (bDebugMode)
		sData = strPatternSubst(CONSTLIT("\t\t<Default unid=\"0x%x\" debugMode=\"true\">"), dwAdventure);
	else
		sData = strPatternSubst(CONSTLIT("\t\t<Default unid=\"0x%x\">"), dwAdventure);

	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength()))
		return error;

	//	First we write out the list of enabled extensions

	sData = CONSTLIT("<Enabled>");
	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength()))
		return error;

	bool bFirstEntry = true;
	for (i = 0; i < List.GetCount(); i++)
		{
		if (List.GetValue(i))
			{
			CString sEntry;
			if (bFirstEntry)
				{
				sEntry = strPatternSubst(CONSTLIT("0x%x"), List.GetKey(i));
				bFirstEntry = false;
				}
			else
				sEntry = strPatternSubst(CONSTLIT(", 0x%x"), List.GetKey(i));

			if (error = pOutput->Write(sEntry.GetPointer(), sEntry.GetLength()))
				return error;
			}
		}

	sData = CONSTLIT("</Enabled>");
	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength()))
		return error;

	//	Now we write the disabled extensions

	sData = CONSTLIT("<Disabled>");
	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength()))
		return error;

	if (bDisabledIfNotInList)
		{
		sData = CONSTLIT("*");
		if (error = pOutput->Write(sData.GetPointer(), sData.GetLength()))
			return error;
		}
	else
		{
		bFirstEntry = true;
		for (i = 0; i < List.GetCount(); i++)
			{
			if (!List.GetValue(i))
				{
				CString sEntry;
				if (bFirstEntry)
					{
					sEntry = strPatternSubst(CONSTLIT("0x%x"), List.GetKey(i));
					bFirstEntry = false;
					}
				else
					sEntry = strPatternSubst(CONSTLIT(", 0x%x"), List.GetKey(i));

				if (error = pOutput->Write(sEntry.GetPointer(), sEntry.GetLength()))
					return error;
				}
			}
		}

	sData = CONSTLIT("</Disabled>");
	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength()))
		return error;

	//	Done

	sData = CONSTLIT("</Default>\r\n");
	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength()))
		return error;

	return NOERROR;
	}

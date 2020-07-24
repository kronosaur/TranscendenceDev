//	CGameStats.cpp
//
//	CGameStats object
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const DWORD GAME_STAT_VERSION =				1;

#define FIELD_DESC								CONSTLIT("desc")
#define FIELD_DESC_ID							CONSTLIT("descID")
#define FIELD_SECTION							CONSTLIT("section")
#define FIELD_SECTION_ID						CONSTLIT("sectionID")
#define FIELD_SORT								CONSTLIT("sort")
#define FIELD_VALUE								CONSTLIT("value")
#define FIELD_VALUE_ID							CONSTLIT("valueID")

void CGameStats::GetEntry (int iIndex, CString *retsStatName, CString *retsStatValue, CString *retsSection) const

//	GetEntry
//
//	Returns the given stat

	{
	if (retsStatName)
		*retsStatName = m_Stats[iIndex].sStatName;

	if (retsStatValue)
		*retsStatValue = m_Stats[iIndex].sStatValue;

	if (retsSection)
		{
		//	Look for the separator

		char *pStart = m_Stats[iIndex].sSortKey.GetASCIIZPointer();
		char *pPos = pStart;
		while (*pPos != '/' && *pPos != '\0')
			pPos++;

		if (*pPos == '/')
			*retsSection = CString(pStart, pPos - pStart);
		else
			*retsSection = NULL_STR;
		}
	}

CString CGameStats::GetTextValue (CDesignType &Type, const CString &sIDField, const CString &sTextField, const ICCItem &Entry)

//	GetTextValue
//
//	Looks for sIDField in pEntry; if found, we lookup the language element in 
//	pType. Otherwise, we get the value of the sTextField in pEntry.

	{
	//	If we have an ID to a language entry, use that.

	const ICCItem *pItem = Entry.GetElement(sIDField);
	if (pItem && !pItem->IsNil())
		{
		//	NOTE: We pass the entry as data so that it can be used by the
		//	translation code.

		CString sText;
		if (!Type.TranslateText(pItem->GetStringValue(), &Entry, &sText))
			return NULL_STR;

		return sText;
		}

	//	Otherwise, we expect the text to be in a field.

	pItem = Entry.GetElement(sTextField);
	if (pItem == NULL || pItem->IsNil())
		return NULL_STR;
	else if (pItem->IsInteger())
		return strFormatInteger(pItem->GetIntegerValue(), -1, FORMAT_THOUSAND_SEPARATOR | FORMAT_UNSIGNED);
	else
		return pItem->GetStringValue();
	}

void CGameStats::Insert (CDesignType &Type, const ICCItem &Achievement)

//	Insert
//
//	Insert a ICCItem stat

	{
	if (Achievement.IsNil())
		;
	else if (Achievement.IsSymbolTable())
		{
		CString sName = GetTextValue(Type, FIELD_DESC_ID, FIELD_DESC, Achievement);
		CString sValue = GetTextValue(Type, FIELD_VALUE_ID, FIELD_VALUE, Achievement);
		CString sSection = GetTextValue(Type, FIELD_SECTION_ID, FIELD_SECTION, Achievement);
		CString sSort = ParseAchievementSort(Achievement.GetElement(FIELD_SORT));

		if (!sName.IsBlank())
			Insert(sName, sValue, sSection, sSort);
		}
	else if (Achievement.GetCount() > 0)
		{
		CString sName = Achievement.GetElement(0)->GetStringValue();
		CString sValue = ParseAchievementValue(Achievement.GetElement(1));
		CString sSection = ParseAchievementSection(Achievement.GetElement(2));
		CString sSort = ParseAchievementSort(Achievement.GetElement(3));

		if (!sName.IsBlank())
			Insert(sName, sValue, sSection, sSort);
		}
	}

void CGameStats::Insert (const CString &sStatName, const CString &sStatValue, const CString &sSection, const CString &sSortKey)

//	Insert
//
//	Insert a stat

	{
	ASSERT(!sStatName.IsBlank());

	SStat *pNew = m_Stats.Insert();
	pNew->sStatName = sStatName;
	pNew->sStatValue = sStatValue;

	//	We store the section name in the sort key (OK if sSection is blank)

	pNew->sSortKey = strPatternSubst(CONSTLIT("%s/%s"),
			sSection,
			(sSortKey.IsBlank() ? sStatName : sSortKey));
	}

void CGameStats::InsertFromCCItem (CDesignType &Type, const ICCItem &Entry)

//	InsertFromCCItem
//
//	Inserts from a value returned by <GetGlobalAchievements>.

	{
	if (Entry.IsNil())
		;

	else if (Entry.IsSymbolTable())
		Insert(Type, Entry);

	else if (Entry.IsList() && Entry.GetCount() > 0)
		{
		//	If we have a list of lists, then we have 
		//	a list of achievements

		if (Entry.GetElement(0)->IsList() || Entry.GetElement(0)->IsSymbolTable())
			{
			for (int i = 0; i < Entry.GetCount(); i++)
				Insert(Type, *Entry.GetElement(i));
			}

		//	Otherwise, we have a single achievement

		else
			Insert(Type, Entry);
		}
	}

ALERROR CGameStats::LoadFromStream (IReadStream *pStream)

//	LoadFromStream
//
//	DWORD		game stats version
//	CString		m_sDefaultSectionName
//	DWORD		No of entries
//	CString			sSortKey
//	CString			sStatName
//	CString			sStatValue

	{
	int i;
	DWORD dwLoad;

	DWORD dwVersion;
	pStream->Read((char *)&dwVersion, sizeof(DWORD));
	if (dwVersion > GAME_STAT_VERSION)
		return ERR_FAIL;

	DeleteAll();

	m_sDefaultSectionName.ReadFromStream(pStream);

	pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_Stats.InsertEmpty(dwLoad);

	for (i = 0; i < (int)dwLoad; i++)
		{
		m_Stats[i].sSortKey.ReadFromStream(pStream);
		m_Stats[i].sStatName.ReadFromStream(pStream);
		m_Stats[i].sStatValue.ReadFromStream(pStream);
		}

	return NOERROR;
	}

CString CGameStats::ParseAchievementSection (const ICCItem *pItem)
	{
	if (pItem == NULL)
		return NULL_STR;
	else if (pItem->IsNil())
		return NULL_STR;
	else
		return pItem->GetStringValue();
	}

CString CGameStats::ParseAchievementSort (const ICCItem *pItem)
	{
	if (pItem == NULL)
		return NULL_STR;
	else if (pItem->IsNil())
		return NULL_STR;
	else if (pItem->IsInteger())
		return strPatternSubst(CONSTLIT("%08x"), pItem->GetIntegerValue());
	else
		return pItem->GetStringValue();
	}

CString CGameStats::ParseAchievementValue (const ICCItem *pItem)
	{
	if (pItem == NULL)
		return NULL_STR;
	else if (pItem->IsNil())
		return NULL_STR;
	else if (pItem->IsInteger())
		return strFormatInteger(pItem->GetIntegerValue(), -1, FORMAT_THOUSAND_SEPARATOR | FORMAT_UNSIGNED);
	else
		return pItem->GetStringValue();
	}
	
void CGameStats::ParseSortKey (const CString &sSortKey, CString *retsSection, CString *retsSectionSortKey)

//	ParseSortKey
//
//	Separates sSortKey into a section and section sort key.

	{
	char *pPos = sSortKey.GetASCIIZPointer();

	char *pStart = pPos;
	while (*pPos != '/' && *pPos != '\0')
		pPos++;

	if (retsSection)
		*retsSection = CString(pStart, (int)(pPos - pStart));

	//	key

	if (*pPos == '/')
		pPos++;

	if (retsSectionSortKey)
		*retsSectionSortKey = CString(pPos);
	}

void CGameStats::SaveToJSON (CJSONValue *retOutput) const

//	SaveToJSON
//
//	Save as a JSON object. The output is an array of stats. Each stat is an 
//	array with the following elements:
//
//	1.	Name of stat
//	2.	Value of stat (or nulll)
//	3.	Section (or null)
//	4.	Sort key (or null)

	{
	int i;

	*retOutput = CJSONValue(CJSONValue::typeArray);

	for (i = 0; i < m_Stats.GetCount(); i++)
		{
		CJSONValue Stat(CJSONValue::typeArray);

		//	Add the stats name

		Stat.Insert(CJSONValue(m_Stats[i].sStatName));

		//	Parse the value

		int iValue;
		if (m_Stats[i].sStatValue.IsBlank())
			Stat.Insert(CJSONValue(CJSONValue::typeNull));
		else if (strIsInt(m_Stats[i].sStatValue, PARSE_THOUSAND_SEPARATOR, &iValue))
			Stat.Insert(CJSONValue(iValue));
		else
			Stat.Insert(CJSONValue(m_Stats[i].sStatValue));

		//	Split the section out of the sort key

		CString sSection;
		CString sSectionSortKey;
		ParseSortKey(m_Stats[i].sSortKey, &sSection, &sSectionSortKey);

		if (sSection.IsBlank())
			Stat.Insert(CJSONValue(CJSONValue::typeNull));
		else
			Stat.Insert(CJSONValue(sSection));

		//	Add the sort key

		if (!strEquals(sSectionSortKey, m_Stats[i].sStatName))
			Stat.Insert(CJSONValue(sSectionSortKey));

		//	Append to large array

		retOutput->InsertHandoff(Stat);
		}
	}

ALERROR CGameStats::SaveToStream (IWriteStream *pStream) const

//	SaveToStream
//
//	DWORD		game stats version
//	CString		m_sDefaultSectionName
//	DWORD		No of entries
//	CString			sSortKey
//	CString			sStatName
//	CString			sStatValue

	{
	int i;
	DWORD dwSave;

	dwSave = GAME_STAT_VERSION;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	m_sDefaultSectionName.WriteToStream(pStream);

	dwSave = m_Stats.GetCount();
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	for (i = 0; i < m_Stats.GetCount(); i++)
		{
		m_Stats[i].sSortKey.WriteToStream(pStream);
		m_Stats[i].sStatName.WriteToStream(pStream);
		m_Stats[i].sStatValue.WriteToStream(pStream);
		}

	return NOERROR;
	}

void CGameStats::TakeHandoff (CGameStats &Source)

//	TakeHandoff
//
//	Takes a handoff of the underlying array

	{
	m_Stats.TakeHandoff(Source.m_Stats);
	m_sDefaultSectionName = Source.m_sDefaultSectionName;
	}

ALERROR CGameStats::WriteAsText (IWriteStream *pOutput) const

//	WriteAsText
//
//	Outputs game stats as text suitable for clipboard of text file

	{
	ALERROR error;
	int i;

	CString sPrevSection = CONSTLIT("(start)");
	for (i = 0; i < GetCount(); i++)
		{
		//	Get the stats

		CString sName;
		CString sValue;
		CString sSection;
		GetEntry(i, &sName, &sValue, &sSection);

		//	If necessary, add a new section header

		if (!strEquals(sSection, sPrevSection))
			{
			if (i != 0)
				if (error = pOutput->Write("\r\n", 2))
					return error;

			CString sSectionUpper = (sSection.IsBlank() ? ::strToUpper(GetDefaultSectionName()) : ::strToUpper(sSection));
			if (error = pOutput->Write(sSectionUpper.GetASCIIZPointer(), sSectionUpper.GetLength()))
				return error;

			if (error = pOutput->Write("\r\n\r\n", 4))
				return error;

			sPrevSection = sSection;
			}

		//	Add the two lines (with a tab separator)

		if (pOutput->Write(sName.GetASCIIZPointer(), sName.GetLength()))
			return error;
		if (pOutput->Write("\t", 1))
			return error;
		if (pOutput->Write(sValue.GetASCIIZPointer(), sValue.GetLength()))
			return error;
		if (pOutput->Write("\r\n", 2))
			return error;
		}

	return NOERROR;
	}

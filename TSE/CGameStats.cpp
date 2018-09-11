//	CGameStats.cpp
//
//	CGameStats object

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

CString CGameStats::GetTextValue (CDesignType *pType, const CString &sIDField, const CString &sTextField, ICCItem *pEntry)

//	GetTextValue
//
//	Looks for sIDField in pEntry; if found, we lookup the language element in 
//	pType. Otherwise, we get the value of the sTextField in pEntry.

	{
	//	If we have an ID to a language entry, use that.

	ICCItem *pItem = pEntry->GetElement(sIDField);
	if (pItem && !pItem->IsNil())
		{
		CString sText;
		if (!pType->TranslateText(NULL, pItem->GetStringValue(), NULL, &sText))
			return NULL_STR;

		return sText;
		}

	//	Otherwise, we expect the text to be in a field.

	pItem = pEntry->GetElement(sTextField);
	if (pItem == NULL || pItem->IsNil())
		return NULL_STR;
	else if (pItem->IsInteger())
		return strFormatInteger(pItem->GetIntegerValue(), -1, FORMAT_THOUSAND_SEPARATOR | FORMAT_UNSIGNED);
	else
		return pItem->GetStringValue();
	}

void CGameStats::Insert (CDesignType *pType, ICCItem *pAchievement)

//	Insert
//
//	Insert a ICCItem stat

	{
	if (pAchievement == NULL || pAchievement->IsNil())
		;
	else if (pAchievement->IsSymbolTable())
		{
		CString sName = GetTextValue(pType, FIELD_DESC_ID, FIELD_DESC, pAchievement);
		CString sValue = GetTextValue(pType, FIELD_VALUE_ID, FIELD_VALUE, pAchievement);
		CString sSection = GetTextValue(pType, FIELD_SECTION_ID, FIELD_SECTION, pAchievement);
		CString sSort = ParseAchievementSort(pAchievement->GetElement(FIELD_SORT));

		if (!sName.IsBlank())
			Insert(sName, sValue, sSection, sSort);
		}
	else if (pAchievement->GetCount() > 0)
		{
		CString sName = pAchievement->GetElement(0)->GetStringValue();
		CString sValue = ParseAchievementValue(pAchievement->GetElement(1));
		CString sSection = ParseAchievementSection(pAchievement->GetElement(2));
		CString sSort = ParseAchievementSort(pAchievement->GetElement(3));

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

CString CGameStats::ParseAchievementSection (ICCItem *pItem)
	{
	if (pItem == NULL)
		return NULL_STR;
	else if (pItem->IsNil())
		return NULL_STR;
	else
		return pItem->GetStringValue();
	}

CString CGameStats::ParseAchievementSort (ICCItem *pItem)
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

CString CGameStats::ParseAchievementValue (ICCItem *pItem)
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

		Stat.InsertHandoff(CJSONValue(m_Stats[i].sStatName));

		//	Parse the value

		int iValue;
		if (m_Stats[i].sStatValue.IsBlank())
			Stat.Insert(CJSONValue(CJSONValue::typeNull));
		else if (strIsInt(m_Stats[i].sStatValue, PARSE_THOUSAND_SEPARATOR, &iValue))
			Stat.InsertHandoff(CJSONValue(iValue));
		else
			Stat.InsertHandoff(CJSONValue(m_Stats[i].sStatValue));

		//	Split the section out of the sort key

		CString sSection;
		CString sSectionSortKey;
		ParseSortKey(m_Stats[i].sSortKey, &sSection, &sSectionSortKey);

		if (sSection.IsBlank())
			Stat.Insert(CJSONValue(CJSONValue::typeNull));
		else
			Stat.InsertHandoff(CJSONValue(sSection));

		//	Add the sort key

		if (!strEquals(sSectionSortKey, m_Stats[i].sStatName))
			Stat.InsertHandoff(CJSONValue(sSectionSortKey));

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

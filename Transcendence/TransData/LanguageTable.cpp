//	LanguageTable.cpp
//
//	Dumps language elements.
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define CODE_ATTRIB							CONSTLIT("code")
#define CRITERIA_ATTRIB						CONSTLIT("criteria")

#define FIELD_ID							CONSTLIT("id")
#define FIELD_TEXT							CONSTLIT("text")
#define FIELD_UNID							CONSTLIT("unid")

static constexpr int MAX_CODE_LENGTH =		256;

struct SLanguageTableOptions
	{
	bool bShowCode = false;
	};

void OutputLanguageBlock (CDesignType *pType, const TArray<CString> &Cols, const SLanguageTableOptions &Options);
void OutputLanguageEntry (CDesignType *pType, const CLanguageDataBlock::SEntryDesc &Entry, const TArray<CString> &Cols, const SLanguageTableOptions &Options);

void GenerateLanguageTable (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	int i;

	//	Get the criteria from the command line.

	CString sCriteria = pCmdLine->GetAttribute(CRITERIA_ATTRIB);
	if (sCriteria.IsBlank())
		sCriteria = CONSTLIT("*");

	//	Parse it

	CDesignTypeCriteria Criteria;
	if (CDesignTypeCriteria::ParseCriteria(sCriteria, &Criteria) != NOERROR)
		{
		printf("ERROR: Unable to parse criteria.\n");
		return;
		}

	//	Options

	SLanguageTableOptions Options;
	Options.bShowCode = pCmdLine->GetAttributeBool(CODE_ATTRIB);

	//	Generate a table of all matching types

	TSortMap<CString, CDesignType *> Table;

	//	Loop over all items for this level and add them to
	//	a sorted table.

	for (i = 0; i < Universe.GetDesignTypeCount(); i++)
		{
		CDesignType *pType = Universe.GetDesignType(i);
		int iLevel = pType->GetLevel();

		if (!pType->MatchesCriteria(Criteria))
			continue;

		//	Skip if we don't have an language elements.

		if (!pType->HasLanguageBlock())
			continue;

		//	Figure out the sort order

		char szBuffer[1024];
		wsprintf(szBuffer, "%08x", 
				pType->GetUNID());
		Table.Insert(CString(szBuffer), pType);
		}

	//	No entries

	if (Table.GetCount() == 0)
		{
		printf("No entries match criteria.\n");
		return;
		}

	//	Generate a list of columns to display

	TArray<CString> Cols;
	Cols.Insert(FIELD_UNID);
	Cols.Insert(FIELD_ID);
	Cols.Insert(FIELD_TEXT);

	//	Output the header

	for (i = 0; i < Cols.GetCount(); i++)
		{
		if (i != 0)
			printf("\t");

		printf(Cols[i].GetASCIIZPointer());
		}

	printf("\n");

	//	Output each row

	for (i = 0; i < Table.GetCount(); i++)
		{
		CDesignType *pType = Table[i];

		OutputLanguageBlock(pType, Cols, Options);
		}

	printf("\n");
	}

void OutputLanguageBlock (CDesignType *pType, const TArray<CString> &Cols, const SLanguageTableOptions &Options)
	{
	int i;

	CLanguageDataBlock Language = pType->GetMergedLanguageBlock();
	for (i = 0; i < Language.GetCount(); i++)
		{
		CLanguageDataBlock::SEntryDesc Entry = Language.GetEntry(i);

		//	In code mode we only show entries with code.

		if (Options.bShowCode)
			{
			if (!Entry.pCode)
				continue;

			OutputLanguageEntry(pType, Entry, Cols, Options);
			}

		//	Otherwise we show entries with text.

		else
			{
			if (Entry.sText.IsBlank())
				continue;

			OutputLanguageEntry(pType, Entry, Cols, Options);
			}
		}
	}

void OutputLanguageEntry (CDesignType *pType, const CLanguageDataBlock::SEntryDesc &Entry, const TArray<CString> &Cols, const SLanguageTableOptions &Options)
	{
	int i;
	
	for (i = 0; i < Cols.GetCount(); i++)
		{
		if (strEquals(Cols[i], FIELD_ID))
			printf((LPSTR)Entry.sID);
		else if (strEquals(Cols[i], FIELD_TEXT))
			{
			if (Options.bShowCode)
				{
				CCodeChain &CC = g_pUniverse->GetCC();
				CString sCode = CC.Unlink(Entry.pCode);

				printf("%s", (LPSTR)strSubString(sCode, 0, MAX_CODE_LENGTH));
				}
			else
				printf("%s", (LPSTR)Entry.sText);
			}
		else if (strEquals(Cols[i], FIELD_UNID))
			printf("%0x8", pType->GetUNID());

		if (i == Cols.GetCount() - 1)
			printf("\n");
		else
			printf("\t");
		}
	}

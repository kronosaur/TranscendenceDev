//	LanguageTable.cpp
//
//	Dumps language elements.
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Console.h"

#define CODE_ATTRIB							CONSTLIT("code")
#define CRITERIA_ATTRIB						CONSTLIT("criteria")
#define INHERITED_ATTRIB					CONSTLIT("inherited")
#define SCRIPT_ATTRIB						CONSTLIT("script")

#define FIELD_ID							CONSTLIT("id")
#define FIELD_TEXT							CONSTLIT("text")
#define FIELD_UNID							CONSTLIT("unid")

static constexpr int MAX_CODE_LENGTH =		256;

enum class ELanguageOutputTypes
	{
	none,

	script,
	table,
	};

struct SLanguageTableOptions
	{
	ELanguageOutputTypes iType = ELanguageOutputTypes::table;
	bool bShowCode = false;
	bool bShowInherited = false;
	};

void OutputLanguageBlock (CDesignType *pType, const TArray<CString> &Cols, const SLanguageTableOptions &Options);
void OutputLanguageEntry (CDesignType *pType, const CLanguageDataBlock::SEntryDesc &Entry, const TArray<CString> &Cols, const SLanguageTableOptions &Options);
void OutputScript (CUniverse &Universe, const TSortMap<CString, CDesignType *> &Table, const SLanguageTableOptions &Options);
void OutputScriptBlock (const CDesignType &Type, const SLanguageTableOptions &Options);
void OutputTable (CUniverse &Universe, const TSortMap<CString, CDesignType *> &Table, const SLanguageTableOptions &Options);

void GenerateLanguageTable (CUniverse &Universe, CXMLElement *pCmdLine)
	{
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
	if (pCmdLine->GetAttributeBool(SCRIPT_ATTRIB))
		Options.iType = ELanguageOutputTypes::script;
	else
		Options.iType = ELanguageOutputTypes::table;

	Options.bShowCode = pCmdLine->GetAttributeBool(CODE_ATTRIB);
	Options.bShowInherited = pCmdLine->GetAttributeBool(INHERITED_ATTRIB);

	//	Generate a table of all matching types

	TSortMap<CString, CDesignType *> Table;

	//	Loop over all items for this level and add them to
	//	a sorted table.

	for (int i = 0; i < Universe.GetDesignTypeCount(); i++)
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

	//	Output

	switch (Options.iType)
		{
		case ELanguageOutputTypes::script:
			OutputScript(Universe, Table, Options);
			break;

		default:
			OutputTable(Universe, Table, Options);
			break;
		}
	}

void OutputLanguageBlock (CDesignType *pType, const TArray<CString> &Cols, const SLanguageTableOptions &Options)
	{
	//	Get the appropriate block.

	const CLanguageDataBlock *pLanguage = NULL;
	CLanguageDataBlock MergedBlock;

	if (Options.bShowInherited)
		{
		MergedBlock = pType->GetMergedLanguageBlock();
		pLanguage = &MergedBlock;
		}
	else
		pLanguage = &pType->GetLanguageBlock();

	//	Output each entry in the block.

	for (int i = 0; i < pLanguage->GetCount(); i++)
		{
		CLanguageDataBlock::SEntryDesc Entry = pLanguage->GetEntry(i);

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
				CString sCode = CCodeChain::Unlink(Entry.pCode);

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

void OutputScript (CUniverse &Universe, const TSortMap<CString, CDesignType *> &Table, const SLanguageTableOptions &Options)
	{
	for (int i = 0; i < Table.GetCount(); i++)
		{
		const CDesignType &Type = *Table[i];

		CString sTypeName = strPatternSubst(CONSTLIT("%08x: %s"), Type.GetUNID(), strToUpper(Type.GetNounPhrase()));
		printf("%s\n%s\n\n", (LPSTR)CConsoleFormat::CenterLine(sTypeName), (LPSTR)CConsoleFormat::CenterLine(strRepeat(CONSTLIT("-"), sTypeName.GetLength())));

		OutputScriptBlock(Type, Options);
		}
	}

void OutputScriptBlock (const CDesignType &Type, const SLanguageTableOptions &Options)
	{
	//	Get the appropriate block.

	const CLanguageDataBlock *pLanguage = NULL;
	CLanguageDataBlock MergedBlock;

	if (Options.bShowInherited)
		{
		MergedBlock = Type.GetMergedLanguageBlock();
		pLanguage = &MergedBlock;
		}
	else
		pLanguage = &Type.GetLanguageBlock();

	//	Output each entry in the block.

	for (int i = 0; i < pLanguage->GetCount(); i++)
		{
		CLanguageDataBlock::SEntryDesc Entry = pLanguage->GetEntry(i);

		if (Options.bShowCode)
			{
			if (!Entry.pCode)
				continue;

			printf("%s\n\n", (LPSTR)CConsoleFormat::CenterLine(Entry.sID));

			CString sCode = CCodeChain::Unlink(Entry.pCode);
			printf("%s\n\n", (LPSTR)sCode);
			}
		else
			{
			if (Entry.sText.IsBlank())
				continue;

			printf("%s\n\n", (LPSTR)CConsoleFormat::CenterLine(Entry.sID));

			TArray<CString> Lines;
			CConsoleFormat::WrapText(Entry.sText, Lines);
			for (int j = 0; j < Lines.GetCount(); j++)
				printf("%s\n", (LPSTR)Lines[j]);

			printf("\n");
			}
		}
	}

void OutputTable (CUniverse &Universe, const TSortMap<CString, CDesignType *> &Table, const SLanguageTableOptions &Options)
	{
	TArray<CString> Cols;
	Cols.Insert(FIELD_UNID);
	Cols.Insert(FIELD_ID);
	Cols.Insert(FIELD_TEXT);

	//	Output the header

	for (int i = 0; i < Cols.GetCount(); i++)
		{
		if (i != 0)
			printf("\t");

		printf(Cols[i].GetASCIIZPointer());
		}

	printf("\n");

	//	Output each row

	for (int i = 0; i < Table.GetCount(); i++)
		{
		CDesignType *pType = Table[i];

		OutputLanguageBlock(pType, Cols, Options);
		}

	printf("\n");
	}

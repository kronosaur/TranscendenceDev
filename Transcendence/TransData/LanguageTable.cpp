//	LanguageTable.cpp
//
//	Dumps language elements.
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Console.h"

#define CODE_ATTRIB							CONSTLIT("code")
#define CRITERIA_ATTRIB						CONSTLIT("criteria")
#define INHERITED_ATTRIB					CONSTLIT("inherited")
#define LEXICON_ATTRIB						CONSTLIT("lexicon")
#define REFERENCES_ATTRIB					CONSTLIT("references")
#define SCRIPT_ATTRIB						CONSTLIT("script")

#define FIELD_ID							CONSTLIT("id")
#define FIELD_TEXT							CONSTLIT("text")
#define FIELD_UNID							CONSTLIT("unid")

static constexpr int MAX_CODE_LENGTH =		256;

enum class ELanguageOutputTypes
	{
	none,

	lexicon,
	script,
	table,
	};

struct SLanguageTableOptions
	{
	ELanguageOutputTypes iType = ELanguageOutputTypes::table;
	bool bShowCode = false;
	bool bIncludeScript = false;
	bool bShowInherited = false;
	bool bShowReferences = false;
	bool bIncludeAllTypes = false;
	};

struct SLexiconEntry
	{
	CString sWord;
	int iCount = 0;
	TArray<CString> References;
	};

void OutputLanguageBlock (CDesignType *pType, const TArray<CString> &Cols, const SLanguageTableOptions &Options);
void OutputLanguageEntry (CDesignType *pType, const CLanguageDataBlock::SEntryDesc &Entry, const TArray<CString> &Cols, const SLanguageTableOptions &Options);
void OutputLexicon (CUniverse &Universe, const TSortMap<CString, CDesignType *> &Table, const SLanguageTableOptions &Options);
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
	if (pCmdLine->GetAttributeBool(LEXICON_ATTRIB))
		Options.iType = ELanguageOutputTypes::lexicon;
	else if (pCmdLine->GetAttributeBool(SCRIPT_ATTRIB))
		Options.iType = ELanguageOutputTypes::script;
	else
		Options.iType = ELanguageOutputTypes::table;

	Options.bShowCode = pCmdLine->GetAttributeBool(CODE_ATTRIB);
	Options.bShowInherited = pCmdLine->GetAttributeBool(INHERITED_ATTRIB);
	Options.bShowReferences = pCmdLine->GetAttributeBool(REFERENCES_ATTRIB);
	Options.bIncludeAllTypes = (Options.iType == ELanguageOutputTypes::lexicon);

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

		if (!Options.bIncludeAllTypes 
				&& !pType->HasLanguageBlock())
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
		case ELanguageOutputTypes::lexicon:
			OutputLexicon(Universe, Table, Options);
			break;

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

			if (!Options.bIncludeScript
					&& CLanguageDataBlock::IsScriptEntry(Entry))
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

void AccumulateLexicon (const CString &sReference, const CString &sText, const SLanguageTableOptions &Options, TSortMap<CString, SLexiconEntry> &Lexicon)
	{
	CWordParser Parser(sText.GetASCIIZPointer(), sText.GetASCIIZPointer() + sText.GetLength());
	Parser.SetExcludePercentFields();
	Parser.SetHandleAccelerators();

	TSortMap<CString, int> WordsInEntry;
	WordsInEntry.GrowToFit(1000);

	while (true)
		{
		CString sWord = Parser.ParseNextWord();
		if (sWord.IsBlank())
			break;

		bool bNew;
		auto *pCount = WordsInEntry.SetAt(sWord, &bNew);
		if (bNew)
			*pCount = 1;
		else
			(*pCount)++;
		}

	for (int i = 0; i < WordsInEntry.GetCount(); i++)
		{
		const CString &sWord = WordsInEntry.GetKey(i);

		auto *pEntry = Lexicon.SetAt(sWord);
		if (pEntry->sWord.IsBlank())
			pEntry->sWord = sWord;

		//	If this word comes in capital and lowercase versions, show the 
		//	lowercase version.

		else if (strIsUpper(pEntry->sWord.GetASCIIZPointer()) && !strIsUpper(sWord.GetASCIIZPointer()))
			pEntry->sWord = sWord;

		pEntry->iCount += WordsInEntry[i];
		pEntry->References.Insert(sReference);
		}
	}

void AccumulateLexicon (const CString &sTypeEntity, const CLanguageDataBlock &Language, const SLanguageTableOptions &Options, TSortMap<CString, SLexiconEntry> &Lexicon)
	{
	for (int i = 0; i < Language.GetCount(); i++)
		{
		const auto Entry = Language.GetEntry(i);

		if (Entry.sText.IsBlank())
			continue;

		if (!Options.bIncludeScript
				&& CLanguageDataBlock::IsScriptEntry(Entry))
			continue;

		CString sReference = strPatternSubst(CONSTLIT("%s::%s"), sTypeEntity, Entry.sID);

		AccumulateLexicon(sReference, Entry.sText, Options, Lexicon);
		}
	}

void OutputLexicon (CUniverse &Universe, const TSortMap<CString, CDesignType *> &Table, const SLanguageTableOptions &Options)
	{
	TSortMap<CString, SLexiconEntry> Lexicon;
	Lexicon.GrowToFit(10000);

	for (int i = 0; i < Table.GetCount(); i++)
		{
		const CDesignType &Type = *Table[i];

		const CLanguageDataBlock *pLanguage = NULL;
		CLanguageDataBlock MergedBlock;

		if (Options.bShowInherited)
			{
			MergedBlock = Type.GetMergedLanguageBlock();
			pLanguage = &MergedBlock;
			}
		else
			pLanguage = &Type.GetLanguageBlock();

		CString sEntityName = Type.GetEntityName();
		if (sEntityName.IsBlank())
			sEntityName = strPatternSubst(CONSTLIT("%08x"), Type.GetUNID());

		AccumulateLexicon(sEntityName, *pLanguage, Options, Lexicon);

		//	We pull text from other places for specific types.

		switch (Type.GetType())
			{
			case designItemType:
				{
				const CItemType *pItemType = CItemType::AsType(&Type);
				if (!pItemType)
					continue;

				AccumulateLexicon(strPatternSubst(CONSTLIT("%s/name"), sEntityName), pItemType->GetNounPhrase(nounActual), Options, Lexicon);
				AccumulateLexicon(strPatternSubst(CONSTLIT("%s/desc"), sEntityName), pItemType->GetDesc(true), Options, Lexicon);
				break;
				}
			}
		}

	//	Output

	int iTotalWords = 0;
	for (int i = 0; i < Lexicon.GetCount(); i++)
		{
		const auto &Entry = Lexicon[i];

		printf("%s: ", (LPSTR)Entry.sWord);

		if (Options.bShowReferences)
			{
			for (int j = 0; j < Entry.References.GetCount(); j++)
				{
				if (j != 0)
					printf(", ");

				printf("%s", (LPSTR)Entry.References[j]);
				}

			printf("\n\n");
			}
		else
			{
			printf("%s\n", (LPSTR)strFormatInteger(Entry.iCount, -1, FORMAT_THOUSAND_SEPARATOR));
			}

		iTotalWords += Entry.iCount;
		}

	printf("\n");
	printf("LEXICON ENTRIES: %s\n", (LPSTR)strFormatInteger(Lexicon.GetCount(), -1, FORMAT_THOUSAND_SEPARATOR));
	printf("TOTAL WORDS: %s\n", (LPSTR)strFormatInteger(iTotalWords, -1, FORMAT_THOUSAND_SEPARATOR));
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

			if (!Options.bIncludeScript
					&& CLanguageDataBlock::IsScriptEntry(Entry))
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

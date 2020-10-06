//	CScript.cpp
//
//	CScript class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Console.h"
#include "ScriptParserImpl.h"

void CScript::AddScriptText (const CDesignCollection &Design, const SScriptEntry &Entry)

//	AddScriptText
//
//	Parses script syntax and adds paragraphs as appropriate.

	{
	CScriptParser Parser(Design, Entry.pSource, Entry.sScript.GetASCIIZPointer(), Entry.sScript.GetLength());

	SParagraph Para;
	while (Parser.ParseNextParagraph(&Para))
		{
		m_Script.Insert(Para);
		}
	}

void CScript::Init (const CDesignCollection &Design, const TSortMap<CString, SScriptEntry> &Text)

//	Init
//
//	Initialize the script from markup.

	{
	DeleteAll();

	for (int i = 0; i < Text.GetCount(); i++)
		AddScriptText(Design, Text[i]);
	}

void CScript::OutputConsole (void) const

//	OutputConsole
//
//	Outputs the entire script to stdout.

	{
	constexpr int COLUMNS = 80;

	for (int i = 0; i < m_Script.GetCount(); i++)
		{
		const SParagraph &Para = m_Script[i];

		TArray<CString> Lines;

		switch (Para.iStyle)
			{
			case ParagraphStyle::normal:
			case ParagraphStyle::normalLine:
				{
				CConsoleFormat::WrapText(Para.sText, Lines, COLUMNS);
				break;
				}

			case ParagraphStyle::centered:
			case ParagraphStyle::centeredLine:
				{
				CConsoleFormat::WrapText(Para.sText, Lines, COLUMNS);
				for (int j = 0; j < Lines.GetCount(); j++)
					Lines[j] = CConsoleFormat::CenterLine(Lines[j], COLUMNS);

				break;
				}
			}

		for (int j = 0; j < Lines.GetCount(); j++)
			printf("%s\n", (LPCSTR)Lines[j]);

		switch (Para.iStyle)
			{
			case ParagraphStyle::centered:
			case ParagraphStyle::normal:
				printf("\n");
				break;
			}
		}
	}

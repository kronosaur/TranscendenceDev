//	CCTransDataLibrary.cpp
//
//	Functions and placeholders defined by TransData
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

static constexpr DWORD FN_TSE_PATTERN =					1;

ICCItem *fnNil (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);
ICCItem *fnTransEngine (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

static PRIMITIVEPROCDEF g_Library[] =
	{
		//	The following are placeholder functions that are normally 
		//	implemented in Transcendence (above TSE).

		{	"scrTranslate",					fnNil,		0,
			"PLACEHOLDER",
			"iv*",	0,	},

		//	These are engine diagnostic functions

		{	"tsePattern",					fnTransEngine,		FN_TSE_PATTERN,
			"(tsePattern pattern ...) -> string",
			"s*",	0,	},
	};

static constexpr int FUNCTION_COUNT	=	((sizeof g_Library) / (sizeof g_Library[0]));

void GetCCTransDataLibrary (SPrimitiveDefTable *retpTable)

//	GetCCTransDataLibrary
//
//	Registers extensions

	{
	retpTable->pTable = g_Library;
	retpTable->iCount = FUNCTION_COUNT;
	}

ICCItem *fnNil (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnNil
//
//	Always returns Nil

	{
	CCodeChain *pCC = pEvalCtx->pCC;
	return pCC->CreateNil();
	}

ICCItem *fnTransEngine (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnTransEngine

	{
	CCodeChain *pCC = pEvalCtx->pCC;

	switch (dwData)
		{
		case FN_TSE_PATTERN:
			{
			static constexpr int MAX_ARG_BLOCK_SIZE = 4000;

			CString sPattern = pArgs->GetElement(0)->GetStringValue();

			//	Generate a block holding pointers to actual arguments. We also
			//	keep a list of strings that we allocated.

			TArray<CString> Strings;
			Strings.InsertEmpty(pArgs->GetCount());
			CMemoryWriteStream ArgBlock(MAX_ARG_BLOCK_SIZE);
			if (ArgBlock.Create() != NOERROR)
				return pCC->CreateNil();

			//	Add all arguments.

			for (int i = 1; i < pArgs->GetCount(); i++)
				{
				ICCItem *pArg = pArgs->GetElement(i);
				switch (pArg->GetValueType())
					{
					case ICCItem::String:
						{
						CString *pSrcString = &Strings[i];
						*pSrcString = pArg->GetStringValue();
						ArgBlock.Write((char *)pSrcString, sizeof(CString));
						break;
						}

					case ICCItem::Integer:
						{
						int iSrcInt = pArg->GetIntegerValue();
						ArgBlock.Write((char *)&iSrcInt, sizeof(int));
						break;
						}

					case ICCItem::Double:
						{
						double rSrcDouble = pArg->GetDoubleValue();
						ArgBlock.Write((char *)&rSrcDouble, sizeof(double));
						break;
						}

					default:
						return pCC->CreateError(CONSTLIT("Unsupported type"), pArg);
					}
				}

			//	Execute

			CString sResult;
			try
				{
				sResult = strPattern(sPattern, (LPVOID *)ArgBlock.GetPointer());
				}
			catch (...)
				{
				return pCC->CreateError(CONSTLIT("Crash in strPattern"));
				}

			return pCC->CreateString(sResult);
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

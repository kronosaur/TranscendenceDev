//	CCTransDataLibrary.cpp
//
//	Functions and placeholders defined by TransData
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

static constexpr DWORD FN_TSE_PATTERN =					1;
static constexpr DWORD FN_TSE_SET_SYSTEM =				2;
static constexpr DWORD FN_TSE_UPDATE_SYSTEM =			3;

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

		{	"diagSetSystem",				fnTransEngine,		FN_TSE_SET_SYSTEM,
			"(diagSetSystem nodeID) -> True/error",
			"s",	PPFLAG_SIDEEFFECTS,	},

		{	"diagUpdateSystem",				fnTransEngine,		FN_TSE_UPDATE_SYSTEM,
			"(diagUpdateSystem [updates]) -> True/error",
			"*",	PPFLAG_SIDEEFFECTS,	},

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
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;
	CUniverse &Universe = pCtx->GetUniverse();

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

		case FN_TSE_SET_SYSTEM:
			{
			CString sNodeID = pArgs->GetElement(0)->GetStringValue();
			CTopologyNode *pNode = Universe.FindTopologyNode(sNodeID);
			if (pNode == NULL)
				return pCC->CreateError("Unknown nodeID", pArgs->GetElement(0));

			//	If system is not loaded, then we fail.

			CSystem *pSystem = pNode->GetSystem();
			if (pSystem == NULL)
				return pCC->CreateError("System not loaded", pArgs->GetElement(0));

			//	Set as current system.

			Universe.SetCurrentSystem(pSystem);
			return pCC->CreateTrue();
			}

		case FN_TSE_UPDATE_SYSTEM:
			{
			int iUpdates;
			if (pArgs->GetCount() > 0)
				iUpdates = pArgs->GetElement(0)->GetIntegerValue();
			else
				iUpdates = 1;

			if (iUpdates <= 0)
				return pCC->CreateNil();

			SSystemUpdateCtx UpdateCtx;
			UpdateCtx.bForceEventFiring = true;
			UpdateCtx.bForcePainted = true;

			for (int i = 0; i < iUpdates; i++)
				{
				Universe.Update(UpdateCtx);
				}

			return pCC->CreateTrue();
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

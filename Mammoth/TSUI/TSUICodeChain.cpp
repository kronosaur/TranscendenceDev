//	TSUICodeChain.cpp
//
//	Registers CodeChain primitives from TSUI.
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "stdafx.h"

#define FN_GET_MUSIC_CATALOG			0
#define FN_PLAY_MUSIC					1
#define FN_STOP_MUSIC					2
#define FN_CAN_PLAY_MUSIC				3
#define FN_GET_MUSIC_STATE				4
#define FN_KEY_PRESSED					5
#define FN_GET_MOUSE_POSITION			6

#define STR_COMMAND_PREFIX				CONSTLIT("command:")

ICCItem *fnUI (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData);

static PRIMITIVEPROCDEF g_Primitives[] =
	{
		//	UI functions
		//	------------

		{	"uiCanPlayMusic",				fnUI,			FN_CAN_PLAY_MUSIC,
			"(uiCanPlayMusic filename) -> True/Nil",
			"s",	0,	},

		{	"uiGetMusicCatalog",			fnUI,			FN_GET_MUSIC_CATALOG,
			"(uiGetMusicCatalog) -> list of files",
			"",	0,	},

		{   "uiGetMousePos",				fnUI,			FN_GET_MOUSE_POSITION,
			"(uiGetMousePos) -> (x y)",
			"",	0,  },

		{	"uiGetMusicState",				fnUI,			FN_GET_MUSIC_STATE,
			"(uiGetMusicState) -> ('playing filename position length)",
			"",	0,	},

		{	"uiIsKeyPressed",				fnUI,			FN_KEY_PRESSED,
			"(uiIsKeyPressed key|command) -> True/Nil\n\n"

			"Key is a string, all alphanumeric keys as well as the following are supported:\n\n"

			"   'Alt\n"
			"   'Backquote\n"
			"   'Backslash\n"
			"   'Backspace\n"
			"   'CloseBracket\n"
			"   'Comma\n"
			"   'Ctrl\n"
			"   'Delete\n"
			"   'Down\n"
			"   'End\n"
			"   'Equal\n"
			"   'Esc\n"
			"   'F1, 'F2, ...\n"
			"   'Home\n"
			"   'Insert\n"
			"   'LButton\n"
			"   'Left\n"
			"   'Minus\n"
			"   'OpenBracket\n"
			"   'PageDown\n"
			"   'PageUp\n"
			"   'Period\n"
			"   'Quote\n"
			"   'RButton\n"
			"   'Return\n"
			"   'Right\n"
			"   'SemiColon\n"
			"   'Shift\n"
			"   'Slash\n"
			"   'Space\n"
			"   'Tab\n"
			"   'Up\n"
			"\n"
			"Specify a command by prefixing with \"command:\".\n"
			"E.g., (uiIsKeyPressed \"command:ThrustForward\")\n\n",

			"s",	0, },

		{	"uiPlayMusic",					fnUI,			FN_PLAY_MUSIC,
			"(uiPlayMusic filename [pos]) -> True/Nil",
			"s*",	0,	},

		{	"uiStopMusic",					fnUI,			FN_STOP_MUSIC,
			"(uiStopMusic)",
			"",	0,	},
	};

#define PRIMITIVE_COUNT		(sizeof(g_Primitives) / sizeof(g_Primitives[0]))

void CHumanInterface::GetCodeChainPrimitives (SPrimitiveDefTable *retTable)

//	GetCodeChainPrimitives
//
//	Returns a table of primitives

	{
	retTable->pTable = g_Primitives;
	retTable->iCount = PRIMITIVE_COUNT;
	}

ICCItem *fnUI (CEvalContext *pEvalCtx, ICCItem *pArgs, DWORD dwData)

//	fnUI
//
//	UI functions

	{
	int i;
	CCodeChain *pCC = pEvalCtx->pCC;
	CCodeChainCtx *pCtx = (CCodeChainCtx *)pEvalCtx->pExternalCtx;

	switch (dwData)
		{
		case FN_CAN_PLAY_MUSIC:
			{
			//	Get the filespec

			CString sFilespec = pathAddComponent(g_pHI->GetOptions().m_sMusicFolder, pArgs->GetElement(0)->GetStringValue());
			
			//	Can play it?

			return pCC->CreateBool(g_pHI->GetSoundMgr().CanPlayMusic(sFilespec));
			}

		case FN_GET_MUSIC_CATALOG:
			{
			//	Get the list of music

			TArray<CString> Catalog;
			g_pHI->GetSoundMgr().GetMusicCatalog(g_pHI->GetOptions().m_sMusicFolder, &Catalog);

			//	Create a list to return

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			//	Add to list

			for (i = 0; i < Catalog.GetCount(); i++)
				pList->AppendString(pathMakeRelative(Catalog[i], g_pHI->GetOptions().m_sMusicFolder, true));

			return pResult;
			}

		case FN_GET_MOUSE_POSITION:
			{
			//  Get mouse position

			int iScreenResolutionX = g_pHI->GetScreenWidth();
			int iScreenResolutionY = g_pHI->GetScreenHeight();

			int x, y;
			g_pHI->GetMousePos(&x, &y);

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			//	Add to list

			pList->AppendInteger((x + 1) - (iScreenResolutionX / 2));
			pList->AppendInteger((y + 1) - (iScreenResolutionY / 2));

			return pResult;
			}

		case FN_GET_MUSIC_STATE:
			{
			//	Get the state

			//	Are we playing?

			CSoundMgr::SMusicPlayState State;
			if (!g_pHI->GetSoundMgr().GetMusicPlayState(&State))
				return pCC->CreateNil();

			//	Create a list to return

			ICCItem *pResult = pCC->CreateLinkedList();
			if (pResult->IsError())
				return pResult;

			CCLinkedList *pList = (CCLinkedList *)pResult;

			//	Add to list

			pList->AppendString(CONSTLIT("playing"));
			pList->AppendString(pathMakeRelative(State.sFilename, g_pHI->GetOptions().m_sMusicFolder, true));
			pList->AppendInteger(State.iPos);
			pList->AppendInteger(State.iLength);

			return pResult;
			}

		case FN_KEY_PRESSED:
			{
			CString sKey = pArgs->GetElement(0)->GetStringValue();
			if (strStartsWith(sKey, STR_COMMAND_PREFIX))
				{
				CString sCmd = strSubString(sKey, STR_COMMAND_PREFIX.GetLength());
				DWORD dwKey;
				if (!pCtx->GetUniverse().GetHost()->FindCommandKey(sCmd, &dwKey))
					{
					if (pCtx->GetUniverse().InDebugMode())
						return pCC->CreateError(CONSTLIT("Invalid command"), pArgs->GetElement(0));
					else
						return pCC->CreateNil();
					}

				if (dwKey == CVirtualKeyData::INVALID_VIRT_KEY)
					return pCC->CreateNil();
				else
					return pCC->CreateBool(uiIsKeyDown(dwKey));
				}
			else
				{
				DWORD dwKey = CVirtualKeyData::GetKey(sKey);
				if (dwKey == CVirtualKeyData::INVALID_VIRT_KEY)
					{
					//	For backwards compatibility we handle these strings.

					if (strEquals(sKey, "lmb"))
						dwKey = VK_LBUTTON;
					else if (strEquals(sKey, "rmb"))
						dwKey = VK_RBUTTON;

					//	Not found

					else if (pCtx->GetUniverse().InDebugMode())
						return pCC->CreateError(CONSTLIT("Invalid key code"), pArgs->GetElement(0));
					else
						return pCC->CreateNil();
					}

				return pCC->CreateBool(uiIsKeyDown(dwKey));
				}
			}

		case FN_PLAY_MUSIC:
			{
			CString sTrack = pArgs->GetElement(0)->GetStringValue();

			//	Get the filespec

			CString sFilespec = pathAddComponent(g_pHI->GetOptions().m_sMusicFolder, sTrack);
			int iPos = (pArgs->GetCount() > 1 ? pArgs->GetElement(1)->GetIntegerValue() : 0);
			
			//	Play it

			CString sError;
			if (!g_pHI->GetSoundMgr().PlayMusic(sFilespec, iPos, &sError))
				return pCC->CreateError(sError);

			//	Done

			return pCC->CreateTrue();
			}

		case FN_STOP_MUSIC:
			{
			g_pHI->GetSoundMgr().StopMusic();
			return pCC->CreateTrue();
			}

		default:
			ASSERT(false);
			return pCC->CreateNil();
		}
	}

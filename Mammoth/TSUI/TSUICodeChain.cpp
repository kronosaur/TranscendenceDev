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

		{	"uiGetMusicState",				fnUI,			FN_GET_MUSIC_STATE,
			"(uiGetMusicState) -> ('playing filename position length)",
			"",	0,	},

		{	"uiIsKeyPressed",				fnUI,			FN_KEY_PRESSED,
			"(uiIsKeyPressed key) -> True/Nil\n\n"

			"Key is a string, all alphanumeric keys as well as the following are supported:\n\n"

			"   'up\n"
			"   'down\n"
			"   'left\n"
			"   'right\n"
			"   'ctrl\n"
			"   'alt\n"
			"   'space\n"
			"   'shift\n"
			"   'backspace\n"
			"   'pgup\n"
			"   'pgdown\n"
			"   'ins\n"
			"   'del\n"
			"   'pause\n"
			"   'tab\n"
			"   'enter\n"
			"   'home\n"
			"   'end\n"
			"   'esc\n"
			"   'lmb\n"
			"   'rmb\n",

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
			char *pKeyString = sKey.GetASCIIZPointer();

			//  Check to see if it's a string of more than one character. If so, it's not an alphanumeric
			//  key, and we should use a switch block.
			if (sKey.GetLength() <= 1)
				{
				//  Alphanumeric case. Check letter/number keys. (We won't support non-alphanumeric
				//  char keys, since GetAsyncKeyState doesn't work with them as easily)
				char iKey = toupper(pKeyString[0]);

				if (uiIsKeyDown(iKey) == true)
					return pCC->CreateTrue();
				else
					return pCC->CreateNil();
				}
			else
			{
				//  Non-alphanumeric case. Use a switch statement.
				char iKey = 0;
				if (strEquals(sKey, "shift"))
				{
					iKey = VK_SHIFT;
				}
				else if (strEquals(sKey, "up"))
				{
					iKey = VK_UP;
				}
				else if (strEquals(sKey, "down"))
				{
					iKey = VK_DOWN;
				}
				else if (strEquals(sKey, "left"))
				{
					iKey = VK_LEFT;
				}
				else if (strEquals(sKey, "right"))
				{
					iKey = VK_RIGHT;
				}
				else if (strEquals(sKey, "up"))
				{
					iKey = VK_UP;
				}
				else if (strEquals(sKey, "space"))
				{
					iKey = VK_SPACE;
				}
				else if (strEquals(sKey, "esc"))
				{
					iKey = VK_ESCAPE;
				}
				else if (strEquals(sKey, "ctrl"))
				{
					iKey = VK_CONTROL;
				}
				else if (strEquals(sKey, "alt"))
				{
					iKey = VK_MENU;
				}
				else if (strEquals(sKey, "pgup"))
				{
					iKey = VK_PRIOR;
				}
				else if (strEquals(sKey, "pgdown"))
				{
					iKey = VK_NEXT;
				}
				else if (strEquals(sKey, "enter"))
				{
					iKey = VK_RETURN;
				}
				else if (strEquals(sKey, "ins"))
				{
					iKey = VK_INSERT;
				}
				else if (strEquals(sKey, "del"))
				{
					iKey = VK_DELETE;
				}
				else if (strEquals(sKey, "pause"))
				{
					iKey = VK_PAUSE;
				}
				else if (strEquals(sKey, "tab"))
				{
					iKey = VK_TAB;
				}
				else if (strEquals(sKey, "backspace"))
				{
					iKey = VK_BACK;
				}
				else if (strEquals(sKey, "home"))
				{
					iKey = VK_HOME;
				}
				else if (strEquals(sKey, "end"))
				{
					iKey = VK_END;
				}
				else if (strEquals(sKey, "lmb"))
				{
					iKey = VK_LBUTTON;
				}
				else if (strEquals(sKey, "rmb"))
				{
					iKey = VK_RBUTTON;
				}
				else return pCC->CreateNil();

				if (uiIsKeyDown(iKey) == true)
					return pCC->CreateTrue();
				else
					return pCC->CreateNil();
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

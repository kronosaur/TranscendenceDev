//	CGameSession.cpp
//
//	CGameSession class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"

#define CMD_GAME_END_DELETE						CONSTLIT("gameEndDelete")
#define CMD_GAME_END_SAVE						CONSTLIT("gameEndSave")
#define CMD_GAME_REVERT							CONSTLIT("gameRevert")
#define CMD_GAME_SELF_DESTRUCT					CONSTLIT("gameSelfDestruct")
#define CMD_GAME_SELF_DESTRUCT_CANCEL			CONSTLIT("gameSelfDestructCancel")
#define CMD_GAME_SELF_DESTRUCT_CONFIRM			CONSTLIT("gameSelfDestructConfirm")

#define CMD_UI_SHOW_GAME_STATS					CONSTLIT("uiShowGameStats")
#define CMD_UI_SHOW_HELP						CONSTLIT("uiShowHelp")
#define CMD_UI_SHOW_SETTINGS    				CONSTLIT("uiShowSettings")

CMenuData CGameSession::CreateGameMenu (void) const

//	CreateGameMenu
//
//	Returns the game menu data structure.

	{
	CMenuData MenuData;
	MenuData.SetTitle(CONSTLIT("Transcendence"));
	MenuData.AddMenuItem(CMD_UI_SHOW_HELP,			CONSTLIT("1"),	CONSTLIT("[H]elp"));
	MenuData.AddMenuItem(CMD_UI_SHOW_GAME_STATS,	NULL_STR,		CONSTLIT("[G]ame Stats"));
	MenuData.AddMenuItem(CMD_GAME_END_SAVE,			CONSTLIT("2"),	CONSTLIT("[S]ave & Exit Game"));
	MenuData.AddMenuItem(CMD_GAME_SELF_DESTRUCT,	CONSTLIT("3"),	CONSTLIT("Self-[D]estruct"));
	MenuData.AddMenuItem(CMD_UI_SHOW_SETTINGS,		NULL_STR,		CONSTLIT("Keyb[o]ard Settings"));

	//	Debug mode includes more special functions

	if (GetUniverse().InDebugMode())
		{
		MenuData.AddMenuItem(CMD_GAME_REVERT, CONSTLIT("9"), CONSTLIT("[R]evert Game"));
		MenuData.AddMenuItem(CMD_GAME_END_DELETE, CONSTLIT("0"), CONSTLIT("Dele[t]e Game"));
		}

	return MenuData;
	}

CMenuData CGameSession::CreateSelfDestructMenu (void) const

//	CreateSelfDestructMenu
//
//	Returns the self-destruct menu data structure.

	{
	CMenuData MenuData;
	MenuData.SetTitle(CONSTLIT("Self-Destruct"));
	MenuData.AddMenuItem(CMD_GAME_SELF_DESTRUCT_CONFIRM, CONSTLIT("1"), CONSTLIT("[C]onfirm"));
	MenuData.AddMenuItem(CMD_GAME_SELF_DESTRUCT_CANCEL, CONSTLIT("2"), CONSTLIT("Ca[n]cel"));

	return MenuData;
	}

ALERROR CGameSession::OnCommand (const CString &sCmd, void *pData)

//	OnCommand
//
//	Handle a command, usually from a menu.

	{
	if (strEquals(sCmd, CMD_GAME_END_DELETE))
		{
		m_HI.HICommand(sCmd);
		}
	else if (strEquals(sCmd, CMD_GAME_END_SAVE))
		{
		m_HI.HICommand(sCmd);
		}
	else if (strEquals(sCmd, CMD_GAME_REVERT))
		{
		m_HI.HICommand(sCmd);
		}
	else if (strEquals(sCmd, CMD_GAME_SELF_DESTRUCT))
		{
		DisplayMessage(CONSTLIT("Warning: Self-Destruct Activated"));

		ShowMenu(menuSelfDestructConfirm);
		}
	else if (strEquals(sCmd, CMD_GAME_SELF_DESTRUCT_CANCEL))
		{
		DismissMenu();
		}
	else if (strEquals(sCmd, CMD_GAME_SELF_DESTRUCT_CONFIRM))
		{
		DismissMenu();

		CPlayerShipController *pPlayer = m_Model.GetPlayer();
		if (pPlayer == NULL)
			return ERR_FAIL;

		pPlayer->GetShip()->Destroy(killedBySelf, CDamageSource(NULL, killedBySelf));
		}
	else if (strEquals(sCmd, CMD_UI_SHOW_GAME_STATS))
		{
		m_HI.HICommand(sCmd);
		DismissMenu();
		}
	else if (strEquals(sCmd, CMD_UI_SHOW_HELP))
		{
		m_HI.HICommand(sCmd);
		DismissMenu();
		}
	else if (strEquals(sCmd, CMD_UI_SHOW_SETTINGS))
		{
		m_HI.HICommand(sCmd);
		DismissMenu();
		}
	else
		return ERR_NOTFOUND;

	return NOERROR;
	}

bool CGameSession::ShowInvokeMenu ()

//	ShowInvokeMenu
//
//	Show the invoke menu.

	{
	CPlayerShipController *pPlayer = m_Model.GetPlayer();
	if (!pPlayer)
		return false;

	CShip *pPlayerShip = pPlayer->GetShip();
	if (!pPlayerShip)
		return false;

	g_pTrans->m_MenuData.SetTitle(CONSTLIT("Invoke Powers"));
	g_pTrans->m_MenuData.DeleteAll();

	bool bUseLetters = m_Settings.GetBoolean(CGameSettings::allowInvokeLetterHotKeys);

	TSortMap<CString, bool> KeyMap;
	char chInvokeKey = m_Settings.GetKeyMap().GetKeyIfChar(CGameKeys::keyInvokePower);
	KeyMap.Insert(CString(&chInvokeKey, 1), true);

	//	Add the powers

	for (int i = 0; i < GetUniverse().GetPowerCount(); i++)
		{
		CPower *pPower = GetUniverse().GetPower(i);

		CString sError;
		if (pPower->OnShow(pPlayerShip, NULL, &sError))
			{
			CString sKey = pPower->GetInvokeKey();
			if (sKey.IsBlank())
				continue;

			//	If we're the default letter keys, then make sure we don't
			//	conflict.

			if (bUseLetters)
				{
				//	Make sure key is one character long (we use a double-
				//	letter syntax below).

				sKey.Truncate(1);

				//	If the key conflicts, then pick another key (the next 
				//	key in the sequence).

				while (!sKey.IsBlank() && KeyMap.GetAt(sKey) != NULL)
					{
					char chChar = (*sKey.GetASCIIZPointer()) + 1;
					if (chChar == ':')
						chChar = 'A';

					if (chChar <= 'Z')
						sKey = CString(&chChar, 1);
					else
						sKey = NULL_STR;
					}
				}

			//	If we're not using letters, then convert to a number

			else
				{
				char chLetter = *sKey.GetASCIIZPointer();
				int iOrdinal = chLetter - 'A';

				//	A double letter means that we really want a letter, so we
				//	offset to increment past the numbers.

				if (sKey.GetLength() == 2)
					iOrdinal += 9;

				sKey = CMenuDisplayOld::GetHotKeyFromOrdinal(&iOrdinal, KeyMap);
				}

			//	Add the menu. (We check again to see if the key is valid
			//	because we might have collided and failed to find a substitute.)

			if (!sKey.IsBlank())
				{
				g_pTrans->m_MenuData.AddMenuItem(NULL_STR,
						sKey,
						pPower->GetName(),
						CMenuData::FLAG_SORT_BY_KEY,
						(DWORD)pPower);

				KeyMap.Insert(sKey, true);
				}
			}

		if (!sError.IsBlank())
			{
			DisplayMessage(sError);
			return false;
			}
		}

	//	If no powers are available, say so

	if (g_pTrans->m_MenuData.GetCount() == 0)
		{
		DisplayMessage(CONSTLIT("No Powers available"));
		return false;
		}

	//	Show menu

	g_pTrans->m_MenuDisplay.Invalidate();
	return true;
	}

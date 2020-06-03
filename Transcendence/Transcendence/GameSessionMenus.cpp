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

#define CMD_UI_SHOW_HELP						CONSTLIT("uiShowHelp")
#define CMD_UI_SHOW_SETTINGS    				CONSTLIT("uiShowSettings")

CMenuData CGameSession::CreateGameMenu (void) const

//	CreateGameMenu
//
//	Returns the game menu data structure.

	{
	CMenuData MenuData;
	MenuData.SetTitle(CONSTLIT("Transcendence"));
	MenuData.AddMenuItem(CMD_UI_SHOW_HELP, CONSTLIT("1"), CONSTLIT("[H]elp"));
	MenuData.AddMenuItem(CMD_GAME_END_SAVE, CONSTLIT("2"), CONSTLIT("[S]ave & Exit Game"));
	MenuData.AddMenuItem(CMD_GAME_SELF_DESTRUCT, CONSTLIT("3"), CONSTLIT("Self-[D]estruct"));
	MenuData.AddMenuItem(CMD_UI_SHOW_SETTINGS, NULL_STR, CONSTLIT("Keyb[o]ard Settings"));

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

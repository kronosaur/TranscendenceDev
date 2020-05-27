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

#define CMD_UI_SHOW_HELP						CONSTLIT("uiShowHelp")

CMenuData CGameSession::CreateGameMenu (void) const

//	CreateGameMenu
//
//	Returns the game menu data structur.

	{
	CMenuData MenuData;
	MenuData.SetTitle(CONSTLIT("Transcendence"));
	MenuData.AddMenuItem(CMD_UI_SHOW_HELP, CONSTLIT("1"), CONSTLIT("Help [F1]"));
	MenuData.AddMenuItem(CMD_GAME_END_SAVE, CONSTLIT("2"), CONSTLIT("Save & Quit"));
	MenuData.AddMenuItem(CMD_GAME_SELF_DESTRUCT, CONSTLIT("3"), CONSTLIT("Self-Destruct"));

	//	Debug mode includes more special functions

	if (GetUniverse().InDebugMode())
		{
		MenuData.AddMenuItem(CMD_GAME_REVERT, CONSTLIT("9"), CONSTLIT("Revert"));
		MenuData.AddMenuItem(CMD_GAME_END_DELETE, CONSTLIT("0"), CONSTLIT("Delete & Quit"));
		}

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
		m_HI.HICommand(sCmd);
		}
	else if (strEquals(sCmd, CMD_UI_SHOW_HELP))
		{
		m_HI.HICommand(sCmd);
		DismissMenu();
		}
	else
		return ERR_NOTFOUND;

	return NOERROR;
	}

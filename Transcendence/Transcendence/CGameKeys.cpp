//	CGameKeys.cpp
//
//	CGameKeys class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"
#include "Transcendence.h"
#include "DefaultKeyMappings.h"

#define COMMAND_ATTRIB						CONSTLIT("command")
#define KEY_ATTRIB							CONSTLIT("key")
#define LAYOUT_ATTRIB						CONSTLIT("layout")

#define COMMAND_NONE						CONSTLIT("noCommand")

#define LAYOUT_CUSTOM                       CONSTLIT("custom")
#define LAYOUT_DEFAULT                      CONSTLIT("default")
#define LAYOUT_WASD							CONSTLIT("wasd")

struct SGameKeyData
	{
	enum EFlags
		{
		FLAG_HIDDEN =       0x00000001,
		FLAG_DEBUG_ONLY =   0x00000002,
		FLAG_STATEFULL =	0x00000004,		//	Tracks when key is down vs. up (thrust, fire, etc.).
		FLAG_NO_REPEAT =	0x00000008,		//	Ignore repeated keys
		FLAG_XY_INPUT =		0x00000010,		//	Can only be mapped to an XY input (e.g., move position)
		};

	const char *pszName;
	const char *pszLabel;
	DWORD dwFlags;
	};

SGameKeyData g_GameKeyData[CGameKeys::keyCount] =
	{
		{	NULL },	//	keyNone

		{	"Autopilot",            "Accelerate Time",				SGameKeyData::FLAG_NO_REPEAT },
		{	"EnableDevices",        "Enable/Disable Devices",       SGameKeyData::FLAG_NO_REPEAT },
		{	"Communications",       "Communications",               SGameKeyData::FLAG_NO_REPEAT },
		{	"Dock",                 "Dock",			                SGameKeyData::FLAG_NO_REPEAT },
		{	"TargetNextFriendly",   "Target Next Friendly",         0 },
		{	"EnterGate",            "Enter Stargate",               SGameKeyData::FLAG_NO_REPEAT },
		{	"InvokePower",          "Invoke Power",                 SGameKeyData::FLAG_NO_REPEAT },
		{	"ShowMap",              "System Map",                   SGameKeyData::FLAG_NO_REPEAT },
		{	"Pause",                "Pause Game",                   SGameKeyData::FLAG_NO_REPEAT },
		{	"CommandSquadron",      "Squadron Communications",      SGameKeyData::FLAG_NO_REPEAT | SGameKeyData::FLAG_HIDDEN },
		{	"ClearTarget",          "Clear Target",                 0 },
		{	"ShipStatus",           "Ship's Status",                SGameKeyData::FLAG_NO_REPEAT },
		{	"TargetNextEnemy",      "Target Next Enemy",            0 },
		{	"UseItem",              "Use Item",                     SGameKeyData::FLAG_NO_REPEAT },
		{	"NextWeapon",           "Select Next Weapon",           SGameKeyData::FLAG_NO_REPEAT },
		{	"ThrustForward",        "Forward Thrust",               SGameKeyData::FLAG_STATEFULL },
		{	"Stop",                 "Stop Momentum",                SGameKeyData::FLAG_STATEFULL },
		{	"RotateLeft",           "Rotate Left",                  SGameKeyData::FLAG_STATEFULL },
		{	"RotateRight",          "Rotate Right",                 SGameKeyData::FLAG_STATEFULL },
		{	"FireWeapon",           "Fire Weapon",                  SGameKeyData::FLAG_STATEFULL },
		{	"FireMissile",          "Fire Missile",                 SGameKeyData::FLAG_STATEFULL },
		{	"NextMissile",          "Select Next Missile",          SGameKeyData::FLAG_NO_REPEAT },
		{	"ShowHelp",             "Help",                         SGameKeyData::FLAG_NO_REPEAT },
		{	"ShowGameStats",        "Game Stats",                   SGameKeyData::FLAG_NO_REPEAT },
		{	NULL },
		{	NULL },
		{	NULL },
		{	NULL },
		{	"VolumeUp",             "Volume Up",                    0 },
		{	"VolumeDown",           "Volume Down",                  0 },
		{	"ShowConsole",          "Debug Console",                SGameKeyData::FLAG_DEBUG_ONLY },

		{	"Special1",    NULL,                            SGameKeyData::FLAG_HIDDEN  },
		{	"Special2",    NULL,                            SGameKeyData::FLAG_HIDDEN  },
		{	"Special3",    NULL,                            SGameKeyData::FLAG_HIDDEN  },
		{	"Special4",    NULL,                            SGameKeyData::FLAG_HIDDEN  },
		{	"Special5",    NULL,                            SGameKeyData::FLAG_HIDDEN  },
		{	"Special6",    NULL,                            SGameKeyData::FLAG_HIDDEN  },
		{	"Special7",    NULL,                            SGameKeyData::FLAG_HIDDEN  },
		{	"Special8",    NULL,                            SGameKeyData::FLAG_HIDDEN  },
		{	"Special9",    NULL,                            SGameKeyData::FLAG_HIDDEN  },
		{	"Special0",    NULL,                            SGameKeyData::FLAG_HIDDEN  },

		{	"EnableAllDevices",    NULL,                            SGameKeyData::FLAG_HIDDEN },
		{	"DisableAllDevices",    NULL,                           SGameKeyData::FLAG_HIDDEN },
		{	"ToggleEnableAllDevices",    NULL,                            SGameKeyData::FLAG_HIDDEN },
		{	"ToggleEnableDevice00",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice01",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice02",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice03",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice04",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice05",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice06",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice07",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice08",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice09",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice10",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice11",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice12",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice13",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice14",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice15",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice16",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice17",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice18",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice19",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice20",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice21",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice22",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice23",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice24",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice25",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice26",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice27",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice28",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice29",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice30",    NULL,                            SGameKeyData::FLAG_HIDDEN	},
		{	"ToggleEnableDevice31",    NULL,                            SGameKeyData::FLAG_HIDDEN	},

		{	"TargetPreviousFriendly",   "Target Previous Friendly",     SGameKeyData::FLAG_NO_REPEAT | SGameKeyData::FLAG_HIDDEN	},
		{	"TargetPreviousEnemy",      "Target Previous Enemy",        SGameKeyData::FLAG_NO_REPEAT | SGameKeyData::FLAG_HIDDEN	},
		{	"PreviousWeapon",           "Select Previous Weapon",       SGameKeyData::FLAG_NO_REPEAT | SGameKeyData::FLAG_HIDDEN	},
		{	"PreviousMissile",          "Select Previous Missile",      SGameKeyData::FLAG_NO_REPEAT | SGameKeyData::FLAG_HIDDEN	},
		{	"ShowGalacticMap",          "Stargate Map",                 SGameKeyData::FLAG_NO_REPEAT },
		{	"AimShip",					"Aim Ship",						SGameKeyData::FLAG_XY_INPUT },
		{	"Interact",					"Dock or Enter Stargate",		SGameKeyData::FLAG_NO_REPEAT },
		{	"CycleTarget",				"Cycle Target",					SGameKeyData::FLAG_NO_REPEAT },
		{	"SquadronUI",				"Squadron Status",				SGameKeyData::FLAG_NO_REPEAT },
	};

const int GAME_KEY_DATA_COUNT = (sizeof(g_GameKeyData) / sizeof(g_GameKeyData[0]));

CGameKeys::CGameKeys (void)

//	CGameKeys constructor

	{
	ASSERT(GAME_KEY_DATA_COUNT == CGameKeys::keyCount);
	SetLayout(layoutDefault);
	}

void CGameKeys::ClearKey (TArray<DWORD> Map[], DWORD dwVirtKey)

//	ClearKey
//
//	Removes the key from the mappings.

	{
	for (int i = 0; i < keyCount; i++)
		{
		for (int j = 0; j < Map[i].GetCount(); j++)
			{
			if (Map[i][j] == dwVirtKey)
				{
				Map[i].Delete(j);
				j--;
				}
			}
		}
	}

void CGameKeys::ClearKeyMap (TArray<DWORD> Map[])

//	ClearKeyMap
//
//	Clears the map of commands.

	{
	for (int i = 0; i < keyCount; i++)
		Map[i].DeleteAll();
	}

CString CGameKeys::GetCommandID (Keys iCommand)

//	GetCommandID
//
//	Returns the command ID

	{
	return CString(g_GameKeyData[iCommand].pszName, -1, true);
	}

void CGameKeys::GetCommands (TArray<SCommandKeyDesc> &Result, bool bIncludeDebug) const

//  GetCommands
//
//  Returns a list of all commands and their key bindings.

	{
	Result.DeleteAll();
	Result.GrowToFit(keyCount);

	for (int i = 0; i < keyCount; i++)
		{
		const SGameKeyData &Data = g_GameKeyData[i];

		//  If no name, or if it is hidden, then we skip

		if (Data.pszName == NULL
				|| (Data.dwFlags & SGameKeyData::FLAG_HIDDEN))
			continue;

		//	If debug only, skip unless desired.

		if (!bIncludeDebug && (Data.dwFlags & SGameKeyData::FLAG_DEBUG_ONLY))
			continue;

		//  Add the command

		SCommandKeyDesc *pNewCmd = Result.Insert();
		pNewCmd->iCmd = (Keys)i;
		pNewCmd->sCmdID = CString(Data.pszName);
		if (Data.pszLabel)
			pNewCmd->sCmdLabel = CString(Data.pszLabel);
		else
			pNewCmd->sCmdLabel = CONSTLIT("(unknown)");

		//	Add the mappings

		for (int j = 0; j < GetKeyMappedToCommandCount((Keys)i); j++)
			{
			SBindingDesc *pBinding = pNewCmd->Keys.Insert();
			pBinding->dwVirtKey = GetKeyMappedToCommand((Keys)i, j);
			pBinding->sKeyID = CVirtualKeyData::GetKeyID(pBinding->dwVirtKey);
			}
		}
	}

CGameKeys::Keys CGameKeys::GetGameCommand (const CString &sCmd)

//	GetGameCommand
//
//	Returns game command from string

	{
	if (strEquals(sCmd, COMMAND_NONE))
		return CGameKeys::keyNone;

	for (int i = 0; i < CGameKeys::keyCount; i++)
		if (strEquals(sCmd, CString(g_GameKeyData[i].pszName, -1, true)))
			return (CGameKeys::Keys)i;

	return CGameKeys::keyError;
	}

CGameKeys::Keys CGameKeys::GetGameCommandFromChar (char chChar) const

//  GetGameCommand
//
//  Returns game command from character

	{
	const DWORD VK_FIRST_NUMBER = 0x30;
	const DWORD VK_FIRST_LETTER = 0x41;

	//  Convert from character to virtual key code

	if (chChar >= '0' && chChar <= '9')
		return GetGameCommand(VK_FIRST_NUMBER + (DWORD)(chChar - '0'));
	else if (chChar >= 'A' && chChar <= 'Z')
		return GetGameCommand(VK_FIRST_LETTER + (DWORD)(chChar - 'A'));
	else if (chChar >= 'a' && chChar <= 'z')
		return GetGameCommand(VK_FIRST_LETTER + (DWORD)(chChar - 'a'));
	else
		return keyError;
	}

DWORD CGameKeys::GetKey (Keys iCommand) const

//  GetKey
//
//  Returns the virtual key mapped to the given command. If there are multiple
//	keys bound to the same command, we return the first key.

	{
	return (m_CommandMap[iCommand].GetCount() > 0 ? m_CommandMap[iCommand].GetAt(0) : CVirtualKeyData::INVALID_VIRT_KEY);
	}

char CGameKeys::GetKeyIfChar (Keys iCommand) const

//	GetKeyIfChar
//
//	If the given command is bound to a character key, then return the character.
//	Otherwise we return 0.

	{
	DWORD dwVirtKey = GetKey(iCommand);

	if (dwVirtKey >= 'A' && dwVirtKey <= 'Z')
		return (char)dwVirtKey;
	else
		return '\0';
	}

CString CGameKeys::GetLayoutID (ELayouts iLayout)

//  GetLayoutID
//
//  Returns the layout ID (which is used to identify a layout in XML).

	{
	switch (iLayout)
		{
		case layoutDefault:
			return LAYOUT_DEFAULT;

		case layoutWASD:
			return LAYOUT_WASD;

		case layoutCustom:
			return LAYOUT_CUSTOM;

		default:
			return NULL_STR;
		}
	}

CGameKeys::ELayouts CGameKeys::GetLayoutFromID (const CString &sLayoutID)

//  GetLayoutFromID
//
//  Parses a layout ID and return a layout. Returns layoutNone if we cannot
//  recognize the ID.

	{
	if (sLayoutID.IsBlank() || strEquals(sLayoutID, LAYOUT_DEFAULT))
		return layoutDefault;
	else if (strEquals(sLayoutID, LAYOUT_CUSTOM))
		return layoutCustom;
	else if (strEquals(sLayoutID, LAYOUT_WASD))
		return layoutWASD;
	else
		return layoutNone;
	}

CString CGameKeys::GetLayoutName (ELayouts iLayout)

//  GetLayoutName
//
//  Returns the human readable name of the layout.

	{
	switch (iLayout)
		{
		case layoutDefault:
			return CONSTLIT("Default");

		case layoutCustom:
			return CONSTLIT("Custom");

		case layoutWASD:
			return CONSTLIT("WASD");

		default:
			return CONSTLIT("(Unknown)");
		}
	}

void CGameKeys::InitKeyMap (void) const

//	InitKeyMap
//
//	Initialize a map from key to command.

	{
	if (m_bKeyMapValid)
		return;

	//	Clear out all keys

	for (int i = 0; i < 256; i++)
		m_KeyToCommandMap[i] = keyNone;

	//	Loop over all commands and add its keys

	for (int i = 0; i < keyCount; i++)
		{
		Keys iCmd = (Keys)i;
		for (int j = 0; j < GetKeyMappedToCommandCount(iCmd); j++)
			{
			DWORD dwVirtKey = GetKeyMappedToCommand(iCmd, j);
			if (dwVirtKey > 255)
				continue;

			m_KeyToCommandMap[dwVirtKey] = iCmd;
			}
		}

	m_bKeyMapValid = true;
	}

bool CGameKeys::IsKeyDown (Keys iCommand) const

//	IsKeyDown
//
//	Returns TRUE if the key for the given command is currently down. This handles
//	multiple keys mapped to the same command.

	{
	DWORD dwVirtKey = GetKey(iCommand);
	if (dwVirtKey == CVirtualKeyData::INVALID_VIRT_KEY)
		return false;

	return ::uiIsKeyDown(dwVirtKey);
	}

bool CGameKeys::IsNonRepeatCommand (Keys iCommand)

//	IsNonRepeatCommand
//
//	Returns TRUE if this command ignored repeated keys.

	{
	if (iCommand <= 0 || iCommand >= keyCount)
		{
		ASSERT(false);
		return false;
		}

	const SGameKeyData &Data = g_GameKeyData[iCommand];
	return (Data.dwFlags & SGameKeyData::FLAG_NO_REPEAT ? true : false);
	}

bool CGameKeys::IsStatefulCommand (Keys iCommand)

//	IsStatefulCommand
//
//	Returns TRUE if this command tracks the state of a key (up or down).

	{
	if (iCommand <= 0 || iCommand >= keyCount)
		{
		ASSERT(false);
		return false;
		}

	const SGameKeyData &Data = g_GameKeyData[iCommand];
	return (Data.dwFlags & SGameKeyData::FLAG_STATEFULL ? true : false);
	}

bool CGameKeys::IsXYInputCommand (Keys iCommand)

//	IsXYInputCommand
//
//	Returns TRUE if this command needs XY input.

	{
	if (iCommand <= 0 || iCommand >= keyCount)
		{
		ASSERT(false);
		return false;
		}

	const SGameKeyData &Data = g_GameKeyData[iCommand];
	return (Data.dwFlags & SGameKeyData::FLAG_XY_INPUT ? true : false);
	}

ALERROR CGameKeys::ReadFromXML (CXMLElement *pDesc)

//	ReadFromXML
//
//	Read the key map from XML

	{
	bool bBindMouseMoveToShipAim = true;

	//  For backwards compatibility, we assume a custom layout if we don't
	//  have a layout specified.

	ELayouts iLayout = layoutCustom;
	ClearKeyMap(m_SavedCommandMap);

	//  Load the layout

	CString sLayout;
	if (pDesc->FindAttribute(LAYOUT_ATTRIB, &sLayout))
		{
		iLayout = GetLayoutFromID(sLayout);
		if (iLayout == layoutNone)
			iLayout = layoutDefault;
		}

	//	If loading old settings, then we initialize with the default map and
	//	override as appropriate

	else
		{
		for (int i = 0; i < DEFAULT_MAP_COUNT; i++)
			{
			auto &KeyList = m_SavedCommandMap[DEFAULT_MAP[i].iGameKey];
			KeyList.Insert(DEFAULT_MAP[i].iVirtKey);
			}
		}

	//	Load the map into a separate array so that we can detect
	//	duplicates

	Keys iNewMap[256];
	for (int i = 0; i < 256; i++)
		iNewMap[i] = keyError;

	for (int i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pMap = pDesc->GetContentElement(i);
		
		DWORD dwVirtKey = CVirtualKeyData::GetKey(pMap->GetAttribute(KEY_ATTRIB));
		if (dwVirtKey == CVirtualKeyData::INVALID_VIRT_KEY)
			{
			kernelDebugLogPattern("Unknown key: %s", pMap->GetAttribute(KEY_ATTRIB));
			continue;
			}

		Keys iCommand = GetGameCommand(pMap->GetAttribute(COMMAND_ATTRIB));
		if (iCommand == keyError)
			{
			kernelDebugLogPattern("Unknown game command: %s", pMap->GetAttribute(COMMAND_ATTRIB));
			continue;
			}
		else if (iCommand == keyNone)
			{
			if (dwVirtKey == CVirtualKeyData::VK_MOUSE_MOVE)
				bBindMouseMoveToShipAim = false;

			continue;
			}

		if (iNewMap[dwVirtKey] != keyError)
			{
			kernelDebugLogPattern("Duplicate key mapping: %s", pMap->GetAttribute(COMMAND_ATTRIB));
			continue;
			}

		iNewMap[dwVirtKey] = iCommand;

		//	Insert at the beginning because we want saved settings to override
		//	any defaults.

		m_SavedCommandMap[iCommand].Insert(dwVirtKey, 0);
		}

	//	If we haven't explicitly unbound mouse move, then map it to ship aim.
	//	We need this for backwards compatibility (i.e., if we load an old
	//	Settings.xml).

	if (bBindMouseMoveToShipAim 
			&& iNewMap[CVirtualKeyData::VK_MOUSE_MOVE] == keyError)
		{
		m_SavedCommandMap[keyAimShip].Insert(CVirtualKeyData::VK_MOUSE_MOVE);
		}

	//  Set the layout

	SetLayout(iLayout);
	m_bModified = false;

	return NOERROR;
	}

void CGameKeys::SetGameKey (const CString &sKeyID, Keys iCommand)

//  SetGameKey
//
//  Binds the command to the given key (in the custom layout only).

	{
	DWORD dwVirtKey = CVirtualKeyData::GetKey(sKeyID);
	if (dwVirtKey == CVirtualKeyData::INVALID_VIRT_KEY)
		return;

	//	If we're trying to change a default layout, switch over to the custom
	//	layout.

	if (m_iLayout != layoutCustom)
		{
		for (int i = 0; i < keyCount; i++)
			m_SavedCommandMap[i] = m_CommandMap[i];

		m_iLayout = layoutCustom;
		}

	//	Clear the old key

	ClearKey(m_CommandMap, dwVirtKey);
	ClearKey(m_SavedCommandMap, dwVirtKey);

	//	Update the key (and add at the beginning of the array because we want
	//	this key to take precedence.

	if (iCommand != keyNone)
		{
		m_CommandMap[iCommand].Insert(dwVirtKey, 0);
		m_SavedCommandMap[iCommand].Insert(dwVirtKey, 0);
		}

	//	Need to save out

	m_bModified = true;
	m_bKeyMapValid = false;
	}

void CGameKeys::SetLayout (ELayouts iLayout)

//  SetLayout
//
//  Switches to the given layout

	{
	switch (iLayout)
		{
		case layoutDefault:
			SetLayoutFromStatic(DEFAULT_MAP, DEFAULT_MAP_COUNT);
			break;

		case layoutWASD:
			SetLayoutFromStatic(WASD_MAP, WASD_MAP_COUNT);
			break;

		case layoutCustom:
			for (int i = 0; i < keyCount; i++)
				m_CommandMap[i] = m_SavedCommandMap[i];
			break;

		default:
			//  Unknown layout, so do nothing

			return;
		}

	m_iLayout = iLayout;
	m_bModified = true;
	m_bKeyMapValid = false;
	}

void CGameKeys::SetLayoutFromStatic (const SKeyMapEntry *pLayout, int iLayoutCount)

//  SetLayoutFromStatic
//
//  Sets the current layout from the given mapping

	{
	ClearKeyMap(m_CommandMap);

	for (int i = 0; i < iLayoutCount; i++)
		m_CommandMap[pLayout[i].iGameKey].Insert(pLayout[i].iVirtKey);

	m_bKeyMapValid = false;
	}

ALERROR CGameKeys::WriteAsXML (IWriteStream *pOutput)

//	WriteAsXML
//
//	Write the key map as an XML sub-element

	{
	ALERROR error;

	//	Open tag

	CString sData;
	sData = strPatternSubst(CONSTLIT("\t<KeyMap layout='%s'>\r\n"), GetLayoutID(m_iLayout));
	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength(), NULL))
		return error;

	//	Write out the custom map

	for (int i = 0; i < keyCount; i++)
		{
		for (int j = 0; j < m_SavedCommandMap[i].GetCount(); j++)
			{
			CString sKey = CVirtualKeyData::GetKeyID(m_SavedCommandMap[i][j]);
			if (sKey.IsBlank())
				sKey = strPatternSubst(CONSTLIT("0x%02x"), i);

			sData = strPatternSubst(CONSTLIT("\t\t<Map key=\"%s\" command=\"%s\"/>\r\n"),
					sKey,
					CString(g_GameKeyData[i].pszName, -1, true));

			if (error = pOutput->Write(sData.GetPointer(), sData.GetLength(), NULL))
				return error;
			}
		}

	//	We always write out the binding for mouse move because we use it to tell
	//	if the player has explicitly disabled it (as opposed to not being 
	//	defined in an older version of Settings.xml).

	const Keys iCmdBoundToMouseMove = GetGameCommand(CVirtualKeyData::VK_MOUSE_MOVE);
	if (iCmdBoundToMouseMove == keyNone)
		{
		sData = strPatternSubst(CONSTLIT("\t\t<Map key=\"%s\" command=\"%s\"/>\r\n"),
				CVirtualKeyData::GetKeyID(CVirtualKeyData::VK_MOUSE_MOVE),
				COMMAND_NONE);

		if (error = pOutput->Write(sData.GetPointer(), sData.GetLength(), NULL))
			return error;
		}

	//	Close tag

	sData = CONSTLIT("\t</KeyMap>\r\n");
	if (error = pOutput->Write(sData.GetPointer(), sData.GetLength(), NULL))
		return error;

	return NOERROR;
	}


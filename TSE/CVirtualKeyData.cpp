//	CVirtualKeyData.cpp
//
//	CVirtualKeyData class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CVirtualKeyData::SVirtKeyData CVirtualKeyData::m_VirtKeyData[] =
	{
		//	0x00 - 0x0F
		{	NULL,	NULL,   0	},
		{	"LButton",	NULL,   0	},
		{	"RButton",	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	"MButton",	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},

		{	"Backspace",	"Backspace",   0	},
		{	"Tab",			"Tab",   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	"Clear",		NULL,   FLAG_NON_STANDARD	},
		{	"Return",		"Enter",   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},

		//	0x10 - 0x1F
		{	"Shift",		"Shift",   0	},
		{	"Control",		"Ctrl",   0	},
		{	"Menu",			NULL,   FLAG_NON_STANDARD	},
		{	"Pause",		NULL,   FLAG_NON_STANDARD	},
		{	"Capital",		NULL,   0	},
		{	"Kana",			NULL,   FLAG_NON_STANDARD	},
		{	NULL,			NULL,   0	},
		{	"Junja",		NULL,   FLAG_NON_STANDARD	},

		{	"Final",		NULL,   FLAG_NON_STANDARD	},
		{	"Kanji",		NULL,   FLAG_NON_STANDARD	},
		{	NULL,			NULL,   0	},
		{	"Escape",		"Esc",   0	},
		{	"Convert",		NULL,   FLAG_NON_STANDARD	},
		{	"NonConvert",	NULL,   FLAG_NON_STANDARD	},
		{	"Accept",		NULL,   FLAG_NON_STANDARD	},
		{	"ModeChange",	NULL,   FLAG_NON_STANDARD	},

		//	0x20 - 0x2F
		{	"Space",		"SpaceBar",   0	},
		{	"PageUp",		"PgUp",   0	},
		{	"PageDown",		"PgDn",   0	},
		{	"End",			"End",   0	},
		{	"Home",			"Home",   0	},
		{	"Left",			"Left",   0	},
		{	"Up",			"Up",   0	},
		{	"Right",		"Right",   0	},

		{	"Down",			"Down",   0	},
		{	"Select",		NULL,   FLAG_NON_STANDARD	},
		{	"Print",		"PrtScn",   FLAG_NON_STANDARD	},
		{	"Execute",		NULL,   0	},
		{	"Snapshot",		NULL,   0	},
		{	"Insert",		"Ins",   0	},
		{	"Delete",		"Del",   0	},
		{	"Help",			NULL,   FLAG_NON_STANDARD	},

		//	0x30 - 0x3F
		{	"0",	NULL,   0	},
		{	"1",	NULL,   0	},
		{	"2",	NULL,   0	},
		{	"3",	NULL,   0	},
		{	"4",	NULL,   0	},
		{	"5",	NULL,   0	},
		{	"6",	NULL,   0	},
		{	"7",	NULL,   0	},

		{	"8",	NULL,   0	},
		{	"9",	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},

		//	0x40 - 0x4F
		{	NULL,	NULL,   0	},
		{	"A",	NULL,   0	},
		{	"B",	NULL,   0	},
		{	"C",	NULL,   0	},
		{	"D",	NULL,   0	},
		{	"E",	NULL,   0	},
		{	"F",	NULL,   0	},
		{	"G",	NULL,   0	},

		{	"H",	NULL,   0	},
		{	"I",	NULL,   0	},
		{	"J",	NULL,   0	},
		{	"K",	NULL,   0	},
		{	"L",	NULL,   0	},
		{	"M",	NULL,   0	},
		{	"N",	NULL,   0	},
		{	"O",	NULL,   0	},

		//	0x50 - 0x5F
		{	"P",	NULL,   0	},
		{	"Q",	NULL,   0	},
		{	"R",	NULL,   0	},
		{	"S",	NULL,   0	},
		{	"T",	NULL,   0	},
		{	"U",	NULL,   0	},
		{	"V",	NULL,   0	},
		{	"W",	NULL,   0	},

		{	"X",	NULL,   0	},
		{	"Y",	NULL,   0	},
		{	"Z",	NULL,   0	},
		{	"LWindows",	NULL,   FLAG_NON_STANDARD	},
		{	"RWindows",	NULL,   FLAG_NON_STANDARD	},
		{	"Apps",	NULL,   FLAG_NON_STANDARD	},
		{	NULL,	NULL,   0	},
		{	"Sleep",	NULL,   FLAG_NON_STANDARD	},

		//	0x60 - 0x6F
		{	"Numpad0",	NULL,	0	},
		{	"Numpad1",	NULL,   0	},
		{	"Numpad2",	NULL,	0	},
		{	"Numpad3",	NULL,	0	},
		{	"Numpad4",	NULL,	0	},
		{	"Numpad5",	NULL,	0	},
		{	"Numpad6",	NULL,	0	},
		{	"Numpad7",	NULL,	0	},

		{	"Numpad8",	NULL,	0	},
		{	"Numpad9",	NULL,	0	},
		{	"NumpadStar",	NULL,	0	},
		{	"NumpadPlus",	NULL,	0	},
		{	"NumpadSeparator",	NULL,   FLAG_NON_STANDARD	},
		{	"NumpadMinus",	NULL,	0	},
		{	"NumpadPeriod",	NULL,	0	},
		{	"NumpadSlash",	NULL,	0	},

		//	0x70 - 0x7F
		{	"F1",			"F1",   0	},
		{	"F2",			"F2",   0	},
		{	"F3",			"F3",   0	},
		{	"F4",			"F4",   0	},
		{	"F5",			"F5",   0	},
		{	"F6",			"F6",   0	},
		{	"F7",			"F7",   0	},
		{	"F8",			"F8",   0	},

		{	"F9",			"F9",   0	},
		{	"F10",			"F10",   0	},
		{	"F11",			"F11",   0	},
		{	"F12",			"F12",   0	},
		{	"F13",			"F13",   FLAG_NON_STANDARD	},
		{	"F14",			"F14",   FLAG_NON_STANDARD	},
		{	"F15",			"F15",   FLAG_NON_STANDARD	},
		{	"F16",			"F16",   FLAG_NON_STANDARD	},

		//	0x80 - 0x8F
		{	"F17",			"F17",   FLAG_NON_STANDARD	},
		{	"F18",			"F18",   FLAG_NON_STANDARD	},
		{	"F19",			"F19",   FLAG_NON_STANDARD	},
		{	"F20",			"F20",   FLAG_NON_STANDARD	},
		{	"F21",			"F21",   FLAG_NON_STANDARD	},
		{	"F22",			"F22",   FLAG_NON_STANDARD	},
		{	"F23",			"F23",   FLAG_NON_STANDARD	},
		{	"F24",			"F24",   FLAG_NON_STANDARD	},

		{	NULL,	NULL	},
		{	NULL,	NULL	},
		{	NULL,	NULL	},
		{	NULL,	NULL	},
		{	NULL,	NULL	},
		{	NULL,	NULL	},
		{	NULL,	NULL	},
		{	NULL,	NULL	},

		//	0x90 - 0x9F
		{	"NumLock",		"NumLock",   FLAG_NON_STANDARD	},
		{	"ScrollLock",	"ScrLock",   FLAG_NON_STANDARD	},
		{	"FJ_Jisho",	NULL,   FLAG_NON_STANDARD	},
		{	"FJ_Masshou",	NULL,   FLAG_NON_STANDARD	},
		{	"FJ_Touroku",	NULL,   FLAG_NON_STANDARD	},
		{	"FJ_Loya",	NULL,   FLAG_NON_STANDARD	},
		{	"FJ_Roya",	NULL,   FLAG_NON_STANDARD	},
		{	NULL,	NULL,   0	},

		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},

		//	0xA0 - 0xAF
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	"BrowserBack",	NULL,   FLAG_NON_STANDARD	},
		{	"BrowserForward",	NULL,   FLAG_NON_STANDARD	},

		{	"BrowserRefresh",	NULL,   FLAG_NON_STANDARD	},
		{	"BrowserStop",	NULL,   FLAG_NON_STANDARD	},
		{	"BrowserSearch",	NULL,   FLAG_NON_STANDARD	},
		{	"BrowserFavorites",	NULL,   FLAG_NON_STANDARD	},
		{	"BrowserHome",	NULL,   FLAG_NON_STANDARD	},
		{	"VolumeMute",	NULL,   FLAG_NON_STANDARD	},
		{	"VolumeDown",	NULL,   FLAG_NON_STANDARD	},
		{	"VolumeUp",	NULL,   FLAG_NON_STANDARD	},

		//	0xB0 - 0xBF
		{	"MediaNext",	NULL,   FLAG_NON_STANDARD	},
		{	"MediaPrev",	NULL,   FLAG_NON_STANDARD	},
		{	"MediaStop",	NULL,   FLAG_NON_STANDARD	},
		{	"MediaPlay",	NULL,   FLAG_NON_STANDARD	},
		{	"LaunchMail",	NULL,   FLAG_NON_STANDARD	},
		{	"LaunchMediaSelect",	NULL,   FLAG_NON_STANDARD	},
		{	"LaunchApp1",	NULL,   FLAG_NON_STANDARD	},
		{	"LaunchApp2",	NULL,   FLAG_NON_STANDARD	},

		{	NULL	},
		{	NULL	},
		{	"SemiColon",	NULL,   0	},
		{	"Equal",	NULL,   0	},
		{	"Comma",	NULL,   0	},
		{	"Minus",	NULL,   0	},
		{	"Period",	NULL,   0	},
		{	"Slash",	NULL,   0	},

		//	0xC0 - 0xCF
		{	"BackQuote",	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},

		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},

		//	0xD0 - 0xDF
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},

		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	"OpenBracket",	NULL,   0	},
		{	"Backslash",	NULL,   0	},
		{	"CloseBracket",	NULL,   0	},
		{	"Quote",	NULL,   0	},
		{	NULL,	NULL,   0	},

		//	0xE0 - 0xEF
		//	NOTE: These are normally reserved for OEMs, so we use them fo special
		//	keys which don't have their own VK code (such as Numpad Enter).

		{	"NumpadEnter",	NULL,   FLAG_SPECIAL_KEY	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},

		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},

		//	0xF0 - 0xFF
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},

		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
		{	NULL,	NULL,   0	},
	};

DWORD CVirtualKeyData::GetKey (const CString &sKey)

//	GetKey
//
//	Returns virtual key from string

	{
	int i;

	//	Handle single digits specially (otherwise we get them
	//	confused with Hex codes)

	if (sKey.GetLength() == 1)
		{
		char chChar = *sKey.GetASCIIZPointer();
		if (chChar >= '0' && chChar <= '9')
			return chChar;
		else if (chChar >= 'A' && chChar <= 'Z')
			return chChar;
		}

	//	See if this is a hex code

	DWORD dwVirtKey = ::strToInt(sKey, INVALID_VIRT_KEY);
	if (dwVirtKey != INVALID_VIRT_KEY)
		return dwVirtKey;

	//	Otherwise, look up in table

	for (i = 0; i < 256; i++)
		if (m_VirtKeyData[i].pszName && strEquals(sKey, CString(m_VirtKeyData[i].pszName, -1, true)))
			return i;

	return INVALID_VIRT_KEY;
	}

CString CVirtualKeyData::GetKeyID (DWORD dwVirtKey)

//	GetKeyID
//
//	Returns the label for the given key

	{
    if (dwVirtKey >= 256)
        return NULL_STR;

	return CString(m_VirtKeyData[dwVirtKey].pszName, -1, true);
	}

DWORD CVirtualKeyData::GetKeyFlags (DWORD dwVirtKey)

//	GetKeyFlags
//
//	Returns the flags

	{
    if (dwVirtKey >= 256)
        return 0;

	return m_VirtKeyData[dwVirtKey].dwFlags;
	}

CString CVirtualKeyData::GetKeyLabel (DWORD dwVirtKey)

//	GetKeyLabel
//
//	Returns the label for the given key

	{
    if (dwVirtKey >= 256)
        return NULL_STR;

	if (m_VirtKeyData[dwVirtKey].pszLabel)
		return CString(m_VirtKeyData[dwVirtKey].pszLabel, -1, true);
	else
		return CString(m_VirtKeyData[dwVirtKey].pszName, -1, true);
	}

DWORD CVirtualKeyData::TranslateVirtKey (DWORD dwVirtKey, DWORD dwKeyData)

//	TranslateVirtKey
//
//	We need to translate some keys

	{
	const DWORD EXTENDED_BIT = (1 << 24);
	bool bExtended = ((dwKeyData & EXTENDED_BIT) == EXTENDED_BIT);

	switch (dwVirtKey)
		{
		//	When NumLock is off, we need to translate these to their numpad
		//	equivalents.

		case VK_INSERT:
			return (!bExtended ? VK_NUMPAD0 : dwVirtKey);

		case VK_END:
			return (!bExtended ? VK_NUMPAD1 : dwVirtKey);

		case VK_DOWN:
			return (!bExtended ? VK_NUMPAD2 : dwVirtKey);

		case VK_NEXT:
			return (!bExtended ? VK_NUMPAD3 : dwVirtKey);

		case VK_LEFT:
			return (!bExtended ? VK_NUMPAD4 : dwVirtKey);

		case VK_CLEAR:
			return (!uiIsNumLockOn() ? VK_NUMPAD5 : dwVirtKey);

		case VK_RIGHT:
			return (!bExtended ? VK_NUMPAD6 : dwVirtKey);

		case VK_HOME:
			return (!bExtended ? VK_NUMPAD7 : dwVirtKey);

		case VK_UP:
			return (!bExtended ? VK_NUMPAD8 : dwVirtKey);

		case VK_PRIOR:
			return (!bExtended ? VK_NUMPAD9 : dwVirtKey);

		case VK_RETURN:
			return (bExtended ? VK_NUMPAD_ENTER : dwVirtKey);

		case VK_DELETE:
			return (!bExtended ? VK_DECIMAL : dwVirtKey);

		default:
			return dwVirtKey;
		}
	}

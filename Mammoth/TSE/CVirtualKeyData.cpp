//	CVirtualKeyData.cpp
//
//	CVirtualKeyData class
//	Copyright (c) 2018 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CVirtualKeyData::SVirtKeyData CVirtualKeyData::m_VirtKeyData[] =
	{
		//	0x00 - 0x0F
		{	NULL	},
		{	"LButton",			NULL,		"left-click",		0	},
		{	"RButton",			NULL,		"right-click",		0	},
		{	NULL,				NULL,		NULL,   0	},
		{	"MButton",			NULL,		"scroll-wheel",		FLAG_SCROLL_INPUT	},
		{	NULL,	},
		{	NULL,	},
		{	"MouseMove",		NULL,		"mouse",			FLAG_XY_INPUT | FLAG_SPECIAL_KEY	},

		{	"Backspace",		"Backspace",NULL,				0	},
		{	"Tab",				"Tab",		NULL,				0	},
		{	NULL,	},
		{	NULL,	},
		{	"Clear",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"Return",			"Enter",	NULL,				0	},
		{	NULL,	},
		{	NULL,	},

		//	0x10 - 0x1F
		{	"Shift",			"Shift",	NULL,				0	},
		{	"Control",			"Ctrl",		NULL,				0	},
		{	"Menu",				"Alt",		NULL,				FLAG_NON_STANDARD	},
		{	"Pause",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"Capital",			NULL,		NULL,				0	},
		{	"Kana",				NULL,		NULL,				FLAG_NON_STANDARD	},
		{	NULL,	},
		{	"Junja",			NULL,		NULL,				FLAG_NON_STANDARD	},

		{	"Final",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"Kanji",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	NULL,	},
		{	"Escape",			"Esc",		NULL,				0	},
		{	"Convert",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"NonConvert",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"Accept",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"ModeChange",		NULL,		NULL,				FLAG_NON_STANDARD	},

		//	0x20 - 0x2F
		{	"Space",			"SpaceBar",	"space bar",		0	},
		{	"PageUp",			"PgUp",		NULL,				0	},
		{	"PageDown",			"PgDn",		NULL,				0	},
		{	"End",				"End",		NULL,				0	},
		{	"Home",				"Home",		NULL,				0	},
		{	"Left",				"Left",		NULL,				0	},
		{	"Up",				"Up",		NULL,				0	},
		{	"Right",			"Right",	NULL,				0	},

		{	"Down",				"Down",		NULL,				0	},
		{	"Select",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"Print",			"PrtScn",	NULL,				FLAG_NON_STANDARD	},
		{	"Execute",			NULL,		NULL,				0	},
		{	"Snapshot",			NULL,		NULL,				0	},
		{	"Insert",			"Ins",		NULL,				0	},
		{	"Delete",			"Del",		NULL,				0	},
		{	"Help",				NULL,		NULL,				FLAG_NON_STANDARD	},

		//	0x30 - 0x3F
		{	"0",				NULL,		NULL,				0	},
		{	"1",				NULL,		NULL,				0	},
		{	"2",				NULL,		NULL,				0	},
		{	"3",				NULL,		NULL,				0	},
		{	"4",				NULL,		NULL,				0	},
		{	"5",				NULL,		NULL,				0	},
		{	"6",				NULL,		NULL,				0	},
		{	"7",				NULL,		NULL,				0	},

		{	"8",				NULL,		NULL,				0	},
		{	"9",				NULL,		NULL,				0	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},

		//	0x40 - 0x4F
		{	NULL,	},
		{	"A",				NULL,		NULL,				0	},
		{	"B",				NULL,		NULL,				0	},
		{	"C",				NULL,		NULL,				0	},
		{	"D",				NULL,		NULL,				0	},
		{	"E",				NULL,		NULL,				0	},
		{	"F",				NULL,		NULL,				0	},
		{	"G",				NULL,		NULL,				0	},

		{	"H",				NULL,		NULL,				0	},
		{	"I",				NULL,		NULL,				0	},
		{	"J",				NULL,		NULL,				0	},
		{	"K",				NULL,		NULL,				0	},
		{	"L",				NULL,		NULL,				0	},
		{	"M",				NULL,		NULL,				0	},
		{	"N",				NULL,		NULL,				0	},
		{	"O",				NULL,		NULL,				0	},

		//	0x50 - 0x5F
		{	"P",				NULL,		NULL,				0	},
		{	"Q",				NULL,		NULL,				0	},
		{	"R",				NULL,		NULL,				0	},
		{	"S",				NULL,		NULL,				0	},
		{	"T",				NULL,		NULL,				0	},
		{	"U",				NULL,		NULL,				0	},
		{	"V",				NULL,		NULL,				0	},
		{	"W",				NULL,		NULL,				0	},

		{	"X",				NULL,		NULL,				0	},
		{	"Y",				NULL,		NULL,				0	},
		{	"Z",				NULL,		NULL,				0	},
		{	"LWindows",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"RWindows",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"Apps",				NULL,		NULL,				FLAG_NON_STANDARD	},
		{	NULL,	},
		{	"Sleep",			NULL,		NULL,				FLAG_NON_STANDARD	},

		//	0x60 - 0x6F
		{	"Numpad0",			NULL,		NULL,				0	},
		{	"Numpad1",			NULL,		NULL,				0	},
		{	"Numpad2",			NULL,		NULL,				0	},
		{	"Numpad3",			NULL,		NULL,				0	},
		{	"Numpad4",			NULL,		NULL,				0	},
		{	"Numpad5",			NULL,		NULL,				0	},
		{	"Numpad6",			NULL,		NULL,				0	},
		{	"Numpad7",			NULL,		NULL,				0	},

		{	"Numpad8",			NULL,		NULL,				0	},
		{	"Numpad9",			NULL,		NULL,				0	},
		{	"NumpadStar",		NULL,		NULL,				0	},
		{	"NumpadPlus",		NULL,		NULL,				0	},
		{	"NumpadSeparator",	NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"NumpadMinus",		NULL,		NULL,				0	},
		{	"NumpadPeriod",		NULL,		NULL,				0	},
		{	"NumpadSlash",		NULL,		NULL,				0	},

		//	0x70 - 0x7F
		{	"F1",				"F1",		NULL,				0	},
		{	"F2",				"F2",		NULL,				0	},
		{	"F3",				"F3",		NULL,				0	},
		{	"F4",				"F4",		NULL,				0	},
		{	"F5",				"F5",		NULL,				0	},
		{	"F6",				"F6",		NULL,				0	},
		{	"F7",				"F7",		NULL,				0	},
		{	"F8",				"F8",		NULL,				0	},

		{	"F9",				"F9",		NULL,				0	},
		{	"F10",				"F10",		NULL,				0	},
		{	"F11",				"F11",		NULL,				0	},
		{	"F12",				"F12",		NULL,				0	},
		{	"F13",				"F13",		NULL,				FLAG_NON_STANDARD	},
		{	"F14",				"F14",		NULL,				FLAG_NON_STANDARD	},
		{	"F15",				"F15",		NULL,				FLAG_NON_STANDARD	},
		{	"F16",				"F16",		NULL,				FLAG_NON_STANDARD	},

		//	0x80 - 0x8F
		{	"F17",				"F17",		NULL,				FLAG_NON_STANDARD	},
		{	"F18",				"F18",		NULL,				FLAG_NON_STANDARD	},
		{	"F19",				"F19",		NULL,				FLAG_NON_STANDARD	},
		{	"F20",				"F20",		NULL,				FLAG_NON_STANDARD	},
		{	"F21",				"F21",		NULL,				FLAG_NON_STANDARD	},
		{	"F22",				"F22",		NULL,				FLAG_NON_STANDARD	},
		{	"F23",				"F23",		NULL,				FLAG_NON_STANDARD	},
		{	"F24",				"F24",		NULL,				FLAG_NON_STANDARD	},

		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},

		//	0x90 - 0x9F
		{	"NumLock",			"NumLock",	NULL,				FLAG_NON_STANDARD	},
		{	"ScrollLock",		"ScrLock",	NULL,				FLAG_NON_STANDARD	},
		{	"FJ_Jisho",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"FJ_Masshou",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"FJ_Touroku",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"FJ_Loya",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"FJ_Roya",			NULL,		NULL,				FLAG_NON_STANDARD	},
		{	NULL,				NULL,		NULL,				0	},

		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},

		//	0xA0 - 0xAF
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	"BrowserBack",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"BrowserForward",	NULL,		NULL,				FLAG_NON_STANDARD	},

		{	"BrowserRefresh",	NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"BrowserStop",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"BrowserSearch",	NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"BrowserFavorites",	NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"BrowserHome",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"VolumeMute",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"VolumeDown",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"VolumeUp",			NULL,		NULL,				FLAG_NON_STANDARD	},

		//	0xB0 - 0xBF
		{	"MediaNext",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"MediaPrev",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"MediaStop",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"MediaPlay",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"LaunchMail",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"LaunchMediaSelect",NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"LaunchApp1",		NULL,		NULL,				FLAG_NON_STANDARD	},
		{	"LaunchApp2",		NULL,		NULL,				FLAG_NON_STANDARD	},

		{	NULL	},
		{	NULL	},
		{	"SemiColon",		";",		NULL,				0	},
		{	"Equal",			"=",		NULL,				0	},
		{	"Comma",			",",		NULL,				0	},
		{	"Minus",			"-",		NULL,				0	},
		{	"Period",			".",		NULL,				0	},
		{	"Slash",			"/",		NULL,				0	},

		//	0xC0 - 0xCF
		{	"BackQuote",		"`",		NULL,				0	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},

		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},

		//	0xD0 - 0xDF
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},

		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	"OpenBracket",		"[",		NULL,				0	},
		{	"Backslash",		"\\",		NULL,				0	},
		{	"CloseBracket",		"]",		NULL,				0	},
		{	"Quote",			"\'",		NULL,				0	},
		{	NULL,	},

		//	0xE0 - 0xEF
		//	NOTE: These are normally reserved for OEMs, so we use them for special
		//	keys which don't have their own VK code (such as Numpad Enter).

		{	"NumpadEnter",		NULL,		NULL,				FLAG_SPECIAL_KEY	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},

		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},

		//	0xF0 - 0xFF
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},

		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
		{	NULL,	},
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
		else if (chChar >= 'a' && chChar <= 'z')
			return 'A' + (chChar - 'a');
		}

	//	See if this is a hex code

	DWORD dwVirtKey = ::strToInt(sKey, INVALID_VIRT_KEY);
	if (dwVirtKey != INVALID_VIRT_KEY)
		return dwVirtKey;

	//	Otherwise, look up in table

	for (i = 0; i < 256; i++)
		if (m_VirtKeyData[i].pszID 
				&& strEquals(sKey, CString(m_VirtKeyData[i].pszID, -1, true)))
			return i;
		else if (m_VirtKeyData[i].pszLabel
				&& strEquals(sKey, CString(m_VirtKeyData[i].pszLabel, -1, true)))
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

	//	If no name, we represent as a hex value

	if (m_VirtKeyData[dwVirtKey].pszID == NULL)
		return strPatternSubst(CONSTLIT("0x%02x"), dwVirtKey);

	return CString(m_VirtKeyData[dwVirtKey].pszID, -1, true);
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
		return CString(m_VirtKeyData[dwVirtKey].pszID, -1, true);
	}

CString CVirtualKeyData::GetKeyName (DWORD dwVirtKey)

//	GetKeyName
//
//	Returns the name of the given key

	{
	if (dwVirtKey >= 256)
		return NULL_STR;

	if (m_VirtKeyData[dwVirtKey].pszName)
		return CString(m_VirtKeyData[dwVirtKey].pszName, -1, true);
	else if (m_VirtKeyData[dwVirtKey].pszLabel)
		return CString(m_VirtKeyData[dwVirtKey].pszLabel, -1, true);
	else
		return CString(m_VirtKeyData[dwVirtKey].pszID, -1, true);
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

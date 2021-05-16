//	DefaultKeyMappings.h
//
//	CGameKeys class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Default Key Mapping
//
//	NOTES
//
//	* Do not use F12 as it causes a break when running with a debugger

const CGameKeys::SKeyMapEntry CGameKeys::DEFAULT_MAP[] =
	{
		{	'A',				CGameKeys::keyAutopilot },
		{	'B',				CGameKeys::keyEnableDevice },
		{	'C',				CGameKeys::keyCommunications },
		{	'D',				CGameKeys::keyDock },
		//	'E' unused
		{	'F',				CGameKeys::keyTargetNextFriendly },
		{	'G',				CGameKeys::keyEnterGate },
		//	'H' unused
		{	'I',				CGameKeys::keyInvokePower },
		//	'J' unused
		//	'K' unused
		//	'L' unused
		{	'M',				CGameKeys::keyShowMap },
		{	'N',				CGameKeys::keyShowGalacticMap },
		//	'O' unused
		{	'P',				CGameKeys::keyPause },
		{	'Q',				CGameKeys::keySquadronUI },
		{	'R',				CGameKeys::keyClearTarget },
		{	'S',				CGameKeys::keyShipStatus },
		{	'T',				CGameKeys::keyTargetNextEnemy },
		{	'U',				CGameKeys::keyUseItem },
		//	'V' unused
		{	'W',				CGameKeys::keyNextWeapon },
		//	'X' unused
		//	'Y' unused
		//	'Z' unused

		{	CVirtualKeyData::VK_MOUSE_MOVE,			CGameKeys::keyAimShip },
		{	VK_LBUTTON,			CGameKeys::keyFireWeapon },
		{	VK_MBUTTON,			CGameKeys::keyCycleTarget },
		{	VK_RBUTTON,			CGameKeys::keyThrustForward },

		{	VK_CONTROL,			CGameKeys::keyFireWeapon },
		{	VK_DOWN,			CGameKeys::keyStop },
		{	VK_LEFT,			CGameKeys::keyRotateLeft },
		{	VK_PAUSE,			CGameKeys::keyPause },
		{	VK_RIGHT,			CGameKeys::keyRotateRight },
		{	VK_SHIFT,			CGameKeys::keyFireMissile },
		{	VK_SPACE,			CGameKeys::keyFireWeapon },
		{	VK_TAB,				CGameKeys::keyNextMissile },
		{	VK_UP,				CGameKeys::keyThrustForward },

		{	VK_F1,				CGameKeys::keyShowHelp },
		{	VK_F2,				CGameKeys::keyShowGameStats },
		{	VK_F6,				CGameKeys::keyToggleHUD	},
		{	VK_F7,				CGameKeys::keyVolumeDown },
		{	VK_F8,				CGameKeys::keyVolumeUp },
		{	VK_F9,				CGameKeys::keyShowConsole },
	};

const int CGameKeys::DEFAULT_MAP_COUNT = (sizeof(DEFAULT_MAP) / sizeof(DEFAULT_MAP[0]));

const CGameKeys::SKeyMapEntry CGameKeys::WASD_MAP[] =
	{
		{	'A',				CGameKeys::keyRotateLeft },
		{	'B',				CGameKeys::keyEnableDevice },
		{	'C',				CGameKeys::keyUseItem },
		{	'D',				CGameKeys::keyRotateRight },
		//	'E' unused
		{	'F',				CGameKeys::keyAutopilot },
		{	'G',				CGameKeys::keyEnterGate },
		//	'H' unused
		//	'I' unused
		//	'J' unused
		//	'K' unused
		//	'L' unused
		//	'M' unused
		{	'N',				CGameKeys::keyShowGalacticMap },
		//	'O' unused
		{	'P',				CGameKeys::keyPause },
		//	'Q' unused
		{	'R',				CGameKeys::keyInteract },
		{	'S',				CGameKeys::keyStop },
		{	'T',				CGameKeys::keyTargetNextEnemy },
		{	'U',				CGameKeys::keySquadronUI },
		{	'V',				CGameKeys::keyCommunications },
		{	'W',				CGameKeys::keyThrustForward },
		{	'X',				CGameKeys::keyInvokePower },
		{	'Y',				CGameKeys::keyTargetNextFriendly },
		{	'Z',				CGameKeys::keyShowMap },

		{	VK_LBUTTON,			CGameKeys::keyFireWeapon },
		{	VK_MBUTTON,			CGameKeys::keyCycleTarget },
		{	VK_RBUTTON,			CGameKeys::keyFireMissile },

		//	VK_CONTROL unused
		{	VK_DOWN,			CGameKeys::keyStop },
		{	VK_LEFT,			CGameKeys::keyRotateLeft },
		//	VK_OEM_PERIOD unused
		{	VK_PAUSE,			CGameKeys::keyPause },
		//	VK_RETURN unused
		{	VK_RIGHT,			CGameKeys::keyRotateRight },
		{	VK_SHIFT,			CGameKeys::keyNextWeapon },
		{	VK_SPACE,			CGameKeys::keyShipStatus },
		{	VK_TAB,				CGameKeys::keyNextMissile },
		{	VK_UP,				CGameKeys::keyThrustForward },

		{	VK_F1,				CGameKeys::keyShowHelp },
		{	VK_F2,				CGameKeys::keyShowGameStats },
		{	VK_F6,				CGameKeys::keyToggleHUD	},
		{	VK_F7,				CGameKeys::keyVolumeDown },
		{	VK_F8,				CGameKeys::keyVolumeUp },
		{	VK_F9,				CGameKeys::keyShowConsole },
	};

const int CGameKeys::WASD_MAP_COUNT = (sizeof(WASD_MAP) / sizeof(WASD_MAP[0]));

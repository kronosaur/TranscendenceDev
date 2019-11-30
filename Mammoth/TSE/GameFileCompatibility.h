//	CGameFile.cpp
//
//	CGameFile class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SGameHeader8
	{
	DWORD dwVersion;				//	Game file format version

	DWORD dwUniverse;				//	Location of universe data
	DWORD dwSystemMap;				//	Location of system directory. The system
									//		directory is an array of file IDs
									//		indexed by system UNID and prefixed
									//		by a count.
	char szSystemName[CGameFile::GAME_HEADER_MAX_SYSTEM_NAME];
	DWORD dwFlags;					//	Flags for game
	DWORD dwResurrectCount;			//	Number of times we're been resurrected
	DWORD dwGameStats;				//	Location of game stats
	DWORD dwCreateVersion;			//	Product version that created this save file
	DWORD dwPartialSave;			//	System entry that was partially saved (while entering a gate)

	DWORD dwSpare[4];
	};

struct SGameHeader9
	{
	DWORD dwVersion;				//	Game file format version

	DWORD dwUniverse;				//	Location of universe data
	DWORD dwSystemMap;				//	Location of system directory. The system
									//		directory is an array of file IDs
									//		indexed by system UNID and prefixed
									//		by a count.
	char szSystemName[CGameFile::GAME_HEADER_MAX_SYSTEM_NAME];
	DWORD dwFlags;					//	Flags for game
	DWORD dwResurrectCount;			//	Number of times we're been resurrected
	DWORD dwGameStats;				//	Location of game stats
	DWORD dwCreateVersion;			//	Product version that created this save file
	DWORD dwPartialSave;			//	System entry that was partially saved (while entering a gate)

	//	New in SGameHeader 9
	char szUsername[CGameFile::USERNAME_MAX];	//	Username (may be NULL if not a regulation game)
	char szGameID[CGameFile::GAME_ID_MAX];		//	GameID (may be NULL if not a regulation game)
	DWORD dwAdventure;				//	UNID of adventure (extension)
	char szPlayerName[CGameFile::PLAYER_NAME_MAX];
	DWORD dwGenome;					//	Player genome
	DWORD dwPlayerShip;				//	UNID of player ship
	DWORD dwScore;					//	Current score
	char szEpitaph[CGameFile::EPITAPH_MAX];	//	Epitaph (if dead)
	};

struct SGameHeader10
	{
	DWORD dwVersion = 0;				//	Game file format version

	DWORD dwUniverse = 0;				//	Location of universe data
	DWORD dwSystemMap = 0;				//	Location of system directory. The system
										//		directory is an array of file IDs
										//		indexed by system UNID and prefixed
										//		by a count.
	char szSystemName[CGameFile::GAME_HEADER_MAX_SYSTEM_NAME] = "";
	DWORD dwFlags = 0;					//	Flags for game
	DWORD dwResurrectCount = 0;			//	Number of times we're been resurrected
	DWORD dwGameStats = 0;				//	Location of game stats
	DWORD dwCreateVersion = 0;			//	Product version that created this save file
	DWORD dwPartialSave = 0;			//	System entry that was partially saved (while entering a gate)

	//	New in SGameHeader 9
	char szUsername[CGameFile::USERNAME_MAX] = "";	//	Username (may be NULL if not a regulation game)
	char szGameID[CGameFile::GAME_ID_MAX] = "";	//	GameID (may be NULL if not a regulation game)
	DWORD dwAdventure = 0;				//	UNID of adventure (extension)
	char szPlayerName[CGameFile::PLAYER_NAME_MAX] = "";
	DWORD dwGenome = 0;					//	Player genome
	DWORD dwPlayerShip = 0;				//	UNID of player ship
	DWORD dwScore = 0;					//	Current score
	char szEpitaph[CGameFile::EPITAPH_MAX] = "";	//	Epitaph (if dead)

	//	New in SGameHeader 10
	DWORD dwCreateAPI = 0;				//	Created on this API version
	DWORD dwCreateVersionMajor = 0;
	DWORD dwCreateVersionMinor = 0;
	DWORD dwCreateVersionPoint = 0;
	DWORD dwCreateVersionBuild = 0;
	char szCreateVersion[CGameFile::VERSION_MAX] = "";
	};


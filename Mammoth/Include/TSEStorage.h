//	Storage.h
//
//	Classes and methods for saving Transcendence files
//	Copyright (c) 2012 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CGameFile
	{
	public:
		enum Flags
			{
			FLAG_CHECKPOINT =						0x00000001,
			FLAG_ENTER_GATE =						0x00000002,
			FLAG_EXIT_GATE =						0x00000004,
			FLAG_ACCEPT_MISSION =					0x00000008,
			};

		static constexpr int EPITAPH_MAX =					256;
		static constexpr int GAME_HEADER_MAX_SYSTEM_NAME =	128;
		static constexpr int GAME_ID_MAX =					128;
		static constexpr int PLAYER_NAME_MAX =				128;
		static constexpr int SHIP_IMAGE_SIZE =				96;
		static constexpr int SHIP_NAME_MAX =				128;
		static constexpr int USERNAME_MAX =					256;
		static constexpr int VERSION_MAX =					32;
		static constexpr int INVALID_ENTRY =				0xffffffff;

		CGameFile (void);
		~CGameFile (void);

		ALERROR ClearRegistered (void);
		ALERROR ClearGameResurrect(void);
		void Close (void);
		ALERROR Create (const CString &sFilename, const CString &sUsername);
		static CString GenerateFilename (const CString &sName);
		DWORD GetAdventure (void) const { return m_Header.dwAdventure; }

		static constexpr DWORD FLAG_VERSION_NUMBERS =	0x00000001;
		static constexpr DWORD FLAG_VERSION_STRING =	0x00000002;
		CString GetCreateVersion (DWORD dwFlags = FLAG_VERSION_STRING) const;

		CDifficultyOptions::ELevels GetDifficulty (void) const;
		CString GetEpitaph (void) const { return CString((char *)m_Header.szEpitaph); }
		CString GetFilespec (void) const { return (IsOpen() ? m_pFile->GetFilename() : NULL_STR); }
		CString GetGameID (void) { return CString(m_Header.szGameID); }
		DWORD GetLastSavedOn (void) const { return m_dwLastSavedOn; }
		GenomeTypes GetPlayerGenome (void) const { return (GenomeTypes)m_Header.dwGenome; }
		CString GetPlayerName (void) const;
		DWORD GetPlayerShip (void) const { return m_Header.dwPlayerShip; }
		CString GetPlayerShipClassName (void) const;
		bool GetPlayerShipImage (CG32bitImage &Image) const;
		int GetResurrectCount (void) { return m_Header.dwResurrectCount; }
		int GetScore (void) const { return (int)m_Header.dwScore; }
		CString GetSystemName (void) const;
		CString GetUsername (void) { return CString(m_Header.szUsername); }
		bool IsDebug (void) const { return ((m_Header.dwFlags & GAME_FLAG_DEBUG) ? true : false); }
		bool IsEndGame (void) const { return ((m_Header.dwFlags & GAME_FLAG_END_GAME) ? true : false); }
		bool IsGameResurrect (void) { return ((m_Header.dwFlags & GAME_FLAG_RESURRECT) ? true : false); }
		bool IsOpen (void) const { return (m_pFile != NULL); }
		bool IsRegistered (void) const { return ((m_Header.dwFlags & GAME_FLAG_REGISTERED) ? true : false); }
		bool IsUniverseValid (void) { return (m_Header.dwUniverse != INVALID_ENTRY); }

		static constexpr DWORD FLAG_NO_UPGRADE =	0x00000001;
		ALERROR Open (const CString &sFilename, DWORD dwFlags);

		ALERROR LoadGameStats (CGameStats *retStats);
		ALERROR LoadSystem (DWORD dwUNID, CSystem **retpSystem, CString *retsError, DWORD dwObjID = OBJID_NULL, CSpaceObject **retpObj = NULL, CSpaceObject *pPlayerShip = NULL);
		ALERROR LoadUniverse (CUniverse &Univ, DWORD *retdwSystemID, DWORD *retdwPlayerID, CString *retsError);
		ALERROR SaveGameStats (const CGameStats &Stats);
		ALERROR SaveSystem (DWORD dwUNID, CSystem *pSystem, DWORD dwFlags = 0);
		ALERROR SaveUniverse (CUniverse &Univ, DWORD dwFlags);
		ALERROR SetGameResurrect (void);
		ALERROR SetGameStatus (int iScore, const CString &sEpitaph, bool bEndGame = false);

	private:
		enum GameFlags
			{
			GAME_FLAG_RESURRECT =					0x00000001,	//	If we load game when flag is set, then it counts as resurrect
			GAME_FLAG_DEBUG =						0x00000002,	//	This is a debug game
			GAME_FLAG_IN_STARGATE =					0x00000004,	//	We are in the middle of entering a stargate
			GAME_FLAG_REGISTERED =					0x00000008,	//	This is a registered game
			GAME_FLAG_END_GAME =					0x00000010,	//	This game is done and cannot be continued

			GAME_FLAG_DIFFICULTY_MASK =				0x00000020 | 0x00000040,
			GAME_FLAG_DIFFICULTY_SHIFT =			5,
			};

		struct SGameHeader
			{
			DWORD dwVersion = 0;				//	Game file format version

			DWORD dwUniverse = INVALID_ENTRY;	//	Location of universe data
			DWORD dwSystemMap = INVALID_ENTRY;	//	Location of system directory. The system
												//		directory is an array of file IDs
												//		indexed by system UNID and prefixed
												//		by a count.
			char szSystemName[GAME_HEADER_MAX_SYSTEM_NAME] = "";
			DWORD dwFlags = 0;					//	Flags for game
			DWORD dwResurrectCount = 0;			//	Number of times we're been resurrected
			DWORD dwGameStats = 0;				//	Location of game stats
			DWORD dwCreateVersion = 0;			//	Product version that created this save file
			DWORD dwPartialSave = 0;			//	System entry that was partially saved (while entering a gate)

			//	New in SGameHeader 9
			char szUsername[USERNAME_MAX] = "";	//	Username (may be NULL if not a regulation game)
			char szGameID[GAME_ID_MAX] = "";	//	GameID (may be NULL if not a regulation game)
			DWORD dwAdventure = 0;				//	UNID of adventure (extension)
			char szPlayerName[PLAYER_NAME_MAX] = "";
			DWORD dwGenome = 0;					//	Player genome
			DWORD dwPlayerShip = 0;				//	UNID of player ship
			DWORD dwScore = 0;					//	Current score
			char szEpitaph[EPITAPH_MAX] = "";	//	Epitaph (if dead)

			//	New in SGameHeader 10
			DWORD dwCreateAPI = 0;				//	Created on this API version
			DWORD dwCreateVersionMajor = 0;
			DWORD dwCreateVersionMinor = 0;
			DWORD dwCreateVersionPoint = 0;
			DWORD dwCreateVersionBuild = 0;
			char szCreateVersion[VERSION_MAX] = "";

			//	New in SGameHeader 11
			DWORD dwShipImage = INVALID_ENTRY;	//	Location of ship image
			char szShipClassName[SHIP_NAME_MAX] = "";
			};

		struct SSystemData
			{
			DWORD dwEntry = 0;				//	Entry in data file
			bool bCompressed = false;		//	Entry is compressed
			};

		ALERROR ComposeLoadError (const CString &sError, CString *retsError);
		ALERROR LoadGameHeader (SGameHeader *retHeader);
		void LoadSystemMapFromStream (DWORD dwVersion, const CString &sStream);
		ALERROR SaveShipImage (CUniverse &Universe, CSpaceObject &ShipObj);
		ALERROR SaveGameHeader (SGameHeader &Header);
		void SaveSystemMapToStream (CString *retsStream);

		static DWORD EncodeDifficulty (CDifficultyOptions::ELevels iLevel);

		int m_iRefCount = 0;
		DWORD m_dwLastSavedOn = 0;					//	Last tick that we saved on this session

		CDataFile *m_pFile = NULL;

		int m_iHeaderID = 0;						//	Entry of header
		SGameHeader m_Header;						//	Loaded header
		TSortMap<DWORD, SSystemData> m_SystemMap;	//	Map from system ID to save file ID
	};


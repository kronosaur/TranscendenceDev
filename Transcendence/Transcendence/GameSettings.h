//  GameSettings.h
//
//  Game settings classes and functions
//  Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	Key mapping class ---------------------------------------------------------

class CGameKeys
	{
	public:
		enum ELayouts
			{
			layoutNone =                -1,

			layoutDefault =             0,  //  Default layout
			layoutWASD =				1,  //  WASD
			layoutCustom =              2,  //  Configured by player

			layoutCount =               3,
			};

		enum Keys
			{
			keyError =					-1,
			keyNone =					0,

			keyAutopilot =				1,	//	'A'
			keyEnableDevice =			2,	//	'B'
			keyCommunications =			3,	//	'C'
			keyDock =					4,	//	'D'
			keyTargetNextFriendly =		5,	//	'F'
			keyEnterGate =				6,	//	'G'
			keyInvokePower =			7,	//	'I'
			keyShowMap =				8,	//	'M'
			keyPause =					9,	//	'P' and VK_PAUSE
			keySquadronCommands =		10,	//	'Q'
			keyClearTarget =			11,	//	'R'
			keyShipStatus =				12,	//	'S'
			keyTargetNextEnemy =		13,	//	'T'
			keyUseItem =				14,	//	'U'
			keyNextWeapon =				15,	//	'W'
			keyThrustForward =			16,	//	Up and Down
			keyStop =					17,	//	VK_OEM_PERIOD
			keyRotateLeft =				18,	//	left
			keyRotateRight =			19,	//	right
			keyFireWeapon =				20,	//	Ctrl and Space
			keyFireMissile =			21,	//	Shift and Return
			keyNextMissile =			22,	//	Tab
			keyShowHelp =				23,	//	F1
			keyShowGameStats =			24,	//	F2
			keyReserved3 =				25,	//	F3
			keyReserved4 =				26,	//	F4
			keyReserved5 =				27,	//	F5
			keyReserved6 =				28,	//	F6
			keyVolumeUp =				29,	//	F7
			keyVolumeDown =				30,	//	F8
			keyShowConsole =			31,	//	F9

			keySpecial1 =				32,	//	'1'
			keySpecial2 =				33,	//	'2'
			keySpecial3 =				34,	//	'3'
			keySpecial4 =				35,	//	'4'
			keySpecial5 =				36,	//	'5'
			keySpecial6 =				37,	//	'6'
			keySpecial7 =				38,	//	'7'
			keySpecial8 =				39,	//	'8'
			keySpecial9 =				40,	//	'9'
			keySpecial0 =				41,	//	'0'

			keyEnableAllDevices =		42,
			keyDisableAllDevices =		43,
			keyEnableAllDevicesToggle =	44,
			keyEnableDeviceToggle00 =	45,
			keyEnableDeviceToggle01 =	46,
			keyEnableDeviceToggle02 =	47,
			keyEnableDeviceToggle03 =	48,
			keyEnableDeviceToggle04 =	49,
			keyEnableDeviceToggle05 =	50,
			keyEnableDeviceToggle06 =	51,
			keyEnableDeviceToggle07 =	52,
			keyEnableDeviceToggle08 =	53,
			keyEnableDeviceToggle09 =	54,
			keyEnableDeviceToggle10 =	55,
			keyEnableDeviceToggle11 =	56,
			keyEnableDeviceToggle12 =	57,
			keyEnableDeviceToggle13 =	58,
			keyEnableDeviceToggle14 =	59,
			keyEnableDeviceToggle15 =	60,
			keyEnableDeviceToggle16 =	61,
			keyEnableDeviceToggle17 =	62,
			keyEnableDeviceToggle18 =	63,
			keyEnableDeviceToggle19 =	64,
			keyEnableDeviceToggle20 =	65,
			keyEnableDeviceToggle21 =	66,
			keyEnableDeviceToggle22 =	67,
			keyEnableDeviceToggle23 =	68,
			keyEnableDeviceToggle24 =	69,
			keyEnableDeviceToggle25 =	70,
			keyEnableDeviceToggle26 =	71,
			keyEnableDeviceToggle27 =	72,
			keyEnableDeviceToggle28 =	73,
			keyEnableDeviceToggle29 =	74,
			keyEnableDeviceToggle30 =	75,
			keyEnableDeviceToggle31 =	76,

			keyTargetPrevFriendly =		77,
			keyTargetPrevEnemy =		78,
			keyPrevWeapon =				79,
			keyPrevMissile =			80,
			keyShowGalacticMap =		81,
			keyAimShip =				82,
			keyInteract =				83,
			keyCycleTarget =			84,

			keyCount =					85,
			};

		struct SBindingDesc
			{
			CString sKeyID;
			DWORD dwVirtKey;
			};

		struct SCommandKeyDesc
			{
			CGameKeys::Keys iCmd;
			CString sCmdID;
			CString sCmdLabel;

			TArray<SBindingDesc> Keys;
			};

		CGameKeys (void);

		void GetCommands (TArray<SCommandKeyDesc> &Result, bool bIncludeDebug) const;
		Keys GetGameCommand (DWORD dwVirtKey) const { InitKeyMap(); return m_KeyToCommandMap[(dwVirtKey < 256 ? dwVirtKey : 0)]; }
		Keys GetGameCommandFromChar (char chChar) const;
		char GetKeyIfChar (Keys iCommand) const;
		DWORD GetKey (Keys iCommand) const;
		ELayouts GetLayout (void) const { return m_iLayout; }
		bool IsKeyBound (Keys iCommand) const { return GetKey(iCommand) != CVirtualKeyData::INVALID_VIRT_KEY; }
		bool IsKeyDown (Keys iCommand) const;
		bool IsModified (void) const { return m_bModified; }
		static bool IsNonRepeatCommand (Keys iCommand);
		static bool IsStatefulCommand (Keys iCommand);
		static bool IsXYInputCommand (Keys iCommand);
		ALERROR ReadFromXML (CXMLElement *pDesc);
		void SetGameKey (const CString &sKeyID, Keys iCommand);
		void SetLayout (ELayouts iLayout);
		ALERROR WriteAsXML (IWriteStream *pOutput);

		static CString GetCommandID (Keys iCommand);
		static CGameKeys::Keys GetGameCommand (const CString &sCmd);
		static CString GetLayoutName (ELayouts iLayout);

	private:
		struct SKeyMapEntry
			{
			int iVirtKey;
			CGameKeys::Keys iGameKey;
			};

		void ClearKey (TArray<DWORD> Map[], DWORD dwVirtKey);
		void ClearKeyMap (TArray<DWORD> Map[]);
		int GetKeyMappedToCommandCount (Keys iCommand) const { return m_CommandMap[iCommand].GetCount(); }
		DWORD GetKeyMappedToCommand (Keys iCommand, int iKeyIndex) const { return m_CommandMap[iCommand].GetAt(iKeyIndex); }
		void InitKeyMap (void) const;
		void SetLayoutFromStatic (const SKeyMapEntry *pLayout, int iLayoutCount);

		static CString GetLayoutID (ELayouts iLayout);
		static ELayouts GetLayoutFromID (const CString &sLayoutID);

		ELayouts m_iLayout = layoutNone;			//  Current layout to use
		TArray<DWORD> m_CommandMap[keyCount];		//	Current map from command to ordered array of virtual keys for that command

		TArray<DWORD> m_SavedCommandMap[keyCount];	//	Last defined custom map (to switch between default and custom)

		bool m_bModified = false;					//  TRUE if modified since we loaded

		mutable Keys m_KeyToCommandMap[256];		//	Map from virtual key to command
		mutable bool m_bKeyMapValid = false;		//	TRUE if m_KeyToCommandMap is valid

		static const SKeyMapEntry DEFAULT_MAP[];
		static const int DEFAULT_MAP_COUNT;

		static const SKeyMapEntry WASD_MAP[];
		static const int WASD_MAP_COUNT;
	};

//	Game settings class -------------------------------------------------------

class IExtraSettingsHandler
	{
	public:
		virtual ALERROR OnLoadSettings (CXMLElement *pDesc, bool *retbModified) { return NOERROR; }
		virtual ALERROR OnSaveSettings (IWriteStream *pOutput) { return NOERROR; }
	};

class CGameSettings
	{
	public:
		//	TO ADD A NEW OPTION:
		//
		//	1.	Add an entry here. OK to add in the middle.
		//	2.	Increment OPTIONS_COUNT
		//	3.	In CGameSettings.cpp add an entry to g_OptionData at the appropriate spot.

		enum Options
			{
			//	Game play options
			playerName,						//	Default player name
			playerGenome,					//	Default player genome ("humanMale" or "humanFemale")
			playerShipClass,				//	Default player ship class
			lastAdventure,					//	Last adventure created
			lastDifficulty,					//	Last difficulty level played

			dockPortIndicator,				//	Options for dock port indicator
			allowInvokeLetterHotKeys,		//	Allow invoke entries to have letter hot keys
			noAutoSave,						//	NOT YET IMPLEMENTED
			noFullCreate,					//	If TRUE, we don't create all systems in the topology
			showManeuverEffects,			//	Shows maneuvering effects
			noMissionCheckpoint,			//	Do not save on mission accept
			defaultBuyTab,					//	Default tab when buying items
			noMouseAim,						//	Disable mouse aiming
			forcePermadeath,				//	We treat a game file as ended when we would normally resurrect
			showIconBar,					//	Show or hide icon bar

			//	Installation options
			useTDB,							//	Force use of .TDB
			noAutoUpdate,					//	Do not auto upgrade the game

			//	Video options
			forceDirectX,					//	Use DirectX to access screen directly
			forceNonDirectX,				//	Use GDI to draw to screen
			forceOpenGL,	 				//	Use OpenGL to draw to screen
			forceExclusive,					//	Blt directly to primary surface
			forceNonExclusive,				//	Blt using cooperative mode
			windowedMode,					//	Use a window (instead of full screen)
			useBackgroundBlt,				//	Blt in the background
			force1024Res,					//	Force 1024x768 resolution
			force600Res,					//	Force 1024x600 resolution
			force1280Res,					//	Force 1280x768 resolution
			graphicsQuality,				//	SFX vs performance
			no3DExtras,						//	Do not create out of plane objects
			no3DSystemMap,					//	3D system map projection
			maxResolution,					//	Maximum vertical resolution
			noGPUAcceleration,				//	Do not use GPU textures for blts

			//	Sounds options
			noSound,						//	No sound (either music or sound effects)
			noMusic,						//	TRUE if we should play music
			soundVolume,					//	FX volume level
			musicVolume,					//	Music volume level
			musicPath,						//	Path to music folder (may be NULL_STR)

			//  Accessibility options
			colorIFFPlayer,					//	Color of player & owned assets on minimap
			colorIFFFriendly,				//	Color of friendly assets on minimap
			colorIFFNeutral,				//	Color of neutral assets on minimap
			colorIFFEnemy,					//	Color of enemy assets on minimap
			colorIFFAngry,					//	Color of angered assets on minimap
			colorIFFEscort,				//	Color of player's non-owned escorts on minimap
			colorIFFProjectile,			//	Color of weapon projectiles on mimimap

			//	Debug options
			debugMode,						//	Program in debug mode
			debugGame,						//	Game instance in debug mode
			debugSaveFiles,					//	Adds debug info to game load screen
			noDebugLog,						//	Do not write out a debug log
			debugVideo,						//	Write out video information
			noCrashPost,					//	Do not post crash log to Multiverse
			noCollectionDownload,			//	Do not automatically download collection
			noCollectionLoad,			    //	Do not load the collection directory (debug only)
			debugSoundtrack,				//	Soundtrack debugging UI
			introSpoilers,					//	Show high-level ships in intro

			//	Constants
			OPTIONS_COUNT = 54,
			};

		CGameSettings (IExtraSettingsHandler *pExtra = NULL) : m_pExtra(pExtra) { }

		const CString &GetAppDataFolder (void) const { return m_sAppData; }
		bool GetBoolean (int iOption) const { return m_Options[iOption].bValue; }
		void GetDefaultExtensions (DWORD dwAdventure, bool bDebugMode, TArray<DWORD> *retList) const { m_Extensions.GetList(dwAdventure, bDebugMode, retList); }
		void GetDefaultExtensions (DWORD dwAdventure, const TArray<CExtension *> &Available, bool bDebugMode, TArray<DWORD> *retList) const { m_Extensions.GetList(dwAdventure, Available, bDebugMode, retList); }
		const TSortMap<DWORD, bool> &GetDisabledExtensionList (void) const { return m_Extensions.GetDisabledExtensionList(); }
		const TArray<CString> &GetExtensionFolders (void) const { return m_ExtensionFolders; }
		const CString &GetInitialSaveFile (void) const { return m_sSaveFile; }
		int GetInteger (int iOption) const { return m_Options[iOption].iValue; }
		CGameKeys &GetKeyMap (void) { return m_KeyMap; }
		const CGameKeys &GetKeyMap (void) const { return m_KeyMap; }
		const CString &GetString (int iOption) const { return m_Options[iOption].sValue; }
		ALERROR Load (const CString &sFilespec, CString *retsError = NULL);
		ALERROR ParseCommandLine (char *pszCmdLine);
		ALERROR Save (const CString &sFilespec);
		void SetBoolean (int iOption, bool bValue, bool bModifySettings = true) { SetValueBoolean(iOption, bValue, bModifySettings); if (bModifySettings) m_bModified = true; }
		void SetDefaultExtensions (DWORD dwAdventure, const TArray<CExtension *> &Available, bool bDebugMode, const TArray<DWORD> &List) { m_Extensions.SetList(dwAdventure, Available, bDebugMode, List); m_bModified = true; }
		void SetExtensionEnabled (DWORD dwUNID, bool bEnabled = true) { m_Extensions.SetExtensionEnabled(dwUNID, bEnabled); m_bModified = true; }
		void SetInteger (int iOption, int iValue, bool bModifySettings = true) { SetValueInteger(iOption, iValue, bModifySettings); if (bModifySettings) m_bModified = true; }
		void SetModified (void) { m_bModified = true; }
		void SetSettingsHandler (IExtraSettingsHandler *pExtra) { m_pExtra = pExtra; }
		void SetString (int iOption, const CString &sValue, bool bModifySettings = true) { SetValueString(iOption, sValue, bModifySettings); if (bModifySettings) m_bModified = true; }

	private:
		struct SOption
			{
			bool bValue;					//	Current boolean value
			int iValue;						//	Current integer value
			CString sValue;					//	Current string value

			CString sSettingsValue;			//	Value of this option in settings file
											//	NOTE: This may differ from the current value
											//	above if we don't want to save a setting
			};

		void LoadFromRegistry (void);
		void SetValue (int iOption, const CString &sValue, bool bSetSettings = false);
		void SetValueBoolean (int iOption, bool bValue, bool bSetSettings = false);
		void SetValueInteger (int iOption, int iValue, bool bSetSettings = false);
		void SetValueString (int iOption, const CString &sValue, bool bSetSettings = false);

		IExtraSettingsHandler *m_pExtra = NULL;	//	Additional settings handler
		SOption m_Options[OPTIONS_COUNT];	//	Options
		CGameKeys m_KeyMap;					//	Key map
		CExtensionListMap m_Extensions;		//	Default extensions

		CString m_sAppData;					//	Location of Settings.xml
		TArray<CString> m_SaveFileFolders;	//	List of folders for save files (1st is default)
		TArray<CString> m_ExtensionFolders;//	List of folders for extensions (may be empty)

		CString m_sSaveFile;				//	Optional save file to open on game start

		bool m_bModified = false;			//	If TRUE, we need to save out settings
	};


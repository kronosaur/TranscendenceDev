//	TSEGameRecords.h
//
//	Defines classes and interfaces for game records, high scores, etc.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

struct SBasicGameStats
	{
	int iSystemsVisited;
	int iEnemiesDestroyed;
	DWORD dwBestEnemyDestroyed;
	int iBestEnemyDestroyedCount;
	};

class CGameRecord
	{
	public:
		enum EDescParts
			{
			descCharacter =				0x00000001,
			descShip =					0x00000002,
			descEpitaph =				0x00000004,
			descDate =					0x00000008,
			descPlayTime =				0x00000010,
			descResurrectCount =		0x00000020,

			descAll =					0x0000003F,
			};

		enum Flags
			{
			FLAG_OMIT_WAS =				0x00000001,	//	Remove leading "was" from "was destroyed..."
			};

		CGameRecord (void);

		CString GetAdventureID (void) const;
		inline DWORD GetAdventureUNID (void) const { return m_dwAdventure; }
		CString GetDescription (DWORD dwParts = descAll) const;
		CString GetEndGameEpitaph (DWORD dwFlags = 0) const;
		inline const CString &GetEndGameReason (void) const { return m_sEndGameReason; }
		inline const CString &GetGameID (void) const { return m_sGameID; }
		inline GenomeTypes GetPlayerGenome (void) const { return m_iGenome; }
		inline const CString &GetPlayerName (void) const { return m_sName; }
		inline CString GetPlayTimeString (void) const { return m_Duration.Format(NULL_STR); }
		inline int GetResurrectCount (void) const { return m_iResurrectCount; }
		inline int GetScore (void) const { return m_iScore; }
		inline CString GetShipClass (void) const { return m_sShipClass; }
		inline const CString &GetUsername (void) const { return m_sUsername; }
		ALERROR InitFromJSON (const CJSONValue &Value);
		ALERROR InitFromXML (CXMLElement *pDesc);
		inline bool IsDebug (void) const { return m_bDebugGame; }
		inline bool IsRegistered (void) const { return m_bRegisteredGame; }
		static GenomeTypes LoadGenome (const CString &sAttrib);
		void SaveToJSON (CJSONValue *retOutput) const;
		inline void SetAdventureUNID (DWORD dwUNID) { m_dwAdventure = dwUNID; }
		inline void SetDebug (bool bDebug = true) { m_bDebugGame = bDebug; }
		inline void SetEndGameEpitaph (const CString &sEpitaph) { m_sEpitaph = sEpitaph; }
		inline void SetEndGameReason (const CString &sReason) { m_sEndGameReason = sReason; }
		inline void SetExtensions (const TArray<DWORD> &Extensions) { m_Extensions = Extensions; }
		inline void SetGameID (const CString &sGameID) { m_sGameID = sGameID; }
		inline void SetPlayerGenome (GenomeTypes iGenome) { m_iGenome = iGenome; }
		inline void SetPlayerName (const CString &sName) { m_sName = sName; }
		inline void SetPlayTime (const CTimeSpan &Time) { m_Duration = Time; }
		inline void SetRegistered (bool bRegistered = true) { m_bRegisteredGame = bRegistered; }
		inline void SetResurrectCount (int iCount) { m_iResurrectCount = iCount; }
		inline void SetScore (int iScore) { m_iScore = iScore; }
		void SetShipClass (DWORD dwUNID);
		void SetSystem (CSystem *pSystem);
		inline void SetUsername (const CString &sUsername) { m_sUsername = sUsername; }
		ALERROR WriteToXML (IWriteStream &Stream);

	private:
		//	Basic
		CString m_sUsername;					//	Username
		CString m_sGameID;						//	ID of game played
		DWORD m_dwAdventure;					//	UNID of adventure
		TArray<DWORD> m_Extensions;				//	UNID of included extensions

		CString m_sName;						//	Character name
		GenomeTypes m_iGenome;					//	Character genome

		DWORD m_dwShipClass;					//	Ship class UNID
		CString m_sShipClass;					//	Ship class
		CString m_sSystem;						//	NodeID of current system
		CString m_sSystemName;					//	Name of current system

		CTimeDate m_CreatedOn;					//	Game created on this date (set by server)
		CTimeDate m_ReportedOn;					//	Time/date of latest report (set by server)
		CTimeSpan m_Duration;					//	Time played
		bool m_bRegisteredGame;					//	If TRUE, this is a registered game
		bool m_bDebugGame;						//	If TRUE, this is a debug game

		//	Stats
		int m_iScore;
		int m_iResurrectCount;

		//	End Game
		CString m_sEndGameReason;
		CString m_sEpitaph;
	};

class CGameStats
	{
	public:
		inline void DeleteAll (void) { m_Stats.DeleteAll(); }
		inline int GetCount (void) const { return m_Stats.GetCount(); }
		inline const CString &GetDefaultSectionName (void) const { return m_sDefaultSectionName; }
		void GetEntry (int iIndex, CString *retsStatName, CString *retsStatValue, CString *retsSection) const;
		void Insert (CDesignType *pType, ICCItem *pAchievement);
		void Insert (const CString &sStatName, const CString &sStatValue = NULL_STR, const CString &sSection = NULL_STR, const CString &sSortKey = NULL_STR);
		ALERROR LoadFromStream (IReadStream *pStream);
		void SaveToJSON (CJSONValue *retOutput) const;
		ALERROR SaveToStream (IWriteStream *pStream) const;
		inline void SetDefaultSectionName (const CString &sName) { m_sDefaultSectionName = sName; }
		inline void Sort (void) { m_Stats.Sort(); }
		void TakeHandoff (CGameStats &Source);
		ALERROR WriteAsText (IWriteStream *pOutput) const;

	private:
		struct SStat
			{
			//	Needed for TArray's sort method
			bool operator > (const SStat &Right) const { return (strCompareAbsolute(sSortKey, Right.sSortKey) == 1); }
			bool operator < (const SStat &Right) const { return (strCompareAbsolute(sSortKey, Right.sSortKey) == -1); }

			CString sSortKey;
			CString sStatName;
			CString sStatValue;
			};

		static CString GetTextValue (CDesignType *pType, const CString &sIDField, const CString &sTextField, ICCItem *pEntry);
		static CString ParseAchievementSection (ICCItem *pItem);
		static CString ParseAchievementSort (ICCItem *pItem);
		static CString ParseAchievementValue (ICCItem *pItem);
		static void ParseSortKey (const CString &sSortKey, CString *retsSection, CString *retsSectionSortKey);

		TArray<SStat> m_Stats;
		CString m_sDefaultSectionName;
	};

class CAdventureHighScoreList
	{
	public:
		struct SSelect
			{
			DWORD dwAdventure;
			CString sUsername;
			int iScore;
			};

		CAdventureHighScoreList (DWORD dwAdventure = 0) : m_dwAdventure(dwAdventure),
				m_iSelection(-1)
			{ }

		inline void DeleteAll (void) { m_HighScores.DeleteAll(); m_iSelection = -1; }
		inline int GetCount (void) const { return m_HighScores.GetCount(); }
		inline int GetSelection (void) const { return m_iSelection; }
		ALERROR InitFromJSON (DWORD dwAdventureUNID, const CJSONValue &Value);
		void InsertSimpleScore (const CString &sUsername, int iScore);
		inline const CGameRecord &GetEntry (int iIndex) const { return m_HighScores[iIndex]; }
		void SetSelection (const CString &sUsername, int iScore);

	private:
		DWORD m_dwAdventure;
		TArray<CGameRecord> m_HighScores;
		int m_iSelection;
	};

class CAdventureRecord
	{
	public:
		enum ESpecialScoreIDs
			{
			personalBest =			10000,
			mostRecent =			10001,

			specialIDFirst =		10000,
			specialIDCount =		2,
			};

		CAdventureRecord (void) : m_dwAdventure(0) { }
		CAdventureRecord (DWORD dwAdventure, int iHighScoreCount);

		bool FindRecordByGameID (const CString &sGameID, DWORD *retdwID = NULL) const;
		CString GetAdventureName (void) const;
		DWORD GetAdventureUNID (void) const { return m_dwAdventure; }
		inline int GetHighScoreCount (void) const { return m_HighScores.GetCount(); }
		CGameRecord &GetRecordAt (DWORD dwID);
		void Init (DWORD dwAdventure);
		CGameRecord &InsertHighScore (void) { return *m_HighScores.Insert(); }
		inline bool IsSpecialID (DWORD dwID) const { return (dwID >= specialIDFirst && dwID < (specialIDFirst + specialIDCount)); }

	private:
		DWORD m_dwAdventure;
		TArray<CGameRecord> m_HighScores;
		CGameRecord m_Special[specialIDCount];
	};

class CUserProfile
	{
	public:
		CUserProfile (void) { }

		bool FindAdventureRecord (DWORD dwAdventure, int *retiIndex = NULL);
		inline CAdventureRecord &GetAdventureRecord (int iIndex) { return m_Records[iIndex]; }
		inline int GetAdventureRecordCount (void) { return m_Records.GetCount(); }
		void Init (const CString &sUsername);
		CAdventureRecord &InsertAdventureRecord (DWORD dwAdventure);

	private:
		CString m_sUsername;
		TArray<CAdventureRecord> m_Records;
	};


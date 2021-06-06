//	TSEPlayer.h
//
//	Player classes
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

class CPlayerGameStats
	{
	public:
		enum EEventTypes
			{
			eventNone =							0,

			eventEnemyDestroyedByPlayer	=		1,		//	The player destroyed something important
			eventFriendDestroyedByPlayer =		2,		//	The player destroyed something important
			eventSavedByPlayer =				3,		//	The player saved something from being destroyed
			eventMajorDestroyed =				4,		//	A major object was destroyed, but not by the player
			eventMissionSuccess =				5,		//	The player successfully completed a mission
			eventMissionFailure =				6,		//	The player failed a mission
			};

		CPlayerGameStats (CUniverse &Universe);

		int CalcEndGameScore (void) const;
        ICCItem *FindProperty (const CString &sProperty) const;
		void GenerateGameStats (CGameStats &Stats, CSpaceObject *pPlayer, bool bGameOver) const;
		int GetBestEnemyShipsDestroyed (DWORD *retdwUNID = NULL) const;
        CTimeSpan GetGameTime (void) const;
		CString GetItemStat (const CString &sStat, ICCItem *pItemCriteria) const;
		CString GetKeyEventStat (const CString &sStat, const CString &sNodeID, const CDesignTypeCriteria &Crit) const;
        CTimeSpan GetPlayTime (void) const;
		ICCItemPtr GetStat (const CString &sStat) const;
		CString GetStatString (const CString &sStat) const;
		DWORD GetSystemEnteredTime (const CString &sNodeID);
        DWORD GetSystemLastVisitedTime (const CString &sNodeID);
		ICCItemPtr GetSystemStat (const CString &sStat, const CString &sNodeID) const;
		bool HasVisitedMultipleSystems (void) const;
		int IncItemStat (const CString &sStat, DWORD dwUNID, int iInc);
		int IncScore (int iScore) { m_iScore = Max(0, m_iScore + iScore); return m_iScore; }
		int IncStat (const CString &sStat, int iInc = 1);
		int IncSystemStat (const CString &sStat, const CString &sNodeID, int iInc);
        void OnFuelConsumed (CSpaceObject *pPlayer, Metric rFuel) { m_rFuelConsumed += rFuel; }
		void OnGameEnd (CSpaceObject *pPlayer);
		void OnItemBought (const CItem &Item, CurrencyValue iTotalPrice);
		void OnItemDamaged (const CItem &Item, int iHP);
		void OnItemFired (const CItem &Item);
		void OnItemInstalled (const CItem &Item);
		void OnItemSold (const CItem &Item, CurrencyValue iTotalPrice);
		void OnItemUninstalled (const CItem &Item);
		void OnKeyEvent (EEventTypes iType, CSpaceObject *pObj, DWORD dwCauseUNID);
		void OnObjDestroyedByPlayer (const SDestroyCtx &Ctx, CSpaceObject *pPlayer);
		void OnPowerInvoked (const CPower &Power);
		void OnSwitchPlayerShip (const CShip &NewShip, const CShip *pOldShip = NULL);
		void OnSystemEntered (CSystem *pSystem, int *retiLastVisit = NULL);
		void OnSystemLeft (CSystem *pSystem);
		void ReadFromStream (SLoadCtx &Ctx);
		void SetStat (const CString &sStat, const CString &sValue);
		void WriteToStream (IWriteStream *pStream);

	private:
		static constexpr DWORD INVALID_TIME = 0xffffffff;

		struct SItemTypeStats
			{
			int iCountSold = 0;						//	Number of items sold
			CurrencyValue iValueSold = 0;			//	Total value received for selling (credits)

			int iCountBought = 0;					//	Number of items bought
			CurrencyValue iValueBought = 0;			//	Total value spent buying (credits)

			int iCountInstalled = 0;				//	Number of items currently installed on player ship
			DWORD dwFirstInstalled = INVALID_TIME;	//	First time item was installed on player ship
			DWORD dwLastInstalled = INVALID_TIME;	//	Last time item was installed on player ship
			DWORD dwLastUninstalled = INVALID_TIME;	//	Last time item was uninstalled from player ship
			DWORD dwTotalInstalledTime = 0;			//	Total time installed

			int iCountMined = 0;					//	Number of items mined by player
			int iCountFired = 0;					//	Number of times item (weapon) has been fired by player
			int iHPDamaged = 0;						//	HP absorbed by this item type when installed on player
			};

		struct SKeyEventStats
			{
			EEventTypes iType = eventNone;			//	Type of event
			DWORD dwTime = 0;						//	When the event happened
			DWORD dwObjUNID = 0;					//	UNID of object
			CString sObjName;						//	For unique objects (e.g., CSCs)
			DWORD dwObjNameFlags = 0;				//	Flags for the name
			DWORD dwCauseUNID = 0;					//	The UNID of the sovereign who caused the event
			};

		struct SKeyEventStatsResult
			{
			CString sNodeID;
			SKeyEventStats *pStats = NULL;
			bool bMarked = false;
			};

		struct SPlayerShipStats
			{
			DWORD dwFirstEntered = INVALID_TIME;	//	First time we started this ship class
			DWORD dwLastEntered = INVALID_TIME;		//	Last time we started using this ship class
			DWORD dwLastLeft = INVALID_TIME;		//	Last time we stopped using ship class
			DWORD dwTotalTime = 0;					//	Total time using ship
			};

		struct SPowerStats
			{
			DWORD dwInvoked = 0;					//	Number of times player invoked a power
			};

		struct SShipClassStats
			{
			int iEnemyDestroyed = 0;				//	Number of enemy ships destroyed
			int iFriendDestroyed = 0;				//	Number of friendly ships destroyed
			};

		struct SStationTypeStats
			{
			int iDestroyed = 0;						//	Number of stations destroyed
			};

		struct SSystemStats
			{
			DWORD dwFirstEntered = INVALID_TIME;	//	First time this system was entered (0xffffffff = never)
			DWORD dwLastEntered = INVALID_TIME;		//	Last time this system was entered (0xffffffff = never)
			DWORD dwLastLeft = INVALID_TIME;		//	Last time this system was left (0xffffffff = never)
			DWORD dwTotalTime = 0;					//	Total time in system (all visits)

			int iAsteroidsMined = 0;				//	Count of asteroids explored for resources
			};

		bool AddMatchingKeyEvents (const CString &sNodeID, const CDesignTypeCriteria &Crit, TArray<SKeyEventStats> *pEventList, TArray<SKeyEventStatsResult> *retList) const;
		bool FindItemStats (DWORD dwUNID, SItemTypeStats **retpStats) const;
		CString GenerateKeyEventStat (TArray<SKeyEventStatsResult> &List) const;
		SItemTypeStats *GetItemStats (DWORD dwUNID);
		bool GetMatchingKeyEvents (const CString &sNodeID, const CDesignTypeCriteria &Crit, TArray<SKeyEventStatsResult> *retList) const;
		SShipClassStats *GetShipStats (DWORD dwUNID);
		SStationTypeStats *GetStationStats (DWORD dwUNID);
		const SSystemStats *GetSystemStats (const CString &sNodeID) const;
		SSystemStats *SetSystemStats (const CString &sNodeID);

		static void WriteTimeValue (CMemoryWriteStream &Output, DWORD dwTime);

		CUniverse &m_Universe;
		int m_iScore = 0;						//	Total score for player
		int m_iResurrectCount = 0;				//	Number of times player has resurrected a game
		CTimeSpan m_PlayTime;					//	Total time spent playing the game
		CTimeSpan m_GameTime;					//	Total elapsed time in the game
        Metric m_rFuelConsumed = 0.0;			//  Total fuel consumed (fuel units)

		TMap<DWORD, SItemTypeStats> m_ItemStats;
		TSortMap<CString, SPlayerShipStats> m_PlayerShipStats;
		TSortMap<DWORD, SPowerStats> m_PowerStats;
		TMap<DWORD, SShipClassStats> m_ShipStats;
		TMap<DWORD, SStationTypeStats> m_StationStats;
		TMap<CString, SSystemStats> m_SystemStats;
		TMap<CString, TArray<SKeyEventStats>> m_KeyEventStats;

		int m_iExtraSystemsVisited = 0;			//	For backwards compatibility
		int m_iExtraEnemyShipsDestroyed = 0;	//	For backwards compatibility
	};

class IPlayerController
	{
	public:
		enum EUIMode
			{
			uimodeUnknown,

			uimodeSpace,						//	In normal gameplay mode
			uimodeDockScreen,					//	A dockscreen is open (but time still passes)
			uimodeInStargate,					//	Passing through a stargate
			};

		IPlayerController (void);
		virtual ~IPlayerController (void) { }

		void OnPowerInvoked (const CPower &Power);
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void Update (SUpdateCtx &UpdateCtx) { OnUpdate(UpdateCtx); }
		void WriteToStream (IWriteStream *pStream);

		//	IPlayerController interface

		virtual ICCItem *CreateGlobalRef (CCodeChain &CC) { return CC.CreateInteger((int)this); }
        virtual CPlayerGameStats *GetGameStats (void) const { return NULL; }
		virtual GenomeTypes GetGenome (void) const { return genomeUnknown; }
		virtual CString GetName (void) const { return NULL_STR; }
		virtual bool GetPropertyInteger (const CString &sProperty, int *retiValue) { return false; }
		virtual bool GetPropertyItemList (const CString &sProperty, CItemList *retItemList) { return false; }
		virtual bool GetPropertyString (const CString &sProperty, CString *retsValue) { return false; }
		virtual CSovereign *GetSovereign (void) const;
		virtual EUIMode GetUIMode (void) const { return uimodeUnknown; }
		virtual void OnMessageFromObj (const CSpaceObject *pSender, const CString &sMessage) { }
		virtual bool SetPropertyInteger (const CString &sProperty, int iValue) { return false; }
		virtual bool SetPropertyItemList (const CString &sProperty, const CItemList &ItemList) { return false; }
		virtual bool SetPropertyString (const CString &sProperty, const CString &sValue) { return false; }

	protected:

		//	IPlayerController interface

		virtual void OnReadFromStream (SUniverseLoadCtx &Ctx) { }
		virtual void OnUpdate (SUpdateCtx &UpdateCtx) { }
		virtual void OnWriteToStream (IWriteStream *pStream) { }

		mutable CSovereign *m_pSovereign;			//	Cached sovereign
	};


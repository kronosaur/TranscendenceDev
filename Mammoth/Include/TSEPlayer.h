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

		CPlayerGameStats (void);

		int CalcEndGameScore (void) const;
        ICCItem *FindProperty (const CString &sProperty) const;
		void GenerateGameStats (CGameStats &Stats, CSpaceObject *pPlayer, bool bGameOver) const;
		int GetBestEnemyShipsDestroyed (DWORD *retdwUNID = NULL) const;
        CTimeSpan GetGameTime (void) const;
		CString GetItemStat (const CString &sStat, ICCItem *pItemCriteria) const;
		CString GetKeyEventStat (const CString &sStat, const CString &sNodeID, const CDesignTypeCriteria &Crit) const;
        CTimeSpan GetPlayTime (void) const;
		CString GetStat (const CString &sStat) const;
		DWORD GetSystemEnteredTime (const CString &sNodeID);
        DWORD GetSystemLastVisitedTime (const CString &sNodeID);
		int IncItemStat (const CString &sStat, DWORD dwUNID, int iInc);
		inline int IncScore (int iScore) { m_iScore = Max(0, m_iScore + iScore); return m_iScore; }
		int IncStat (const CString &sStat, int iInc = 1);
        inline void OnFuelConsumed (CSpaceObject *pPlayer, Metric rFuel) { m_rFuelConsumed += rFuel; }
		void OnGameEnd (CSpaceObject *pPlayer);
		void OnItemBought (const CItem &Item, CurrencyValue iTotalPrice);
		void OnItemDamaged (const CItem &Item, int iHP);
		void OnItemFired (const CItem &Item);
		void OnItemInstalled (const CItem &Item);
		void OnItemSold (const CItem &Item, CurrencyValue iTotalPrice);
		void OnItemUninstalled (const CItem &Item);
		void OnKeyEvent (EEventTypes iType, CSpaceObject *pObj, DWORD dwCauseUNID);
		void OnObjDestroyedByPlayer (const SDestroyCtx &Ctx, CSpaceObject *pPlayer);
		void OnSystemEntered (CSystem *pSystem, int *retiLastVisit = NULL);
		void OnSystemLeft (CSystem *pSystem);
		void ReadFromStream (SLoadCtx &Ctx);
		void SetStat (const CString &sStat, const CString &sValue);
		void WriteToStream (IWriteStream *pStream);

	private:
		struct SItemTypeStats
			{
			int iCountSold;						//	Number of items sold
			CurrencyValue iValueSold;			//	Total value received for selling (credits)

			int iCountBought;					//	Number of items bought
			CurrencyValue iValueBought;			//	Total value spent buying (credits)

			int iCountInstalled;				//	Number of items currently installed on player ship
			DWORD dwFirstInstalled;				//	First time item was installed on player ship
			DWORD dwLastInstalled;				//	Last time item was installed on player ship
			DWORD dwLastUninstalled;			//	Last time item was uninstalled from player ship
			DWORD dwTotalInstalledTime;			//	Total time installed

			int iCountFired;					//	Number of times item (weapon) has been fired by player
			int iHPDamaged;						//	HP absorbed by this item type when installed on player
			};

		struct SKeyEventStats
			{
			EEventTypes iType;					//	Type of event
			DWORD dwTime;						//	When the event happened
			DWORD dwObjUNID;					//	UNID of object
			CString sObjName;					//	For unique objects (e.g., CSCs)
			DWORD dwObjNameFlags;				//	Flags for the name
			DWORD dwCauseUNID;					//	The UNID of the sovereign who caused the event
			};

		struct SKeyEventStatsResult
			{
			CString sNodeID;
			SKeyEventStats *pStats;
			bool bMarked;
			};

		struct SShipClassStats
			{
			int iEnemyDestroyed;				//	Number of enemy ships destroyed
			int iFriendDestroyed;				//	Number of friendly ships destroyed
			};

		struct SStationTypeStats
			{
			int iDestroyed;						//	Number of stations destroyed
			};

		struct SSystemStats
			{
			DWORD dwFirstEntered;				//	First time this system was entered (0xffffffff = never)
			DWORD dwLastEntered;				//	Last time this system was entered (0xffffffff = never)
			DWORD dwLastLeft;					//	Last time this system was left (0xffffffff = never)
			DWORD dwTotalTime;					//	Total time in system (all visits)
			};

		bool AddMatchingKeyEvents (const CString &sNodeID, const CDesignTypeCriteria &Crit, TArray<SKeyEventStats> *pEventList, TArray<SKeyEventStatsResult> *retList) const;
		bool FindItemStats (DWORD dwUNID, SItemTypeStats **retpStats) const;
		CString GenerateKeyEventStat (TArray<SKeyEventStatsResult> &List) const;
		SItemTypeStats *GetItemStats (DWORD dwUNID);
		bool GetMatchingKeyEvents (const CString &sNodeID, const CDesignTypeCriteria &Crit, TArray<SKeyEventStatsResult> *retList) const;
		SShipClassStats *GetShipStats (DWORD dwUNID);
		SStationTypeStats *GetStationStats (DWORD dwUNID);
		SSystemStats *GetSystemStats (const CString &sNodeID);

		int m_iScore;							//	Total score for player
		int m_iResurrectCount;					//	Number of times player has resurrected a game
		CTimeSpan m_PlayTime;					//	Total time spent playing the game
		CTimeSpan m_GameTime;					//	Total elapsed time in the game
        Metric m_rFuelConsumed;                 //  Total fuel consumed (fuel units)

		TMap<DWORD, SItemTypeStats> m_ItemStats;
		TMap<DWORD, SShipClassStats> m_ShipStats;
		TMap<DWORD, SStationTypeStats> m_StationStats;
		TMap<CString, SSystemStats> m_SystemStats;
		TMap<CString, TArray<SKeyEventStats>> m_KeyEventStats;

		int m_iExtraSystemsVisited;				//	For backwards compatibility
		int m_iExtraEnemyShipsDestroyed;		//	For backwards compatibility
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
		virtual void OnMessageFromObj (CSpaceObject *pSender, const CString &sMessage) { }
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


//	CPlayerGameStats.cpp
//
//	CPlayerGameStats class
//	Copyright (c) 2010 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

#define ACHIEVEMENT_CORE_BURIED_KILL			CONSTLIT("core.buriedKill")
#define ACHIEVEMENT_CORE_COLLATERAL_DAMAGE		CONSTLIT("core.collateralDamage")
#define ACHIEVEMENT_CORE_PARALYZE_KILL			CONSTLIT("core.paralyzeKill")
#define ACHIEVEMENT_CORE_RADIATION_KILL			CONSTLIT("core.radiationKill")

#define ATTRIB_ORE								CONSTLIT("ore")

#define ASTEROIDS_MINED_STAT					CONSTLIT("asteroidsMined")
#define BEST_ENEMY_SHIPS_DESTROYED_STAT			CONSTLIT("bestEnemyShipsDestroyed")
#define ENEMY_OBJS_DESTROYED_STAT				CONSTLIT("enemyObjsDestroyed")
#define ENEMY_SHIPS_DESTROYED_STAT				CONSTLIT("enemyShipsDestroyed")
#define ENEMY_STATIONS_DESTROYED_STAT			CONSTLIT("enemyStationsDestroyed")
#define FRIENDLY_OBJS_DESTROYED_STAT			CONSTLIT("friendlyObjsDestroyed")
#define FRIENDLY_SHIPS_DESTROYED_STAT			CONSTLIT("friendlyShipsDestroyed")
#define FRIENDLY_STATIONS_DESTROYED_STAT		CONSTLIT("friendlyStationsDestroyed")
#define ITEMS_BOUGHT_COUNT_STAT					CONSTLIT("itemsBoughtCount")
#define ITEMS_BOUGHT_VALUE_STAT					CONSTLIT("itemsBoughtValue")
#define ITEMS_DAMAGED_HP_STAT					CONSTLIT("itemsDamagedHP")
#define ITEMS_FIRED_COUNT_STAT					CONSTLIT("itemsFiredCount")
#define ITEMS_MINED_COUNT_STAT					CONSTLIT("itemsMinedCount")
#define ITEMS_MINED_VALUE_STAT					CONSTLIT("itemsMinedValue")
#define ITEMS_SOLD_COUNT_STAT					CONSTLIT("itemsSoldCount")
#define ITEMS_SOLD_VALUE_STAT					CONSTLIT("itemsSoldValue")
#define MISSION_COMPLETED_STAT					CONSTLIT("missionCompleted")
#define MISSION_FAILURE_STAT					CONSTLIT("missionFailure")
#define MISSION_SUCCESS_STAT					CONSTLIT("missionSuccess")
#define OBJS_DESTROYED_STAT						CONSTLIT("objsDestroyed")
#define RESURRECT_COUNT_STAT					CONSTLIT("resurrectCount")
#define SCORE_STAT								CONSTLIT("score")
#define SYSTEM_DATA_STAT						CONSTLIT("systemData")
#define SYSTEMS_VISITED_STAT					CONSTLIT("systemsVisited")
#define TONS_OF_ORE_MINED_STAT					CONSTLIT("tonsOfOreMined")

#define NIL_VALUE								CONSTLIT("Nil")

#define PROPERTY_FUEL_CONSUMED                  CONSTLIT("fuelConsumed")
#define PROPERTY_SYSTEMS_VISITED                CONSTLIT("systemsVisited")
#define PROPERTY_SYSTEMS_VISITED_COUNT          CONSTLIT("systemsVisitedCount")

#define STR_DESTROYED							CONSTLIT("destroyed")
#define STR_ENEMY_DESTROYED						CONSTLIT("enemyDestroyed")
#define STR_FRIEND_DESTROYED					CONSTLIT("friendDestroyed")
#define STR_MISSION_FAILURE						CONSTLIT("missionFailure")
#define STR_MISSION_SUCCESS						CONSTLIT("missionSuccess")
#define STR_SAVED								CONSTLIT("saved")

std::initializer_list<CPlayerGameStats::SOreMinedAchievementDesc> CPlayerGameStats::m_OreMinedAchievements = {
	{	"",										1			},
	{	"core.mineOre1",						100			},
	{	"core.mineOre100",						1000		},
	{	"core.mineOre1000",						10000		},
	{	"core.mineOre10000",					100000		},
	{	"core.mineOre100000",					100000000	},
	};

std::initializer_list<CPlayerGameStats::SProfitAchievementDesc> CPlayerGameStats::m_ProfitAchievements = {
	{	"",										10'000			},
	{	"core.trade10K",						50'000			},
	{	"core.trade50K",						150'000			},
	{	"core.trade150K",						500'000			},
	{	"core.trade500K",						1'000'000		},
	{	"core.trade1M",							5'000'000		},
	{	"core.trade5M",							1'000'000'000	},
	};

class CStatCounterArray
	{
	public:
		void DeleteAll (void) { m_Array.DeleteAll(); }
		void GenerateGameStats (CGameStats &Stats);
		void Insert (const CString &sStat, int iCount, const CString &sSection, const CString &sSort);

	private:
		struct SEntry
			{
			int iCount;
			CString sSort;
			};

		TMap<CString, TMap<CString, SEntry>> m_Array;
	};

CPlayerGameStats::CPlayerGameStats (CUniverse &Universe) : 
		m_Universe(Universe)

//	CPlayerGameStats constructor

	{
	}

bool CPlayerGameStats::AddMatchingKeyEvents (const CString &sNodeID, const CDesignTypeCriteria &Crit, TArray<SKeyEventStats> *pEventList, TArray<SKeyEventStatsResult> *retList) const

//	AddMatchingKeyEvents
//
//	Adds all of the matching events from pEventList to the result

	{
	int i;

	for (i = 0; i < pEventList->GetCount(); i++)
		{
		SKeyEventStats *pStats = &pEventList->GetAt(i);

		CDesignType *pType = m_Universe.FindDesignType(pStats->dwObjUNID);
		if (pType == NULL)
			continue;

		if (pType->MatchesCriteria(Crit))
			{
			SKeyEventStatsResult *pResult = retList->Insert();
			pResult->sNodeID = sNodeID;
			pResult->pStats = pStats;
			pResult->bMarked = false;
			}
		}

	return true;
	}

int CPlayerGameStats::CalcEndGameScore (void) const

//	CalcEndGameScore
//
//	Calculates the score if it were end game (by adjusting for
//	resurrection count

	{
	return m_iScore / (1 + min(9, m_iResurrectCount));
	}

const CShipClass *CPlayerGameStats::FindClassByName (SLoadCtx &Ctx, const CString &sClassName)

//	FindClassByName
//
//	Looks for the ship class by name.

	{
	for (int i = 0; i < Ctx.GetUniverse().GetShipClassCount(); i++)
		{
		const CShipClass *pClass = Ctx.GetUniverse().GetShipClass(i);
		if (strEquals(sClassName, pClass->GetNounPhrase()))
			return pClass;
		}

	return NULL;
	}

bool CPlayerGameStats::FindItemStats (DWORD dwUNID, SItemTypeStats **retpStats) const

//	FindItemStats
//
//	Returns a pointer to stats

	{
	SItemTypeStats *pStats = m_ItemStats.Find(dwUNID);
	if (pStats == NULL)
		return false;

	if (retpStats)
		*retpStats = pStats;

	return true;
	}

ICCItem *CPlayerGameStats::FindProperty (const CString &sProperty) const

//  FindProperty
//
//  Returns a the given property (or NULL).

	{
	if (strEquals(sProperty, BEST_ENEMY_SHIPS_DESTROYED_STAT))
		{
		DWORD dwUNID;
		int iCount = GetBestEnemyShipsDestroyed(&dwUNID);
		if (iCount == 0)
			return CCodeChain::CreateNil();

		ICCItem *pResult = CCodeChain::CreateSymbolTable();
		pResult->SetIntegerAt(CONSTLIT("unid"), dwUNID);
		pResult->SetIntegerAt(CONSTLIT("count"), iCount);
		return pResult;
		}
	else if (strEquals(sProperty, ENEMY_SHIPS_DESTROYED_STAT))
		{
		CMapIterator i;
		int iCount = 0;
		m_ShipStats.Reset(i);
		while (m_ShipStats.HasMore(i))
			{
			SShipClassStats *pStats;
			DWORD dwUNID = m_ShipStats.GetNext(i, &pStats);

			iCount += pStats->iEnemyDestroyed;
			}

		return CCodeChain::CreateInteger(iCount + m_iExtraEnemyShipsDestroyed);
		}
	else if (strEquals(sProperty, ENEMY_STATIONS_DESTROYED_STAT))
		{
		CSovereign *pPlayerSovereign = m_Universe.FindSovereign(g_PlayerSovereignUNID);
		if (pPlayerSovereign == NULL)
			return CCodeChain::CreateInteger(0);

		CMapIterator i;
		int iCount = 0;
		m_StationStats.Reset(i);
		while (m_StationStats.HasMore(i))
			{
			SStationTypeStats *pStats;
			DWORD dwUNID = m_StationStats.GetNext(i, &pStats);
			CStationType *pType = m_Universe.FindStationType(dwUNID);
			if (pType == NULL)
				continue;

			if (pType->GetSovereign()->IsEnemy(pPlayerSovereign))
				iCount += pStats->iDestroyed;
			}

		return CCodeChain::CreateInteger(iCount);
		}
	else if (strEquals(sProperty, PROPERTY_FUEL_CONSUMED))
		return CCodeChain::CreateDouble(m_rFuelConsumed);

	else if (strEquals(sProperty, FRIENDLY_SHIPS_DESTROYED_STAT))
		{
		CMapIterator i;
		int iCount = 0;
		m_ShipStats.Reset(i);
		while (m_ShipStats.HasMore(i))
			{
			SShipClassStats *pStats;
			DWORD dwUNID = m_ShipStats.GetNext(i, &pStats);

			iCount += pStats->iFriendDestroyed;
			}

		return CCodeChain::CreateInteger(iCount);
		}
	else if (strEquals(sProperty, FRIENDLY_STATIONS_DESTROYED_STAT))
		{
		CSovereign *pPlayerSovereign = m_Universe.FindSovereign(g_PlayerSovereignUNID);
		if (pPlayerSovereign == NULL)
			return CCodeChain::CreateInteger(0);

		CMapIterator i;
		int iCount = 0;
		m_StationStats.Reset(i);
		while (m_StationStats.HasMore(i))
			{
			SStationTypeStats *pStats;
			DWORD dwUNID = m_StationStats.GetNext(i, &pStats);
			CStationType *pType = m_Universe.FindStationType(dwUNID);
			if (pType == NULL)
				continue;

			if (!pType->GetSovereign()->IsEnemy(pPlayerSovereign))
				iCount += pStats->iDestroyed;
			}

		return CCodeChain::CreateInteger(iCount);
		}
	else if (strEquals(sProperty, RESURRECT_COUNT_STAT))
		return CCodeChain::CreateInteger(m_iResurrectCount);

	else if (strEquals(sProperty, SCORE_STAT))
		return CCodeChain::CreateInteger(m_iScore);

	else if (strEquals(sProperty, PROPERTY_SYSTEMS_VISITED))
		{
		CMemoryWriteStream Output;
		if (Output.Create() != NOERROR)
			return CCodeChain::CreateNil();

		ICCItem *pResult = CCodeChain::CreateLinkedList();

		CMapIterator i;
		m_SystemStats.Reset(i);
		while (m_SystemStats.HasMore(i))
			{
			SSystemStats *pStats;
			const CString &sNodeID = m_SystemStats.GetNext(i, &pStats);

			ICCItem *pSysData = CCodeChain::CreateSymbolTable();
			pSysData->SetStringAt(CONSTLIT("nodeID"), sNodeID);

			if (pStats->dwFirstEntered != INVALID_TIME)
				pSysData->SetIntegerAt(CONSTLIT("firstEnteredOn"), pStats->dwFirstEntered);

			if (pStats->dwLastEntered != INVALID_TIME)
				pSysData->SetIntegerAt(CONSTLIT("lastEnteredOn"), pStats->dwLastEntered);

			if (pStats->dwLastLeft != INVALID_TIME)
				pSysData->SetIntegerAt(CONSTLIT("lastLeftOn"), pStats->dwLastLeft);

			if (pStats->dwTotalTime != INVALID_TIME)
				pSysData->SetIntegerAt(CONSTLIT("totalTimeSpent"), pStats->dwTotalTime);

			if (pStats->iAsteroidsMined > 0)
				pSysData->SetIntegerAt(CONSTLIT("asteroidsMined"), pStats->iAsteroidsMined);

			pResult->Append(pSysData);
			pSysData->Discard();
			}

		return pResult;
		}
	else if (strEquals(sProperty, PROPERTY_SYSTEMS_VISITED_COUNT))
		{
		CMapIterator i;
		int iCount = 0;
		m_SystemStats.Reset(i);
		while (m_SystemStats.HasMore(i))
			{
			SSystemStats *pStats;
			const CString &sNodeID = m_SystemStats.GetNext(i, &pStats);

			if (pStats->dwLastEntered != INVALID_TIME)
				iCount++;
			}

		return CCodeChain::CreateInteger(iCount + m_iExtraSystemsVisited);
		}
	else
		return NULL;
	}

bool CPlayerGameStats::FireMineOreAchievement (int iLastValue, int iCurrentValue)

//	FireMineOreAchievement
//
//	Fires an achievement if the new value crosses a threshold.

	{
	if (iLastValue >= iCurrentValue)
		return false;

	int iLastThreshold = -1;

	for (const auto &Entry : m_OreMinedAchievements)
		{
		if (iLastThreshold == -1 && iLastValue < Entry.iOreMinedThreshold)
			iLastThreshold = Entry.iOreMinedThreshold;

		if (iCurrentValue < Entry.iOreMinedThreshold)
			{
			//	If we're at the same level still, then nothing to do.

			if (iLastThreshold == Entry.iOreMinedThreshold)
				return false;

			//	Otherwise, fire an achievement.

			m_Universe.SetAchievement(Entry.sAchievementID);
			return true;
			}
		}

	return false;
	}

bool CPlayerGameStats::FireProfitAchievement (CurrencyValue LastValue, CurrencyValue CurrentValue)

//	FireProfitAchievement
//
//	Fires an achievement if the new value crosses a threshold.

	{
	if (CurrentValue <= 0 || LastValue >= CurrentValue)
		return false;

	CurrencyValue LastThreshold = -1;

	for (const auto &Entry : m_ProfitAchievements)
		{
		if (LastThreshold == -1 && LastValue < Entry.ProfitThreshold)
			LastThreshold = Entry.ProfitThreshold;

		if (CurrentValue < Entry.ProfitThreshold)
			{
			//	If we're at the same level still, then nothing to do.

			if (LastThreshold == Entry.ProfitThreshold)
				return false;

			//	Otherwise, fire an achievement.

			m_Universe.SetAchievement(Entry.sAchievementID);
			return true;
			}
		}

	return false;
	}

void CPlayerGameStats::GenerateGameStats (CGameStats &Stats, CSpaceObject *pPlayerShip, bool bGameOver) const

//	GenerateGameStats
//
//	Generates a stats for everything we track

	{
	int j;

	CShip *pShip = (pPlayerShip ? pPlayerShip->AsShip() : NULL);
	if (pShip == NULL)
		return;

	CSovereign *pPlayerSovereign = m_Universe.FindSovereign(g_PlayerSovereignUNID);
	if (pPlayerSovereign == NULL)
		return;

	//	Base stats

	Stats.Insert(CONSTLIT("Gender"), strCapitalize(GetGenomeName(m_Universe.GetPlayerGenome())));
	Stats.Insert(CONSTLIT("Score"), strFormatInteger(CalcEndGameScore(), -1, FORMAT_THOUSAND_SEPARATOR | FORMAT_UNSIGNED));
	Stats.Insert(CONSTLIT("Ship class"), pShip->GetNounPhrase(0));

	CTimeSpan Time = GetPlayTime();
	if (!Time.IsBlank())
		Stats.Insert(CONSTLIT("Time played"), Time.Format(NULL_STR));

#ifdef REAL_TIME
	Time = GetGameTime();
	if (!Time.IsBlank())
		Stats.Insert(CONSTLIT("Time elapsed in game"), Time.Format(NULL_STR));
#endif

	//	Stats for player ships used (sorted by when we first entered the ship).

	TSortMap<CString, SPlayerShipStats> PlayerShipStats;
	for (int i = 0; i < m_PlayerShipStats.GetCount(); i++)
		{
		bool bNew;
		SPlayerShipStats *pStats = PlayerShipStats.SetAt(m_PlayerShipStats[i].sClassName,  &bNew);
		if (bNew)
			*pStats = m_PlayerShipStats[i];
		else
			{
			if (m_PlayerShipStats[i].dwFirstEntered < pStats->dwFirstEntered)
				pStats->dwFirstEntered = m_PlayerShipStats[i].dwFirstEntered;

			if (m_PlayerShipStats[i].iMaxSpeed > pStats->iMaxSpeed)
				pStats->iMaxSpeed = m_PlayerShipStats[i].iMaxSpeed;
			}
		}

	for (int i = 0; i < PlayerShipStats.GetCount(); i++)
		{
		CString sSort = strPatternSubst(CONSTLIT("%012d"), PlayerShipStats[i].dwFirstEntered);

		Stats.Insert(PlayerShipStats[i].sClassName, NULL_STR, CONSTLIT("Ships Used"), sSort);
		}

	//	Some combat stats

	CString sDestroyed = GetStatString(ENEMY_SHIPS_DESTROYED_STAT);
	if (!sDestroyed.IsBlank())
		Stats.Insert(CONSTLIT("Enemy ships destroyed"), sDestroyed, CONSTLIT("combat"));

	sDestroyed = GetStatString(FRIENDLY_SHIPS_DESTROYED_STAT);
	if (!sDestroyed.IsBlank())
		Stats.Insert(CONSTLIT("Friendly ships destroyed"), sDestroyed, CONSTLIT("combat"));

	sDestroyed = GetStatString(ENEMY_STATIONS_DESTROYED_STAT);
	if (!sDestroyed.IsBlank())
		Stats.Insert(CONSTLIT("Enemy stations destroyed"), sDestroyed, CONSTLIT("combat"));

	sDestroyed = GetStatString(FRIENDLY_STATIONS_DESTROYED_STAT);
	if (!sDestroyed.IsBlank())
		Stats.Insert(CONSTLIT("Friendly stations destroyed"), sDestroyed, CONSTLIT("combat"));

	//	Add stat for every station destroyed

	CStatCounterArray CounterArray;

	CMapIterator i;
	m_StationStats.Reset(i);
	while (m_StationStats.HasMore(i))
		{
		SStationTypeStats *pStats;
		DWORD dwUNID = m_StationStats.GetNext(i, &pStats);
		CStationType *pType = m_Universe.FindStationType(dwUNID);
		if (pType == NULL)
			continue;

		CString sName = pType->GetNounPhrase(0);
		CString sSort = strPatternSubst(CONSTLIT("%03d%s"), 100 - pType->GetLevel(), sName);

		if (pType->GetSovereign()->IsEnemy(pPlayerSovereign))
			CounterArray.Insert(sName, pStats->iDestroyed, CONSTLIT("Enemy stations destroyed"), sSort);
		else
			CounterArray.Insert(sName, pStats->iDestroyed, CONSTLIT("Friendly stations destroyed"), sSort);
		}

	CounterArray.GenerateGameStats(Stats);
		
	//	Add stat for every ship class destroyed

	CounterArray.DeleteAll();
	m_ShipStats.Reset(i);
	while (m_ShipStats.HasMore(i))
		{
		SShipClassStats *pStats;
		DWORD dwUNID = m_ShipStats.GetNext(i, &pStats);
		CShipClass *pClass = m_Universe.FindShipClass(dwUNID);
		if (pClass == NULL)
			continue;

		CString sName = pClass->GetNounPhrase(nounGeneric);
		CString sSort = strPatternSubst(CONSTLIT("%09d%s"), 100000000 - pClass->GetScore(), sName);

		if (pStats->iEnemyDestroyed > 0)
			CounterArray.Insert(sName, pStats->iEnemyDestroyed, CONSTLIT("Enemy ships destroyed"), sSort);

		if (pStats->iFriendDestroyed > 0)
			CounterArray.Insert(sName, pStats->iFriendDestroyed, CONSTLIT("Friendly ships destroyed"), sSort);
		}

	CounterArray.GenerateGameStats(Stats);

	//	Add stat for every item

	m_ItemStats.Reset(i);
	while (m_ItemStats.HasMore(i))
		{
		SItemTypeStats *pStats;
		DWORD dwUNID = m_ItemStats.GetNext(i, &pStats);
		CItemType *pItemType = m_Universe.FindItemType(dwUNID);
		if (pItemType == NULL)
			continue;

		CString sName = pItemType->GetNounPhrase(nounShort);
		CString sSort = strPatternSubst(CONSTLIT("%03d%s"), 100 - pItemType->GetLevel(), sName);

		//	Installed items

		if (pStats->dwFirstInstalled != INVALID_TIME)
			Stats.Insert(sName, NULL_STR, CONSTLIT("Items installed"), sSort);

		if (pStats->iCountFired > 0)
			Stats.Insert(sName, 
					strFormatInteger(pStats->iCountFired, -1, FORMAT_THOUSAND_SEPARATOR | FORMAT_UNSIGNED), 
					CONSTLIT("Weapons fired"), 
					sSort);

		if (pStats->iHPDamaged > 0)
			Stats.Insert(sName,
					strFormatInteger(pStats->iHPDamaged, -1, FORMAT_THOUSAND_SEPARATOR | FORMAT_UNSIGNED),
					CONSTLIT("Damage sustained"),
					sSort);

		//	Mined ore

		if (pStats->iCountMined > 0)
			{
			Stats.Insert(sName, 
					strFormatInteger(pStats->iCountMined, -1, FORMAT_THOUSAND_SEPARATOR | FORMAT_UNSIGNED), 
					CONSTLIT("Ore mined"), 
					sSort);
			}
		}

	//	Stats for powers used

	CounterArray.DeleteAll();
	for (int i = 0; i < m_PowerStats.GetCount(); i++)
		{
		const CPower *pPower = m_Universe.FindPower(m_PowerStats.GetKey(i));
		if (!pPower)
			continue;

		auto &Stats = m_PowerStats[i];

		if (Stats.dwInvoked > 0)
			{
			CString sSort = strPatternSubst(CONSTLIT("%03d-%s"), 99 - pPower->GetLevel(), pPower->GetName());
			CounterArray.Insert(pPower->GetName(), Stats.dwInvoked, CONSTLIT("Powers invoked"), sSort);
			}
		}

	CounterArray.GenerateGameStats(Stats);

	//	Stats for player equipment (but only if the game is done)

	if (bGameOver)
		{
		TSortMap<CString, CItem> InstalledItems;

		//	First we generate a sorted list of installed items
		//	(We do this in case there are multiple of the same device/armor so that
		//	we can coalesce them together into a single line).

		CItemListManipulator ItemList(pShip->GetItemList());
		ItemList.ResetCursor();
		while (ItemList.MoveCursorForward())
			{
			const CItem &Item(ItemList.GetItemAtCursor());

			if (Item.IsInstalled())
				{
				CString sEnhancement = Item.GetEnhancedDesc();
				CString sItemName = Item.GetNounPhrase(nounActual | nounCountOnly | nounShort);
				CString sLine = (sEnhancement.IsBlank() ? sItemName : strPatternSubst(CONSTLIT("%s [%s]"), sItemName, sEnhancement));

				bool bInserted;
				CItem *pEntry = InstalledItems.SetAt(sLine, &bInserted);
				if (bInserted)
					{
					*pEntry = Item;
					pEntry->SetCount(1);
					}
				else
					pEntry->SetCount(pEntry->GetCount() + 1);
				}
			}

		//	Now add all the installed items to the stats

		for (j = 0; j < InstalledItems.GetCount(); j++)
			{
			//	Redo the line now that we know the proper count

			CString sEnhancement = InstalledItems[j].GetEnhancedDesc();
			DWORD dwFlags = nounActual | nounCountOnly;
			if (!sEnhancement.IsBlank())
				dwFlags |= nounNoModifiers;

			CString sItemName = InstalledItems[j].GetNounPhrase(dwFlags);
			CString sLine = (sEnhancement.IsBlank() ? sItemName : strPatternSubst(CONSTLIT("%s [%s]"), sItemName, sEnhancement));

			//	Compute the sort order

			int iOrder;
			switch (InstalledItems[j].GetType()->GetCategory())
				{
				case itemcatWeapon:
					iOrder = 0;
					break;

				case itemcatLauncher:
					iOrder = 1;
					break;

				case itemcatShields:
					iOrder = 2;
					break;

				case itemcatArmor:
					iOrder = 3;
					break;

				case itemcatReactor:
					iOrder = 4;
					break;

				case itemcatDrive:
					iOrder = 5;
					break;

				default:
					iOrder = 6;
					break;
				}

			CString sSort = strPatternSubst(CONSTLIT("%d%03d%s"), iOrder, 100 - InstalledItems[j].GetType()->GetLevel(), sLine);
			Stats.Insert(sLine, NULL_STR, CONSTLIT("Final equipment"), sSort);
			}

		//	Add the remaining items

		ItemList.ResetCursor();
		while (ItemList.MoveCursorForward())
			{
			const CItem &Item(ItemList.GetItemAtCursor());

			if (!Item.IsInstalled())
				{
				CString sEnhancement = Item.GetEnhancedDesc();
				DWORD dwFlags = nounActual | nounCountOnly;
				if (!sEnhancement.IsBlank())
					dwFlags |= nounNoModifiers;

				CString sItemName = Item.GetNounPhrase(dwFlags);
				CString sLine = (sEnhancement.IsBlank() ? sItemName : strPatternSubst(CONSTLIT("%s [%s]"), sItemName, sEnhancement));
				CString sSort = strPatternSubst(CONSTLIT("%03d%s"), 100 - Item.GetType()->GetLevel(), sLine);

				Stats.Insert(sLine, NULL_STR, CONSTLIT("Final items"), sSort);
				}
			}
		}
	}

CString CPlayerGameStats::GenerateKeyEventStat (TArray<SKeyEventStatsResult> &List) const

//	GenerateKeyEventStat
//
//	Generates a stat from the list of events

	{
	int i;

	CMemoryWriteStream Output;
	if (Output.Create() != NOERROR)
		return NIL_VALUE;

	Output.Write("'(", 2);

	for (i = 0; i < List.GetCount(); i++)
		{
		SKeyEventStatsResult *pResult = &List[i];

		if (pResult->bMarked)
			{
			Output.Write("(", 1);

			//	Type

			switch (pResult->pStats->iType)
				{
				case eventEnemyDestroyedByPlayer:
					Output.Write(STR_ENEMY_DESTROYED.GetASCIIZPointer(), STR_ENEMY_DESTROYED.GetLength());
					break;

				case eventFriendDestroyedByPlayer:
					Output.Write(STR_FRIEND_DESTROYED.GetASCIIZPointer(), STR_FRIEND_DESTROYED.GetLength());
					break;

				case eventMajorDestroyed:
					Output.Write(STR_DESTROYED.GetASCIIZPointer(), STR_DESTROYED.GetLength());
					break;

				case eventMissionFailure:
					Output.Write(STR_MISSION_FAILURE.GetASCIIZPointer(), STR_MISSION_FAILURE.GetLength());
					break;

				case eventMissionSuccess:
					Output.Write(STR_MISSION_SUCCESS.GetASCIIZPointer(), STR_MISSION_SUCCESS.GetLength());
					break;

				case eventSavedByPlayer:
					Output.Write(STR_SAVED.GetASCIIZPointer(), STR_SAVED.GetLength());
					break;

				default:
					Output.Write("?", 1);
					break;
				}

			//	sNodeID time, UNID, cause, name, flags

			CString sValue = strPatternSubst(" %s 0x%x 0x%x 0x%x %s 0x%x) ", 
					CCString::Print(pResult->sNodeID),
					pResult->pStats->dwTime, 
					pResult->pStats->dwObjUNID, 
					pResult->pStats->dwCauseUNID,
					CCString::Print(pResult->pStats->sObjName),
					pResult->pStats->dwObjNameFlags);
			Output.Write(sValue.GetASCIIZPointer(), sValue.GetLength());
			}
		}

	Output.Write(")", 1);

	return CString(Output.GetPointer(), Output.GetLength());
	}

int CPlayerGameStats::GetBestEnemyShipsDestroyed (DWORD *retdwUNID) const

//	GetBestEnemyShipDestroyed
//
//	Returns the number of enemy ships destroyed of the most powerful ship class

	{
	CMapIterator i;
	int iBestScore = 0;
	DWORD dwBestUNID = 0;
	SShipClassStats *pBest = NULL;

	m_ShipStats.Reset(i);
	while (m_ShipStats.HasMore(i))
		{
		SShipClassStats *pStats;
		DWORD dwUNID = m_ShipStats.GetNext(i, &pStats);
		CShipClass *pClass = m_Universe.FindShipClass(dwUNID);
		if (pClass)
			{
			int iScore = pClass->GetScore();
			if (iScore > iBestScore
					&& pStats->iEnemyDestroyed > 0)
				{
				dwBestUNID = dwUNID;
				iBestScore = iScore;
				pBest = pStats;
				}
			}
		}

	if (pBest == NULL)
		return 0;

	if (retdwUNID)
		*retdwUNID = dwBestUNID;

	return pBest->iEnemyDestroyed;
	}

CPlayerGameStats::SPlayerShipStats *CPlayerGameStats::GetCurrentPlayerShipStats ()

//	GetCurrentPlayerShipStats
//
//	Returns the entry for the current player ship.

	{
	for (int i = 0; i < m_PlayerShipStats.GetCount(); i++)
		if (m_PlayerShipStats[i].dwLastLeft == INVALID_TIME)
			return &m_PlayerShipStats[i];

	return NULL;
	}

CTimeSpan CPlayerGameStats::GetGameTime (void) const

//  GetGameTime
//
//  Returns the amount of real time spent playing
	
	{
	return (!m_GameTime.IsBlank() ? m_GameTime : m_Universe.GetElapsedGameTime());
	}

CString CPlayerGameStats::GetItemStat (const CString &sStat, ICCItem *pItemCriteria) const

//	GetItemStat
//
//	Returns the given stat

	{
	int j;

	struct SEntry
		{
		CItemType *pType;
		SItemTypeStats *pStats;
		};

	//	Generate a list of all item stats that match criteria

	TArray<SEntry> List;

	//	If the criteria is an integer, then expect an UNID

	if (pItemCriteria->IsInteger())
		{
		DWORD dwUNID = (DWORD)pItemCriteria->GetIntegerValue();
		SItemTypeStats *pStats;

		if (FindItemStats(dwUNID, &pStats))
			{
			SEntry *pEntry = List.Insert();
			pEntry->pType = m_Universe.FindItemType(dwUNID);
			pEntry->pStats = pStats;
			}
		}

	//	Otherwise, we expect this to be a criteria

	else
		{
		CItemCriteria Crit(pItemCriteria->GetStringValue());

		CMapIterator i;
		m_ItemStats.Reset(i);
		while (m_ItemStats.HasMore(i))
			{
			SItemTypeStats *pStats;
			DWORD dwUNID = m_ItemStats.GetNext(i, &pStats);
			CItemType *pType = m_Universe.FindItemType(dwUNID);
			if (pType == NULL)
				continue;

			CItem theItem(pType, 1);
			if (theItem.MatchesCriteria(Crit))
				{
				SEntry *pEntry = List.Insert();
				pEntry->pType = pType;
				pEntry->pStats = pStats;
				}
			}
		}

	//	Handle each case separately

	if (strEquals(sStat, ITEMS_BOUGHT_COUNT_STAT))
		{
		int iTotalCount = 0;
		for (j = 0; j < List.GetCount(); j++)
			iTotalCount += List[j].pStats->iCountBought;
		return ::strFromInt(iTotalCount);
		}
	else if (strEquals(sStat, ITEMS_BOUGHT_VALUE_STAT))
		{
		CurrencyValue iTotal = 0;
		for (j = 0; j < List.GetCount(); j++)
			iTotal += List[j].pStats->iValueBought;
		return ::strFromInt((int)iTotal);
		}
	else if (strEquals(sStat, ITEMS_DAMAGED_HP_STAT))
		{
		int iTotalCount = 0;
		for (j = 0; j < List.GetCount(); j++)
			iTotalCount += List[j].pStats->iHPDamaged;
		return ::strFromInt(iTotalCount);
		}
	else if (strEquals(sStat, ITEMS_FIRED_COUNT_STAT))
		{
		int iTotalCount = 0;
		for (j = 0; j < List.GetCount(); j++)
			iTotalCount += List[j].pStats->iCountFired;
		return ::strFromInt(iTotalCount);
		}
	else if (strEquals(sStat, ITEMS_MINED_COUNT_STAT))
		{
		int iTotalCount = 0;
		for (j = 0; j < List.GetCount(); j++)
			iTotalCount += List[j].pStats->iCountMined;
		return ::strFromInt(iTotalCount);
		}
	else if (strEquals(sStat, ITEMS_MINED_VALUE_STAT))
		{
		CurrencyValue iTotal = 0;
		for (j = 0; j < List.GetCount(); j++)
			{
			CItem Ore(List[j].pType, 1);
			iTotal += (CurrencyValue)Ore.GetRawPrice(true) * List[j].pStats->iCountMined;
			}
		return ::strFromInt((int)iTotal);
		}
	else if (strEquals(sStat, ITEMS_SOLD_COUNT_STAT))
		{
		int iTotalCount = 0;
		for (j = 0; j < List.GetCount(); j++)
			iTotalCount += List[j].pStats->iCountSold;
		return ::strFromInt(iTotalCount);
		}
	else if (strEquals(sStat, ITEMS_SOLD_VALUE_STAT))
		{
		CurrencyValue iTotal = 0;
		for (j = 0; j < List.GetCount(); j++)
			iTotal += List[j].pStats->iValueSold;
		return ::strFromInt((int)iTotal);
		}
	else
		return NULL_STR;
	}

CPlayerGameStats::SItemTypeStats *CPlayerGameStats::GetItemStats (DWORD dwUNID)

//	GetItemStats
//
//	Get the stats for item type

	{
	SItemTypeStats *pStats = m_ItemStats.Find(dwUNID);
	if (pStats == NULL)
		pStats = m_ItemStats.Insert(dwUNID);

	return pStats;
	}

CString CPlayerGameStats::GetKeyEventStat (const CString &sStat, const CString &sNodeID, const CDesignTypeCriteria &Crit) const

//	GetKeyEventStat
//
//	Returns the given key event stat

	{
	int i;

	//	Get the list of stats

	TArray<SKeyEventStatsResult> List;
	if (!GetMatchingKeyEvents(sNodeID, Crit, &List))
		return NIL_VALUE;

	if (strEquals(sStat, OBJS_DESTROYED_STAT))
		{
		//	Mark the events that we're interested in

		for (i = 0; i < List.GetCount(); i++)
			List[i].bMarked = ((List[i].pStats->iType == eventEnemyDestroyedByPlayer) 
					|| (List[i].pStats->iType == eventFriendDestroyedByPlayer)
					|| (List[i].pStats->iType == eventMajorDestroyed));

		}

	else if (strEquals(sStat, MISSION_COMPLETED_STAT))
		{
		//	Mark the events that we're interested in

		for (i = 0; i < List.GetCount(); i++)
			List[i].bMarked = ((List[i].pStats->iType == eventMissionFailure) 
					|| (List[i].pStats->iType == eventMissionSuccess));
		}

	//	Otherwise we check for the type that we want

	else
		{
		EEventTypes iEvent;

		if (strEquals(sStat, ENEMY_OBJS_DESTROYED_STAT))
			iEvent = eventEnemyDestroyedByPlayer;
		else if (strEquals(sStat, FRIENDLY_OBJS_DESTROYED_STAT))
			iEvent = eventFriendDestroyedByPlayer;
		else if (strEquals(sStat, MISSION_FAILURE_STAT))
			iEvent = eventMissionFailure;
		else if (strEquals(sStat, MISSION_SUCCESS_STAT))
			iEvent = eventMissionSuccess;
		else
			return NIL_VALUE;

		//	Mark the events that we're interested in

		for (i = 0; i < List.GetCount(); i++)
			List[i].bMarked = (List[i].pStats->iType == iEvent);
		}

	//	Done

	return GenerateKeyEventStat(List);
	}

bool CPlayerGameStats::GetMatchingKeyEvents (const CString &sNodeID, const CDesignTypeCriteria &Crit, TArray<SKeyEventStatsResult> *retList) const

//	GetMatchingKeyEvents
//
//	Returns a list of all key events that match the criteria

	{
	//	If no nodeID then match all

	if (sNodeID.IsBlank())
		{
		CMapIterator i;

		m_KeyEventStats.Reset(i);
		while (m_KeyEventStats.HasMore(i))
			{
			TArray<SKeyEventStats> *pEventList;
			const CString &sNodeID = m_KeyEventStats.GetNext(i, &pEventList);

			//	Recurse

			if (!AddMatchingKeyEvents(sNodeID, Crit, pEventList, retList))
				return false;
			}
		}

	//	Otherwise look up the node ID

	else
		{
		TArray<SKeyEventStats> *pEventList = m_KeyEventStats.Find(sNodeID);
		if (pEventList == NULL)
			return true;

		if (!AddMatchingKeyEvents(sNodeID, Crit, pEventList, retList))
			return false;
		}

	//	Done

	return true;
	}

CTimeSpan CPlayerGameStats::GetPlayTime (void) const

//  GetPlayTime
//
//  Returns the amount of real-time spent playing
	
	{
	return (!m_PlayTime.IsBlank() ? m_PlayTime : m_Universe.StopGameTime());
	}

CPlayerGameStats::SShipClassStats *CPlayerGameStats::GetShipStats (DWORD dwUNID)

//	GetShipStats
//
//	Get the stats

	{
	SShipClassStats *pStats = m_ShipStats.Find(dwUNID);
	if (pStats == NULL)
		{
		pStats = m_ShipStats.Insert(dwUNID);
		pStats->iEnemyDestroyed = 0;
		pStats->iFriendDestroyed = 0;
		}

	return pStats;
	}

ICCItemPtr CPlayerGameStats::GetStat (const CString &sStat) const

//	GetStat
//
//	Returns the given stat

	{
	if (strEquals(sStat, ASTEROIDS_MINED_STAT))
		{
		//	Total asteroids mined across all systems.

		CMapIterator i;
		int iCount = 0;
		m_SystemStats.Reset(i);
		while (m_SystemStats.HasMore(i))
			{
			SSystemStats *pStats;
			m_SystemStats.GetNext(i, &pStats);

			iCount += pStats->iAsteroidsMined;
			}

		if (iCount == 0)
			return ICCItemPtr::Nil();
		else
			return ICCItemPtr(iCount);
		}
	else if (strEquals(sStat, TONS_OF_ORE_MINED_STAT))
		{
		if (m_iTonsOfOreMined == 0)
			return ICCItemPtr::Nil();
		else
			return ICCItemPtr(m_iTonsOfOreMined);
		}
	else
		{
		CString sResult = GetStatString(sStat);
		if (sResult.IsBlank())
			return ICCItemPtr::Nil();
		else
			return CCodeChain::LinkCode(sResult);
		}
	}

CString CPlayerGameStats::GetStatString (const CString &sStat) const

//	GetStatString
//
//	Returns the given stat

	{
	if (strEquals(sStat, BEST_ENEMY_SHIPS_DESTROYED_STAT))
		{
		DWORD dwUNID;
		int iCount = GetBestEnemyShipsDestroyed(&dwUNID);
		if (iCount == 0)
			return NULL_STR;

		return strPatternSubst(CONSTLIT("'(%d %d)"), dwUNID, iCount);
		}
	else if (strEquals(sStat, ENEMY_SHIPS_DESTROYED_STAT))
		{
		CMapIterator i;
		int iCount = 0;
		m_ShipStats.Reset(i);
		while (m_ShipStats.HasMore(i))
			{
			SShipClassStats *pStats;
			DWORD dwUNID = m_ShipStats.GetNext(i, &pStats);

			iCount += pStats->iEnemyDestroyed;
			}

		if (iCount + m_iExtraEnemyShipsDestroyed == 0)
			return NULL_STR;

		return ::strFromInt(iCount + m_iExtraEnemyShipsDestroyed);
		}
	else if (strEquals(sStat, ENEMY_STATIONS_DESTROYED_STAT))
		{
		CSovereign *pPlayerSovereign = m_Universe.FindSovereign(g_PlayerSovereignUNID);
		if (pPlayerSovereign == NULL)
			return NULL_STR;

		CMapIterator i;
		int iCount = 0;
		m_StationStats.Reset(i);
		while (m_StationStats.HasMore(i))
			{
			SStationTypeStats *pStats;
			DWORD dwUNID = m_StationStats.GetNext(i, &pStats);
			CStationType *pType = m_Universe.FindStationType(dwUNID);
			if (pType == NULL)
				continue;

			if (pType->GetSovereign()->IsEnemy(pPlayerSovereign))
				iCount += pStats->iDestroyed;
			}

		if (iCount == 0)
			return NULL_STR;

		return ::strFromInt(iCount);
		}
	else if (strEquals(sStat, FRIENDLY_SHIPS_DESTROYED_STAT))
		{
		CMapIterator i;
		int iCount = 0;
		m_ShipStats.Reset(i);
		while (m_ShipStats.HasMore(i))
			{
			SShipClassStats *pStats;
			DWORD dwUNID = m_ShipStats.GetNext(i, &pStats);

			iCount += pStats->iFriendDestroyed;
			}

		if (iCount == 0)
			return NULL_STR;

		return ::strFromInt(iCount);
		}
	else if (strEquals(sStat, FRIENDLY_STATIONS_DESTROYED_STAT))
		{
		CSovereign *pPlayerSovereign = m_Universe.FindSovereign(g_PlayerSovereignUNID);
		if (pPlayerSovereign == NULL)
			return NULL_STR;

		CMapIterator i;
		int iCount = 0;
		m_StationStats.Reset(i);
		while (m_StationStats.HasMore(i))
			{
			SStationTypeStats *pStats;
			DWORD dwUNID = m_StationStats.GetNext(i, &pStats);
			CStationType *pType = m_Universe.FindStationType(dwUNID);
			if (pType == NULL)
				continue;

			if (!pType->GetSovereign()->IsEnemy(pPlayerSovereign))
				iCount += pStats->iDestroyed;
			}

		if (iCount == 0)
			return NULL_STR;

		return ::strFromInt(iCount);
		}
	else if (strEquals(sStat, RESURRECT_COUNT_STAT))
		return ::strFromInt(m_iResurrectCount);
	else if (strEquals(sStat, SCORE_STAT))
		return ::strFromInt(m_iScore);
	else if (strEquals(sStat, SYSTEM_DATA_STAT))
		{
		CMemoryWriteStream Output;
		if (Output.Create() != NOERROR)
			return NIL_VALUE;

		Output.Write("'(", 2);

		CMapIterator i;
		m_SystemStats.Reset(i);
		while (m_SystemStats.HasMore(i))
			{
			SSystemStats *pStats;
			const CString &sNodeID = m_SystemStats.GetNext(i, &pStats);

			Output.Write("(", 1);
			Output.Write(sNodeID.GetASCIIZPointer(), sNodeID.GetLength());
			Output.Write(" ", 1);
			WriteTimeValue(Output, pStats->dwFirstEntered);
			Output.Write(" ", 1);
			WriteTimeValue(Output, pStats->dwLastEntered);
			Output.Write(" ", 1);
			WriteTimeValue(Output, pStats->dwLastLeft);
			Output.Write(" ", 1);
			WriteTimeValue(Output, pStats->dwTotalTime);
			Output.Write(") ", 2);
			}

		Output.Write(")", 1);

		return CString(Output.GetPointer(), Output.GetLength());
		}
	else if (strEquals(sStat, SYSTEMS_VISITED_STAT))
		{
		CMapIterator i;
		int iCount = 0;
		m_SystemStats.Reset(i);
		while (m_SystemStats.HasMore(i))
			{
			SSystemStats *pStats;
			const CString &sNodeID = m_SystemStats.GetNext(i, &pStats);

			if (pStats->dwLastEntered != INVALID_TIME)
				iCount++;
			}

		return ::strFromInt(iCount + m_iExtraSystemsVisited);
		}
	else
		return NULL_STR;
	}

bool CPlayerGameStats::HasVisitedMultipleSystems (void) const

//	HasVisitedMultipleSystem
//
//	Returns TRUE if we've visited more than one system.

	{
	CMapIterator i;
	int iCount = 0;
	m_SystemStats.Reset(i);
	while (m_SystemStats.HasMore(i))
		{
		SSystemStats *pStats;
		const CString &sNodeID = m_SystemStats.GetNext(i, &pStats);

		if (pStats->dwLastEntered != INVALID_TIME)
			{
			if (++iCount > 1)
				return true;
			}
		}

	return (iCount + m_iExtraSystemsVisited) > 1;
	}

CPlayerGameStats::SStationTypeStats *CPlayerGameStats::GetStationStats (DWORD dwUNID)

//	GetStationStats
//
//	Get station stats

	{
	SStationTypeStats *pStats = m_StationStats.Find(dwUNID);
	if (pStats == NULL)
		{
		pStats = m_StationStats.Insert(dwUNID);
		pStats->iDestroyed = 0;
		}

	return pStats;
	}

DWORD CPlayerGameStats::GetSystemEnteredTime (const CString &sNodeID)

//	GetSystemEnteredTime
//
//	Returns the tick on which the player last entered the system.
//	0xffffffff if the player has never entered it.

	{
	SSystemStats *pStats = m_SystemStats.Find(sNodeID);
	if (pStats == NULL)
		return INVALID_TIME;

	return pStats->dwLastEntered;
	}

DWORD CPlayerGameStats::GetSystemLastVisitedTime (const CString &sNodeID)

//  GetSystemLastVisitedTime
//
//  Returns the tick on which the player last was in the system.
//  0xffffffff if the player has never entered it.

	{
	SSystemStats *pStats = m_SystemStats.Find(sNodeID);
	if (pStats == NULL)
		return INVALID_TIME;

	//  If we have a value, then return it.

	if (pStats->dwLastLeft != INVALID_TIME)
		return pStats->dwLastLeft;

	//  If we've previously entered the system, but we don't have a LastLeft
	//  value, then we are still in the system.

	if (pStats->dwLastEntered != INVALID_TIME)
		return m_Universe.GetTicks();

	//  Otherwise, we have never been here

	return INVALID_TIME;
	}

const CPlayerGameStats::SSystemStats *CPlayerGameStats::GetSystemStats (const CString &sNodeID) const

//	GetSystemStats
//
//	Get system stats

	{
	if (sNodeID.IsBlank())
		{
		const CTopologyNode *pNode = m_Universe.GetCurrentTopologyNode();
		if (pNode == NULL)
			return NULL;

		return m_SystemStats.Find(pNode->GetID());
		}
	else
		{
		return m_SystemStats.Find(sNodeID);
		}
	}

ICCItemPtr CPlayerGameStats::GetSystemStat (const CString &sStat, const CString &sNodeID) const

//	GetSystemStat
//
//	Returns a stat about the given system.

	{
	const SSystemStats *pStats = GetSystemStats(sNodeID);
	if (pStats == NULL)
		return ICCItemPtr::Nil();

	if (strEquals(sStat, ASTEROIDS_MINED_STAT))
		return ICCItemPtr(pStats->iAsteroidsMined);
	else
		return ICCItemPtr::Nil();
	}

int CPlayerGameStats::IncItemStat (const CString &sStat, const CItemType &ItemType, int iInc)

//	IncItemStat
//
//	Increments a stat for the given item.

	{
	SItemTypeStats *pStats = GetItemStats(ItemType.GetUNID());

	if (strEquals(sStat, ITEMS_BOUGHT_COUNT_STAT))
		{
		pStats->iCountBought += Max(0, iInc);
		return pStats->iCountBought;
		}
	else if (strEquals(sStat, ITEMS_BOUGHT_VALUE_STAT))
		{
		//	NOTE: We assume default currency
		pStats->iValueBought += (CurrencyValue)Max(0, iInc);
		m_TotalValueBought += (CurrencyValue)Max(0, iInc);

		return (int)pStats->iValueBought;
		}
	else if (strEquals(sStat, ITEMS_DAMAGED_HP_STAT))
		{
		pStats->iHPDamaged += Max(0, iInc);
		return pStats->iHPDamaged;
		}
	else if (strEquals(sStat, ITEMS_FIRED_COUNT_STAT))
		{
		pStats->iCountFired += Max(0, iInc);
		return pStats->iCountFired;
		}
	else if (strEquals(sStat, ITEMS_MINED_COUNT_STAT))
		{
		pStats->iCountMined += Max(0, iInc);

		//	For ore, we track some other values and potentially fire 
		//	achievements.

		if (ItemType.HasAttribute(ATTRIB_ORE) && iInc > 0)
			OnOreMined(ItemType, iInc);

		return pStats->iCountMined;
		}
	else if (strEquals(sStat, ITEMS_SOLD_COUNT_STAT))
		{
		pStats->iCountSold += Max(0, iInc);
		return pStats->iCountSold;
		}
	else if (strEquals(sStat, ITEMS_SOLD_VALUE_STAT))
		{
		//	NOTE: We assume default currency
		pStats->iValueSold += (CurrencyValue)Max(0, iInc);
		m_TotalValueSold += (CurrencyValue)Max(0, iInc);

		return (int)pStats->iValueSold;
		}
	else
		return 0;
	}

int CPlayerGameStats::IncStat (const CString &sStat, int iInc)

//	IncStat
//
//	Increments the given stat (and returns the new value)

	{
	if (strEquals(sStat, SCORE_STAT))
		return (m_iScore += iInc);
	else
		return 0;
	}

int CPlayerGameStats::IncSystemStat (const CString &sStat, const CString &sNodeID, int iInc)

//	IntSystemStat
//
//	Increments a system stat

	{
	SSystemStats *pStats = SetSystemStats(sNodeID);
	if (pStats == NULL)
		return 0;

	if (strEquals(sStat, ASTEROIDS_MINED_STAT))
		{
		pStats->iAsteroidsMined += iInc;
		return pStats->iAsteroidsMined;
		}
	else
		return 0;
	}

void CPlayerGameStats::OnGameEnd (CSpaceObject *pPlayer)

//	OnGameEnd
//
//	Game over

	{
	DEBUG_TRY

	//	Mark how long we spent playing the game

	m_GameTime = m_Universe.GetElapsedGameTime();
	m_PlayTime = m_Universe.StopGameTime();

	//	This counts as leaving the system

	OnSystemLeft(m_Universe.GetCurrentSystem());

	//	Set the final time for all installed items

	DEBUG_CATCH
	}

void CPlayerGameStats::OnItemBought (const CItem &Item, const CCurrencyAndValue &TotalValue)

//	OnItemBought
//
//	Player bought an item

	{
	//	Convert to default currency.

	CurrencyValue iTotalValue = m_Universe.GetDefaultCurrency().Exchange(TotalValue);
	if (iTotalValue <= 0)
		return;

	SItemTypeStats *pStats = GetItemStats(Item.GetType()->GetUNID());
	pStats->iCountBought += Item.GetCount();
	pStats->iValueBought += iTotalValue;

	m_TotalValueBought += iTotalValue;
	}

void CPlayerGameStats::OnItemDamaged (const CItem &Item, int iHP)

//	OnItemDamaged
//
//	Player sustained damage

	{
	SItemTypeStats *pStats = GetItemStats(Item.GetType()->GetUNID());
	pStats->iHPDamaged += iHP;
	}

void CPlayerGameStats::OnItemFired (const CItem &Item)

//	OnItemFired
//
//	Player fired the item (weapon or missile)

	{
	SItemTypeStats *pStats = GetItemStats(Item.GetType()->GetUNID());
	pStats->iCountFired++;
	}

void CPlayerGameStats::OnItemInstalled (const CItem &Item)

//	OnItemInstalled
//
//	Player installed an item

	{
	SItemTypeStats *pStats = GetItemStats(Item.GetType()->GetUNID());

	if (pStats->iCountInstalled == 0)
		{
		pStats->dwLastInstalled = m_Universe.GetTicks();
		if (pStats->dwFirstInstalled == INVALID_TIME)
			pStats->dwFirstInstalled = pStats->dwLastInstalled;
		pStats->dwLastUninstalled = INVALID_TIME;
		}

	pStats->iCountInstalled++;
	}

void CPlayerGameStats::OnItemSold (const CItem &Item, const CCurrencyAndValue &TotalValue)

//	OnItemSold
//
//	Player sold an item

	{
	//	Convert to default currency.

	CurrencyValue iTotalValue = m_Universe.GetDefaultCurrency().Exchange(TotalValue);
	if (iTotalValue <= 0)
		return;

	//	Add item stats

	SItemTypeStats *pStats = GetItemStats(Item.GetType()->GetUNID());
	pStats->iCountSold += Item.GetCount();
	pStats->iValueSold += iTotalValue;

	//	Compute profit

	CurrencyValue PrevProfit = m_TotalValueSold - m_TotalValueBought;
	m_TotalValueSold += iTotalValue;
	CurrencyValue CurProfit = PrevProfit + iTotalValue;

	if (CurProfit > 0)
		FireProfitAchievement(PrevProfit, CurProfit);
	}

void CPlayerGameStats::OnItemUninstalled (const CItem &Item)

//	OnItemUninstalled
//
//	Player uninstalled an item

	{
	SItemTypeStats *pStats = GetItemStats(Item.GetType()->GetUNID());

	//	NOTE: This code does not work properly when switching ships.
	//	Since we don't count items on switched ships, when we try to
	//	uninstall, we will underflow.
	//
	//	We need to fix this by adding all installed items to stats 
	//	when we switch ships.

	if (pStats->iCountInstalled <= 0)
		return;

	pStats->iCountInstalled--;

	if (pStats->iCountInstalled == 0)
		{
		ASSERT(pStats->dwLastInstalled != INVALID_TIME);

		pStats->dwLastUninstalled = m_Universe.GetTicks();
		pStats->dwTotalInstalledTime += (pStats->dwLastUninstalled - pStats->dwLastInstalled);
		}
	}

void CPlayerGameStats::OnKeyEvent (EEventTypes iType, CSpaceObject *pObj, DWORD dwCauseUNID)

//	OnKeyEvent
//
//	Adds a key event involving an object

	{
	if (!pObj)
		throw CException(ERR_FAIL);

	CSystem *pSystem = pObj->GetSystem();
	if (pSystem == NULL)
		{
		pSystem = m_Universe.GetCurrentSystem();
		if (pSystem == NULL)
			return;
		}

	//	Get the NodeID where the event happened

	CTopologyNode *pNode = pSystem->GetTopology();
	if (pNode == NULL)
		return;

	const CString &sNodeID = pNode->GetID();

	//	Get the object's type

	CDesignType *pType = pObj->GetType();
	if (pType == NULL)
		return;

	//	Get the object's name

	DWORD dwNameFlags;
	CString sName = pObj->GetNamePattern(0, &dwNameFlags);
	
	//	If the object name is the same as the type name then we don't bother
	//	storing it in the event (to save memory)

	if (sName.IsBlank() || strEquals(sName, pType->GetNamePattern()))
		{
		sName = NULL_STR;
		dwNameFlags = 0;
		}

	//	Look for the list of events for this NodeID

	TArray<SKeyEventStats> *pEventList = m_KeyEventStats.Set(sNodeID);
	SKeyEventStats *pStats = pEventList->Insert();
	pStats->iType = iType;
	pStats->dwTime = m_Universe.GetTicks();
	pStats->dwObjUNID = pType->GetUNID();
	pStats->sObjName = sName;
	pStats->dwObjNameFlags = dwNameFlags;
	pStats->dwCauseUNID = dwCauseUNID;
	}

bool CPlayerGameStats::OnNewMaxSpeed (int iNewMaxSpeed)

//	OnNewMaxSpeed
//
//	Track a new max speed. Returns TRUE if we recorded a new max speed.

	{
	if (auto pStats = GetCurrentPlayerShipStats())
		{
		if (iNewMaxSpeed > pStats->iMaxSpeed)
			{
			pStats->iMaxSpeed = iNewMaxSpeed;
			return true;
			}
		}

	return false;
	}

void CPlayerGameStats::OnObjDestroyedByPlayer (const SDestroyCtx &Ctx, CSpaceObject *pPlayer)

//	OnDestroyedByPlayer
//
//	Object destroyed by player

	{
	bool bIsEnemy = Ctx.Obj.IsEnemy(pPlayer);

	//	Is this a ship?

	CShip *pShip;
	if (Ctx.Obj.GetCategory() == CSpaceObject::catShip && (pShip = Ctx.Obj.AsShip()))
		{
		CShipClass *pClass = pShip->GetClass();
		SShipClassStats *pStats = GetShipStats(pClass->GetUNID());

		if (bIsEnemy)
			{
			pStats->iEnemyDestroyed++;

			//	Adjust score for difficulty

			int iScore = Max(1, mathRound(pClass->GetScore() * m_Universe.GetDifficulty().GetScoreAdj()));
			m_iScore += iScore;

			//	If we killed something with radiation, then achievement.

			if (Ctx.Attacker.GetCause() == killedByRadiationPoisoning)
				{
				m_Universe.SetAchievement(ACHIEVEMENT_CORE_RADIATION_KILL);
				}

			//	If this was caused by an explosion, then we get an achievement.

			if (Ctx.Attacker.IsExplosion())
				{
				m_Universe.SetAchievement(ACHIEVEMENT_CORE_COLLATERAL_DAMAGE);
				}

			//	If the ship was paralyzed, then we get an achievement.

			if (pShip->GetCondition(ECondition::paralyzed))
				{
				m_Universe.SetAchievement(ACHIEVEMENT_CORE_PARALYZE_KILL);
				}
			}
		else
			pStats->iFriendDestroyed++;

		//	If the ship class has an achievement associated...

		if (!pClass->GetAchievement().IsBlank())
			{
			m_Universe.SetAchievement(pClass->GetAchievement());
			}

		//	Tell the sovereign that the player destroyed one of their ships.

		CSovereign *pSovereign = Ctx.Obj.GetSovereign();
		if (pSovereign)
			pSovereign->OnObjDestroyedByPlayer(Ctx);
		}

	//	Is this a station?

	else if (Ctx.Obj.GetCategory() == CSpaceObject::catStation)
		{
		const CStation *pStation = Ctx.Obj.AsStation();

		if (Ctx.Obj.HasAttribute(CONSTLIT("populated"))
				|| Ctx.Obj.HasAttribute(CONSTLIT("score")))
			{
			SStationTypeStats *pStats = GetStationStats(Ctx.Obj.GetType()->GetUNID());

			pStats->iDestroyed++;

			//	Tell the sovereign that the player destroyed one of their stations.

			CSovereign *pSovereign = Ctx.Obj.GetSovereign();
			if (pSovereign)
				pSovereign->OnObjDestroyedByPlayer(Ctx);

			if (bIsEnemy)
				{
				//	If this is a buried station, then achievement.

				if (pStation && pStation->GetHull().GetHullType() == CStationHullDesc::hullUnderground)
					{
					m_Universe.SetAchievement(ACHIEVEMENT_CORE_BURIED_KILL);
					}
				}
			}
		}
	}

void CPlayerGameStats::OnOreMined (const CItemType &ItemType, int iTonsMined)

//	OnOreMined
//
//	Ore has been mined.

	{
	//	Add to our running count.

	int iPrevValue = m_iTonsOfOreMined;
	m_iTonsOfOreMined += iTonsMined;

	//	See if we've reached various achievements, and if so, then fire.

	FireMineOreAchievement(iPrevValue, m_iTonsOfOreMined);
	}

void CPlayerGameStats::OnPowerInvoked (const CPower &Power)

//	OnPowerInvoked
//
//	Player has invoked a power.

	{
	SPowerStats *pStats = m_PowerStats.SetAt(Power.GetUNID());
	pStats->dwInvoked++;
	}

void CPlayerGameStats::OnSwitchPlayerShip (const CShip &NewShip, const CShip *pOldShip)

//	OnSwitchPlayerShip
//
//	Player has switched to a new ship.

	{
	DWORD dwNow = m_Universe.GetTicks();

	//	Stop using the old ship

	if (pOldShip)
		{
		DWORD dwOldClass = pOldShip->GetType()->GetUNID();
		SPlayerShipStats *pOldStats = m_PlayerShipStats.GetAt(dwOldClass);
		if (pOldStats)
			{
			pOldStats->dwLastLeft = dwNow;
			pOldStats->dwTotalTime += dwNow - pOldStats->dwLastEntered;
			}
		}

	//	Add an entry for the new ship.

	DWORD dwNewClass = NewShip.GetType()->GetUNID();
	SPlayerShipStats *pNewStats = m_PlayerShipStats.SetAt(dwNewClass);
	if (pNewStats->dwFirstEntered == INVALID_TIME)
		{
		pNewStats->sClassName = NewShip.GetType()->GetNounPhrase();
		pNewStats->dwFirstEntered = dwNow;
		}
	pNewStats->dwLastEntered = dwNow;
	pNewStats->dwLastLeft = INVALID_TIME;

	int iNewMaxSpeed = mathRound(100.0 * NewShip.GetMaxSpeed() / LIGHT_SPEED);
	if (iNewMaxSpeed > pNewStats->iMaxSpeed)
		pNewStats->iMaxSpeed = iNewMaxSpeed;

	//	If we're switching to a new ship (and we've got an old ship) then track
	//	installed items.
	//
	//	NOTE: We don't need to do this for the first ship, but we do need it for
	//	subsequent ships.

	if (pOldShip)
		{
		CItemListManipulator ItemList(const_cast<CItemList &>(NewShip.GetItemList()));
		while (ItemList.MoveCursorForward())
			{
			const CItem &Item = ItemList.GetItemAtCursor();

			if (Item.IsInstalled()
					&& (Item.IsArmor() || Item.IsDevice()))
				OnItemInstalled(Item);
			}
		}
	}

void CPlayerGameStats::OnSystemEntered (CSystem *pSystem, int *retiLastVisit)

//	OnSystemEntered
//
//	Player just entered the system

	{
	CTopologyNode *pNode = pSystem->GetTopology();
	if (pNode == NULL)
		{
		if (retiLastVisit)
			*retiLastVisit = -1;
		return;
		}

	SSystemStats *pStats = SetSystemStats(pNode->GetID());
	if (pStats == NULL)
		return;

	pStats->dwLastEntered = m_Universe.GetTicks();
	if (pStats->dwFirstEntered == INVALID_TIME)
		{
		pStats->dwFirstEntered = pStats->dwLastEntered;
		if (retiLastVisit)
			*retiLastVisit = -1;
		}
	else
		{
		if (retiLastVisit)
			*retiLastVisit = (pStats->dwLastLeft != INVALID_TIME ? pStats->dwLastLeft : -1);
		}

	pStats->dwLastLeft = INVALID_TIME;
	}

void CPlayerGameStats::OnSystemLeft (CSystem *pSystem)

//	OnSystemLeft
//
//	Player just left the system

	{
	CTopologyNode *pNode = pSystem->GetTopology();
	if (pNode == NULL)
		return;

	SSystemStats *pStats = SetSystemStats(pNode->GetID());
	if (pStats == NULL)
		return;

	ASSERT(pStats->dwLastEntered != INVALID_TIME);
	pStats->dwLastLeft = m_Universe.GetTicks();
	pStats->dwTotalTime += pStats->dwLastLeft - pStats->dwLastEntered;
	}

void CPlayerGameStats::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from stream
//
//	DWORD		m_iScore
//	DWORD		m_iExtraSystemsVisited
//	DWORD		m_iExtraEnemyShipsDestroyed
//	CTimeSpan	m_PlayTime
//	CTimeSpan	m_GameTime
//  Metric      m_rFuelConsumed
//	DWORD		m_iTonsOfOreMined
//	CurrencyValue	m_TotalValueSold
//	CurrencyValue	m_TotalValueBought
//
//	DWORD		Count of item types
//	DWORD			UNID
//	DWORD			iCountSold
//	LONGLONG		iValueSold
//	DWORD			iCountBought
//	LONGLONG		iValueBought
//	DWORD			iCountInstalled
//	DWORD			dwFirstInstalled
//	DWORD			dwLastInstalled
//	DWORD			dwLastUninstalled
//	DWORD			dwTotalInstalledTime
//	DWORD			iCountFired
//	DWORD			iCountMined
//	DWORD			iHPDamaged
//
//	DWORD		Count of ship classes
//	DWORD			UNID
//	DWORD			iEnemyDestroyed
//	DWORD			iFriendDestroyed
//
//	DWORD		Count of station types
//	DWORD			UNID
//	DWORD			iDestroyed
//
//	DWORD		Count of systems
//	CString			NodeID
//	DWORD			dwFirstEntered
//	DWORD			dwLastEntered
//	DWORD			dwLastLeft
//	DWORD			dwTotalTime
//	DWORD			iAsteroidsMined
//
//	DWORD		Count of systems
//	CString			NodeID
//	DWORD			count of key events
//	DWORD				iType
//	DWORD				dwTime
//	DWORD				dwObjUNID
//	DWORD				dwCauseUNID
//	DWORD				dwObjNameFlags
//	CString				sObjName
//
//	DWORD		Count of powers
//	DWORD			UNID
//	DWORD			dwInvoked

	{
	DWORD dwLoad, dwCount;

	//	Prior to version 49 we only saved:
	//
	//	DWORD		m_iScore
	//	DWORD		m_iSystemsVisited
	//	DWORD		m_iEnemiesDestroyed
	//	DWORD		m_pBestEnemyDestroyed (UNID)
	//	DWORD		m_iBestEnemyDestroyedCount

	if (Ctx.dwVersion < 49)
		{
		Ctx.pStream->Read(m_iScore);
		Ctx.pStream->Read(m_iExtraSystemsVisited);
		Ctx.pStream->Read(m_iExtraEnemyShipsDestroyed);

		//	Load the ship class and count of the best enemy

		Ctx.pStream->Read(dwLoad);
		Ctx.pStream->Read(dwCount);

		if (dwLoad)
			{
			SShipClassStats *pShipStats = GetShipStats(dwLoad);
			pShipStats->iEnemyDestroyed += dwCount;

			//	We subtract from extra ships (because these ships are already
			//	included there)

			m_iExtraEnemyShipsDestroyed -= dwCount;
			ASSERT(m_iExtraEnemyShipsDestroyed >= 0);
			}

		return;
		}

	//	New version

	Ctx.pStream->Read(m_iScore);
	Ctx.pStream->Read(m_iExtraSystemsVisited);
	Ctx.pStream->Read(m_iExtraEnemyShipsDestroyed);
	if (Ctx.dwVersion >= 53)
		{
		m_PlayTime.ReadFromStream(Ctx.pStream);
		m_GameTime.ReadFromStream(Ctx.pStream);
		}

	if (Ctx.dwVersion >= 129)
		Ctx.pStream->Read(m_rFuelConsumed);

	if (Ctx.dwVersion >= 207)
		Ctx.pStream->Read(m_iTonsOfOreMined);

	if (Ctx.dwVersion >= 208)
		{
		Ctx.pStream->Read(m_TotalValueSold);
		Ctx.pStream->Read(m_TotalValueBought);
		}

	Ctx.pStream->Read(dwCount);
	for (int i = 0; i < (int)dwCount; i++)
		{
		Ctx.pStream->Read(dwLoad);
		SItemTypeStats *pStats = m_ItemStats.Insert(dwLoad);

		Ctx.pStream->Read(pStats->iCountSold);
		Ctx.pStream->Read(pStats->iValueSold);
		Ctx.pStream->Read(pStats->iCountBought);
		Ctx.pStream->Read(pStats->iValueBought);
		Ctx.pStream->Read(pStats->iCountInstalled);
		Ctx.pStream->Read(pStats->dwFirstInstalled);
		Ctx.pStream->Read(pStats->dwLastInstalled);
		Ctx.pStream->Read(pStats->dwLastUninstalled);
		Ctx.pStream->Read(pStats->dwTotalInstalledTime);
		Ctx.pStream->Read(pStats->iCountFired);

		if (Ctx.dwVersion >= 179)
			Ctx.pStream->Read(pStats->iCountMined);

		if (Ctx.dwVersion >= 102)
			Ctx.pStream->Read(pStats->iHPDamaged);
		}

	Ctx.pStream->Read(dwCount);
	for (int i = 0; i < (int)dwCount; i++)
		{
		Ctx.pStream->Read(dwLoad);
		SShipClassStats *pStats = m_ShipStats.Insert(dwLoad);

		Ctx.pStream->Read(pStats->iEnemyDestroyed);
		Ctx.pStream->Read(pStats->iFriendDestroyed);
		}

	Ctx.pStream->Read(dwCount);
	for (int i = 0; i < (int)dwCount; i++)
		{
		Ctx.pStream->Read(dwLoad);
		SStationTypeStats *pStats = m_StationStats.Insert(dwLoad);

		Ctx.pStream->Read(pStats->iDestroyed);
		}

	Ctx.pStream->Read(dwCount);
	for (int i = 0; i < (int)dwCount; i++)
		{
		CString sNodeID;
		sNodeID.ReadFromStream(Ctx.pStream);
		SSystemStats *pStats = m_SystemStats.Insert(sNodeID);

		Ctx.pStream->Read(pStats->dwFirstEntered);
		Ctx.pStream->Read(pStats->dwLastEntered);
		Ctx.pStream->Read(pStats->dwLastLeft);
		Ctx.pStream->Read(pStats->dwTotalTime);

		if (Ctx.dwVersion >= 179)
			Ctx.pStream->Read(pStats->iAsteroidsMined);
		}

	//	Read the m_KeyEventStats

	if (Ctx.dwVersion >= 74)
		{
		Ctx.pStream->Read(dwCount);
		for (int i = 0; i < (int)dwCount; i++)
			{
			CString sNodeID;
			sNodeID.ReadFromStream(Ctx.pStream);
			TArray<SKeyEventStats> *pEventList = m_KeyEventStats.Insert(sNodeID);

			DWORD dwListCount;
			Ctx.pStream->Read(dwListCount);
			for (int j = 0; j < (int)dwListCount; j++)
				{
				SKeyEventStats *pStats = pEventList->Insert();

				Ctx.pStream->Read(dwLoad);
				pStats->iType = (EEventTypes)dwLoad;

				Ctx.pStream->Read(pStats->dwTime);
				Ctx.pStream->Read(pStats->dwObjUNID);
				Ctx.pStream->Read(pStats->dwCauseUNID);
				Ctx.pStream->Read(pStats->dwObjNameFlags);
				pStats->sObjName.ReadFromStream(Ctx.pStream);
				}
			}
		}

	//	Read m_PlayerShipStats

	if (Ctx.dwVersion >= 209)
		{
		Ctx.pStream->Read(dwCount);
		for (int i = 0; i < (int)dwCount; i++)
			{
			DWORD dwUNID;
			Ctx.pStream->Read(dwUNID);
			SPlayerShipStats *pStats = m_PlayerShipStats.SetAt(dwUNID);

			pStats->sClassName.ReadFromStream(Ctx.pStream);
			Ctx.pStream->Read(pStats->dwFirstEntered);
			Ctx.pStream->Read(pStats->dwLastEntered);
			Ctx.pStream->Read(pStats->dwLastLeft);
			Ctx.pStream->Read(pStats->dwTotalTime);
			Ctx.pStream->Read(pStats->iMaxSpeed);
			}
		}
	else if (Ctx.dwVersion >= 193)
		{
		Ctx.pStream->Read(dwCount);
		for (int i = 0; i < (int)dwCount; i++)
			{
			CString sClassName;
			sClassName.ReadFromStream(Ctx.pStream);
			const CShipClass *pClass = FindClassByName(Ctx, sClassName);

			SPlayerShipStats *pStats = m_PlayerShipStats.SetAt(pClass ? pClass->GetUNID() : 0);
			pStats->sClassName = sClassName;
			Ctx.pStream->Read(pStats->dwFirstEntered);
			Ctx.pStream->Read(pStats->dwLastEntered);
			Ctx.pStream->Read(pStats->dwLastLeft);
			Ctx.pStream->Read(pStats->dwTotalTime);
			if (pClass)
				pStats->iMaxSpeed = mathRound(100.0 * pClass->GetDriveDesc().GetMaxSpeed() / LIGHT_SPEED);
			}
		}

	//	Read m_PowerStats

	if (Ctx.dwVersion >= 202)
		{
		Ctx.pStream->Read(dwCount);
		for (int i = 0; i < (int)dwCount; i++)
			{
			DWORD dwUNID;
			Ctx.pStream->Read(dwUNID);
			auto *pStats = m_PowerStats.SetAt(dwUNID);

			Ctx.pStream->Read(pStats->dwInvoked);
			}
		}
	}

void CPlayerGameStats::SetStat (const CString &sStat, const CString &sValue)

//	SetStat
//
//	Set the stat value

	{
	if (strEquals(sStat, RESURRECT_COUNT_STAT))
		{
		bool bFailed;
		int iValue = strToInt(sValue, 0, &bFailed);
		if (!bFailed)
			m_iResurrectCount = iValue;
		}
	else if (strEquals(sStat, SCORE_STAT))
		{
		bool bFailed;
		int iValue = strToInt(sValue, 0, &bFailed);
		if (!bFailed)
			m_iScore = iValue;
		}
	}

CPlayerGameStats::SSystemStats *CPlayerGameStats::SetSystemStats (const CString &sNodeID)

//	SetSystemStats
//
//	Finds or allocates stats for the given system. If sNodeID is NULL, then we
//	assume the current system.

	{
	if (sNodeID.IsBlank())
		{
		const CTopologyNode *pNode = m_Universe.GetCurrentTopologyNode();
		if (pNode == NULL)
			return NULL;

		return m_SystemStats.Set(pNode->GetID());
		}
	else
		{
		return m_SystemStats.Set(sNodeID);
		}
	}

void CPlayerGameStats::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write to stream
//
//	DWORD		m_iScore
//	DWORD		m_iExtraSystemsVisited
//	DWORD		m_iExtraEnemyShipsDestroyed
//	CTimeSpan	m_PlayTime
//	CTimeSpan	m_GameTime
//  Metric      m_rFuelConsumed
//	DWORD		m_iTonsOfOreMined
//	CurrencyValue	m_TotalValueSold
//	CurrencyValue	m_TotalValueBought
//
//	DWORD		Count of item types
//	DWORD			UNID
//	DWORD			iCountSold
//	LONGLONG		iValueSold
//	DWORD			iCountBought
//	LONGLONG		iValueBought
//	DWORD			iCountInstalled
//	DWORD			dwFirstInstalled
//	DWORD			dwLastInstalled
//	DWORD			dwLastUninstalled
//	DWORD			dwTotalInstalledTime
//	DWORD			iCountFired
//	DWORD			iCountMined
//	DWORD			iHPDamaged
//
//	DWORD		Count of ship classes
//	DWORD			UNID
//	DWORD			iEnemyDestroyed
//	DWORD			iFriendDestroyed
//
//	DWORD		Count of station types
//	DWORD			UNID
//	DWORD			iDestroyed
//
//	DWORD		Count of systems
//	CString			NodeID
//	DWORD			dwFirstEntered
//	DWORD			dwLastEntered
//	DWORD			dwLastLeft
//	DWORD			dwTotalTime
//	DWORD			iAsteroidsMined
//
//	DWORD		Count of systems
//	CString			NodeID
//	DWORD			count of key events
//	DWORD				iType
//	DWORD				dwTime
//	DWORD				dwObjUNID
//	DWORD				dwCauseUNID
//	DWORD				dwObjNameFlags
//	CString				sObjName
//
//	DWORD		Count of powers
//	DWORD			UNID
//	DWORD			dwInvoked

	{
	DWORD dwSave;
	CMapIterator i;

	pStream->Write(m_iScore);
	pStream->Write(m_iExtraSystemsVisited);
	pStream->Write(m_iExtraEnemyShipsDestroyed);
	m_PlayTime.WriteToStream(pStream);
	m_GameTime.WriteToStream(pStream);
	pStream->Write(m_rFuelConsumed);
	pStream->Write(m_iTonsOfOreMined);
	pStream->Write(m_TotalValueSold);
	pStream->Write(m_TotalValueBought);

	dwSave = m_ItemStats.GetCount();
	pStream->Write(dwSave);
	m_ItemStats.Reset(i);
	while (m_ItemStats.HasMore(i))
		{
		SItemTypeStats *pStats;
		DWORD dwUNID = m_ItemStats.GetNext(i, &pStats);

		pStream->Write(dwUNID);
		pStream->Write(pStats->iCountSold);
		pStream->Write(pStats->iValueSold);
		pStream->Write(pStats->iCountBought);
		pStream->Write(pStats->iValueBought);
		pStream->Write(pStats->iCountInstalled);
		pStream->Write(pStats->dwFirstInstalled);
		pStream->Write(pStats->dwLastInstalled);
		pStream->Write(pStats->dwLastUninstalled);
		pStream->Write(pStats->dwTotalInstalledTime);
		pStream->Write(pStats->iCountFired);
		pStream->Write(pStats->iCountMined);
		pStream->Write(pStats->iHPDamaged);
		}

	dwSave = m_ShipStats.GetCount();
	pStream->Write(dwSave);
	m_ShipStats.Reset(i);
	while (m_ShipStats.HasMore(i))
		{
		SShipClassStats *pStats;
		DWORD dwUNID = m_ShipStats.GetNext(i, &pStats);

		pStream->Write(dwUNID);
		pStream->Write(pStats->iEnemyDestroyed);
		pStream->Write(pStats->iFriendDestroyed);
		}

	dwSave = m_StationStats.GetCount();
	pStream->Write(dwSave);
	m_StationStats.Reset(i);
	while (m_StationStats.HasMore(i))
		{
		SStationTypeStats *pStats;
		DWORD dwUNID = m_StationStats.GetNext(i, &pStats);

		pStream->Write(dwUNID);
		pStream->Write(pStats->iDestroyed);
		}

	dwSave = m_SystemStats.GetCount();
	pStream->Write(dwSave);
	m_SystemStats.Reset(i);
	while (m_SystemStats.HasMore(i))
		{
		SSystemStats *pStats;
		const CString &sNodeID = m_SystemStats.GetNext(i, &pStats);

		sNodeID.WriteToStream(pStream);
		pStream->Write(pStats->dwFirstEntered);
		pStream->Write(pStats->dwLastEntered);
		pStream->Write(pStats->dwLastLeft);
		pStream->Write(pStats->dwTotalTime);
		pStream->Write(pStats->iAsteroidsMined);
		}

	dwSave = m_KeyEventStats.GetCount();
	pStream->Write(dwSave);
	m_KeyEventStats.Reset(i);
	while (m_KeyEventStats.HasMore(i))
		{
		TArray<SKeyEventStats> *pEventList;
		const CString &sNodeID = m_KeyEventStats.GetNext(i, &pEventList);

		sNodeID.WriteToStream(pStream);

		DWORD dwCount = pEventList->GetCount();
		pStream->Write(dwCount);
		for (int j = 0; j < (int)dwCount; j++)
			{
			SKeyEventStats *pStats = &pEventList->GetAt(j);

			dwSave = (DWORD)pStats->iType;
			pStream->Write(dwSave);

			pStream->Write(pStats->dwTime);
			pStream->Write(pStats->dwObjUNID);
			pStream->Write(pStats->dwCauseUNID);
			pStream->Write(pStats->dwObjNameFlags);
			pStats->sObjName.WriteToStream(pStream);
			}
		}

	pStream->Write(m_PlayerShipStats.GetCount());
	for (int i = 0; i < m_PlayerShipStats.GetCount(); i++)
		{
		pStream->Write(m_PlayerShipStats.GetKey(i));

		const auto &Stats = m_PlayerShipStats[i];
		Stats.sClassName.WriteToStream(pStream);
		pStream->Write(Stats.dwFirstEntered);
		pStream->Write(Stats.dwLastEntered);
		pStream->Write(Stats.dwLastLeft);
		pStream->Write(Stats.dwTotalTime);
		pStream->Write(Stats.iMaxSpeed);
		}

	pStream->Write(m_PowerStats.GetCount());
	for (int i = 0; i < m_PowerStats.GetCount(); i++)
		{
		pStream->Write(m_PowerStats.GetKey(i));
		pStream->Write(m_PowerStats[i].dwInvoked);
		}
	}

void CPlayerGameStats::WriteTimeValue (CMemoryWriteStream &Output, DWORD dwTime)
	{
	if (dwTime == INVALID_TIME)
		Output.Write(NIL_VALUE.GetASCIIZPointer(), NIL_VALUE.GetLength());
	else
		{
		CString sInt = strFromInt(dwTime);
		Output.Write(sInt.GetASCIIZPointer(), sInt.GetLength());
		}
	}

//	CStatCounterArray ---------------------------------------------------------

void CStatCounterArray::GenerateGameStats (CGameStats &Stats)

//	GenerateGameStats
//
//	Adds the list of stats to the target

	{
	//	Loop over all sections

	CMapIterator i;
	m_Array.Reset(i);
	while (m_Array.HasMore(i))
		{
		TMap<CString, SEntry> *pSection;
		CString sSection = m_Array.GetNext(i, &pSection);

		//	Loop over all stats

		CMapIterator j;
		pSection->Reset(j);
		while (pSection->HasMore(j))
			{
			SEntry *pEntry;
			CString sStat = pSection->GetNext(j, &pEntry);

			//	Add it

			Stats.Insert(sStat, 
					::strFormatInteger(pEntry->iCount, -1, FORMAT_THOUSAND_SEPARATOR | FORMAT_UNSIGNED), 
					sSection, 
					pEntry->sSort);
			}
		}
	}

void CStatCounterArray::Insert (const CString &sStat, int iCount, const CString &sSection, const CString &sSort)

//	Insert
//
//	Adds the given stat. If the stat already exists (by name) the counts are added

	{
	if (iCount == 0)
		return;

	ASSERT(!sStat.IsBlank());
	ASSERT(!sSection.IsBlank());

	//	First look for the section

	TMap<CString, SEntry> *pSection = m_Array.Find(sSection);
	if (pSection == NULL)
		pSection = m_Array.Insert(sSection);

	//	Next look for the entry

	SEntry *pEntry = pSection->Find(sStat);
	if (pEntry == NULL)
		{
		pEntry = pSection->Insert(sStat);
		pEntry->iCount = iCount;
		pEntry->sSort = sSort;
		}
	else
		{
		if (iCount > pEntry->iCount)
			pEntry->sSort = sSort;

		pEntry->iCount += iCount;
		}
	}


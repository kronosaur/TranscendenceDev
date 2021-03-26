//	SystemTest.cpp
//
//	Generate statistics about station frequency by system
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int MAX_LEVEL = 26;
static constexpr int DEFAULT_SYSTEM_UPDATES =		60;

#define NO_LOGO_SWITCH						CONSTLIT("nologo")
#define FIELD_NAME							CONSTLIT("shortName")

#define BY_ITEM_ATTRIB						CONSTLIT("byItem")
#define BY_LEVEL_ATTRIB						CONSTLIT("byLevel")
#define BY_SYSTEM_ATTRIB					CONSTLIT("bySystem")
#define COUNT_ATTRIB						CONSTLIT("count")
#define CRITERIA_ATTRIB						CONSTLIT("criteria")
#define OBJ_CRITERIA_ATTRIB					CONSTLIT("objCriteria")

class CSystemTestGenerator
	{
	public:
		CSystemTestGenerator (CUniverse &Universe, const CXMLElement &CmdLine) :
				m_Universe(Universe),
				m_CmdLine(CmdLine)
			{
			Init();
			}

		void RunItemFrequencyTest (void);
		void RunSystemTest (void);

	private:
		enum class EOutput
			{
			ByLevel,
			BySystem,
			ByItemType,
			};

		struct SShipClassInfo
			{
			CShipClass *pClass = NULL;
			int iTotalCount = 0;
			};

		struct SSystemInfo
			{
			CString sName;
			int iLevel;
			DWORD dwSystemType;
			int iCount;											//	Number of times this system instance 
																//	has appeared.

			TSortMap<CString, StationInfo> Stations;			//	All station types that have ever appeared
																//	in this system instance.

			TSortMap<DWORD, ItemInfo> Items;					//	All items types that have ever appeared in
																//	this system instace.

			TSortMap<DWORD, SShipClassInfo> ShipBrokers;		//	All ship classes available at ship brokers
			};

		void ComputeShipBrokerStats (const CSpaceObject &ShipBrokerObj, ICCItem &ShipList, SSystemInfo &ioSystemInfo) const;
		void ComputeStationStats (const TSortMap<CString, SSystemInfo> &AllSystems, TArray<TSortMap<CString, StationInfo>> &retAllStations) const;
		CString GenerateStationKey (CStationType *pType, CString *retsCategory = NULL) const;
		void Init (void);
		static void PrintItemStats (const TSortMap<CString, SSystemInfo> &AllSystems, int iSampleCount);
		void PrintStationStats (const TArray<TSortMap<CString, StationInfo>> &AllStations) const;
		void PrintStats (const TSortMap<CString, SSystemInfo> &AllSystems) const;
		void PrintSystemStats (const SSystemInfo &SystemInfo) const;
		static void PrintSystemItemStats (const TSortMap<CString, SSystemInfo> &AllSystems, int iSampleCount);
		bool RunItemFrequencyTestGame (TSortMap<CString, SSystemInfo> &AllSystems, int iSampleIndex);
		bool RunSystemTestGame (TSortMap<CString, SSystemInfo> &AllSystems);
		void UpdateSystem (CSystem &System, int iUpdateCount);

		CUniverse &m_Universe;
		const CXMLElement &m_CmdLine;

		bool m_bLogo = false;
		int m_iSystemSample = 0;
		EOutput m_iOutput = EOutput::BySystem;

		CSpaceObjectCriteria m_ObjCriteria;
		CItemCriteria m_ItemCriteria;

		CSovereign *m_pPlayer = NULL;
		CString m_sInitError;
	};

void GenerateItemFrequencyTable (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	CSystemTestGenerator Test(Universe, *pCmdLine);
	Test.RunItemFrequencyTest();
	}

void GenerateSystemTest (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	CSystemTestGenerator Test(Universe, *pCmdLine);
	Test.RunSystemTest();
	}

void CSystemTestGenerator::ComputeShipBrokerStats (const CSpaceObject &ShipBrokerObj, ICCItem &ShipList, SSystemInfo &ioSystemInfo) const
	{
	CSystem *pSystem = ShipBrokerObj.GetSystem();
	if (pSystem == NULL)
		return;

	//	Each value in the list is an ID.

	for (int i = 0; i < ShipList.GetCount(); i++)
		{
		DWORD dwID = ShipList.GetElement(i)->GetIntegerValue();
		const CSpaceObject *pObj = pSystem->FindObject(dwID);
		if (pObj == NULL)
			continue;

		CShipClass *pClass = CShipClass::AsType(pObj->GetType());
		if (pClass == NULL)
			continue;

		bool bNew;
		SShipClassInfo *pEntry = ioSystemInfo.ShipBrokers.SetAt(pClass->GetUNID(), &bNew);
		if (bNew)
			{
			pEntry->pClass = pClass;
			pEntry->iTotalCount = 1;
			}
		else
			pEntry->iTotalCount++;
		}
	}

void CSystemTestGenerator::ComputeStationStats (const TSortMap<CString, SSystemInfo> &AllSystems, TArray<TSortMap<CString, StationInfo>> &retAllStations) const
	{
	retAllStations.InsertEmpty(MAX_LEVEL);

	//	Compute stats for stations by level

	for (int i = 0; i < AllSystems.GetCount(); i++)
		{
		const SSystemInfo &SystemEntry = AllSystems[i];

		if (SystemEntry.iCount > 0)
			{
			//	Get the destination

			TSortMap<CString, StationInfo> &Dest = retAllStations[SystemEntry.iLevel];

			//	Figure out the probability that this system will appear

			int iSystemProb = SystemEntry.iCount * 100 / m_iSystemSample;

			//	Add stations from this system

			for (int j = 0; j < SystemEntry.Stations.GetCount(); j++)
				{
				const StationInfo &Entry = SystemEntry.Stations[j];

				//	Figure out the average number of stations of this type
				//	in this system per game (in hundredths)

				int iCount = Entry.iTotalCount * 100 / SystemEntry.iCount;
				iCount = iCount * iSystemProb / 100;

				//	Add it

				CString sCategory;
				CString sKey = GenerateStationKey(Entry.pType, &sCategory);

				bool bNew;
				StationInfo *pDestEntry = Dest.SetAt(sKey, &bNew);
				if (bNew)
					{
					pDestEntry->sCategory = sCategory;
					pDestEntry->pType = Entry.pType;
					pDestEntry->iTotalCount = iCount;
					}
				else
					pDestEntry->iTotalCount += iCount;
				}
			}
		}
	}

CString CSystemTestGenerator::GenerateStationKey (CStationType *pType, CString *retsCategory) const
	{
	//	Generate sort order

	const char *pCat;
	if (pType->HasLiteralAttribute(CONSTLIT("debris")))
		pCat = "debris";
	else if (pType->GetControllingSovereign()->IsEnemy(m_pPlayer))
		pCat = "enemy";
	else if (pType->GetScale() == scaleStar || pType->GetScale() == scaleWorld)
		pCat = "-";
	else
		pCat = "friend";

	char szBuffer[1024];
	wsprintf(szBuffer, "%s %s", pCat, pType->GetNounPhrase().GetASCIIZPointer());

	if (retsCategory)
		*retsCategory = CString(pCat);

	return CString(szBuffer);
	}

void CSystemTestGenerator::Init (void)
	{
	m_bLogo = !m_CmdLine.GetAttributeBool(NO_LOGO_SWITCH);
	m_iSystemSample = m_CmdLine.GetAttributeIntegerBounded(COUNT_ATTRIB, 1, -1, 1);

	if (m_CmdLine.GetAttributeBool(BY_SYSTEM_ATTRIB))
		m_iOutput = EOutput::BySystem;
	else if (m_CmdLine.GetAttributeBool(BY_LEVEL_ATTRIB))
		m_iOutput = EOutput::ByLevel;
	else if (m_CmdLine.GetAttributeBool(BY_ITEM_ATTRIB))
		m_iOutput = EOutput::ByItemType;
	else
		m_iOutput = EOutput::BySystem;

	m_pPlayer = m_Universe.FindSovereign(g_PlayerSovereignUNID);
	if (m_pPlayer == NULL)
		{
		m_sInitError = CONSTLIT("Unable to find player sovereign.");
		return;
		}

	CString sCriteria;
	if (m_CmdLine.FindAttribute(OBJ_CRITERIA_ATTRIB, &sCriteria))
		m_ObjCriteria = CSpaceObjectCriteria(sCriteria);
	else
		m_ObjCriteria = CSpaceObjectCriteria(CONSTLIT("*"));

	m_ItemCriteria = CItemCriteria(m_CmdLine.GetAttribute(CRITERIA_ATTRIB), CItemCriteria::ALL);
	}

void CSystemTestGenerator::PrintItemStats (const TSortMap<CString, SSystemInfo> &AllSystems, int iSampleCount)
	{
	printf("Level\tItem\tLocation\tCount\t%% Games\n");

	//	Aggregate counts for each unique type and level.

	TSortMap<CString, ItemInfo::SFoundDesc> AllEntries;
	for (int i = 0; i < AllSystems.GetCount(); i++)
		{
		for (int j = 0; j < AllSystems[i].Items.GetCount(); j++)
			{
			for (int k = 0; k < AllSystems[i].Items[j].FoundOn.GetCount(); k++)
				{
				const auto &Entry = AllSystems[i].Items[j].FoundOn[k];

				bool bNew;
				auto *pNewEntry = AllEntries.SetAt(AllSystems[i].Items[j].FoundOn.GetKey(k), &bNew);
				if (bNew)
					*pNewEntry = Entry;
				else
					{
					pNewEntry->iTotalCount += Entry.iTotalCount;

					for (int l = 0; l < Entry.Games.GetCount(); l++)
						{
						bool bNew;
						auto *pGames = pNewEntry->Games.SetAt(Entry.Games.GetKey(l), &bNew);
						if (bNew)
							*pGames = Entry.Games[l];
						else
							*pGames += Entry.Games[l];
						}
					}
				}
			}
		}

	//	Now sort by item name and then system level

	TSortMap<CString, int> Sort;
	for (int i = 0; i < AllEntries.GetCount(); i++)
		{
		Sort.Insert(strPatternSubst(CONSTLIT("%s/%02d"), AllEntries[i].pItemType->GetNounPhrase(), AllEntries[i].iSystemLevel), i);
		}

	//	Output

	for (int i = 0; i < Sort.GetCount(); i++)
		{
		const auto &Entry = AllEntries[Sort[i]];

		double rGames = 100.0 * Entry.Games.GetCount() / (double)iSampleCount;

		printf("%d\t%s\t%s\t%s\t%s%%\n",
			Entry.iSystemLevel,
			(LPSTR)Entry.pItemType->GetNounPhrase(),
			(LPSTR)Entry.pFoundOn->GetNounPhrase(),
			(LPSTR)strFromDouble((double)Entry.iTotalCount / (double)iSampleCount, 2),
			(LPSTR)strFromDouble(rGames, 1)
			);
		}
	}

void CSystemTestGenerator::PrintStationStats (const TArray<TSortMap<CString, StationInfo>> &AllStations) const
	{
	//	Output stats for stations by level

	printf("AVERAGE STATION ENCOUNTERS BY LEVEL\n\n");

	printf("Level\tFOF\tEncounter\tCount\tFreq\n");

	for (int i = 0; i < MAX_LEVEL; i++)
		{
		if (AllStations[i].GetCount())
			{
			for (int j = 0; j < AllStations[i].GetCount(); j++)
				{
				const StationInfo *pEntry = &AllStations[i].GetValue(j);
				const CStationEncounterDesc &EncounterDesc = pEntry->pType->GetEncounterDescConst();

				printf("%d\t%s\t%s\t%d.%02d\t%d\n", 
						i,
						pEntry->sCategory.GetASCIIZPointer(),
						pEntry->pType->GetNounPhrase().GetASCIIZPointer(),
						pEntry->iTotalCount / 100,
						pEntry->iTotalCount % 100,
						pEntry->pType->GetFrequencyByLevel(i, EncounterDesc));
				}
			}
		}
	}

void CSystemTestGenerator::PrintStats (const TSortMap<CString, SSystemInfo> &AllSystems) const
	{
	printf("FINAL SYSTEM STATISTICS\n\n");

	//	Output stats for all systems

	for (int i = 0; i < AllSystems.GetCount(); i++)
		{
		const SSystemInfo &SystemEntry = AllSystems[i];
		PrintSystemStats(SystemEntry);
		}
	}

void CSystemTestGenerator::PrintSystemItemStats (const TSortMap<CString, SSystemInfo> &AllSystems, int iSampleCount)
	{
	printf("Level\tSystem\tItem\tCount\n");

	for (int i = 0; i < AllSystems.GetCount(); i++)
		{
		const SSystemInfo SystemEntry = AllSystems[i];

		for (int j = 0; j < SystemEntry.Items.GetCount(); j++)
			{
			const ItemInfo &Entry = SystemEntry.Items[j];

			printf("%d\t%s\t%s\t%.2f\n",
					SystemEntry.iLevel,
					SystemEntry.sName.GetASCIIZPointer(),
					Entry.pType->GetDataField(FIELD_NAME).GetASCIIZPointer(),
					(double)Entry.iTotalCount / (double)iSampleCount);
			}
		}
	}

void CSystemTestGenerator::PrintSystemStats (const SSystemInfo &SystemInfo) const
	{
	//	Print

	printf("Level %s: %s (%x) %d\n\n", 
			(LPSTR)strLevel(SystemInfo.iLevel),
			SystemInfo.sName.GetASCIIZPointer(),
			SystemInfo.dwSystemType,
			SystemInfo.iCount);

	//	Output stations in sorted order

	printf("STATION TYPES\n");
	for (int j = 0; j < SystemInfo.Stations.GetCount(); j++)
		{
		const StationInfo &Entry = SystemInfo.Stations[j];

		printf("%s\t%s\t", 
				Entry.sCategory.GetASCIIZPointer(),
				Entry.pType->GetNounPhrase().GetASCIIZPointer());

		for (int k = 0; k < MAX_FREQUENCY_COUNT; k++)
			{
			int iPercent = 100 * Entry.iFreqCount[k] / SystemInfo.iCount;

			if (Entry.iFreqCount[k])
				printf("%d (%d%%); ", k, iPercent);
			}

		printf("%d average\n", Entry.iTotalCount / SystemInfo.iCount);
		}

	printf("\n");

	//	Output ship broker inventory

	if (SystemInfo.ShipBrokers.GetCount() > 0)
		{
		printf("SHIP BROKER INVENTORY\n");
		for (int i = 0; i < SystemInfo.ShipBrokers.GetCount(); i++)
			{
			const SShipClassInfo &Entry = SystemInfo.ShipBrokers[i];

			printf("%08x: Level %d %s\t%s\n",
				Entry.pClass->GetUNID(),
				Entry.pClass->GetLevel(),
				(LPSTR)Entry.pClass->GetNounPhrase(),
				(LPSTR)strFromDouble((double)Entry.iTotalCount / (double)SystemInfo.iCount, 2)
				);
			}

		printf("\n");
		}
	}

void CSystemTestGenerator::RunItemFrequencyTest ()
	{
	CString sError;

	if (!m_sInitError.IsBlank())
		{
		printf("ERROR: %s\n", (LPSTR)m_sInitError);
		return;
		}

	TSortMap<CString, SSystemInfo> AllSystems;
	for (int i = 0; i < m_iSystemSample; i++)
		{
		printf("pass %d...\n", i + 1);

		if (!RunItemFrequencyTestGame(AllSystems, i))
			return;

		m_Universe.Reinit();
		if (m_Universe.InitGame(0, &sError) != NOERROR)
			{
			printf("ERROR: %s\n", sError.GetASCIIZPointer());
			return;
			}
		}

	//	Output all items

	MarkItemsKnown(m_Universe);

	switch (m_iOutput)
		{
		case EOutput::ByItemType:
			if (m_bLogo)
				printf("FINAL ITEM STATISTICS\n\n");

			PrintItemStats(AllSystems, m_iSystemSample);
			break;

		default:
			if (m_bLogo)
				printf("FINAL SYSTEM STATISTICS\n\n");

			PrintSystemItemStats(AllSystems, m_iSystemSample);
			break;
		}
	}

bool CSystemTestGenerator::RunItemFrequencyTestGame (TSortMap<CString, SSystemInfo> &AllSystems, int iSampleIndex)
	{
	ALERROR error;

	for (int iNode = 0; iNode < m_Universe.GetTopologyNodeCount(); iNode++)
		{
		CTopologyNode *pNode = m_Universe.GetTopologyNode(iNode);
		if (pNode->IsEndGame())
			continue;

		printf("%s\n", (LPSTR)pNode->GetSystemName());

		//	Create the system

		CSystem *pSystem;
		CString sError;
		if (error = m_Universe.CreateStarSystem(*pNode, &pSystem, &sError))
			{
			printf("ERROR: %s\n", sError.GetASCIIZPointer());
			return false;
			}

		UpdateSystem(*pSystem, DEFAULT_SYSTEM_UPDATES);

		//	Find this system in the table.

		bool bNew;
		SSystemInfo *pSystemEntry = AllSystems.SetAt(pNode->GetSystemName(), &bNew);
		if (bNew)
			{
			pSystemEntry->sName = pNode->GetSystemName();
			pSystemEntry->iLevel = pNode->GetLevel();
			pSystemEntry->dwSystemType = pNode->GetSystemTypeUNID();
			pSystemEntry->iCount = 1;
			}
		else
			pSystemEntry->iCount++;

		//	Create a table of all items

		for (int j = 0; j < pSystem->GetObjectCount(); j++)
			{
			CSpaceObject *pObj = pSystem->GetObject(j);
			if (pObj == NULL)
				continue;

			if (!pObj->MatchesCriteria(m_ObjCriteria))
				continue;

			//	Enumerate the items in this object

			CItemListManipulator ItemList(pObj->GetItemList());
			ItemList.ResetCursor();
			while (ItemList.MoveCursorForward())
				{
				const CItem &Item(ItemList.GetItemAtCursor());

				if (!Item.MatchesCriteria(m_ItemCriteria))
					continue;

				if (Item.IsInstalled() || Item.IsDamaged())
					continue;

				bool bNew;
				ItemInfo *pEntry = pSystemEntry->Items.SetAt(Item.GetType()->GetUNID(), &bNew);
				if (bNew)
					{
					pEntry->pType = Item.GetType();
					pEntry->iTotalCount = Item.GetCount();
					}
				else
					pEntry->iTotalCount += Item.GetCount();

				if (const CDesignType *pType = pObj->GetType())
					{
					pEntry->AddFoundOnEntry(Item, *pType, pNode->GetLevel(), iSampleIndex);

					//	Also add an entry representing all systems.

					pEntry->AddFoundOnEntry(Item, *pType, 0, iSampleIndex);
					}
				}
			}

		//	Done with old system

		m_Universe.DestroySystem(pSystem);
		}

	return true;
	}

void CSystemTestGenerator::RunSystemTest (void)
	{
	CString sError;

	if (!m_sInitError.IsBlank())
		{
		printf("ERROR: %s\n", (LPSTR)m_sInitError);
		return;
		}

	TSortMap<CString, SSystemInfo> AllSystems;
	for (int i = 0; i < m_iSystemSample; i++)
		{
		printf("pass %d...\n", i + 1);

		if (!RunSystemTestGame(AllSystems))
			return;

		m_Universe.Reinit();
		if (m_Universe.InitGame(0, &sError) != NOERROR)
			{
			printf("ERROR: %s\n", sError.GetASCIIZPointer());
			return;
			}
		}

	switch (m_iOutput)
		{
		case EOutput::ByLevel:
			{
			TArray<TSortMap<CString, StationInfo>> AllStations;
			ComputeStationStats(AllSystems, AllStations);
			PrintStationStats(AllStations);
			break;
			}

		default:
			{
			PrintStats(AllSystems);
			break;
			}
		}
	}

bool CSystemTestGenerator::RunSystemTestGame (TSortMap<CString, SSystemInfo> &AllSystems)
	{
	ALERROR error;
	CCodeChainCtx CCCtx(m_Universe);

	for (int iNode = 0; iNode < m_Universe.GetTopologyNodeCount(); iNode++)
		{
		CTopologyNode *pNode = m_Universe.GetTopologyNode(iNode);
		if (pNode->IsEndGame())
			continue;

		printf("%s\n", (LPSTR)pNode->GetSystemName());

		//	Create the system

		CSystem *pSystem;
		CString sError;
		if (error = m_Universe.CreateStarSystem(*pNode, &pSystem, &sError))
			{
			printf("ERROR: %s\n", sError.GetASCIIZPointer());
			return false;
			}

		//	Find this system in the table.

		bool bNew;
		SSystemInfo *pSystemEntry = AllSystems.SetAt(pNode->GetSystemName(), &bNew);
		if (bNew)
			{
			pSystemEntry->sName = pNode->GetSystemName();
			pSystemEntry->iLevel = pNode->GetLevel();
			pSystemEntry->dwSystemType = pNode->GetSystemTypeUNID();
			pSystemEntry->iCount = 1;
			}
		else
			pSystemEntry->iCount++;

		//	Reset stations counts for this system

		for (int j = 0; j < pSystemEntry->Stations.GetCount(); j++)
			{
			StationInfo *pEntry = &pSystemEntry->Stations[j];
			pEntry->iTempCount = 0;
			}

		//	Create a table of all stations

		for (int j = 0; j < pSystem->GetObjectCount(); j++)
			{
			CSpaceObject *pObj = pSystem->GetObject(j);
			CStationType *pType;

			if (pObj 
					&& (pType = pObj->GetEncounterInfo()))
				{
				//	Generate sort order

				CString sCategory;
				CString sKey = GenerateStationKey(pType, &sCategory);

				//	See if we have this type in the table

				bool bNew;
				StationInfo *pEntry = pSystemEntry->Stations.SetAt(sKey, &bNew);
				if (bNew)
					{
					pEntry->sCategory = sCategory;
					pEntry->pType = pType;
					pEntry->iSystemCount = 0;
					pEntry->iTotalCount = 0;

					for (int k = 0; k < MAX_FREQUENCY_COUNT; k++)
						pEntry->iFreqCount[k] = 0;

					pEntry->iTempCount = 1;
					}
				else
					pEntry->iTempCount++;

				//	If this station is a ship broker, then figure out which 
				//	ships it has for sale.

				ICCItemPtr pShipList = pObj->GetProperty(CCCtx, CONSTLIT("rpg.shipBrokerInventory"));
				if (pShipList && !pShipList->IsNil())
					ComputeShipBrokerStats(*pObj, *pShipList, *pSystemEntry);
				}
			}

		//	Output in sorted order

		for (int j = 0; j < pSystemEntry->Stations.GetCount(); j++)
			{
			StationInfo *pEntry = &pSystemEntry->Stations[j];

			if (pEntry->iTempCount)
				{
				pEntry->iSystemCount++;
				pEntry->iTotalCount += pEntry->iTempCount;

				if (pEntry->iTempCount < MAX_FREQUENCY_COUNT)
					pEntry->iFreqCount[pEntry->iTempCount]++;
				}
			}

		//	Done with old system

		m_Universe.DestroySystem(pSystem);
		}

	return true;
	}

void CSystemTestGenerator::UpdateSystem (CSystem &System, int iUpdateCount)
	{
	SSystemUpdateCtx UpdateCtx;
	UpdateCtx.bForceEventFiring = true;
	UpdateCtx.bForcePainted = true;

	//	Set the POV

	CSpaceObject *pPOV = System.GetObject(0);
	m_Universe.SetPOV(pPOV);
	System.SetPOVLRS(pPOV);

	//	Prepare system

	m_Universe.UpdateExtended();
	m_Universe.GarbageCollectLibraryBitmaps();

	//	Run diagnostics start

	//	Update for a while

	for (int i = 0; i < iUpdateCount; i++)
		{
		m_Universe.Update(UpdateCtx);
		}
	}

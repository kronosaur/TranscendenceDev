//	SystemTest.cpp
//
//	Generate statistics about station frequency by system
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

const int MAX_LEVEL = 26;

#define NO_LOGO_SWITCH						CONSTLIT("nologo")
#define FIELD_NAME							CONSTLIT("shortName")

#define BY_LEVEL_ATTRIB						CONSTLIT("byLevel")
#define BY_SYSTEM_ATTRIB					CONSTLIT("bySystem")
#define COUNT_ATTRIB						CONSTLIT("count")

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
		enum EOutputType
			{
			outputByLevel,
			outputBySystem,
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
		void PrintStationStats (const TArray<TSortMap<CString, StationInfo>> &AllStations) const;
		void PrintStats (const TSortMap<CString, SSystemInfo> &AllSystems) const;
		void PrintSystemStats (const SSystemInfo &SystemInfo) const;
		bool RunItemFrequencyTestGame (TSortMap<CString, SSystemInfo> &AllSystems);
		bool RunSystemTestGame (TSortMap<CString, SSystemInfo> &AllSystems);

		CUniverse &m_Universe;
		const CXMLElement &m_CmdLine;

		bool m_bLogo = false;
		int m_iSystemSample = 0;
		EOutputType m_iOutput = outputBySystem;

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

	char *pCat;
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
		m_iOutput = outputBySystem;
	else if (m_CmdLine.GetAttributeBool(BY_LEVEL_ATTRIB))
		m_iOutput = outputByLevel;
	else
		m_iOutput = outputBySystem;

	m_pPlayer = m_Universe.FindSovereign(g_PlayerSovereignUNID);
	if (m_pPlayer == NULL)
		{
		m_sInitError = CONSTLIT("Unable to find player sovereign.");
		return;
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

				printf("%d\t%s\t%s\t%d.%02d\t%d\n", 
						i,
						pEntry->sCategory.GetASCIIZPointer(),
						pEntry->pType->GetNounPhrase().GetASCIIZPointer(),
						pEntry->iTotalCount / 100,
						pEntry->iTotalCount % 100,
						pEntry->pType->GetFrequencyByLevel(i));
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

void CSystemTestGenerator::RunItemFrequencyTest (void)
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

		if (!RunItemFrequencyTestGame(AllSystems))
			return;

		m_Universe.Reinit();
		if (m_Universe.InitGame(0, &sError) != NOERROR)
			{
			printf("ERROR: %s\n", sError.GetASCIIZPointer());
			return;
			}
		}

	if (m_bLogo)
		printf("FINAL SYSTEM STATISTICS\n\n");

	printf("Level\tSystem\tItem\tCount\n");

	//	Output all items

	MarkItemsKnown(m_Universe);

	for (int i = 0; i < AllSystems.GetCount(); i++)
		{
		SSystemInfo *pSystemEntry = &AllSystems[i];

		for (int j = 0; j < pSystemEntry->Items.GetCount(); j++)
			{
			ItemInfo *pEntry = &pSystemEntry->Items[j];

			printf("%d\t%s\t%s\t%.2f\n",
					pSystemEntry->iLevel,
					pSystemEntry->sName.GetASCIIZPointer(),
					pEntry->pType->GetDataField(FIELD_NAME).GetASCIIZPointer(),
					(double)pEntry->iTotalCount / (double)m_iSystemSample);
			}
		}
	}

bool CSystemTestGenerator::RunItemFrequencyTestGame (TSortMap<CString, SSystemInfo> &AllSystems)
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
		if (error = m_Universe.CreateStarSystem(pNode, &pSystem, &sError))
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

		//	Create a table of all items

		for (int j = 0; j < pSystem->GetObjectCount(); j++)
			{
			CSpaceObject *pObj = pSystem->GetObject(j);

			if (pObj)
				{
				//	Enumerate the items in this object

				CItemListManipulator ItemList(pObj->GetItemList());
				ItemList.ResetCursor();
				while (ItemList.MoveCursorForward())
					{
					const CItem &Item(ItemList.GetItemAtCursor());

					if (!Item.IsInstalled() && !Item.IsDamaged())
						{
						bool bNew;
						ItemInfo *pEntry = pSystemEntry->Items.SetAt(Item.GetType()->GetUNID(), &bNew);
						if (bNew)
							{
							pEntry->pType = Item.GetType();
							pEntry->iTotalCount = Item.GetCount();
							}
						else
							pEntry->iTotalCount += Item.GetCount();
						}
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
		case outputByLevel:
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
		if (error = m_Universe.CreateStarSystem(pNode, &pSystem, &sError))
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

				ICCItemPtr pShipList(pObj->GetProperty(CCCtx, CONSTLIT("rpg.shipBrokerInventory")));
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


//	LootSim.cpp
//
//	Generate statistics about loot
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define NO_LOGO_SWITCH						CONSTLIT("nologo")

#define FIELD_NAME							CONSTLIT("shortName")

#define LOOT_TYPE_ASTEROIDS					CONSTLIT("asteroids")
#define LOOT_TYPE_ENEMIES					CONSTLIT("enemies")
#define LOOT_TYPE_INVENTORY					CONSTLIT("inventory")

enum ELootTypes
	{
	lootEnemies =			0x00000001,		//	Loot enemy ships and stations
	lootMining =			0x00000002,		//	Asteroid ore
	lootInventory =			0x00000004,		//	Inventory on friendly stations (for trade)
	};

struct SSystemInfo
	{
	CString sName;
	int iLevel;
	DWORD dwSystemType;
	int iCount;								//	Number of times this system instance 
											//	has appeared.

	CItemInfoTable Items;					//	All items types that have ever appeared in
											//	this system instance.

	int iTotalStations = 0;
	int iTotalLootValue = 0;
	int iTotalDeviceValue = 0;
	int iTotalArmorValue = 0;
	int iTotalOtherValue = 0;
	};

struct SSourceInfo
	{
	const CDesignType *pType = NULL;
	CString sName;
	int iLevel = 0;

	int iTotalCount = 0;
	CItemInfoTable Items;
	};

void AddItems (CSpaceObject &Obj, const CItemCriteria &Criteria, SSystemInfo *pSystemEntry);
void AddItems (CSpaceObject &Obj, const CItemCriteria &Criteria, TSortMap<DWORD, SSourceInfo> &AllSources);
void OutputItemsBySource (CUniverse &Universe, const TSortMap<DWORD, SSourceInfo> &AllSources, int iSystemSample);

void GenerateLootSim (CUniverse &Universe, CXMLElement *pCmdLine)
	{
	ALERROR error;
	CString sError;
	int i, j;
	CSovereign *pPlayer = Universe.FindSovereign(g_PlayerSovereignUNID);

	int iSystemSample = pCmdLine->GetAttributeIntegerBounded(CONSTLIT("count"), 1, -1, 1);
	bool bLogo = !pCmdLine->GetAttributeBool(NO_LOGO_SWITCH);

	//	Criteria

	CString sCriteria = pCmdLine->GetAttribute(CONSTLIT("criteria"));
	if (sCriteria.IsBlank())
		sCriteria = CONSTLIT("*");

	CSpaceObjectCriteria Criteria(sCriteria);

	CItemCriteria ItemCriteria;
	CString sItemCriteria = pCmdLine->GetAttribute(CONSTLIT("itemCriteria"));
	if (!sItemCriteria.IsBlank())
		CItem::ParseCriteria(sItemCriteria, &ItemCriteria);
	else
		CItem::InitCriteriaAll(&ItemCriteria);

	bool bBySource = pCmdLine->GetAttributeBool(CONSTLIT("bySource"));
	bool bByLevel = !bBySource;

	//	Figure out what we're looting

	DWORD dwLootType = 0;
	if (pCmdLine->GetAttributeBool(LOOT_TYPE_ENEMIES))
		dwLootType |= lootEnemies;

	if (pCmdLine->GetAttributeBool(LOOT_TYPE_ASTEROIDS))
		dwLootType |= lootMining;

	if (pCmdLine->GetAttributeBool(LOOT_TYPE_INVENTORY))
		dwLootType |= lootInventory;

	//	Default to enemies

	if (dwLootType == 0)
		dwLootType = lootEnemies;

	DWORD dwTotalTime = 0;

	//	Generate systems for multiple games

	TSortMap<CString, SSystemInfo> AllSystems;
	TSortMap<DWORD, SSourceInfo> AllSources;
	for (i = 0; i < iSystemSample; i++)
		{
		if (bLogo)
			printf("pass %d...\n", i+1);

		int iNode;
		for (iNode = 0; iNode < Universe.GetTopologyNodeCount(); iNode++)
			{
			CTopologyNode *pNode = Universe.GetTopologyNode(iNode);
			if (pNode == NULL || pNode->IsEndGame())
				continue;

			//	Create the system

			DWORD dwStartTime = ::GetTickCount();

			CSystem *pSystem;
			if (error = Universe.CreateStarSystem(pNode, &pSystem))
				{
				printf("ERROR: Unable to create star system.\n");
				return;
				}

			dwTotalTime += ::sysGetTicksElapsed(dwStartTime);

			//	Find this system in the table.

			bool bNew;
			SSystemInfo *pSystemEntry = AllSystems.SetAt(pNode->GetID(), &bNew);
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

			CSpaceObjectCriteria::SCtx Ctx(Criteria);
			for (j = 0; j < pSystem->GetObjectCount(); j++)
				{
				CSpaceObject *pObj = pSystem->GetObject(j);
				if (pObj == NULL
						|| !pObj->MatchesCriteria(Ctx, Criteria))
					continue;

				//	Asteroid mining

				if ((dwLootType & lootMining)
						&& pObj->GetScale() == scaleWorld
						&& pObj->GetItemList().GetCount() != 0)
					{
					pSystemEntry->iTotalStations++;
					AddItems(*pObj, ItemCriteria, pSystemEntry);
					AddItems(*pObj, ItemCriteria, AllSources);
					}

				//	Find any objects that are lootable by the player

				else if ((dwLootType & lootEnemies)
						&& pObj->GetSovereign()
						&& pObj->GetSovereign()->IsEnemy(pPlayer))
					{
					//	Destroy the object and get the loot from the wreckage

					CSpaceObject *pWreck;
					if (pObj->GetCategory() == CSpaceObject::catShip)
						pObj->Destroy(killedByDamage, CDamageSource(NULL, killedByDamage), NULL, &pWreck);
					else
						pWreck = pObj;

					//	Get the loot

					if (pWreck)
						{
						pSystemEntry->iTotalStations++;
						AddItems(*pWreck, ItemCriteria, pSystemEntry);
						AddItems(*pWreck, ItemCriteria, AllSources);
						}
					}

				//	Inventory

				else if ((dwLootType & lootInventory)
						&& pObj->GetSovereign()
						&& !pObj->GetSovereign()->IsEnemy(pPlayer)
						&& pObj->HasTradeService(serviceSell))
					{
					pSystemEntry->iTotalStations++;
					AddItems(*pObj, ItemCriteria, pSystemEntry);
					AddItems(*pObj, ItemCriteria, AllSources);
					}
				}

			//	Done with old system

			Universe.DestroySystem(pSystem);
			}

		//	Reinitialize

		Universe.Reinit();
		if (Universe.InitGame(0, &sError) != NOERROR)
			{
			printf("ERROR: %s\n", sError.GetASCIIZPointer());
			return;
			}
		}

	if (bLogo)
		printf("FINAL SYSTEM STATISTICS\n\n");

	MarkItemsKnown(Universe);

	//	Sort by level then system name

	if (bByLevel)
		{
		TSortMap<CString, int> Sorted;
		for (i = 0; i < AllSystems.GetCount(); i++)
			{
			Sorted.Insert(strPatternSubst(CONSTLIT("%04d-%s"), AllSystems[i].iLevel, AllSystems[i].sName), i);
			}

		//	Output total value stats

		printf("Level\tSystem\tObjects\tLoot\tDevices\tArmor\tTreasure\n");

		for (i = 0; i < Sorted.GetCount(); i++)
			{
			const SSystemInfo &SystemEntry = AllSystems[Sorted[i]];

			printf("%d\t%s\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
					SystemEntry.iLevel,
					SystemEntry.sName.GetASCIIZPointer(),
					(double)SystemEntry.iTotalStations / (double)iSystemSample,
					(double)SystemEntry.iTotalLootValue / (double)iSystemSample,
					(double)SystemEntry.iTotalDeviceValue / (double)iSystemSample,
					(double)SystemEntry.iTotalArmorValue / (double)iSystemSample,
					(double)SystemEntry.iTotalOtherValue / (double)iSystemSample
					);
			}

		printf("\n");

		//	Output all items

		printf("Level\tSystem\tItem\tCount\tValue\n");

		CItem NULL_ITEM;
		CItemCtx ItemCtx(NULL_ITEM);

		for (i = 0; i < Sorted.GetCount(); i++)
			{
			const SSystemInfo &SystemEntry = AllSystems[Sorted[i]];

			for (j = 0; j < SystemEntry.Items.GetCount(); j++)
				{
				const CItemType &ItemType = SystemEntry.Items.GetItemType(j);
				int iItemCount = SystemEntry.Items.GetItemCount(j);

				printf("%d\t%s\t%s\t%.2f\t%.2f\n",
						SystemEntry.iLevel,
						SystemEntry.sName.GetASCIIZPointer(),
						ItemType.GetDataField(FIELD_NAME).GetASCIIZPointer(),
						(double)iItemCount / (double)iSystemSample,
						(double)ItemType.GetValue(ItemCtx, true) * iItemCount / (double)iSystemSample);
				}
			}
		}

	//	Output all sources

	if (bBySource)
		OutputItemsBySource(Universe, AllSources, iSystemSample);

	printf("Average time to create systems: %.2f seconds.\n", (double)dwTotalTime / (1000.0 * iSystemSample));
	}

void AddItems (CSpaceObject &Obj, const CItemCriteria &Criteria, SSystemInfo *pSystemEntry)
	{
	CItemListManipulator ItemList(Obj.GetItemList());
	ItemList.ResetCursor();
	while (ItemList.MoveCursorForward())
		{
		const CItem &Item(ItemList.GetItemAtCursor());

		//	Skip items we're not interested in.

		if (!Item.MatchesCriteria(Criteria)
				|| Item.IsInstalled()
				|| Item.IsDamaged())
			continue;

		//	Add the item to the system list

		pSystemEntry->Items.AddItem(Item);

		//	Increment value

		int iValue = Item.GetTradePrice(NULL, true) * Item.GetCount();
		pSystemEntry->iTotalLootValue += iValue;

		if (Item.IsDevice())
			pSystemEntry->iTotalDeviceValue += iValue;
		else if (Item.IsArmor())
			pSystemEntry->iTotalArmorValue += iValue;
		else
			pSystemEntry->iTotalOtherValue += iValue;
		}
	}

void AddItems (CSpaceObject &Obj, const CItemCriteria &Criteria, TSortMap<DWORD, SSourceInfo> &AllSources)
	{
	bool bNew;
	SSourceInfo *pTable = AllSources.SetAt(Obj.GetType()->GetUNID(), &bNew);
	if (bNew)
		{
		pTable->pType = Obj.GetType();
		pTable->sName = pTable->pType->GetPropertyString(CONSTLIT("name"));
		pTable->iLevel = pTable->pType->GetLevel();
		}

	pTable->iTotalCount++;

	CItemListManipulator ItemList(Obj.GetItemList());
	ItemList.ResetCursor();
	while (ItemList.MoveCursorForward())
		{
		const CItem &Item(ItemList.GetItemAtCursor());

		//	Skip items we're not interested in.

		if (!Item.MatchesCriteria(Criteria)
				|| Item.IsInstalled()
				|| Item.IsDamaged())
			continue;

		//	Add the item to the table

		pTable->Items.AddItem(Item);
		}
	}

void OutputItemsBySource (CUniverse &Universe, const TSortMap<DWORD, SSourceInfo> &AllSources, int iSystemSample)
	{
	printf("Level\tSource\tItem\tTotal Count\tAve. Appearing\n");

	for (int i = 0; i < AllSources.GetCount(); i++)
		{
		const SSourceInfo &Source = AllSources[i];

		for (int j = 0; j < Source.Items.GetCount(); j++)
			{
			const CItemType &ItemType = Source.Items.GetItemType(j);
			int iItemCount = Source.Items.GetItemCount(j);

			printf("%d\t%s\t%s\t%.2f\t%.2f\n",
					Source.iLevel,
					(LPSTR)Source.sName,
					(LPSTR)ItemType.GetNounPhrase(),
					(double)iItemCount / (double)iSystemSample,
					(double)iItemCount / (double)Source.Items.GetInstanceCount(j));
			}
		}
	}

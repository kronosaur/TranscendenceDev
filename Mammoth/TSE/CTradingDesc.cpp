//	CTradingDesc.cpp
//
//	CTradingDesc class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ACCEPT_DONATION_TAG						CONSTLIT("AcceptDonation")
#define BUY_TAG									CONSTLIT("Buy")
#define INSTALL_ARMOR_TAG						CONSTLIT("InstallArmor")
#define INSTALL_DEVICE_TAG						CONSTLIT("InstallDevice")
#define REFUEL_TAG								CONSTLIT("Refuel")
#define REMOVE_DEVICE_TAG						CONSTLIT("RemoveDevice")
#define REPAIR_ARMOR_TAG						CONSTLIT("RepairArmor")
#define SELL_TAG								CONSTLIT("Sell")

#define ACTUAL_PRICE_ATTRIB						CONSTLIT("actualPrice")
#define CURRENCY_ATTRIB							CONSTLIT("currency")
#define CREDIT_CONVERSION_ATTRIB				CONSTLIT("creditConversion")
#define CRITERIA_ATTRIB							CONSTLIT("criteria")
#define IMPACT_ATTRIB							CONSTLIT("impact")
#define INVENTORY_ADJ_ATTRIB					CONSTLIT("inventoryAdj")
#define ITEM_ATTRIB								CONSTLIT("item")
#define LEVEL_FREQUENCY_ATTRIB					CONSTLIT("levelFrequency")
#define MAX_ATTRIB								CONSTLIT("max")
#define MESSAGE_ID_ATTRIB						CONSTLIT("messageID")
#define NO_DESCRIPTION_ATTRIB					CONSTLIT("noDescription")
#define PRICE_ADJ_ATTRIB						CONSTLIT("priceAdj")
#define REPLENISH_ATTRIB						CONSTLIT("replenish")
#define UPGRADE_INSTALL_ONLY_ATTRIB				CONSTLIT("upgradeInstallOnly")

#define CONSTANT_PREFIX							CONSTLIT("constant")
#define UNAVAILABLE_PREFIX						CONSTLIT("unavailable")
#define VARIABLE_PREFIX							CONSTLIT("variable")

struct SServiceData
	{
	const char *pszName;
	const char *pszIDPrefix;
	const char *pszTag;
	};

static SServiceData SERVICE_DATA[serviceCount] =
	{
		{	"(serviceNone)",			"",		""				},	//	serviceNone

		{	"priceOfferedToPlayer",		"B",	"Buy"			},	//	serviceBuy
		{	"priceForPlayerToBuy",		"S",	"Sell"			},	//	serviceSell
		{	"valueOfDonation",			"A",	"AcceptDonation"},	//	serviceAcceptDonations
		{	"priceToRefuel",			"F",	"Refuel"		},	//	serviceRefuel
		{	"priceToRepairArmor",		"Ra",	"RepairArmor"	},	//	serviceRepairArmor

		{	"priceToReplaceArmor",		"Ia",	"ReplaceArmor"	},	//	serviceReplaceArmor
		{	"priceToInstallDevice",		"Id",	"InstallDevice"	},	//	serviceInstallDevice
		{	"priceToRemoveDevice",		"Vd",	"RemoveDevice"	},	//	serviceRemoveDevice
		{	"priceToUpgradeDevice",		"Ud",	"UpgradeDevice"	},	//	serviceUpgradeDevice
		{	"priceToEnhanceItem",		"Ei",	"EnhanceItem"	},	//	serviceEnhanceItem

		{	"priceToRepairItem",		"Ri",	"RepairItem"	},	//	serviceRepairItem
		{	"customPrice",				"X",	"Custom"		},	//	serviceCustom
		{	"priceOfShipOffered",		"Bs",	"BuyShip"		},	//	serviceBuyShip
		{	"priceOfShipForSale",		"Ss",	"SellShip"		},	//	serviceSellShip
		{	"consumeTrade",				"Tc",	"ConsumeTrade"	},	//	serviceConsume

		{	"produceTrade",				"Tp",	"ProduceTrade"	},	//	serviceProduce
		{	"balanceTrade",				"Tb",	"BalanceTrade"	},	//	serviceTrade
		{	"decontaminate",			"Rr",	"Decontaminate"	},	//	serviceTrade
	};

static CurrencyValue MAX_BALANCE_CREDITS[MAX_SYSTEM_LEVEL + 1] =
	{
					  0,

				 10'000,		//	Level 1
				 20'000,
				 30'000,
				 50'000,
				100'000,		//	Level 5

				200'000,
				400'000,
				800'000,
			  1'500'000,
			  3'000'000,		//	Level 10

			  6'000'000,
			 12'000'000,
			 25'000'000,
			 50'000'000,
			100'000'000,		//	Level 15

			200'000'000,
			400'000'000,
			800'000'000,
		  1'500'000'000,
		  3'000'000'000,		//	Level 20

		  6'000'000'000,
		 12'000'000'000,
		 25'000'000'000,
		 50'000'000'000,
		100'000'000'000,		//	Level 25
	};

CTradingDesc::CTradingDesc (void)

//	CTradingDesc constructor

	{
	}

CTradingDesc::~CTradingDesc (void)

//	CTradingDesc destructor

	{
	}

bool CTradingDesc::AccumulateServiceDesc (const ICCItem &Value, TArray<SServiceDesc> &retServices)

//	AccumulateServiceDesc
//
//	Parses the value (expecting a struct) and adds it to the services array.

	{
	const ICCItem *pValue = Value.GetElement(CONSTLIT("service"));
	if (!pValue)
		return false;

	ETradeServiceTypes iService = CTradingDesc::ParseService(pValue->GetStringValue());
	if (iService == serviceNone)
		return false;

	CString sCriteria = Value.GetStringAt(CONSTLIT("criteria"));

	SDesignLoadCtx LoadCtx;
	CFormulaText PriceAdj;
	if (PriceAdj.InitFromString(LoadCtx, Value.GetStringAt(CONSTLIT("priceAdj"))) != NOERROR)
		return false;

	DWORD dwFlags = 0;

	CString sDesc = Value.GetStringAt(CONSTLIT("desc"));
	CString sDescID = Value.GetStringAt(CONSTLIT("descID"));
	if (sDescID.IsBlank())
		sDescID = Value.GetStringAt(CONSTLIT("messageID"));

	//	Add the service.

	SServiceDesc *pNewService = retServices.Insert();
	pNewService->iService = iService;
	pNewService->ItemCriteria.Init(sCriteria);
	pNewService->PriceAdj = PriceAdj;
	pNewService->dwFlags = dwFlags;

	if (!sDesc.IsBlank())
		{
		pNewService->dwFlags |= FLAG_MESSAGE_TEXT;
		pNewService->sMessageID = sDesc;
		}
	else if (!sDescID.IsBlank())
		pNewService->sMessageID = sDescID;

	return true;
	}

void CTradingDesc::AddDynamicServices (const CTradingDesc &Trade)

//	AddDynamicServices
//
//	Adds dynamic services.

	{
	for (int i = 0; i < Trade.m_List.GetCount(); i++)
		{
		const SServiceDesc &NewService = Trade.m_List[i];

		//	We always add dynamic services at the front.

		m_List.Insert(NewService, i);
		
		//	Mark it as dynamic

		m_List[i].dwFlags |= FLAG_DYNAMIC_SERVICE;
		}
	}

void CTradingDesc::AddOrder (ETradeServiceTypes iService, const CString &sCriteria, CItemType *pItemType, int iPriceAdj)

//	AddOrder
//
//	Adds a new trade line

	{
	if (iService == serviceNone)
		return;

	//	We always add at the beginning (because new orders take precedence)

	SServiceDesc Commodity;
	Commodity.iService = iService;
	Commodity.pItemType = pItemType;
	if (pItemType == NULL)
		Commodity.ItemCriteria.Init(sCriteria);
	Commodity.PriceAdj.SetInteger(iPriceAdj);
	Commodity.dwFlags = 0;
	Commodity.sID = ComputeID(iService, Commodity.pItemType.GetUNID(), sCriteria, Commodity.dwFlags);

	//	If we find a previous order for the same criteria, then delete it

	int iDuplicate;
	if (FindServiceToOverride(Commodity, &iDuplicate))
		m_List.Delete(iDuplicate);

	//	Add it at the beginning of the list.

	m_List.Insert(Commodity, 0);
	}

void CTradingDesc::AddOrders (const CTradingDesc &Trade)

//	AddOrders
//
//	Adds the given orders to our block.

	{
	int i;

	//	First delete any existing orders that we overwrite.

	for (i = 0; i < Trade.m_List.GetCount(); i++)
		{
		int iDuplicate;

		if (FindServiceToOverride(Trade.m_List[i], &iDuplicate))
			m_List.Delete(iDuplicate);
		}

	//	Now add this block at the beginning

	m_List.InsertEmpty(Trade.m_List.GetCount(), 0);

	for (i = 0; i < Trade.m_List.GetCount(); i++)
		m_List[i] = Trade.m_List[i];
	}

void CTradingDesc::AddOrder (CItemType *pItemType, const CString &sCriteria, int iPriceAdj, DWORD dwFlags)

//	AddOrder
//
//	Add a new commodity line

	{
	int i;

	//	Convert to new method

	ETradeServiceTypes iService;
	DWORD dwNewFlags;
	ReadServiceFromFlags(dwFlags, &iService, &dwNewFlags);
	if (iService == serviceNone)
		return;

	//	We always add at the beginning (because new orders take precedence)

	SServiceDesc *pCommodity = m_List.InsertAt(0);
	pCommodity->iService = iService;
	pCommodity->pItemType = pItemType;
	if (pItemType == NULL)
		pCommodity->ItemCriteria.Init(sCriteria);
	pCommodity->PriceAdj.SetInteger(iPriceAdj);
	pCommodity->dwFlags = dwNewFlags;
	pCommodity->sID = ComputeID(iService, pCommodity->pItemType.GetUNID(), sCriteria, dwNewFlags);

	//	If we find a previous order for the same criteria, then delete it

	for (i = 1; i < m_List.GetCount(); i++)
		if (strEquals(pCommodity->sID, m_List[i].sID))
			{
			m_List.Delete(i);
			break;
			}
	}

int CTradingDesc::AdjustForSystemPrice (STradeServiceCtx &Ctx, int iPrice)

//	AdjustForSystemPrice
//
//	Returns a price adjusted by the system trading table.

	{
	if (iPrice <= 0)
		return iPrice;

	const CTopologyNode *pNode = Ctx.pNode;
	if (pNode == NULL)
		{
		if (Ctx.pProvider == NULL)
			return iPrice;

		CSystem *pSystem = Ctx.pProvider->GetSystem();
		if (pSystem == NULL)
			return iPrice;

		pNode = pSystem->GetTopology();
		if (pNode == NULL)
			return iPrice;
		}

	int iAdj;
	if (!pNode->GetTradingEconomy().FindPriceAdj(Ctx.pItem->GetType(), &iAdj))
		return iPrice;

	return Max(1, iPrice * iAdj / 100);
	}

ICCItemPtr CTradingDesc::AsCCItem (CCodeChainCtx &CCX) const

//	AsCCItem
//
//	Returns a struct of services

	{
	//	We start by sorting everything by service type

	TSortMap<CString, TArray<const SServiceDesc *>> Sorted;
	for (int i = 0; i < m_List.GetCount(); i++)
		{
		const SServiceDesc &Service = m_List[i];

		auto pList = Sorted.SetAt(ServiceToString(Service.iService));
		pList->Insert(&Service);
		}

	//	Create a structure and output entries

	ICCItemPtr pResult(ICCItem::SymbolTable);
	for (int i = 0; i < Sorted.GetCount(); i++)
		{
		const auto &Services = Sorted[i];

		ICCItemPtr pList(ICCItem::List);
		for (int j = 0; j < Services.GetCount(); j++)
			{
			const auto pService = Services[j];

			ICCItemPtr pEntry(ICCItem::SymbolTable);
			pEntry->SetStringAt(CONSTLIT("type"), Sorted.GetKey(i));
			pEntry->SetStringAt(CONSTLIT("id"), pService->sID);

			pList->Append(pEntry);
			}

		pResult->SetAt(Sorted.GetKey(i), pList);
		}

	return pResult;
	}

ICCItemPtr CTradingDesc::AsCCItem (const SServiceStatus &Status)

//	AsCCItem
//
//	Encodes in a CCItem struct.

	{
	ICCItemPtr pResult = ICCItemPtr(ICCItem::SymbolTable);
	pResult->SetBooleanAt(CONSTLIT("available"), Status.bAvailable);
	if (Status.bAvailable
			&& (Status.dwPriceFlags & PRICE_UPGRADE_INSTALL_ONLY))
		pResult->SetBooleanAt(CONSTLIT("upgradeInstallOnly"), true);

	if (Status.iMaxLevel != -1)
		pResult->SetIntegerAt(CONSTLIT("maxLevel"), Status.iMaxLevel);

	//	NOTE: Message is valid even if we cannot install

	if (!Status.Message.sDesc.IsBlank())
		pResult->SetStringAt(CONSTLIT("desc"), Status.Message.sDesc);

	else if (!Status.Message.sDescID.IsBlank())
		pResult->SetStringAt(CONSTLIT("descID"), Status.Message.sDescID);

	return pResult;
	}

CurrencyValue CTradingDesc::CalcMaxBalance (int iLevel, const CEconomyType *pCurrency)

//	CalcMaxBalance
//
//	Compute the max balance for a station of the given level (in the given
//	currency).

	{
	ASSERT(iLevel >= 1 && iLevel <= MAX_SYSTEM_LEVEL);

	CurrencyValue iMaxBalanceCredits = MAX_BALANCE_CREDITS[Min(Max(0, iLevel), MAX_SYSTEM_LEVEL)];
	if (pCurrency)
		return pCurrency->Exchange(NULL, iMaxBalanceCredits);
	else
		return iMaxBalanceCredits;
	}

CurrencyValue CTradingDesc::CalcMaxBalance (CSpaceObject *pObj, CurrencyValue *retReplenish) const

//	ComputeMaxCurrency
//
//	Computes max balance

	{
	CurrencyValue iMaxBalance = GetMaxBalance(pObj->GetLevel());

	if (retReplenish)
		{
		if (m_iReplenishCurrency)
			*retReplenish = m_iReplenishCurrency;
		else
			*retReplenish = iMaxBalance / 20;
		}

	return iMaxBalance * (90 + ((pObj->GetDestiny() + 9) / 18)) / 100;
	}

int CTradingDesc::CalcPriceForService (ETradeServiceTypes iService, const CSpaceObject *pProvider, const CItem &Item, int iCount, DWORD dwFlags)

//	CalcPriceForService
//
//	Compute a price for a service (used by objects without a Trade descriptor).

	{
	SServiceDesc Default;
	Default.iService = iService;
	Default.PriceAdj.InitFromInteger(100);
	Default.dwFlags = 0;

	STradeServiceCtx Ctx;
	Ctx.iService = iService;
	Ctx.pProvider = pProvider;
	Ctx.pCurrency = pProvider->GetDefaultEconomy();
	Ctx.pItem = &Item;
	Ctx.iCount = iCount;

	return ComputePrice(Ctx, Default, dwFlags);
	}

bool CTradingDesc::ComposeDescription (CUniverse &Universe, CString *retsDesc) const

//  ComposeDescription
//
//  Composes a description of the trading desc, suitable for map description.
//  We return FALSE if we have no relevant services.

	{
	CString sDesc;

	//  Short-circuit

	if (!HasServices())
		return false;

	//  Buying and selling ships

	SServiceTypeInfo BuyShips;
	GetServiceTypeInfo(Universe, serviceBuyShip, BuyShips);

	SServiceTypeInfo SellShips;
	GetServiceTypeInfo(Universe, serviceSellShip, SellShips);

	CString sBuySellShips;
	if (BuyShips.bAvailable && SellShips.bAvailable)
		sDesc = CONSTLIT("Buys and sells ships");
	else if (BuyShips.bAvailable)
		sDesc = CONSTLIT("Buys ships");
	else if (BuyShips.bAvailable)
		sDesc = CONSTLIT("Sells ships");

	//  Refuels

	SServiceTypeInfo Refuel;
	if (GetServiceTypeInfo(Universe, serviceRefuel, Refuel))
		{
		CString sText = strPatternSubst(CONSTLIT("Refuels up to level %d"), Refuel.iMaxLevel);

		if (!sDesc.IsBlank())
			sDesc = strPatternSubst(CONSTLIT("%s %&mdash; %s"), sDesc, sText);
		else
			sDesc = sText;
		}

	//  Repair armor

	SServiceTypeInfo RepairArmor;
	GetServiceTypeInfo(Universe, serviceRepairArmor, RepairArmor);

	SServiceTypeInfo InstallArmor;
	GetServiceTypeInfo(Universe, serviceReplaceArmor, InstallArmor);

	if (RepairArmor.iMaxLevel != -1 || InstallArmor.iMaxLevel != -1)
		{
		CString sPurchased = (InstallArmor.bUpdateInstallOnly ? CONSTLIT("purchased ") : NULL_STR);

		CString sText;
		if (RepairArmor.iMaxLevel == InstallArmor.iMaxLevel)
			sText = strPatternSubst(CONSTLIT("Repairs/installs %sarmor up to level %d"), sPurchased, RepairArmor.iMaxLevel);
		else if (RepairArmor.iMaxLevel != -1 && InstallArmor.iMaxLevel != -1)
			sText = strPatternSubst(CONSTLIT("Repairs armor up to level %d %&mdash; Installs %sarmor up to level %d"), RepairArmor.iMaxLevel, sPurchased, InstallArmor.iMaxLevel);
		else if (RepairArmor.iMaxLevel != -1)
			sText = strPatternSubst(CONSTLIT("Repairs armor up to level %d"), RepairArmor.iMaxLevel);
		else
			sText = strPatternSubst(CONSTLIT("Installs %sarmor up to level %d"), sPurchased, InstallArmor.iMaxLevel);

		if (!sDesc.IsBlank())
			sDesc = strPatternSubst(CONSTLIT("%s %&mdash; %s"), sDesc, sText);
		else
			sDesc = sText;
		}

	//  Install devices

	SServiceTypeInfo InstallDevice;
	GetServiceTypeInfo(Universe, serviceInstallDevice, InstallDevice);

	if (InstallDevice.iMaxLevel != -1)
		{
		CString sPurchased = (InstallDevice.bUpdateInstallOnly ? CONSTLIT("purchased ") : NULL_STR);
		CString sText = strPatternSubst(CONSTLIT("Installs %sdevices up to level %d"), sPurchased, InstallDevice.iMaxLevel);

		if (!sDesc.IsBlank())
			sDesc = strPatternSubst(CONSTLIT("%s %&mdash; %s"), sDesc, sText);
		else
			sDesc = sText;
		}

	SServiceTypeInfo Buys;
	GetServiceTypeInfo(Universe, serviceBuy, Buys);

	SServiceTypeInfo Sells;
	GetServiceTypeInfo(Universe, serviceSell, Sells);

	if (Buys.bAvailable || Sells.bAvailable)
		{
		CString sText;
		if (Buys.bAvailable && Sells.bAvailable)
			sText = CONSTLIT("Buys and sells commodities");
		else if (Buys.bAvailable)
			sText = CONSTLIT("Buys commodities");
		else
			sText = CONSTLIT("Sells commodities");

		if (!sDesc.IsBlank())
			sDesc = strPatternSubst(CONSTLIT("%s %&mdash; %s"), sDesc, sText);
		else
			sDesc = sText;
		}

	//  If we don't have any trade descriptor, then we're done

	if (sDesc.IsBlank())
		return false;

	//  Return it

	if (retsDesc)
		*retsDesc = sDesc;

	return true;
	}

CCurrencyAndValue CTradingDesc::ComputeDecontaminationBasePrice (STradeServiceCtx &Ctx)

//	ComputeDecontaminationBasePrice
//
//	Computes the base price for decontaminating an object.

	{
	if (Ctx.pObj)
		{
		auto &Armor = Ctx.pObj->GetArmorSystem();

		const int iLevel = Armor.GetMaxLevel();
		if (iLevel == -1)
			return CCurrencyAndValue();

		const int iLevelRepairCostPerHP = CArmorClass::GetStdStats(iLevel).iRepairCost;
		const int iTotalCost = iLevelRepairCostPerHP * DECON_COST_FACTOR * Armor.GetSegmentCount();

		return CCurrencyAndValue(iTotalCost, &Ctx.pObj->GetUniverse().GetCreditCurrency());
		}
	else if (Ctx.pType)
		{
		const CShipClass *pClass = CShipClass::AsType(Ctx.pType);
		auto &Armor = pClass->GetArmorDesc();

		const int iLevel = Armor.GetMaxLevel();
		if (iLevel == -1)
			return CCurrencyAndValue();

		const int iLevelRepairCostPerHP = CArmorClass::GetStdStats(iLevel).iRepairCost;
		const int iTotalCost = iLevelRepairCostPerHP * DECON_COST_FACTOR * Armor.GetCount();

		return CCurrencyAndValue(iTotalCost, &Ctx.pType->GetUniverse().GetCreditCurrency());
		}
	else
		return CCurrencyAndValue();
	}

CString CTradingDesc::ComputeID (ETradeServiceTypes iService, DWORD dwUNID, const CString &sCriteria, DWORD dwFlags)

//	ComputeID
//
//	Generates a string ID for the order. Two identical orders should have the same ID

	{
	CString sService = ((iService >= 0 && iService < serviceCount) ? CString(SERVICE_DATA[iService].pszIDPrefix) : CONSTLIT("?"));

	if (dwUNID)
		return strPatternSubst(CONSTLIT("%s:%x"), sService, dwUNID);
	else
		return strPatternSubst(CONSTLIT("%s:%s"), sService, sCriteria);
	}

int CTradingDesc::ComputePrice (STradeServiceCtx &Ctx, DWORD dwFlags) const

//	ComputePrice
//
//	Computes the price

	{
	//	For ships we match by object

	const SServiceDesc *pDesc;
	switch (Ctx.iService)
		{
		case serviceBuyShip:
		case serviceDecontaminate:
		case serviceSellShip:
			{
			if (Ctx.pObj)
				{
				if (!FindService(Ctx.iService, Ctx.pObj->GetType(), &pDesc))
					return -1;
				}
			else if (Ctx.pType)
				{
				if (!FindService(Ctx.iService, Ctx.pType, &pDesc))
					return -1;
				}
			else
				{
				ASSERT(false);
				return -1;
				}

			break;
			}

		default:
			{
			if (!Ctx.pItem)
				throw CException(ERR_FAIL);

			if (!FindService(Ctx.iService, *Ctx.pItem, &pDesc))
				return -1;

			break;
			}
		}

	//	Now we can compute the price

	return ComputePrice(Ctx, *pDesc, dwFlags);
	}

int CTradingDesc::ComputePrice (STradeServiceCtx &Ctx, const SServiceDesc &Commodity, DWORD dwFlags)

//	ComputePrice
//
//	Computes the price of the item in the proper currency

	{
	bool bActual = ((Commodity.dwFlags & FLAG_ACTUAL_PRICE ? true : false) || ((dwFlags & FLAG_CHARGE_ACTUAL_PRICE) ? true : false));
	bool bPlayerAdj = !(dwFlags & FLAG_NO_PLAYER_ADJ);

	//	Get the raw price from the item

	int iBasePrice;
	const CEconomyType *pBaseEconomy;
	switch (Commodity.iService)
		{
		case serviceRepairArmor:
			{
			const CArmorItem ArmorItem = Ctx.pItem->AsArmorItem();
			if (!ArmorItem)
				return -1;

			iBasePrice = (int)ArmorItem.GetRepairCost(Ctx.iCount);
			pBaseEconomy = Ctx.pItem->GetCurrencyType();
			break;
			}

		case serviceInstallDevice:
		case serviceReplaceArmor:
			{
			iBasePrice = Ctx.iCount * Ctx.pItem->GetInstallCost();
			pBaseEconomy = Ctx.pItem->GetCurrencyType();
			break;
			}

		case serviceRemoveDevice:
			{
			iBasePrice = Ctx.iCount * Ctx.pItem->GetInstallCost() / 2;
			pBaseEconomy = Ctx.pItem->GetCurrencyType();
			break;
			}

		case serviceBuyShip:
		case serviceSellShip:
			{
			CCurrencyAndValue Value;
			if (Ctx.pObj)
				Value = Ctx.pObj->GetTradePrice(Ctx.pProvider);
			else if (Ctx.pType)
				Value = Ctx.pType->GetTradePrice(Ctx.pProvider);
			else
				{
				ASSERT(false);
				return -1;
				}

			iBasePrice = (int)Value.GetValue();
			pBaseEconomy = Value.GetCurrencyType();
			break;
			}

		case serviceBuy:
			{
			//	If this item cannot be sold when used, then check if it is used.

			if (!Ctx.pItem->GetType()->CanBeSoldIfUsed()
					&& Ctx.pItem->IsUsed())
				return -1;

			//	Otherwise, normal price

			iBasePrice = Ctx.iCount * Ctx.pItem->GetTradePrice(Ctx.pProvider, bActual);
			pBaseEconomy = Ctx.pItem->GetCurrencyType();

			//	Adjust for system trade prices

			iBasePrice = AdjustForSystemPrice(Ctx, iBasePrice);
			break;
			}

		case serviceRefuel:
		case serviceSell:
			{
			iBasePrice = Ctx.iCount * Ctx.pItem->GetTradePrice(Ctx.pProvider, bActual);
			pBaseEconomy = Ctx.pItem->GetCurrencyType();
			iBasePrice = AdjustForSystemPrice(Ctx, iBasePrice);
			break;
			}

		case serviceDecontaminate:
			{
			CCurrencyAndValue Value = ComputeDecontaminationBasePrice(Ctx);
			if (Value.IsEmpty())
				return -1;

			iBasePrice = (int)Value.GetValue();
			pBaseEconomy = Value.GetCurrencyType();
			break;
			}

		default:
			iBasePrice = Ctx.iCount * Ctx.pItem->GetTradePrice(Ctx.pProvider, bActual);
			pBaseEconomy = Ctx.pItem->GetCurrencyType();
			break;
		}

	//	Adjust the price appropriately

	bool bEventAdj = false;
	CurrencyValue iPrice;
	CString sPrefix;
	int iPriceAdj = Commodity.PriceAdj.EvalAsInteger(Ctx.pProvider, &sPrefix);

	if (sPrefix.IsBlank())
		iPrice = iPriceAdj * iBasePrice / 100;
	else if (strEquals(sPrefix, CONSTANT_PREFIX))
		iPrice = iPriceAdj;
	else if (strEquals(sPrefix, UNAVAILABLE_PREFIX))
		return -1;
	else if (strEquals(sPrefix, VARIABLE_PREFIX))
		{
		iPrice = iBasePrice;
		bEventAdj = true;
		}
	else
		{
		kernelDebugLogPattern("Unknown priceAdj prefix: %s", sPrefix);
		return -1;
		}

	//	Let the provide adjust prices, if it has an event

	int iPlayerPriceAdj;
	if (bPlayerAdj 
			&& bEventAdj
			&& Ctx.pProvider
			&& Ctx.pProvider->FireGetPlayerPriceAdj(Ctx, NULL, &iPlayerPriceAdj))
		{
		if (iPlayerPriceAdj < 0)
			return -1;

		iPrice = iPlayerPriceAdj * iPrice / 100;
		}

	//	Let global types adjust the price

	if (bPlayerAdj 
			&& g_pUniverse->GetDesignCollection().FireGetGlobalPlayerPriceAdj(Ctx, NULL, &iPlayerPriceAdj))
		{
		if (iPlayerPriceAdj < 0)
			return -1;

		iPrice = iPlayerPriceAdj * iPrice / 100;
		}

	//	Convert to proper currency

	return (int)Ctx.pCurrency->Exchange(pBaseEconomy, iPrice);
	}

ALERROR CTradingDesc::CreateFromXML (SDesignLoadCtx &Ctx, const CXMLElement *pDesc, CTradingDesc **retpTrade)

//	InitFromXML
//
//	Initialize from an XML element

	{
	ALERROR error;
	int i, j;

	//	Allocate the trade structure

	CTradingDesc *pTrade = new CTradingDesc;
	pTrade->m_pCurrency.LoadUNID(pDesc->GetAttribute(CURRENCY_ATTRIB));
	pTrade->m_iMaxCurrency = pDesc->GetAttributeIntegerBounded(MAX_ATTRIB, 0, -1, 0);
	pTrade->m_iReplenishCurrency = pDesc->GetAttributeIntegerBounded(REPLENISH_ATTRIB, 0, -1, 0);

	//	Allocate the array

	int iCount = pDesc->GetContentElementCount();
	if (iCount)
		{
		pTrade->m_List.InsertEmpty(iCount);

		//	Load

		for (i = 0; i < iCount; i++)
			{
			CXMLElement *pLine = pDesc->GetContentElement(i);
			SServiceDesc *pCommodity = &pTrade->m_List[i];

			//	Service

			pCommodity->iService = serviceNone;
			for (j = 0; j < serviceCount; j++)
				{
				if (strEquals(pLine->GetTag(), CString(SERVICE_DATA[j].pszTag, -1, true)))
					{
					pCommodity->iService = (ETradeServiceTypes)j;
					break;
					}
				}

			if (pCommodity->iService == serviceNone)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Unknown Trade directive: %s."), pLine->GetTag());
				return ERR_FAIL;
				}

			//	Get the criteria. Depending on the service this might be either
			//	an item criteria or a type criteria.

			CString sCriteria = pLine->GetAttribute(CRITERIA_ATTRIB);

			//	Depending on the service we get a slightly different set of 
			//	parameters.
			//
			//	For selling ships, we need the type criteria.

			switch (pCommodity->iService)
				{
				case serviceBuyShip:
				case serviceDecontaminate:
				case serviceSellShip:
					{
					if (error = CDesignTypeCriteria::ParseCriteria(sCriteria, &pCommodity->TypeCriteria))
						{
						Ctx.sError = strPatternSubst(CONSTLIT("Unable to parse criteria: %s."), sCriteria);
						return ERR_FAIL;
						}
					break;
					}

				default:
					{
					pCommodity->ItemCriteria.Init(sCriteria, CItemCriteria::ALL);

					//	Item

					if (error = pCommodity->pItemType.LoadUNID(Ctx, pLine->GetAttribute(ITEM_ATTRIB)))
						return error;

					//	Some items are replenished over time.

					if (error = pCommodity->InventoryAdj.InitFromString(Ctx, pLine->GetAttribute(INVENTORY_ADJ_ATTRIB)))
						return error;

					break;
					}
				}

			//	Level frequency

			pCommodity->sLevelFrequency = pLine->GetAttribute(LEVEL_FREQUENCY_ATTRIB);

			//	Price

			switch (pCommodity->iService)
				{
				case serviceConsume:
				case serviceProduce:
				case serviceTrade:
					if (error = pCommodity->PriceAdj.InitFromString(Ctx, pLine->GetAttribute(IMPACT_ATTRIB)))
						return error;
					break;

				default:
					if (error = pCommodity->PriceAdj.InitFromString(Ctx, pLine->GetAttribute(PRICE_ADJ_ATTRIB)))
						return error;
					break;
				}

			//	Message ID

			pCommodity->sMessageID = pLine->GetAttribute(MESSAGE_ID_ATTRIB);

			//	Flags

			pCommodity->dwFlags = 0;
			if (pLine->GetAttributeBool(ACTUAL_PRICE_ATTRIB))
				pCommodity->dwFlags |= FLAG_ACTUAL_PRICE;

			if (!pCommodity->InventoryAdj.IsEmpty())
				pCommodity->dwFlags |= FLAG_INVENTORY_ADJ;

			if (pLine->GetAttributeBool(UPGRADE_INSTALL_ONLY_ATTRIB))
				pCommodity->dwFlags |= FLAG_UPGRADE_INSTALL_ONLY;

			if (pLine->GetAttributeBool(NO_DESCRIPTION_ATTRIB))
				pCommodity->dwFlags |= FLAG_NO_DESCRIPTION;

			//	Set ID

			pCommodity->sID = pTrade->ComputeID(pCommodity->iService, pCommodity->pItemType.GetUNID(), sCriteria, pCommodity->dwFlags);
			}
		}

	//	Done

	*retpTrade = pTrade;

	return NOERROR;
	}

bool CTradingDesc::Buys (STradeServiceCtx &Ctx, const CItem &Item, DWORD dwFlags, int *retiPrice, int *retiMaxCount)

//	Buys
//
//	Returns TRUE if the given object buys items of the given type.
//	Optionally returns a price and a max number.
//
//	Note that we always return a price for items we are willing to buy, even if we
//	don't currently have enough to buy it.

	{
	int i;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (((m_List[i].iService == serviceBuy) || (m_List[i].iService == serviceAcceptDonations))
				&& Matches(Item, m_List[i]))
			{
			//	If this is virtual price and we don't want virtual, then skip

			if ((m_List[i].iService == serviceAcceptDonations)
					&& (dwFlags & FLAG_NO_DONATION))
				return false;

			//	Set the service

			Ctx.iService = m_List[i].iService;

			//	Compute price

			int iPrice = ComputePrice(Ctx, m_List[i], dwFlags);
			if (iPrice < 0)
				return false;

			//	Compute the maximum number of this item that we are willing
			//	to buy. First we figure out how much money the station has left

			int iBalance = (Ctx.pProvider ? (int)Ctx.pProvider->GetBalance(m_pCurrency->GetUNID()) : 0);
			int iMaxCount = (iPrice > 0 ? (iBalance / iPrice) : 0);

			//	Done

			if (retiMaxCount)
				*retiMaxCount = iMaxCount;

			if (retiPrice)
				*retiPrice = iPrice;

			return true;
			}

	return false;
	}

bool CTradingDesc::Buys (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, int *retiMaxCount)

//	Buys
//
//	Returns TRUE if the given object buys items of the given type.
//	Optionally returns a price and a max number.
//
//	Note that we always return a price for items we are willing to buy, even if we
//	don't currently have enough to buy it.

	{
	STradeServiceCtx Ctx;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pItem = &Item;
	Ctx.iCount = 1;

	return Buys(Ctx, Item, dwFlags, retiPrice, retiMaxCount);
	}

bool CTradingDesc::BuysShip (CSpaceObject *pObj, CShipClass *pClass, DWORD dwFlags, int *retiPrice)

//	BuysShip
//
//	Returns TRUE if the given object can buys the given ship. Optionally returns
//	a price.

	{
	STradeServiceCtx Ctx;
	Ctx.iService = serviceBuyShip;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pType = pClass;
	Ctx.iCount = 1;

	//	Compute price

	int iPrice = ComputePrice(Ctx, dwFlags);
	if (iPrice <= 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

bool CTradingDesc::BuysShip (CSpaceObject *pObj, CSpaceObject *pShip, DWORD dwFlags, int *retiPrice)

//	BuysShip
//
//	Returns TRUE if the given object can buys the given ship. Optionally returns
//	a price.

	{
	STradeServiceCtx Ctx;
	Ctx.iService = serviceBuyShip;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pObj = pShip;
	Ctx.iCount = 1;

	//	Compute price

	int iPrice = ComputePrice(Ctx, dwFlags);
	if (iPrice <= 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

int CTradingDesc::Charge (CSpaceObject *pObj, int iCharge)

//	Charge
//
//	Charge out of the station's balance

	{
	return (int)pObj->ChargeMoney(m_pCurrency->GetUNID(), iCharge);
	}

void CTradingDesc::DeleteDynamicServices ()

//	DeleteDynamicServices
//
//	Delete all dynamic services.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].dwFlags & FLAG_DYNAMIC_SERVICE)
			{
			m_List.Delete(i);
			i--;
			}
	}

bool CTradingDesc::FindByID (const CString &sID, int *retiIndex) const

//	FindByID
//
//	Finds an entry by ID

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		if (strEquals(sID, m_List[i].sID))
			{
			if (retiIndex)
				*retiIndex = i;

			return true;
			}

	return false;
	}

bool CTradingDesc::FindService (ETradeServiceTypes iService, const CItem &Item, const SServiceDesc **retpDesc) const

//	FindService
//
//	Finds the given service for the given item.

	{
	//	Loop over the commodity list and find the first entry that matches

	for (int i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService 
				&& Matches(Item, m_List[i]))
			{
			if (retpDesc)
				*retpDesc = &m_List[i];

			return true;
			}
	
	return false;
	}

bool CTradingDesc::FindService (ETradeServiceTypes iService, const CDesignType *pType, const SServiceDesc **retpDesc) const

//	FindService
//
//	Finds the given service for the given item.

	{
	//	Loop over the commodity list and find the first entry that matches

	for (int i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService 
				&& Matches(pType, m_List[i]))
			{
			if (retpDesc)
				*retpDesc = &m_List[i];

			return true;
			}
	
	return false;
	}

bool CTradingDesc::FindServiceToOverride (const SServiceDesc &NewService, int *retiIndex) const

//	FindServiceToOverride
//
//	Finds a service in the current list that would be overridden by the given
//	new service.

	{
	int i;

	for (i = 0; i < m_List.GetCount(); i++)
		{
		const SServiceDesc &Service = m_List[i];

		switch (NewService.iService)
			{
			case serviceConsume:
			case serviceProduce:
			case serviceTrade:
				{
				if ((Service.iService == serviceConsume
							|| Service.iService == serviceProduce
							|| Service.iService == serviceTrade)
						&& HasSameCriteria(Service, NewService))
					{
					if (retiIndex)
						*retiIndex = i;

					return true;
					}

				break;
				}

			default:
				{
				if (Service.iService == NewService.iService
						&& HasSameCriteria(Service, NewService))
					{
					if (retiIndex)
						*retiIndex = i;

					return true;
					}
				break;
				}
			}
		}

	return false;
	}

bool CTradingDesc::InitFromGetTradeServices (const CSpaceObject &ProviderObj)

//	InitFromGetTradeServices
//
//	Initialize from a call to <GetTradeServices>. Returns TRUE if any services 
//	were returned.

	{
	m_List.DeleteAll();

	FireGetTradeServices(ProviderObj, m_List);
	return (m_List.GetCount() > 0);
	}

bool CTradingDesc::FireGetTradeServices (const CSpaceObject &ProviderObj, TArray<SServiceDesc> &retServices)

//	FireGetTradeServices
//
//	Allows the provider to provide dynamic services. We use this to implement 
//	the auton bay, which provides installation services.

	{
	bool bHasServices = false;

	//	Loop over all installed devices on the provider and see if they have
	//	services.

	const CItemList &ItemList = ProviderObj.GetItemList();
	for (int i = 0; i < ItemList.GetCount(); i++)
		{
		const CItem &Item = ItemList.GetItem(i);
		if (!Item.IsInstalled() || !Item.IsDevice())
			continue;

		if (FireGetTradeServices(ProviderObj, Item, retServices))
			bHasServices = true;
		}

	return bHasServices;
	}

bool CTradingDesc::FireGetTradeServices (const CSpaceObject &ProviderObj, const CItem &ProviderItem, TArray<SServiceDesc> &retServices)

//	FireGetTradeServices
//
//	Allows an item to specify trader services (e.g., Auton Bay).

	{
	//	Fire event. If no results, then no services.

	ICCItemPtr pResult = ProviderItem.FireGetTradeServices(ProviderObj);
	if (!pResult || pResult->IsNil())
		return false;

	//	Otherwise, we parse the services.

	if (pResult->IsList())
		{
		for (int i = 0; i < pResult->GetCount(); i++)
			{
			if (!AccumulateServiceDesc(*pResult->GetElement(i), retServices))
				return false;
			}

		return true;
		}
	else if (pResult->IsSymbolTable())
		{
		if (!AccumulateServiceDesc(*pResult, retServices))
			return false;

		return true;
		}
	else
		{
		ProviderObj.GetUniverse().LogOutput(CONSTLIT("GetTradeService: Invalid result."));
		return false;
		}
	}

bool CTradingDesc::HasConsumerService (void) const

//	HasConsumerService
//
//	Returns TRUE if we have at least one service on which the player can spend
//	money. Excludes donations, balance of trade services, etc.

	{
	for (int i = 0; i < m_List.GetCount(); i++)
		switch (m_List[i].iService)
			{
			case serviceBuy:
			case serviceSell:
			case serviceRefuel:
			case serviceRepairArmor:
			case serviceReplaceArmor:
			case serviceInstallDevice:
			case serviceRemoveDevice:
			case serviceUpgradeDevice:
			case serviceEnhanceItem:
			case serviceRepairItem:
			case serviceCustom:
			case serviceBuyShip:
			case serviceSellShip:
				return true;

			default:
				break;
			}
	
	return false;
	}

bool CTradingDesc::HasSameCriteria (const SServiceDesc &S1, const SServiceDesc &S2)

//	HasSameCriteria
//
//	Returns TRUE if the two services have the same criteria.

	{
	switch (S1.iService)
		{
		case serviceBuyShip:
		case serviceDecontaminate:
		case serviceSellShip:
			return S1.TypeCriteria.IsEqual(S2.TypeCriteria);

		default:
			{
			if (S1.pItemType)
				return (S1.pItemType == S2.pItemType);
			else
				return (S1.ItemCriteria == S2.ItemCriteria);
			}
		}
	}

bool CTradingDesc::GetArmorInstallPrice (const CSpaceObject *pProvider, const CItem &Item, DWORD dwFlags, int *retiPrice, SReasonText *retReason) const

//	GetArmorInstallPrice
//
//	Returns the price to install the given armor

	{
	//	Look for the service

	const SServiceDesc *pService;
	if (!FindService(serviceReplaceArmor, Item, &pService))
		return false;

	//	Compute price

	STradeServiceCtx Ctx;
	Ctx.iService = serviceReplaceArmor;
	Ctx.pProvider = pProvider;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pItem = &Item;
	Ctx.iCount = 1;

	int iPrice = ComputePrice(Ctx, *pService, dwFlags);
	if (iPrice < 0)
		{
		GetReason(*pService, retReason);
		return false;
		}

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	GetReason(*pService, retReason);
	return true;
	}

bool CTradingDesc::GetArmorRepairPrice (const CSpaceObject *pProvider, CSpaceObject *pSource, const CItem &Item, int iHPToRepair, DWORD dwFlags, int *retiPrice) const

//	GetArmorRepairPrice
//
//	Returns the price for repairing the given armor.

	{
	//	Look for the service

	const SServiceDesc *pService;
	if (!FindService(serviceRepairArmor, Item, &pService))
		return false;

	//	Compute price

	STradeServiceCtx Ctx;
	Ctx.iService = serviceRepairArmor;
	Ctx.pProvider = pProvider;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pObj = pSource;
	Ctx.pItem = &Item;
	Ctx.iCount = iHPToRepair;

	int iPrice = ComputePrice(Ctx, *pService, dwFlags);
	if (iPrice < 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

bool CTradingDesc::GetDeviceInstallPrice (const CSpaceObject *pProvider, const CItem &Item, DWORD dwFlags, int *retiPrice, SReasonText *retReason, DWORD *retdwPriceFlags) const

//	GetDeviceInstallPrice
//
//	Returns the price to install the given device

	{
	//	Look for the service

	const SServiceDesc *pService;
	if (!FindService(serviceInstallDevice, Item, &pService))
		return false;

	//	Compute price

	STradeServiceCtx Ctx;
	Ctx.iService = serviceInstallDevice;
	Ctx.pProvider = pProvider;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pItem = &Item;
	Ctx.iCount = 1;

	int iPrice = ComputePrice(Ctx, *pService, dwFlags);
	if (iPrice < 0)
		{
		GetReason(*pService, retReason);
		return false;
		}

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	if (retdwPriceFlags)
		{
		(*retdwPriceFlags) = 0;
		if (pService->dwFlags & FLAG_UPGRADE_INSTALL_ONLY)
			(*retdwPriceFlags) |= PRICE_UPGRADE_INSTALL_ONLY;
		}

	GetReason(*pService, retReason);
	return true;
	}

bool CTradingDesc::GetDeviceRemovePrice (const CSpaceObject *pProvider, const CItem &Item, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags) const

//	GetDeviceRemovePrice
//
//	Returns the price to remove the given device

	{
	//	Look for the service

	const SServiceDesc *pService;
	if (!FindService(serviceRemoveDevice, Item, &pService))
		return false;

	//	Compute price

	STradeServiceCtx Ctx;
	Ctx.iService = serviceRemoveDevice;
	Ctx.pProvider = pProvider;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pItem = &Item;
	Ctx.iCount = 1;

	int iPrice = ComputePrice(Ctx, *pService, dwFlags);
	if (iPrice < 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	if (retdwPriceFlags)
		{
		(*retdwPriceFlags) = 0;
		if (pService->dwFlags & FLAG_UPGRADE_INSTALL_ONLY)
			(*retdwPriceFlags) |= PRICE_UPGRADE_INSTALL_ONLY;
		}

	return true;
	}

int CTradingDesc::GetMaxLevelMatched (CUniverse &Universe, ETradeServiceTypes iService, bool bDescriptionOnly) const

//	GetMaxLevelMatched
//
//	Returns the max item level matched for the given service. Returns -1 if the
//	service is not available.

	{
	int i;
	int iMaxLevel = -1;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);
			if (strEquals(sPrefix, UNAVAILABLE_PREFIX))
				continue;

			//  If we're looking for a description, ignore any entries that
			//  don't want one.

			if (bDescriptionOnly && (m_List[i].dwFlags & FLAG_NO_DESCRIPTION))
				continue;

			int iLevel;
			if (m_List[i].pItemType)
				iLevel = m_List[i].pItemType->GetLevel();
			else
				iLevel = m_List[i].ItemCriteria.GetMaxLevelMatched(Universe);

			if (iLevel > iMaxLevel)
				iMaxLevel = iLevel;
			}
	
	return iMaxLevel;
	}

void CTradingDesc::GetReason (const SServiceDesc &Service, SReasonText *retReason) const

//	GetReason
//
//	Returns the reason text.

	{
	if (retReason)
		{
		if (Service.dwFlags & FLAG_MESSAGE_TEXT)
			retReason->sDesc = Service.sMessageID;
		else
			retReason->sDescID = Service.sMessageID;
		}
	}

bool CTradingDesc::GetRefuelItemAndPrice (const CSpaceObject *pProvider, CSpaceObject *pObjToRefuel, DWORD dwFlags, CItemType **retpItemType, int *retiPrice) const

//	GetRefuelItemAndPrice
//
//	Returns the appropriate fuel and price to refuel the given object.

	{
	int i, j;
	CShip *pShipToRefuel = pObjToRefuel->AsShip();
	if (pShipToRefuel == NULL)
		return false;

	CUniverse &Universe = pShipToRefuel->GetUniverse();

	STradeServiceCtx Ctx;
	Ctx.iService = serviceRefuel;
	Ctx.pProvider = pProvider;
	Ctx.pCurrency = m_pCurrency;
	Ctx.iCount = 1;

	Metric rMaxFuel = pShipToRefuel->GetMaxFuel();

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == serviceRefuel)
			{
			int iBestPrice = 0;
			int iBestMetric = 0;
			CItemType *pBestItem = NULL;

			//	Find the lowest-level item that matches the given criteria.
			//	If we find it, then we use it. We use the lowest-level item 
			//	because we want the cheapest price per fuel.

			for (j = 0; j < Universe.GetItemTypeCount(); j++)
				{
				CItemType *pType = Universe.GetItemType(j);
				CItem Item(pType, 1);

				if (Item.MatchesCriteria(m_List[i].ItemCriteria)
						&& pShipToRefuel->IsFuelCompatible(Item))
					{
					//	Compute how good this fuel is relative to others. Any fuel 
					//	that requires at least 10 items to fill the ship is worth
					//	100 points.
					//
					//	We add the item level to that base. We need to do this because
					//	we don't want to refuel with in big chunks (or else we'll
					//	think the ship is close enough to full).

					Metric rFuelPerItem = Max(1.0, strToDouble(Item.GetType()->GetData(), 0.0, NULL));
					Metric rItemsToFillShip = rMaxFuel / rFuelPerItem;

					int iMetric = ((rItemsToFillShip >= 10.0) ? 100 : 0) + (MAX_ITEM_LEVEL - pType->GetLevel());
					if (pBestItem == NULL || iMetric > iBestMetric)
						{
						//	Compute the price, because if we don't sell it, then we
						//	skip it.
						//
						//	NOTE: Unlike selling, we allow 0 prices because some 
						//	stations give fuel for free.

						Ctx.pItem = &Item;
						
						int iPrice = ComputePrice(Ctx, m_List[i], dwFlags);
						if (iPrice >= 0)
							{
							pBestItem = pType;
							iBestPrice = iPrice;
							iBestMetric = iMetric;
							}
						}
					}
				}

			//	If found, then return it.

			if (pBestItem)
				{
				if (retpItemType)
					*retpItemType = pBestItem;

				if (retiPrice)
					*retiPrice = iBestPrice;

				return true;
				}
			}

	//	Otherwise, not found

	return false;
	}

void CTradingDesc::GetServiceInfo (int iIndex, SServiceInfo &Result) const

//	GetServiceInfo
//
//	Returns information about the given service.

	{
	const SServiceDesc &Service = m_List[iIndex];

	Result.iService = Service.iService;
	Result.pItemType = Service.pItemType;
	Result.sItemCriteria = Service.ItemCriteria.AsString();
	Result.sTypeCriteria = Service.TypeCriteria.AsString();
	Result.iPriceAdj = Service.PriceAdj.EvalAsInteger(NULL);

	Result.bInventoryAdj = ((Service.dwFlags & FLAG_INVENTORY_ADJ) ? true : false);
	Result.bUpgradeInstallOnly = ((Service.dwFlags & FLAG_UPGRADE_INSTALL_ONLY) ? true : false);
	}

bool CTradingDesc::GetServiceStatus (CUniverse &Universe, ETradeServiceTypes iService, SServiceStatus &retStatus) const

//	GetServiceStatus
//
//	Returns service status.

	{
	SReasonText UnavailableReason;
	SReasonText AvailableReason;
	int iMaxLevel = -1;
	bool bUpgradeInstallOnly = false;

	//	Loop over the commodity list and find the first entry that matches

	for (int i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);
			if (strEquals(sPrefix, UNAVAILABLE_PREFIX))
				{
				GetReason(m_List[i], &UnavailableReason);
				continue;
				}

			int iLevel;
			if (m_List[i].pItemType)
				iLevel = m_List[i].pItemType->GetLevel();
			else
				iLevel = m_List[i].ItemCriteria.GetMaxLevelMatched(Universe);

			if (iLevel > iMaxLevel)
				{
				iMaxLevel = iLevel;

				bUpgradeInstallOnly = ((m_List[i].dwFlags & FLAG_UPGRADE_INSTALL_ONLY) ? true : false);
				GetReason(m_List[i], &AvailableReason);
				}
			}

	//	Compose result

	retStatus.bAvailable = (iMaxLevel != -1);
	retStatus.iMaxLevel = iMaxLevel;
	retStatus.iPrice = -1;
	retStatus.Message = (iMaxLevel != -1 ? AvailableReason : UnavailableReason);

	retStatus.dwPriceFlags = 0;
	if (bUpgradeInstallOnly)
		retStatus.dwPriceFlags |= PRICE_UPGRADE_INSTALL_ONLY;

	return retStatus.bAvailable;
	}

bool CTradingDesc::GetServiceTypeInfo (CUniverse &Universe, ETradeServiceTypes iService, SServiceTypeInfo &Info) const

//  GetServiceTypeInfo
//
//  Returns information about the given service suitable for creating a human-
//  readable description.

	{
	int i;

	//  Initialize to defaults.

	Info.bAvailable = false;
	Info.bUpdateInstallOnly = true;
	Info.iMaxLevel = -1;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);

			//  Ignore entries that don't count towards the description

			if (strEquals(sPrefix, UNAVAILABLE_PREFIX)
					|| (m_List[i].dwFlags & FLAG_NO_DESCRIPTION))
				continue;

			//  Found at least one service.

			Info.bAvailable = true;

			//  If we don't require a purchase, then clear the flag

			if (!(m_List[i].dwFlags & FLAG_UPGRADE_INSTALL_ONLY))
				Info.bUpdateInstallOnly = false;

			//  Compute the level

			int iLevel;
			if (m_List[i].pItemType)
				iLevel = m_List[i].pItemType->GetLevel();
			else
				iLevel = m_List[i].ItemCriteria.GetMaxLevelMatched(Universe);

			if (iLevel > Info.iMaxLevel)
				Info.iMaxLevel = iLevel;
			}

	return Info.bAvailable;
	}

bool CTradingDesc::HasService (CUniverse &Universe, ETradeServiceTypes iService, const SHasServiceOptions &Options) const

//	HasService
//
//	Returns true if it has the given service.

	{
	int i;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);
			if (strEquals(sPrefix, UNAVAILABLE_PREFIX))
				continue;

			//	Make sure we match the options

			if (!MatchesHasServiceOptions(Universe, Options, m_List[i]))
				continue;

			//	Found it!

			return true;
			}
	
	return false;
	}

bool CTradingDesc::HasServiceUpgradeOnly (ETradeServiceTypes iService) const

//	HasServiceUpgradeOnly
//
//	Returns TRUE if we have the service, but for upgrade only.

	{
	int i;

	bool bHasUpgrade = false;
	bool bHasNonUpgrade = false;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);
			if (strEquals(sPrefix, UNAVAILABLE_PREFIX))
				continue;

			if (m_List[i].dwFlags & FLAG_UPGRADE_INSTALL_ONLY)
				bHasUpgrade = true;
			else
				bHasNonUpgrade = true;
			}
	
	return (bHasUpgrade && !bHasNonUpgrade);
	}

bool CTradingDesc::HasServiceDescription (ETradeServiceTypes iService) const

//	HasServiceDescription
//
//	Returns TRUE if it has the given service AND we should add it to the 
//  service description (in ComposeDescription).

	{
	int i;

	//	Loop over the commodity list and find the first entry that matches

	for (i = 0; i < m_List.GetCount(); i++)
		if (m_List[i].iService == iService)
			{
			CString sPrefix;
			int iPriceAdj = m_List[i].PriceAdj.EvalAsInteger(NULL, &sPrefix);
			if (strEquals(sPrefix, UNAVAILABLE_PREFIX)
					|| (m_List[i].dwFlags & FLAG_NO_DESCRIPTION))
				continue;

			return true;
			}
	
	return false;
	}

void CTradingDesc::Init (const CTradingDesc &Src)

//	Init
//
//	Initializes from another descriptor

	{
	m_pCurrency.Set(Src.GetEconomyType());
	m_iMaxCurrency = Src.m_iMaxCurrency;
	m_iReplenishCurrency = Src.m_iReplenishCurrency;
	}

bool CTradingDesc::Matches (const CItem &Item, const SServiceDesc &Commodity) const

//	Matches
//
//	Returns TRUE if the given item matches the commodity

	{
	if (Commodity.pItemType)
		return (Commodity.pItemType == Item.GetType());
	else
		return Item.MatchesCriteria(Commodity.ItemCriteria);
	}

bool CTradingDesc::Matches (const CDesignType *pType, const SServiceDesc &Commodity) const

//	Matches
//
//	Returns TRUE if the given type matches the commodity

	{
	if (pType == NULL)
		return false;

	return pType->MatchesCriteria(Commodity.TypeCriteria);
	}

bool CTradingDesc::MatchesHasServiceOptions (CUniverse &Universe, const SHasServiceOptions &Options, const SServiceDesc &Service) const

//	MatchesHasServiceOptions
//
//	Returns TRUE if the given service matches the set of options.

	{
	//	If we only want full install, then make sure this service does
	//	not require an upgrade

	if ((Options.bFullInstallOnly)
			&& (Service.dwFlags & FLAG_UPGRADE_INSTALL_ONLY))
		return false;

	//	Match criteria

	switch (Service.iService)
		{
		case serviceBuy:
		case serviceSell:
		case serviceAcceptDonations:
		case serviceRefuel:
		case serviceRepairArmor:
		case serviceReplaceArmor:
		case serviceInstallDevice:
		case serviceRemoveDevice:
		case serviceUpgradeDevice:
		case serviceEnhanceItem:
		case serviceRepairItem:
			{
			if (!Options.ItemCriteria.IsEmpty())
				{
				if (Service.pItemType && !CItem(Service.pItemType, 1).MatchesCriteria(Options.ItemCriteria))
					return false;
				else if (!Service.ItemCriteria.Intersects(Universe, Options.ItemCriteria))
					return false;
				}
			break;
			}
		}

	//	Success!

	return true;
	}

void CTradingDesc::OnCreate (CSpaceObject *pObj)

//	OnCreate
//
//	Station is created

	{
	//	Give the station a limited amount of money

	CurrencyValue iMaxCurrency = CalcMaxBalance(pObj);
	pObj->CreditMoney(m_pCurrency->GetUNID(), iMaxCurrency);
	}

ALERROR CTradingDesc::OnDesignLoadComplete (SDesignLoadCtx &Ctx)

//	OnDesignLoadComplete
//
//	Design loaded

	{
	ALERROR error;
	int i;

	if (error = m_pCurrency.Bind(Ctx))
		return error;

	for (i = 0; i < m_List.GetCount(); i++)
		if (error = m_List[i].pItemType.Bind(Ctx))
			return error;

	return NOERROR;
	}

void CTradingDesc::OnUpdate (CSpaceObject *pObj)

//	OnUpdate
//
//	Station updates (call roughly every 1800 ticks)

	{
	DEBUG_TRY

	CurrencyValue iReplenish;
	CurrencyValue iBalance = pObj->GetBalance(m_pCurrency->GetUNID());
	CurrencyValue iMaxCurrency = CalcMaxBalance(pObj, &iReplenish);

	if (iBalance < iMaxCurrency && iReplenish)
		pObj->CreditMoney(m_pCurrency->GetUNID(), iReplenish);

	DEBUG_CATCH
	}

bool CTradingDesc::ParseHasServiceOptions (ICCItem *pOptions, SHasServiceOptions &retOptions)

//	ParseHasServiceFlags
//
//	Returns flags for HasService.

	{
	//	Short-circuit

	retOptions = SHasServiceOptions();
	if (pOptions == NULL || pOptions->IsNil())
		return true;

	//	Parse flags

	retOptions.bFullInstallOnly = pOptions->GetBooleanAt(CONSTLIT("fullInstallOnly"));

	//	Item criteria

	retOptions.ItemCriteria.Init(pOptions->GetStringAt(CONSTLIT("itemCriteria")), CItemCriteria::NONE);

	return true;
	}

ETradeServiceTypes CTradingDesc::ParseService (const CString &sService)

//	ParseService
//
//	Returns the service type

	{
	int i;

	for (i = 0; i < serviceCount; i++)
		if (strEquals(sService, CString(SERVICE_DATA[i].pszName))
				|| strEquals(sService, CString(SERVICE_DATA[i].pszTag)))
			return (ETradeServiceTypes)i;

	return serviceNone;
	}

void CTradingDesc::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from a stream
//
//	DWORD			m_pCurrency UNID
//	DWORD			m_iMaxCurrency
//	DWORD			m_iReplenishCurrency
//	DWORD			No of orders
//
//	DWORD			iService
//	CString			sID
//	DWORD			pItemType
//	CString			ItemCriteria or TypeCriteria
//	CFormulaText	PriceAdj
//	CFormulaText	InventoryAdj
//	CString			sMessageID
//	DWORD			dwFlags

	{
	int i;
	DWORD dwLoad;

	if (Ctx.dwVersion >= 62)
		{
		Ctx.pStream->Read(dwLoad);
		m_pCurrency.Set(Ctx.GetUniverse(), dwLoad);
		if (m_pCurrency == NULL)
			m_pCurrency.Set(Ctx.GetUniverse(), DEFAULT_ECONOMY_UNID);
		}
	else
		{
		CString sDummy;
		sDummy.ReadFromStream(Ctx.pStream);
		Ctx.pStream->Read(dwLoad);

		//	Previous versions are always credits

		m_pCurrency.Set(Ctx.GetUniverse(), DEFAULT_ECONOMY_UNID);
		}

	Ctx.pStream->Read(m_iMaxCurrency);
	Ctx.pStream->Read(m_iReplenishCurrency);

	Ctx.pStream->Read(dwLoad);
	if (dwLoad > 0)
		{
		m_List.InsertEmpty(dwLoad);

		for (i = 0; i < m_List.GetCount(); i++)
			{
			SServiceDesc &Commodity = m_List[i];

			if (Ctx.dwVersion >= 83)
				{
				Ctx.pStream->Read(dwLoad);
				Commodity.iService = (ETradeServiceTypes)dwLoad;
				}

			//	Temporarily initialize because we might check it below before we
			//	can properly load this.

			else
				Commodity.iService = serviceNone;

			Commodity.sID.ReadFromStream(Ctx.pStream);

			Ctx.pStream->Read(dwLoad);
			Commodity.pItemType = Ctx.GetUniverse().FindItemType(dwLoad);

			CString sCriteria;
			sCriteria.ReadFromStream(Ctx.pStream);

			//	For ships, this is a type criteria

			switch (Commodity.iService)
				{
				case serviceBuyShip:
				case serviceDecontaminate:
				case serviceSellShip:
					{
					CDesignTypeCriteria::ParseCriteria(sCriteria, &Commodity.TypeCriteria);
					Commodity.ItemCriteria.Init(CItemCriteria::ALL);
					break;
					}

				default:
					Commodity.ItemCriteria.Init(sCriteria);
					break;
				}

			//	Prices

			if (Ctx.dwVersion >= 62)
				{
				Commodity.PriceAdj.ReadFromStream(Ctx);
				Commodity.InventoryAdj.ReadFromStream(Ctx);
				}
			else
				{
				Ctx.pStream->Read(dwLoad);
				Commodity.PriceAdj.SetInteger(dwLoad);
				}

			if (Ctx.dwVersion >= 142)
				Commodity.sLevelFrequency.ReadFromStream(Ctx.pStream);

			if (Ctx.dwVersion >= 113)
				Commodity.sMessageID.ReadFromStream(Ctx.pStream);

			Ctx.pStream->Read(Commodity.dwFlags);

			//	If necessary we need to load backwards-compatible service

			if (Ctx.dwVersion < 83)
				ReadServiceFromFlags(Commodity.dwFlags, &Commodity.iService, &Commodity.dwFlags);

			//	For now we don't support inventory adj in dynamic trade descs
			
			Commodity.dwFlags &= ~FLAG_INVENTORY_ADJ;
			}
		}
	}

void CTradingDesc::ReadServiceFromFlags (DWORD dwFlags, ETradeServiceTypes *retiService, DWORD *retdwFlags)

//	ReadServiceFromFlags
//
//	Before we had service types we encoded info in flags. This function converts
//	to the new method.

	{
	if (retiService)
		{
		if (dwFlags & FLAG_SELLS)
			*retiService = serviceSell;
		else if (dwFlags & FLAG_BUYS)
			*retiService = serviceBuy;
		else if (dwFlags & FLAG_ACCEPTS_DONATIONS)
			*retiService = serviceAcceptDonations;
		else
			{
			ASSERT(false);
			*retiService = serviceNone;
			}
		}

	if (retdwFlags)
		*retdwFlags = (dwFlags & ~(FLAG_SELLS | FLAG_BUYS | FLAG_ACCEPTS_DONATIONS));
	}

void CTradingDesc::RefreshInventory (CSpaceObject *pObj, int iPercent)

//	RefreshInventory
//
//	Refresh the inventory for all entries that have an inventory
//	adjustment factor.

	{
	DEBUG_TRY

	if (pObj == NULL)
		return;

	int i, j;
	bool bCargoChanged = false;

	//	Notify any dock screens that we might modify an item
	//	Null item means preserve current selection.

	IDockScreenUI::SModifyItemCtx ModifyCtx;
	pObj->OnModifyItemBegin(ModifyCtx, CItem());

	//	Add

	for (i = 0; i < m_List.GetCount(); i++)
		{
		SServiceDesc &Service = m_List[i];
		if (!(Service.dwFlags & FLAG_INVENTORY_ADJ))
			continue;

		//	See if we have a level frequency.

		CString sLevelFrequency = Service.sLevelFrequency;
		if (strFind(sLevelFrequency, CONSTLIT(":")) != -1)
			sLevelFrequency = GenerateLevelFrequency(sLevelFrequency, pObj->GetSystem()->GetLevel());

		//	Make a list of all item types that match the given
		//	criteria.

		TArray<CItemType *> ItemTable;
		for (j = 0; j < pObj->GetUniverse().GetItemTypeCount(); j++)
			{
			CItemType *pType = pObj->GetUniverse().GetItemType(j);
			CItem theItem(pType, 1);
			if (theItem.MatchesCriteria(Service.ItemCriteria))
				ItemTable.Insert(pType);

			//	Skip if we some percentage of items if we're not refreshing the
			//	entire inventory.

			if (iPercent < 100 && mathRandom(1, 100) > iPercent)
				continue;
			}

		//	Loop over the count

		if (ItemTable.GetCount() == 0)
			continue;

		//	Loop over all items refreshing them

		for (j = 0; j < ItemTable.GetCount(); j++)
			{
			//	Compute the probability based on the item frequency.

			int iChance = 100 * ItemTable[j]->GetFrequency() / ftCommon;

			//	If the refresh percent is greater than 100, then we multiply the probability.

			if (iPercent > 100)
				iChance = iPercent * iChance / 100;

			//	Adjust based on level, if necessary.

			if (!sLevelFrequency.IsBlank())
				iChance = iChance * GetFrequencyByLevel(sLevelFrequency, ItemTable[j]->GetLevel()) / ftCommon;

			//	Roll

			if (iChance >= 100 || mathRandom(1, 100) <= iChance)
				{
				if (SetInventoryCount(pObj, Service, ItemTable[j]))
					bCargoChanged = true;
				}
			}
		}

	if (bCargoChanged)
		{
		pObj->OnComponentChanged(comCargo);
		pObj->OnModifyItemComplete(ModifyCtx, CItem());
		}

	DEBUG_CATCH
	}

bool CTradingDesc::RemovesCondition (const CSpaceObject *pProvider, const CShipClass &Class, ECondition iCondition, DWORD dwFlags, int *retiPrice) const

//	RemovesCondition
//
//	Returns TRUE if we remove the given condition.

	{
	STradeServiceCtx Ctx;

	switch (iCondition)
		{
		case ECondition::radioactive:
			Ctx.iService = serviceDecontaminate;
			break;

		default:
			return false;
		}

	Ctx.pProvider = pProvider;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pType = &Class;
	Ctx.iCount = 1;

	//	Compute price

	int iPrice = ComputePrice(Ctx, dwFlags);
	if (iPrice <= 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

bool CTradingDesc::RemovesCondition (const CSpaceObject *pProvider, const CSpaceObject &Ship, ECondition iCondition, DWORD dwFlags, int *retiPrice) const

//	RemovesCondition
//
//	Returns TRUE if we remove the given condition.

	{
	STradeServiceCtx Ctx;

	switch (iCondition)
		{
		case ECondition::radioactive:
			Ctx.iService = serviceDecontaminate;
			break;

		default:
			return false;
		}

	Ctx.pProvider = pProvider;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pObj = &Ship;
	Ctx.iCount = 1;

	//	Compute price

	int iPrice = ComputePrice(Ctx, dwFlags);
	if (iPrice <= 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

bool CTradingDesc::Sells (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice)

//	Sells
//
//	Returns TRUE if the given object can currently sell the given item type.
//	Optionally returns a price

	{
	STradeServiceCtx Ctx;
	Ctx.iService = serviceSell;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pItem = &Item;
	Ctx.iCount = 1;

	//	Compute price

	int iPrice = ComputePrice(Ctx, dwFlags);
	if (iPrice <= 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

bool CTradingDesc::SellsShip (CSpaceObject *pObj, CShipClass *pClass, DWORD dwFlags, int *retiPrice)

//	SellsShip
//
//	Returns TRUE if the given object can sell the given ship. Optionally returns
//	a price.

	{
	STradeServiceCtx Ctx;
	Ctx.iService = serviceSellShip;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pType = pClass;
	Ctx.iCount = 1;

	//	Compute price

	int iPrice = ComputePrice(Ctx, dwFlags);
	if (iPrice <= 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

bool CTradingDesc::SellsShip (CSpaceObject *pObj, CSpaceObject *pShip, DWORD dwFlags, int *retiPrice)

//	SellsShip
//
//	Returns TRUE if the given object can sell the given ship. Optionally returns
//	a price.

	{
	STradeServiceCtx Ctx;
	Ctx.iService = serviceSellShip;
	Ctx.pProvider = pObj;
	Ctx.pCurrency = m_pCurrency;
	Ctx.pObj = pShip;
	Ctx.iCount = 1;

	//	Compute price

	int iPrice = ComputePrice(Ctx, dwFlags);
	if (iPrice <= 0)
		return false;

	//	Done

	if (retiPrice)
		*retiPrice = iPrice;

	return true;
	}

CString CTradingDesc::ServiceToString (ETradeServiceTypes iService)

//	ServiceToString
//
//	Returns a string representing the service

	{
	if (iService >= 0 && iService < serviceCount)
		return CString(SERVICE_DATA[iService].pszName);
	else
		return CONSTLIT("unknown");
	}

bool CTradingDesc::SetInventoryCount (CSpaceObject *pObj, SServiceDesc &Desc, CItemType *pItemType)

//	SetInventoryCount
//
//	Sets the count for the given item
//	Returns TRUE if the count was changed; FALSE otherwise.

	{
	bool bCargoChanged = false;
	CItemListManipulator ItemList(pObj->GetItemList());

	//	Roll number appearing

	int iItemCount = pItemType->GetNumberAppearing().Roll();

	//	Adjust based on inventory adjustment

	iItemCount = iItemCount * Desc.InventoryAdj.EvalAsInteger(pObj) / 100;

	//	If the item exists, set the count

	if (ItemList.SetCursorAtItem(CItem(pItemType, 1)))
		{
		ItemList.SetCountAtCursor(iItemCount);
		bCargoChanged = true;
		}

	//	Otherwise, add the appropriate number of items

	else if (iItemCount > 0)
		{
		ItemList.AddItem(CItem(pItemType, iItemCount));
		bCargoChanged = true;
		}

	//	Done

	return bCargoChanged;
	}

void CTradingDesc::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Writes to a stream
//
//	DWORD			m_pCurrency UNID
//	DWORD			m_iMaxCurrency
//	DWORD			m_iReplenishCurrency
//	DWORD			No of orders
//
//	DWORD			iService
//	CString			sID
//	DWORD			pItemType
//	CString			ItemCriteria or TypeCriteria
//	CFormulaText	PriceAdj
//	CFormulaText	InventoryAdj
//	CString			sLevelFrequency
//	CString			sMessageID
//	DWORD			dwFlags

	{
	int i;
	DWORD dwSave;

	dwSave = (m_pCurrency ? m_pCurrency->GetUNID() : 0);
	pStream->Write(dwSave);

	pStream->Write(m_iMaxCurrency);
	pStream->Write(m_iReplenishCurrency);

	dwSave = m_List.GetCount();
	pStream->Write(dwSave);

	for (i = 0; i < m_List.GetCount(); i++)
		{
		const SServiceDesc &Commodity = m_List[i];

		dwSave = Commodity.iService;
		pStream->Write(dwSave);

		Commodity.sID.WriteToStream(pStream);

		dwSave = (Commodity.pItemType ? Commodity.pItemType->GetUNID() : 0);
		pStream->Write(dwSave);

		//	Criteria is different depending on service

		CString sCriteria;
		switch (Commodity.iService)
			{
			case serviceBuyShip:
			case serviceDecontaminate:
			case serviceSellShip:
				sCriteria = Commodity.TypeCriteria.AsString();
				break;

			default:
				sCriteria = Commodity.ItemCriteria.AsString();
				break;
			}

		sCriteria.WriteToStream(pStream);

		Commodity.PriceAdj.WriteToStream(pStream);
		Commodity.InventoryAdj.WriteToStream(pStream);
		Commodity.sLevelFrequency.WriteToStream(pStream);

		Commodity.sMessageID.WriteToStream(pStream);

		pStream->Write(Commodity.dwFlags);
		}
	}


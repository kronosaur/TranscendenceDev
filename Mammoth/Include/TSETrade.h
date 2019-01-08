//	TSETrade.h
//
//	Defines classes and interfaces for station services.
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

enum ETradeServiceTypes
	{
	serviceNone =						0,

	serviceBuy =						1,	//	Object buys item from the player
	serviceSell =						2,	//	Object sells item to the player
	serviceAcceptDonations =			3,
	serviceRefuel =						4,
	serviceRepairArmor =				5,
	serviceReplaceArmor =				6,
	serviceInstallDevice =				7,
	serviceRemoveDevice =				8,
	serviceUpgradeDevice =				9,
	serviceEnhanceItem =				10,
	serviceRepairItem =					11,
	serviceCustom =						12,
	serviceBuyShip =					13,	//	Object buys ship from the player
	serviceSellShip =					14,	//	Object sells ship to the player
	serviceConsume =					15,	//	Object consumes items (and thus makes them more expensive)
	serviceProduce =					16,	//	Object produces items (and thus makes them cheaper)
	serviceTrade =						17,	//	Object balance trade (consumption/production)

	serviceCount =						18,
	};

struct STradeServiceCtx
	{
	ETradeServiceTypes iService = serviceNone;		//	Service
	CSpaceObject *pProvider = NULL;					//	Object providing the service
	const CEconomyType *pCurrency = NULL;			//	Currency to use
	const CTopologyNode *pNode = NULL;				//	Optional node (in case we have no provider)

	int iCount = 0;									//	Number of items
	const CItem *pItem = NULL;						//	For item-based services
	CDesignType *pType = NULL;						//	For type-based services (e.g., serviceSellShip)
	CSpaceObject *pObj = NULL;						//	For obj-based services (e.g., serviceSellShip)
	};

class CTradingDesc
	{
	public:
		enum Flags
			{
			FLAG_NO_INVENTORY_CHECK =	0x00000001,	//	Do not check to see if item exists
			FLAG_NO_DONATION =			0x00000002,	//	Do not return prices for donations
			FLAG_NO_PLAYER_ADJ =		0x00000004,	//	Do not compute special player adjs
			};

		enum PriceFlags
			{
			PRICE_UPGRADE_INSTALL_ONLY =	0x00000001,	//	Install available only if you buy item
			};

		struct SServiceInfo
			{
			ETradeServiceTypes iService =  serviceNone;	//	Type of service

			CItemType *pItemType = NULL;				//	Item type
			CString sItemCriteria;						//	If ItemType is NULL, this is the criteria
			CString sTypeCriteria;						//	Type criteria (for selling ships, etc.).

			int iPriceAdj = -1;

			bool bInventoryAdj = false;					//	TRUE if we restock inventory for this service
			bool bUpgradeInstallOnly = false;			//	TRUE if we only install as part of upgrade
			};

		struct SHasServiceOptions
			{
			CItemCriteria ItemCriteria;					//	Service that matches this criteria

			bool bFullInstallOnly = false;				//	If TRUE, service must not require purchase to install
			};

		CTradingDesc (void);
		~CTradingDesc (void);

		void AddOrder (ETradeServiceTypes iService, const CString &sCriteria, CItemType *pItemType, int iPriceAdj);
		void AddOrders (const CTradingDesc &Trade);

		inline void AddBuyOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj)
			{ AddOrder(pType, sCriteria, iPriceAdj, FLAG_BUYS); }
		inline void AddSellOrder (CItemType *pType, const CString &sCriteria, int iPriceAdj)
			{ AddOrder(pType, sCriteria, iPriceAdj, FLAG_SELLS); }

		bool Buys (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice = NULL, int *retiMaxCount = NULL);
		bool Buys (STradeServiceCtx &Ctx, const CItem &Item, DWORD dwFlags, int *retiPrice = NULL, int *retiMaxCount = NULL);
		bool BuysShip (CSpaceObject *pObj, CShipClass *pClass, DWORD dwFlags, int *retiPrice = NULL);
		bool BuysShip (CSpaceObject *pObj, CSpaceObject *pShip, DWORD dwFlags, int *retiPrice = NULL);
		int Charge (CSpaceObject *pObj, int iCharge);
        bool ComposeDescription (CString *retsDesc) const;
		bool GetArmorInstallPrice (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason = NULL) const;
		bool GetArmorRepairPrice (CSpaceObject *pObj, CSpaceObject *pSource, const CItem &Item, int iHPToRepair, DWORD dwFlags, int *retiPrice) const;
		bool GetDeviceInstallPrice (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, CString *retsReason = NULL, DWORD *retdwPriceFlags = NULL) const;
		bool GetDeviceRemovePrice (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice, DWORD *retdwPriceFlags = NULL) const;
		inline const CEconomyType *GetEconomyType (void) { return m_pCurrency; }
		inline int GetMaxCurrency (void) { return m_iMaxCurrency; }
		int GetMaxLevelMatched (ETradeServiceTypes iService, bool bDescriptionOnly = false) const;
		bool GetRefuelItemAndPrice (CSpaceObject *pObj, CSpaceObject *pObjToRefuel, DWORD dwFlags, CItemType **retpItemType, int *retiPrice) const;
		inline int GetReplenishCurrency (void) { return m_iReplenishCurrency; }
		inline int GetServiceCount (void) const { return m_List.GetCount(); }
		void GetServiceInfo (int iIndex, SServiceInfo &Result) const;
		bool HasConsumerService (void) const;
		bool HasService (ETradeServiceTypes iService, const SHasServiceOptions &Options = SHasServiceOptions()) const;
		bool HasServiceUpgradeOnly (ETradeServiceTypes iService) const;
        inline bool HasServices (void) const { return (m_List.GetCount() > 0); }
		bool Sells (CSpaceObject *pObj, const CItem &Item, DWORD dwFlags, int *retiPrice = NULL);
		bool SellsShip (CSpaceObject *pObj, CShipClass *pClass, DWORD dwFlags, int *retiPrice = NULL);
		bool SellsShip (CSpaceObject *pObj, CSpaceObject *pShip, DWORD dwFlags, int *retiPrice = NULL);
		void SetEconomyType (const CEconomyType *pCurrency) { m_pCurrency.Set(pCurrency); }
		void SetMaxCurrency (int iMaxCurrency) { m_iMaxCurrency = iMaxCurrency; }
		void SetReplenishCurrency (int iReplenishCurrency) { m_iReplenishCurrency = iReplenishCurrency; }

		static ALERROR CreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, CTradingDesc **retpTrade);
		void OnCreate (CSpaceObject *pObj);
		ALERROR OnDesignLoadComplete (SDesignLoadCtx &Ctx);
		void OnUpdate (CSpaceObject *pObj);
		void ReadFromStream (SLoadCtx &Ctx);
		void RefreshInventory (CSpaceObject *pObj, int iPercent = 100);
		void WriteToStream (IWriteStream *pStream);

		static int CalcPriceForService (ETradeServiceTypes iService, CSpaceObject *pProvider, const CItem &Item, int iCount, DWORD dwFlags);
		static CString ServiceToString (ETradeServiceTypes iService);
		static bool ParseHasServiceOptions (ICCItem *pOptions, SHasServiceOptions &retOptions);
		static ETradeServiceTypes ParseService (const CString &sService);

	private:
		enum InternalFlags
			{
			//	Flags for SServiceDesc

			FLAG_ACTUAL_PRICE =			0x00000004,	//	TRUE if we compute actual price
			FLAG_INVENTORY_ADJ =		0x00000008,	//	TRUE if we adjust the inventory
			FLAG_UPGRADE_INSTALL_ONLY =	0x00000020,	//	TRUE if we must purchase an item to install
            FLAG_NO_DESCRIPTION =       0x00000040, //  If TRUE, we exclude this service from ComposeDescription

			//	DEPRECATED: We don't store these flags, but we require the values
			//	for older versions.

			FLAG_SELLS =				0x00000001,	//	TRUE if station sells this item type
			FLAG_BUYS =					0x00000002,	//	TRUE if station buys this item type
			FLAG_ACCEPTS_DONATIONS =	0x00000010,	//	TRUE if we accept donations
			};

		struct SServiceDesc
			{
			ETradeServiceTypes iService = serviceNone;		//	Type of service
			CString sID;						//	ID of order

			CItemTypeRef pItemType = NULL;		//	Item type
			CItemCriteria ItemCriteria;			//	If ItemType is NULL, this is the criteria
			CFormulaText InventoryAdj;			//	% of item count found at any one time
			CString sLevelFrequency;			//	Level frequency for inventory adj

			CDesignTypeCriteria TypeCriteria;	//	Type criteria (for selling ships, etc.).

			CFormulaText PriceAdj;				//	Price adjustment

			CString sMessageID;					//	ID of language element to return if we match.
			DWORD dwFlags = 0;					//	Flags
			};

        struct SServiceTypeInfo
            {
            bool bAvailable;                    //  Service available
            int iMaxLevel;                      //  Max level for this service
            bool bUpdateInstallOnly;            //  Requires purchase
            };

		void AddOrder (CItemType *pItemType, const CString &sCriteria, int iPriceAdj, DWORD dwFlags);
		CString ComputeID (ETradeServiceTypes iService, DWORD dwUNID, const CString &sCriteria, DWORD dwFlags);
		int ComputeMaxCurrency (CSpaceObject *pObj);
		int ComputePrice (STradeServiceCtx &Ctx, DWORD dwFlags);
		bool FindByID (const CString &sID, int *retiIndex = NULL) const;
		bool FindService (ETradeServiceTypes iService, const CItem &Item, const SServiceDesc **retpDesc);
		bool FindService (ETradeServiceTypes iService, CDesignType *pType, const SServiceDesc **retpDesc);
		bool FindServiceToOverride (const SServiceDesc &NewService, int *retiIndex = NULL) const;
        bool GetServiceTypeInfo (ETradeServiceTypes iService, SServiceTypeInfo &Info) const;
		bool HasServiceDescription (ETradeServiceTypes iService) const;
		bool Matches (const CItem &Item, const SServiceDesc &Commodity) const;
		bool Matches (CDesignType *pType, const SServiceDesc &Commodity) const;
		bool MatchesHasServiceOptions (const SHasServiceOptions &Options, const SServiceDesc &Service) const;
		void ReadServiceFromFlags (DWORD dwFlags, ETradeServiceTypes *retiService, DWORD *retdwFlags);
		bool SetInventoryCount (CSpaceObject *pObj, SServiceDesc &Desc, CItemType *pItemType);

		static int AdjustForSystemPrice (STradeServiceCtx &Ctx, int iPrice);
		static int ComputePrice (STradeServiceCtx &Ctx, const SServiceDesc &Commodity, DWORD dwFlags);
		static bool HasSameCriteria (const SServiceDesc &S1, const SServiceDesc &S2);

		CEconomyTypeRef m_pCurrency;
		int m_iMaxCurrency;
		int m_iReplenishCurrency;

		TArray<SServiceDesc> m_List;
	};

class CTradingEconomy
	{
	public:
		CString GetDescription (void) const;
		bool FindPriceAdj (CItemType *pItem, int *retiAdj = NULL) const;
		void ReadFromStream (SUniverseLoadCtx &Ctx);
		void Refresh (CSystem *pSystem);
		void WriteToStream (IWriteStream *pStream) const;

	private:
		enum EImpactCategories
			{
			impactNone =					-1,

			impactDesperateShortage =		0,
			impactMajorShortage =			1,
			impactShortage =				2,
			impactImports =					3,

			impactMassiveGlut =				4,
			impactMajorGlut =				5,
			impactGlut =					6,
			impactExports =					7,

			impactBalanced =				8,
			};

		struct SCriteriaEntry
			{
			CItemCriteria Criteria;
			int iImpact = 0;
			};

		EImpactCategories CalcImpactCategory (int iImpact) const;
		CString CalcImpactDesc (EImpactCategories iImpact) const;
		CString CalcImpactSortKey (int iImpact, const CString &sName) const;
		int GetPriceImpact (CItemType *pItem) const;
		void RefreshFromTradeDesc (CSystem *pSystem, CTradingDesc *pTrade);

		TSortMap<CString, SCriteriaEntry> m_CriteriaImpact;
		TSortMap<CItemType *, int> m_ItemTypeImpact;
		TSortMap<CString, SCriteriaEntry> m_TradeImpact;
	};

class CTradingComputer
	{
	public:
		enum EFlags
			{
			//	GetItemPriceList

			FLAG_KNOWN_ONLY =				0x00000001,
			};

		static int GetItemEstimatedPrice (const CTopologyNode *pNode, CItemType *pItemType);
		static void GetItemEstimatedPriceList (const CUniverse &Universe, CItemType *pItemType, DWORD dwFlags, TSortMap<int, TArray<const CTopologyNode *>> &NodesAtPrice);

		static int GetItemBuyPrice (const CUniverse &Universe, const CTopologyNode *pNode, const CDesignTypeCriteria &Criteria, const CItem &Item, DWORD *retdwObjID = NULL);
	};

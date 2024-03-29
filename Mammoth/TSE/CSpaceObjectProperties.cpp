//	CSpaceObjectProperties.cpp
//
//	CSpaceObject class
//	Copyright (c) 2019 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define CATEGORY_BEAM							CONSTLIT("beam")
#define CATEGORY_EFFECT							CONSTLIT("effect")
#define CATEGORY_MARKER							CONSTLIT("marker")
#define CATEGORY_MISSILE						CONSTLIT("missile")
#define CATEGORY_MISSION						CONSTLIT("mission")
#define CATEGORY_SHIP							CONSTLIT("ship")
#define CATEGORY_STATION						CONSTLIT("station")

#define UPDATE_CATEGORY_BEAM					CONSTLIT("update.beam")
#define UPDATE_CATEGORY_EFFECT					CONSTLIT("update.effect")
#define UPDATE_CATEGORY_MARKER					CONSTLIT("update.marker")
#define UPDATE_CATEGORY_MISSILE					CONSTLIT("update.missile")
#define UPDATE_CATEGORY_MISSION					CONSTLIT("update.mission")
#define UPDATE_CATEGORY_SHIP					CONSTLIT("update.ship")
#define UPDATE_CATEGORY_STATION					CONSTLIT("update.station")

#define FIELD_ARMOR_INTEGRITY					CONSTLIT("armorIntegrity")
#define FIELD_DESC								CONSTLIT("desc")
#define FIELD_DESC_ID							CONSTLIT("descID")
#define FIELD_CAN_INSTALL						CONSTLIT("canInstall")
#define FIELD_CAN_REMOVE						CONSTLIT("canRemove")
#define FIELD_HULL_INTEGRITY					CONSTLIT("hullIntegrity")
#define FIELD_OBJ_ID							CONSTLIT("objID")
#define FIELD_POS								CONSTLIT("pos")
#define FIELD_PRICE								CONSTLIT("price")
#define FIELD_SHIELD_LEVEL						CONSTLIT("shieldLevel")
#define FIELD_STATUS							CONSTLIT("status")
#define FIELD_UPGRADE_INSTALL_ONLY				CONSTLIT("upgradeInstallOnly")

#define PROPERTY_ASCENDED						CONSTLIT("ascended")
#define PROPERTY_CAN_ATTACK						CONSTLIT("canAttack")
#define PROPERTY_CAN_BE_ATTACKED				CONSTLIT("canBeAttacked")
#define PROPERTY_CATEGORY						CONSTLIT("category")
#define PROPERTY_COMMS_KEY						CONSTLIT("commsKey")
#define PROPERTY_CURRENCY						CONSTLIT("currency")
#define PROPERTY_CURRENCY_NAME					CONSTLIT("currencyName")
#define PROPERTY_CYBER_DEFENSE_LEVEL			CONSTLIT("cyberDefenseLevel")
#define PROPERTY_DAMAGE_DESC					CONSTLIT("damageDesc")
#define PROPERTY_DEBUG							CONSTLIT("debug")
#define PROPERTY_DESTINY						CONSTLIT("destiny")
#define PROPERTY_DOCKING_PORTS					CONSTLIT("dockingPorts")
#define PROPERTY_EVENT_SUBSCRIBERS				CONSTLIT("eventSubscribers")
#define PROPERTY_HAS_DOCKING_PORTS				CONSTLIT("hasDockingPorts")
#define PROPERTY_ID								CONSTLIT("id")
#define PROPERTY_IDENTIFIED						CONSTLIT("identified")
#define PROPERTY_INSTALL_ARMOR_MAX_LEVEL		CONSTLIT("installArmorMaxLevel")
#define PROPERTY_INSTALL_DEVICE_MAX_LEVEL		CONSTLIT("installDeviceMaxLevel")
#define PROPERTY_INSTALL_DEVICE_UPGRADE_ONLY	CONSTLIT("installDeviceUpgradeOnly")
#define PROPERTY_KNOWN							CONSTLIT("known")
#define PROPERTY_LEVEL							CONSTLIT("level")
#define PROPERTY_MASS							CONSTLIT("mass")
#define PROPERTY_NAME_PATTERN					CONSTLIT("namePattern")
#define PROPERTY_PAINT_LAYER					CONSTLIT("paintLayer")
#define PROPERTY_PLAYER_MISSIONS_GIVEN			CONSTLIT("playerMissionsGiven")
#define PROPERTY_RADIOACTIVE					CONSTLIT("radioactive")
#define PROPERTY_REFUEL_MAX_LEVEL				CONSTLIT("refuelMaxLevel")
#define PROPERTY_REMOVE_DEVICE_MAX_LEVEL		CONSTLIT("removeDeviceMaxLevel")
#define PROPERTY_REPAIR_ARMOR_MAX_LEVEL			CONSTLIT("repairArmorMaxLevel")
#define PROPERTY_REPAIR_ARMOR_RATE				CONSTLIT("repairArmorRate")
#define PROPERTY_SCALE							CONSTLIT("scale")
#define PROPERTY_SHOW_AS_DESTINATION			CONSTLIT("showAsDestination")
#define PROPERTY_SIZE_PIXELS					CONSTLIT("sizePixels")
#define PROPERTY_SOVEREIGN						CONSTLIT("sovereign")
#define PROPERTY_SQUADRON_ID					CONSTLIT("squadronID")
#define PROPERTY_SUSPENDED						CONSTLIT("suspended")
#define PROPERTY_TYPE							CONSTLIT("type")
#define PROPERTY_UNDER_ATTACK					CONSTLIT("underAttack")

#define SCALE_STAR								CONSTLIT("star")
#define SCALE_WORLD								CONSTLIT("world")
#define SCALE_STATION							CONSTLIT("station")
#define SCALE_SHIP								CONSTLIT("ship")
#define SCALE_FLOTSAM							CONSTLIT("flotsam")

TPropertyHandler<CSpaceObject> CSpaceObject::m_BasePropertyTable = std::array<TPropertyHandler<CSpaceObject>::SPropertyDef, 18> {{
		{
		"ascended",		"True|Nil",
		[](const CSpaceObject &Obj, const CString &sProperty) { return ICCItemPtr(Obj.IsAscended()); },
		NULL,
		},

		{
		"canAttack",	"True|Nil",
		[](const CSpaceObject &Obj, const CString &sProperty) { return ICCItemPtr(Obj.CanAttack()); },
		NULL,
		},

		{
		"canBeAttacked",	"True|Nil",
		[](const CSpaceObject &Obj, const CString &sProperty) { return ICCItemPtr(Obj.CanBeAttacked()); },
		NULL,
		},

		{
		"canBeHitByFriends",	"True|Nil",
		[](const CSpaceObject &Obj, const CString &sProperty) { return ICCItemPtr(Obj.CanBeHitByFriends()); },
		[](CSpaceObject &Obj, const CString &sProperty, const ICCItem &Value, CString *retsError) { Obj.SetNoFriendlyTarget(Value.IsNil()); return true; },
		},

		{
		"data",				"Map of object data (including some properties)",
		[](const CSpaceObject &Obj, const CString &sProperty) { return Obj.m_Data.GetDataAsItem(CONSTLIT("*")); },
		[](CSpaceObject &Obj, const CString &sProperty, const ICCItem &Value, CString *retsError) { Obj.m_Data.SetData(CONSTLIT("*"), &Value); return true; },
		},
		
		{
		"debug",			"True|Nil",
		[](const CSpaceObject &Obj, const CString &sProperty) { return ICCItemPtr(Obj.InDebugMode()); },
		[](CSpaceObject &Obj, const CString &sProperty, const ICCItem &Value, CString *retsError) { if (Obj.GetUniverse().InDebugMode()) Obj.m_fDebugMode = !Value.IsNil(); return true; },
		},
		
		{
		"detectRange",		"Max range at which normal perception can detect us (light-seconds)",
		[](const CSpaceObject &Obj, const CString &sProperty) 
			{
			Metric rRange = CPerceptionCalc::GetRange(CPerceptionCalc::GetRangeIndex(Obj.GetStealth(), perceptNormal));
			return ICCItemPtr(mathRound(rRange / LIGHT_SECOND));
			},
		NULL,
		},
		
		{
		"escortingPlayer",	"True|Nil",
		[](const CSpaceObject &Obj, const CString &sProperty) { return ICCItemPtr(Obj.IsPlayerEscort()); },
		NULL,
		},

		{
		"events.system",	"Returns list of registered system events for this object.",
		[](const CSpaceObject &Obj, const CString &sProperty)
			{
			const CSystem *pSystem = Obj.GetSystem();
			if (!pSystem)
				return ICCItemPtr::Nil();

			auto Events = pSystem->GetEvents().FindEventsForObj(Obj);
			if (Events.GetCount() == 0)
				return ICCItemPtr::Nil();
			else if (Events.GetCount() == 1)
				return ICCItemPtr(Events[0]->GetEventHandlerName());
			else
				{
				ICCItemPtr pResult(ICCItem::List);
				for (int i = 0; i < Events.GetCount(); i++)
					pResult->Append(ICCItemPtr(Events[i]->GetEventHandlerName()));
				return pResult;
				}
			},
		NULL,
		},
		
		{
		"hudColor",		"Color value",
		[](const CSpaceObject &Obj, const CString &sProperty) { return ICCItemPtr(Obj.GetSymbolColor().AsHTMLColor()); },
		NULL,
		},
		
		{
		"installArmorStatus",		"Returns ability to install armor",
		[](const CSpaceObject &Obj, const CString &sProperty) 
			{
			CTradingServices Services(Obj);

			CTradingDesc::SServiceStatus Status;
			bool bOK = Services.GetServiceStatus(serviceReplaceArmor, Status);

			return CTradingDesc::AsCCItem(Status);
			},
		NULL,
		},
		
		{
		"installDeviceStatus",		"Returns ability to install devices",
		[](const CSpaceObject &Obj, const CString &sProperty) 
			{
			CTradingServices Services(Obj);

			CTradingDesc::SServiceStatus Status;
			bool bOK = Services.GetServiceStatus(serviceInstallDevice, Status);

			return CTradingDesc::AsCCItem(Status);
			},
		NULL,
		},
		
		{
		"questTarget",			"True|Nil",
		[](const CSpaceObject &Obj, const CString &sProperty) { return ICCItemPtr(Obj.m_fQuestTarget || Obj.HasAttribute(CONSTLIT("questTarget"))); },
		[](CSpaceObject &Obj, const CString &sProperty, const ICCItem &Value, CString *retsError) { Obj.m_fQuestTarget = !Value.IsNil(); return true; },
		},
		
		{
		"stealth",			"Returns object stealth level.",
		[](const CSpaceObject &Obj, const CString &sProperty) 
			{
			return ICCItemPtr(Obj.GetStealth());
			},
		NULL,
		},
		
		{
		"usableItems",	"List of items that can be used",
		[](const CSpaceObject &Obj, const CString &sProperty)
			{
			SUsableItemOptions Options;
			CMenuData List = Obj.GetUsableItems(Options);
			if (List.GetCount() == 0)
				return ICCItemPtr::Nil();
			else
				{
				const CItemList &ItemList = Obj.GetItemList();

				ICCItemPtr pResult(ICCItem::List);
				for (int i = 0; i < List.GetCount(); i++)
					{
					int iIndex = List.GetItemData(i);

					ICCItemPtr pItem(CreateListFromItem(ItemList.GetItem(iIndex)));
					pResult->Append(pItem);
					}

				return pResult;
				}
			},
		NULL,
		},
		
		{
		"visibleDamage",		"0-100: 0 = no damage.",
		[](const CSpaceObject &Obj, const CString &sProperty) { return ICCItemPtr(Obj.GetVisibleDamage()); },
		NULL,
		},
		
		{
		"visibleDamageColor",	"Green if <50; Yellow if 50-74; Red if >= 75",
		[](const CSpaceObject &Obj, const CString &sProperty)
			{
			int iVisibleDamage = Obj.GetVisibleDamage();
			if (iVisibleDamage >= 75)
				return ICCItemPtr(CG32bitPixel(255, 80, 80).AsHTMLColor());
			else if (iVisibleDamage >= 50)
				return ICCItemPtr(CG32bitPixel(255, 255, 80).AsHTMLColor());
			else
				return ICCItemPtr(CG32bitPixel(80, 255, 80).AsHTMLColor());
			},
		NULL,
		},
		
		{
		"visibleToPlayer",		"True if object is in visual range of player",
		[](const CSpaceObject &Obj, const CString &sProperty) 
			{
			const CSpaceObject *pPlayer = Obj.GetPlayerShip();
			if (!pPlayer)
				return ICCItemPtr::Nil();

			CPerceptionCalc Perception(pPlayer->GetPerception());
			return ICCItemPtr(Perception.CanVisuallyScan(Obj, pPlayer->GetDistance2(&Obj)));
			},
		NULL,
		},
		
		}};

bool CSpaceObject::FindCustomProperty (const CString &sProperty, ICCItemPtr &pResult) const

//	FindCustomProperty
//
//	Finds and evaluates a custom property.

	{
	CDesignType *pType;
	EPropertyType iType = EPropertyType::propNone;

	//	First look for property in override

	if (m_pOverride
			&& m_pOverride->FindCustomProperty(sProperty, pResult, &iType))
		{ }

	//	Otherwise, look in object type

	else if ((pType = GetType())
			&& pType->FindCustomProperty(sProperty, pResult, &iType))
		{ }

	//	Otherwise, not found

	else
		return false;

	//	If the property is an object property, then we need to look in 
	//	object data.

	if (iType == EPropertyType::propVariant 
			|| iType == EPropertyType::propData
			|| iType == EPropertyType::propObjData)
		{
		pResult = GetData(sProperty);
		return true;
		}

	//	If this is a dynamic property, we need to evaluate.

	else if (iType == EPropertyType::propDynamicData)
		{
		CCodeChainCtx CCX(GetUniverse());
		CCX.SaveAndDefineSourceVar(this);

		pResult = CCX.RunCode(pResult);
		return true;
		}

	//	Otherwise we have a valid property.

	else
		return true;
	}

ICCItemPtr CSpaceObject::GetProperty (CCodeChainCtx &CCX, const CString &sProperty) const

//	GetProperty
//
//	Returns the property.

	{
	ICCItemPtr pValue;

	//	Always start with custom properties because they override built-in 
	//	properties. [We need this in case we add a new engine property that
	//	conflicts with a custom propertie. We don't want old code to break.]

	if (FindCustomProperty(sProperty, pValue))
		return pValue;

	else if (pValue = OnFindProperty(CCX, sProperty))
		return pValue;

	else if (m_BasePropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;

	else
		{
		ICCItemPtr pResult(GetPropertyCompatible(CCX, sProperty));
		return pResult;
		}
	}

CString CSpaceObject::GetCategoryName (Categories iCategory)
	{
	switch (iCategory)
		{
		case catShip:
			return CATEGORY_SHIP;

		case catStation:
			return CATEGORY_STATION;

		case catBeam:
			return CATEGORY_BEAM;

		case catMissile:
			return CATEGORY_MISSILE;

		case catMission:
			return CATEGORY_MISSION;

		case catMarker:
			return CATEGORY_MARKER;

		default:
			return CATEGORY_EFFECT;
		}
	}

CString CSpaceObject::GetUpdatePerformanceID (Categories iCategory)
	{
	switch (iCategory)
		{
		case catShip:
			return UPDATE_CATEGORY_SHIP;

		case catStation:
			return UPDATE_CATEGORY_STATION;

		case catBeam:
			return UPDATE_CATEGORY_BEAM;

		case catMissile:
			return UPDATE_CATEGORY_MISSILE;

		case catMission:
			return UPDATE_CATEGORY_MISSION;

		case catMarker:
			return UPDATE_CATEGORY_MARKER;

		default:
			return UPDATE_CATEGORY_EFFECT;
		}
	}

ICCItem *CSpaceObject::GetPropertyCompatible (CCodeChainCtx &Ctx, const CString &sName) const

//	GetProperty
//
//	Returns the property

	{
	CCodeChain &CC = GetUniverse().GetCC();

	if (strEquals(sName, PROPERTY_CATEGORY))
		return CC.CreateString(GetCategoryName(GetCategory()));

	else if (strEquals(sName, PROPERTY_COMMS_KEY))
		{
		if (m_iHighlightChar)
			{
			char chChar = m_iHighlightChar;
			return CC.CreateString(CString(&chChar, 1));
			}
		else if (m_iDesiredHighlightChar)
			{
			char chChar = m_iHighlightChar;
			return CC.CreateString(CString(&chChar, 1));
			}
		else
			return CC.CreateNil();
		}
	else if (strEquals(sName, PROPERTY_CURRENCY))
		return CC.CreateInteger(GetDefaultEconomy()->GetUNID());

	else if (strEquals(sName, PROPERTY_CURRENCY_NAME))
		return CC.CreateString(GetDefaultEconomy()->GetSID());

	else if (strEquals(sName, PROPERTY_CYBER_DEFENSE_LEVEL))
		return CC.CreateInteger(GetCyberDefenseLevel());

	else if (strEquals(sName, PROPERTY_DAMAGE_DESC))
		{
		ICCItem *pResult = CC.CreateSymbolTable();
		SVisibleDamage Damage;
		GetVisibleDamageDesc(Damage);

		if (Damage.iShieldLevel != -1)
			pResult->SetIntegerAt(FIELD_SHIELD_LEVEL, Damage.iShieldLevel);

		if (Damage.iArmorLevel != -1)
			pResult->SetIntegerAt(FIELD_ARMOR_INTEGRITY, Damage.iArmorLevel);

		if (Damage.iHullLevel != -1)
			pResult->SetIntegerAt(FIELD_HULL_INTEGRITY, Damage.iHullLevel);

		return pResult;
		}

	else if (strEquals(sName, PROPERTY_DESTINY))
		return CC.CreateInteger(GetDestiny());

	else if (strEquals(sName, PROPERTY_DOCKING_PORTS))
		{
		const CDockingPorts *pPorts = GetDockingPorts();
		if (pPorts == NULL || pPorts->GetPortCount(this) == 0)
			return CC.CreateNil();

		ICCItem *pList = CC.CreateLinkedList();
		for (int i = 0; i < pPorts->GetPortCount(this); i++)
			{
			ICCItem *pPortDesc = CC.CreateSymbolTable();

			//	Status

			if (pPorts->IsPortEmpty(this, i))
				pPortDesc->SetStringAt(FIELD_STATUS, CONSTLIT("empty"));
			else
				pPortDesc->SetStringAt(FIELD_STATUS, CONSTLIT("inUse"));

			//	Position

			ICCItem *pValue = ::CreateListFromVector(pPorts->GetPortPos(this, i, NULL));
			pPortDesc->SetAt(FIELD_POS, pValue);
			pValue->Discard();

			//	ObjectID

			CSpaceObject *pObj = pPorts->GetPortObj(this, i);
			if (pObj)
				pPortDesc->SetIntegerAt(FIELD_OBJ_ID, pObj->GetID());

			//	Add to list

			pList->Append(pPortDesc);
			pPortDesc->Discard();
			}

		return pList;
		}

	else if (strEquals(sName, PROPERTY_EVENT_SUBSCRIBERS))
		{
		ICCItem *pResult = CC.CreateLinkedList();
		for (int i = 0; i < m_SubscribedObjs.GetCount(); i++)
			{
			CSpaceObject *pObj = m_SubscribedObjs.GetObj(i);
			if (!pObj->IsDestroyed())
				pResult->AppendInteger((int)pObj);
			}

		if (pResult->GetCount() == 0)
			{
			pResult->Discard();
			return CC.CreateNil();
			}
		else
			return pResult;
		}

	else if (strEquals(sName, PROPERTY_HAS_DOCKING_PORTS))
		return CC.CreateBool(GetDockingPortCount() > 0);

	else if (strEquals(sName, PROPERTY_ID))
		return CC.CreateInteger(GetID());

	else if (strEquals(sName, PROPERTY_IDENTIFIED))
		return CC.CreateBool(IsIdentified());

	else if (strEquals(sName, PROPERTY_INSTALL_ARMOR_MAX_LEVEL))
		{
		CTradingServices Services(*this);
		int iMaxLevel = Services.GetMaxLevel(serviceReplaceArmor);
		return (iMaxLevel != -1 ? CC.CreateInteger(iMaxLevel) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_INSTALL_DEVICE_MAX_LEVEL))
		{
		CTradingServices Services(*this);
		int iMaxLevel = Services.GetMaxLevel(serviceInstallDevice);
		return (iMaxLevel != -1 ? CC.CreateInteger(iMaxLevel) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_INSTALL_DEVICE_UPGRADE_ONLY))
		return CC.CreateBool(HasTradeUpgradeOnly(serviceInstallDevice));

	else if (strEquals(sName, PROPERTY_KNOWN))
		return CC.CreateBool(IsKnown());

	else if (strEquals(sName, PROPERTY_LEVEL))
		return CC.CreateInteger(GetLevel());

	else if (strEquals(sName, PROPERTY_MASS))
		return CC.CreateInteger((int)GetMass());

	else if (strEquals(sName, PROPERTY_NAME_PATTERN))
		{
		ICCItem *pResult = CC.CreateSymbolTable();
		DWORD dwFlags;
		pResult->SetStringAt(CONSTLIT("pattern"), GetNamePattern(0, &dwFlags));
		pResult->SetIntegerAt(CONSTLIT("flags"), dwFlags);
		return pResult;
		}

	else if (strEquals(sName, PROPERTY_PAINT_LAYER))
		return CC.CreateString(GetPaintLayerID(GetPaintLayer()));

	else if (strEquals(sName, PROPERTY_PLAYER_MISSIONS_GIVEN))
		{
		int iCount = GetUniverse().GetObjStats(GetID()).iPlayerMissionsGiven;
		if (iCount > 0)
			return CC.CreateInteger(iCount);
		else
			return CC.CreateNil();
		}

	else if (strEquals(sName, PROPERTY_RADIOACTIVE))
		return CC.CreateBool(IsRadioactive());

	else if (strEquals(sName, PROPERTY_REFUEL_MAX_LEVEL))
		{
		CTradingServices Services(*this);
		int iMaxLevel = Services.GetMaxLevel(serviceRefuel);
		return (iMaxLevel != -1 ? CC.CreateInteger(iMaxLevel) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_REMOVE_DEVICE_MAX_LEVEL))
		{
		CTradingServices Services(*this);
		int iMaxLevel = Services.GetMaxLevel(serviceRemoveDevice);
		return (iMaxLevel != -1 ? CC.CreateInteger(iMaxLevel) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_REPAIR_ARMOR_MAX_LEVEL))
		{
		CTradingServices Services(*this);
		int iMaxLevel = Services.GetMaxLevel(serviceRepairArmor);
		return (iMaxLevel != -1 ? CC.CreateInteger(iMaxLevel) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_REPAIR_ARMOR_RATE))
		{
		CTradingServices Services(*this);
		CRegenDesc Regen = Services.GetArmorRepairRate(0, GetDefaultShipRepair());
		Metric rHP = Regen.GetHPPer180(CTradingServices::DEFAULT_REPAIR_CYCLE);
		if (rHP == 0.0)
			return CC.CreateNil();
		else if (rHP == (Metric)(int)rHP)
			return CC.CreateInteger((int)rHP);
		else
			return CC.CreateDouble(rHP);
		}

	else if (strEquals(sName, PROPERTY_SCALE))
		{
		switch (GetScale())
			{
			case scaleStar:
				return CC.CreateString(SCALE_STAR);

			case scaleWorld:
				return CC.CreateString(SCALE_WORLD);

			case scaleStructure:
				return CC.CreateString(SCALE_STATION);

			case scaleShip:
				return CC.CreateString(SCALE_SHIP);

			case scaleFlotsam:
				return CC.CreateString(SCALE_FLOTSAM);

			default:
				return CC.CreateNil();
			}
		}

	else if (strEquals(sName, PROPERTY_SHOW_AS_DESTINATION))
		{
		if (!IsPlayerDestination())
			return CC.CreateNil();

		ICCItemPtr pResult(ICCItem::SymbolTable);
		pResult->SetBooleanAt(CONSTLIT("showDestination"), true);

		if (m_fAutoClearDestination)			pResult->SetBooleanAt(CONSTLIT("autoClear"), true);
		if (m_fAutoClearDestinationOnDestroy)	pResult->SetBooleanAt(CONSTLIT("autoClearOnDestroy"), true);
		if (m_fAutoClearDestinationOnDock)		pResult->SetBooleanAt(CONSTLIT("autoClearOnDock"), true);
		if (m_fAutoClearDestinationOnGate)		pResult->SetBooleanAt(CONSTLIT("autoClearOnGate"), true);
		if (m_fShowDistanceAndBearing)			pResult->SetBooleanAt(CONSTLIT("showDistance"), true);
		if (m_fShowHighlight)					pResult->SetBooleanAt(CONSTLIT("showHighlight"), true);

		return pResult->Reference();
		}

	else if (strEquals(sName, PROPERTY_SIZE_PIXELS))
		return CC.CreateInteger(GetHitSizePixels());

	else if (strEquals(sName, PROPERTY_SOVEREIGN))
		{
		CSovereign *pSovereign = GetSovereign();
		if (pSovereign)
			return CC.CreateInteger(pSovereign->GetUNID());
		else
			return CC.CreateNil();
		}

	else if (strEquals(sName, PROPERTY_SQUADRON_ID))
		{
		CSquadronID ID = GetSquadronID();
		if (ID.IsEmpty())
			return CC.CreateNil();
		else
			{
			ICCItemPtr pResult(ICCItem::SymbolTable);
			pResult->SetIntegerAt(CONSTLIT("leaderID"), ID.GetLeaderID());
			pResult->SetStringAt(CONSTLIT("squadronID"), ID.GetID());
			return pResult->Reference();
			}
		}

	else if (strEquals(sName, PROPERTY_SUSPENDED))
		return CC.CreateBool(IsSuspended());

	else if (strEquals(sName, PROPERTY_TYPE))
		{
		if (CDesignType *pType = GetType())
			return CC.CreateInteger(pType->GetUNID());
		else
			return CC.CreateNil();
		}

	else if (strEquals(sName, PROPERTY_UNDER_ATTACK))
		return CC.CreateBool(IsUnderAttack());

	else if (const CDesignType *pType = GetType())
		{
		ICCItemPtr pValue;
		if (pType->FindEngineProperty(Ctx, sName, pValue))
			return pValue->Reference();

		return CC.CreateNil();
		}
	else
		return CC.CreateNil();
	}

ICCItemPtr CSpaceObject::GetTypeProperty (CCodeChainCtx &CCX, const CString &sProperty) const

//	GetCustomProperty
//
//	Returns a property from the type.

	{
	if (CDesignType *pType = GetType())
		{
		EPropertyType iType;
		ICCItemPtr pResult = pType->GetProperty(CCX, sProperty, &iType);

		//	If the property is an object property, then we need to look in 
		//	object data.

		if (iType == EPropertyType::propVariant 
				|| iType == EPropertyType::propData
				|| iType == EPropertyType::propObjData)
			return GetData(sProperty);

		//	If this is a dynamic property, we need to evaluate.

		else if (iType == EPropertyType::propDynamicData)
			{
			CCodeChainCtx Ctx(GetUniverse());
			Ctx.SaveAndDefineSourceVar(this);

			ICCItemPtr pValue = Ctx.RunCode(pResult);
			return pValue;
			}

		//	Otherwise we have a valid property.

		else
			return pResult;
		}
	else
		return ICCItemPtr::Nil();
	}

bool CSpaceObject::SetProperty (const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets an object property

	{
	CString sError;
	bool bSuccess;
	if ((bSuccess = m_BasePropertyTable.SetProperty(*this, sName, *pValue, &sError)) || !sError.IsBlank())
		{
		if (!bSuccess && retsError)
			*retsError = sError;

		return bSuccess;
		}
	else if (strEquals(sName, PROPERTY_IDENTIFIED))
		{
		SetIdentified(!pValue->IsNil());
		return true;
		}
	else if (strEquals(sName, PROPERTY_COMMS_KEY))
		{
		CString sKey = pValue->GetStringValue();
		m_iDesiredHighlightChar = *sKey.GetASCIIZPointer();
		return true;
		}
	else if (strEquals(sName, PROPERTY_KNOWN))
		{
		SetKnown(!pValue->IsNil());
		return true;
		}
	else if (strEquals(sName, PROPERTY_SOVEREIGN))
		{
		CSovereign *pSovereign = GetUniverse().FindSovereign(pValue->GetIntegerValue());
		if (pSovereign == NULL)
			{
			if (retsError) *retsError = strPatternSubst(CONSTLIT("Unknown sovereign: %s."), pValue->GetStringValue());
			return false;
			}

		SetSovereign(pSovereign);
		return true;
		}

	//	See if this is a custom property, we set data

	else
		{
		EPropertyType iType = EPropertyType::propNone;
		ICCItemPtr pDummy;
		CDesignType *pType;

		if (m_pOverride
				&& m_pOverride->FindCustomProperty(sName, pDummy, &iType))
			{ }
		else if ((pType = GetType())
				&& pType->FindCustomProperty(sName, pDummy, &iType))
			{ }
		else
			return false;

		switch (iType)
			{
			case EPropertyType::propGlobal:
				pType->SetGlobalData(sName, pValue);
				return true;

			case EPropertyType::propData:
			case EPropertyType::propObjData:
				SetData(sName, pValue);
				return true;

			default:
				return false;
			}
		}
	}

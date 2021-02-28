//	ShipProperties.cpp
//
//	CShip class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_ALWAYS_LEAVE_WRECK				CONSTLIT("alwaysLeaveWreck")
#define PROPERTY_ARMOR_COUNT					CONSTLIT("armorCount")
#define PROPERTY_AUTO_TARGET					CONSTLIT("autoTarget")
#define PROPERTY_AVAILABLE_DEVICE_SLOTS			CONSTLIT("availableDeviceSlots")
#define PROPERTY_AVAILABLE_LAUNCHER_SLOTS		CONSTLIT("availableLauncherSlots")
#define PROPERTY_AVAILABLE_NON_WEAPON_SLOTS		CONSTLIT("availableNonWeaponSlots")
#define PROPERTY_AVAILABLE_WEAPON_SLOTS			CONSTLIT("availableWeaponSlots")
#define PROPERTY_BLINDING_IMMUNE				CONSTLIT("blindingImmune")
#define PROPERTY_CARGO_SPACE					CONSTLIT("cargoSpace")
#define PROPERTY_CARGO_SPACE_FREE_KG			CONSTLIT("cargoSpaceFreeKg")
#define PROPERTY_CARGO_SPACE_USED_KG			CONSTLIT("cargoSpaceUsedKg")
#define PROPERTY_CONTAMINATION_TIMER			CONSTLIT("contaminationTimer")
#define PROPERTY_COUNTER_INCREMENT_RATE			CONSTLIT("counterIncrementRate")
#define PROPERTY_COUNTER_VALUE					CONSTLIT("counterValue")
#define PROPERTY_COUNTER_VALUE_INCREMENT		CONSTLIT("counterValueIncrement")
#define PROPERTY_CHALLENGE_RATING				CONSTLIT("challengeRating")
#define PROPERTY_CHARACTER						CONSTLIT("character")
#define PROPERTY_CHARACTER_NAME					CONSTLIT("characterName")
#define PROPERTY_DEVICE_DAMAGE_IMMUNE			CONSTLIT("deviceDamageImmune")
#define PROPERTY_DEVICE_DISRUPT_IMMUNE			CONSTLIT("deviceDisruptImmune")
#define PROPERTY_DISINTEGRATION_IMMUNE			CONSTLIT("disintegrationImmune")
#define PROPERTY_DOCKED_AT_ID					CONSTLIT("dockedAtID")
#define PROPERTY_DOCKING_ENABLED				CONSTLIT("dockingEnabled")
#define PROPERTY_DOCKING_PORT_COUNT				CONSTLIT("dockingPortCount")
#define PROPERTY_DRIVE_POWER					CONSTLIT("drivePowerUse")
#define PROPERTY_EMP_IMMUNE						CONSTLIT("EMPImmune")
#define PROPERTY_EXIT_GATE_TIMER				CONSTLIT("exitGateTimer")
#define PROPERTY_FUEL_LEFT      				CONSTLIT("fuelLeft")
#define PROPERTY_FUEL_LEFT_EXACT				CONSTLIT("fuelLeftExact")
#define PROPERTY_HEALER_LEFT        			CONSTLIT("healerLeft")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_HULL_PRICE						CONSTLIT("hullPrice")
#define PROPERTY_INTERIOR_HP					CONSTLIT("interiorHP")
#define PROPERTY_MAX_COUNTER					CONSTLIT("maxCounter")
#define PROPERTY_MAX_FUEL						CONSTLIT("maxFuel")
#define PROPERTY_MAX_FUEL_EXACT					CONSTLIT("maxFuelExact")
#define PROPERTY_MAX_HP							CONSTLIT("maxHP")
#define PROPERTY_MAX_INTERIOR_HP				CONSTLIT("maxInteriorHP")
#define PROPERTY_MAX_SPEED						CONSTLIT("maxSpeed")
#define PROPERTY_OPEN_DOCKING_PORT_COUNT		CONSTLIT("openDockingPortCount")
#define PROPERTY_OPERATING_SPEED				CONSTLIT("operatingSpeed")
#define PROPERTY_PLAYER_BLACKLISTED				CONSTLIT("playerBlacklisted")
#define PROPERTY_PLAYER_ESCORT					CONSTLIT("playerEscort")
#define PROPERTY_PLAYER_WINGMAN					CONSTLIT("playerWingman")
#define PROPERTY_POWER							CONSTLIT("power")
#define PROPERTY_POWER_USE						CONSTLIT("powerUse")
#define PROPERTY_PRICE							CONSTLIT("price")
#define PROPERTY_RADIOACTIVE					CONSTLIT("radioactive")
#define PROPERTY_RADIATION_IMMUNE				CONSTLIT("radiationImmune")
#define PROPERTY_ROTATION						CONSTLIT("rotation")
#define PROPERTY_ROTATION_SPEED					CONSTLIT("rotationSpeed")
#define PROPERTY_SELECTED_LAUNCHER				CONSTLIT("selectedLauncher")
#define PROPERTY_SELECTED_MISSILE				CONSTLIT("selectedMissile")
#define PROPERTY_SELECTED_WEAPON				CONSTLIT("selectedWeapon")
#define PROPERTY_SHATTER_IMMUNE					CONSTLIT("shatterImmune")
#define PROPERTY_SHOW_MAP_LABEL					CONSTLIT("showMapLabel")
#define PROPERTY_STEALTH_ADJ					CONSTLIT("stealthAdj")
#define PROPERTY_STEALTH_ADJ_AT_MAX_HEAT		CONSTLIT("stealthAdjAtMaxHeat")
#define PROPERTY_TARGET							CONSTLIT("target")
#define PROPERTY_THRUST							CONSTLIT("thrust")
#define PROPERTY_THRUST_TO_WEIGHT				CONSTLIT("thrustToWeight")

#define SPEED_EMERGENCY							CONSTLIT("emergency")
#define SPEED_FULL								CONSTLIT("full")
#define SPEED_HALF								CONSTLIT("half")
#define SPEED_QUARTER							CONSTLIT("quarter")

#define STR_NEXT								CONSTLIT("next")
#define STR_PREV								CONSTLIT("prev")

TPropertyHandler<CShip> CShip::m_PropertyTable = std::array<TPropertyHandler<CShip>::SPropertyDef, 3> {{
		{
		"squadron",			"Desc of each member of squadron",
		[](const CShip &ShipObj, const CString &sProperty) 
			{
			CSystem &System = *ShipObj.GetSystem();

			ICCItemPtr pResult(ICCItem::List);

			//	Add all autons/wingmates in the system.

			for (int i = 0; i < System.GetObjectCount(); i++)
				{
				const CSpaceObject *pObj = System.GetObject(i);

				if (pObj && ShipObj.IsInOurSquadron(*pObj))
					{
					ICCItemPtr pEntry(ICCItem::SymbolTable);
					pEntry->SetStringAt(CONSTLIT("status"), CONSTLIT("deployed"));
					pEntry->SetAt(CONSTLIT("obj"), CTLispConvert::CreateObject(pObj));
					pEntry->SetIntegerAt(CONSTLIT("objID"), pObj->GetID());
					pEntry->SetStringAt(CONSTLIT("name"), pObj->GetNounPhrase());
					pEntry->SetIntegerAt(CONSTLIT("type"), pObj->GetType()->GetUNID());

					pResult->Append(pEntry);
					}
				}

			//	Now add all autons (even damaged ones).

			const CItemList &ItemList = ShipObj.GetItemList();
			for (int i = 0; i < ItemList.GetCount(); i++)
				{
				const CItem &Item = ItemList.GetItem(i);
				if (Item.HasAttribute(CONSTLIT("auton")))
					{
					CItem SingleItem = Item;
					SingleItem.SetCount(1);

					for (int j = 0; j < Item.GetCount(); j++)
						{
						ICCItemPtr pEntry(ICCItem::SymbolTable);
						pEntry->SetStringAt(CONSTLIT("status"), CONSTLIT("stored"));
						pEntry->SetStringAt(CONSTLIT("name"), Item.GetNounPhrase());
						pEntry->SetAt(CONSTLIT("item"), CTLispConvert::CreateItem(SingleItem));

						pResult->Append(pEntry);
						}
					}
				}

			//	Lastly, add all wingmates in other systems.

			CObjectTrackerCriteria Criteria;
			if (!Criteria.ParseCriteria(CONSTLIT("sQ")))
				return ICCItemPtr::Error(CONSTLIT("INTERNAL ERROR: Invalid object tracker criteria."));
				
			TArray<CObjectTracker::SObjEntry> WaitingList;
			System.GetUniverse().GetGlobalObjects().Find(NULL_STR, Criteria, &WaitingList);
			for (int i = 0; i < WaitingList.GetCount(); i++)
				{
				const CObjectTracker::SObjEntry &Entry = WaitingList[i];
				if (Entry.pNode != System.GetTopology())
					{
					ICCItemPtr pEntry(ICCItem::SymbolTable);
					pEntry->SetStringAt(CONSTLIT("status"), CONSTLIT("waiting"));
					pEntry->SetIntegerAt(CONSTLIT("objID"), Entry.dwObjID);
					pEntry->SetStringAt(CONSTLIT("nodeID"), Entry.pNode->GetID());
					pEntry->SetIntegerAt(CONSTLIT("type"), Entry.pType->GetUNID());

					pEntry->SetStringAt(CONSTLIT("name"), CLanguage::ComposeNounPhrase(Entry.sName, 1, NULL_STR, Entry.dwNameFlags, 0));

					ICCItemPtr pName(ICCItem::SymbolTable);
					pName->SetStringAt(CONSTLIT("pattern"), Entry.sName);
					pName->SetIntegerAt(CONSTLIT("flags"), Entry.dwNameFlags);
					pEntry->SetAt(CONSTLIT("namePattern"), pName);

					pResult->Append(pEntry);
					}
				}

			//	Done

			if (pResult->GetCount() > 0)
				return pResult;
			else
				return ICCItemPtr::Nil();
			},
		NULL,
		},

		{
		"squadronCommsStatus",		"List of valid squadron messages",
		[](const CShip &ShipObj, const CString &sProperty) 
			{
			DWORD dwStatus = ShipObj.GetSquadronCommsStatus();
			if (dwStatus == 0)
				return ICCItemPtr::Nil();

			ICCItemPtr pResult(ICCItem::List);

			if (dwStatus & resCanAttack)
				pResult->Append(ICCItemPtr(CONSTLIT("msgAttackTarget")));

			if (dwStatus & resCanBreakAndAttack)
				pResult->Append(ICCItemPtr(CONSTLIT("msgBreakAndAttack")));

			if ((dwStatus & resCanFormUp) || (dwStatus & resCanAbortAttack))
				pResult->Append(ICCItemPtr(CONSTLIT("msgFormUp")));

			if (dwStatus & resCanAttackInFormation)
				pResult->Append(ICCItemPtr(CONSTLIT("msgAttackInFormation")));

			if (dwStatus & resCanWait)
				pResult->Append(ICCItemPtr(CONSTLIT("msgWait")));

			if (dwStatus & resCanBeInFormation)
				pResult->Append(ICCItemPtr(CONSTLIT("msgSetFormation")));

			if (pResult->GetCount())
				return pResult;
			else
				return ICCItemPtr::Nil();
			},
		NULL,
		},

		{
		"trackFuel",		"True/Nil if we track and consume fuel",
		[](const CShip &ShipObj, const CString &sProperty) 
			{
			return ICCItemPtr(ShipObj.m_pPowerUse != NULL);
			},
		[](CShip &ShipObj, const CString &sProperty, const ICCItem &Value, CString *retsError)
			{
			ShipObj.TrackFuel(!Value.IsNil());
			return true;
			},
		},

	}};

ICCItemPtr CShip::OnFindProperty (CCodeChainCtx &CCX, const CString &sProperty) const

//	OnFindProperty
//
//	Finds the property in the table.

	{
	//	First look for a property in our table.

	ICCItemPtr pValue;
	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;

	//	Otherwise, not found

	else
		return ICCItemPtr();
	}

ICCItem *CShip::GetPropertyCompatible (CCodeChainCtx &Ctx, const CString &sName) const

//	GetProperty
//
//	Returns a property

	{
	CCodeChain &CC = GetUniverse().GetCC();
	ICCItem *pResult;

	if (strEquals(sName, PROPERTY_ALWAYS_LEAVE_WRECK))
		return CC.CreateBool(m_fAlwaysLeaveWreck || m_pClass->GetWreckChance() >= 100);

	else if (strEquals(sName, PROPERTY_ARMOR_COUNT))
		return CC.CreateInteger(GetArmorSectionCount());

	else if (strEquals(sName, PROPERTY_AUTO_TARGET))
		{
		CSpaceObject *pTarget = GetTarget();
		return (pTarget ? CC.CreateInteger((int)pTarget) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_AVAILABLE_DEVICE_SLOTS))
		{
		int iAll = CalcDeviceSlotsInUse();

		return CC.CreateInteger(m_pClass->GetHullDesc().GetMaxDevices() - iAll);
		}
	else if (strEquals(sName, PROPERTY_AVAILABLE_LAUNCHER_SLOTS))
		{
		int iLauncher;
		int iAll = CalcDeviceSlotsInUse(NULL, NULL, &iLauncher);

		return CC.CreateInteger(Max(0, Min(m_pClass->GetHullDesc().GetMaxLaunchers() - iLauncher, m_pClass->GetHullDesc().GetMaxDevices() - iAll)));
		}
	else if (strEquals(sName, PROPERTY_AVAILABLE_NON_WEAPON_SLOTS))
		{
		int iNonWeapon;
		int iAll = CalcDeviceSlotsInUse(NULL, &iNonWeapon);

		return CC.CreateInteger(Max(0, Min(m_pClass->GetHullDesc().GetMaxNonWeapons() - iNonWeapon, m_pClass->GetHullDesc().GetMaxDevices() - iAll)));
		}
	else if (strEquals(sName, PROPERTY_AVAILABLE_WEAPON_SLOTS))
		{
		int iWeapon;
		int iAll = CalcDeviceSlotsInUse(&iWeapon);

		return CC.CreateInteger(Max(0, Min(m_pClass->GetHullDesc().GetMaxWeapons() - iWeapon, m_pClass->GetHullDesc().GetMaxDevices() - iAll)));
		}
	else if (strEquals(sName, PROPERTY_BLINDING_IMMUNE))
		return CC.CreateBool(IsImmuneTo(specialBlinding));

	else if (strEquals(sName, PROPERTY_CARGO_SPACE))
		return CC.CreateInteger(CalcMaxCargoSpace());

	else if (strEquals(sName, PROPERTY_CARGO_SPACE_FREE_KG))
		return CC.CreateInteger(mathRound(GetCargoSpaceLeft() * 1000.0));

	else if (strEquals(sName, PROPERTY_CARGO_SPACE_USED_KG))
		{
		InvalidateItemMass();
		return CC.CreateInteger(mathRound(GetCargoMass() * 1000.0));
		}

	else if (strEquals(sName, PROPERTY_CONTAMINATION_TIMER))
		return CC.CreateInteger(m_iContaminationTimer);

	else if (strEquals(sName, PROPERTY_COUNTER_VALUE))
		return CC.CreateInteger(GetCounterValue());

	else if (strEquals(sName, PROPERTY_COUNTER_INCREMENT_RATE))
		return CC.CreateInteger(GetCounterIncrementRate());

	else if (strEquals(sName, PROPERTY_CHALLENGE_RATING))
		return CC.CreateInteger(CChallengeRatingCalculator::CalcChallengeRating(*this));
		
	else if (strEquals(sName, PROPERTY_CHARACTER))
		return (m_pCharacter ? CC.CreateInteger(m_pCharacter->GetUNID()) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_CHARACTER_NAME))
		return (m_pCharacter ? m_pCharacter->GetStaticData(CONSTLIT("Name"))->Reference() : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_DEVICE_DAMAGE_IMMUNE))
		return CC.CreateBool(m_Armor.IsImmune(specialDeviceDamage));

	else if (strEquals(sName, PROPERTY_DEVICE_DISRUPT_IMMUNE))
		return CC.CreateBool(m_Armor.IsImmune(specialDeviceDisrupt));

	else if (strEquals(sName, PROPERTY_DISINTEGRATION_IMMUNE))
		return CC.CreateBool(m_Armor.IsImmune(specialDisintegration));

	else if (strEquals(sName, PROPERTY_DOCKED_AT_ID))
		return (!m_fShipCompartment && m_pDocked ? CC.CreateInteger(m_pDocked->GetID()) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_DOCKING_ENABLED))
		return CC.CreateBool(CanObjRequestDock(GetPlayerShip()) == CSpaceObject::dockingOK);

	else if (strEquals(sName, PROPERTY_DOCKING_PORT_COUNT))
		return CC.CreateInteger(m_DockingPorts.GetPortCount(this));

	else if (strEquals(sName, PROPERTY_EMP_IMMUNE))
		return CC.CreateBool(IsImmuneTo(specialEMP));

	else if (strEquals(sName, PROPERTY_EXIT_GATE_TIMER))
		return (IsInGate() ? CC.CreateInteger(m_iExitGateTimer) : CC.CreateNil());

	else if (strEquals(sName, PROPERTY_FUEL_LEFT))
		return CC.CreateInteger(mathRound(GetFuelLeft() / FUEL_UNITS_PER_STD_ROD));

	else if (strEquals(sName, PROPERTY_FUEL_LEFT_EXACT))
		return CC.CreateDouble(GetFuelLeft());

	else if (strEquals(sName, PROPERTY_HEALER_LEFT))
		return CC.CreateInteger(m_Armor.GetHealerLeft());

	else if (strEquals(sName, PROPERTY_HP))
		return CC.CreateInteger(GetTotalArmorHP());

	else if (strEquals(sName, PROPERTY_HULL_PRICE))
		return CC.CreateInteger((int)GetHullValue().GetValue());

	else if (strEquals(sName, PROPERTY_INTERIOR_HP))
		{
		int iHP;
		m_Interior.GetHitPoints(*this, m_pClass->GetInteriorDesc(), &iHP);
		return CC.CreateInteger(iHP);
		}

	else if (strEquals(sName, PROPERTY_MAX_COUNTER))
		return CC.CreateInteger(m_pClass->GetHullDesc().GetMaxCounter());

	else if (strEquals(sName, PROPERTY_MAX_FUEL))
		return CC.CreateInteger(mathRound(GetMaxFuel() / FUEL_UNITS_PER_STD_ROD));

	else if (strEquals(sName, PROPERTY_MAX_FUEL_EXACT))
		return CC.CreateDouble(GetMaxFuel());

	else if (strEquals(sName, PROPERTY_MAX_HP))
		{
		int iMaxHP;
		GetTotalArmorHP(&iMaxHP);
		return CC.CreateInteger(iMaxHP);
		}

	else if (strEquals(sName, PROPERTY_MAX_INTERIOR_HP))
		{
		int iHP;
		int iMaxHP;
		m_Interior.GetHitPoints(*this, m_pClass->GetInteriorDesc(), &iHP, &iMaxHP);
		return CC.CreateInteger(iMaxHP);
		}

	else if (strEquals(sName, PROPERTY_OPEN_DOCKING_PORT_COUNT))
		return CC.CreateInteger(GetOpenDockingPortCount());

	else if (strEquals(sName, PROPERTY_OPERATING_SPEED))
		{
		if (m_fEmergencySpeed)
			return CC.CreateString(SPEED_EMERGENCY);
		else if (m_fHalfSpeed)
			return CC.CreateString(SPEED_HALF);
		else if (m_fQuarterSpeed)
			return CC.CreateString(SPEED_QUARTER);
		else
			return CC.CreateString(SPEED_FULL);
		}

	else if (strEquals(sName, PROPERTY_PLAYER_BLACKLISTED))
		return CC.CreateBool(m_pController->IsPlayerBlacklisted());

	else if (strEquals(sName, PROPERTY_PLAYER_ESCORT))
		return CC.CreateBool(IsPlayerEscort());

	else if (strEquals(sName, PROPERTY_PLAYER_WINGMAN))
		return CC.CreateBool(m_pController->IsPlayerWingman());

	else if (strEquals(sName, PROPERTY_POWER_USE))
		return CC.CreateDouble(GetPowerConsumption() * 100.0);

	else if (strEquals(sName, PROPERTY_PRICE))
		return CC.CreateInteger((int)GetTradePrice(NULL).GetValue());

	else if (strEquals(sName, PROPERTY_RADIATION_IMMUNE))
		return CC.CreateBool(IsImmuneTo(specialRadiation));

	else if (strEquals(sName, PROPERTY_ROTATION))
		return CC.CreateInteger(GetRotation());

	else if (strEquals(sName, PROPERTY_ROTATION_SPEED))
		return CC.CreateDouble(m_Rotation.GetRotationSpeedDegrees(m_Perf.GetIntegralRotationDesc()));

	else if (strEquals(sName, PROPERTY_SELECTED_LAUNCHER))
		{
		CItem theItem = GetNamedItem(devMissileWeapon);
		if (theItem.GetType() == NULL)
			return CC.CreateNil();

		return CreateListFromItem(theItem);
		}
	else if (strEquals(sName, PROPERTY_SELECTED_MISSILE))
		{
		const CInstalledDevice *pLauncher = GetNamedDevice(devMissileWeapon);
		if (pLauncher == NULL)
			return CC.CreateNil();

		CItemType *pType;
		pLauncher->GetSelectedVariantInfo(this, NULL, NULL, &pType);
		if (pType == NULL)
			return CC.CreateNil();

		if (pType->IsMissile())
			{
			CItemListManipulator ItemList(const_cast<CShip *>(this)->GetItemList());
			CItem theItem(pType, 1);
			if (!ItemList.SetCursorAtItem(theItem))
				return CC.CreateNil();

			return CreateListFromItem(ItemList.GetItemAtCursor());
			}

		//	Sometimes a launcher has no ammo (e.g., a disposable missile
		//	launcher). In that case we return the launcher itself.

		else
			{
			CItem theItem = GetNamedItem(devMissileWeapon);
			if (theItem.GetType() == NULL)
				return CC.CreateNil();

			return CreateListFromItem(theItem);
			}
		}
	else if (strEquals(sName, PROPERTY_SELECTED_WEAPON))
		{
		CItem theItem = GetNamedItem(devPrimaryWeapon);
		if (theItem.GetType() == NULL)
			return CC.CreateNil();

		return CreateListFromItem(theItem);
		}
	else if (strEquals(sName, PROPERTY_SHATTER_IMMUNE))
		return CC.CreateBool(m_Armor.IsImmune(specialShatter));

	else if (strEquals(sName, PROPERTY_SHOW_MAP_LABEL))
		return CC.CreateBool(m_fShowMapLabel);

	else if (strEquals(sName, PROPERTY_STEALTH_ADJ))
		return CC.CreateInteger(m_pClass->GetHullDesc().GetStealthAdj());

	else if (strEquals(sName, PROPERTY_STEALTH_ADJ_AT_MAX_HEAT))
		return CC.CreateInteger(m_pClass->GetHullDesc().GetStealthAdjAtMaxHeat());

	//	Drive properties

	else if (strEquals(sName, PROPERTY_DRIVE_POWER))
		return CC.CreateInteger(m_Perf.GetDriveDesc().GetPowerUse() * 100);

	else if (strEquals(sName, PROPERTY_MAX_SPEED))
		return CC.CreateInteger(mathRound(100.0 * GetMaxSpeed() / LIGHT_SPEED));

	else if (strEquals(sName, PROPERTY_TARGET))
		{
		CSpaceObject *pTarget = GetTarget(IShipController::FLAG_ACTUAL_TARGET);
		return (pTarget ? CC.CreateInteger((int)pTarget) : CC.CreateNil());
		}

	else if (strEquals(sName, PROPERTY_THRUST))
		return CC.CreateInteger((int)GetThrustProperty());

	else if (strEquals(sName, PROPERTY_THRUST_TO_WEIGHT))
		{
		Metric rMass = GetMass();
		int iRatio = mathRound(200.0 * (rMass > 0.0 ? GetThrust() / rMass : 0.0));
		return CC.CreateInteger(10 * iRatio);
		}

	//	Reactor properties

	else if (CReactorDesc::IsExportedProperty(sName))
		{
		if (pResult = m_Perf.GetReactorDesc().FindProperty(sName))
			return pResult;
		else
			return CC.CreateNil();
		}

	//  Controller properties

	else if (pResult = m_pController->FindProperty(sName))
		return pResult;

	else
		return CSpaceObject::GetPropertyCompatible(Ctx, sName);
	}

bool CShip::SetProperty (const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets an object property

	{
	CCodeChain &CC = GetUniverse().GetCC();
	ESetPropertyResult iResult;

	if (strEquals(sName, PROPERTY_ALWAYS_LEAVE_WRECK))
		{
		m_fAlwaysLeaveWreck = !pValue->IsNil();
		return true;
		}
	else if (strEquals(sName, PROPERTY_COUNTER_VALUE))
		{
		SetCounterValue(pValue->GetIntegerValue());
		return true;
		}
	else if (strEquals(sName, PROPERTY_COUNTER_VALUE_INCREMENT))
		{
		IncCounterValue(pValue->GetIntegerValue());
		return true;
		}
	else if (strEquals(sName, PROPERTY_CHARACTER))
		{
		if (pValue->IsNil())
			m_pCharacter = NULL;
		else
			{
			CGenericType *pCharacter = GetUniverse().FindGenericType((DWORD)pValue->GetIntegerValue());
			if (pCharacter == NULL)
				{
				*retsError = CONSTLIT("Unknown character UNID");
				return false;
				}

			m_pCharacter = pCharacter;
			}
		return true;
		}

	else if (strEquals(sName, PROPERTY_DOCKING_ENABLED))
		{
		m_fDockingDisabled = pValue->IsNil();
		return true;
		}
	else if (strEquals(sName, PROPERTY_FUEL_LEFT))
		{
		if (m_pPowerUse)
			m_pPowerUse->SetFuelLeft(Max(0.0, Min(pValue->GetIntegerValue() * FUEL_UNITS_PER_STD_ROD, GetMaxFuel())));
		return true;
		}

	else if (strEquals(sName, PROPERTY_FUEL_LEFT_EXACT))
		{
		if (m_pPowerUse)
			m_pPowerUse->SetFuelLeft(Max(0.0, Min(pValue->GetDoubleValue(), GetMaxFuel())));
		return true;
		}

	else if (strEquals(sName, PROPERTY_EXIT_GATE_TIMER))
		{
		if (IsInGate())
			m_iExitGateTimer = Max(0, pValue->GetIntegerValue());
		return true;
		}

	else if (strEquals(sName, PROPERTY_HEALER_LEFT))
		{
		m_Armor.SetHealerLeft(pValue->GetIntegerValue());

		//	Update the armor status because some armor HUDs show healer level.

		m_pController->OnShipStatus(IShipController::statusArmorRepaired, -1);
		return true;
		}

	else if (strEquals(sName, PROPERTY_HP))
		{
		SetTotalArmorHP(pValue->GetIntegerValue());
		return true;
		}

	else if (strEquals(sName, PROPERTY_INTERIOR_HP))
		{
		m_Interior.SetHitPoints(this, m_pClass->GetInteriorDesc(), pValue->GetIntegerValue());
		return true;
		}
	else if (strEquals(sName, PROPERTY_OPERATING_SPEED))
		{
		CString sSpeed = pValue->GetStringValue();
		if (strEquals(sSpeed, SPEED_EMERGENCY))
			SetMaxSpeedEmergency();
		else if (strEquals(sSpeed, SPEED_FULL))
			ResetMaxSpeed();
		else if (strEquals(sSpeed, SPEED_HALF))
			SetMaxSpeedHalf();
		else if (strEquals(sSpeed, SPEED_QUARTER))
			SetMaxSpeedQuarter();
		else
			{
			*retsError = strPatternSubst(CONSTLIT("Invalid speed setting: %s"), sSpeed);
			return false;
			}

		return true;
		}
	else if (strEquals(sName, PROPERTY_PLAYER_BLACKLISTED))
		{
		m_pController->SetPlayerBlacklisted(!pValue->IsNil());
		return true;
		}
	else if (strEquals(sName, PROPERTY_PLAYER_ESCORT))
		{
		m_pController->SetPlayerEscort(!pValue->IsNil());
		return true;
		}
	else if (strEquals(sName, PROPERTY_PLAYER_WINGMAN))
		{
		SetPlayerWingman(!pValue->IsNil());
		return true;
		}
	else if (strEquals(sName, PROPERTY_RADIOACTIVE))
		{
		SApplyConditionOptions Options;
		Options.bNoImmunityCheck = true;

		if (pValue->IsNil())
			RemoveCondition(ECondition::radioactive, Options);
		else
			ApplyCondition(ECondition::radioactive, Options);

		return true;
		}
	else if (strEquals(sName, PROPERTY_ROTATION))
		{
		SetRotation(pValue->GetIntegerValue());
		return true;
		}
	else if (strEquals(sName, PROPERTY_ROTATION_SPEED))
		{
		m_Rotation.SetRotationSpeedDegrees(m_Perf.GetIntegralRotationDesc(), pValue->GetDoubleValue());
		return true;
		}

	else if (strEquals(sName, PROPERTY_SELECTED_MISSILE))
		{
		//	Nil means that we don't want to make a change

		if (pValue->IsNil())
			return true;

		//	"Next" means select the next missile

		else if (strEquals(pValue->GetStringValue(), STR_NEXT))
			{
			ReadyNextMissile(1);
			return true;
			}

		//	"Prev" means select the previous missile

		else if (strEquals(pValue->GetStringValue(), STR_PREV))
			{
			ReadyNextMissile(-1);
			return true;
			}

		//	Otherwise, we expect an item

		else
			{
			CInstalledDevice *pLauncher = GetNamedDevice(devMissileWeapon);
			if (pLauncher == NULL)
				{
				*retsError = CONSTLIT("No launcher installed.");
				return false;
				}

			CCodeChainCtx Ctx(GetUniverse());
			CItem Item = Ctx.AsItem(pValue);
			CItemListManipulator ShipItems(GetItemList());
			if (!ShipItems.SetCursorAtItem(Item))
				{
				*retsError = CONSTLIT("Item is not on ship.");
				return false;
				}

			//	If the item is the same as the launcher then it means that the
			//	launcher has no ammo. In this case, we succeed.

			if (Item.GetType() == pLauncher->GetItem()->GetType())
				return true;

			//	Otherwise we figure out what ammo variant this is.

			int iVariant = pLauncher->GetClass()->GetAmmoVariant(Item.GetType());
			if (iVariant == -1)
				{
				*retsError = CONSTLIT("Item is not compatible with launcher.");
				return false;
				}

			SelectWeapon(m_Devices.GetNamedIndex(devMissileWeapon), iVariant);
			return true;
			}
		}
	else if (strEquals(sName, PROPERTY_SELECTED_WEAPON))
		{
		//	Nil means that we don't want to make a change

		if (pValue->IsNil())
			return true;

		//	"Next" means select the next weapon

		else if (strEquals(pValue->GetStringValue(), STR_NEXT))
			{
			ReadyNextWeapon(1);
			return true;
			}

		//	"Prev" means select the previous weapon

		else if (strEquals(pValue->GetStringValue(), STR_PREV))
			{
			ReadyNextWeapon(-1);
			return true;
			}

		//	Otherwise, we expect an item value

		else
			{
			CCodeChainCtx Ctx(GetUniverse());
			int iDev;
			if (!FindInstalledDeviceSlot(Ctx.AsItem(pValue), &iDev))
				{
				*retsError = CONSTLIT("Item is not an installed device on ship.");
				return false;
				}

			if (m_Devices.GetDevice(iDev).GetCategory() != itemcatWeapon)
				{
				*retsError = CONSTLIT("Item is not a weapon.");
				return false;
				}

			SelectWeapon(iDev, 0);
			return true;
			}
		}
	else if (strEquals(sName, PROPERTY_SHOW_MAP_LABEL))
		{
		m_fShowMapLabel = !pValue->IsNil();
		return true;
		}
	else if ((iResult = m_pController->SetProperty(sName, *pValue, retsError)) != ESetPropertyResult::notFound)
		{
		return (iResult == ESetPropertyResult::set);
		}
	else
		return CSpaceObject::SetProperty(sName, pValue, retsError);
	}


//	ShipClassProperties.cpp
//
//	CShipClass class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PROPERTY_ARMOR_ITEM						CONSTLIT("armorItem")
#define PROPERTY_ARMOR_SPEED_ADJ				CONSTLIT("armorSpeedAdj")
#define PROPERTY_ARMOR_SPEED_ADJ_PARAM			CONSTLIT("armorSpeedAdj:")
#define PROPERTY_CHARACTER_NAME					CONSTLIT("characterName")
#define PROPERTY_CURRENCY						CONSTLIT("currency")
#define PROPERTY_CURRENCY_NAME					CONSTLIT("currencyName")
#define PROPERTY_CYBER_DEFENSE_LEVEL			CONSTLIT("cyberDefenseLevel")
#define PROPERTY_DEFAULT_SOVEREIGN				CONSTLIT("defaultSovereign")
#define PROPERTY_DRIVE_POWER					CONSTLIT("drivePowerUse")
#define PROPERTY_FREQUENCY						CONSTLIT("frequency")
#define PROPERTY_FREQUENCY_RATIO				CONSTLIT("frequencyRatio")
#define PROPERTY_FUEL_EFFICIENCY				CONSTLIT("fuelEfficiency")
#define PROPERTY_HAS_TRADE_DESC					CONSTLIT("hasTradeDesc")
#define PROPERTY_HAS_VARIANTS					CONSTLIT("hasVariants")
#define PROPERTY_HULL_CARGO_SPACE				CONSTLIT("hullCargoSpace")
#define PROPERTY_HULL_POINTS					CONSTLIT("hullPoints")
#define PROPERTY_HULL_VALUE						CONSTLIT("hullValue")
#define PROPERTY_IMAGE_ROTATION_COUNT			CONSTLIT("imageRotationCount")
#define PROPERTY_LAUNCHER_ITEM					CONSTLIT("launcherItem")
#define PROPERTY_MAX_ARMOR_CLASS				CONSTLIT("maxArmorClass")
#define PROPERTY_MAX_ARMOR_CLASS_NAME			CONSTLIT("maxArmorClassName")
#define PROPERTY_MAX_ARMOR_MASS					CONSTLIT("maxArmorMass")
#define PROPERTY_MAX_DEVICES					CONSTLIT("maxDevices")
#define PROPERTY_MAX_SPEED						CONSTLIT("maxSpeed")
#define PROPERTY_MAX_SPEED_AT_MAX_ARMOR			CONSTLIT("maxSpeedAtMaxArmor")
#define PROPERTY_MAX_SPEED_AT_MIN_ARMOR			CONSTLIT("maxSpeedAtMinArmor")
#define PROPERTY_MAX_SPEED_BY_ARMOR_MASS		CONSTLIT("maxSpeedByArmorMass")
#define PROPERTY_MISC_DEVICE_ITEMS				CONSTLIT("miscDeviceItems")
#define PROPERTY_PERCEPTION						CONSTLIT("perception")
#define PROPERTY_POWER							CONSTLIT("power")
#define PROPERTY_PRICE							CONSTLIT("price")
#define PROPERTY_RATED_POWER					CONSTLIT("ratedPower")
#define PROPERTY_SHIELD_ITEM					CONSTLIT("shieldItem")
#define PROPERTY_STD_ARMOR_CLASS				CONSTLIT("stdArmorClass")
#define PROPERTY_STD_ARMOR_CLASS_NAME			CONSTLIT("stdArmorClassName")
#define PROPERTY_STD_ARMOR_MASS					CONSTLIT("stdArmorMass")
#define PROPERTY_STEALTH						CONSTLIT("stealth")
#define PROPERTY_THRUST							CONSTLIT("thrust")
#define PROPERTY_THRUST_RATIO					CONSTLIT("thrustRatio")
#define PROPERTY_THRUST_TO_WEIGHT				CONSTLIT("thrustToWeight")
#define PROPERTY_THRUSTER_POWER					CONSTLIT("thrusterPower")
#define PROPERTY_TREASURE_ITEM_NAMES			CONSTLIT("treasureItemNames")
#define PROPERTY_VIEWPORT_SIZE					CONSTLIT("viewportSize")
#define PROPERTY_WEAPON_ITEMS					CONSTLIT("weaponItems")
#define PROPERTY_WRECK_HAS_ITEMS				CONSTLIT("wreckHasItems")
#define PROPERTY_WRECK_HAS_INSTALLED_ITEMS		CONSTLIT("wreckHasInstalledItems")
#define PROPERTY_WRECK_STRUCTURAL_HP			CONSTLIT("wreckStructuralHP")
#define PROPERTY_WRECK_TYPE						CONSTLIT("wreckType")
#define PROPERTY_WRECK_TYPE_NAME				CONSTLIT("wreckTypeName")

TPropertyHandler<CShipClass> CShipClass::m_PropertyTable = std::array<TPropertyHandler<CShipClass>::SPropertyDef, 5> {{
		{
		"ai.combatStyle",			"Combat style",
		[](const CShipClass &ShipClass, const CString &sProperty) 
			{
			return ICCItemPtr(CAISettings::ConvertToID(ShipClass.m_AISettings.GetCombatStyle()));
			},
		NULL,
		},

		{
		"character",			"UNID of associated character",
		[](const CShipClass &ShipClass, const CString &sProperty) 
			{
			return (ShipClass.m_Character.GetUNID() ? ICCItemPtr(ShipClass.m_Character.GetUNID()) : ICCItemPtr::Nil());
			},
		NULL,
		},

		{
		"compartmentRegen",		"Regeneration level for compartments",
		[](const CShipClass &ShipClass, const CString &sProperty) 
			{
			for (int i = 0; i < ShipClass.m_AverageDevices.GetCount(); i++)
				{
				const CDeviceItem DeviceItem = ShipClass.m_AverageDevices.GetDeviceItem(i);
				ICCItemPtr pResult = DeviceItem.GetProperty(CONSTLIT("compartmentRegen"));
				if (!pResult->IsNil())
					return pResult;
				}

			return ICCItemPtr::Nil();
			},
		NULL,
		},

		{
		"maneuver",				"Max rotation (degrees/second)",
		[](const CShipClass &ShipClass, const CString &sProperty) 
			{
			Metric rManeuver = g_SecondsPerUpdate * ShipClass.GetIntegralRotationDesc().GetMaxRotationSpeedDegrees();
			return ICCItemPtr(rManeuver);
			},
		NULL,
		},

		{
		"maxAcceleration",		"Maximum acceleration (klicks/second)",
		[](const CShipClass &ShipClass, const CString &sProperty) 
			{
			Metric rMass = ShipClass.GetHullDesc().GetMass();
			if (rMass <= 0.0)
				return ICCItemPtr::Nil();

			return ICCItemPtr(ShipClass.m_Perf.GetDriveDesc().GetThrust() * 1000.0 / rMass);
			},
		NULL,
		},

	}};

ICCItemPtr CShipClass::OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	OnGetProperty
//
//	Returns a property of the ship class

	{
	CCodeChain &CC = GetUniverse().GetCC();
	ICCItemPtr pValue;

	if (m_PropertyTable.FindProperty(*this, sProperty, pValue))
		return pValue;

	else if (strEquals(sProperty, PROPERTY_ARMOR_ITEM))
		{
		if (m_Armor.GetCount() == 0)
			return ICCItemPtr(ICCItem::Nil);
		
		return ICCItemPtr(CreateListFromItem(m_Armor.GetSegment(0).GetArmorItem()));
		}
	else if (strEquals(sProperty, PROPERTY_ARMOR_SPEED_ADJ))
		{
		TArray<CItemCtx> Armor;
		Armor.InsertEmpty(m_Armor.GetCount());
		for (int i = 0; i < Armor.GetCount(); i++)
			{
			CItem ArmorItem(m_Armor.GetSegment(i).GetArmorClass()->GetItemType(), 1);
			if (m_Hull.GetArmorLimits().CanInstallArmor(ArmorItem) != CArmorLimits::resultOK)
				return ICCItemPtr(CONSTLIT("[Armor incompatible]"));

			Armor[i] = CItemCtx(ArmorItem);
			}

		int iBonus = m_Hull.GetArmorLimits().CalcArmorSpeedBonus(Armor);
		if (iBonus)
			return ICCItemPtr(iBonus);
		else
			return ICCItemPtr(ICCItem::Nil);
		}

	else if (strStartsWith(sProperty, PROPERTY_ARMOR_SPEED_ADJ_PARAM))
		{
		const CArmorLimits &ArmorLimits = m_Hull.GetArmorLimits();
		if (!ArmorLimits.HasArmorLimits())
			return ICCItemPtr(ICCItem::Nil);

		CString sArmorClassID = strSubString(sProperty, PROPERTY_ARMOR_SPEED_ADJ_PARAM.GetLength());

		int iSpeedAdj;
		if (!m_Hull.GetArmorLimits().CalcArmorSpeedBonus(sArmorClassID, m_Armor.GetCount(), &iSpeedAdj))
			return ICCItemPtr(CONSTLIT("[Armor incompatible]"));

		return ICCItemPtr(iSpeedAdj);
		}

	else if (strEquals(sProperty, PROPERTY_CHARACTER_NAME))
		return (m_Character.GetUNID() ? m_Character->GetStaticData(CONSTLIT("Name")) : ICCItemPtr::Nil());
		
	else if (strEquals(sProperty, PROPERTY_CURRENCY))
		return ICCItemPtr(GetEconomyType()->GetUNID());
		
	else if (strEquals(sProperty, PROPERTY_CURRENCY_NAME))
		return ICCItemPtr(GetEconomyType()->GetSID());
		
	else if (strEquals(sProperty, PROPERTY_CYBER_DEFENSE_LEVEL))
		return ICCItemPtr(CProgramDesc::CalcLevel(m_Hull.GetCyberDefenseLevel(), m_Perf.GetCyberDefenseAdj()));

	else if (strEquals(sProperty, PROPERTY_DEFAULT_SOVEREIGN))
		return (m_pDefaultSovereign.GetUNID() ? ICCItemPtr(m_pDefaultSovereign.GetUNID()) : ICCItemPtr(ICCItem::Nil));

	else if (strEquals(sProperty, PROPERTY_FREQUENCY))
		return ICCItemPtr(GetFrequencyName(GetFrequency()));

	else if (strEquals(sProperty, PROPERTY_FREQUENCY_RATIO))
		return ICCItemPtr((double)(int)GetFrequency() / (double)ftCommon);

	else if (strEquals(sProperty, PROPERTY_FUEL_EFFICIENCY))
		return ICCItemPtr(mathRound(CalcFuelEfficiency(m_AverageDevices)));

	else if (strEquals(sProperty, PROPERTY_HAS_TRADE_DESC))
		return ICCItemPtr(m_pTrade != NULL);

	else if (strEquals(sProperty, PROPERTY_HAS_VARIANTS))
		return ICCItemPtr(m_pDevices && m_pDevices->IsVariant());

	else if (strEquals(sProperty, PROPERTY_HULL_CARGO_SPACE))
		return ICCItemPtr(m_Hull.GetCargoSpace());

	else if (strEquals(sProperty, PROPERTY_HULL_POINTS))
		{
		CHullPointsCalculator Calc(*this, GetStandard());
		return ICCItemPtr(mathRound(Calc.GetTotalPoints() * 10.0));
		}

	else if (strEquals(sProperty, PROPERTY_IMAGE_ROTATION_COUNT))
		{
		return ICCItemPtr(m_Image.GetSimpleImage().GetRotationCount());
		}

	else if (strEquals(sProperty, PROPERTY_LAUNCHER_ITEM))
		{
		const SDeviceDesc *pDesc = m_AverageDevices.GetDeviceDescByName(devMissileWeapon);
		if (pDesc == NULL)
			return ICCItemPtr(ICCItem::Nil);

		return ICCItemPtr(CreateListFromItem(pDesc->Item));
		}

	else if (strEquals(sProperty, PROPERTY_HULL_VALUE))
		return CTLispConvert::CreateCurrencyValue(GetEconomyType()->Exchange(m_Hull.GetValue()));

	else if (strEquals(sProperty, PROPERTY_MAX_ARMOR_CLASS))
		return (!m_Hull.GetArmorLimits().GetMaxArmorClass().IsBlank() ? ICCItemPtr(m_Hull.GetArmorLimits().GetMaxArmorClass()) : ICCItemPtr(ICCItem::Nil));

	else if (strEquals(sProperty, PROPERTY_MAX_ARMOR_CLASS_NAME))
		{
		const CString &sMassClass = m_Hull.GetArmorLimits().GetMaxArmorClass();
		if (!sMassClass.IsBlank())
			{
			const CArmorMassDefinitions &MassDef = GetUniverse().GetDesignCollection().GetArmorMassDefinitions();
			return ICCItemPtr(MassDef.GetMassClassLabel(sMassClass));
			}
		else if (m_Hull.GetArmorLimits().GetMaxArmorMass() > 0) 
			return ICCItemPtr(CLanguage::ComposeNumber(CLanguage::numberMass, m_Hull.GetArmorLimits().GetMaxArmorMass()));
		else
			return ICCItemPtr();
		}

	else if (strEquals(sProperty, PROPERTY_MAX_ARMOR_MASS))
		return (m_Hull.GetArmorLimits().GetMaxArmorMass() > 0 ? ICCItemPtr(m_Hull.GetArmorLimits().GetMaxArmorMass()) : ICCItemPtr(ICCItem::Nil));

	else if (strEquals(sProperty, PROPERTY_MAX_DEVICES))
		return ICCItemPtr(m_Hull.GetMaxDevices());

	else if (strEquals(sProperty, PROPERTY_MAX_SPEED_AT_MAX_ARMOR))
		return ICCItemPtr(m_Perf.GetDriveDesc().GetMaxSpeedFrac() + m_Hull.GetArmorLimits().GetMaxArmorSpeedPenalty());

	else if (strEquals(sProperty, PROPERTY_MAX_SPEED_AT_MIN_ARMOR))
		return ICCItemPtr(m_Perf.GetDriveDesc().GetMaxSpeedFrac() + m_Hull.GetArmorLimits().GetMinArmorSpeedBonus());

	else if (strEquals(sProperty, PROPERTY_MAX_SPEED_BY_ARMOR_MASS))
		return CalcMaxSpeedByArmorMass(Ctx);

	else if (strEquals(sProperty, PROPERTY_MISC_DEVICE_ITEMS))
		{
		ICCItemPtr pResult(ICCItem::List);
		for (int i = 0; i < m_AverageDevices.GetCount(); i++)
			{
			const SDeviceDesc &Desc = m_AverageDevices.GetDeviceDesc(i);
			if (Desc.Item.GetType()->GetCategory() == itemcatMiscDevice)
				{
				ICCItemPtr pItem(CreateListFromItem(Desc.Item));
				pResult->Append(pItem);
				}
			}

		return pResult;
		}

	else if (strEquals(sProperty, PROPERTY_PERCEPTION))
		return ICCItemPtr(Max((int)CSpaceObject::perceptMin, Min(GetAISettings().GetPerception() + m_Perf.GetPerceptionAdj(), (int)CSpaceObject::perceptMax)));

	else if (strEquals(sProperty, PROPERTY_PRICE))
		return ICCItemPtr((int)GetTradePrice(NULL, true).GetValue());

	else if (strEquals(sProperty, PROPERTY_RATED_POWER))
		return ICCItemPtr(CalcRatedPowerUse(m_AverageDevices));

	else if (strEquals(sProperty, PROPERTY_SHIELD_ITEM))
		{
		const SDeviceDesc *pDesc = m_AverageDevices.GetDeviceDescByName(devShields);
		if (pDesc == NULL)
			return ICCItemPtr(ICCItem::Nil);

		return ICCItemPtr(CreateListFromItem(pDesc->Item));
		}

	else if (strEquals(sProperty, PROPERTY_STD_ARMOR_CLASS))
		return (!m_Hull.GetArmorLimits().GetStdArmorClass().IsBlank() ? ICCItemPtr(m_Hull.GetArmorLimits().GetStdArmorClass()) : ICCItemPtr(ICCItem::Nil));

	else if (strEquals(sProperty, PROPERTY_STD_ARMOR_CLASS_NAME))
		{
		const CString &sMassClass = m_Hull.GetArmorLimits().GetStdArmorClass();
		if (!sMassClass.IsBlank())
			{
			const CArmorMassDefinitions &MassDef = GetUniverse().GetDesignCollection().GetArmorMassDefinitions();
			return ICCItemPtr(MassDef.GetMassClassLabel(sMassClass));
			}
		else if (m_Hull.GetArmorLimits().GetStdArmorMass() > 0) 
			return ICCItemPtr(CLanguage::ComposeNumber(CLanguage::numberMass, m_Hull.GetArmorLimits().GetStdArmorMass()));
		else
			return ICCItemPtr();
		}

	else if (strEquals(sProperty, PROPERTY_STD_ARMOR_MASS))
		return (m_Hull.GetArmorLimits().GetStdArmorMass() > 0 ? ICCItemPtr(m_Hull.GetArmorLimits().GetStdArmorMass()) : ICCItemPtr(ICCItem::Nil));

	else if (strEquals(sProperty, PROPERTY_STEALTH))
		return ICCItemPtr(m_Perf.GetStealth());

	else if (strEquals(sProperty, PROPERTY_WRECK_HAS_ITEMS))
		return ICCItemPtr(m_WreckDesc.AreItemsAddedToWreck());

	else if (strEquals(sProperty, PROPERTY_WRECK_HAS_INSTALLED_ITEMS))
		return ICCItemPtr(m_WreckDesc.AreInstalledItemsAddedToWreck());

	else if (strEquals(sProperty, PROPERTY_WRECK_STRUCTURAL_HP))
		return ICCItemPtr(GetMaxStructuralHitPoints());

	else if (strEquals(sProperty, PROPERTY_WRECK_TYPE))
		return (GetWreckDesc().GetWreckType() ? ICCItemPtr(GetWreckDesc().GetWreckType()->GetUNID()) : ICCItemPtr::Nil());

	else if (strEquals(sProperty, PROPERTY_WRECK_TYPE_NAME))
		return (GetWreckDesc().GetWreckType() ? ICCItemPtr(GetWreckDesc().GetWreckType()->GetNounPhrase()) : ICCItemPtr::Nil());

	//	Drive properties

	else if (strEquals(sProperty, PROPERTY_THRUST_TO_WEIGHT))
		{
		Metric rMass = CalcMass(m_AverageDevices);
		int iRatio = mathRound((200.0 * (rMass > 0.0 ? m_Perf.GetDriveDesc().GetThrust() / rMass : 0.0)));
		return ICCItemPtr(10 * iRatio);
		}
	else if (strEquals(sProperty, PROPERTY_THRUST_RATIO))
		{
		Metric rMass = CalcMass(m_AverageDevices);
		Metric rRatio = 2.0 * (rMass > 0.0 ? m_Perf.GetDriveDesc().GetThrust() / rMass : 0.0);
		return ICCItemPtr((double)mathRound(rRatio * 10.0) / 10.0);
		}
	else if (strEquals(sProperty, PROPERTY_VIEWPORT_SIZE))
		return ICCItemPtr(GetImageViewportSize());

	else if (strEquals(sProperty, PROPERTY_WEAPON_ITEMS))
		{
		//	First make a list of all weapons and organize by type
		//	(so that weapons of the same type are bundled together).

		TSortMap<CString, CItem> Weapons;
		for (int i = 0; i < m_AverageDevices.GetCount(); i++)
			{
			const SDeviceDesc &Desc = m_AverageDevices.GetDeviceDesc(i);
			if (Desc.Item.GetType()->GetCategory() == itemcatWeapon)
				{
				bool bNew;
				CString sSort = strPatternSubst(CONSTLIT("%02d-%08x"), MAX_ITEM_LEVEL - Desc.Item.GetLevel(), Desc.Item.GetType()->GetUNID());
				CItem *pWeapon = Weapons.SetAt(sSort, &bNew);
				if (bNew)
					*pWeapon = Desc.Item;
				else
					pWeapon->SetCount(pWeapon->GetCount() + 1);
				}
			}

		//	Now generate the list

		ICCItemPtr pResult(ICCItem::List);
		for (int i = 0; i < Weapons.GetCount(); i++)
			{
			ICCItemPtr pWeapon(CreateListFromItem(Weapons[i]));
			pResult->Append(pWeapon);
			}

		return pResult;
		}
	else if (strEquals(sProperty, PROPERTY_THRUSTER_POWER))
		{
		const CObjectEffectDesc &Effects = GetEffectsDesc();
		int iThrustersPerSide = Max(1, Effects.GetEffectCount(CObjectEffectDesc::effectThrustLeft));
		int iThrusterPower = Max(1, mathRound(((double)m_Hull.GetMass() / iThrustersPerSide) * m_RotationDesc.GetRotationAccelPerTick()));
		return ICCItemPtr(iThrusterPower);
		}
	else if (strEquals(sProperty, PROPERTY_TREASURE_ITEM_NAMES))
		{
		if (m_pItems == NULL)
			return ICCItemPtr(ICCItem::Nil);

		//	Generate sample treasure

		CItemList SampleItems;
		CItemListManipulator SampleList(SampleItems);
		SItemAddCtx AddCtx(SampleList);
		AddCtx.iLevel = GetLevel();

		m_pItems->AddItems(AddCtx);

		//	Output as a list of item names.

		TArray<CString> Items;
		Items.InsertEmpty(SampleItems.GetCount());
		for (int i = 0; i < SampleItems.GetCount(); i++)
			{
			Items[i] = SampleItems.GetItem(i).GetNounPhrase();
			}

		return ICCItemPtr(strJoin(Items, CONSTLIT("oxfordComma")));
		}

	else if (strEquals(sProperty, PROPERTY_DRIVE_POWER)
			|| strEquals(sProperty, PROPERTY_MAX_SPEED)
			|| strEquals(sProperty, PROPERTY_THRUST))
		return ICCItemPtr(CDriveClass::GetDriveProperty(m_Perf.GetDriveDesc(), sProperty));

	//	Reactor properties

	else if (CReactorDesc::IsExportedProperty(sProperty))
		return ICCItemPtr(m_Perf.GetReactorDesc().FindProperty(sProperty));

	else
		return NULL;
	}


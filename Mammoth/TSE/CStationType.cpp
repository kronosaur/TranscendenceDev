//	CStationType.cpp
//
//	CStationType class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ANIMATIONS_TAG							CONSTLIT("Animations")
#define ASTEROID_TAG							CONSTLIT("Asteroid")
#define COMMUNICATIONS_TAG						CONSTLIT("Communications")
#define COMPOSITE_TAG							CONSTLIT("ImageComposite")
#define CONSTRUCTION_TAG						CONSTLIT("Construction")
#define DEVICES_TAG								CONSTLIT("Devices")
#define DOCKING_PORTS_TAG						CONSTLIT("DockingPorts")
#define DOCK_SCREENS_TAG						CONSTLIT("DockScreens")
#define ENCOUNTER_GROUP_TAG						CONSTLIT("EncounterGroup")
#define ENCOUNTER_TYPE_TAG						CONSTLIT("EncounterType")
#define ENCOUNTERS_TAG							CONSTLIT("Encounters")
#define EVENTS_TAG								CONSTLIT("Events")
#define HERO_IMAGE_TAG							CONSTLIT("HeroImage")
#define IMAGE_TAG								CONSTLIT("Image")
#define IMAGE_EFFECT_TAG						CONSTLIT("ImageEffect")
#define IMAGE_LOOKUP_TAG						CONSTLIT("ImageLookup")
#define IMAGE_SHIPWRECK_TAG						CONSTLIT("ImageShipwreck")
#define IMAGE_VARIANTS_TAG						CONSTLIT("ImageVariants")
#define INITIAL_DATA_TAG						CONSTLIT("InitialData")
#define ITEMS_TAG								CONSTLIT("Items")
#define NAMES_TAG								CONSTLIT("Names")
#define REINFORCEMENTS_TAG						CONSTLIT("Reinforcements")
#define SATELLITES_TAG							CONSTLIT("Satellites")
#define SHIP_TAG								CONSTLIT("Ship")
#define SHIPS_TAG								CONSTLIT("Ships")
#define STATION_TAG								CONSTLIT("Station")
#define TABLE_TAG								CONSTLIT("Table")
#define TRADE_TAG								CONSTLIT("Trade")

#define ABANDONED_SCREEN_ATTRIB					CONSTLIT("abandonedScreen")
#define ALERT_WHEN_ATTACKED_ATTRIB				CONSTLIT("alertWhenAttacked")
#define ALERT_WHEN_DESTROYED_ATTRIB				CONSTLIT("alertWhenDestroyed")
#define ALLOW_ENEMY_DOCKING_ATTRIB				CONSTLIT("allowEnemyDocking")
#define ANONYMOUS_ATTRIB						CONSTLIT("anonymous")
#define BACKGROUND_PLANE_ATTRIB					CONSTLIT("backgroundPlane")
#define BARRIER_EFFECT_ATTRIB					CONSTLIT("barrierEffect")
#define BEACON_ATTRIB							CONSTLIT("beacon")
#define BUILD_REINFORCEMENTS_ATTRIB				CONSTLIT("buildReinforcements")
#define CAN_ATTACK_ATTRIB						CONSTLIT("canAttack")
#define CHALLENGE_ATTRIB						CONSTLIT("challenge")
#define CHALLENGE_RATING_ATTRIB					CONSTLIT("challengeRating")
#define CHANCE_ATTRIB							CONSTLIT("chance")
#define CONSTRUCTION_RATE_ATTRIB				CONSTLIT("constructionRate")
#define CONTROLLING_SOVEREIGN_ATTRIB			CONSTLIT("controllingSovereign")
#define COUNT_ATTRIB							CONSTLIT("count")
#define DEFAULT_BACKGROUND_ID_ATTRIB			CONSTLIT("defaultBackgroundID")
#define DESTROY_WHEN_ABANDONED_ATTRIB			CONSTLIT("destroyWhenAbandoned")
#define DESTROY_WHEN_EMPTY_ATTRIB				CONSTLIT("destroyWhenEmpty")
#define DOCK_SCREEN_ATTRIB						CONSTLIT("dockScreen")
#define DOCKING_PORTS_ATTRIB					CONSTLIT("dockingPorts")
#define ENEMY_EXCLUSION_RADIUS_ATTRIB			CONSTLIT("enemyExclusionRadius")
#define EJECTA_ADJ_ATTRIB						CONSTLIT("ejectaAdj")
#define EJECTA_TYPE_ATTRIB						CONSTLIT("ejectaType")
#define EXPLOSION_TYPE_ATTRIB					CONSTLIT("explosionType")
#define FIRE_RATE_ADJ_ATTRIB					CONSTLIT("fireRateAdj")
#define FREQUENCY_ATTRIB						CONSTLIT("frequency")
#define HIT_POINTS_ATTRIB						CONSTLIT("hitPoints")
#define IGNORE_FRIENDLY_FIRE_ATTRIB				CONSTLIT("ignoreFriendlyFire")
#define IMAGE_VARIANT_ATTRIB					CONSTLIT("imageVariant")
#define IMMUTABLE_ATTRIB						CONSTLIT("immutable")
#define INACTIVE_ATTRIB							CONSTLIT("inactive")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define LEVEL_FREQUENCY_ATTRIB					CONSTLIT("levelFrequency")
#define LOCATION_CRITERIA_ATTRIB				CONSTLIT("locationCriteria")
#define MASS_ATTRIB								CONSTLIT("mass")
#define MAX_CONSTRUCTION_ATTRIB					CONSTLIT("maxConstruction")
#define MAX_HIT_POINTS_ATTRIB					CONSTLIT("maxHitPoints")
#define MAX_STRUCTURAL_HIT_POINTS_ATTRIB		CONSTLIT("maxStructuralHitPoints")
#define MIN_SHIPS_ATTRIB						CONSTLIT("minShips")
#define MOBILE_ATTRIB							CONSTLIT("mobile")
#define MULTI_HULL_ATTRIB						CONSTLIT("multiHull")
#define NAME_ATTRIB								CONSTLIT("name")
#define NO_BLACKLIST_ATTRIB						CONSTLIT("noBlacklist")
#define NO_FRIENDLY_FIRE_ATTRIB					CONSTLIT("noFriendlyFire")
#define NO_FRIENDLY_TARGET_ATTRIB				CONSTLIT("noFriendlyTarget")
#define NO_INDEPENDENT_ATTACK_ATTRIB			CONSTLIT("noIndependentAttack")
#define NO_MAP_DETAILS_ATTRIB					CONSTLIT("noMapDetails")
#define NO_MAP_ICON_ATTRIB						CONSTLIT("noMapIcon")
#define NO_MAP_LABEL_ATTRIB						CONSTLIT("noMapLabel")
#define PAINT_LAYER_ATTRIB						CONSTLIT("paintLayer")
#define RADIOACTIVE_ATTRIB						CONSTLIT("radioactive")
#define RANDOM_ENCOUNTERS_ATTRIB				CONSTLIT("randomEncounters")
#define REGEN_ATTRIB							CONSTLIT("regen")
#define REPAIR_RATE_ATTRIB						CONSTLIT("repairRate")
#define REVERSE_ARTICLE_ATTRIB					CONSTLIT("reverseArticle")
#define SCALE_ATTRIB							CONSTLIT("scale")
#define SEGMENT_ATTRIB							CONSTLIT("segment")
#define SHIP_ENCOUNTER_ATTRIB					CONSTLIT("shipEncounter")
#define SHIP_REGEN_ATTRIB						CONSTLIT("shipRegen")
#define SHIP_REPAIR_RATE_ATTRIB					CONSTLIT("shipRepairRate")
#define SHIPWRECK_UNID_ATTRIB					CONSTLIT("shipwreckID")
#define SHOW_MAP_LABEL_ATTRIB					CONSTLIT("showMapLabel")
#define SIGN_ATTRIB								CONSTLIT("sign")
#define SIZE_ATTRIB								CONSTLIT("size")
#define SOVEREIGN_ATTRIB						CONSTLIT("sovereign")
#define STANDING_COUNT_ATTRIB					CONSTLIT("standingCount")
#define STEALTH_ATTRIB							CONSTLIT("stealth")
#define STRUCTURAL_HIT_POINTS_ATTRIB			CONSTLIT("structuralHitPoints")
#define TIME_STOP_IMMUNE_ATTRIB					CONSTLIT("timeStopImmune")
#define TYPE_ATTRIB								CONSTLIT("type")
#define UNID_ATTRIB								CONSTLIT("UNID")
#define UNIQUE_ATTRIB							CONSTLIT("unique")
#define VIRTUAL_ATTRIB							CONSTLIT("virtual")
#define WALL_ATTRIB								CONSTLIT("barrier")
#define X_ATTRIB								CONSTLIT("x")
#define X_OFFSET_ATTRIB							CONSTLIT("xOffset")
#define Y_ATTRIB								CONSTLIT("y")
#define Y_OFFSET_ATTRIB							CONSTLIT("yOffset")

#define LARGE_DAMAGE_IMAGE_ID_ATTRIB			CONSTLIT("largeDamageImageID")
#define LARGE_DAMAGE_WIDTH_ATTRIB				CONSTLIT("largeDamageWidth")
#define LARGE_DAMAGE_HEIGHT_ATTRIB				CONSTLIT("largeDamageHeight")
#define LARGE_DAMAGE_COUNT_ATTRIB				CONSTLIT("largeDamageCount")
#define MEDIUM_DAMAGE_IMAGE_ID_ATTRIB			CONSTLIT("mediumDamageImageID")
#define MEDIUM_DAMAGE_WIDTH_ATTRIB				CONSTLIT("mediumDamageWidth")
#define MEDIUM_DAMAGE_HEIGHT_ATTRIB				CONSTLIT("mediumDamageHeight")
#define MEDIUM_DAMAGE_COUNT_ATTRIB				CONSTLIT("mediumDamageCount")

#define STAR_SCALE								CONSTLIT("star")
#define WORLD_SCALE								CONSTLIT("world")
#define STRUCTURE_SCALE							CONSTLIT("structure")
#define SHIP_SCALE								CONSTLIT("ship")
#define FLOTSAM_SCALE							CONSTLIT("flotsam")

#define UNIQUE_IN_SYSTEM						CONSTLIT("inSystem")
#define UNIQUE_IN_UNIVERSE						CONSTLIT("inUniverse")

#define EVENT_ON_MINING							CONSTLIT("OnMining")

#define FIELD_ABANDONED_DOCK_SCREEN				CONSTLIT("abandonedDockScreen")
#define FIELD_ARMOR_CLASS						CONSTLIT("armorClass")
#define FIELD_ARMOR_LEVEL						CONSTLIT("armorLevel")
#define FIELD_BALANCE							CONSTLIT("balance")
#define FIELD_CAN_ATTACK						CONSTLIT("canAttack")
#define FIELD_CATEGORY							CONSTLIT("category")
#define FIELD_DEFENDER_STRENGTH					CONSTLIT("defenderStrength")
#define FIELD_DOCK_SCREEN						CONSTLIT("dockScreen")
#define FIELD_EXPLOSION_TYPE					CONSTLIT("explosionType")
#define FIELD_FIRE_RATE_ADJ						CONSTLIT("fireRateAdj")
#define FIELD_HITS_TO_DESTROY					CONSTLIT("hitsToDestroy")			//	# of hits by std level weapon to destroy station
#define FIELD_INSTALL_DEVICE_MAX_LEVEL			CONSTLIT("installDeviceMaxLevel")
#define FIELD_HP								CONSTLIT("hp")
#define FIELD_LEVEL								CONSTLIT("level")
#define FIELD_LOCATION_CRITERIA					CONSTLIT("locationCriteria")
#define FIELD_MAX_LIGHT_RADIUS					CONSTLIT("maxLightRadius")
#define FIELD_NAME								CONSTLIT("name")
#define FIELD_REGEN								CONSTLIT("regen")					//	hp repaired per 180 ticks
#define FIELD_SATELLITE_STRENGTH				CONSTLIT("satelliteStrength")
#define FIELD_SIZE								CONSTLIT("size")
#define FIELD_TREASURE_BALANCE					CONSTLIT("treasureBalance")			//	100 = treasure appropriate for defenses, 200 = twice as much treasure
#define FIELD_TREASURE_VALUE					CONSTLIT("treasureValue")
#define FIELD_WEAPON_STRENGTH					CONSTLIT("weaponStrength")			//	Strength of weapons (100 = level weapon @ 1/4 fire rate).

#define PROPERTY_ASTEROID_TYPE					CONSTLIT("asteroidType")
#define PROPERTY_AUTO_LEVEL_FREQUENCY			CONSTLIT("autoLevelFrequency")
#define PROPERTY_CAN_BE_MINED					CONSTLIT("canBeMined")
#define PROPERTY_CHALLENGE_RATING				CONSTLIT("challengeRating")
#define PROPERTY_CURRENCY						CONSTLIT("currency")
#define PROPERTY_CURRENCY_NAME					CONSTLIT("currencyName")
#define PROPERTY_ENCOUNTERED_BY_NODE			CONSTLIT("encounteredByNode")
#define PROPERTY_ENCOUNTERED_TOTAL				CONSTLIT("encounteredTotal")
#define PROPERTY_HULL_TYPE						CONSTLIT("hullType")
#define PROPERTY_LEVEL_FREQUENCY				CONSTLIT("levelFrequency")
#define PROPERTY_MAX_BALANCE					CONSTLIT("maxBalance")
#define PROPERTY_NAME							CONSTLIT("name")
#define PROPERTY_PRIMARY_WEAPON					CONSTLIT("primaryWeapon")
#define PROPERTY_PRIMARY_WEAPON_LEVEL			CONSTLIT("primaryWeaponLevel")
#define PROPERTY_SHOWS_UNEXPLORED_ANNOTATION	CONSTLIT("showsUnexploredAnnotation")
#define PROPERTY_SOVEREIGN						CONSTLIT("sovereign")
#define PROPERTY_SOVEREIGN_NAME					CONSTLIT("sovereignName")
#define PROPERTY_STD_TREASURE					CONSTLIT("stdTreasure")
#define PROPERTY_SYSTEM_CRITERIA				CONSTLIT("systemCriteria")
#define PROPERTY_TREASURE_DESIRED_VALUE			CONSTLIT("treasureDesiredValue")
#define PROPERTY_TREASURE_DESIRED_VALUE_LOOP_COUNT	CONSTLIT("treasureDesiredValueLoopCount")
#define PROPERTY_TREASURE_DESIRED_VALUE_SCALE		CONSTLIT("treasureDesiredValueScale")

#define VALUE_FALSE								CONSTLIT("false")
#define VALUE_SHIPWRECK							CONSTLIT("shipwreck")
#define VALUE_TRUE								CONSTLIT("true")

#define MAX_ATTACK_DISTANCE						(g_KlicksPerPixel * 512)

#define SPECIAL_IS_ENEMY_OF						CONSTLIT("isEnemyOf:")
#define SPECIAL_IS_SHIP_ENCOUNTER				CONSTLIT("isShipEncounter:")
#define SPECIAL_IS_STATION_ENCOUNTER			CONSTLIT("isStationEncounter:")
#define SPECIAL_SCALE							CONSTLIT("scale:")
#define SPECIAL_SIZE_CLASS						CONSTLIT("sizeClass:")

#define SPECIAL_VALUE_TRUE						CONSTLIT("true")

#define ON_CREATE_EVENT							CONSTLIT("OnCreate")

static constexpr Metric TREASURE_BALANCE_POWER =			0.7;

struct SSizeData
	{
	int iMinSize;
	int iMaxSize;
	};

//	NOTE: These must match the values of ESizeClass

static SSizeData SIZE_DATA[] = 
	{
		{	0,			0,	},

		{	1,			24,	},
		{	25,			74,	},
		{	75,			299,	},
		{	300,		749,	},

		{	750,		1499,	},
		{	1500,		2999,	},
		{	3000,		4499,	},

		{	4500,		7499,	},
		{	7500,		14999,	},
		{	15000,		29999,	},

		{	30000,		74999,	},
		{	75000,		149999,	},
		{	150000,		1000000,	},
	};

//	Standard station table

struct SStdStationDesc
	{
	Metric rChallenge = 0.0;					//	Std challenge rating at this level
	CurrencyValue dwTreasureValue = 0;			//	Std value of treasure at this level
	};

//	STD_STATION_DATA
//
//	Challenge is computed as std treasure value / 375.0.

static const SStdStationDesc STD_STATION_DATA[] =
	{
		{	         0.0,	            0,	},

		{	         2.0,	          750,	},
		{	         2.7,	        1'000,	},
		{	         4.0,	        1'500,	},
		{	         5.3,	        2'000,	},
		{	        10.7,	        4'000,	},

		{	        21.3,	        8'000,	},
		{	        42.7,	       16'000,	},
		{	        85.3,	       32'000,	},
		{	       171,		       64'000,	},
		{	       341,		      128'000,	},

		{	       683,		      256'000,	},
		{	     1'365,		      512'000,	},
		{	     2'667,		    1'000'000,	},
		{	     5'333,		    2'000'000,	},
		{	    10'933,		    4'100'000,	},

		{	    21'867,		    8'200'000,	},
		{	    43'733,		   16'400'000,	},
		{	    87'467,		   32'800'000,	},
		{	   174'667,		   65'500'000,	},
		{	   349'333,		  131'000'000,	},

		{	   698'667,		  262'000'000,	},
		{	 1'397'333,		  524'000'000,	},
		{	 2'666'667,		1'000'000'000,	},
		{	 5'600'000,		2'100'000'000,	},
		{	11'200'000,		4'200'000'000,	},
	};

CIntegralRotationDesc CStationType::m_DefaultRotation;

CStationType::CStationType (void)

//	CStationType constructor

	{
	}

CStationType::~CStationType (void)

//	CStationType destructor

	{
	if (m_pDevices)
		delete m_pDevices;

	if (m_pItems)
		delete m_pItems;

	if (m_pInitialShips)
		delete m_pInitialShips;

	if (m_pReinforcements)
		delete m_pReinforcements;

	if (m_pConstruction)
		delete m_pConstruction;

	if (m_pEncounters)
		delete m_pEncounters;

	if (m_pAnimations)
		delete [] m_pAnimations;

	if (m_pTrade)
		delete m_pTrade;
	}

void CStationType::AddTypesUsedByXML (CXMLElement *pElement, TSortMap<DWORD, bool> *retTypesUsed)

//	AddTypesUsedByXML
//
//	Add type used by the <Station> XML element (recursively)

	{
	int i;

	if (strEquals(pElement->GetTag(), STATION_TAG))
		retTypesUsed->SetAt(pElement->GetAttributeInteger(TYPE_ATTRIB), true);

	for (i = 0; i < pElement->GetContentElementCount(); i++)
		AddTypesUsedByXML(pElement->GetContentElement(i), retTypesUsed);
	}

Metric CStationType::CalcBalance (void) const

//	CalcBalance
//
//	Calculates the station defense balance assuming the station is at the given level.

	{
	//	Compute balance, if necessary

	if (!m_fBalanceValid)
		{
		int iLevel = GetLevel();
		if (iLevel <= 0 || iLevel > MAX_TECH_LEVEL)
			return 0.0;

		m_rCombatBalance = GetLevelStrength(iLevel);
		}

	return m_rCombatBalance;
	}

Metric CStationType::CalcBalanceHitsAdj (int iLevel) const

//	CalcBalanceHitsAdj
//
//	Calculate adjustments to balance based on how many hits the station can
//	withstand.

	{
	static constexpr Metric BALANCE_STD_HITS_TO_DESTROY = 25.0;
	static constexpr Metric MAX_HITS_ADJ = 2.0;

	return Min(MAX_HITS_ADJ, sqrt((Metric)CalcHitsToDestroy(iLevel) / BALANCE_STD_HITS_TO_DESTROY));
	}

int CStationType::CalcChallengeRating (const int iLevel, const Metric rBalance)

//	CalcChallengeRating
//
//	Computes the station challenge rating based on level and balance (1.0 =
//	balanced).

	{
	static constexpr Metric CHALLENGE_RATING_K1 = 1.25;

	if (iLevel < 1 || iLevel > MAX_SYSTEM_LEVEL)
		return 0;

	Metric rChallenge = pow(rBalance, TREASURE_BALANCE_POWER) * STD_STATION_DATA[iLevel].rChallenge;
	return Max(1, mathRound(mathLog(rChallenge, CHALLENGE_RATING_K1)));
	}

Metric CStationType::CalcDefenderStrength (int iLevel) const

//	CalcDefenderStrength
//
//	Returns the strength of station defenders
//	1.0 = 1 defender of station level.

	{
	Metric rTotal = 0.0;

	//	Add ship defenders

	if (m_pInitialShips)
		rTotal += m_pInitialShips->GetAverageLevelStrength(iLevel);

	//	Add satellite defenses

	if (m_pSatellitesDesc)
		rTotal += CalcSatelliteStrength(m_pSatellitesDesc, iLevel);

	//	Done

	return rTotal;
	}

int CStationType::CalcHitsToDestroy (int iLevel) const

//	CalcHitsToDestroy
//
//	Returns the number of hits that it would take to destroy the station when 
//	hit by a standard weapon of the given level.

	{
	Metric rTotalHits = m_HullDesc.CalcHitsToDestroy(iLevel);

	//	Add hits to destroy satellites

	if (m_pSatellitesDesc)
		rTotalHits += CalcSatelliteHitsToDestroy(m_pSatellitesDesc, iLevel);

	//	DOne

	return (int)rTotalHits;
	}

Metric CStationType::CalcSatelliteHitsToDestroy (CXMLElement *pSatellites, int iLevel, bool bIgnoreChance)

//	CalcSatelliteHitsToDestroy
//
//	Calculate the total hits to destroy all satellites

	{
	int i;

	const CString &sTag = pSatellites->GetTag();

	int iChance;
	Metric rChanceAdj;
	if (!bIgnoreChance
			&& pSatellites->FindAttributeInteger(CHANCE_ATTRIB, &iChance))
		rChanceAdj = (Metric)iChance / 100.0;
	else
		rChanceAdj = 1.0;

	Metric rTotalHits = 0.0;
	if (strEquals(sTag, SHIP_TAG))
		{
		//	Skip ships
		}
	else if (strEquals(sTag, STATION_TAG))
		{
		CStationType *pStationType = g_pUniverse->FindStationType((DWORD)pSatellites->GetAttributeInteger(TYPE_ATTRIB));
		if (pStationType == NULL || (!pStationType->CanAttack() && !pSatellites->GetAttributeBool(SEGMENT_ATTRIB)))
			return 0.0;

		rTotalHits += (rChanceAdj * pStationType->CalcHitsToDestroy(iLevel));
		}
	else if (strEquals(sTag, TABLE_TAG))
		{
		int iTotalChance = 0;
		for (i = 0; i < pSatellites->GetContentElementCount(); i++)
			iTotalChance += pSatellites->GetContentElement(i)->GetAttributeInteger(CHANCE_ATTRIB);

		if (iTotalChance > 0)
			{
			for (i = 0; i < pSatellites->GetContentElementCount(); i++)
				rTotalHits += rChanceAdj * CalcSatelliteHitsToDestroy(pSatellites->GetContentElement(i), iLevel, true) 
						* (Metric)pSatellites->GetContentElement(i)->GetAttributeInteger(CHANCE_ATTRIB)
						/ 100.0;
			}
		}
	else
		{
		Metric rCount = 1.0;

		CString sAttrib;
		if (pSatellites->FindAttribute(COUNT_ATTRIB, &sAttrib))
			{
			DiceRange Count;
			Count.LoadFromXML(sAttrib);
			rCount = Count.GetAveValueFloat();
			}

		for (i = 0; i < pSatellites->GetContentElementCount(); i++)
			rTotalHits += rCount * rChanceAdj * CalcSatelliteHitsToDestroy(pSatellites->GetContentElement(i), iLevel);
		}

	//	Done

	return rTotalHits;
	}

Metric CStationType::CalcSatelliteStrength (CXMLElement *pSatellites, int iLevel, bool bIgnoreChance)

//	CalcSatelliteStrength
//
//	Computes the total strength of satellites
//	100 = 1 satellite at station level.

	{
	int i;

	const CString &sTag = pSatellites->GetTag();

	int iChance;
	Metric rChanceAdj;
	if (!bIgnoreChance
			&& pSatellites->FindAttributeInteger(CHANCE_ATTRIB, &iChance))
		rChanceAdj = (Metric)iChance / 100.0;
	else
		rChanceAdj = 1.0;

	Metric rTotal = 0.0;
	if (strEquals(sTag, SHIP_TAG))
		{
		//	Load generator

		SDesignLoadCtx Ctx;
		IShipGenerator *pGenerator;
		if (IShipGenerator::CreateFromXML(Ctx, pSatellites, &pGenerator) != NOERROR)
			return 0.0;

		if (pGenerator->OnDesignLoadComplete(Ctx) != NOERROR)
			{
			delete pGenerator;
			return 0.0;
			}

		rTotal += rChanceAdj * pGenerator->GetAverageLevelStrength(iLevel);

		delete pGenerator;
		}
	else if (strEquals(sTag, STATION_TAG))
		{
		CStationType *pStationType = g_pUniverse->FindStationType((DWORD)pSatellites->GetAttributeInteger(TYPE_ATTRIB));
		if (pStationType == NULL)
			return 0.0;

		rTotal += rChanceAdj * pStationType->GetLevelStrength(iLevel);
		}
	else if (strEquals(sTag, TABLE_TAG))
		{
		int iTotalChance = 0;
		for (i = 0; i < pSatellites->GetContentElementCount(); i++)
			iTotalChance += pSatellites->GetContentElement(i)->GetAttributeInteger(CHANCE_ATTRIB);

		if (iTotalChance > 0)
			{
			for (i = 0; i < pSatellites->GetContentElementCount(); i++)
				rTotal += rChanceAdj * CalcSatelliteStrength(pSatellites->GetContentElement(i), iLevel, true) 
						* (Metric)pSatellites->GetContentElement(i)->GetAttributeInteger(CHANCE_ATTRIB)
						/ 100.0;
			}
		}
	else
		{
		Metric rCount = 1.0;

		CString sAttrib;
		if (pSatellites->FindAttribute(COUNT_ATTRIB, &sAttrib))
			{
			DiceRange Count;
			Count.LoadFromXML(sAttrib);
			rCount = Count.GetAveValueFloat();
			}

		for (i = 0; i < pSatellites->GetContentElementCount(); i++)
			rTotal += rCount * rChanceAdj * CalcSatelliteStrength(pSatellites->GetContentElement(i), iLevel);
		}

	//	Done

	return rTotal;
	}

Metric CStationType::CalcSatelliteTreasureValue (CXMLElement *pSatellites, int iLevel, bool bIgnoreChance)

//	CalcSatelliteTreasureValue
//
//	Computes the total treasure value in satellites

	{
	int i;

	const CString &sTag = pSatellites->GetTag();

	int iChance;
	Metric rChanceAdj;
	if (!bIgnoreChance
			&& pSatellites->FindAttributeInteger(CHANCE_ATTRIB, &iChance))
		rChanceAdj = (Metric)iChance / 100.0;
	else
		rChanceAdj = 1.0;

	Metric rTotal = 0.0;
	if (strEquals(sTag, SHIP_TAG))
		{
		//	Skip
		}
	else if (strEquals(sTag, STATION_TAG))
		{
		CStationType *pStationType = g_pUniverse->FindStationType((DWORD)pSatellites->GetAttributeInteger(TYPE_ATTRIB));
		if (pStationType == NULL)
			return 0.0;

		Metric rTreasure = rChanceAdj * pStationType->CalcTreasureValue(iLevel);

		//	Treasure in asteroids should be discounted (since not everyone has
		//	a mining cannon).

		if (!pStationType->CanAttack() 
				&& pStationType->HasAttribute(CONSTLIT("asteroid")))
			rTreasure *= 0.2;

		rTotal += rTreasure;
		}
	else if (strEquals(sTag, TABLE_TAG))
		{
		int iTotalChance = 0;
		for (i = 0; i < pSatellites->GetContentElementCount(); i++)
			iTotalChance += pSatellites->GetContentElement(i)->GetAttributeInteger(CHANCE_ATTRIB);

		if (iTotalChance > 0)
			{
			for (i = 0; i < pSatellites->GetContentElementCount(); i++)
				rTotal += rChanceAdj * CalcSatelliteTreasureValue(pSatellites->GetContentElement(i), iLevel, true) 
						* (Metric)pSatellites->GetContentElement(i)->GetAttributeInteger(CHANCE_ATTRIB)
						/ 100.0;
			}
		}
	else
		{
		Metric rCount = 1.0;

		CString sAttrib;
		if (pSatellites->FindAttribute(COUNT_ATTRIB, &sAttrib))
			{
			DiceRange Count;
			Count.LoadFromXML(sAttrib);
			rCount = Count.GetAveValueFloat();
			}

		for (i = 0; i < pSatellites->GetContentElementCount(); i++)
			rTotal += rCount * rChanceAdj * CalcSatelliteTreasureValue(pSatellites->GetContentElement(i), iLevel);
		}

	//	Done

	return rTotal;
	}

TArray<CStationType::SSatImageDesc> CStationType::CalcSegmentDesc (void) const

//	CalcSegmentDesc
//
//	Returns an array of segment images.

	{
	if (const CXMLElement *pSatellites = GetSatellitesDesc())
		{
		SSelectorInitCtx InitCtx;

		TArray<SSatImageDesc> Result;

		for (int i = 0; i < pSatellites->GetContentElementCount(); i++)
			{
			const CXMLElement *pSatDesc = pSatellites->GetContentElement(i);
			if (!pSatDesc->FindAttribute(SEGMENT_ATTRIB)
					|| !strEquals(STATION_TAG, pSatDesc->GetTag()))
				continue;

			//	Get the type of the satellite

			CStationType *pSatType = GetUniverse().FindStationType(pSatDesc->GetAttributeInteger(TYPE_ATTRIB));
			if (pSatType == NULL)
				continue;

			//	Prepare the image for the satellite

			SSatImageDesc *pSatImage = Result.Insert();
			pSatImage->pType = pSatType;
			pSatType->SetImageSelector(InitCtx, &pSatImage->Selector);

			//	If we have an image variant, then set it

			CString sVariant;
			if (pSatDesc->FindAttribute(IMAGE_VARIANT_ATTRIB, &sVariant))
				{
				IImageEntry *pRoot = pSatType->GetImage().GetRoot();
				DWORD dwID = (pRoot ? pRoot->GetID() : DEFAULT_SELECTOR_ID);
				int iVariant = pSatType->GetImage().GetVariantByID(sVariant);

				if (iVariant != -1)
					{
					pSatImage->Selector.DeleteAll();
					pSatImage->Selector.AddVariant(dwID, iVariant);
					}
				}

			pSatImage->pImage = &pSatType->GetImage(pSatImage->Selector, CCompositeImageModifiers());

			//	Now get the offset

			pSatImage->xOffset = pSatDesc->GetAttributeInteger(X_OFFSET_ATTRIB);
			pSatImage->yOffset = pSatDesc->GetAttributeInteger(Y_OFFSET_ATTRIB);
			}

		return Result;
		}
	else
		{
		return TArray<SSatImageDesc>();
		}
	}

Metric CStationType::CalcTreasureValue (int iLevel) const

//	CalcTreasureValue
//
//	Calculates the value of total treasure, include treasure on satellites.

	{
	Metric rTotal = 0.0;

	if (m_pItems)
		{
		SItemAddCtx AddItemCtx(GetUniverse());
		AddItemCtx.iLevel = iLevel;
		rTotal += m_pItems->GetAverageValue(AddItemCtx);
		}

	if (m_pSatellitesDesc)
		rTotal += CalcSatelliteTreasureValue(m_pSatellitesDesc, iLevel);

	return rTotal;
	}

Metric CStationType::CalcWeaponStrength (int iLevel) const

//	CalcWeaponStrength
//
//	Computes the total strength of all station weapons.
//	1.0 = level weapon at 1/4 fire rate.

	{
	static constexpr Metric MULTI_WEAPON_BONUS = 1.25;
	static constexpr Metric MULTI_WEAPON_BONUS_THRESHOLD = 0.8;

	//	Start by adding up all weapons.

	Metric rTotal = 0.0;
	for (int i = 0; i < m_AverageDevices.GetCount(); i++)
		{
		const CDeviceItem DeviceItem = m_AverageDevices.GetDeviceItem(i);
		if (DeviceItem.GetCategory() != itemcatWeapon
				&& DeviceItem.GetCategory() != itemcatLauncher)
			continue;

		int iDevLevel = DeviceItem.GetLevel();

		//	Adjust if directional.

		Metric rFireArcAdj = m_AverageDevices.GetFireArc(i) / 360.0;
		if (i != 0 && rFireArcAdj <= MULTI_WEAPON_BONUS_THRESHOLD)
			rFireArcAdj = Min(rFireArcAdj * MULTI_WEAPON_BONUS, 1.0);

		//	Lower level weapons count less; higher level weapons count more.

		rTotal += rFireArcAdj * ::CalcLevelDiffStrength(iDevLevel - iLevel);
		}

	//	Adjust for fire rate. Double the fire rate means double the strength.

	rTotal *= (40.0 / (Metric)m_iFireRateAdj);

	//	Done

	return rTotal;
	}

TSharedPtr<CG32bitImage> CStationType::CreateFullImage (SGetImageCtx &ImageCtx, const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers, RECT &retrcImage, int &retxCenter, int &retyCenter) const

//	CreateFullImage
//
//	Returns an image of the station, including any satellite segments.

	{
	//	Get the main image

	int iVariant;
	const CObjectImageArray *pMainImage = &GetImage(Selector, Modifiers, &iVariant);

	//	Compute any segment images

	TArray<SSatImageDesc> SatImages = CalcSegmentDesc();

	//	If no segments, then we just return the basic image

	if (SatImages.GetCount() == 0)
		{
		//	If we don't have a main image, then return NULL. Callers should 
		//	handle this and use a different image. NOTE: We check here because
		//	it is possible to not have a main image but to have satellites
		//	(e.g., St. Kats).

		if (!pMainImage->IsLoaded())
			return NULL;

		CG32bitImage *pBmpImage = &pMainImage->GetImage(CONSTLIT("CStationType::CreateFullImage"));
		retrcImage = pMainImage->GetImageRect(0, iVariant, &retxCenter, &retyCenter);

		//	AddRef because we don't want callers to free this bitmap, since it
		//	is owned by the type.

		return TSharedPtr<CG32bitImage>(pBmpImage->AddRef());
		}

	//	Otherwise, we need to composite all segments together with the main 
	//	image.

	else
		{
		//	Loop over all satellites and get metrics

		RECT rcMainImage = pMainImage->GetImageRect();
		RECT rcBounds;
		rcBounds.left = -(RectWidth(rcMainImage) / 2);
		rcBounds.top = -(RectHeight(rcMainImage) / 2);
		rcBounds.right = rcBounds.left + RectWidth(rcMainImage);
		rcBounds.bottom = rcBounds.top + RectHeight(rcMainImage);

		for (int i = 0; i < SatImages.GetCount(); i++)
			{
			const SSatImageDesc &SatImage = SatImages[i];

			//	Compute the satellite rect

			RECT rcSatImage = SatImage.pImage->GetImageRect();
			RECT rcSatBounds;
			rcSatBounds.left = SatImage.xOffset - (RectWidth(rcSatImage) / 2);
			rcSatBounds.top = -SatImage.yOffset - (RectHeight(rcSatImage) / 2);
			rcSatBounds.right = rcSatBounds.left + RectWidth(rcSatImage);
			rcSatBounds.bottom = rcSatBounds.top + RectHeight(rcSatImage);

			//	Increase the size of the bounds

			rcBounds.left = Min(rcBounds.left, rcSatBounds.left);
			rcBounds.right = Max(rcBounds.right, rcSatBounds.right);
			rcBounds.top = Min(rcBounds.top, rcSatBounds.top);
			rcBounds.bottom = Max(rcBounds.bottom, rcSatBounds.bottom);
			}

		//	Create an image that will hold the composite

		TSharedPtr<CG32bitImage> pCompositeImage(new CG32bitImage);
		pCompositeImage->Create(RectWidth(rcBounds), RectHeight(rcBounds), CG32bitImage::alpha8, CG32bitPixel::Null());
		int xCenter = -rcBounds.left;
		int yCenter = -rcBounds.top;

		//	Paint the main image

		pMainImage->PaintImage(*pCompositeImage, xCenter, yCenter, 0, Modifiers.GetRotation(), true);

		//	Paint all the satellites

		for (int i = 0; i < SatImages.GetCount(); i++)
			SatImages[i].pImage->PaintImage(*pCompositeImage, xCenter + SatImages[i].xOffset, yCenter - SatImages[i].yOffset, 0, 0, true);

		//	Now return the composite image

		retrcImage.left = 0;
		retrcImage.top = 0;
		retrcImage.right = RectWidth(rcBounds);
		retrcImage.bottom = RectHeight(rcBounds);

		retxCenter = xCenter;
		retyCenter = yCenter;

		return pCompositeImage;
		}
	}

bool CStationType::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Returns meta-data

	{
	if (m_HullDesc.FindDataField(sField, retsValue))
		return true;
	else if (strEquals(sField, FIELD_ABANDONED_DOCK_SCREEN))
		*retsValue = m_pAbandonedDockScreen.GetStringUNID(const_cast<CStationType *>(this));
	else if (strEquals(sField, FIELD_BALANCE))
		*retsValue = strFromInt((int)(CalcBalance() * 100.0));
	else if (strEquals(sField, FIELD_CATEGORY))
		{
		if (!GetEncounterDesc().CanBeRandomlyEncountered())
			*retsValue = CONSTLIT("04-Not Random");
		else if (HasLiteralAttribute(CONSTLIT("debris")))
			*retsValue = CONSTLIT("03-Debris");
		else if (HasLiteralAttribute(CONSTLIT("enemy")))
			*retsValue = CONSTLIT("02-Enemy");
		else if (HasLiteralAttribute(CONSTLIT("friendly")))
			*retsValue = CONSTLIT("01-Friendly");
		else
			*retsValue = CONSTLIT("04-Not Random");
		}
	else if (strEquals(sField, FIELD_DEFENDER_STRENGTH))
		*retsValue = strFromInt((int)(100.0 * CalcDefenderStrength(GetLevel())));
	else if (strEquals(sField, FIELD_DOCK_SCREEN))
		*retsValue = m_pFirstDockScreen.GetStringUNID(const_cast<CStationType *>(this));
	else if (strEquals(sField, FIELD_LEVEL))
		*retsValue = strFromInt(GetLevel());
	else if (strEquals(sField, FIELD_LOCATION_CRITERIA))
		*retsValue = GetEncounterDesc().GetLocationCriteria().AsString();
	else if (strEquals(sField, FIELD_NAME))
		*retsValue = GetNounPhrase();
	else if (strEquals(sField, FIELD_FIRE_RATE_ADJ))
		*retsValue = strFromInt(10000 / m_iFireRateAdj);
	else if (strEquals(sField, FIELD_HITS_TO_DESTROY))
		*retsValue = strFromInt(CalcHitsToDestroy(GetLevel()));
	else if (strEquals(sField, FIELD_INSTALL_DEVICE_MAX_LEVEL))
		{
		int iMaxLevel = (m_pTrade ? m_pTrade->GetMaxLevelMatched(GetUniverse(), serviceInstallDevice) : -1);
		*retsValue = (iMaxLevel != -1 ? strFromInt(iMaxLevel) : NULL_STR);
		}

	else if (strEquals(sField, FIELD_CAN_ATTACK))
		*retsValue = (CanAttack() ? VALUE_TRUE : VALUE_FALSE);
	else if (strEquals(sField, FIELD_EXPLOSION_TYPE))
		{
		if (m_pExplosionType)
			{
			CDeviceClass *pClass = GetUniverse().FindDeviceClass((DWORD)strToInt(m_pExplosionType->GetUNID(), 0));
			CWeaponClass *pWeapon = (pClass ? pClass->AsWeaponClass() : NULL);
			if (pWeapon)
				{
				*retsValue = pWeapon->GetItemType()->GetNounPhrase();
				return true;
				}
			}

		*retsValue = CONSTLIT("none");
		}
	else if (strEquals(sField, FIELD_MAX_LIGHT_RADIUS))
		*retsValue = strFromInt(GetMaxLightDistance());
	else if (strEquals(sField, FIELD_SATELLITE_STRENGTH))
		*retsValue = strFromInt((m_pSatellitesDesc ? (int)(100.0 * CalcSatelliteStrength(m_pSatellitesDesc, GetLevel())) : 0));
	else if (strEquals(sField, FIELD_SIZE))
		*retsValue = strFromInt(m_iSize);
	else if (strEquals(sField, FIELD_TREASURE_BALANCE))
		{
		int iLevel = GetLevel();
		Metric rExpected = (Metric)GetBalancedTreasure();
		Metric rTreasure = CalcTreasureValue(iLevel);
		*retsValue = strFromInt((int)(100.0 * (rExpected > 0.0 ? rTreasure / rExpected : 0.0)));
		}
	else if (strEquals(sField, FIELD_TREASURE_VALUE))
		*retsValue = strFromInt((int)CalcTreasureValue(GetLevel()));
	else if (strEquals(sField, FIELD_WEAPON_STRENGTH))
		*retsValue = strFromInt((int)(100.0 * CalcWeaponStrength(GetLevel())));
	else
		return CDesignType::FindDataField(sField, retsValue);

	return true;
	}

void CStationType::GenerateDevices (int iLevel, CDeviceDescList &Devices) const

//	GenerateDevices
//
//	Generate a list of devices
	
	{
	Devices.DeleteAll();

	if (m_pDevices)
		{
		SDeviceGenerateCtx Ctx(GetUniverse());
		Ctx.iLevel = iLevel;
		Ctx.pRoot = m_pDevices;
		Ctx.pResult = &Devices;
		Ctx.bNoDefaultPos = true;

		m_pDevices->AddDevices(Ctx);
		}
	}

const CAsteroidDesc &CStationType::GetAsteroidDesc (void) const

//	GetAsteroidDesc
//
//	Returns the asteroid descriptor.

	{
	if (!m_Asteroid.IsEmpty())
		return m_Asteroid;
	else if (const CStationType *pParent = CStationType::AsType(GetInheritFrom()))
		return pParent->GetAsteroidDesc();
	else
		return m_Asteroid;
	}

CurrencyValue CStationType::GetBalancedTreasure (void) const

//	GetBalancedTreasure
//
//	Returns the value of treasure (in credits) for this station that would 
//	balance against its difficulty.

	{
	return (CurrencyValue)(pow(CalcBalance(), TREASURE_BALANCE_POWER) * (Metric)STD_STATION_DATA[GetLevel()].dwTreasureValue);
	}

int CStationType::GetChallengeRating (void) const

//	GetChallengeRating
//
//	An integer value representing the combat strength of the station.

	{
	//	If we have an explicit CR, return that.

	if (m_iChallengeRating)
		return m_iChallengeRating;

	//	Otherwise, compute it.

	else
		return CalcChallengeRating(GetLevel(), CalcBalance());
	}

CCommunicationsHandler *CStationType::GetCommsHandler (void)

//	GetCommsHandler
//
//	Returns the comms handler to use

	{
	CDesignType *pParent = GetInheritFrom();
	CCommunicationsHandler *pParentHandler;

	if (pParent && (pParentHandler = pParent->GetCommsHandler()))
		{
		if (!m_fCommsHandlerInit)
			{
			m_CommsHandler.Merge(m_OriginalCommsHandler);
			m_CommsHandler.Merge(*pParentHandler);

			m_fCommsHandlerInit = true;
			}

		return (m_CommsHandler.GetCount() ? &m_CommsHandler : NULL);
		}
	else
		return (m_OriginalCommsHandler.GetCount() ? &m_OriginalCommsHandler : NULL);
	}

CSovereign *CStationType::GetControllingSovereign (void) const

//	GetControllingSovereign
//
//	Returns the sovereign that controls the station

	{
	if (m_pControllingSovereign)
		return m_pControllingSovereign;
	else
		return m_pSovereign;
	}

const CStationEncounterDesc &CStationType::GetEncounterDesc (void) const

//	GetEncounterDesc
//
//	Get the encounter descriptor for this station.

	{
	return (m_pEncounterDescOverride ? *m_pEncounterDescOverride : m_EncounterDesc);
	}

CStationEncounterDesc &CStationType::GetEncounterDesc (void)

//	GetEncounterDesc
//
//	Get the encounter descriptor for this station.

	{
	return (m_pEncounterDescOverride ? *m_pEncounterDescOverride : m_EncounterDesc);
	}

int CStationType::GetLevel (int *retiMinLevel, int *retiMaxLevel) const

//	GetLevel
//
//	Returns the average level of the station.
//
//	NOTE: We don't return a level range yet because we're not sure of the 
//	backwards compatibility issues.

	{
	//	If necessary, calculate level.

	if (m_iLevel == 0)
		{
		m_iLevel = GetEncounterDesc().CalcLevelFromFrequency();

		if (m_iLevel == 0)
			{
			m_iLevel = m_HullDesc.GetArmorLevel();

			//	Take the highest level of armor or devices

			for (int i = 0; i < m_AverageDevices.GetCount(); i++)
				{
				const CDeviceItem DeviceItem = m_AverageDevices.GetDeviceItem(i);

				int iDeviceLevel = DeviceItem.GetLevel();
				if (iDeviceLevel > m_iLevel)
					m_iLevel = iDeviceLevel;
				}
			}
		}

	//	Return

	if (retiMinLevel) *retiMinLevel = m_iLevel;
	if (retiMaxLevel) *retiMaxLevel = m_iLevel;
	return m_iLevel;
	}

Metric CStationType::GetLevelStrength (int iLevel) const

//	GetLevelStrength
//
//	Returns the level strength of the given station (relative to iLevel).
//	1.0 = station with level appropriate weapon and 25 hits to destroy.

	{
	static constexpr Metric BALANCE_STD_DEFENDERS_PER_STATION = 3.5;

	Metric rHitsAdj = CalcBalanceHitsAdj(iLevel);

	Metric rWeaponStrength = rHitsAdj * CalcWeaponStrength(iLevel);
	Metric rDefenderStrength = CalcDefenderStrength(iLevel);

	return (rWeaponStrength + rDefenderStrength) / BALANCE_STD_DEFENDERS_PER_STATION;
	}

CString CStationType::GetNamePattern (DWORD dwNounFormFlags, DWORD *retdwFlags) const

//	GetNamePattern
//
//	Returns the noun pattern.

	{
	return m_Name.GetConstantName(retdwFlags);
	}

CItem CStationType::GetPrimaryWeapon (void) const

//	GetPrimaryWeapon
//
//	Returns the highest-level weapon on the station.

	{
	const CItem *pBestItem = NULL;

	for (int i = 0; i < m_AverageDevices.GetCount(); i++)
		{
		const CItem &Item = m_AverageDevices.GetDeviceItem(i);
		if (Item.GetCategory() == itemcatWeapon || Item.GetCategory() == itemcatLauncher)
			{
			if (pBestItem == NULL 
					|| Item.GetLevel() > pBestItem->GetLevel())
				pBestItem = &Item;
			}
		}

	if (pBestItem)
		return *pBestItem;
	else
		return CItem();
	}

IShipGenerator *CStationType::GetReinforcementsTable (void)

//	GetReinforcementsTable
//
//	Return the reinforcements table.
	
	{
	//	If we have a ship count structure, then we always use the main ship 
	//	table.

	if (m_DefenderCount.GetCountType() == CShipChallengeDesc::countStanding)
		return m_pInitialShips;

	//	Otherwise, if we have an explicit reinforcements table, use that.

	else if (m_pReinforcements)
		return m_pReinforcements;

	//	Otherwise, we use the main table.

	else
		return m_pInitialShips;
	}

const CIntegralRotationDesc &CStationType::GetRotationDesc (void)

//	GetRotationDesc
//
//	Returns the rotation parameters.

	{
	if (m_DefaultRotation.GetFrameCount() != 360)
		m_DefaultRotation.Init(360);

	return m_DefaultRotation;
	}

bool CStationType::IsSizeClass (ESizeClass iClass) const

//	IsSizeClass
//
//	Returns TRUE if we are the given size class.

	{
	switch (GetScale())
		{
		case scaleWorld:
			return (iClass >= worldSizeA && iClass <= worldSizeM
					&& m_iSize >= SIZE_DATA[iClass].iMinSize 
					&& m_iSize <= SIZE_DATA[iClass].iMaxSize);

		default:
			return false;
		}
	}

void CStationType::MarkImages (const CCompositeImageSelector &Selector, const CCompositeImageModifiers &Modifiers)

//	MarkImages
//
//	Marks images used by the station

	{
	SGetImageCtx Ctx(GetUniverse());
	m_Image.MarkImage(Ctx, Selector, Modifiers);

	//	Cache the destroyed station image, if necessary

	if (HasWreckImage())
		{
		CCompositeImageModifiers Modifiers;
		Modifiers.SetStationDamage();
		m_Image.MarkImage(Ctx, Selector, Modifiers);
		}

	//	Explosions and other effects

	if (m_pExplosionType)
		m_pExplosionType->MarkImages();

	if (m_pEjectaType)
		m_pEjectaType->MarkImages();

	m_Stargate.MarkImages();

	//	NOTE: We don't bother marking the hero image because we don't need it
	//	to paint the main screen. [We only needed when docking, and it's OK to
	//	delay slightly.]
	}

void CStationType::OnAccumulateStats (SStats &Stats) const

//	OnAccumulateStats
//
//	Accumulate stats.

	{
	Stats.dwGraphicsMemory += m_Image.GetMemoryUsage();
	}

void CStationType::OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const

//	OnAccumulateXMLMergeFlags
//
//	Add flags to merge XML

	{
	//	We know how to handle these tags through the inheritance hierarchy.

	MergeFlags.SetAt(CXMLElement::GetKeywordID(ASTEROID_TAG), CXMLElement::MERGE_OVERRIDE);
	MergeFlags.SetAt(CXMLElement::GetKeywordID(COMMUNICATIONS_TAG), CXMLElement::MERGE_OVERRIDE);
	}

void CStationType::OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	OnAddTypesUsed
//
//	Add design types used by this station type

	{
	int i;

	retTypesUsed->SetAt(m_pSovereign.GetUNID(), true);

	m_HullDesc.AddTypesUsed(retTypesUsed);

	if (m_pDevices)
		m_pDevices->AddTypesUsed(retTypesUsed);

	if (m_pItems)
		m_pItems->AddTypesUsed(retTypesUsed);

	m_Image.AddTypesUsed(retTypesUsed);
	m_HeroImage.AddTypesUsed(retTypesUsed);

	for (i = 0; i < m_ShipWrecks.GetCount(); i++)
		retTypesUsed->SetAt(m_ShipWrecks[i], true);

	for (i = 0; i < m_iAnimationsCount; i++)
		retTypesUsed->SetAt(m_pAnimations[i].m_Image.GetBitmapUNID(), true);

	retTypesUsed->SetAt(strToInt(m_pFirstDockScreen.GetUNID(), 0), true);
	retTypesUsed->SetAt(strToInt(m_pAbandonedDockScreen.GetUNID(), 0), true);
	retTypesUsed->SetAt(m_dwDefaultBkgnd, true);

	if (m_pSatellitesDesc)
		AddTypesUsedByXML(m_pSatellitesDesc, retTypesUsed);

	if (m_pInitialShips)
		m_pInitialShips->AddTypesUsed(retTypesUsed);

	if (m_pReinforcements)
		m_pReinforcements->AddTypesUsed(retTypesUsed);

	if (m_pEncounters)
		m_pEncounters->AddTypesUsed(retTypesUsed);

	if (m_pConstruction)
		m_pConstruction->AddTypesUsed(retTypesUsed);

	retTypesUsed->SetAt(m_pExplosionType.GetUNID(), true);
	retTypesUsed->SetAt(m_pEjectaType.GetUNID(), true);
	retTypesUsed->SetAt(m_pBarrierEffect.GetUNID(), true);
	retTypesUsed->SetAt(m_pControllingSovereign.GetUNID(), true);

	m_Stargate.AddTypesUsed(retTypesUsed);
	}

ALERROR CStationType::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design

	{
	ALERROR error;

	//	Images

	if (error = m_Image.OnDesignLoadComplete(Ctx))
		return ComposeLoadError(Ctx, Ctx.sError);

	if (error = m_HeroImage.OnDesignLoadComplete(Ctx))
		return ComposeLoadError(Ctx, Ctx.sError);

	for (int i = 0; i < m_iAnimationsCount; i++)
		if (error = m_pAnimations[i].m_Image.OnDesignLoadComplete(Ctx))
			return ComposeLoadError(Ctx, Ctx.sError);

	//	Sovereigns

	if (error = m_pSovereign.Bind(Ctx))
		return ComposeLoadError(Ctx, Ctx.sError);

	if (error = m_pControllingSovereign.Bind(Ctx))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Armor

	if (error = m_HullDesc.Bind(Ctx))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Resolve screen

	if (error = m_pAbandonedDockScreen.Bind(Ctx, GetLocalScreens()))
		return ComposeLoadError(Ctx, Ctx.sError);

	if (error = m_pFirstDockScreen.Bind(Ctx, GetLocalScreens()))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Resolve the devices pointer

	if (m_pDevices)
		{
		if (error = m_pDevices->OnDesignLoadComplete(Ctx))
			return ComposeLoadError(Ctx, Ctx.sError);

		//	NOTE: Can't call GetLevel because it relies on m_AverageDevices.

		int iLevel = Max(m_iLevel, GetEncounterDesc().CalcLevelFromFrequency());
		GenerateDevices(iLevel, m_AverageDevices);
		}

	//	Items

	if (m_pItems)
		if (error = m_pItems->OnDesignLoadComplete(Ctx))
			return ComposeLoadError(Ctx, Ctx.sError);

	if (m_pTrade)
		if (error = m_pTrade->OnDesignLoadComplete(Ctx))
			return ComposeLoadError(Ctx, Ctx.sError);

	//	Ships

	if (m_pInitialShips)
		if (error = m_pInitialShips->OnDesignLoadComplete(Ctx))
			return ComposeLoadError(Ctx, Ctx.sError);

	if (m_pReinforcements)
		if (error = m_pReinforcements->OnDesignLoadComplete(Ctx))
			return ComposeLoadError(Ctx, Ctx.sError);

	if (m_pConstruction)
		if (error = m_pConstruction->OnDesignLoadComplete(Ctx))
			return ComposeLoadError(Ctx, Ctx.sError);

	if (m_pEncounters)
		if (error = m_pEncounters->OnDesignLoadComplete(Ctx))
			return ComposeLoadError(Ctx, Ctx.sError);

	//	Resolve the explosion pointer

	if (error = m_pExplosionType.Bind(Ctx))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Resolve ejecta pointer

	if (error = m_pEjectaType.Bind(Ctx))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Resolve effects

	if (error = m_pBarrierEffect.Bind(Ctx))
		return ComposeLoadError(Ctx, Ctx.sError);

	if (error = m_Stargate.Bind(Ctx))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Virtual objects always get some settings by default

	if (IsVirtual())
		{
		m_HullDesc.SetImmutable();
		m_fNoMapIcon = true;
		}

	//	If we have an OnMining event then we show an unexplored flag when
	//	necessary (this is used for asteroid mining).

	bool bHasOnMining = FindEventHandler(EVENT_ON_MINING);
	m_fShowsUnexploredAnnotation = bHasOnMining;

	//	Figure out if this is static

	m_fStatic = (m_HullDesc.GetMaxHitPoints() == 0)
			&& (m_HullDesc.GetMaxStructuralHP() == 0)
			&& (m_pDevices == NULL)
			&& (GetAbandonedScreen() == NULL)
			&& (GetFirstDockScreen() == NULL)
			&& (m_pInitialShips == NULL)
			&& (m_pReinforcements == NULL)
			&& (m_pEncounters == NULL)
			&& (m_pConstruction == NULL)
			&& (m_pItems == NULL)
			&& !HasEvents()
			&& (m_pBarrierEffect == NULL)
			&& !m_fMobile
			&& !m_fWall
			&& !m_fSign
			&& !m_fBeacon
			&& !m_fShipEncounter
			&& !m_fStationEncounter;

	//	Any object has no HP and is a star or a world is immutable
	//	by default.

	if (!m_HullDesc.IsImmutable()
			&& (m_iScale == scaleStar || m_iScale == scaleWorld)
			&& m_HullDesc.GetMaxHitPoints() == 0
			&& m_HullDesc.GetMaxStructuralHP() == 0
			&& m_iEjectaAdj == 0
			&& !bHasOnMining)
		m_HullDesc.SetImmutable();

	return NOERROR;
	}

ALERROR CStationType::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML

	{
	int i;
	ALERROR error;

	ASSERT(pDesc);
	if (!pDesc)
		return ERR_FAIL;

	//	Initialize basic info

	m_iLevel = pDesc->GetAttributeIntegerBounded(LEVEL_ATTRIB, 1, MAX_SYSTEM_LEVEL, 0);
	m_iChallengeRating = pDesc->GetAttributeIntegerBounded(CHALLENGE_RATING_ATTRIB, 1, -1, 0);
	m_fCalcLevel = (m_iLevel == 0);
	m_fBalanceValid = false;

	if (error = m_pSovereign.LoadUNID(Ctx, pDesc->GetAttribute(SOVEREIGN_ATTRIB), (GetAPIVersion() >= 26 ? UNID_NEUTRAL_SOVEREIGN : 0)))
		return error;

	if (error = m_pControllingSovereign.LoadUNID(Ctx, pDesc->GetAttribute(CONTROLLING_SOVEREIGN_ATTRIB)))
		return error;

	m_fVirtual = pDesc->GetAttributeBool(VIRTUAL_ATTRIB);
	m_fMobile = pDesc->GetAttributeBool(MOBILE_ATTRIB);
	m_fWall = pDesc->GetAttributeBool(WALL_ATTRIB);
	m_fNoFriendlyFire = pDesc->GetAttributeBool(NO_FRIENDLY_FIRE_ATTRIB);
	m_fNoFriendlyTarget = pDesc->GetAttributeBool(NO_FRIENDLY_TARGET_ATTRIB);
	m_fNoIndependentAttack = pDesc->GetAttributeBool(NO_INDEPENDENT_ATTACK_ATTRIB);
	m_fInactive = pDesc->GetAttributeBool(INACTIVE_ATTRIB);
	m_fDestroyWhenAbandoned = pDesc->GetAttributeBool(DESTROY_WHEN_ABANDONED_ATTRIB);
	m_fDestroyWhenEmpty = pDesc->GetAttributeBool(DESTROY_WHEN_EMPTY_ATTRIB);
	m_fAllowEnemyDocking = pDesc->GetAttributeBool(ALLOW_ENEMY_DOCKING_ATTRIB);
	m_fSign = pDesc->GetAttributeBool(SIGN_ATTRIB);
	m_fBeacon = pDesc->GetAttributeBool(BEACON_ATTRIB);
	m_fRadioactive = pDesc->GetAttributeBool(RADIOACTIVE_ATTRIB);
	m_fNoMapIcon = pDesc->GetAttributeBool(NO_MAP_ICON_ATTRIB);
	m_fNoMapDetails = pDesc->GetAttributeBool(NO_MAP_DETAILS_ATTRIB);
	m_fTimeStopImmune = pDesc->GetAttributeBool(TIME_STOP_IMMUNE_ATTRIB);
	m_fCanAttack = pDesc->GetAttributeBool(CAN_ATTACK_ATTRIB);
	m_fReverseArticle = pDesc->GetAttributeBool(REVERSE_ARTICLE_ATTRIB);
	m_fNoBlacklist = (pDesc->GetAttributeBool(NO_BLACKLIST_ATTRIB) || pDesc->GetAttributeBool(IGNORE_FRIENDLY_FIRE_ATTRIB));
	m_fAnonymous = pDesc->GetAttributeBool(ANONYMOUS_ATTRIB);
	m_iAlertWhenAttacked = pDesc->GetAttributeInteger(ALERT_WHEN_ATTACKED_ATTRIB);
	m_iAlertWhenDestroyed = pDesc->GetAttributeInteger(ALERT_WHEN_DESTROYED_ATTRIB);
	m_iStealth = pDesc->GetAttributeIntegerBounded(STEALTH_ATTRIB, CSpaceObject::stealthMin, CSpaceObject::stealthMax, CSpaceObject::stealthNormal);

	//	Suppress or force map label
	//
	//	If showMapLabel="true", we force showing the label, even if we normally
	//	would not. Conversely, if ="false", we always suppress showing the 
	//	label. If not specified, we let the station do the default behavior.

	bool bValue;
	if (pDesc->FindAttributeBool(SHOW_MAP_LABEL_ATTRIB, &bValue))
		{
		m_fForceMapLabel = bValue;
		m_fSuppressMapLabel = !bValue;
		}

	//	Handle this for backwards compatibility.

	else if (pDesc->FindAttributeBool(NO_MAP_LABEL_ATTRIB, &bValue))
		{
		m_fSuppressMapLabel = bValue;
		m_fForceMapLabel = false;
		}

	//	Else, let the station do default behavior.

	else
		{
		m_fForceMapLabel = false;
		m_fSuppressMapLabel = false;
		}
		
	m_iPaintOrder = CPaintOrder::Parse(pDesc->GetAttribute(PAINT_LAYER_ATTRIB));
	if (m_iPaintOrder == CPaintOrder::error)
		return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Invalid paintLayer: %s"), pDesc->GetAttribute(PAINT_LAYER_ATTRIB)));

	//	Get the scale (default to structure)

	m_iScale = ParseScale(pDesc->GetAttribute(SCALE_ATTRIB));
	if (m_iScale == scaleNone)
		m_iScale = scaleStructure;

	//	Encounter wrapper

	if (strEquals(pDesc->GetTag(), ENCOUNTER_TYPE_TAG))
		{
		//	If we have an <EncounterGroup> tag then this is an encounter group
		//	handled by CreateSystem.

		if (pDesc->GetContentElementByTag(ENCOUNTER_GROUP_TAG))
			{
			m_fStationEncounter = true;
			m_fShipEncounter = false;
			}

		//	Otherwise, if we have a <Ships> tag, then we handle this as a ship
		//	encounter.

		else if (pDesc->GetContentElementByTag(SHIPS_TAG))
			{
			m_fStationEncounter = false;
			m_fShipEncounter = true;
			m_iScale = scaleShip;
			}

		//	Otherwise this is an error.

		else
			return ComposeLoadError(Ctx, CONSTLIT("Expected <EncounterGroup>."));
		}
	else
		{
		m_fStationEncounter = false;
		m_fShipEncounter = pDesc->GetAttributeBool(SHIP_ENCOUNTER_ATTRIB);
		}

	//	Ship repair rate

	CString sRegen;
	int iRepairRate;
	if (pDesc->FindAttribute(SHIP_REGEN_ATTRIB, &sRegen))
		{
		if (error = m_ShipRegen.InitFromRegenString(Ctx, sRegen, STATION_REPAIR_FREQUENCY))
			return error;
		}
	else if (pDesc->FindAttributeInteger(SHIP_REPAIR_RATE_ATTRIB, &iRepairRate) && iRepairRate > 0)
		m_ShipRegen.InitFromRegen(6.0 * iRepairRate, STATION_REPAIR_FREQUENCY);

	//	Starting in API 23 we change the default to 40 instead of 80

	int iDefaultFireRateAdj = (GetAPIVersion() >= 23 ? 40 : 80);
	m_iFireRateAdj = strToInt(pDesc->GetAttribute(FIRE_RATE_ADJ_ATTRIB), iDefaultFireRateAdj);

	//	Load names

	if (error = m_Name.InitFromXMLRoot(Ctx, pDesc))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Placement

	if (error = m_EncounterDesc.InitFromStationTypeXML(Ctx, pDesc))
		return error;

	//	Background objects

	m_rParallaxDist = pDesc->GetAttributeIntegerBounded(BACKGROUND_PLANE_ATTRIB, 0, -1, 100) / 100.0;
	m_fOutOfPlane = (m_rParallaxDist != 1.0);

	//	Get hull desc

	if (error = m_HullDesc.InitFromStationXML(Ctx, pDesc))
		return error;

	//	Mass & Size
	
	m_iSize = pDesc->GetAttributeIntegerBounded(SIZE_ATTRIB, 1, -1, 0);

	if (!pDesc->FindAttributeDouble(MASS_ATTRIB, &m_rMass))
		{
		if (m_iScale == scaleWorld || m_iScale == scaleStar)
			m_rMass = 1.0;	//	1 Earth mass or 1 solar mass.
		else
			m_rMass = 1000000.0;
		}

	//	Load devices

	CXMLElement *pDevices = pDesc->GetContentElementByTag(DEVICES_TAG);
	if (pDevices)
		{
		if (error = IDeviceGenerator::CreateFromXML(Ctx, pDevices, &m_pDevices))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Keep the descriptor
	
	m_pDesc = pDesc;

	//	Load items

	CXMLElement *pItems = pDesc->GetContentElementByTag(ITEMS_TAG);
	if (pItems)
		{
		if (error = IItemGenerator::CreateFromXML(Ctx, pItems, &m_pItems))
			return error;
		}

	//	Load trade

	CXMLElement *pTrade = pDesc->GetContentElementByTag(TRADE_TAG);
	if (pTrade)
		{
		if (error = CTradingDesc::CreateFromXML(Ctx, pTrade, &m_pTrade))
			return error;
		}

	//	Find an element describing the image
	//	HACK: At some point, this should be handled by m_Image.

	CXMLElement *pImage = pDesc->GetContentElementByTag(IMAGE_TAG);
	if (pImage == NULL)
		pImage = pDesc->GetContentElementByTag(IMAGE_VARIANTS_TAG);
	if (pImage == NULL)
		pImage = pDesc->GetContentElementByTag(COMPOSITE_TAG);
	if (pImage == NULL)
		pImage = pDesc->GetContentElementByTag(IMAGE_EFFECT_TAG);
	if (pImage == NULL)
		pImage = pDesc->GetContentElementByTag(IMAGE_LOOKUP_TAG);
	if (pImage == NULL)
		pImage = pDesc->GetContentElementByTag(IMAGE_SHIPWRECK_TAG);

	//	Load the image

	if (pImage)
		{
		//	If this image comes from a shipwreck then load it from
		//	a ship class object. Otherwise, initialize the image from the XML
		//
		//	NOTE: This is for bacwards compatibility the new way of doing this
		//	is to have an <ImageVariants> element with a set of <Shipwreck>
		//	elements.
		//
		//	LATER: We should convert this to the new format so that we can
		//	get rid of the m_ShipWrecks variable.

		if (pImage->AttributeExists(SHIPWRECK_UNID_ATTRIB))
			{
			if (error = pImage->GetAttributeIntegerList(SHIPWRECK_UNID_ATTRIB, &m_ShipWrecks))
				return ComposeLoadError(Ctx, CONSTLIT("Unable to load ship wreck list"));

			if (m_ShipWrecks.GetCount() == 0)
				return ComposeLoadError(Ctx, CONSTLIT("Expected ship wreck list"));

			if (error = m_Image.InitAsShipwreck(Ctx))
				return ComposeLoadError(Ctx, Ctx.sError);
			}

		//	Otherwise, load the image

		else
			{
			if (error = m_Image.InitFromXML(Ctx, pImage))
				return ComposeLoadError(Ctx, Ctx.sError);
			}
		}

	//	If we don't have an image defined, and this is a shipwreck, then we 
	//	default to shipwreck image.

	else if (m_iScale == scaleShip && HasAttribute(VALUE_SHIPWRECK))
		{
		if (error = m_Image.InitAsShipwreck(Ctx))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Load hero image

	if (pImage = pDesc->GetContentElementByTag(HERO_IMAGE_TAG))
		{
		if (error = m_HeroImage.InitFromXML(Ctx, pImage))
			return ComposeLoadError(Ctx, Ctx.sError);
		}

	//	Load animations

	CXMLElement *pAnimations = pDesc->GetContentElementByTag(ANIMATIONS_TAG);
	if (pAnimations)
		{
		m_iAnimationsCount = pAnimations->GetContentElementCount();
		m_pAnimations = new SAnimationSection [m_iAnimationsCount];

		for (i = 0; i < m_iAnimationsCount; i++)
			{
			CXMLElement *pSection = pAnimations->GetContentElement(i);
			m_pAnimations[i].m_x = pSection->GetAttributeInteger(X_ATTRIB);
			m_pAnimations[i].m_y = pSection->GetAttributeInteger(Y_ATTRIB);

			if (pSection->GetContentElementCount() > 0)
				{
				CXMLElement *pImage = pSection->GetContentElement(0);
				if (error = m_pAnimations[i].m_Image.InitFromXML(Ctx, *pImage))
					return ComposeLoadError(Ctx, CONSTLIT("Unable to load animation image"));
				}
			}
		}

	//	Now get the first dock screen

	m_pFirstDockScreen.LoadUNID(Ctx, pDesc->GetAttribute(DOCK_SCREEN_ATTRIB));
	m_pAbandonedDockScreen.LoadUNID(Ctx, pDesc->GetAttribute(ABANDONED_SCREEN_ATTRIB));

	//	Background screens

	if (error = LoadUNID(Ctx, pDesc->GetAttribute(DEFAULT_BACKGROUND_ID_ATTRIB), &m_dwDefaultBkgnd))
		return error;
	
	//	Load communications

	CXMLElement *pComms = pDesc->GetContentElementByTag(COMMUNICATIONS_TAG);
	if (pComms)
		if (error = m_OriginalCommsHandler.InitFromXML(pComms, &Ctx.sError))
			return ComposeLoadError(Ctx, Ctx.sError);

	m_fCommsHandlerInit = false;

	//	Load initial ships

	m_fBuildReinforcements = false;

	CXMLElement *pShips = pDesc->GetContentElementByTag(SHIPS_TAG);
	if (pShips)
		{
		if (error = IShipGenerator::CreateFromXMLAsGroup(Ctx, pShips, &m_pInitialShips))
			return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("<Ships>: %s"), Ctx.sError));

		//	See if we have a challenge rating; in that case, we use it as both
		//	initial ships and reinforcements.

		CString sCount;
		if (pShips->FindAttribute(CHALLENGE_ATTRIB, &sCount))
			{
			if (!m_DefenderCount.InitFromChallengeRating(sCount))
				return ComposeLoadError(Ctx, CONSTLIT("Invalid challenge attribute in <Ships>"));
			}

		//	If defined, we use this count to create initial ships AND reinforcements.

		else if (pShips->FindAttribute(STANDING_COUNT_ATTRIB, &sCount))
			{
			if (!m_DefenderCount.Init(CShipChallengeDesc::countStanding, sCount))
				return ComposeLoadError(Ctx, CONSTLIT("Invalid count attribute in <Ships>"));
			}

		//	Otherwise, see if we define minShips, in which case we use that value for
		//	reinforcements only.

		else if (pShips->FindAttribute(MIN_SHIPS_ATTRIB, &sCount))
			{
			if (!m_DefenderCount.Init(CShipChallengeDesc::countReinforcements, sCount))
				return ComposeLoadError(Ctx, CONSTLIT("Invalid count attribute in <Ships>"));
			}

		//	Build instead of gate in

		bool bValue;
		if (pShips->FindAttributeBool(BUILD_REINFORCEMENTS_ATTRIB, &bValue) && bValue)
			m_fBuildReinforcements = true;
		}

	//	Load reinforcements

	CXMLElement *pReinforcements = m_pDesc->GetContentElementByTag(REINFORCEMENTS_TAG);
	if (pReinforcements)
		{
		if (error = IShipGenerator::CreateFromXMLAsGroup(Ctx, pReinforcements, &m_pReinforcements))
			return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("<Reinforcements>: %s"), Ctx.sError));

		//	Figure out the minimum number of reinforcements at this base

		if (!m_DefenderCount.Init(CShipChallengeDesc::countReinforcements, pReinforcements->GetAttribute(MIN_SHIPS_ATTRIB)))
			return ComposeLoadError(Ctx, CONSTLIT("Invalid count attribute in <Reinforcements>"));

		//	Build instead of gate in

		bool bValue;
		if (pReinforcements->FindAttributeBool(BUILD_REINFORCEMENTS_ATTRIB, &bValue) && bValue)
			m_fBuildReinforcements = true;
		}

	//	Load encounter table

	CXMLElement *pEncounters = m_pDesc->GetContentElementByTag(ENCOUNTERS_TAG);
	if (pEncounters)
		{
		if (error = IShipGenerator::CreateFromXMLAsGroup(Ctx, pEncounters, &m_pEncounters))
			return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("<Encounters>: %s"), Ctx.sError));

		m_iEncounterFrequency = GetFrequency(pEncounters->GetAttribute(FREQUENCY_ATTRIB));
		}

	//	If we don't have an encounter table then we might have a custom encounter event

	else
		{
		m_iEncounterFrequency = GetFrequency(pDesc->GetAttribute(RANDOM_ENCOUNTERS_ATTRIB));
		m_pEncounters = NULL;
		}

	//	Load construction table

	CXMLElement *pConstruction = m_pDesc->GetContentElementByTag(CONSTRUCTION_TAG);
	if (pConstruction)
		{
		if (error = IShipGenerator::CreateFromXMLAsGroup(Ctx, pConstruction, &m_pConstruction))
			return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("<Construction>: %s"), Ctx.sError));

		m_iShipConstructionRate = pConstruction->GetAttributeInteger(CONSTRUCTION_RATE_ATTRIB);
		m_iMaxConstruction = pConstruction->GetAttributeInteger(MAX_CONSTRUCTION_ATTRIB);
		}

	//	Load satellites

	m_pSatellitesDesc = pDesc->GetContentElementByTag(SATELLITES_TAG);

	//	Explosion

	if (error = m_pExplosionType.LoadUNID(Ctx, pDesc->GetAttribute(EXPLOSION_TYPE_ATTRIB)))
		return error;

	//	Ejecta

	if (error = m_pEjectaType.LoadUNID(Ctx, pDesc->GetAttribute(EJECTA_TYPE_ATTRIB)))
		return error;

	if (m_pEjectaType.GetUNID())
		{
		m_iEjectaAdj = pDesc->GetAttributeInteger(EJECTA_ADJ_ATTRIB);
		if (m_iEjectaAdj == 0)
			m_iEjectaAdj = 100;
		}
	else
		m_iEjectaAdj = 0;

	//	Special object descriptors

	if (error = m_Asteroid.InitFromStationTypeXML(Ctx, *pDesc))
		return ComposeLoadError(Ctx, Ctx.sError);

	if (error = m_Star.InitFromStationTypeXML(Ctx, *pDesc))
		return ComposeLoadError(Ctx, Ctx.sError);

	if (error = m_Stargate.InitFromStationTypeXML(Ctx, *pDesc))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Miscellaneous

	if (error = m_pBarrierEffect.LoadUNID(Ctx, pDesc->GetAttribute(BARRIER_EFFECT_ATTRIB)))
		return error;

	//	Done

	return NOERROR;
	}

ICCItemPtr CStationType::OnGetProperty (CCodeChainCtx &Ctx, const CString &sProperty) const

//	OnGetProperty
//
//	Returns a property

	{
	CCodeChain &CC = GetUniverse().GetCC();

	if (strEquals(sProperty, PROPERTY_ASTEROID_TYPE))
		{
		auto iType = GetAsteroidDesc().GetType();
		if (iType == EAsteroidType::none)
			return ICCItemPtr::Nil();
		else
			return ICCItemPtr(CAsteroidDesc::CompositionID(iType));
		}

	else if (strEquals(sProperty, PROPERTY_AUTO_LEVEL_FREQUENCY))
		return (GetEncounterDesc().HasAutoLevelFrequency() ? ICCItemPtr(GetEncounterDesc().GetLevelFrequency()) : ICCItemPtr(ICCItem::Nil));

	else if (strEquals(sProperty, PROPERTY_CAN_BE_MINED))
		return ICCItemPtr(ShowsUnexploredAnnotation());

	else if (strEquals(sProperty, PROPERTY_CHALLENGE_RATING))
		{
		int iRating = GetChallengeRating();
		if (iRating <= 0)
			return ICCItemPtr::Nil();
		else
			return ICCItemPtr(iRating);
		}

	else if (strEquals(sProperty, PROPERTY_CURRENCY))
		{
		if (m_pTrade)
			return ICCItemPtr(m_pTrade->GetEconomyType()->GetUNID());
		else
			return ICCItemPtr::Nil();
		}

	else if (strEquals(sProperty, PROPERTY_CURRENCY_NAME))
		{
		if (m_pTrade)
			return ICCItemPtr(m_pTrade->GetEconomyType()->GetSID());
		else
			return ICCItemPtr::Nil();
		}

	else if (strEquals(sProperty, PROPERTY_ENCOUNTERED_BY_NODE))
		{
		ICCItemPtr pResult(ICCItem::SymbolTable);
		TSortMap<CString, int> ByNode = m_EncounterRecord.GetEncounterCountByNode();
		for (int i = 0; i < ByNode.GetCount(); i++)
			pResult->SetIntegerAt(ByNode.GetKey(i), ByNode[i]);

		return pResult;
		}
	else if (strEquals(sProperty, PROPERTY_ENCOUNTERED_TOTAL))
		return ICCItemPtr(m_EncounterRecord.GetTotalCount());

	else if (strEquals(sProperty, PROPERTY_HULL_TYPE))
		{
		if (IsShipEncounter())
			return ICCItemPtr::Nil();
		else if (m_HullDesc.IsImmutable())
			return ICCItemPtr(CONSTLIT("immutable"));
		else if (m_HullDesc.GetHullType() == CStationHullDesc::hullUnknown)
			return ICCItemPtr::Nil();
		else
			return ICCItemPtr(CStationHullDesc::GetID(m_HullDesc.GetHullType()));
		}

	else if (strEquals(sProperty, PROPERTY_LEVEL_FREQUENCY))
		return ICCItemPtr(GetEncounterDesc().GetLevelFrequency());

	else if (strEquals(sProperty, PROPERTY_MAX_BALANCE))
		{
		if (m_pTrade)
			return CTLispConvert::CreateCurrencyValue(m_pTrade->GetMaxBalance(GetLevel()));
		else
			return ICCItemPtr::Nil();
		}

	else if (strEquals(sProperty, PROPERTY_PRIMARY_WEAPON))
		{
		CItem PrimaryWeapon = GetPrimaryWeapon();
		return (PrimaryWeapon.IsEmpty() ? ICCItemPtr(ICCItem::Nil) : ICCItemPtr(PrimaryWeapon.GetNounPhrase()));
		}

	else if (strEquals(sProperty, PROPERTY_PRIMARY_WEAPON_LEVEL))
		{
		CItem PrimaryWeapon = GetPrimaryWeapon();
		return (PrimaryWeapon.IsEmpty() ? ICCItemPtr(ICCItem::Nil) : ICCItemPtr(PrimaryWeapon.GetLevel()));
		}

	else if (strEquals(sProperty, PROPERTY_SHOWS_UNEXPLORED_ANNOTATION))
		return ICCItemPtr(ShowsUnexploredAnnotation());

	else if (strEquals(sProperty, PROPERTY_SOVEREIGN))
		return (m_pSovereign ? ICCItemPtr(m_pSovereign->GetUNID()) : ICCItemPtr(ICCItem::Nil));

	else if (strEquals(sProperty, PROPERTY_SOVEREIGN_NAME))
		{
		if (m_pSovereign == NULL)
			return ICCItemPtr(ICCItem::Nil);

		return m_pSovereign->GetProperty(Ctx, PROPERTY_NAME);
		}
	else if (strEquals(sProperty, PROPERTY_STD_TREASURE))
		{
		Metric rTreasure = (Metric)GetBalancedTreasure();
		if (rTreasure <= 0.0)
			return ICCItemPtr::Nil();

		return ICCItemPtr(rTreasure);
		}
	else if (strEquals(sProperty, PROPERTY_SYSTEM_CRITERIA))
		{
		const CTopologyNodeCriteria *pSystemCriteria;
		if (!GetEncounterDesc().HasSystemCriteria(&pSystemCriteria))
			return ICCItemPtr(ICCItem::Nil);

		if (pSystemCriteria->GetAttributeCriteria().IsEmpty())
			return ICCItemPtr(ICCItem::Nil);

		return ICCItemPtr(pSystemCriteria->GetAttributeCriteria().AsString());
		}

	else if (strEquals(sProperty, PROPERTY_TREASURE_DESIRED_VALUE))
		{
		if (m_pItems == NULL)
			return ICCItemPtr(ICCItem::Nil);

		SItemAddCtx AddItemCtx(GetUniverse());
		CCurrencyAndValue Value = m_pItems->GetDesiredValue(AddItemCtx, GetLevel());
		if (Value.IsEmpty())
			return ICCItemPtr(ICCItem::Nil);

		return ICCItemPtr((DWORD)GetDefaultCurrency().Exchange(Value));
		}

	else if (strEquals(sProperty, PROPERTY_TREASURE_DESIRED_VALUE_LOOP_COUNT))
		{
		if (m_pItems == NULL)
			return ICCItemPtr(ICCItem::Nil);

		SItemAddCtx AddItemCtx(GetUniverse());
		int iLoopCount;
		if (m_pItems->GetDesiredValue(AddItemCtx, GetLevel(), &iLoopCount).IsEmpty())
			return ICCItemPtr(ICCItem::Nil);

		return ICCItemPtr(iLoopCount);
		}

	else if (strEquals(sProperty, PROPERTY_TREASURE_DESIRED_VALUE_SCALE))
		{
		if (m_pItems == NULL)
			return ICCItemPtr(ICCItem::Nil);

		SItemAddCtx AddItemCtx(GetUniverse());
		Metric rScale;
		if (m_pItems->GetDesiredValue(AddItemCtx, GetLevel(), NULL, &rScale).IsEmpty())
			return ICCItemPtr(ICCItem::Nil);

		return ICCItemPtr(rScale);
		}

	else
		return NULL;
	}

bool CStationType::OnHasSpecialAttribute (const CString &sAttrib) const

//	OnHasSpecialAttribute
//
//	Returns TRUE if we have the special attribute

	{
	if (strStartsWith(sAttrib, SPECIAL_IS_ENEMY_OF))
		{
		CString sValue = strSubString(sAttrib, SPECIAL_IS_ENEMY_OF.GetLength());
		DWORD dwSovereign = (DWORD)strToInt(sValue, 0);
		CSovereign *pSovereign = GetUniverse().FindSovereign(dwSovereign);
		if (pSovereign == NULL)
			return false;

		return m_pSovereign->IsEnemy(pSovereign);
		}
	else if (strStartsWith(sAttrib, SPECIAL_SCALE))
		{
		CString sValue = strSubString(sAttrib, SPECIAL_SCALE.GetLength());
		return (ParseScale(sValue) == GetScale());
		}
	else if (strStartsWith(sAttrib, SPECIAL_IS_SHIP_ENCOUNTER))
		{
		CString sValue = strSubString(sAttrib, SPECIAL_IS_SHIP_ENCOUNTER.GetLength());
		return (IsShipEncounter() == strEquals(sValue, SPECIAL_VALUE_TRUE));
		}
	else if (strStartsWith(sAttrib, SPECIAL_IS_STATION_ENCOUNTER))
		{
		CString sValue = strSubString(sAttrib, SPECIAL_IS_STATION_ENCOUNTER.GetLength());
		return (IsStationEncounter() == strEquals(sValue, SPECIAL_VALUE_TRUE));
		}
	else if (strStartsWith(sAttrib, SPECIAL_SIZE_CLASS))
		{
		CString sValue = strSubString(sAttrib, SPECIAL_SIZE_CLASS.GetLength());
		ESizeClass iClass = ParseSizeClass(sValue);
		return IsSizeClass(iClass);
		}
	else
		return false;
	}

void CStationType::OnInitObjectData (CSpaceObject &Obj, CAttributeDataBlock &Data) const

//	OnInitObjectData
//
//	Initializes the object's data

	{
	CXMLElement *pInitialData = m_pDesc->GetContentElementByTag(INITIAL_DATA_TAG);
	if (pInitialData)
		Obj.SetDataFromXML(pInitialData);
	}

void CStationType::OnMarkImages (void)

//	OnMarkImages
//
//	Mark images in use.

	{
	//	Since we are generally loading images for this type, we need to create
	//	a default image selector

	SSelectorInitCtx InitCtx;
	CCompositeImageSelector Selector;
	SetImageSelector(InitCtx, &Selector);

	//	Mark

	MarkImages(Selector, CCompositeImageModifiers());
	}

void CStationType::OnReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from stream
//
//	DWORD		flags
//	CAttributeDataBlock	m_Data

	{
	DWORD dwLoad;
	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	
	bool bEncountered;
	bEncountered =		((dwLoad & 0x00000001) ? true : false);

	//	Load encounter record

	if (Ctx.dwVersion >= 25)
		GetEncounterDesc().ReadFromStream(Ctx);

	if (Ctx.dwVersion >= 19)
		m_EncounterRecord.ReadFromStream(Ctx);
	else
		{
		if (bEncountered)
			m_EncounterRecord.AddEncounter();
		}

	//	Load opaque data

	ReadGlobalData(Ctx);

	//	Clear cache so we recalculate

	if (m_fCalcLevel)
		m_iLevel = 0;

	m_fBalanceValid = false;
	}

void CStationType::OnReinit (void)

//	OnReinit
//
//	Reinitialize the type

	{
	//	If level is calculated, then reset so that we recalculated it.

	if (m_fCalcLevel)
		m_iLevel = 0;

	//	Clear other values so we recalculate

	m_fBalanceValid = false;

	//	Reinitialize

	m_Name.Reinit();
	m_EncounterRecord.Reinit(GetEncounterDesc());
	m_Image.Reinit();
	m_HeroImage.Reinit();
	}

void CStationType::OnShipEncounterCreated (SSystemCreateCtx &CreateCtx, CSpaceObject *pObj, const COrbit &Orbit)

//	OnShipEncounterCreated
//
//	This is called at the end of system create for a ship encounter.
//	pObj is the main ship created (may be NULL).

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_CREATE_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.SetSystemCreateCtx(&CreateCtx);

		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(NULL);
		Ctx.DefineSpaceObject(CONSTLIT("aObj"), pObj);
		Ctx.DefineInteger(CONSTLIT("aEncounterType"), GetUNID());
		Ctx.SaveAndDefineDataVar(NULL);
		Ctx.DefineOrbit(CONSTLIT("aOrbit"), Orbit);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			ReportEventError(ON_CREATE_EVENT, pResult);
		Ctx.Discard(pResult);
		}
	}

void CStationType::OnTopologyInitialized (void)

//	OnTopologyInitialize
//
//	The topology has been initialized.

	{
	//	We take this opportunity to resolve the level of certain encounters
	//	(now that we know the topology).

	GetEncounterDesc().InitLevelFrequency(GetUniverse().GetTopology());
	m_EncounterRecord.Reinit(GetEncounterDesc());
	}

void CStationType::OnUnbindDesign (void)

//	OnUnbindDesign
//
//	Undo binding

	{
	//	Reset comms handler because our inheritance chain might
	//	have changed.

	m_fCommsHandlerInit = false;
	m_CommsHandler.DeleteAll();

	//	Reset encounter overrides

	m_pEncounterDescOverride.Set(NULL);
	}

void CStationType::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Writes out the variable portions of the item type
//
//	DWORD		flags

	{
	DWORD dwSave;

	dwSave = 0;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	GetEncounterDesc().WriteToStream(pStream);
	m_EncounterRecord.WriteToStream(pStream);
	}

bool CStationType::OverrideEncounterDesc (const CXMLElement &Override, CString *retsError)

//	OverrideEncounterDesc
//
//	Overrides the current encounter descriptor with the given XML element. This
//	must be done during Bind because we do not persist the override.

	{
	if (!m_pEncounterDescOverride)
		m_pEncounterDescOverride.Set(new CStationEncounterDesc);

	if (!m_pEncounterDescOverride->InitAsOverride(m_EncounterDesc, Override, retsError))
		return false;

	return true;
	}

void CStationType::PaintAnimations (CG32bitImage &Dest, int x, int y, int iTick)

//	PaintAnimations
//
//	Paint animations

	{
	int i;

	for (i = 0; i < m_iAnimationsCount; i++)
		{
		m_pAnimations[i].m_Image.PaintImage(Dest,
				x + m_pAnimations[i].m_x,
				y - m_pAnimations[i].m_y,
				iTick,
				0);
		}
	}

void CStationType::PaintDevicePositions (CG32bitImage &Dest, int x, int y)

//	PaintDevicePositions
//
//	Paints the position of all the devices

	{
	const int ARC_RADIUS = 30;
	CG32bitPixel rgbLine = CG32bitPixel(255, 255, 0);
	CG32bitPixel rgbArc = CG32bitPixel(rgbLine, 128);

	SSelectorInitCtx InitCtx;
	CCompositeImageSelector Selector;
	m_Image.InitSelector(InitCtx, &Selector);

	int iScale = GetImage(Selector, CCompositeImageModifiers()).GetImageViewportSize();

	for (int i = 0; i < m_AverageDevices.GetCount(); i++)
		{
		auto &DeviceDesc = m_AverageDevices.GetDeviceDesc(i);

		int xPos;
		int yPos;
		if (DeviceDesc.b3DPosition)
			C3DConversion::CalcCoord(iScale, DeviceDesc.iPosAngle, DeviceDesc.iPosRadius, DeviceDesc.iPosZ, &xPos, &yPos);
		else
			C3DConversion::CalcCoordCompatible(DeviceDesc.iPosAngle, DeviceDesc.iPosRadius, &xPos, &yPos);

		int xCenter = x + xPos;
		int yCenter = y + yPos;

		Dest.DrawDot(xCenter, yCenter, rgbLine, markerMediumCross);

		//	Draw fire arc

		if (!DeviceDesc.bOmnidirectional)
			{
			int iMinFireArc = DeviceDesc.iMinFireArc;
			int iMaxFireArc = DeviceDesc.iMaxFireArc;

			CGDraw::Arc(Dest, CVector(xCenter, yCenter), ARC_RADIUS, mathDegreesToRadians(iMinFireArc), mathDegreesToRadians(iMaxFireArc), ARC_RADIUS / 2, rgbArc);

			int iFireAngle = AngleMiddle(iMinFireArc, iMaxFireArc);
			CVector vDir = PolarToVector(iFireAngle, ARC_RADIUS);
			CGDraw::Line(Dest, xCenter, yCenter, xCenter + (int)vDir.GetX(), yCenter - (int)vDir.GetY(), 1, rgbLine);
			}

		//	If short-range, draw range circle

		if (DeviceDesc.iMaxFireRange)
			{
			int iRadius = mathRound(DeviceDesc.iMaxFireRange * LIGHT_SECOND / g_KlicksPerPixel);
			CGDraw::CircleOutline(Dest, xCenter, yCenter, iRadius, 1, rgbLine);
			}
		}

	//	If we have satellites, recurse

	TArray<SSatImageDesc> SatImages = CalcSegmentDesc();
	for (int i = 0; i < SatImages.GetCount(); i++)
		{
		SatImages[i].pType->PaintDevicePositions(Dest, x + SatImages[i].xOffset, y - SatImages[i].yOffset);
		}
	}

void CStationType::PaintDockPortPositions (CG32bitImage &Dest, int x, int y)

//	PaintDockPortPositions
//
//	Paints the position of all the docking ports for this type.

	{
	//	Get an image

	SSelectorInitCtx InitCtx;
	CCompositeImageSelector Selector;
	SetImageSelector(InitCtx, &Selector);
	const CObjectImageArray &Image = GetImage(Selector, CCompositeImageModifiers());

	//	Compute some image properties

	int iScale = Image.GetImageViewportSize();
	int iStationRotation = m_Image.GetActualRotation(Selector);

	//	We need to initialize a docking ports structure

	CDockingPorts Ports;
	Ports.InitPortsFromXML(NULL, m_pDesc, iScale);

	//	Paint all ports

	Ports.DebugPaint(Dest, x, y, iStationRotation, iScale);

	//	If we have satellites, recurse

	TArray<SSatImageDesc> SatImages = CalcSegmentDesc();
	for (int i = 0; i < SatImages.GetCount(); i++)
		{
		SatImages[i].pType->PaintDockPortPositions(Dest, x + SatImages[i].xOffset, y - SatImages[i].yOffset);
		}
	}

ScaleTypes CStationType::ParseScale (const CString sValue)

//	ParseScale
//
//	Parses a scale value

	{
	if (strEquals(sValue, STAR_SCALE))
		return scaleStar;
	else if (strEquals(sValue, WORLD_SCALE))
		return scaleWorld;
	else if (strEquals(sValue, STRUCTURE_SCALE))
		return scaleStructure;
	else if (strEquals(sValue, SHIP_SCALE))
		return scaleShip;
	else if (strEquals(sValue, FLOTSAM_SCALE))
		return scaleFlotsam;
	else
		return scaleNone;
	}

CStationType::ESizeClass CStationType::ParseSizeClass (const CString sValue)

//	ParseSizeClass
//
//	Parses size class values

	{
	char *pPos = sValue.GetASCIIZPointer();

	if (*pPos >= 'a' && *pPos <= 'm')
		return (ESizeClass)(*pPos - 'a' + worldSizeA);
	else if (*pPos >= 'A' && *pPos <= 'M')
		return (ESizeClass)(*pPos - 'A' + worldSizeA);
	else
		return sizeNone;
	}

void CStationType::Reinit (void)

//	Reinit
//
//	Reinitialize global data

	{
	}

void CStationType::SetImageSelector (SSelectorInitCtx &InitCtx, CCompositeImageSelector *retSelector)

//	SetImageSelector
//
//	Sets the image for the station (if necessary). Also, sets the variant

	{
	if (m_ShipWrecks.GetCount())
		{
		DWORD dwShipwreckID = m_ShipWrecks[mathRandom(0, m_ShipWrecks.GetCount()-1)];
		CShipClass *pClass = GetUniverse().FindShipClass(dwShipwreckID);
		if (pClass == NULL)
			return;

		retSelector->AddShipwreck(DEFAULT_SELECTOR_ID, pClass);
		}
	else
		{
		m_Image.InitSelector(InitCtx, retSelector);
		}
	}

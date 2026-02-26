//	TSEEngineDefs.h
//
//	Engine definitions
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	LANGUAGE ELEMENTS ----------------------------------------------------------

//	core.charges
//
//	This noun is used to describe the charges for an item.
//
#define LANGID_CORE_CHARGES						CONSTLIT("core.charges")

//	core.descLore
//
//	Ship Class: A lore description of the ship class, for codex/encyclopedia
//	screens
//
#define LANGID_CORE_DESC_LORE					CONSTLIT("core.descLore")

//	core.desc
//
//	Ship Class: A gameplay description of the ship class, usually displayed
//	when picking a new ship.
//
#define LANGID_CORE_DESC_PLAYER					CONSTLIT("core.desc")

//	core.dockingRequestDenied
//
//	An object may define this language element to override the default text for
//	a denied docking request.
//
#define LANGID_DOCKING_REQUEST_DENIED			CONSTLIT("core.dockingRequestDenied")

//	core.help
//
//	Power: A short line describing the power. Shown in the invoke menu.
//
#define LANGID_CORE_HELP						CONSTLIT("core.help")

//	core.launchedBy
//
//	This is a text pattern used to override the default text for the launcher of
//	a given shot. The pattern should be something like:
//
//	Launched by %launcherName%
//
#define LANGID_CORE_LAUNCHED_BY					CONSTLIT("core.launchedBy")

//	core.mapDesc???
//
//	These language elements are used to determine galactic map descriptions for
//	objects.
//
#define LANGID_CORE_MAP_DESC                    CONSTLIT("core.mapDesc")
#define LANGID_CORE_MAP_DESC_ABANDONED          CONSTLIT("core.mapDescAbandoned")
#define LANGID_CORE_MAP_DESC_ABANDONED_CUSTOM	CONSTLIT("core.mapDescAbandonedCustom")
#define LANGID_CORE_MAP_DESC_CUSTOM				CONSTLIT("core.mapDescCustom")
#define LANGID_CORE_MAP_DESC_EXTRA              CONSTLIT("core.mapDescExtra")
#define LANGID_CORE_MAP_DESC_MAIN				CONSTLIT("core.mapDescMain")

//	core.reference
//
//	This is the reference string for the item.
//
#define LANGID_CORE_REFERENCE					CONSTLIT("core.reference")

//	PROPERTIES -----------------------------------------------------------------

//	core.defaultInteraction
//
//	Adventure Property: Evaluated at bind time. The default interaction controls
//	the probability of shots hitting each other. If an interaction is defined by
//	the weapon then we always use that. Otherwise, we check this value. If this
//	property is -1, then we calculate an interaction based on shot properties.
//
//	Otherwise, we use the value as the interaction value. 0 = no interaction,
//	100 = shots always hit each other.
//
#define PROPERTY_CORE_DEFAULT_INTERACTION		CONSTLIT("core.defaultInteraction")

//	core.defaultShotHP
//
//	Adventure Property: Evaluated at bind time. The default shot HP is the 
//	default number of HP for shots (if the weapon does not specify shot HP).
//	A value of -1 means calculate shot HP based on shot properties.
//
#define PROPERTY_CORE_DEFAULT_SHOT_HP			CONSTLIT("core.defaultShotHP")

//	core.enhancement
//
//	Item Property: This property is used for items that confer enhancement on
//	other items (e.g., Kinetic Enhancer).
//
#define PROPERTY_CORE_ENHANCEMENT				CONSTLIT("core.enhancement")

//	core.gameStats
//
//	Type Property: Evaluated to generate stats. Return a gameStat struct or a 
//	list of gameStat structs.
//
//	gameStat:
//		descID: Stat description language element (translated on the type).
//		sectionID: Stat section language element
//		sort: Sort order string.
//		valueID: Stat value language element (optional)
//
//		desc: Stat description (only if no descID).
//		section: Stat section (only if no sectionID).
//		value: Stat value (only if no valueID).
//
#define PROPERTY_CORE_GAME_STATS				CONSTLIT("core.gameStats")

//	core.damageMethod.system
//
//	Adventure Property <Constant>: Evaluated at bind time. This property selects what damage
//	method system is to be used by this adventure.
//
//	Valid options:
//		WMD:	Use the legacy WMD system
// 
//	Options currently pending implementation:
//		physicalizedDamageMethods
//
#define PROPERTY_CORE_DMG_METHOD_SYSTEM					CONSTLIT("core.damageMethod.system")
#define VALUE_CORE_DMG_METHOD_SYSTEM_WMD				CONSTLIT("WMD")
#define VALUE_CORE_DMG_METHOD_SYSTEM_DAMAGE_METHODS		CONSTLIT("physicalizedDamageMethods")

//	code.damageMethod.minDamage
//
//	Adventure Property <Constant>: Evaluated at bind time. This property defines the default
//	minimum damage that can result from damage method adjustment.
// 
//	Returns a floating point 0.0 or greater (values less than 1 are treated stochastically)
//
#define PROPERTY_CORE_DMG_METHOD_MIN_DAMAGE				CONSTLIT("core.damageMethod.minDamage")

//	core.damageMethod.fortify.[Target]
//
//	Adventure Property <Constant>: Evaluated at bind time. These properties control defaults
//	for damage method (WMD, or future systems) fortification adjustment
//	
//	Fortification of 0 means that the damage adjustment for the damage method is 1.0 regardless
//	of level
//	Fortification of 1.0 means that the damage adjustment for the damage method follows the
//	adventure defined curve for that damage method
//	Values of fortification > 1 mean that damage without that damage method is penalized more
//	than the adventure defined curve
//		The curve also become biased towards having high amounts of that damage method
//	Values of fortification < 1 mean that damage without that damage method is penalized less
//  than the adventure defined curve
//		The curve also becomes biased towards rewarding any amount of that damage method
//	Fortification cannot be negative
// 
//	[Target] may be one of the following types:
//
//	Item:			Sets default adj for armor and shield item types
//	Ship:			Sets default adj for ships of various types
//	Station:		Sets default adj for stations of various types
//
//	These properties should return a struct of damageMethod structs.
// 
//	The damageMethod struct depends on the damage method system selected using
//		core.damageMethod.system (See core.damageMethod.minAdj for more details)
// 
//  itemDamageMethod struct:
//		armor:		Sets default adj for armor types
//			(WMD)
//			WMD:	0.0
//			(physicalizedDamageMethods)
//			crush:	0.0
//			pierce:	0.0
//			shred:	0.0
//		shield:		Sets default adj for shield types
//			(WMD)
//			WMD:	0.0
//			(physicalizedDamageMethods)
//			crush:	0.0
//			pierce:	0.0
//			shred:	0.0
// 
//	shipDamageMethod struct:
//		armor:
//			critical:
//				(WMD)
//				WMD:	0.0
//				(physicalizedDamageMethods)
//				crush:	0.0
//				pierce:	0.0
//				shred:	0.0
//			uncrewedCritical:
//				(WMD)
//				WMD:	0.0
//				(physicalizedDamageMethods)
//				crush:	0.0
//				pierce:	0.0
//				shred:	0.0
//			nonCritical:
//				(WMD)
//				WMD:	0.0
//				(physicalizedDamageMethods)
//				crush:	0.0
//				pierce:	0.0
//				shred:	0.0
//			nonCriticalDestructionChance: 0.05	;;	This expresses the base chance of dying
//			nonCriticalDestruction:	;;	This expresses the additional chance of dying based on relative damage of the hit
//				(WMD)
//				WMD:	1.0
//				(physicalizedDamageMethods)
//				crush:	0.0
//				pierce:	0.0
//				shred:	1.0
//		compartments:
//			(see ECompartmentTypes)
//			general:
//				(WMD)
//				WMD:	1.0
//				(physicalizedDamageMethods)
//				crush:	0.0
//				pierce:	0.0
//				shred:	1.0
//			mainDrive:
//				(WMD)
//				WMD:	1.0
//				(physicalizedDamageMethods)
//				crush:	0.0
//				pierce:	0.0
//				shred:	1.0
//			cargo:
//				(WMD)
//				WMD:	1.0
//				(physicalizedDamageMethods)
//				crush:	1.0
//				pierce:	0.0
//				shred:	0.0
//			uncrewed:
//				(WMD)
//				WMD:	1.0
//				(physicalizedDamageMethods)
//				crush:	1.0
//				pierce:	0.0
//				shred:	0.0
// 
//	stationDamageMethod struct:
//		hull:
//			(see CStationHullDesc::EHullTypes)
//			single:
//				(WMD)
//				WMD:	0.0
//				(physicalizedDamageMethods)
//				crush:	0.0
//				pierce:	0.0
//				shred:	0.0
//			multi:
//				(WMD)
//				WMD:	1.0
//				(physicalizedDamageMethods)
//				crush:	0.0
//				pierce:	0.0
//				shred:	1.0
//			asteroid:
//				(WMD)
//				WMD:	1.0		Note: Hardcoded to use mining or WMD
//				(physicalizedDamageMethods)
//				crush:	.31
//				pierce:	.31
//				shred:	0.0
//			underground:
//				(WMD)
//				WMD:	1.0		Note: Hardcoded to use mining
//				(physicalizedDamageMethods)
//				crush:	1.0
//				pierce:	0.0
//				shred:	0.0
//			uncrewed:
//				(WMD)
//				WMD:	1.0
//				(physicalizedDamageMethods)
//				crush:	1.0
//				pierce:	0.0
//				shred:	0.0
//			armor:
//				(WMD)
//				WMD:	1.0
//				(physicalizedDamageMethods)
//				crush:	0.0
//				pierce:	1.0
//				shred:	0.0
//
#define PROPERTY_CORE_DMG_METHOD_ITEM					CONSTLIT("core.damageMethod.item")
#define KEY_CORE_DMG_METHOD_ITEM_ARMOR						CONSTLIT("armor")
#define KEY_CORE_DMG_METHOD_ITEM_SHIELD						CONSTLIT("shield")

#define PROPERTY_CORE_DMG_METHOD_SHIP					CONSTLIT("core.damageMethod.ship")
#define KEY_CORE_DMG_METHOD_SHIP_ARMOR						CONSTLIT("armor")
#define KEY_CORE_DMG_METHOD_SHIP_ARMOR_CRITICAL						CONSTLIT("critical")
#define KEY_CORE_DMG_METHOD_SHIP_ARMOR_CRITICAL_UNCREWED			CONSTLIT("criticalUncrewed")
#define KEY_CORE_DMG_METHOD_SHIP_ARMOR_NONCRITICAL					CONSTLIT("nonCritical")
#define KEY_CORE_DMG_METHOD_SHIP_ARMOR_NONCRITICAL_DESTRUCTION		CONSTLIT("nonCriticalDestruction")
#define KEY_CORE_DMG_METHOD_SHIP_ARMOR_NONCRITICAL_DESTRUCTION_CHANCE	CONSTLIT("nonCriticalDestructionChance")
#define KEY_CORE_DMG_METHOD_SHIP_COMPARTMENT				CONSTLIT("compartments")
#define KEY_CORE_DMG_METHOD_SHIP_COMPARTMENT_GENERAL				CONSTLIT("general")
#define KEY_CORE_DMG_METHOD_SHIP_COMPARTMENT_MAIN_DRIVE				CONSTLIT("mainDrive")
#define KEY_CORE_DMG_METHOD_SHIP_COMPARTMENT_CARGO					CONSTLIT("cargo")
#define KEY_CORE_DMG_METHOD_SHIP_COMPARTMENT_UNCREWED				CONSTLIT("uncrewed")

#define PROPERTY_CORE_DMG_METHOD_STATION				CONSTLIT("core.damageMethod.station")
#define KEY_CORE_DMG_METHOD_STATION_HULL					CONSTLIT("hull")
#define KEY_CORE_DMG_METHOD_STATION_HULL_SINGLE						CONSTLIT("single")
#define KEY_CORE_DMG_METHOD_STATION_HULL_MULTI						CONSTLIT("multi")
#define KEY_CORE_DMG_METHOD_STATION_HULL_ASTEROID					CONSTLIT("asteroid")
#define KEY_CORE_DMG_METHOD_STATION_HULL_UNDERGROUND				CONSTLIT("underground")
#define KEY_CORE_DMG_METHOD_STATION_HULL_UNCREWED					CONSTLIT("uncrewed")
#define KEY_CORE_DMG_METHOD_STATION_HULL_ARMOR						CONSTLIT("armor")

#define KEY_CORE_DMG_METHOD_WMD							CONSTLIT("wmd")
#define KEY_CORE_DMG_METHOD_CRUSH						CONSTLIT("crush")
#define KEY_CORE_DMG_METHOD_PIERCE						CONSTLIT("pierce")
#define KEY_CORE_DMG_METHOD_SHRED						CONSTLIT("shred")

//	core.hide???Immune
//
//	Adventure Property: Evaluated at bind time. These properties control whether
//	the UI should hide these armor immunities.
//
#define PROPERTY_CORE_HIDE_DISINTEGRATION_IMMUNE	CONSTLIT("core.hideDisintegrationImmune")
#define PROPERTY_CORE_HIDE_IONIZE_IMMUNE		CONSTLIT("core.hideIonizeImmune")
#define PROPERTY_CORE_HIDE_RADIATION_IMMUNE		CONSTLIT("core.hideRadiationImmune")
#define PROPERTY_CORE_HIDE_SHATTER_IMMUNE		CONSTLIT("core.hideShatterImmune")

//	core.item.xmlMassToRealVolume
//
//	Adventure Property: Evaluated at bind time. This property exists for adventures
//	that balance cargo/mass divergently from pre-2.0 Stars of the Pilgrim, but still
//	wish to convert items from old mods/extensions.
//	This is relevant because in legacy Stars of the Pilgrim, cargo holds assumed items
//	have a density much less than water, so the max mass of the cargo holds was divided
//	by approximately 4. This computation converts the xml mass
//	from kg to tons BEFORE applying this ratio.
// 
//	Notice: if this is not the inverse of core.item.defaultDensity, you will need to
//	update your armor mass classes accordingly or switch to using armor size classes
//	Notice: it is strongly recommended to switch to armor size classes instead of
//	using the legacy armor mass classes
//	
//	Example values:
//	1.0 = volume is set to the mass provided in the XML
//	1.5 = volume is set to 1.5x the mass provided in the XML
//	
//	Default: 1.0
//
#define PROPERTY_CORE_ITEM_LEGACY_MASS_TO_VOLUME	CONSTLIT("core.item.xmlMassToRealVolume")

//	core.item.defaultDensity
//
//	Adventure Property: Evaluated at bind time. This property allows adventures to
//	assume a specific density for items that do not provide a volume. This calculation
//	Is applied after core.item.legacyMassToValue to compute the actual mass the item
//	should be converted to in this adventure.
// 
//	Notice: if this is not the inverse of core.item.defaultDensity, you will need to
//	update your armor mass classes accordingly or switch to using armor size classes
//	Notice: it is strongly recommended to switch to armor size classes instead of
//	using the legacy armor mass classes
//
//	Example values:
//	1.0 = mass is set to the computed volume (equal density to water)
//	0.125 = mass is set to one quarter the computed volume
// 
//	Default: 1.0
//
#define PROPERTY_CORE_ITEM_DEFAULT_DENSITY			CONSTLIT("core.item.defaultDensity")

//	core.item.shield.idlePowerAdj
//
//	Adventure Property: Evaluated at bind time. This property controls the
//	power consumption of shields when they are idle (not regenerating) as
//	a fraction of active power consumption.
//	0.0 = no power consumption, 1.0 = full power consumption
//
#define PROPERTY_CORE_ITEM_SHIELD_IDLE_POWER_ADJ	CONSTLIT("core.item.shield.idlePowerAdj")

//	core.miningDifficulty
//
//	Object Property: Evaluated when mining to determine how hard it is to mine
//	0 = very easy; 100 = very hard. This value is usually computed based on the
//	asteroid type, but an object can override it with this property.
//
#define PROPERTY_CORE_MINING_DIFFICULTY			CONSTLIT("core.miningDifficulty")

//	core.namePattern
//
//	Type Property: For generic type definitions (<Type>), this defines the name
//	of the type. This is mostly used for debugging and TransData lists.
//
#define PROPERTY_CORE_NAME_PATTERN				CONSTLIT("core.namePattern")

//	core.noBiologicalEffects
//
//	Object Property: If this property is True for an object, the object will not
//	be affected by biological attacks targeted at crew/pilot. For example, it 
//	makes a ship immune to neuroplague launcher pods.
//
#define PROPERTY_CORE_NO_BIOLOGICAL_EFFECTS		CONSTLIT("core.noBiologicalEffects")

//	core.noRadiationDeath
//
//	Object Property: If this property is True for an object, the object will not
//	die from radiation poisoning. NOTE: This is separate from radiation 
//	immunity.
//
#define PROPERTY_CORE_NO_RADIATION_DEATH		CONSTLIT("core.noRadiationDeath")

//	core.noSalvage
//
//	Object Property: Evaluated when the AI is looking for objects to salvage.
//	If the property is non-Nil then we do not try to salvage it.
//
#define PROPERTY_CORE_NO_SALVAGE				CONSTLIT("core.noSalvage")

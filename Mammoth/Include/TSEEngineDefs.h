//	TSEEngineDefs.h
//
//	Engine definitions
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

//	LANGUAGE ELEMENTS ----------------------------------------------------------

//	core.charges
//
//	This noun is used to describe the charges for an item.

#define LANGID_CORE_CHARGES						CONSTLIT("core.charges")

//	core.desc
//
//	Ship Class: A description of the ship class, usually displayed when picking
//	a new ship.

#define LANGID_CORE_DESC						CONSTLIT("core.desc")

//	core.dockingRequestDenied
//
//	An object may define this language element to override the default text for
//	a denied docking request.

#define LANGID_DOCKING_REQUEST_DENIED			CONSTLIT("core.dockingRequestDenied")

//	core.launchedBy
//
//	This is a text pattern used to override the default text for the launcher of
//	a given shot. The pattern should be something like:
//
//	Launched by %launcherName%

#define LANGID_CORE_LAUNCHED_BY					CONSTLIT("core.launchedBy")

//	core.mapDesc???
//
//	These language elements are used to determine galactic map descriptions for
//	objects.

#define LANGID_CORE_MAP_DESC                    CONSTLIT("core.mapDesc")
#define LANGID_CORE_MAP_DESC_ABANDONED          CONSTLIT("core.mapDescAbandoned")
#define LANGID_CORE_MAP_DESC_ABANDONED_CUSTOM	CONSTLIT("core.mapDescAbandonedCustom")
#define LANGID_CORE_MAP_DESC_CUSTOM				CONSTLIT("core.mapDescCustom")
#define LANGID_CORE_MAP_DESC_EXTRA              CONSTLIT("core.mapDescExtra")
#define LANGID_CORE_MAP_DESC_MAIN				CONSTLIT("core.mapDescMain")

//	core.reference
//
//	This is the reference string for the item.

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

#define PROPERTY_CORE_DEFAULT_INTERACTION		CONSTLIT("core.defaultInteraction")

//	core.defaultShotHP
//
//	Adventure Property: Evaluated at bind time. The default shot HP is the 
//	default number of HP for shots (if the weapon does not specify shot HP).
//	A value of -1 means calculate shot HP based on shot properties.

#define PROPERTY_CORE_DEFAULT_SHOT_HP			CONSTLIT("core.defaultShotHP")

//	core.enhancement
//
//	Item Property: This property is used for items that confer enhancement on
//	other items (e.g., Kinetic Enhancer).

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

#define PROPERTY_CORE_GAME_STATS				CONSTLIT("core.gameStats")

//	core.hide???Immune
//
//	Adventure Property: Evaluated at bind time. These properties control whether
//	the UI should hide these armor immunities.

#define PROPERTY_CORE_HIDE_DISINTEGRATION_IMMUNE	CONSTLIT("core.hideDisintegrationImmune")
#define PROPERTY_CORE_HIDE_IONIZE_IMMUNE		CONSTLIT("core.hideIonizeImmune")
#define PROPERTY_CORE_HIDE_RADIATION_IMMUNE		CONSTLIT("core.hideRadiationImmune")
#define PROPERTY_CORE_HIDE_SHATTER_IMMUNE		CONSTLIT("core.hideShatterImmune")

//	core.miningDifficulty
//
//	Object Property: Evaluated when mining to determine how hard it is to mine
//	0 = very easy; 100 = very hard. This value is usually computed based on the
//	asteroid type, but an object can override it with this property.

#define PROPERTY_CORE_MINING_DIFFICULTY			CONSTLIT("core.miningDifficulty")

//	core.noBiologicalEffects
//
//	Object Property: If this property is True for an object, the object will not
//	be affected by biological attacks targeted at crew/pilot. For example, it 
//	makes a ship immune to neuroplague launcher pods.

#define PROPERTY_CORE_NO_BIOLOGICAL_EFFECTS		CONSTLIT("core.noBiologicalEffects")

//	core.noRadiationDeath
//
//	Object Property: If this property is True for an object, the object will not
//	die from radiation poisoning. NOTE: This is separate from radiation 
//	immunity.

#define PROPERTY_CORE_NO_RADIATION_DEATH		CONSTLIT("core.noRadiationDeath")

//	core.noSalvage
//
//	Object Property: Evaluated when the AI is looking for objects to salvage.
//	If the property is non-Nil then we do not try to salvage it.

#define PROPERTY_CORE_NO_SALVAGE				CONSTLIT("core.noSalvage")

//	TSEVersions.h
//
//	Version numbers and history
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

constexpr DWORD API_VERSION =							56;
constexpr DWORD UNIVERSE_SAVE_VERSION =					41;
constexpr DWORD SYSTEM_SAVE_VERSION =					214;

//	Uncomment out the following define when building a stable release

//#define TRANSCENDENCE_STABLE_RELEASE

//	API VERSION HISTORY ---------------------------------------------------
//
//	 0: Unknown version (0.9 or older)
//
//	 1 "1": 0.95-0.96b
//		Added support for extensions
//		API Version specified as version="1"
//
//	 2 "2": 0.97
//		Changed gStation to gSource
// 
//	 3 "1.0": 1.0/1.01
//      API Version specified as version="1.0"
//		<SmokeTrail>: emitSpeed fixed (used in klicks per tick instead of per second)
//		<Weapon>: shield damage modifier uses a different calculation with more bits for dmg
// 
//	 4 "1.1": 1.02
//      API Version specified as version="1.1"
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=3174
// 
//	 5 "1.1": 1.03
//      API Version specified as version="1.1"
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=3300
// 
//	 6 "1.1": 1.04
//      API Version specified as version="1.1"
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=3563
// 
//	 7 "1.1": 1.05
//      API Version specified as version="1.1"
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=3956
// 
//	 8 "1.1": 1.06
//      API Version specified as version="1.1"
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=4292
// 
//	 9 "1.1": 1.07
//      API Version specified as version="1.1"
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=4612
// 
//	 10 "1.1": 1.08
//      API Version specified as version="1.1"
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=4889
// 
//	 11 "1.1": 1.08b(?)
//      API Version specified as version="1.1"
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=4912
// 
//   12: (1.08c/1.08d)/1.08e
//		1.08c and 1.08d have savegame corruption issues, do not use them.
//      API Version specified as APIVersion="12"
//      <TranscendenceLibrary>: Libraries added
//		<TranscendenceAdventure><TranscendenceExtension>:
//			If no library is selected the compatibility library is
//			automatically loaded.
//	 12 (later): 1.08g-1.08l (?)
//		<MissionType>
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=5387
// 
//	 13: 1.2 Alpha 1
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=5667
// 
//   14: 1.2 Beta 1
//		Transcribed from here: https://wiki.kronosaur.com/doku.php/modding/xml/api_version?s[]=api
//		<EffectType>: (new)
//			Can now specify effect design types for creating dynamic effects
//		<EnhancementAbilities>: (new)
//			type="..."
//				User defined string representing the category of enhancement.
//				Enhancements of the same "type" do not stack.
//			criteria="..."
//				Criteria string limiting what sorts of devices can be enhanced.
//			enhancement="{enhancementCode}:{n}"
//				see shpEnhanceItem
//		<MissionType>:
//			maxAppearing=[int]
//				limits the number of times the mission can be assigned
//		<ShipClass>:
//			thrustRatio=[float]
//				now available in the <ShipClass>
//			<Armor>:
//				now accepts the following parameters instead of individual elements:
//					armorID=[UNID]: the UNID of the armor to use on this ship
//					count=[int]: the number of armor segments to have on this ship
//			<Interior>:
//				Allows for definition of <Compartment> elements that have
//				separate HP pools from the main interior. Damaging them applies different
//				debuffs
//				type="cargo|general|mainDrive":
//					cargo: causes a chance for cargo to be destroyed
//					general: no effect, this is essentially a buffer of non-critical HP
//					mainDrive: causes a chance for max speed to be halved
//		Functions:
//			cnvDrawLine: Draws a line on a <Canvas>
//			objGetArmorRepairPrice:
//		ObjectAscension:
//		ObjectEvents:
//			<onPlayerBlacklisted>: fires on a station when a station blackliists a player
//			<onObjBlacklistedPlayer>: fires when a registered object blacklists the player,
//				must use objRegisterForEvents
//		ObjectProperties:
//			'playerBlacklisted: Returns True/Nil if the player is blacklisted by a station
//			'playerMissionsGiven: The number of missions this object has assigned the player
//			'underAttack: same as (objIsUnderAttack)
//			"property:{propertyName}" is now a valid criteria filter for boolean properties
//				(Note: ItemType is not supported until API 24)
//
//   15: 1.2 Beta 2
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=5938
//
//   16: 1.2 Beta 3?
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=5978
//
//   17: 1.2 Beta 4?
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=6061
//
//   18: 1.2 RC 1?
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=6102
//
//   19: 1.2?
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=6128
//
//   20: 1.3 Beta 1
//		https://multiverse.kronosaur.com/news.hexm?id=258
// 
//	 21: 1.3 Beta 2
//		https://multiverse.kronosaur.com/news.hexm?id=434
// 
//   22: 1.3 RC 1/1.3
//		https://multiverse.kronosaur.com/news.hexm?id=532
//
//	 23: 1.5 Beta 1
//		https://multiverse.kronosaur.com/news.hexm?id=1063
// 
//   24: 1.5 RC 1/1.5
//		https://multiverse.kronosaur.com/news.hexm?id=1096
// 
//   25: 1.6 Beta 2
//		Tlisp supports basic floating point math:
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=7116
// 
//   26: 1.6 Beta 3
//		https://multiverse.kronosaur.com/news.hexm?id=1364
// 
//   27: 1.6 Beta 5
//		https://forums.kronosaur.com/viewtopic.php?f=15&t=7226
// 
//   28: 1.6 RC 1?
//		https://ministry.kronosaur.com/program.hexm?id=1&status=closed&tag=28
// 
//   29: 1.7 Preview
//		http://ministry.kronosaur.com/record.hexm?id=8048
// 
//   30: 1.7 Alpha 2
//		http://ministry.kronosaur.com/record.hexm?id=54396
// 
//   31: 1.7 Beta 1
//		http://ministry.kronosaur.com/record.hexm?id=57824
// 
//   32: 1.7 Beta 3
//		http://ministry.kronosaur.com/record.hexm?id=61158
// 
//   33: 1.7 Beta 4
//		http://ministry.kronosaur.com/record.hexm?id=62046
// 
//   34: 1.7 Beta 5
//		http://ministry.kronosaur.com/record.hexm?id=66822
//
//	 35: 1.7 Beta 6/1.7 
//		https://ministry.kronosaur.com/record.hexm?id=68066
// 
//   36: 1.8 Alpha 1
//		https://ministry.kronosaur.com/record.hexm?id=71132
// 
//   37: 1.8 Alpha 2
//		https://ministry.kronosaur.com/record.hexm?id=72675
// 
//   38: 1.8 Alpha 3
//		https://ministry.kronosaur.com/record.hexm?id=73457
// 
//   39: 1.8 Alpha 4
//		https://ministry.kronosaur.com/record.hexm?id=74282
// 
//   40: 1.8 Beta 1
//		https://ministry.kronosaur.com/record.hexm?id=76059
//  
//   41: 1.8 Beta 2
//		https://ministry.kronosaur.com/record.hexm?id=78005
// 
//   42: 1.8 Beta 3
//		https://ministry.kronosaur.com/record.hexm?id=80464
// 
//   43: 1.8 Beta 4
//		https://ministry.kronosaur.com/record.hexm?id=82308
// 
//   44: 1.8 Beta 5/1.8
//		https://ministry.kronosaur.com/record.hexm?id=84283
// 
//   45: 1.9 Alpha 1
//		https://ministry.kronosaur.com/record.hexm?id=86399
// 
//   46: 1.9 Alpha 2
//		https://ministry.kronosaur.com/record.hexm?id=88044
// 
//	 47: 1.9 Alpha 3
//		https://ministry.kronosaur.com/record.hexm?id=88252
// 
//   48: 1.9 Alpha 4
//		https://ministry.kronosaur.com/record.hexm?id=88609
// 
//   49: 1.9 Beta 1
//		https://ministry.kronosaur.com/record.hexm?id=90206
// 
//	 50: 1.9 Beta 2
//		https://ministry.kronosaur.com/record.hexm?id=90690
// 
//	 51: 1.9 Beta 3
//		https://ministry.kronosaur.com/record.hexm?id=93663
// 
//   52: 1.9 Beta 4
//		https://ministry.kronosaur.com/record.hexm?id=94300
// 
//	 53: 1.9
//		https://ministry.kronosaur.com/record.hexm?id=97615
//
//	 54: 2.0 Alpha 1
//		<CoreLibrary>:
//			Added new CoreLibrary: Compatibility UNID Library (0x00710000)
//				Compatibility UNID Library contains pre-API 54 legacy aliases
//				for older mods, and is automatically loaded for any pre-API 54
//				library/adventure/extension.
//				To continue using the legacy aliases in API 54, please explicitly
//				include the Compatibility UNID Library:
//					<Library unid="&CMPU_unidCompatibilityUNIDLibrary;"/>
//		<ItemType><Weapon><Missile>:
//			detonateOnDestroyed="true|false"
//				Projectile will fragment even if it is killed
//				Default: "true"
//			noDetonationOnImpact="true|false"
//				Projectile will not fragment if it hits the target
//				Default: "false"
//			proximityTriggerOnTargetOnly="true|false"
//				Proximity fragmentation can only be triggered by the intended target
//				Default: "false"
//			proximitySensorArc=[int 0-360]
//				Sets the angle relative to the front of the munition within which
//				it will consider targets
//				Default: 360
//			proximityDistanceImpactTrigger=[float] (units: ls)
//				If a munition detects it will probably impact a valid target within [int]
//				light-seconds, it will automatically detonate early rather than waiting
//				to impact the target or reach the automatic fragmentation distance
//				Default: 0
//			proximityDistanceArmed=[float] (units: ls)
//				This is the maximum proximity sensor range of the munition. It only arms
//				proximity detection. If it detects that it would move away from the nearest
//				valid target within the next tick, it will detonate.
//				Note: this value smartly adjusts based on the size of the candidate
//					fragmentation trigger. For the intended legacy behavior, use
//					fragmentMaxRadius.
//				Default: 4
//			proximityDistanceAutoTrigger=[float] (units: ls)
//				The projectile will automatically fragment if a valid target is within
//				sensor range in this distance. Automatically forces proximityArmedDistance
//				to be its value if it is less.
//				Note: this value smartly adjusts based on the size of the candidate
//					fragmentation trigger. For the intended legacy behavior, use
//					fragmentMinRadius - this value still had a small amount of smart
//					adjustment however.
//				Default: 0.0 (disabled)
//			proximityDistanceFail=[float] (units: ls)
//				The projectile will not fragment if within this distance. Automatically
//				sets disableImpactFragmentation to True. Raises an error if it greater
//				than proximityArmed or proximityAutoTrigger distances.
//				Default: -1 (disabled)
//			proximityDistanceFailsafe=[float] (units: ls)
//				The projectile will not fragment if within this distance of the source.
//				Default: -1 (disabled)
//			fragmentVelocityInheritance="none|newtonian|relativisitc|superluminal"
//				Sets the mode of fragment velocity inheritance.
//				Note: if the missile velocity or defined fragment velocity is over
//					1.0c, then the behavior if not "none" defaults to "superluminal"
//				Default: "none"
//					none: no inheritance, the <Fragment> defined speed is used
//					newtonian: velocities are added, and capped at 1.0c
//					relativistic: velocities are combined in an asymptotic way
//					superluminal: as in newtonian, but velocity is not capped
//			fragmentAngleDirection="direction|velocity|target|trigger|origin|system|random"
//				Sets the middle angle of the fragment arcAngle to be relative to a
//				configurable object. If a value cannot be determined, it will fall back
//				to "direction".
//				API 54 Default: "direction"
//				API <54 Default: hard-coded to "trigger" (legacy behavior)
//					target: direction of the intended target
//					trigger: direction of the object that triggered fragmentation
//					velocity: direction the missile was traveling
//					direction: direction the missile was facing
//					origin: direction of the center of the system
//					system: direction of 0-angle of the system
//					random: direction is random
//			fragmentAngle=[Dice Expression]
//				Degree offset for an individual fragment. Diceroll is rolled
//				for each fragment individually. This is an absolute value so
//				remember to offset the value to center it. This replaces
//				the value specified in arcAngle.
//				Default: 1d360
//			fragmentAngleOffset=[int]
//				Degrees offset for all fragments from either the values in
//				fragmentAngle (an asbolute value diceroll) or in
//				arcAngle (a uniform distribution zeroed on the middle) from the
//				angle source picked with fragmentAngleDirection.
//				Default: 0
//		tlisp:
//			(objDestroy obj sourceObj)
//				Now requires sourceObj in API 54, still works for API 53 and below.
//				For legacy (objDestroy obj) behavior, use (objRemove obj)
//			(objRemove obj)
//				Added, removes the obj from system
//
//	 55: 2.0 Alpha 4
//		tlisp:
//			(dbgApplyTimed fn argsList)
//				Allows timing tlisp function execution
//			(dbgEvalTimed expr)
//				Allows timing tlisp function/block parsing & execution
//			(=== [a b ...])
//				Exact equality operator, does not coerce any types and is case sensitive
//				Can be used to test if a variable is a real empty list (mutable) or
//				actually just Nil (atomic)
//				If given 1 arg, compares that arg to Nil, returns True if (atomic) Nil
//				WARNING, as this function may be used in performance sensitive code,
//				it does not check that you are using API55 first. Ensure your API version
//				is appropriately set to avoid multiverse from downloading your extension
//				onto an incompatible version.
//			(!=== [a b ...])
//				Exact inequality operator, does not coerce any types and is case sensitive
//				Can be used to test if a variable is a real empty list (mutable) or
//				actually just Nil (atomic)
//				If given 1 arg, compares that arg to Nil, returns True if not (atomic) Nil
//				WARNING, as this function may be used in performance sensitive code,
//				it does not check that you are using API55 first. Ensure your API version
//				is appropriately set to avoid multiverse from downloading your extension
//				onto an incompatible version.
//			(gammaScale val inStart inEnd outStart outEnd [gamma=1.0])
//				Native tlisp version of mathScale/mathScaleReal.
//				Returns an int if outMin and outMax are both ints. Otherwise returns a double.
//				Gamma can be an int (multiplied by 100) or a double.
//				Note: properly handles negative curves where an input or output
//					range goes from higher to lower value.
//				Warning: Do not use below API55. API checking is not available for primitive functions.
//			(plyGetName player)
//				Allows getting the player name without needing to use str formatting
//			(strBeginsWith str substr [caseSensitive=Nil])
//				Returns if a string begins with substr. Optionally case sensitive.
//				Warning: do not use below API55, API checking is not available for primitive functions.
//			(strEndsWith str substr [caseSensitive=Nil])
//				Returns if a string ends with substr. Optionally case sensitive.
//				Warning: do not use below API55, API checking is not available for primitive functions.
//			(strContains str substr [caseSensitive=Nil])
//				Returns if a string contains substr. Optionally case sensitive.
//				Higher speed than strCount if you just need to detect the presense of a substr.
//				Warning: do not use below API55, API checking is not available for primitive functions.
//			(strCount str substr [caseSensitive=Nil])
//				Returns the number of instances of substr in str. Optionally case sensitive.
//				Warning: do not use below API55, API checking is not available for primitive functions.
//			(strReplace str substr replaceStr [caseSensitive=Nil])
//				Replaces instances of substr in str with replaceStr. Optionally case sensitive.
//				Warning: do not use below API55, API checking is not available for primitive functions.
//			(strSlice str sliceStart [sliceLen=-1])
//				As subset, but allows negative slice stars from the end of a string.
//				sliceLen < 0 returns remainder of the string
//				Warning: do not use below API55, API checking is not available for primitive functions.
//			(strSplit str delim [caseSensitive=Nil])
//				Splits str based on delimiters. Consecutive delimiters produce empty strings "".
//				Optionally case sensitive.
//				Warning: do not use below API55, API checking is not available for primitive functions.
//			(strStrip str [stripChars=" \t\n\r"] [caseSensitive=Nil])
//				Strips stripChars from the beginning and end of str. Optionally case sensitive.
//				Warning: do not use below API55, API checking is not available for primitive functions.
//		Any <Type>
//			<AttributeDesc>
//				<ItemAttribute> and <LocationAttribute>
//					labelColor: 24-bit or triplet RGB color for tag background
//						Default: uses neutral/positive/negative background colors
//					labelTextColor: 24-bit or triplet RGB color for tag text
//						Default: uses neutral/positive/negative text colors
//		<ItemType>
//			<Weapon>
//				miningMethod: "ablative"|"drill"|"explosive"|"shockwave"
//					Default: uses automatic computation based on weapon desc
//			<Events>
//				<GetDisplayAttributes>
//					Returned struct now accepts the following new values:
//						labelColor: 24-bit or triplet RGB color for tag background
//							Default: uses neutral/positive/negative background colors
//						labelTextColor: 24-bit or triplet RGB color for tag text
//							Default: uses neutral/positive/negative text colors
//			<Weapon>
//				soundVolume: (Double)
//					linear multiplier to fire effect sound volume (relative to max volume)
//					default: 1.0
//				soundFalloffFactor: (Double)
//					distance multiplier to fire effect sound falloff (affects quadratic falloff curve)
//					default: 1.0
//				soundFalloffStart: (Double)
//					distance (in ls) at which fire effect sound falloff starts
//					default: 0.0
//				chargeSoundVolume: (Double)
//					linear multiplier to charge effect sound volume (relative to max volume)
//					default: 1.0
//				chargeSoundFalloffFactor: (Double)
//					distance multiplier to charge effect sound falloff (affects quadratic falloff curve)
//					default: 1.0
//				chargeSoundFalloffStart: (Double)
//					distance (in ls) at which charge effect sound falloff starts
//					default: 0.0
//		<EffectType>
//			soundVolume: (Double)
//				linear multiplier to effect sound volume (relative to max volume)
//				default: 1.0
//			soundFalloffFactor: (Double)
//				distance multiplier to effect sound falloff (affects quadratic falloff curve)
//				default: 1.0
//			soundFalloffStart: (Double)
//				distance (in ls) at which effect sound falloff starts
//				default: 0.0
//			<Events>
//				<GetParameters>
//					Return struct now accepts the following additional sound options:
//						soundVolume: (Double)
//							linear multiplier to sound volume (relative to max volume)
//							default: 1.0
//						soundFalloffFactor: (Double)
//							distance multiplier to sound falloff (affects quadratic falloff curve)
//							default: 1.0
//						soundFalloffStart: (Double)
//							distance (in ls) at which sound falloff starts
//							default: 0.0
//
//	 56: 2.0 Alpha 6
//		tlisp:
//			(sysAddStargateTopology [nodeID] gateID destNodeID destGateID [optionsStruct])
//				optionsStruct: (struct)
//					color: (string: html argb color)
//						Specifies an argb color to use for the topology gate link on the galaxy map
//						If Alpha is not specified, argbLinkColor is assumed to have full alpha (0xFF)
//					attributes: (string)
//						An attributes string. See trnCreateAllStargates for special known-fields.
//					beaconType: (unid)
//						The type of beacon to spawn
//					gateType: (unid)
//						The type of gate to spawn
//					locationCriteria: (string)
//						the in-system location criteria to use for placing the gate
//					fromAttributes: (string)
//						An attributes string for the from-side link.
//						See trnCreateAllStargates for special known-fields.
//					fromBeaconType: (unid)
//						The type of beacon to spawn on the from side
//					fromGateType: (unid)
//						The type of gate to spawn on the from side
//					fromLocationCriteria: (string)
//						the in-system location criteria to use for placing the gate on the from side
//					toAttributes: (string)
//						An attributes string for the to-side link.
//						See trnCreateAllStargates for special known-fields.
//					toBeaconType: (unid)
//						The type of beacon to spawn on the to side
//					toGateType: (unid)
//						The type of gate to spawn on the to side
//					toLocationCriteria: (string)
//						the in-system location criteria to use for placing the gate on the to side
//			(sysGetStargateProperty [nodeID] gateID property)
//				'attributes: new property to retrieve <Stargate> attributes
//				'linkColor: new property to retrieve linkColor as HTML color string if present
//				'locationCriteria: new property for location criteria to use for placing the gate
//				'gateType: the unid of the stargate
//				'beaconType: the unid of the stargate beacons
//		<ShipClass>
//			<Drive>
//				powerUseRatio: (Double)
//					ratio to apply to auto-computed power use, ex: 2.0 doubles power consumption.
//					Ignored if explicit powerUse is set.
//					default: 1.0
//		<StationType>
//			<Encounter>
//				distanceFrequency: (str)
//					Now supports distances beyond 5 at a single frequency
//					Ex: "ccccc|curvv v"
//					Default (if not specified) is NotRandom
//		<SystemMap>
//			<...><Random>
//				(NOTE:) modded systemTypes that do not use trnCreateAllStargates may not respect the
//					following fields except for linkColor.
//				gateLocationCriteria: (string)
//					the in-system location criteria to use for placing the gate
//				beaconType: (unid)
//					The type of beacons to spawn
//				gateType: (unid)
//					The type of gates to spawn
//				linkAttributes: (string)
//					An attributes string. See trnCreateAllStargates for special known-fields.
//				linkColor: (string: html argb color)
//					The color to display this stargate link in on the galaxy map.
//					If Alpha is not specified, linkColor is assumed to have full alpha (0xFF)
//			<...><Stargate>
//				(NOTE:) modded systemTypes that do not use trnCreateAllStargates may not respect the
//					following fields except for linkColor.
//				attributes: (string)
//					An attributes string. See trnCreateAllStargates for special known-fields.
//				beaconType: (unid)
//					The type of beacon to spawn
//				gateType: (unid)
//					The type of gate to spawn
//				linkColor: (string: html argb color)
//					The color to display this stargate link in on the galaxy map.
//					If Alpha is not specified, linkColor is assumed to have full alpha (0xFF)
//				locationCriteria: (string)
//					the in-system location criteria to use for placing the gate
//				<FromGate>/<ToGate>
//					attributes: (string)
//						An attributes string. See trnCreateAllStargates for special known-fields.
//						Overrides the <Stargate> attribute string.
//					beaconType: (unid)
//						The type of beacon to spawn for this side of the gate
//						Overrides the <Stargate> beaconType.
//					gateType: (unid)
//						The type of gate to spawn for this side of the gate
//						Overrides the <Stargate> gateType.
//					locationCriteria: (string)
//						the in-system location criteria to use for placing the gate
//						Overrides the <Stargate> locationCriteria string.
//


//	UNIVERSE VERSION HISTORY ---------------------------------------------------
//
//	 0: Pre-0.98
//
//	 1: 0.98
//		version marker in CUniverse
//		version in CUniverse
//		m_Data in CTopologyNode
//
//	 2: 0.99
//		Adventure UNID in CUniverse
//
//	 3: 0.99
//		All design type info is together
//
//	 4: 0.99d
//		m_ShipNamesIndices, and m_iShipName in CShipClass
//
//	 5: 0.99d
//		m_sEpitaph and m_sEndGameReason in CTopologyNode
//
//	 6: 1.03
//		m_pMap in CTopologyNode
//
//	 7: 1.03
//		m_Time in CUniverse
//
//	 8: 1.03
//		Extension UNID list in CUniverse
//
//	 9: 1.04
//		system save version
//		m_Annotations in CSystemMap
//
//	10: 1.04
//		m_sUnknownName in CItemType
//
//	11: 1.07
//		m_Registered in CDesignType
//
//	12: 1.08
//		CDynamicDesignTable in CUniverse
//
//	13: 1.08
//		Flags in CUniverse
//
//	14: 1.08c
//		dwRelease for extensions
//		Save extension UNID instead of adventureDesc UNID
//
//	15: 1.08d
//		dwRelease in CDynamicDesignTable
//
//	16: Fixed a corruption bug.
//		Versions 15 and 14 are marked are corrrupt
//
//	17: 1.08g
//		Added missions
//
//	18: 1.2 Alpha 1
//		Added m_Objects
//
//	19: 1.2 Alpha 1
//		Added m_EncounterRecord in CStationType
//
//	24: 1.2 Alpha 2
//		Added m_ObjStats in CUniverse
//		Added object names to CObjectTracker
//		Added m_iAccepted to CMissionType
//		Added m_sAttributes to CTopologyNode
//		Added m_AscendedObjects to CUniverse
//
//	26: Save and restore m_pPlayer (IPlayerController)
//
//	27: 1.7 Alpha 1
//		Additional parameters for a stargate in CTopologyNode
//
//	31: 1.8 Alpha 4
//		CDesignType has an extra flags word
//
//	32: 1.8 Alpha 4
//		Added m_Trading to CTopologyNode
//
//	33: 1.8 Alpha 4
//		Added m_AreaHighlights to CSystemMap
//
//	34: 1.8 Alpha 4
//		Added m_sCreatorID to CTopologyNode
//
//	35: 1.8 Beta 2
//		Added m_iExisting to CMissionType
//
//	36: 1.9 Alpha 1
//		Added m_UnknownTypes to CItemType
//
//	37: 1.9 Alpha 1
//		Added m_dwLastAcceptedOn in CMissionType
//
//	38: 1.9 Alpha 3
//		Added m_Difficulty to CUniverse
//
//	39: 1.9 Beta 4
//		Added extra type and extension information
//
//	40: 1.9 Beta 4
//		Added design type in dwFlags or CDesignType.
//
//	41: 2.0 Alpha 6
//		Add gate link RGB color
//		Add gate type
//		Add gate beacon type
//		Add gate link attributes
//		Add gate location criteria
//


//	SYSTEM VERSION HISTORY -----------------------------------------------------
//
//	 0: 0.95
//	 1: 0.95a
//	 2: 0.96a
//		m_pEncounterInfo in CShip
//		m_rItemMass in CShip
//
//	 3: 0.97
//		m_sDeferredDestruction string in CPlayerShipController.
//		m_iAngryCounter in CStation
//		m_vOldPos in CParticleDamage
//
//	 4: 0.97
//		m_iCountdown in CStandardShipAI
//
//	 5: 0.97
//		m_iStealthFromArmor in CShip
//
//	 6-14: 0.98
//		m_Blacklist in CBaseShipAI
//		m_iDeviceCount in CShip
//		m_sName in CMarker
//		m_rKlicksPerPixel in CSystem
//		m_rTimeScale in CSystem
//		m_Blacklist in CStation
//		m_iReinforceRequestCount in CStation
//		m_NavPaths in CSystem
//		m_pNavPath in CBaseShipAI
//		m_iNavPathPos in CBaseShipAI
//		m_iBalance in CStation
//		m_dwID in CSpaceObject
//		m_vStart in CNavigationPath
//		m_pCommandCode in CStandardShipAI
//		m_fIdentified in CShip
//
//	15: 0.98a
//		UNIDs for painters are saved as strings
//
//	16: 0.98c
//		m_sStargateDestNode in CStation
//		m_sStargateDestEntryPoint in CStation
//
//	17: 0.98c
//		m_iRotationCount in CObjectImageArray
//
//	18: 0.99
//		m_iCause in DamageDesc
//		m_iCause in CAreaDamage
//		m_iCause in CMissile
//		m_iCause in CBeam
//		m_iCause in CParticleDamage
//		m_iCause in CRadiusDamage
//
//	19: 0.99
//		m_pTarget in CRadiusDamage
//
//	20: 0.99
//		m_iTick in CShockwavePainter
//
//	21: 0.99
//		m_iInitialDelay in CAreaDamage
//		new particle array in CParticleDamage
//
//	22: 0.99
//		Experiment: CannotBeHit is set to TRUE for background objects
//
//	23: 0.99
//		SExtra structure for CItem
//
//	24: 0.99
//		m_iRotation in CDockingPorts
//
//	25: 0.99
//		m_iPower in CPlayerShipController
//		m_iMaxPower in CPlayerShipController
//		m_iPowerRate in CPlayerShipController
//
//	26: 0.99
//		m_dwNameFlags in CShip
//
//	27: 0.99
//		DiceRange uses 32-bits for all values
//
//	28: 0.99
//		m_iHitPoints in CMissile
//
//	29: 0.99
//		m_iDeviceSlot in CInstalledDevice
//
//	30: 0.99
//		CDamageSource
//
//	31: 0.99
//		m_iMaxStructuralHP in CStation
//
//	32: 0.99
//		m_pSecondarySource in CDamageSource
//
//	33: 0.99b
//		m_iManeuverCounter in CBaseShipAI
//
//	34: 1.0 RC1
//		m_vPotential in CBaseShipAI
//
//	35: 1.0 RC1
//		m_EventHandlers in CSystem
//
//	36: 1.0 RC1
//		m_dwNameFlags in CStation
//
//	37: 1.0 RC1
//		m_pTrade in CStation
//
//	38: 1.0 RC1
//		m_dwID in COverlay
//		m_iPosAngle, m_iPosRadius, m_iRotation in COverlay
//
//	39: 1.0 RC1
//		m_Data in COverlay
//
//	40: 1.0 RC1
//		Save class with IEffectPainter
//
//	41: 1.0 RC1
//		CSystem saves object references using ObjID (instead of index)
//
//	42: 1.0 RC1
//		m_iLastFireTime in CShip
//
//	43: 1.0 RC1
//		Start saving UNID/class for NULL effect creators
//
//	44: 1.0 RC1
//		m_iActivateDelayAdj in CInstalledDevice
//
//	45: 1.0 RC2
//		m_iCause in CDamageSource
//
//	46: 1.0 RC3
//		m_pEnhancer and m_iExpireTime in CItemEnhancement
//
//	47: 1.0 RC3
//		m_pTarget and m_pBase in CStation
//
//	48: 1.0 RC3
//		m_pOverride in CSpaceObject
//
//	49: 1.02
//		m_Stats in CPlayerShipController
//		m_Credits in CPlayerShipController
//
//	50: 1.02
//		Removed m_sDeferredDestruction from CPlayerShipController
//
//	51: 1.02
//		Added m_iRotation to CEffect
//		Added iRotation to SEffectNode in CSpaceObject
//
//	52: 1.03
//		Added flags for CSystemEvent
//
//	53: 1.03
//		Added m_PlayTime and m_GameTime for CPlayerGameStats
//
//	54: 1.03
//		Added m_iSect to CInstalledArmor
//
//	55: 1.04
//		Added m_dwFlags to CDamageSource
//
//	56: 1.04
//		Added m_Overlays to CStation
//
//	57: 1.04
//		Added m_iLastUpdated to CSystem
//
//	58: 1.04
//		Added m_dwDisruptedTime to CItem::SExtra
//		CItemList no longer saves allocation size
//
//	59: 1.05
//		CInstalledArmor no longer saves m_Mods
//
//	60: 1.06
//		Added m_Territories in CSystem
//
//	61: 1.06
//		Added m_ImageSelector to CStation (and removed m_iVariant)
//
//	62: 1.06
//		CCurrencyBlock saves CEconomyType UNIDs instead of strings
//
//	63: 1.06
//		Added m_iDevice to COverlay
//		Added m_UIMsgs to CPlayerShipController
//
//	64: 1.06
//		Added Pos and Vel to CParticleArray::SParticle
//
//	65: 1.06
//		Added m_vOldPos to CSpaceObject
//
//	66: 1.06
//		Added m_dwTargetID to CInstalledDevice
//
//	67: 1.06
//		Stop saving m_vOldPos in CParticleDamage
//		Added m_vOrigin to CParticleArray
//
//	68: 1.06
//		Added m_AISettings to CBaseShipAI
//
//	69: 1.06
//		Added m_HitTest to CShockwavePainter
//
//	70: 1.06
//		Removed m_Hit from CAreaDamage
//		Added m_iRadiusInc to CShockwavePainter
//
//	71: 1.07
//		m_dwCharges in CItem is now 32 bits
//		Added m_dwVariant to CItem; removed m_dwCondition
//
//	72: 1.07
//		Added m_Locations to CSystem
//
//	73: 1.07
//		Added BYTE-length m_ShieldDamage to DamageDesc
//
//	74: 1.07
//		Added m_KeyEventStats to CPlayerGameStats
//
//	75: 1.08
//		Added m_pOrderModule to CBaseShipAI
//
//	76: 1.08c
//		Added m_iState to CAttackOrder
//		Added m_iState to CAttackStationOrder
//		Added m_iState to CGuardOrder
//
//	77: 1.08g
//		Added m_SubscribedObjs to CSpaceObject
//		Added m_iMaxHitPoints to CStation
//
//	78: 1.08h
//		Fixed a bug in m_SubscribedObjs.
//
//	79: 1.08k
//		Fixed another bug in m_SubscribedObjs.
//
//	80: 1.08l
//		Fixed another(!) bug in m_SubscribedObjs.
//
//	81: 1.1
//		m_iMaxDist to CDockingPorts
//
//	82:	1.2 Alpha 1
//		m_pEnvironment in CSystem is saved differently.
//
//	83: 1.2 Alpha 1
//		iService in CTradingDesc
//
//	84: 1.2 Alpha 1
//		m_dwLeftSystemOn in CMission
//
//	85: 1.2 Alpha 1
//		m_dwCreatedOn in CMission
//
//	86: 1.2 Alpha 1
//		m_dwAcceptedOn in CMission
//		m_fAcceptedByPlayer in CMission
//
//	87: 1.2 Alpha 2
//		m_Orders in CBaseShipAI has new structure
//
//	88: 1.2 Alpha 3
//		Save size of tiles in CEnvironmentGrid
//
//	89: 1.2 Alpha 3
//		m_pDebriefer in CMission
//
//	91: 1.2 Alpha 3
//		m_Interior in CShip
//
//	92: 1.2 Alpha 3
//		m_pEnhancements in CInstalledDevice
//		m_pEnhancements in CMissile
//
//	93: 1.2 Beta 2
//		m_iActivateDelay in CInstalledDevice (instead of m_iActivateDelayAdj)
//		CEffectGroupPainter saves a count
//
//	94: 1.2 Beta 3
//		m_rParallaxDist in CStation
//
//	95: 1.2 RC 2
//		Updated to fix a bug in 94 in which asteroids were marked as immutable
//		(preventing mining from working).
//
//	96: 1.3
//		m_iLastHitTime in CShip
//
//	97: 1.3
//		m_Rotation in CShip
//
//	98: 1.3
//		m_bUseObjectCenter in CParticlePatternPainter
//
//	99: 1.3
//		CEffectParamDesc saving
//
//	100: 1.3
//		m_iOrder in CPlayerShipController
//
//	101: 1.4
//		Flags in COverlay
//
//	102: 1.4
//		iHPDamaged in SItemTypeStats
//
//	103: 1.4
//		m_iCounter in COverlay
//		m_sMessage in COverlay
//
//	104: 1.4
//		m_iFramesPerColumn in CObjectImageArray
//
//	105: 1.4
//		m_pTrade in CShip
//
//	106: 1.4
//		m_EncounterRecord in CStationType changed.
//
//	107: 1.5
//		m_fExternal flag in CInstalledDevice
//
//	108: 1.6
//		Store ship controller name instead of CObject ID.
//
//	109: 1.6
//		m_rDistance in CWaitOrder
//
//	110: 1.6
//		New 32-bit colors.
//
//	111: 1.6
//		m_fIsBarrier in CStation
//
//	112: 1.6
//		m_iBarrierClock in CAIBehaviorCtx
//
//	113: 1.6
//		sMessageID in CTradingDesc::SServiceDesc
//
//	114: 1.6
//		m_iFramesPerRow in CObjectImageArray
//
//	117: 1.7 Alpha 1
//		Added components to CItemType
//
//	118: 1.7 Alpha 1
//		None
//
//	119: 1.7 Alpha 1
//		Added iGeneration to CParticleArray:SParticle
//
//	120: 1.7 Alpha 1
//		Added flags to CParticleDamage
//
//	121: 1.7 Alpha 1
//		CShockwavePainter saves params
//
//	122: 1.7 Alpha 1
//		CSingleParticlePainter saves params
//
//	123: 1.7 Alpha 1
//		Use a double for fuel tracking
//
//	124: 1.7 Alpha 1
//		Added m_iTick for COverlay
//
//	125: 1.7 Alpha 1
//		New flocking styles in CAISettings
//
//  126: 1.7 Alpha 2
//      m_iStyle in CMarker
//
//  127: 1.7 Alpha 2
//      Removed saving drive desc UNID for CShip
//
//  128: 1.7 Alpha 2
//      Added m_iHealerLeft to CShip
//
//  129: 1.7 Alpha 2
//      Added m_rFuelConsumed to CPlayerGameStats
//
//  130: 1.7 Alpha 2
//      Replaced symbol table in CAttributeDataBlock
//
//  131: 1.7 Alpha 2
//      Added CCompositeImageSelector to CObjectTracker
//
//  132: 1.7 Alpha 2
//      Added flags to CObjectTracker
//
//  133: 1.7 Alpha 2
//      Added more data CObjectTracker
//
//  134: 1.7 Alpha 2
//      Added m_pMapOrbit to CMarker
//
//  135: 1.7 Beta 1
//      Added delayed commands to CObjectTracker
//
//	136: 1.7 Beta 1
//		Additional flags on CSpaceObject
//
//	137: 1.7 Beta 3
//		Remove m_iCause from objects (since it is already in CDamageSource)
//
//	138: 1.7 Beta 3
//		SFXParticleSystem has new style mapping
//
//	139: 1.7 Beta 3
//		m_LastShotIDs in CInstalledDevice
//
//	140: 1.7 Beta 4
//		m_iNextDetonation in CMissile
//
//	141: 1.7 Beta 5
//		m_pCharacterClass in CPlayerShipController
//
//	142: 1.7 Beta 5
//		sLevelFrequency in CTradingDesc
//		Save hit objects in CContinuousBeam
//
//	143: 1.8 Alpha 1
//		m_Joints in CSystem
//
//	144: 1.8 Alpha 1
//		m_pPowerUse in CShip
//
//	145: 1.8 Alpha 1
//		m_pMoney in CShip
//
//	146: 1.8 Alpha 2
//		m_pRotation in CStation
//
//	147: 1.8 Alpha 2
//		Pos in CDockingPorts::SDockingPort
//
//	148: 1.8 Alpha 2
//		Flags in CObjectJoint
//
//	149: 1.8 Alpha 2
//		pAttached in CShipInterior::SCompartmentEntry
//
//	150: 1.8 Alpha 3
//		m_pManeuverController in CPlayerShipController
//
//	151: 1.8 Alpha 3
//		m_Hull in CStation
//
//	152: 1.8 Alpha 3
//		Added m_pEnhancements to CInstalledArmor
//
//	153: 1.8 Alpha 4
//		Added m_WeaponTargetsOld
//
//	154: 1.8 Alpha 4
//		Added sNodeID and flags to CSystemMap::SMapAnnotation
//
//	155: 1.8 Alpha 4
//		Added m_Abilities to CShip
//
//	156: 1.8 Beta 1
//		Added m_pCharacter to CShip
//
//	157: 1.8 Beta 1
//		Added m_sID to CInstalledDevice
//
//	158: 1.8 Beta 2
//		Removed m_fAutomatedWeapon from CSpaceObject
//
//	159: 1.8 Beta 2
//		Added m_iExtraPowerUse to replace m_iSlotBonus in CInstalledDevice
//
//	160: 1.8 Beta 3
//		Moved some condition flags to COverlayList
//
//	161: 1.8 Beta 3
//		Added m_vPos to CTimedEncounterEvent
//
//	162: 1.8 Beta 3
//		Remove m_xMapLabel and m_yMapLabel from CStation
//
//	163: 1.8 Beta 3
//		Added m_fShowMapLabel to CShip
//
//	165: 1.8 Beta 4
//		Added m_dwCompletedOn to CMission
//
//	166: 1.8 Beta 4
//		m_Targets in CTimedEncounterEvent replaces m_pTarget
//
//	167: 1.8 Beta 4
//		m_dwBackgroundImageOverride in CSystemMap
//
//	168: 1.8 Beta 4
//		m_iPosZ in COverlay
//
//	169: 1.8 Beta 5
//		m_dwVariantCounter in CItem::SExtra
//
//	170: 1.8 Beta 5
//		m_dwVariant changed to m_dwLevel in CItem::SExtra
//
//	171: 1.8 Beta 5
//		Added m_sNode to CTimedMissionEvent
//
//	172: 1.8 Beta 5
//		Added m_iShotSeparationScale to CInstalledDevice.
//
//	173: 1.9 Alpha 1
//		Added m_iDamagedHP to CItem::SExtra
//
//	174: 1.9 Alpha 1
//		Add source to CItem
//
//	175: 1.9 Alpha 3
//		Changes to COrbit
//
//	176: 1.9 Alpha 3
//		m_iType in CStationHull
//
//	177: 1.9 Alpha 4
//		m_iPaintOrder in CStation
//
//	178: 1.9 Alpha 5
//		m_rMaxPlayerDist in CDockingPorts
//
//	179: 1.9 Beta 1
//		iCountMined in CPlayerGameStats
//
//	180: 1.9 Beta 1
//		Replace m_WeaponTargets
//
//	181: 1.9 Beta 1
//		Remove m_iFireDelay and m_iMissileFireDelay from CShip
//
//	182: 1.9 Beta 1
//		Add m_fAnonymous to CStation
//
//	183: 1.9 Beta 1
//		m_iStarlightImageRotation in CStation
//
//	184: 1.9 Beta 1
//		Add m_iHitPoints to COverlay
//
//	185: 1.9 Beta 1
//		Move m_fAutoCreatedPorts to CSpaceObject (from CShip)
//
//	186: 1.9 Beta 1
//		Add m_Source to CParticleEffect
//
//	187: 1.9 Beta 1
//		Add m_sArcTitle and m_sArcStatus to CMission
//
//	188: 1.9 Beta 1
//		m_fIsStargate in CObjectTracker::SObjEntry
//
//	189: 1.9 Beta 2
//		m_sSubordinateID in CStation
//
//	190: 1.9 Beta 2
//		m_MomentumDamage in DamageDesc
//
//	191: 1.9 Beta 2
//		m_sStartingSystem in CPlayerShipController
//
//	192: 1.9 Beta 2
//		m_iMissionNumber in CMission
//
//	193: 1.9 Beta 2
//		m_PlayerShipStats in CPlayerGameStats
//
//	194: 1.9 Beta 2
//		m_fAllowEnemyDocking in CStation
//
//	195: 1.9 Beta 4
//		COrderDesc
//
//	196: 1.9 Beta 4
//		m_Overlays in CMissile
//
//	197: 1.9 Beta 4
//		Add fields to CAISettings
//		Add m_DeterModule to CBaseShipAI
//
//	198: 1.9 Beta 4
//		Remove m_iStealth from CShip
//
//	199: 1.9 Beta 4
//		Added m_SquadronID for CShip
//
//	200: 1.9 Beta 4
//		Changed CIntegralRotationDesc::ROTATION_FRACTION
//
//	201: 1.9 Beta 4
//		Added m_iInstalledIndex to CItem::SExtra
//
//	202: 1.9 Beta 4
//		Added m_PowerStats to CPlayerGameStats
//
//	203: 1.9 Beta 4
//		Move loading of order modules
//
//	204: 1.9 Beta 4
//		Added m_dwLastFireTime and m_dwLastHitTime to CStation
//
//	205: 1.9 Beta 4
//		Added flags to COrderDesc
//
//	206: 1.9 Beta 4
//		Added fields to CMarker
//
//	207: 1.9
//		Added m_iTonsOfOreMined to CPlayerGameStats
//
//	208: 1.9
//		Added m_TotalValueSold and m_TotalValueBought to CPlayerGameStats
//
//	209: 1.9
//		Changed m_PlayerShipStats in CPlayerGameStats
//
//	210: 1.9
//		Added m_Events to CAscendedObjectList
//
//	211: 1.9
//		Change CShockwaveHitTest
//
//	212: 1.9 ??
//		????
//
//	213: 2.0 Alpha 2
//		Change DiceRange to use -1 for not set
//
//	214: 2.0 Alpha 7
//		Change CParticleArray::m_iLifeLeft to milliseconds game time
//		(instead of ticks)

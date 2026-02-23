//	TSEVersions.h
//
//	Version numbers and history
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

constexpr DWORD API_VERSION =							59;
constexpr DWORD UNIVERSE_SAVE_VERSION =					41;
constexpr DWORD SYSTEM_SAVE_VERSION =					221;

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
//	 57: 2.0 Alpha 7
//		tlisp:
//			(bAnd x1 [x2 ... xn])
//				Returns the bitwise AND of all arguments.
//				All arguments are coerced to 32-bit integers.
//			(bOr x1 [x2 ... xn])
//				Returns the bitwise OR of all arguments.
//				All arguments are coerced to 32-bit integers.
//			(bXor x1 [x2 ... xn])
//				Returns the bitwise XOR of all arguments.
//				All arguments are coerced to 32-bit integers.
//			(bNot x)
//				Returns the bitwise NOT of x.
//				Argument is coerced to a 32-bit integer; result is also 32-bit signed.
//			(bShL x count)
//				Returns x shifted left by count bits (logical).
//				Low bits are filled with zeros; high bits are discarded.
//			(bShR x count)
//				Returns x shifted right by count bits (logical).
//				High bits are filled with zeros; low bits are discarded.
//			(bRoL x count)
//				Returns x rotated left by count bits in 32-bit space.
//				Bits shifted out of the high end wrap around to the low end.
//			(bRoR x count)
//				Returns x rotated right by count bits in 32-bit space.
//				Bits shifted out of the low end wrap around to the high end.
//			(dbgGet [option] value)
//				New option parameters:
//					'forceSTPaint: forces single threaded painting
//					'showPaintLocation: shows the upper left and lower right corners of
//						each thread's painted area
//					'showPaintTime: shows the time to paint the sprite in
//						microseconds
//			(dbgSet [option] value)
//				New option parameters:
//					'forceSTPaint: forces single threaded painting
//					'showPaintLocation: shows the upper left and lower right corners of
//						each thread's painted area
//					'showPaintTime: shows the time to paint the sprite in
//						microseconds
//			(help function)
//				Returns the docstring for the function (accepts both primitives
//				and lambdas)
//			(help strFilter [typefilter])
//				Upgraded to accept a typeFilter argument
//					typFilter: (str: '*|'l|'lambda|'lambdas|'p|'primitive|'primitives)
//						This argument allows including:
//							*: all functions
//							lambdas: only lambdas
//							primitives: only primitives
//						Default: 'primitives
//			(itmGetDataKeys item)
//				Returns a list of data keys for the given item
//			(itm@Keys item)
//				Returns a list of property keys for the given item
//			(lambda args [docstring] expr)
//				Lambda now accepts an optional docstring that can be printed out with (help lambda)
//			(msnGetDataKeys obj)
//				Returns a list of typData keys for the given msn
//			(msnGetDataKeys obj)
//				Returns a list of typData keys for the given msn type
//			(msnGetStaticDataKeys type)
//				Returns a list of static data keys for the given msn type
//			(msn@Keys type)
//				Returns a list of all instance property and custom global property keys for the given msn
//			(objGetDataKeys obj)
//				Returns a list of typData keys for the given obj
//			(objGetDataKeys obj)
//				Returns a list of typData keys for the given obj type
//			(objGetStaticDataKeys type)
//				Returns a list of static datakeys for the given obj type
//			(obj@Keys type)
//				Returns a list of all instance property and custom global property keys for the given obj
//			(objGetOverlayDataKeys obj overlayID)
//				Returns a list of all data keys for the given overlay on the given obj
//			(sysGetDataKeys [node])
//				Returns a list of all data keys for the given system.
//				If [node] is Nil/not provided, it gets the current system.
//			(typGetDataKeys type)
//				Returns a list of typData keys for the given type
//			(typGetStaticDataKeys type)
//				Returns a list of static datakeys for the given type
//			(typ@Keys type)
//				Returns a list of custom global property keys for the given type
//		<AdventureDesc>
//			<Constants>
//				<MiningMaxOreLevels>
//					miningMaxOreLevel: (damageAdj-style list of ints)
//						The maximum level ore that this weapon shot can extract
//						Specifying a "+" before a level means this is a positive
//							offset relative to the level of the item
//						Specifying a "-" before a level means this is a negative
//							offset relative to the level of the item
//		<Image> (Type)
//			pngBitmaskAlphaSource: (str: "alpha"|"red"|"green"|"blue")
//				Specify a specific channel to use from a png as a bitmask alpha source
//				Default: "alpha"
//		<ItemType>
//			<Weapon>
//				miningMaxOreLevel: (int: 0-25)
//					The maximum level ore that this weapon shot can extract
//					0 allows the damage to probe for ore but does not mine
//					-1 uses the adventure default settings
//				damage:
//					Now accepts special damage type miningScan[:0-1]
//						miningScan: sets aMiningScan to True in obj <onMining>
//							In SotP this is used to scan without actually mining
//							the ore.
//							You can optionally specify miningScan:N and it will
//							treat it as miningScan:1 mining:N. If mining:# is also
//							specified, it will always override the value in
//							miningScan:#.
//							NOTE: API 48-56 weapons with generic:# and mining:#
//							are treated as having miningScan:1
//		<SystemMap>
//			iconScale: (int)
//				the % size to draw icons on the map when the map is zoomed at 100% scale.
//				Default: 100
//			iconScaleFactor: (double)
//				the relative amount to scale the icons as the map is zoomed in or out.
//				1.0 = normal scale, 0.0 = no change in scale. Default: 1.0
//
//	 58: 2.0 Alpha 8
//		tlisp:
//			(@ nestedListsOrStructs idxOrKey1 [idxOrKey2 ...])
//				Returns item index from innermost list or structs (lists are 0-based)
//			(@@ nestedListsOrStructs idxOrKey1 [idxOrKey2 ...])
//				Returns item index from innermost list or structs (lists are 0-based)
//				Supports all functionality of @, but treats negative list indexes as
//				indexing from the end in reverse (as in slice)
//			(scrGetDataKeys obj)
//				Returns a list of typData keys for the given obj type
//			(scrGetStaticDataKeys type)
//				Returns a list of static datakeys for the given obj type
//			(scr@Keys type)
//				Returns a list of all instance property and custom global property keys for the given obj
//			(sysGetNextNodeTo [srcNode] destNode [options])
//				Accepts an options struct now
//				options:
//					blockNodes (list): do not path through these nodes
//					respectOneWayGates (bool): if pathing must obey the directionality of one way gates (default: false)
//					gateCriteria (string): criteria to match against stargate topology attributes (not the stations)
//			(sysGetPathTo [srcNode] destNode [options])
//				options:
//					blockNodes (list): do not path through these nodes
//					respectOneWayGates (bool): if pathing must obey the directionality of one way gates (default: false)
//					gateCriteria (string): criteria to match against stargate topology attributes (not the stations)
//		<AdventureDesc>
//			<Constants>
//				<ExternalDeviceDamageMaxLevels>
//					deviceDamageMaxDeviceLevel: (damageAdj-style list of ints)
//						The maximum level device that this weapon shot can damage
//						Specifying a "+" before a level means this is a positive
//							offset relative to the level of the item
//						Specifying a "-" before a level means this is a negative
//							offset relative to the level of the item
//					deviceDamageTypeAdj:
//						% damageAdj of this damage type to devices
//						generic damage always has 100%
//						null damage has 100% if it also has device:# in its desc
//					deviceHitChance:
//						% chance for a shot of any damage type to hit a device
//				<InternalDeviceDamageMaxLevels>
//					deviceDamageMaxDeviceLevel: (damageAdj-style list of ints)
//						The maximum level device that this weapon shot can damage
//						Specifying a "+" before a level means this is a positive
//							offset relative to the level of the item
//						Specifying a "-" before a level means this is a negative
//							offset relative to the level of the item
//					deviceDamageTypeAdj:
//						% damageAdj of this damage type to devices
//						generic damage always has 100%
//						null damage has 100% if it also has device:# in its desc
//					deviceHitChance:
//						% chance for a shot of any damage type to hit a device
//				<DamageMethodAdj>
//					<WMD>
//						damageMethodAdj: (damageAdj-style list of exactly 8 ints or doubles)
//							The baseline adjustment curve (note that fortification
//							can adjust this further)
//						damageMethodDisplay: (attribute-style list of exactly 7 alphanumeric strings)
//							Strings to display alongside wmdDisplayPrefix for WMD1-WMD7
//							If you provide a "!", a given level will not show the attribute.
//						damageMethodDisplayPrefix: (string)
//							A string prefix (ex, "WMD ") which the given wmdDisplay
//							is appended to at that given WMD level. Default: "WMD "
//			<Properties>
//				<Constant id="core.damageMethod.item">
//					Returns a nested struct specifying for different item types:
//						armor
//						shield
//					The allowed fortification for the following damage methods:
//						WMD (in the WMD system)
//					Default WMD fortification adj properties for low/no WMD damage.
//					Accepts a floating point which adjusts the default WMD curve for that target type
//					by performing a transform on the adventure-standard damage method scale.
//					Note: WMD7 is always 1.0 (full damage), the rest of the curve is adjusted.
//					Values must be 0.0 or greater.
//					0.0: the adjustment is always full damage regardless of damage method level
//					0.0 < fortification < 1.0: The adjustment penalizes not having the damage
//						method less, but also rewards having any of the damage method proportionally
//						more
//					1.0: the adjustment is the adventure standard curve
//					1.0 < fortification: The adjustment penalizes not having the damage method
//						more, but also proportionally favors high amounts of that damage method
//					The exact curve adjustments will vary depending on the adventure curve.
//					The value may be 'inf to confer immunity unless WMD7 is present.
//					The fortification algorithm employed is as follows:
//						adj = WMD[level] ^ (log(WMD0 / (fortification * (1 - WMD0) + WMD0)) / log(WMD0))
//					Default:
//						armor:
//							WMD: 0.0
//						shield:
//							WMD: 0.0
//				<Constant id="core.damageMethod.ship">
//					Returns a nested struct specifying for different target types:
//						armor
//							critical
//							nonCritical
//							nonCriticalDestruction
//							nonCriticalDestructionChance
//								Note: this is the base destruction chance at 0hp when
//								no compartment is present
//						compartment
//								Note: uses the default compartment of the ship
//							general
//							cargo
//							mainDrive
//					The allowed fortification for the following damage methods:
//						WMD (in the WMD system)
//					Default WMD fortification adj properties for low/no WMD damage.
//					Accepts a floating point which adjusts the default WMD curve for that target type
//					by performing a transform on the adventure-standard damage method scale.
//					Note: WMD7 is always 1.0 (full damage), the rest of the curve is adjusted.
//					Values must be 0.0 or greater.
//					0.0: the adjustment is always full damage regardless of damage method level
//					0.0 < fortification < 1.0: The adjustment penalizes not having the damage
//						method less, but also rewards having any of the damage method proportionally
//						more
//					1.0: the adjustment is the adventure standard curve
//					1.0 < fortification: The adjustment penalizes not having the damage method
//						more, but also proportionally favors high amounts of that damage method
//					The exact curve adjustments will vary depending on the adventure curve.
//					The value may be 'inf to confer immunity unless WMD7 is present.
//					The fortification algorithm employed is as follows:
//						adj = WMD[level] ^ (log(WMD0 / (fortification * (1 - WMD0) + WMD0)) / log(WMD0))
//					Default:
//						armor:
//							critical:
//								WMD: 0.0
//							nonCritical:
//								WMD: 0.0
//							nonCriticalDestruction:
//								WMD: 1.0
//							nonCriticalDestructionChance:
//								(this is not a fortification value, but a base chance of destruction)
//								WMD: 0.05
//						compartment:
//							general:
//								WMD: 1.0
//							cargo:
//								WMD: 1.0
//							mainDrive:
//								WMD: 1.0
//				<Constant id="core.damageMethod.station">
//					Returns a nested struct specifying for different target types:
//						hull
//							single
//							multi
//							asteroid
//								Note: In the WMD system, this uses the greater of mining or WMD
//							underground
//								Note: in the WMD system, this use mining instead of WMD
//					The allowed fortification adj for the following damage methods:
//						WMD (in the WMD system)
//					Default WMD fortification properties for low/no WMD damage.
//					Accepts a floating point which adjusts the default WMD curve for that target type
//					by performing a transform on the adventure-standard damage method scale.
//					Note: WMD7 is always 1.0 (full damage), the rest of the curve is adjusted.
//					Values must be 0.0 or greater.
//					0.0: the adjustment is always full damage regardless of damage method level
//					0.0 < fortification < 1.0: The adjustment penalizes not having the damage
//						method less, but also rewards having any of the damage method proportionally
//						more
//					1.0: the adjustment is the adventure standard curve
//					1.0 < fortification: The adjustment penalizes not having the damage method
//						more, but also proportionally favors high amounts of that damage method
//					The exact curve adjustments will vary depending on the adventure curve.
//					The value may be 'inf to confer immunity unless WMD7 is present.
//					The fortification algorithm employed is as follows:
//						adj = WMD[level] ^ (log(WMD0 / (fortification * (1 - WMD0) + WMD0)) / log(WMD0))
//					Default:
//						hull:
//							single:
//								WMD: 0.0
//							multi:
//								WMD: 1.0
//							asteroid:
//								WMD: 1.0
//							underground:
//								WMD: 1.0
//				<Constant id="core.damageMethod.minAdj">
//					(This is a template property, see below for valid property names.)
//					These properties define the minimum adjustment values that can
//					result due to applying WMD fortification adjustment.
//					<Constant id = "core.WMDFortified.DefaultMinAdj">
//						Default 0.0
//						Cannot be greater than 1.0
//						Cannot be lower than 0.0
//				<Constant id="core.damageMethod.minDamage">
//					The min damage dealt after WMD adjustment
//					Accepts a floating point value for stochastic damage
//					Default 0.0
//				<Constant id="core.damageMethod.system">
//					The damage method system to use.
//					Currently only WMD is available.
//					Default 'WMD
//				<Constant id="core.item.shield.idlePowerAdj">
//					Returns a floating point value to adjust idle power consumption of shields.
//					Default: 0.125
//		<ItemType>
//			<Armor>
//				fortificationWMDAdj: (float)
//					Accepts a floating point which adjusts the default WMD curve for that target type
//					by performing a transform on the adventure-standard damage method scale.
//					Note: WMD7 is always 1.0 (full damage), the rest of the curve is adjusted.
//					Values must be 0.0 or greater.
//					0.0: the adjustment is always full damage regardless of damage method level
//					0.0 < fortification < 1.0: The adjustment penalizes not having the damage
//						method less, but also rewards having any of the damage method proportionally
//						more
//					1.0: the adjustment is the adventure standard curve
//					1.0 < fortification: The adjustment penalizes not having the damage method
//						more, but also proportionally favors high amounts of that damage method
//					The exact curve adjustments will vary depending on the adventure curve.
//					The value may be 'inf to confer immunity unless WMD7 is present.
//					The fortification algorithm employed is as follows:
//						adj = WMD[level] ^ (log(WMD0 / (fortification * (1 - WMD0) + WMD0)) / log(WMD0))
//					Default: (reads adventure default: 0.0)
//			<Shield>
//				fortificationWMDAdj: (float)
//					Accepts a floating point which adjusts the default WMD curve for that target type
//					by performing a transform on the adventure-standard damage method scale.
//					Note: WMD7 is always 1.0 (full damage), the rest of the curve is adjusted.
//					Values must be 0.0 or greater.
//					0.0: the adjustment is always full damage regardless of damage method level
//					0.0 < fortification < 1.0: The adjustment penalizes not having the damage
//						method less, but also rewards having any of the damage method proportionally
//						more
//					1.0: the adjustment is the adventure standard curve
//					1.0 < fortification: The adjustment penalizes not having the damage method
//						more, but also proportionally favors high amounts of that damage method
//					The exact curve adjustments will vary depending on the adventure curve.
//					The value may be 'inf to confer immunity unless WMD7 is present.
//					The fortification algorithm employed is as follows:
//						adj = WMD[level] ^ (log(WMD0 / (fortification * (1 - WMD0) + WMD0)) / log(WMD0))
//					Default: (reads adventure default: 0.0)
//			<Weapon>
//				damage: (str: damage desc)
//					damage changes:
//						0-damage is treated as non-hostile as long as no damaging
//							effects are in the damage descriptor
//						0-damage shots trigger hostile onAttached events if they
//							are marked as hostile
//						0-damage shots trigger onHit events if they are marked as
//							hostile
//						0-damage shots will still impart momentum
//					null: New special damage type
//						Null damage is treated as non-hostile
//						Null damage does not trigger hostile onAttacked events
//						Null damage does trigger onHit type events
//						Null damage can impart any status effect while remaining non-hostile
//						All targets of null damage have a damageAdj of 0 (immunity)
//							to null damage's HP value (this allows scripts that utilize
//							damage output, like mining, to function correctly)
//				miningMethod: "ablative"|"drill"|"explosive"|"shockwave"|"universal"
//					universal: new mining method (no bonuses, no penalties)
//						As of API58, this mining method must be manually assigned.
//		<ShipClass>
//			<Armor>
//				fortificationWMDAdj: (float)
//					Accepts a floating point which adjusts the default WMD curve for that target type
//					by performing a transform on the adventure-standard damage method scale.
//					Note: WMD7 is always 1.0 (full damage), the rest of the curve is adjusted.
//					Values must be 0.0 or greater.
//					0.0: the adjustment is always full damage regardless of damage method level
//					0.0 < fortification < 1.0: The adjustment penalizes not having the damage
//						method less, but also rewards having any of the damage method proportionally
//						more
//					1.0: the adjustment is the adventure standard curve
//					1.0 < fortification: The adjustment penalizes not having the damage method
//						more, but also proportionally favors high amounts of that damage method
//					The exact curve adjustments will vary depending on the adventure curve.
//					The value may be 'inf to confer immunity unless WMD7 is present.
//					The fortification algorithm employed is as follows:
//						adj = WMD[level] ^ (log(WMD0 / (fortification * (1 - WMD0) + WMD0)) / log(WMD0))
//					Default: (reads adventure default: 0.0)
//			<Hull>
//				fortificationWMDAdj: (float)
//					Accepts a floating point which adjusts the default WMD curve for that target type
//					by performing a transform on the adventure-standard damage method scale.
//					Note: WMD7 is always 1.0 (full damage), the rest of the curve is adjusted.
//					Values must be 0.0 or greater.
//					0.0: the adjustment is always full damage regardless of damage method level
//					0.0 < fortification < 1.0: The adjustment penalizes not having the damage
//						method less, but also rewards having any of the damage method proportionally
//						more
//					1.0: the adjustment is the adventure standard curve
//					1.0 < fortification: The adjustment penalizes not having the damage method
//						more, but also proportionally favors high amounts of that damage method
//					The exact curve adjustments will vary depending on the adventure curve.
//					The value may be 'inf to confer immunity unless WMD7 is present.
//					The fortification algorithm employed is as follows:
//						adj = WMD[level] ^ (log(WMD0 / (fortification * (1 - WMD0) + WMD0)) / log(WMD0))
//					Default: (reads adventure default: 1.0)
//		<StationType>
//			fortificationWMDAdj: (float)
//				Accepts a floating point which adjusts the default WMD curve for that target type
//				by performing a transform on the adventure-standard damage method scale.
//				Note: WMD7 is always 1.0 (full damage), the rest of the curve is adjusted.
//				Values must be 0.0 or greater.
//				0.0: the adjustment is always full damage regardless of damage method level
//				0.0 < fortification < 1.0: The adjustment penalizes not having the damage
//					method less, but also rewards having any of the damage method proportionally
//					more
//				1.0: the adjustment is the adventure standard curve
//				1.0 < fortification: The adjustment penalizes not having the damage method
//					more, but also proportionally favors high amounts of that damage method
//				The exact curve adjustments will vary depending on the adventure curve.
//				The value may be 'inf to confer immunity unless WMD7 is present.
//				The fortification algorithm employed is as follows:
//					adj = WMD[level] ^ (log(WMD0 / (fortification * (1 - WMD0) + WMD0)) / log(WMD0))
//				Default: (reads adventure default:
//					0.0 for single hull stations, 1.0 for multi/asteroid/underground)
//
//	 59: 2.0 Alpha 9
//		tlisp:
//			(?= ...)
//				Returns true if a valid full-coercion comparison (via =, !=, etc)
//				can be made between the arguments (processes in pairs, left to right)
//				If false, all comparators except != will always return false for the
//				given sequence of types. != will always return true.
//			(?== ...)
//				Returns true if a valid basic-coercion comparison (via eq, neq, etc)
//				can be made between the arguments (processes in pairs, left to right)
//				If false, all comparators except neq will always return false for the
//				given sequence of types. neq will always return true.
//			(?=== ...)
//				Returns true if a valid no-coercion comparison (via ===, !=== etc)
//				can be made between the arguments (processes in pairs, left to right)
//				If false, all comparators except !=== will always return false for the
//				given sequence of types. !=== will always return true.
//			(fmtNumber [format] number)
//				Updated to now accept the following additional formats:
//					'CBM					32.5 kCBM	- cubic meters with SI prefixes
//					'CBMBasic				32500 CBM	- cubic meters, or mCBM if 0.001<=val<1.0, or 0 - mirrors massKg except input in CBM
//					'CBMInt					32500 CBM	- cubic meters, or 0 if <1.0 - mirrors massTons
//					'metric					10.5 G
//					'metricFull				10.5 Giga
//					'metricUnitless			10.5G
//			(itm@ itm [object] property)
//			(itmFind ...)
//				See criteria update in (itmMatches)
//			(itmGetMassKg item)
//				Returns item mass in kg
//			(itmGetMass item)
//				DEPRECATED: (synonym for itemGetVolume, for actually getting the mass use itmGetMassKg instead)
//			(itmGetVolume item)
//				Returns volume in CBM (real)
//			(itmMatches criteria item)
//				Supports volume matching "@" with <,=,> and range operators
//			(obj@ obj property)
//				New and updated properties:
//					'cargoSpace				Available cargo space in cubic meters (previously tons)
//					'cargoSpaceFree			Available cargo space in cubic meters, double
//					'cargoSpaceFreeLiters	Available cargo space in liters, int
//						this is purely for supporting legacy math operations that expected kg
//					'cargoSpaceFreeKg		DEPRECATED: Available cargo space in liters, int (NOT kg)
//						this is purely for supporting legacy cargo math that needs it in liters to check if items will fit
//					'cargoSpaceUsed			Used cargo space in cubic meters, double
//					'cargoSpaceUsedLiters	Used cargo space in liters, int
//						this is purely for supporting legacy math operations that expected kg
//					'cargoSpaceUsedKg		DEPRECATED: Used cargo space in liters, int (NOT kg)
//						this is purely for supporting legacy cargo math that needs it in liters to check if items will fit
//					'maxArmorClass			LEGACY SUPPORT: Maximum armor class in "compatibility tons" (double)
//					'maxArmorClassName		Name of the maximum armor class
//					'maxArmorMass			DEPRECATED: use maxArmorClassName instead (or maxArmorClass if you need to support legacy ships)
//					'maxCargoSpace			Total cargo space in cubic meters (previously tons)
//					'stdArmorClass			LEGACY SUPPORT: Standard armor class in "compatibility tons" (double)
//					'stdArmorClassName		Name of the standard armor class
//					'stdArmorMass			DEPRECATED: use stdArmorClassName instead (or stdArmorClass if you need to support legacy ships)
//			(objGetCargoSpaceLeft obj)
//				DEPRECATED: returns cargo space in liters, for compatibility math. Name implies returning in cubic meters though.
<<<<<<< feature/106285-2arg_gaussianRandom
//			(randomGaussian low [mid] high)
//				Returns double if any argument is a double
//				Now has a 2-arg form which uses the midpoint of low and high
=======
//			(sysAddEncounterEvent / sysAddEncounterEventAtDist / sysCreateEncounter / sysCreateShip)
//				All Encounter functions updated to accept options struct consistent with sysCreateShip
//				Corrections to Encounter functions to match behavior to documentation and sysCreateShip
//				New options:
//					level			Perform ShipTable lookups using the speficied level (instead of system level)
//					levelAdj		Perform ShipTable lookups using system level + levelAdj
//					ignoreLimits	Ignore any ship limits in ShipTable lookups
//			(typGetDataField type field)
//				New and updated fields
//					;;ItemType
//						'volume					returns the volume in CBM as a string
//					;;ShipClass
//						'maxArmorSize			returns max armor size in CBM as a string
//						'maxArmorMass			DEPRECATED: use maxArmorSize instead (returns same value multiplied by 1000)
//		<AdventureDesc>
//			<Properties>
//				<Constant id="core.item.xmlMassToRealVolume">	1.0</Constant>
//					Conversion ratio of tons->cubic meters for legacy items that specify only mass
//					accepts a double
//					Default: 1.0
//				<Constant id="core.item.defaultDensity">		1.0</Constant>
//					Default density in tons per cubic meter to apply to legacy items that specify only mass
//					accepts a double
//					Default: 1.0
//		<Type>
//			<ArmorClassDesc>
//				<ArmorClass>
//					New name for ArmorMass tag
//                  compatibilitySize: A unitless size that is analogous to either mass in tons or volume in CBM, depending on what the item specifies
//					mass: (Deprecated name to avoid confusion) Autoconverts legacy mass= values (mass in kg) to compatibility size
//					size: max size armor for this armor class (in CBM)
//					label: display name of armor class (unchanged from MassClass)
//					shortLabel:	new string field for displaying mass classes when the mass class names are too long
//				<ArmorClassAlias>
//					New feature to allow compatibility support for old armor class names (ex, aliasing Dreadnought -> SuperMassive)
//					id: name of the ArmorClass to alias
//					idAlias: name of the alias
//				<MassClass>
//					Deprecated tag name, use ArmorClass instead (Functions identically)
//				<[Any Other Name]>
//					Deprecated tag name. APIs <59 allowed any named tags instead of just MassClass and they were interpreted as MassClass.
//					Unsupported in API59+ (will cause a load error). This is to allow for additional tag types in the future.
//			<ArmorMassDesc>
//				Deprecated tag name, use ArmorClassDesc instead (Functions identically)
//		<ItemType>
//			enhancementUsesObjectImage: (bool)
//				If True:
//					Display the obj image if seen as an enhancement type
//					Display the item image if just an item
//				(default: false)
//			installedItemUsesObjectImage: (bool)
//				If True:
//					Display the obj image if installed on an object
//					Display the item image if not installed
//				(default: false)
//			density: (double) Species a specific density ratio in metric tons per cubic meter (aka, grams per cubic centimeter)
//			mass: (int) If present without volume, this value is auto-converted using adventure properties to a volume, and the
//				actual mass is computed from that volume back into mass using another adventure property. This is still an int
//				for legacy support reasons. A future API may support doubles.
//			volume: (double) volume of the item in cubic meters. Values <1 liter (0.001CBM) are allowed
//			<Armor>
//				armorClass: explicitly define an armor class ID to assign this armor to (ex, "medium")
//					See the <ArmorClassDesc> loaded with the adventure for valid names other than
//						"medium" and "maximum", which are mandated by the engine.
//		<ShipType>
//			cargoSpace: now specifies CBM instead of tons
//				If <API59, always uses the adventure compatibility mappings
//			maxCargoSpace: now specifies CBM instead of tons
//				If <API59, always uses the adventure compatibility mappings
//			<Hull>
//				cargoSpace: now specifies CBM instead of tons
//					If <API59, always uses the adventure compatibility mappings
//				maxArmor: deprecated
//				maxArmorClass:
//					Not recommended (use <ArmorLimit .../> instead)
//					Provides same functionality as maxArmor, but allows specifying
//					non-classed armor sizes in floating point "compatibility tons",
//					rather than in integer kg.
//				maxCargoSpace: now specifies CBM instead of tons
//					If <API59, always uses the adventure compatibility mappings
//				stdArmor: deprecated
//				stdArmorClass:
//					Not recommended (use <ArmorLimit .../> instead)
//					Provides same functionality as stdArmor, but allows specifying
//					non-classed armor sizes in floating point "compatibility tons",
//					rather than in integer kg.
//			<DeviceSlots>
//				<DeviceSlot>
//					fireAngle: if used with minFireArc and maxFireArc or omnidirectional, can now specify a default angle of fire
//						(including in and out of the normal min and max fire arc) to use when there is no valid
//						target in the fire arc and the weapon is allowed to fire (eg, +linkedfire:always)
//		<Type>
//			<Events>
//				<onGlobalTypesBound>
//					New event for state initialization code
//					Fires after all types are bound, inheritance is resolved, and properties
//						are available
//					It is guaranteed that <onGlobalTypesBoundNewGame> always fires before
//						this event fires, meaning that one-time pre-initialization can be
//						handled in that event, and this event can handle any repeat initialization
//						that has to happen after
//				<onGlobalTypesBoundNewGame>
//					New event for state initialization code
//					Fires after all types are bound, inheritance is resolved, and properties
//						are available
//					Fires only on new games, or when loading into the  main menu
//					Fires BEFORE <onGlobalTypesBound>
//		<ItemType>
//			<Weapon>
//				ignoredByAI:
//					True if the AI should ignore the existence of this weapon for the purposes
//					of aiming, targeting, and best weapon selection.
//					Some configurations (ex, configuring it as a linked fire weapon) may force
//					an AI ship to activate it anyways, as this is a property of the ship's device
//					setup, not a property of the AI deciding to use the weapon as a weapon.
//					Default: false
//				repeatingDelay:
//					Deprecated due to using non-intuitive legacy math
//				repeatingShotDelay:
//					Sets an exact repeating shot delay in simulation seconds
//					Default: 2.0
//		<ShipClass>
//			<Language>
//				<... id="core.descLore">
//					This field is the default field for lore/encyclopedia entries
//					Ex, when viewing an in-game encyclopedia/codex
//					core.desc or playerDesc="" are used as fallbacks (in that order)
//				<... id="core.desc">
//					This field is the default field for gameplay entries
//					Ex, during ship selection
//					playerDesc="" or core.descLore are used as fallbacks (in that order)
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
//	212: 1.9
//		R/W entire SParticle struct directly
//
//	213: 2.0 Alpha 2
//		Change DiceRange to use -1 for not set
//
//	214: 2.0 Alpha 7
//		Change CParticleArray::m_iLifeLeft to milliseconds game time
//		(instead of ticks)
//
//	215: 2.0 Alpha 7
//		Add DamageDesc::m_fMiningScan
// 
//	216: 2.0 Alpha 8
//		Fix DamageDesc::m_MassDestructionAdj
//		makes extra DWORDs in DamageDesc a little bit more change-safe
// 
//  217: 2.0 Alpha 8
//		Switch CInstalledDevice m_rActivateDelay from WORD to double
//		Switch CInstalledDevice m_i
//
//	218: 2.0 Alpha 8
//		Add DamageMethodCrushAdj to SExtraDamage in DamageDesc
//		Add DamageMethodPierceAdj to SExtraDamage in DamageDesc
//		Add DamageMethodShredAdj to SExtraDamage in DamageDesc
//
//	219: 2.0 Alpha 9
//		Add m_fRecalcItemMass to CShip
//		Add m_rItemVolume to CShip
//		Add m_rCargoVolume to CShip
// 
//  220: 2.0 Alpha 9
//		Add m_iDefaultFireAngle to CInstalledDevice
//
//  221: 2.0 Alpha 9
//		Add m_pSovereign to CTimedEncounterEvent
//		Add m_pOverride to CTimedEncounterEvent
//		Add m_iLevel to CTimedEncounterEvent
//		Add m_bIgnoreLimits to CTimedEncounterEvent
//

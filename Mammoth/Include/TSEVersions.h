//	TSEVersions.h
//
//	Version numbers and history
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#pragma once

constexpr DWORD API_VERSION =							46;
constexpr DWORD UNIVERSE_SAVE_VERSION =					37;
constexpr DWORD SYSTEM_SAVE_VERSION =					174;

//	Uncomment out the following define when building a stable release

//#define TRANSCENDENCE_STABLE_RELEASE

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
//		m_iStealth in CShip
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
//		Added m_WeaponTargets
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

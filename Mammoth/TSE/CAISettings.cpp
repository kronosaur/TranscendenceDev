//	CAISettings.cpp
//
//	CAISettings class
//	Copyright (c) 2011 by George Moromisato. All Rights Reserved.

#include "PreComp.h"

#define AGGRESSOR_ATTRIB						CONSTLIT("aggressor")
#define ASCEND_ON_GATE_ATTRIB					CONSTLIT("ascendOnGate")
#define COMBAT_SEPARATION_ATTRIB				CONSTLIT("combatSeparation")
#define COMBAT_STYLE_ATTRIB						CONSTLIT("combatStyle")
#define FIRE_ACCURACY_ATTRIB					CONSTLIT("fireAccuracy")
#define FIRE_RANGE_ADJ_ATTRIB					CONSTLIT("fireRangeAdj")
#define FIRE_RATE_ADJ_ATTRIB					CONSTLIT("fireRateAdj")
#define FLOCK_FORMATION_ATTRIB					CONSTLIT("flockFormation")
#define FLOCKING_STYLE_ATTRIB					CONSTLIT("flockingStyle")
#define FLYBY_COMBAT_ATTRIB						CONSTLIT("flybyCombat")
#define IS_PLAYER_ATTRIB						CONSTLIT("isPlayer")
#define NO_SHIELD_RETREAT_ATTRIB				CONSTLIT("ignoreShieldsDown")
#define NO_ATTACK_ON_THREAT_ATTRIB				CONSTLIT("noAttackOnThreat")
#define NO_DOGFIGHTS_ATTRIB						CONSTLIT("noDogfights")
#define NO_FRIENDLY_FIRE_ATTRIB					CONSTLIT("noFriendlyFire")
#define NO_FRIENDLY_FIRE_CHECK_ATTRIB			CONSTLIT("noFriendlyFireCheck")
#define NO_NAV_PATHS_ATTRIB						CONSTLIT("noNavPaths")
#define NO_ORDER_GIVER_ATTRIB					CONSTLIT("noOrderGiver")
#define NO_TARGETS_OF_OPPORTUNITY_ATTRIB		CONSTLIT("noTargetsOfOpportunity")
#define NON_COMBATANT_ATTRIB					CONSTLIT("nonCombatant")
#define PERCEPTION_ATTRIB						CONSTLIT("perception")
#define REACT_TO_ATTACK_ATTRIB					CONSTLIT("reactToAttack")
#define REACT_TO_THREAT_ATTRIB					CONSTLIT("reactToThreat")
#define STAND_OFF_COMBAT_ATTRIB					CONSTLIT("standOffCombat")
#define THREAT_RANGE_ATTRIB						CONSTLIT("threatRange")

#define COMBAT_STYLE_ADVANCED					CONSTLIT("advanced")
#define COMBAT_STYLE_CHASE						CONSTLIT("chase")
#define COMBAT_STYLE_FLYBY						CONSTLIT("flyby")
#define COMBAT_STYLE_NO_RETREAT					CONSTLIT("noRetreat")
#define COMBAT_STYLE_STANDARD					CONSTLIT("standard")
#define COMBAT_STYLE_STAND_OFF					CONSTLIT("standOff")

#define FLOCKING_STYLE_CLOUD					CONSTLIT("cloud")
#define FLOCKING_STYLE_COMPACT					CONSTLIT("compact")
#define FLOCKING_STYLE_RANDOM					CONSTLIT("random")

#define STR_TRUE								CONSTLIT("True")

static TStaticStringTable<TStaticStringEntry<AIReaction>, 7> REACTION_TABLE = {
	"chase",				AIReaction::Chase,
	"default",				AIReaction::Default,
	"destroy",				AIReaction::Destroy,
	"deter",				AIReaction::Deter,
	"deterNoManeuvers",		AIReaction::DeterWithSecondaries,
	"gate",					AIReaction::Gate,
	"none",					AIReaction::None,
	};

AICombatStyle CAISettings::ConvertToAICombatStyle (const CString &sValue)

//	ConvertToAICombatStyle
//
//	Converts a string to combat style

	{
	if (strEquals(sValue, COMBAT_STYLE_ADVANCED))
		return AICombatStyle::Advanced;
	else if (strEquals(sValue, COMBAT_STYLE_CHASE))
		return AICombatStyle::Chase;
	else if (strEquals(sValue, COMBAT_STYLE_FLYBY))
		return AICombatStyle::Flyby;
	else if (strEquals(sValue, COMBAT_STYLE_NO_RETREAT))
		return AICombatStyle::NoRetreat;
	else if (strEquals(sValue, COMBAT_STYLE_STANDARD))
		return AICombatStyle::StandOff;
	else if (strEquals(sValue, COMBAT_STYLE_STAND_OFF))
		return AICombatStyle::StandOff;
	else
		return AICombatStyle::Standard;
	}

AIFlockingStyle CAISettings::ConvertToFlockingStyle (const CString &sValue)

//	ConvertToFlockingStyle
//
//	Converts a string to flocking style

	{
	if (strEquals(sValue, FLOCKING_STYLE_CLOUD))
		return AIFlockingStyle::Cloud;
	else if (strEquals(sValue, FLOCKING_STYLE_COMPACT))
		return AIFlockingStyle::Compact;
	else if (strEquals(sValue, FLOCKING_STYLE_RANDOM))
		return AIFlockingStyle::Random;
	else
		return AIFlockingStyle::None;
	}

AIReaction CAISettings::ConvertToAIReaction (const CString &sValue)

//	ConvertToAIReaction
//
//	Converts a string to an AIReaction

	{
	auto pEntry = REACTION_TABLE.GetAt(sValue);
	if (pEntry)
		return pEntry->Value;
	else
		return AIReaction::Default;
	}

CString CAISettings::ConvertToID (AICombatStyle iStyle)

//	ConvertToID
//
//	Converts a combat style to a string ID

	{
	switch (iStyle)
		{
		case AICombatStyle::Standard:
			return COMBAT_STYLE_STANDARD;

		case AICombatStyle::StandOff:
			return COMBAT_STYLE_STAND_OFF;

		case AICombatStyle::Flyby:
			return COMBAT_STYLE_FLYBY;

		case AICombatStyle::NoRetreat:
			return COMBAT_STYLE_NO_RETREAT;

		case AICombatStyle::Chase:
			return COMBAT_STYLE_CHASE;

		case AICombatStyle::Advanced:
			return COMBAT_STYLE_ADVANCED;

		default:
			return NULL_STR;
		}
	}

CString CAISettings::ConvertToID (AIFlockingStyle iStyle)

//	ConvertToID
//
//	Converts a flocking style to a string ID

	{
	switch (iStyle)
		{
		case AIFlockingStyle::Cloud:
			return FLOCKING_STYLE_CLOUD;

		case AIFlockingStyle::Compact:
			return FLOCKING_STYLE_COMPACT;

		case AIFlockingStyle::Random:
			return FLOCKING_STYLE_RANDOM;

		default:
			return NULL_STR;
		}
	}

CString CAISettings::ConvertToID (AIReaction iStyle)

//	ConvertToID
//
//	Converts to an ID.

	{
	return REACTION_TABLE.FindKey(iStyle);
	}

CString CAISettings::GetValue (const CString &sSetting)

//	GetValue
//
//	Get the current value of the given setting

	{
	if (strEquals(sSetting, AGGRESSOR_ATTRIB))
		return (m_fAggressor ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, ASCEND_ON_GATE_ATTRIB))
		return (m_fAscendOnGate ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, COMBAT_SEPARATION_ATTRIB))
		return (m_rMinCombatSeparation > 0.0 ? strFromInt((int)(m_rMinCombatSeparation / g_KlicksPerPixel)) : NULL_STR);
	else if (strEquals(sSetting, COMBAT_STYLE_ATTRIB))
		return ConvertToID(m_iCombatStyle);
	else if (strEquals(sSetting, FIRE_ACCURACY_ATTRIB))
		return strFromInt(m_iFireAccuracy);
	else if (strEquals(sSetting, FIRE_RANGE_ADJ_ATTRIB))
		return strFromInt(m_iFireRangeAdj);
	else if (strEquals(sSetting, FIRE_RATE_ADJ_ATTRIB))
		return strFromInt(m_iFireRateAdj);
	else if (strEquals(sSetting, FLOCKING_STYLE_ATTRIB))
		return ConvertToID(m_iFlockingStyle);
	else if (strEquals(sSetting, FLOCK_FORMATION_ATTRIB))
		return (m_iFlockingStyle == AIFlockingStyle::Cloud ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, IS_PLAYER_ATTRIB))
		return (m_fIsPlayer ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, NO_ATTACK_ON_THREAT_ATTRIB))
		return (m_fNoAttackOnThreat ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, NO_TARGETS_OF_OPPORTUNITY_ATTRIB))
		return (m_fNoTargetsOfOpportunity ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, NO_DOGFIGHTS_ATTRIB))
		return (m_fNoDogfights ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, NO_SHIELD_RETREAT_ATTRIB))
		return (m_fNoShieldRetreat ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, NO_FRIENDLY_FIRE_ATTRIB))
		return (m_fNoFriendlyFire ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, NO_FRIENDLY_FIRE_CHECK_ATTRIB))
		return (m_fNoFriendlyFireCheck ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, NO_NAV_PATHS_ATTRIB))
		return (m_fNoNavPaths ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, NO_ORDER_GIVER_ATTRIB))
		return (m_fNoOrderGiver ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, NON_COMBATANT_ATTRIB))
		return (m_fNonCombatant ? STR_TRUE : NULL_STR);
	else if (strEquals(sSetting, PERCEPTION_ATTRIB))
		return strFromInt(m_iPerception);
	else if (strEquals(sSetting, REACT_TO_ATTACK_ATTRIB))
		return ConvertToID(m_iReactToAttack);
	else if (strEquals(sSetting, REACT_TO_THREAT_ATTRIB))
		return ConvertToID(m_iReactToThreat);
	else if (strEquals(sSetting, THREAT_RANGE_ATTRIB))
		return strFromInt(mathRound(m_rThreatRange / LIGHT_SECOND));
	else
		return NULL_STR;
	}

ALERROR CAISettings::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML element

	{
	//	Combat style

	CString sCombatStyle;
	if (pDesc->FindAttribute(COMBAT_STYLE_ATTRIB, &sCombatStyle))
		m_iCombatStyle = ConvertToAICombatStyle(sCombatStyle);
	else
		{
		//	Compatibility with version < 0.97

		if (pDesc->GetAttributeBool(FLYBY_COMBAT_ATTRIB))
			m_iCombatStyle = AICombatStyle::Flyby;
		else if (pDesc->GetAttributeBool(STAND_OFF_COMBAT_ATTRIB))
			m_iCombatStyle = AICombatStyle::StandOff;
		else
			m_iCombatStyle = AICombatStyle::Standard;
		}

	//	Flocking style

	CString sFlockingStyle;
	if (pDesc->FindAttribute(FLOCKING_STYLE_ATTRIB, &sFlockingStyle))
		m_iFlockingStyle = ConvertToFlockingStyle(sFlockingStyle);
	else
		{
		bool bValue;
		if (pDesc->FindAttributeBool(FLOCK_FORMATION_ATTRIB, &bValue) && bValue)
			m_iFlockingStyle = AIFlockingStyle::Cloud;
		else
			m_iFlockingStyle = AIFlockingStyle::None;
		}

	//	Reactions

	m_iReactToAttack = ConvertToAIReaction(pDesc->GetAttribute(REACT_TO_ATTACK_ATTRIB));
	m_iReactToThreat = ConvertToAIReaction(pDesc->GetAttribute(REACT_TO_THREAT_ATTRIB));

	//	Parameters

	m_iFireRateAdj = pDesc->GetAttributeIntegerBounded(FIRE_RATE_ADJ_ATTRIB, 1, -1, 10);
	m_iFireRangeAdj = pDesc->GetAttributeIntegerBounded(FIRE_RANGE_ADJ_ATTRIB, 1, -1, 100);
	m_iFireAccuracy = pDesc->GetAttributeIntegerBounded(FIRE_ACCURACY_ATTRIB, 0, 100, 100);
	m_iPerception = pDesc->GetAttributeIntegerBounded(PERCEPTION_ATTRIB, CSpaceObject::perceptMin, CSpaceObject::perceptMax, CSpaceObject::perceptNormal);

	//	Combat separation

	int iSep;
	if (pDesc->FindAttributeInteger(COMBAT_SEPARATION_ATTRIB, &iSep))
		m_rMinCombatSeparation = iSep * g_KlicksPerPixel;
	else
		//	A negative value means that we compute it based on the image size
		m_rMinCombatSeparation = -1.0;

	//	Threat range

	int iValue = pDesc->GetAttributeIntegerBounded(THREAT_RANGE_ATTRIB, 0, -1, DEFAULT_THREAT_RANGE);
	m_rThreatRange = iValue * LIGHT_SECOND;

	//	Flags

	m_fAscendOnGate = pDesc->GetAttributeBool(ASCEND_ON_GATE_ATTRIB);
	m_fNoShieldRetreat = pDesc->GetAttributeBool(NO_SHIELD_RETREAT_ATTRIB);
	m_fNoDogfights = pDesc->GetAttributeBool(NO_DOGFIGHTS_ATTRIB);
	m_fNonCombatant = pDesc->GetAttributeBool(NON_COMBATANT_ATTRIB);
	m_fNoFriendlyFire = pDesc->GetAttributeBool(NO_FRIENDLY_FIRE_ATTRIB);
	m_fAggressor = pDesc->GetAttributeBool(AGGRESSOR_ATTRIB);
	m_fNoAttackOnThreat = pDesc->GetAttributeBool(NO_ATTACK_ON_THREAT_ATTRIB);
	m_fNoTargetsOfOpportunity = pDesc->GetAttributeBool(NO_TARGETS_OF_OPPORTUNITY_ATTRIB);
	m_fNoFriendlyFireCheck = pDesc->GetAttributeBool(NO_FRIENDLY_FIRE_CHECK_ATTRIB);
	m_fNoNavPaths = pDesc->GetAttributeBool(NO_NAV_PATHS_ATTRIB);
	m_fNoOrderGiver = pDesc->GetAttributeBool(NO_ORDER_GIVER_ATTRIB);
	m_fIsPlayer = false;

	return NOERROR;
	}

void CAISettings::InitToDefault (void)

//	InitToDefault
//
//	Initialize to default settings

	{
	*this = CAISettings();
	}

void CAISettings::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from stream
//
//	DWORD		LO = m_iCombatStyle; HI = m_iFlockingStyle
//	DWORD		LO = m_iFireRateAdj; HI = m_iFireRangeAdj
//	DWORD		LO = m_iFireAccuracy; HI = m_iPerception
//	DWORD		LO = m_iReactToAttack; HI = m_iReactToThreat
//	Metric		m_rThreatRange
//	Metric		m_rMinCombatSeparation
//	DWORD		flags

	{
	DWORD dwLoad;

	Ctx.pStream->Read(dwLoad);
	if (Ctx.dwVersion >= 125)
		{
		m_iCombatStyle = (AICombatStyle)LOWORD(dwLoad);
		m_iFlockingStyle = (AIFlockingStyle)HIWORD(dwLoad);
		}
	else
		{
		m_iCombatStyle = (AICombatStyle)dwLoad;
		m_iFlockingStyle = AIFlockingStyle::None;
		}

	Ctx.pStream->Read(dwLoad);
	m_iFireRateAdj = (int)LOWORD(dwLoad);
	m_iFireRangeAdj = (int)HIWORD(dwLoad);

	Ctx.pStream->Read(dwLoad);
	m_iFireAccuracy = (int)LOWORD(dwLoad);
	m_iPerception = (int)HIWORD(dwLoad);

	if (Ctx.dwVersion >= 197)
		{
		Ctx.pStream->Read(dwLoad);
		m_iReactToAttack = (AIReaction)LOWORD(dwLoad);
		m_iReactToThreat = (AIReaction)HIWORD(dwLoad);

		Ctx.pStream->Read(m_rThreatRange);
		}

	Ctx.pStream->Read(m_rMinCombatSeparation);

	//	Flags

	Ctx.pStream->Read(dwLoad);
	m_fNoShieldRetreat =		((dwLoad & 0x00000001) ? true : false);
	m_fNoDogfights =			((dwLoad & 0x00000002) ? true : false);
	m_fNonCombatant =			((dwLoad & 0x00000004) ? true : false);
	m_fNoFriendlyFire =			((dwLoad & 0x00000008) ? true : false);
	m_fAggressor =				((dwLoad & 0x00000010) ? true : false);
	m_fNoFriendlyFireCheck =	((dwLoad & 0x00000020) ? true : false);
	m_fNoOrderGiver =			((dwLoad & 0x00000040) ? true : false);
	m_fAscendOnGate =			((dwLoad & 0x00000080) ? true : false);
	m_fNoNavPaths =				((dwLoad & 0x00000100) ? true : false);
	m_fNoAttackOnThreat =		((dwLoad & 0x00000200) ? true : false);
	m_fNoTargetsOfOpportunity =	((dwLoad & 0x00000400) ? true : false);
	if (Ctx.dwVersion < 125)
		{
		if (dwLoad & 0x00000800)
			m_iFlockingStyle = AIFlockingStyle::Cloud;
		m_fIsPlayer = false;
		}
	else
		m_fIsPlayer =			((dwLoad & 0x00000800) ? true : false);
	}

CString CAISettings::SetValue (const CString &sSetting, const CString &sValue)

//	SetValue
//
//	Set AISettings value

	{
	if (strEquals(sSetting, AGGRESSOR_ATTRIB))
		m_fAggressor = !sValue.IsBlank();
	else if (strEquals(sSetting, ASCEND_ON_GATE_ATTRIB))
		m_fAscendOnGate = !sValue.IsBlank();
	else if (strEquals(sSetting, COMBAT_SEPARATION_ATTRIB))
		m_rMinCombatSeparation = Max(1, strToInt(sValue, 1)) * g_KlicksPerPixel;
	else if (strEquals(sSetting, COMBAT_STYLE_ATTRIB))
		m_iCombatStyle = ConvertToAICombatStyle(sValue);
	else if (strEquals(sSetting, FIRE_ACCURACY_ATTRIB))
		m_iFireAccuracy = Max(0, Min(strToInt(sValue, 100), 100));
	else if (strEquals(sSetting, FIRE_RANGE_ADJ_ATTRIB))
		m_iFireRangeAdj = Max(1, strToInt(sValue, 100));
	else if (strEquals(sSetting, FIRE_RATE_ADJ_ATTRIB))
		m_iFireRateAdj = Max(1, strToInt(sValue, 10));
	else if (strEquals(sSetting, FLOCKING_STYLE_ATTRIB))
		m_iFlockingStyle = ConvertToFlockingStyle(sValue);
	else if (strEquals(sSetting, FLOCK_FORMATION_ATTRIB))
		m_iFlockingStyle = (!sValue.IsBlank() ? AIFlockingStyle::Cloud : AIFlockingStyle::None);
	else if (strEquals(sSetting, IS_PLAYER_ATTRIB))
		m_fIsPlayer = !sValue.IsBlank();
	else if (strEquals(sSetting, NO_ATTACK_ON_THREAT_ATTRIB))
		m_fNoAttackOnThreat = !sValue.IsBlank();
	else if (strEquals(sSetting, NO_TARGETS_OF_OPPORTUNITY_ATTRIB))
		m_fNoTargetsOfOpportunity = !sValue.IsBlank();
	else if (strEquals(sSetting, NO_DOGFIGHTS_ATTRIB))
		m_fNoDogfights = !sValue.IsBlank();
	else if (strEquals(sSetting, NO_SHIELD_RETREAT_ATTRIB))
		m_fNoShieldRetreat = !sValue.IsBlank();
	else if (strEquals(sSetting, NO_FRIENDLY_FIRE_ATTRIB))
		m_fNoFriendlyFire = !sValue.IsBlank();
	else if (strEquals(sSetting, NO_FRIENDLY_FIRE_CHECK_ATTRIB))
		m_fNoFriendlyFireCheck = !sValue.IsBlank();
	else if (strEquals(sSetting, NO_NAV_PATHS_ATTRIB))
		m_fNoNavPaths = !sValue.IsBlank();
	else if (strEquals(sSetting, NO_ORDER_GIVER_ATTRIB))
		m_fNoOrderGiver = !sValue.IsBlank();
	else if (strEquals(sSetting, NON_COMBATANT_ATTRIB))
		m_fNonCombatant = !sValue.IsBlank();
	else if (strEquals(sSetting, PERCEPTION_ATTRIB))
		m_iPerception = Max((int)CSpaceObject::perceptMin, Min(strToInt(sValue, CSpaceObject::perceptNormal), (int)CSpaceObject::perceptMax));
	else if (strEquals(sSetting, REACT_TO_ATTACK_ATTRIB))
		m_iReactToAttack = ConvertToAIReaction(sValue);
	else if (strEquals(sSetting, REACT_TO_THREAT_ATTRIB))
		m_iReactToThreat = ConvertToAIReaction(sValue);
	else if (strEquals(sSetting, THREAT_RANGE_ATTRIB))
		m_rThreatRange = Max(0, strToInt(sValue, DEFAULT_THREAT_RANGE)) * LIGHT_SECOND;
	else
		return NULL_STR;

	return GetValue(sSetting);
	}

void CAISettings::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	Write to a stream
//
//	DWORD		LO = m_iCombatStyle; HI = m_iFlockingStyle
//	DWORD		LO = m_iFireRateAdj; HI = m_iFireRangeAdj
//	DWORD		LO = m_iFireAccuracy; HI = m_iPerception
//	DWORD		LO = m_iReactToAttack; HI = m_iReactToThreat
//	Metric		m_rThreatRange
//	Metric		m_rMinCombatSeparation
//	DWORD		flags

	{
	DWORD dwSave;

	dwSave = MAKELONG((DWORD)m_iCombatStyle, (DWORD)m_iFlockingStyle);
	pStream->Write(dwSave);

	dwSave = MAKELONG(m_iFireRateAdj, m_iFireRangeAdj);
	pStream->Write(dwSave);

	dwSave = MAKELONG(m_iFireAccuracy, m_iPerception);
	pStream->Write(dwSave);

	dwSave = MAKELONG((DWORD)m_iReactToAttack, (DWORD)m_iReactToThreat);
	pStream->Write(dwSave);

	pStream->Write(m_rThreatRange);
	pStream->Write(m_rMinCombatSeparation);

	//	Flags

	dwSave = 0;
	dwSave |= (m_fNoShieldRetreat ?			0x00000001 : 0);
	dwSave |= (m_fNoDogfights ?				0x00000002 : 0);
	dwSave |= (m_fNonCombatant ?			0x00000004 : 0);
	dwSave |= (m_fNoFriendlyFire ?			0x00000008 : 0);
	dwSave |= (m_fAggressor ?				0x00000010 : 0);
	dwSave |= (m_fNoFriendlyFireCheck ?		0x00000020 : 0);
	dwSave |= (m_fNoOrderGiver ?			0x00000040 : 0);
	dwSave |= (m_fAscendOnGate ?			0x00000080 : 0);
	dwSave |= (m_fNoNavPaths ?				0x00000100 : 0);
	dwSave |= (m_fNoAttackOnThreat ?		0x00000200 : 0);
	dwSave |= (m_fNoTargetsOfOpportunity ?	0x00000400 : 0);
	dwSave |= (m_fIsPlayer ?				0x00000800 : 0);

	pStream->Write(dwSave);
	}

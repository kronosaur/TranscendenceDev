//	CAdventureDesc.cpp
//
//	CAdventureDesc class

#include "PreComp.h"

#define ARMOR_DAMAGE_ADJ_TAG					CONSTLIT("ArmorDamageAdj")
#define CONSTANTS_TAG							CONSTLIT("Constants")
#define ENCOUNTER_OVERRIDES_TAG					CONSTLIT("EncounterOverrides")
#define SHIELD_DAMAGE_ADJ_TAG					CONSTLIT("ShieldDamageAdj")

#define ADVENTURE_UNID_ATTRIB					CONSTLIT("adventureUNID")
#define BACKGROUND_ID_ATTRIB					CONSTLIT("backgroundID")
#define DEFAULT_CURRENCY_ATTRIB					CONSTLIT("defaultCurrency")
#define DESC_ATTRIB								CONSTLIT("desc")
#define INCLUDE_10_STARTING_CLASSES_ATTRIB		CONSTLIT("include10StartingShips")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define NAME_ATTRIB								CONSTLIT("name")
#define STARTING_MAP_ATTRIB						CONSTLIT("startingMap")
#define STARTING_POS_ATTRIB						CONSTLIT("startingPos")
#define STARTING_SHIP_CRITERIA_ATTRIB			CONSTLIT("startingShipCriteria")
#define STARTING_SYSTEM_ATTRIB					CONSTLIT("startingSystem")
#define WELCOME_MESSAGE_ATTRIB					CONSTLIT("welcomeMessage")

#define ON_GAME_START_EVENT						CONSTLIT("OnGameStart")
#define ON_GAME_END_EVENT						CONSTLIT("OnGameEnd")

#define FIELD_NAME								CONSTLIT("name")
#define FIELD_UNID								CONSTLIT("unid")

#define LANGUAGE_DESCRIPTION					CONSTLIT("description")

#define ERR_STARTING_SHIP_CRITERIA				CONSTLIT("Unable to parse startingShipCriteria")

CAdventureDesc::CAdventureDesc (void) :
		m_fIsCurrentAdventure(false),
		m_fInDefaultResource(false),
		m_fIncludeOldShipClasses(false)

//	CAdventureDesc constructor

	{
	m_sWelcomeMessage = CONSTLIT("Welcome to Transcendence!");
	}

bool CAdventureDesc::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Returns meta-data

	{
	if (strEquals(sField, FIELD_NAME))
		*retsValue = m_sName;
	else
		return CDesignType::FindDataField(sField, retsValue);

	return true;
	}

void CAdventureDesc::FireOnGameEnd (const CGameRecord &Game, const SBasicGameStats &BasicStats)

//	FireOnGameEnd
//
//	Fire OnGameEnd event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_GAME_END_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());

		//	Initialize variables

		Ctx.DefineInteger(CONSTLIT("aScore"), Game.GetScore());
		Ctx.DefineInteger(CONSTLIT("aResurrectCount"), Game.GetResurrectCount());
		Ctx.DefineInteger(CONSTLIT("aSystemsVisited"), BasicStats.iSystemsVisited);
		Ctx.DefineInteger(CONSTLIT("aEnemiesDestroyed"), BasicStats.iEnemiesDestroyed);
		Ctx.DefineInteger(CONSTLIT("aBestEnemiesDestroyed"), BasicStats.iBestEnemyDestroyedCount);
		if (BasicStats.dwBestEnemyDestroyed)
			Ctx.DefineInteger(CONSTLIT("aBestEnemyClass"), BasicStats.dwBestEnemyDestroyed);
		else
			Ctx.DefineNil(CONSTLIT("aBestEnemyClass"));

		Ctx.DefineString(CONSTLIT("aEndGameReason"), Game.GetEndGameReason());
		Ctx.DefineString(CONSTLIT("aEpitaph"), Game.GetEndGameEpitaph());
		Ctx.DefineString(CONSTLIT("aEpitaphOriginal"), Game.GetEndGameEpitaph());
		Ctx.DefineString(CONSTLIT("aTime"), Game.GetPlayTimeString());

		//	Invoke

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			kernelDebugLogPattern("OnGameEnd error: %s", pResult->GetStringValue());
		Ctx.Discard(pResult);
		}
	}

void CAdventureDesc::FireOnGameStart (void)

//	FireOnGameStart
//
//	Fire OnGameStart event

	{
	SEventHandlerDesc Event;

	if (FindEventHandler(ON_GAME_START_EVENT, &Event))
		{
		CCodeChainCtx Ctx(GetUniverse());

		//	Run code

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			kernelDebugLogPattern("OnGameStart error: %s", pResult->GetStringValue());
		Ctx.Discard(pResult);
		}
	}

const CEconomyType &CAdventureDesc::GetDefaultCurrency (void) const

//	GetDefaultCurrency
//
//	Returns the default currency for the adventure.

	{
	if (m_pDefaultCurrency)
		return *m_pDefaultCurrency;

	return GetUniverse().GetCreditCurrency();
	}

CString CAdventureDesc::GetDesc (void)

//	GetDesc
//
//	Returns a description for the adventure

	{
	CString sText;
	if (TranslateText(NULL, LANGUAGE_DESCRIPTION, NULL, &sText))
		return CTextBlock::LoadAsRichText(sText);
	else
		return CONSTLIT("{/rtf }");
	}

ALERROR CAdventureDesc::GetStartingShipClasses (TSortMap<CString, CShipClass *> *retClasses, CString *retsError)

//	GetStartingShipClasses
//
//	Returns a sorted list of ship classes for this adventure

	{
	int i;

	bool bShowDebugShips = GetUniverse().InDebugMode();

	//	Make a list

	retClasses->DeleteAll();
	for (i = 0; i < GetUniverse().GetShipClassCount(); i++)
		{
		CShipClass *pClass = GetUniverse().GetShipClass(i);
		if (pClass->IsShownAtNewGame()
				&& IsValidStartingClass(pClass)
				&& (!pClass->IsDebugOnly() || bShowDebugShips))
			{
			retClasses->Insert(pClass->GetPlayerSortString(), pClass);
			}
		}

	return NOERROR;
	}

bool CAdventureDesc::IsValidStartingClass (CShipClass *pClass)

//	IsValidStartingClass
//
//	Return TRUE if this ship class is a valid starting class for this adventure.

	{
	if (m_fIncludeOldShipClasses && pClass->GetAPIVersion() < 12)
		{
		if (pClass->HasLiteralAttribute(CONSTLIT("notInPartI")))
			return false;
		else
			return true;
		}

	return (pClass->IsIncludedInAllAdventures() || pClass->MatchesCriteria(m_StartingShips));
	}

ALERROR CAdventureDesc::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design elements

	{
	ALERROR error;

	if (error = m_pDefaultCurrency.Bind(Ctx))
		return error;

	return NOERROR;
	}

ALERROR CAdventureDesc::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Load from XML

	{
	ALERROR error;
	int i;

	//	If we are part of the default resource, then get the adventure UNID

	if (Ctx.pExtension == NULL)
		{
		m_dwExtensionUNID = pDesc->GetAttributeInteger(ADVENTURE_UNID_ATTRIB);
		m_fInDefaultResource = true;
		}

	//	Otherwise, we remember the extension that we were loaded from

	else
		{
		m_dwExtensionUNID = Ctx.pExtension->GetUNID();
		m_fInDefaultResource = false;
		}

	//	Load the name, etc

	m_sName = pDesc->GetAttribute(NAME_ATTRIB);
	if (error = ::LoadUNID(Ctx, pDesc->GetAttribute(BACKGROUND_ID_ATTRIB), &m_dwBackgroundUNID))
		return ComposeLoadError(Ctx, Ctx.sError);

	//	Starting ship criteria

	CString sCriteria;
	if (!pDesc->FindAttribute(STARTING_SHIP_CRITERIA_ATTRIB, &sCriteria))
		sCriteria = CONSTLIT("*");

	if (error = CDesignTypeCriteria::ParseCriteria(sCriteria, &m_StartingShips))
		return ComposeLoadError(Ctx, ERR_STARTING_SHIP_CRITERIA);

	m_fIncludeOldShipClasses = pDesc->GetAttributeBool(INCLUDE_10_STARTING_CLASSES_ATTRIB);

	//	Starting position

	if (error = ::LoadUNID(Ctx, pDesc->GetAttribute(STARTING_MAP_ATTRIB), &m_dwStartingMap))
		return error;

	m_sStartingNodeID = pDesc->GetAttribute(STARTING_SYSTEM_ATTRIB);
	m_sStartingPos = pDesc->GetAttribute(STARTING_POS_ATTRIB);

	//	Welcome message

	if (!pDesc->FindAttribute(WELCOME_MESSAGE_ATTRIB, &m_sWelcomeMessage))
		m_sWelcomeMessage = CONSTLIT("Welcome to Transcendence!");

	//	Default currency

	m_pDefaultCurrency.LoadUNID(pDesc->GetAttribute(DEFAULT_CURRENCY_ATTRIB));

	//	Init some flags

	m_fIsCurrentAdventure = false;

	//	If we don't have a name, then get it from the extension

	if (m_sName.IsBlank())
		{
		if (Ctx.pExtension)
			m_sName = Ctx.pExtension->GetName();
		}

	//	Otherwise, if the extension doesn't have a name, then we can set it

	else if (Ctx.pExtension && strFind(Ctx.pExtension->GetName(), CONSTLIT("Extension")) == 0)
		Ctx.pExtension->SetName(m_sName);

	//	Load constants

	CXMLElement *pConstants = pDesc->GetContentElementByTag(CONSTANTS_TAG);
	if (pConstants)
		{
		for (i = 0; i < pConstants->GetContentElementCount(); i++)
			{
			CXMLElement *pItem = pConstants->GetContentElement(i);

			if (strEquals(pItem->GetTag(), ARMOR_DAMAGE_ADJ_TAG))
				{
				if (!m_EngineOptions.InitArmorDamageAdjFromXML(Ctx, *pItem))
					return ComposeLoadError(Ctx, Ctx.sError);
				}
			else if (strEquals(pItem->GetTag(), SHIELD_DAMAGE_ADJ_TAG))
				{
				if (!m_EngineOptions.InitShieldDamageAdjFromXML(Ctx, *pItem))
					return ComposeLoadError(Ctx, Ctx.sError);
				}
			else
				return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Invalid constant definition element: %s."), pItem->GetTag()));
			}
		}

	//	Remember the overrides. We will use them later.

	CXMLElement *pEncounterOverrides = pDesc->GetContentElementByTag(ENCOUNTER_OVERRIDES_TAG);
	if (pEncounterOverrides)
		m_EncounterOverridesXML = *pEncounterOverrides;

	return NOERROR;
	}

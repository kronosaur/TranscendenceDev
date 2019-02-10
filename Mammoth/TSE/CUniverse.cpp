//	CUniverse.cpp
//
//	CUniverse class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

//#define DEBUG_FILE_CORRUPTION

#define CONTROLLER_AUTON					CONSTLIT("auton")
#define CONTROLLER_CREW						CONSTLIT("crew")
#define CONTROLLER_FERIAN					CONSTLIT("ferian")
#define CONTROLLER_FLEET					CONSTLIT("fleet")
#define CONTROLLER_FLEET_COMMAND			CONSTLIT("fleetcommand")
#define CONTROLLER_GAIAN_PROCESSOR			CONSTLIT("gaianprocessor")
#define CONTROLLER_GLADIATOR				CONSTLIT("gladiator")
#define CONTROLLER_ZOANTHROPE				CONSTLIT("zoanthrope")

#define PROPERTY_API_VERSION				CONSTLIT("apiVersion")
#define PROPERTY_MIN_API_VERSION			CONSTLIT("minAPIVersion")

struct SExtensionSaveDesc
	{
	DWORD dwUNID;
	DWORD dwRelease;
	};

#define STR_G_PLAYER						CONSTLIT("gPlayer")
#define STR_G_PLAYER_SHIP					CONSTLIT("gPlayerShip")

const DWORD UNIVERSE_VERSION_MARKER =					0xffffffff;

const DWORD UNID_FIRST_DEFAULT_EFFECT =					0x00000010;
const DWORD UNID_FIRST_DEFAULT_FIRE_EFFECT =			0x00000070;

CUniverse *g_pUniverse = NULL;
Metric g_KlicksPerPixel = KLICKS_PER_PIXEL;
Metric g_TimeScale = TIME_SCALE;
Metric g_SecondsPerUpdate =	g_TimeScale / g_TicksPerSecond;

CEffectCreator *g_DefaultFireEffect[damageCount];
CEffectCreator *g_DefaultHitEffect[damageCount];
bool g_bDefaultEffectsInit = false;

#ifdef DEBUG_PROGRAMSTATE
ProgramStates g_iProgramState = psUnknown;
CSpaceObject *g_pProgramObj = NULL;
CSystemEvent *g_pProgramEvent = NULL;
CString *g_sProgramError = NULL;
#endif

#ifdef DEBUG_PERFORMANCE
DWORD g_dwPerformanceTimer;
#endif

static CUniverse::IHost g_DefaultHost;

static char *FONT_TABLE[CUniverse::fontCount] = 
	{
	"Medium",				//	Map label: Tahoma 13
	"SmallBold",			//	Signs: Tahoma 11 bold
	"SmallBold",			//	SRS object name: Tahoma 11 bold
	"Header",				//	SRS object message
	"Small",				//	SRS object counter: 10 pixels
	};

CUniverse::CUniverse (void) : 
		m_bBasicInit(false),

		m_bRegistered(false),
		m_bResurrectMode(false),
		m_iTick(1),
		m_iPaintTick(1),
		m_pAdventure(NULL),
		m_pPOV(NULL),
		m_pPlayer(NULL),
		m_pPlayerShip(NULL),
		m_pCurrentSystem(NULL),
		m_dwNextID(1),
		m_AllMissions(true),

		m_pSoundMgr(NULL),

		m_pHost(&g_DefaultHost),
		m_pSavedGlobalSymbols(NULL),

		m_bDebugMode(false),
		m_bNoSound(false),
		m_iLogImageLoad(0)

//	CUniverse constructor

	{
	ASSERT(g_pUniverse == NULL);
	g_pUniverse = this;
	EuclidInit();

	//	We instantiate a CBeam object so that the code gets linked
	//	(Otherwise, the CBeam code would be optimized out, and need it
	//	because an older version might load one).

	CBeam *pDummy = new CBeam(*this);
	delete pDummy;
	}

CUniverse::~CUniverse (void)

//	CUniverse destructor

	{
    int i;

	SetPOV(NULL);
	m_pPlayerShip = NULL;

	if (m_pSavedGlobalSymbols)
		m_pSavedGlobalSymbols->Discard();

	//	Destroy all star systems. We do this here because we want to
	//	guarantee that we destroy all objects before we destruct
	//	codechain, et al

    for (i = 0; i < m_StarSystems.GetCount(); i++)
        delete m_StarSystems[i];
    m_StarSystems.DeleteAll();

	//	Free up various arrays whose cleanup requires m_CC

	m_Design.CleanUp();
	m_Extensions.CleanUp();
	m_Topology.DeleteAll();

	//	We own m_pPlayer;

	if (m_pPlayer)
		delete m_pPlayer;

	//	Done

	g_pUniverse = NULL;
	}

void CUniverse::AddEvent (CSystemEvent *pEvent)

//	AddEvent
//
//	Adds a new event

	{
	m_Events.AddEvent(pEvent);
	}

#ifdef OLD_SOUND
void CUniverse::AddSound (DWORD dwUNID, int iChannel)

//	AddSound
//
//	Adds a sound to the design

	{
	if (m_pSoundMgr == NULL)
		return;

	//	If this UNID is already in the list, then delete it

	int iOldChannel = FindSound(dwUNID);
	if (iOldChannel != -1)
		{
		m_pSoundMgr->Delete(iOldChannel);
		m_Sounds.RemoveEntry(dwUNID, NULL);
		}

	//	Add the new one

	m_Sounds.AddEntry((int)dwUNID, (CObject *)iChannel);
	}
#endif

ALERROR CUniverse::AddStarSystem (CTopologyNode *pTopology, CSystem *pSystem)

//	AddStarSystem
//
//	Adds a system to the universe

	{
	//	Add it to our list (must be new)

    bool bNew;
    m_StarSystems.SetAt(pSystem->GetID(), pSystem, &bNew);
    ASSERT(bNew);

	//	Set the system for the topoplogy

	if (pTopology)
		{
		ASSERT(pTopology->GetSystemID() == pSystem->GetID());
		pTopology->SetSystem(pSystem);
		}

	return NOERROR;
	}

ALERROR CUniverse::CreateEmptyStarSystem (CSystem **retpSystem)

//	CreateEmptyStarSystem
//
//	Creates an empty star system

	{
	ALERROR error;
	CSystem *pSystem;

	if (error = CSystem::CreateEmpty(*this, NULL, &pSystem))
		return error;

    //  Set a unique ID

	pSystem->SetID(CreateGlobalID());

	//	Add to our list

    bool bNew;
    m_StarSystems.SetAt(pSystem->GetID(), pSystem, &bNew);
    ASSERT(bNew);

	//	Done

	if (retpSystem)
		*retpSystem = pSystem;

	return NOERROR;
	}

ALERROR CUniverse::CreateMission (CMissionType *pType, CSpaceObject *pOwner, ICCItem *pCreateData, CMission **retpMission, CString *retsError)

//	CreateMission
//
//	Creates a new mission. Returns ERR_NOTFOUND if the mission could not be
//	created because conditions do not allow it.

	{
	ALERROR error;

	//	Create the mission object

	CMission *pMission;
	if (error = CMission::Create(*this, pType, pOwner, pCreateData, &pMission, retsError))
		return error;

	//	Add the mission 

	m_AllMissions.Insert(pMission);

	//	Done

	if (retpMission)
		*retpMission = pMission;

	return NOERROR;
	}

ALERROR CUniverse::CreateRandomItem (const CItemCriteria &Crit, 
									 const CString &sLevelFrequency,
									 CItem *retItem)

//	CreateRandomItem
//
//	Generates a random item

	{
	ALERROR error;

	//	Create the generator

	IItemGenerator *pTable;
	if (error = IItemGenerator::CreateRandomItemTable(Crit, sLevelFrequency, &pTable))
		return error;

	//	Pick an item

	CItemList ItemList;
	CItemListManipulator Items(ItemList);
	SItemAddCtx Ctx(Items);
	Ctx.iLevel = (GetCurrentSystem() ? GetCurrentSystem()->GetLevel() : 1);

	pTable->AddItems(Ctx);

	delete pTable;

	//	Done

	Items.ResetCursor();
	if (!Items.MoveCursorForward())
		return ERR_FAIL;

	*retItem = Items.GetItemAtCursor();
	return NOERROR;
	}

ALERROR CUniverse::CreateRandomMission (const TArray<CMissionType *> &Types, CSpaceObject *pOwner, ICCItem *pCreateData, CMission **retpMission, CString *retsError)

//	CreateRandomMission
//
//	Creates a random mission out of the given list of types. Returns 
//	ERR_NOTFOUND if the mission could not be created because conditions do not 
//	allow it.
//
//	We assume that Types has been shuffled.

	{
	ALERROR error;
	int i;

	//	Loop until we create a valid mission (we assume that Types has been 
	//	shuffled).

	for (i = 0; i < Types.GetCount(); i++)
		{
		//	Create a random mission. If successfull, return.

		if (error = CreateMission(Types[i], pOwner, pCreateData, retpMission, retsError))
			{
			if (error == ERR_NOTFOUND)
				continue;
			else
				return error;
			}

		return NOERROR;
		}

	//	If no mission was created, then done

	return ERR_NOTFOUND;
	}

IShipController *CUniverse::CreateShipController (const CString &sAI)

//	CreateShipController
//
//	Creates a ship controller given the AI name

	{
	//	First ask the host if it knows how to create the controller

	IShipController *pController;
	if (pController = m_pHost->CreateShipController(sAI))
		return pController;

	//	Otherwise, we create a controller that we know about.

	if (sAI.IsBlank())
		return new CStandardShipAI;
	else if (strEquals(sAI, CONTROLLER_AUTON))
		return new CAutonAI;
	else if (strEquals(sAI, CONTROLLER_CREW))
		return new CCrewAI;
	else if (strEquals(sAI, CONTROLLER_FERIAN))
		return new CFerianShipAI;
	else if (strEquals(sAI, CONTROLLER_FLEET))
		return new CFleetShipAI;
	else if (strEquals(sAI, CONTROLLER_FLEET_COMMAND))
		return new CFleetCommandAI;
	else if (strEquals(sAI, CONTROLLER_GAIAN_PROCESSOR))
		return new CGaianProcessorAI;
	else if (strEquals(sAI, CONTROLLER_GLADIATOR))
		return new CStandardShipAI;
	else if (strEquals(sAI, CONTROLLER_ZOANTHROPE))
		return new CZoanthropeAI;
	else
		return NULL;
	}

ALERROR CUniverse::CreateStarSystem (const CString &sNodeID, CSystem **retpSystem, CString *retsError, CSystemCreateStats *pStats)

//	CreateStarSystem
//
//	Creates a new system

	{
	ALERROR error;

	//	Figure out where to start

	CString sFirstNode = sNodeID;
	if (sFirstNode.IsBlank())
		{
		sFirstNode = m_Design.GetStartingNodeID();

		if (sFirstNode.IsBlank())
			{
			if (retsError)
				*retsError = CONSTLIT("No first star system defined in topology.");

			return ERR_FAIL;
			}
		}

	//	Find the topology node

	CTopologyNode *pNode = FindTopologyNode(sFirstNode);
	if (pNode == NULL)
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("Unable to find topology node: %s"), sFirstNode);

		return ERR_FAIL;
		}

	//	Create the first star system

	if (error = CreateStarSystem(pNode, retpSystem, retsError, pStats))
		return error;

	//	Done

	return NOERROR;
	}

ALERROR CUniverse::CreateStarSystem (CTopologyNode *pTopology, CSystem **retpSystem, CString *retsError, CSystemCreateStats *pStats)

//	CreateStarSystem
//
//	Creates a new system based on the description

	{
	ALERROR error;
	CSystem *pSystem;

	//	Get the description

	CSystemType *pSystemType = FindSystemType(pTopology->GetSystemTypeUNID());
	if (pSystemType == NULL)
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("Cannot create system %s: Undefined system type %x"), pTopology->GetID(), pTopology->GetSystemTypeUNID());
		return ERR_FAIL;
		}

	//	Create the system

	CString sError;

	SetLogImageLoad(false);
	error = CSystem::CreateFromXML(*this, pSystemType, pTopology, &pSystem, &sError, pStats);
	SetLogImageLoad(true);

	if (error)
		{
		if (retsError)
			*retsError = strPatternSubst(CONSTLIT("Cannot create system %s: %s"), pTopology->GetID(), sError);
		return error;
		}

    //  Set a unique ID

	pSystem->SetID(CreateGlobalID());

	//	Add to our list

    bool bNew;
    m_StarSystems.SetAt(pSystem->GetID(), pSystem, &bNew);
    ASSERT(bNew);

	//	Set the system for the topoplogy

	pTopology->SetSystem(pSystem);
	pTopology->SetSystemID(pSystem->GetID());

	//	Initialize trading economy based on stations in system

	pTopology->GetTradingEconomy().Refresh(pSystem);

	//	Done

	if (retpSystem)
		*retpSystem = pSystem;

	return NOERROR;
	}

void CUniverse::DebugOutput (char *pszLine, ...)

//	DebugOutput
//
//	Outputs debug line

	{
#ifdef DEBUG
	char *pArgs;
	char szBuffer[1024];
	int iLen;

	pArgs = (char *)&pszLine + sizeof(pszLine);
	iLen = wvsprintf(szBuffer, pszLine, pArgs);

	m_pHost->DebugOutput(CString(szBuffer));
#endif
	}

void CUniverse::DestroySystem (CSystem *pSystem)

//	DestroySystem
//
//	Destroys the given system

	{
	//	Change POV, if necessary

	if (m_pPOV && m_pPOV->GetSystem() == pSystem)
		SetPOV(NULL);

	//	Remove and destroy the system

    m_StarSystems.DeleteAt(pSystem->GetID());
    delete pSystem;
	}

CArmorClass *CUniverse::FindArmor (DWORD dwUNID)

//	FindArmor
//
//	Returns the armor class by item UNID

	{
	CItemType *pType = FindItemType(dwUNID);
	if (pType == NULL)
		return NULL;

	return pType->GetArmorClass();
	}

bool CUniverse::FindByUNID (CIDTable &Table, DWORD dwUNID, CObject **retpObj)

//	FindByUNID
//
//	Returns an object by UNID from the appropriate table

	{
	CObject *pObj;

	if (Table.Lookup((int)dwUNID, &pObj) == NOERROR)
		{
		if (retpObj)
			*retpObj = pObj;

		return true;
		}
	else
		return false;
	}

CObject *CUniverse::FindByUNID (CIDTable &Table, DWORD dwUNID)

//	FindByUNID
//
//	Returns an object by UNID from the appropriate table

	{
	CObject *pObj;

	if (Table.Lookup((int)dwUNID, &pObj) == NOERROR)
		return pObj;
	else
		return NULL;
	}

CEffectCreator *CUniverse::FindDefaultFireEffect (DamageTypes iDamage)

//	FindDefaultFireEffect
//
//	Returns the default hit effect for the given damage type

	{
	if (iDamage < damageLaser || iDamage >= damageCount)
		return NULL;

	InitDefaultEffects();
	return g_DefaultFireEffect[iDamage];
	}

CEffectCreator *CUniverse::FindDefaultHitEffect (DamageTypes iDamage)

//	FindDefaultHitEffect
//
//	Returns the default hit effect for the given damage type

	{
	if (iDamage < damageLaser || iDamage >= damageCount)
		return FindEffectType(g_HitEffectUNID);

	InitDefaultEffects();
	return g_DefaultHitEffect[iDamage];
	}

CDeviceClass *CUniverse::FindDeviceClass (DWORD dwUNID)

//	FindDeviceClass
//
//	Returns the device class by item UNID

	{
	CItemType *pType = FindItemType(dwUNID);
	if (pType == NULL)
		return NULL;

	return pType->GetDeviceClass();
	}

CSpaceObject *CUniverse::FindObject (DWORD dwID)

//	FindObject
//
//	Finds the object by ID

	{
	CSpaceObject *pObj;

	//	Look for an object in the current system

	if (m_pCurrentSystem
			&& (pObj = m_pCurrentSystem->FindObject(dwID)))
		return pObj;

	//	Look for a mission

	else if (pObj = m_AllMissions.GetMissionByID(dwID))
		return pObj;

	//	Look for an ascended object

	else if (pObj = m_AscendedObjects.FindByID(dwID))
		return pObj;

	//	Not found

	return NULL;
	}

CShipClass *CUniverse::FindShipClassByName (const CString &sName)

//	FindShipClassByName
//
//	Finds a ship class using partial name matches

	{
	int i;

	//	If this is a hex value, then assume an UNID

	CShipClass *pFound = NULL;
	if (strFind(sName, CONSTLIT("0x")) == 0
			&& (pFound = FindShipClass(strToInt(sName, 0))))
		return pFound;

	//	Otherwise look for a name match

	for (i = 0; i < GetShipClassCount(); i++)
		{
		CShipClass *pClass = GetShipClass(i);
		if (!pClass->IsVirtual()
				&& pClass->HasLiteralAttribute(CONSTLIT("genericClass"))
				&& strEquals(sName, pClass->GetNounPhrase(nounGeneric)))
			return pClass;
		}

	//	If not found, look for a partial match

	for (i = 0; i < GetShipClassCount(); i++)
		{
		CShipClass *pClass = GetShipClass(i);
		if (!pClass->IsVirtual()
				&& pClass->HasLiteralAttribute(CONSTLIT("genericClass"))
				&& strFind(pClass->GetNounPhrase(nounGeneric), sName) != -1)
			return pClass;
		}

	//	If it's still not found, look for a partial match of
	//	any non-generic class

	for (i = 0; i < GetShipClassCount(); i++)
		{
		CShipClass *pClass = GetShipClass(i);
		if (!pClass->IsVirtual()
				&& strFind(pClass->GetNounPhrase(), sName) != -1)
			return pClass;
		}

	//	Not found
	
	return NULL;
	}

CWeaponFireDesc *CUniverse::FindWeaponFireDesc (const CString &sName)

//	FindWeaponFireDesc
//
//	Returns a pointer to the descriptor by name

	{
	return CWeaponFireDesc::FindWeaponFireDescFromFullUNID(sName); 
	}

void CUniverse::FlushStarSystem (CTopologyNode *pTopology)

//	FlushStarSystem
//
//	Flushes the star system after it has been saved.

	{
	ASSERT(pTopology->GetSystemID() != 0xffffffff);

    int iPos;
    if (m_StarSystems.FindPos(pTopology->GetSystemID(), &iPos))
        {
        delete m_StarSystems[iPos];
        m_StarSystems.Delete(iPos);
        }
	}

void CUniverse::GarbageCollectLibraryBitmaps (void)

//	GarbageCollectLibraryBitmaps
//
//	Garbage-collects any unused bitmaps

	{
	ClearLibraryBitmapMarks();
	MarkLibraryBitmaps();
	SweepLibraryBitmaps();
	}

void CUniverse::GenerateGameStats (CGameStats &Stats)

//	GenerateGameStats
//
//	Generates the current game stats

	{
	DEBUG_TRY

	int i;

	//	Ask all design types to generate game stats

	m_Design.FireGetGlobalAchievements(Stats);

	//	Add all extensions

	for (i = 0; i < m_Design.GetExtensionCount(); i++)
		{
		CExtension *pExtension = m_Design.GetExtension(i);

		if (!pExtension->IsPrivate() && pExtension->GetType() != extLibrary)
			{
			CString sName = pExtension->GetName();

			if (pExtension->GetType() == extAdventure)
				Stats.Insert(CONSTLIT("Adventure"), sName);
			else
				Stats.Insert(sName, NULL_STR, CONSTLIT("extensions"));
			}
		}

	//	Add registration status

	if (IsRegistered())
		Stats.Insert(CONSTLIT("Game"), CONSTLIT("Registered"));
	else if (InDebugMode())
		Stats.Insert(CONSTLIT("Game"), CONSTLIT("Debug"));
	else
		Stats.Insert(CONSTLIT("Game"), CONSTLIT("Unregistered"));

	DEBUG_CATCH
	}

const CDamageAdjDesc *CUniverse::GetArmorDamageAdj (int iLevel) const

//	GetArmorDamageAdj
//
//	Returns the armor damage adj table

	{
	if (m_pAdventure)
		return m_pAdventure->GetArmorDamageAdj(iLevel);
	else
		return CAdventureDesc::GetDefaultArmorDamageAdj(iLevel);
	}

CMission *CUniverse::GetCurrentMission (void)

//	GetCurrentMission
//
//	Returns the current player mission (or NULL)

	{
	int i;
	DWORD dwLatestMission = 0;
	CMission *pCurrentMission = NULL;

	for (i = 0; i < m_AllMissions.GetCount(); i++)
		{
		CMission *pMission = m_AllMissions.GetMission(i);
		if (pMission->IsActive() && pMission->IsPlayerMission())
			{
			DWORD dwAcceptedOn = pMission->GetAcceptedOn();
			if (pCurrentMission == NULL || dwAcceptedOn > dwLatestMission)
				{
				pCurrentMission = pMission;
				dwLatestMission = dwAcceptedOn;
				}
			}
		}

	return pCurrentMission;
	}

void CUniverse::GetMissions (CSpaceObject *pSource, const CMission::SCriteria &Criteria, TArray<CMission *> *retList)

//	GetMissions
//
//	Returns a list of missions that match the given criteria

	{
	int i;

	retList->DeleteAll();
	for (i = 0; i < m_AllMissions.GetCount(); i++)
		{
		CMission *pMission = m_AllMissions.GetMission(i);
		if (pMission->MatchesCriteria(pSource, Criteria) 
				&& !pMission->IsDestroyed())
			retList->Insert(pMission);
		}
	}

const CDamageAdjDesc *CUniverse::GetShieldDamageAdj (int iLevel) const

//	GetShieldDamageAdj
//
//	Returns the shield damage table

	{
	if (m_pAdventure)
		return m_pAdventure->GetShieldDamageAdj(iLevel);
	else
		return CAdventureDesc::GetDefaultShieldDamageAdj(iLevel);
	}

void CUniverse::GetCurrentAdventureExtensions (TArray<DWORD> *retList)

//	GetCurrentAdventureExtensions
//
//	Returns the list of extensions enabled for the current adventure.
//	[Does not include any libraries nor any auto-included extension.]

	{
	int i;

	retList->DeleteAll();
	for (i = 0; i < m_Design.GetExtensionCount(); i++)
		{
		CExtension *pExtension = m_Design.GetExtension(i);

		//	Do not include the actual adventure

		if (pExtension->GetType() == extAdventure || pExtension->GetType() == extBase)
			NULL;

		//	Do not include extensions that are explicitly included by the
		//	adventure;

		else if (pExtension->GetType() == extLibrary)
			NULL;

		//	Do no include extensions that are automatically included.

		else if (pExtension->IsAutoInclude())
			NULL;

		//	Include other extensions

		else
			retList->Insert(pExtension->GetUNID());
		}
	}

CString CUniverse::GetExtensionData (EStorageScopes iScope, DWORD dwExtension, const CString &sAttrib)

//	GetExtensionData
//
//	Returns data storage for the given extension

	{
	switch (iScope)
		{
		case storeDevice:
			return m_DeviceStorage.GetData(dwExtension, sAttrib);

		default:
			return NULL_STR;
		}
	}

CTopologyNode *CUniverse::GetFirstTopologyNode (void)

//	GetFirstTopologyNode
//
//	Returns the starting node

	{
	ASSERT(m_StarSystems.GetCount() == 0);

	//	Get the default map

	CAdventureDesc *pAdventure = GetCurrentAdventureDesc();
	DWORD dwStartingMap = (pAdventure ? pAdventure->GetStartingMapUNID() : 0);

	//	Initialize the topology

	CString sError;
	InitTopology(dwStartingMap, &sError);

	//	Figure out the starting node

	CString sNodeID;
	if (pAdventure)
		sNodeID = pAdventure->GetStartingNodeID();

	if (sNodeID.IsBlank())
		{
		TSortMap<CString, CShipClass *> StartingShips;
		if (pAdventure->GetStartingShipClasses(&StartingShips, &sError) != NOERROR)
			return NULL;

		if (StartingShips.GetCount() > 0)
			{
			CShipClass *pClass = StartingShips[0];
			sNodeID = pClass->GetPlayerSettings()->GetStartingNode();
			}
		}

	if (sNodeID.IsBlank())
		sNodeID = m_Design.GetStartingNodeID();

	return FindTopologyNode(sNodeID);
	}

GenomeTypes CUniverse::GetPlayerGenome (void) const

//	GetPlayerGenome
//
//	Returns the player's genome

	{
	if (m_pPlayer == NULL)
		return genomeUnknown;

	return m_pPlayer->GetGenome();
	}

CString CUniverse::GetPlayerName (void) const

//	GetPlayerName
//
//	Returns the player's name

	{
	if (m_pPlayer == NULL)
		return NULL_STR;

	return m_pPlayer->GetName();
	}

CSovereign *CUniverse::GetPlayerSovereign (void) const

//	GetPlayerSovereign
//
//	Returns the player's sovereign

	{
	if (m_pPlayerShip)
		return m_pPlayerShip->GetSovereign();
	else if (m_pPlayer)
		return m_pPlayer->GetSovereign();
	else
		return NULL;
	}

ICCItemPtr CUniverse::GetProperty (CCodeChainCtx &Ctx, const CString &sProperty)

//	GetProperty
//
//	Returns a property

	{
	CCodeChain &CC = GetCC();

	if (strEquals(sProperty, PROPERTY_API_VERSION))
		return ICCItemPtr(API_VERSION);

	else if (strEquals(sProperty, PROPERTY_MIN_API_VERSION))
		return ICCItemPtr(m_Design.GetAPIVersion());

	else
		return ICCItemPtr(ICCItem::Nil);
	}

void CUniverse::GetRandomLevelEncounter (int iLevel, 
										 CDesignType **retpType,
										 IShipGenerator **retpTable, 
										 CSovereign **retpBaseSovereign)

//	GetRandomLevelEncounter
//
//	Returns a random encounter appropriate for the given level

	{
	int i;

	iLevel--;	//	m_LevelEncounterTable is 0-based
	if (iLevel < 0 || iLevel >= m_LevelEncounterTables.GetCount())
		{
		*retpType = NULL;
		*retpTable = NULL;
		*retpBaseSovereign = NULL;
		return;
		}

	//	Get the level table

	const TArray<SLevelEncounter> &Table = m_LevelEncounterTables[iLevel];

	//	Compute the totals for the table

	int iTotal = 0;
	for (i = 0; i < Table.GetCount(); i++)
		iTotal += Table[i].iWeight;

	if (iTotal == 0)
		{
		*retpType = NULL;
		*retpTable = NULL;
		*retpBaseSovereign = NULL;
		return;
		}

	//	Pick a random entry

	int iRoll = mathRandom(0, iTotal - 1);
	int iPos = 0;

	//	Get the position

	while (Table[iPos].iWeight <= iRoll)
		iRoll -= Table[iPos++].iWeight;

	//	Done

	*retpType = Table[iPos].pType;
	*retpTable = Table[iPos].pTable;
	*retpBaseSovereign = Table[iPos].pBaseSovereign;
	}

ALERROR CUniverse::Init (SInitDesc &Ctx, CString *retsError)

//	Init
//
//	Initialize the universe according to the given settings. After this the
//	universe is fully loaded and bound.
//
//	This may be called (almost) at any time to change initialization parameters
//	(e.g., to change adventures or simply to reload the extensions directory.)

	{
	try
		{
		ALERROR error;
		int i;

		//	Boot up

		bool bFirstInit = !m_bBasicInit;
		if (!m_bBasicInit)
			{
			//	Set the host (OK if Ctx.pHost is NULL)

			SetHost(Ctx.pHost);

			//	Initialize CodeChain

			if (error = InitCodeChain(Ctx.CCPrimitives))
				{
				*retsError = CONSTLIT("Unable to initialize CodeChain.");
				return error;
				}

			//	Initialize fonts

			if (error = InitFonts())
				{
				*retsError = CONSTLIT("Unable to initialize fonts.");
				return error;
				}

			//	Load local device storage

			if (error = InitDeviceStorage(retsError))
				return error;

			//	Set folders for Collection and extensions

			if (!Ctx.sCollectionFolder.IsBlank())
				m_Extensions.SetCollectionFolder(Ctx.sCollectionFolder);

			for (i = 0; i < Ctx.ExtensionFolders.GetCount(); i++)
				m_Extensions.AddExtensionFolder(Ctx.ExtensionFolders[i]);

			m_bBasicInit = true;
			}

		//	Otherwise, reinitialize some stuff

		else
			{
			//	If this is not the first time we've initialized, then clean up any 
			//	global definitions that our extensions might have done (in case they
			//	overrode any primitives).

			ICCItem *pNewGlobals = m_pSavedGlobalSymbols->Clone(&m_CC);
			m_CC.SetGlobals(pNewGlobals);
			pNewGlobals->Discard();
			}

		//	Load texture library

		if (!Ctx.bNoResources)
			m_FractalTextureLibrary.Init();

		//	Initialize some stuff

		m_bDebugMode = Ctx.bDebugMode;

		//	If necessary, figure out where the main files are

		CString sMainFilespec;
		if (Ctx.sFilespec.IsBlank())
			{
			//	If we're always using the TDB, then just load that.

			if (Ctx.bForceTDB)
				sMainFilespec = CONSTLIT("Transcendence.tdb");

			//	Check the source subdirector first.

			else if (pathExists("..\\TransCore\\Transcendence.xml"))
				sMainFilespec = CONSTLIT("..\\TransCore\\Transcendence.xml");

			//	If we don't have it, then check the current directory for
			//	backwards compatibility.

			else if (pathExists("Transcendence.xml"))
				sMainFilespec = CONSTLIT("Transcendence.xml");

			//	If nothing is found, then just load the TDB file.

			else
				sMainFilespec = CONSTLIT("Transcendence.tdb");
			}
		else
			{
			//	Unless we're forcing TDB usage, check sources

			if (!Ctx.sSourceFilespec.IsBlank()
					&& !Ctx.bForceTDB
					&& pathExists(Ctx.sSourceFilespec))
				sMainFilespec = Ctx.sSourceFilespec;
			else
				sMainFilespec = Ctx.sFilespec;
			}

		//	We only load adventure desc (no need to load the whole thing)

		DWORD dwFlags = CExtensionCollection::FLAG_DESC_ONLY;

		//	Debug mode

		if (m_bDebugMode)
			dwFlags |= CExtensionCollection::FLAG_DEBUG_MODE;

		//	Only include diagnostics if we explicitly asked for them.

		if (Ctx.bDiagnostics)
			dwFlags |= CExtensionCollection::FLAG_DIAGNOSTICS;

		//	If requested we don't load resources

		if (Ctx.bNoResources)
			dwFlags |= CExtensionCollection::FLAG_NO_RESOURCES;

        //  Don't load collection if we don't want it.
        //  (But we only honor it in debug mode.)

        if (Ctx.bNoCollectionLoad && m_bDebugMode)
            dwFlags |= CExtensionCollection::FLAG_NO_COLLECTION;

		if (Ctx.bNoCollectionCheck)
			dwFlags |= CExtensionCollection::FLAG_NO_COLLECTION_CHECK;

		//	Load everything

		if (error = m_Extensions.Load(sMainFilespec, Ctx.DisabledExtensions, dwFlags, retsError))
			return error;

		//	Figure out the adventure to bind to.

		if (Ctx.pAdventure == NULL && Ctx.dwAdventure)
			{
			//	Look for the adventure by UNID.

			DWORD dwFindFlags = dwFlags | CExtensionCollection::FLAG_ADVENTURE_ONLY;

			if (!m_Extensions.FindBestExtension(Ctx.dwAdventure, 0, dwFindFlags, &Ctx.pAdventure)
					|| m_Extensions.IsExtensionDisabledManually(Ctx.dwAdventure))
				{
				//	Try to recover, if necessary

				if (Ctx.bRecoverAdventureError
						&& Ctx.dwAdventure != DEFAULT_ADVENTURE_EXTENSION_UNID
						&& m_Extensions.FindBestExtension(DEFAULT_ADVENTURE_EXTENSION_UNID, 0, dwFindFlags, &Ctx.pAdventure))

					{
					//	Continue

					Ctx.dwAdventure = DEFAULT_ADVENTURE_EXTENSION_UNID;
					}

				//	For backwards compatibility, sometimes we do not have an adventure. This
				//	can happen when we're using TransData on an old TDB.

				else if (m_Extensions.GetBase()->GetAPIVersion() < 26
						&& Ctx.dwAdventure == DEFAULT_ADVENTURE_EXTENSION_UNID)
					{
					//	Continue with NULL pAdventure
					}

				//	Otherwise, we fail

				else
					{
					*retsError = strPatternSubst(CONSTLIT("Unable to find adventure: %08x."), Ctx.dwAdventure);
					return ERR_FAIL;
					}
				}
			}

		//	If necessary figure out which extensions to add

		if (Ctx.bDefaultExtensions)
			{
			ASSERT(!Ctx.bInLoadGame);

			//	If we want default extensions, then add all possible extensions,
			//	including auto extensions.

			if (error = m_Extensions.ComputeAvailableExtensions(Ctx.pAdventure, 
					dwFlags | CExtensionCollection::FLAG_INCLUDE_AUTO,
					TArray<DWORD>(),
					&Ctx.Extensions, 
					retsError))
				return error;
			}
		else if (Ctx.ExtensionUNIDs.GetCount() > Ctx.Extensions.GetCount())
			{
			//	If our caller passed in an UNID list, then resolve to actual
			//	extension objects.
			//
			//	NOTE: If we're creating a new game we take this opportunity to
			//	add auto extensions.

			if (error = m_Extensions.ComputeAvailableExtensions(Ctx.pAdventure,
					dwFlags | (Ctx.bInLoadGame ? 0 : CExtensionCollection::FLAG_INCLUDE_AUTO),
					Ctx.ExtensionUNIDs,
					&Ctx.Extensions,
					retsError))
				return error;
			}
		else if (!Ctx.bInLoadGame && !m_bDebugMode)
			{
			//	If the caller passed in a list of extension objects (or if we 
			//	didn't add any extensions) then include auto extensions.
			//	(But only if we're in a new game).

			if (error = m_Extensions.ComputeAvailableExtensions(Ctx.pAdventure,
					dwFlags | CExtensionCollection::FLAG_AUTO_ONLY | CExtensionCollection::FLAG_ACCUMULATE,
					TArray<DWORD>(),
					&Ctx.Extensions,
					retsError))
				return error;
			}

		//	If the save file we're loading used the compatibility library, force
		//	include regardess of whether we think we need it.

		if (Ctx.bForceCompatibilityLibrary)
			dwFlags |= CExtensionCollection::FLAG_FORCE_COMPATIBILITY_LIBRARY;

		//	Get the bind order

		TArray<CExtension *> BindOrder;
		if (error = m_Extensions.ComputeBindOrder(Ctx.pAdventure,
				Ctx.Extensions,
				dwFlags, 
				&BindOrder,
				retsError))
			return error;

		//	Reinitialize. This clears out previous game state, but only if we
		//	are creating a new game.

		if (!Ctx.bInLoadGame)
			{
			if (error = Reinit())
				{
				*retsError = CONSTLIT("Unable to reinit.");
				return error;
				}
			}

		//	Set the current adventure (we need to do this before BindDesign, since
		//	we need the current adventure to get the shield and armor damage adj
		//	tables.

		if (Ctx.pAdventure)
			SetCurrentAdventureDesc(Ctx.pAdventure->GetAdventureDesc());

		//	Figure out the minimum API version for all extensions being used.

		DWORD dwAPIVersion = Ctx.dwMinAPIVersion;
		for (i = 0; i < BindOrder.GetCount(); i++)
			{
			if (BindOrder[i]->GetAPIVersion() < dwAPIVersion)
				dwAPIVersion = BindOrder[i]->GetAPIVersion();
			}

		//	Bind
		//
		//	We don't need to log image load

		SetLogImageLoad(false);
		error = m_Design.BindDesign(BindOrder, Ctx.TypesUsed, dwAPIVersion, !Ctx.bInLoadGame, Ctx.bNoResources, retsError);
		SetLogImageLoad(true);

		if (error)
			return error;

		//	Now that we've bound we can delete any previous extensions

		m_Extensions.FreeDeleted();

		return NOERROR;
		} 
	catch (...)
		{
		kernelDebugLogPattern("Crash in CUniverse::Init");
		m_Extensions.DebugDump();
		*retsError = CONSTLIT("Unable to initialize universe.");
		return ERR_FAIL;
		}
	}

ALERROR CUniverse::InitAdventure (IPlayerController *pPlayer, CString *retsError)

//	InitAdventure
//
//	Initializes the adventure and stores the player controller. Note that we 
//	take ownership of pPlayer.

	{
	ASSERT(pPlayer);
	ASSERT(m_pPlayer == NULL);

	//	Define globals for OnGameStart (only gPlayer is defined)
	//	We need this because script may want to reference gPlayer
	//	to get genome information.
	//
	//	The rest of the variables will be set in m_Universe.SetPlayerShip

	SetPlayer(pPlayer);

	//	Invoke Adventure OnGameStart
	//	NOTE: The proper adventure is set by a call to InitAdventure,
	//	when the adventure is chosen.

	CAdventureDesc *pAdventure = GetCurrentAdventureDesc();
	if (pAdventure == NULL)
		{
		//	We don't want to free pPlayer, so we need to clear it out before we
		//	reset.

		m_pPlayer = NULL;
		SetPlayer(NULL);

		*retsError = CONSTLIT("Must have an adventure.");
		return ERR_FAIL;
		}

	SetLogImageLoad(false);
	pAdventure->FireOnGameStart();
	SetLogImageLoad(true);

	//	Done

	return NOERROR;
	}

void CUniverse::InitDefaultEffects (void)

//	InitDefaultHitEffects
//
//	Initializes the default hit effects array

	{
	int i;

	if (!g_bDefaultEffectsInit)
		{
		for (i = 0; i < damageCount; i++)
			{
			//	Find a default hit effect. Default to some standard.

			g_DefaultHitEffect[i] = FindEffectType(UNID_FIRST_DEFAULT_EFFECT + i);
			if (g_DefaultHitEffect[i] == NULL)
				g_DefaultHitEffect[i] = FindEffectType(g_HitEffectUNID);

			//	Find a default fire effect. OK if NULL, we have no effect in that
			//	case.

			g_DefaultFireEffect[i] = FindEffectType(UNID_FIRST_DEFAULT_FIRE_EFFECT + i);
			}

		g_bDefaultEffectsInit = true;
		}
	}

ALERROR CUniverse::InitFonts (void)

//	InitFonts
//
//	Initializes fonts

	{
	int i;

	for (i = 0; i < fontCount; i++)
		{
		if (!m_pHost->FindFont(CString(FONT_TABLE[i]), &m_FontTable[i]))
			{
			//	If we could not find the font then we need to create a default version

			if (m_FontTable[i] == NULL)
				{
				HFONT hFont = ::CreateFont(-13,
						0,
						0,
						0,
						FW_NORMAL,
						false,
						false,
						false,
						ANSI_CHARSET,
						OUT_TT_ONLY_PRECIS,
						CLIP_DEFAULT_PRECIS,
						ANTIALIASED_QUALITY,
						FF_SWISS,
						"Tahoma");

				m_DefaultFonts[i].CreateFromFont(hFont);
				m_FontTable[i] = &m_DefaultFonts[i];
				::DeleteObject(hFont);
				}
			}
		}

	//	Done
	
	return NOERROR;
	}

ALERROR CUniverse::InitGame (DWORD dwStartingMap, CString *retsError)

//	InitGame
//
//	Initializes a game instance, including the topology. We assume that the
//	universe is loaded and bound.

	{
	ALERROR error;
	CAdventureDesc *pAdventure = GetCurrentAdventureDesc();

	//	If starting map is 0, see if we can get it from the adventure

	if (dwStartingMap == 0 && pAdventure)
		dwStartingMap = pAdventure->GetStartingMapUNID();

	//	Initialize the topology. This is the point at which the topology is created

	if (error = InitTopology(dwStartingMap, retsError))
		return error;

	//	Tell all types that the topology has been initialized (we need to do this
	//	before we call InitLevelEncounterTable).

	m_Design.NotifyTopologyInit();

	//	Tell the adventure to initialize its encounter tables, which might
	//	override the encounter desc of specific station types.

	if (pAdventure)
		{
		if (!pAdventure->InitEncounterOverrides(retsError))
			return ERR_FAIL;
		}

	//	Init encounter tables (this must be done AFTER InitTopoloy because it
	//	some station encounters specify a topology node).

	InitLevelEncounterTables();

	return NOERROR;
	}

ALERROR CUniverse::InitRequiredEncounters (CString *retsError)

//	InitRequiredEncounters
//
//	Called from inside InitTopology. If there are any encounter types that need
//	to be created then we distribute them across the topology here.

	{
	int i, j;

	//	Loop over all station types and see if we need to distribute them.

	for (i = 0; i < GetStationTypeCount(); i++)
		{
		CStationType *pType = GetStationType(i);
		int iCount = pType->GetNumberAppearing();
		if (iCount <= 0)
			continue;

		//	Make a list of all the nodes in which this station can appear

		TProbabilityTable<CTopologyNode *> Table;
		for (j = 0; j < m_Topology.GetTopologyNodeCount(); j++)
			{
			CTopologyNode *pNode = m_Topology.GetTopologyNode(j);
			CSystemType *pSystemType = FindSystemType(pNode->GetSystemTypeUNID());
			if (pSystemType == NULL)
				continue;

			int iFreq = pType->GetFrequencyForNode(pNode);
			if (iFreq > 0
					&& pSystemType->HasExtraEncounters())
				Table.Insert(pNode, iFreq);
			}

		//	If no nodes, then report a warning

		if (Table.GetCount() == 0)
			{
			kernelDebugLogPattern("Warning: Not enough appropriate systems to create %d %s [%08x].", iCount, pType->GetNounPhrase(iCount > 1 ? nounPlural : 0), pType->GetUNID());
			continue;
			}

		//	If this station is unique per system then we need at least the 
		//	required number of systems. If not, we adjust the count.

		if (pType->IsUniqueInSystem() && iCount > Table.GetCount())
			{
			iCount = Table.GetCount();
			kernelDebugLogPattern("Warning: Decreasing number appearing of %s [%08x] to %d due to lack of appropriate systems.", pType->GetNounPhrase(nounPlural), pType->GetUNID(), iCount);
			}

		//	Loop over the required number and place them in appropriate nodes.

		for (j = 0; j < iCount; j++)
			{
			CTopologyNode *pNode = Table.GetAt(Table.RollPos());

			//	If the station is unique in the system and we've already got a 
			//	station in this system, then try again.

			if (pType->IsUniqueInSystem() 
					&& pType->GetEncounterMinimum(pNode) > 0)
				{
				j--;
				continue;
				}

			//	Add it to this node

			pType->IncEncounterMinimum(pNode, 1);
			}
		}

	//	Done

	return NOERROR;
	}

void CUniverse::NotifyOnObjDestroyed (SDestroyCtx &Ctx)

//	NotifyOnObjDestroyed
//
//	Called when a ship or station has been destroyed. [This does NOT get called
//	for effects or missiles being destroyed.]
//
//	NOTE: This includes cases where a station becomes abandoned. If the station
//	gets destroyed after that, we this gets called a second time.

	{
	int i;

	//	Call all subscribers
	//
	//	For performance, we don't deal with making a copy of the list before 
	//	calling. Subscribers should NEVER delete themselves from inside a call.

	for (i = 0; i < m_Subscribers.GetCount(); i++)
		m_Subscribers[i]->OnObjDestroyed(Ctx);

	//	Fire global event

	m_Design.FireOnGlobalObjDestroyed(Ctx);
	}

void CUniverse::NotifyOnPlayerEnteredGate (CTopologyNode *pDestNode, const CString &sDestEntryPoint, CSpaceObject *pStargate)

//	NotifyOnPlayerEnteredGate
//
//	Called when the player ship enteres a stargate.

	{
	//	Clear out these globals so that events don't try to send us
	//	orders (Otherwise, an event could set a target for the player. If the
	//	target is destroyed while we are out of the system, we will
	//	never get an OnObjDestroyed message).
	//	Note: We need gPlayer for OnGameEnd event

	GetCC().DefineGlobal(STR_G_PLAYER_SHIP, GetCC().CreateNil());

	//	Mark source and destination stargates as charted.

	CTopologyNode *pSourceNode = GetCurrentTopologyNode();
	if (pSourceNode && pStargate)
		pSourceNode->SetStargateCharted(pStargate->GetStargateID());

	if (pDestNode)
		pDestNode->SetStargateCharted(sDestEntryPoint);
	}

bool CUniverse::IsGlobalResurrectPending (CDesignType **retpType)

//	IsGlobalResurrectPending
//
//	Asks each design type if they want to resurrect the player

	{
	int i;
	TArray<int> Types;
	TArray<int> Chances;
	int iTotalChance = 0;

	//	Ask all design types if they are prepared to resurrect
	//	the player (they return a number from 0 to 100).

	for (i = 0; i < m_Design.GetCount(); i++)
		{
		int iChance = m_Design.GetEntry(i)->FireGetGlobalResurrectPotential();
		if (iChance > 0)
			{
			Types.Insert(i);
			Chances.Insert(iTotalChance + iChance);

			iTotalChance += iChance;
			}
		}

	//	If none want to resurrect, then we're done

	if (iTotalChance == 0)
		return false;

	//	Otherwise, pick a random design type in proportion to the
	//	value that they returned.

	int iRoll = mathRandom(0, iTotalChance-1);
	for (i = 0; i < Types.GetCount(); i++)
		if (iRoll < Chances[i])
			{
			*retpType = m_Design.GetEntry(Types[i]);
			return true;
			}

	ASSERT(false);
	return false;
	}

bool CUniverse::IsStatsPostingEnabled (void)

//	IsStatsPostingEnabled
//
//	Returns TRUE if we should post stats for the current adventure.
//	FALSE otherwise.

	{
	return m_bRegistered;
	}

ALERROR CUniverse::LoadFromStream (IReadStream *pStream, DWORD *retdwSystemID, DWORD *retdwPlayerID, CString *retsError)

//	LoadFromStream
//
//	Loads the universe from a stream
//
//	DWORD		UNIVERSE_VERSION_MARKER
//	DWORD		universe version
//	DWORD		system version
//	DWORD		m_iTick
//
//	DWORD		flags
//	DWORD		m_dwNextID
//
//	CGameTimeKeeper
//
//	DWORD		No of enabled extensions
//	SExtensionSaveDesc for each
//
//	SExtensionSaveDesc for adventure
//
//	CDynamicDesignTable
//
//	DWORD		No of types used
//	DWORD		type: UNID
//
//	DWORD		ID of POV system (0xffffffff if none)
//	DWORD		index of POV (0xffffffff if none)
//	CTimeSpan	time that we've spent playing the game
//
//	IPlayerController m_pPlayer
//	CMissionList	m_AllMissions
//	CAscendedObjectList	m_AscendedObjects;
//	CSystemEventList	m_Events
//
//	DWORD		No of topology nodes
//	CString		node: Node ID
//	DWORD		node: ID of system instance (0xffffffff if not yet created)
//
//	DWORD		No of types
//	DWORD		type: UNID
//	Type-specific data
//
//	CObjectTracker	m_Objects
//	CObjectStats	m_ObjStats
//
//	NOTE: The debug mode flag is stored in the game file header. Callers must 
//	set the universe to debug mode from the game file header flag before calling
//	this function.
	
	{
	int i;
	DWORD dwLoad;
	DWORD dwCount;

	//	Create load structure

	SUniverseLoadCtx Ctx;
	Ctx.pStream = pStream;

	//	Load the version. Version 0 we did not even have a place to store the
	//	version, so we see if the first entry is -1. If it is not, then we
	//	are at version 0.

	pStream->Read((char *)&dwLoad, sizeof(DWORD));
	if (dwLoad == UNIVERSE_VERSION_MARKER)
		{
		pStream->Read((char *)&Ctx.dwVersion, sizeof(DWORD));

		//	Some types (e.g., CSystemMap) require saving IPainterEffects. In those
		//	cases we need to have the system version, so we started saving the system
		//	version here too. Since we never needed this until system version 57, we
		//	can be sure that we are at least version 57.

		if (Ctx.dwVersion >= 9)
			pStream->Read((char *)&Ctx.dwSystemVersion, sizeof(DWORD));
		else
			Ctx.dwSystemVersion = 57;

		//	Read the current tick

		pStream->Read((char *)&m_iTick, sizeof(DWORD));
		}
	else
		{
		Ctx.dwVersion = 0;
		Ctx.dwSystemVersion = 57;
		m_iTick = (int)dwLoad;
		}

	//	Version 14 and 15 are corrupt.

	if (Ctx.dwVersion == 14 || Ctx.dwVersion == 15)
		{
		*retsError = CONSTLIT("The save file cannot be loaded due to a subtle corruption bug in 1.08c and 1.08d. I apologize for the problem.");
		return ERR_FAIL;
		}

	//	Flags

	if (Ctx.dwVersion >= 13)
		pStream->Read((char *)&dwLoad, sizeof(DWORD));
	else
		dwLoad = 0;

	m_bRegistered =					((dwLoad & 0x00000001) ? true : false);
	bool bUseCompatibilityLibrary = ((dwLoad & 0x00000002) ? true : false);

	//	Load basic data

	pStream->Read((char *)&m_dwNextID, sizeof(DWORD));

	//	CGameTimeKeeper m_Time

	if (Ctx.dwVersion >= 7)
		m_Time.ReadFromStream(pStream);

	//	Prepare a universe initialization context
	//	NOTE: Caller has set debug mode based on game file header flag.

	CString sError;
	CUniverse::SInitDesc InitCtx;
	InitCtx.bDebugMode = InDebugMode();
	InitCtx.bInLoadGame = true;

	//	Load list of extensions used in this game

	TArray<SExtensionSaveDesc> ExtensionList;
	if (Ctx.dwVersion >= 14)
		{
		pStream->Read((char *)&dwLoad, sizeof(DWORD));
		ExtensionList.InsertEmpty(dwLoad);
		if (dwLoad > 0)
			pStream->Read((char *)&ExtensionList[0], dwLoad * sizeof(SExtensionSaveDesc));
		}
	else if (Ctx.dwVersion >= 8)
		{
		TArray<DWORD> CompatibleExtensionList;

		pStream->Read((char *)&dwLoad, sizeof(DWORD));
		CompatibleExtensionList.InsertEmpty(dwLoad);
		pStream->Read((char *)&CompatibleExtensionList[0], dwLoad * sizeof(DWORD));

		ExtensionList.InsertEmpty(CompatibleExtensionList.GetCount());
		for (i = 0; i < CompatibleExtensionList.GetCount(); i++)
			{
			ExtensionList[i].dwUNID = CompatibleExtensionList[i];
			ExtensionList[i].dwRelease = 0;
			}
		}

	//	Previous versions did not save the compatibility library, so we need to
	//	calculate that here.

	if (Ctx.dwVersion < 28)
		{
		for (i = 0; i < ExtensionList.GetCount(); i++)
			{
			//	The old versions of the soundtrack and HD extensions used the
			//	compatibility library, but the new ones do not.

			if (ExtensionList[i].dwUNID == UNID_SOUNDTRACK_EXTENSION
					|| ExtensionList[i].dwUNID == UNID_HD_EXTENSION)
				{
				bUseCompatibilityLibrary = true;
				break;
				}
			}
		}

	//	If the save file we're loading needs the compatibility library, make sure
	//	we load it.

	InitCtx.bForceCompatibilityLibrary = bUseCompatibilityLibrary;

	//	Get the actual extensions

	for (i = 0; i < ExtensionList.GetCount(); i++)
		{
		CExtension *pExtension;

		if (!m_Extensions.FindBestExtension(ExtensionList[i].dwUNID,
				ExtensionList[i].dwRelease,
				(InDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0),
				&pExtension))
			{
			*retsError = strPatternSubst(CONSTLIT("Unable to find extension: %08x"), ExtensionList[i]);
			return ERR_FAIL;
			}

		//	Make sure this extension is enabled

		if (pExtension->IsDisabled())
			{
			*retsError = strPatternSubst(CONSTLIT("Unable to load %s (%08x): %s"), pExtension->GetName(), pExtension->GetUNID(), pExtension->GetDisabledReason());
			return ERR_FAIL;
			}
		else if (m_Extensions.IsExtensionDisabledManually(pExtension->GetUNID()))
			{
			*retsError = strPatternSubst(CONSTLIT("Required extension %s (%08x) is disabled. Go to Mod Collection and enable it."), pExtension->GetName(), pExtension->GetUNID());
			return ERR_FAIL;
			}

		InitCtx.Extensions.Insert(pExtension);
		}

	//	Load adventure extension

	if (Ctx.dwVersion >= 14)
		{
		SExtensionSaveDesc Desc;
		pStream->Read((char *)&Desc, sizeof(SExtensionSaveDesc));

		if (!m_Extensions.FindBestExtension(Desc.dwUNID,
				Desc.dwRelease,
				CExtensionCollection::FLAG_ADVENTURE_ONLY | (InDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0),
				&InitCtx.pAdventure))
			{
			*retsError = strPatternSubst(CONSTLIT("Unable to find adventure: %08x"), Desc.dwUNID);
			return ERR_FAIL;
			}
		else if (InitCtx.pAdventure->IsDisabled())
			{
			*retsError = strPatternSubst(CONSTLIT("Unable to load %s (%08x): %s"), InitCtx.pAdventure->GetName(), InitCtx.pAdventure->GetUNID(), InitCtx.pAdventure->GetDisabledReason());
			return ERR_FAIL;
			}
		else if (m_Extensions.IsExtensionDisabledManually(Desc.dwUNID))
			{
			*retsError = strPatternSubst(CONSTLIT("%s (%08x) is disabled. Go to Mod Collection and enable it."), InitCtx.pAdventure->GetName(), InitCtx.pAdventure->GetUNID());
			return ERR_FAIL;
			}
		}
	else if (Ctx.dwVersion >= 2)
		{
		pStream->Read((char *)&dwLoad, sizeof(DWORD));

		if (!m_Extensions.FindAdventureFromDesc(dwLoad, 
				(InDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0),
				&InitCtx.pAdventure))
			{
			*retsError = strPatternSubst(CONSTLIT("Unable to find adventure: %08x"), dwLoad);
			return ERR_FAIL;
			}
		}
	else
		{
		if (!m_Extensions.FindBestExtension(DEFAULT_ADVENTURE_EXTENSION_UNID, 
				0,
				(InDebugMode() ? CExtensionCollection::FLAG_DEBUG_MODE : 0),
				&InitCtx.pAdventure))
			{
			*retsError = strPatternSubst(CONSTLIT("Unable to find default adventure: %08x"), DEFAULT_ADVENTURE_EXTENSION_UNID);
			return ERR_FAIL;
			}
		}

	//	CDynamicDesignTable. Need to load this before we initialize.

	if (Ctx.dwVersion >= 12)
		m_Design.ReadDynamicTypes(Ctx);

	//	Now read a list of all types used by this save file. We need this so 
	//	that we can bind obsolete types.

	if (Ctx.dwVersion >= 30)
		{
		DWORD dwCount;
		pStream->Read(dwCount);
		InitCtx.TypesUsed.GrowToFit(dwCount);
		for (i = 0; i < (int)dwCount; i++)
			{
			DWORD dwUNID;
			pStream->Read(dwUNID);
			InitCtx.TypesUsed.SetAt(dwUNID, true);
			}
		}

	//	Select the proper adventure and extensions and bind design.

	if (Init(InitCtx, &sError) != NOERROR)
		{
		*retsError = strPatternSubst(CONSTLIT("Unable to load universe: %s"), sError);
		return ERR_FAIL;
		}

	//	More data

	pStream->Read((char *)retdwSystemID, sizeof(DWORD));
	pStream->Read((char *)retdwPlayerID, sizeof(DWORD));

	CTimeSpan Time;
	Time.ReadFromStream(pStream);
	CTimeDate Now(CTimeDate::Now);
	m_StartTime = timeSubtractTime(Now, Time);

	//	Load the player controller

	m_pPlayer = m_pHost->CreatePlayerController();
	if (Ctx.dwVersion >= 26)
		{
		pStream->Read((char *)&dwCount, sizeof(DWORD));
		ASSERT(dwCount == 1);

		m_pPlayer->ReadFromStream(Ctx);
		}

	//	Global missions and events

	if (Ctx.dwVersion >= 17)
		{
		SLoadCtx ObjCtx(*this);
		ObjCtx.dwVersion = Ctx.dwSystemVersion;
		ObjCtx.pStream = pStream;

		if (m_AllMissions.ReadFromStream(ObjCtx, retsError) != NOERROR)
			return ERR_FAIL;

		if (Ctx.dwVersion >= 24)
			m_AscendedObjects.ReadFromStream(ObjCtx);

		//	Events

		m_Events.ReadFromStream(ObjCtx);
		}

	//	Load item type data (we only do this for previous versions)

	if (Ctx.dwVersion < 3)
		{
		pStream->Read((char *)&dwCount, sizeof(DWORD));
		for (i = 0; i < (int)dwCount; i++)
			{
			pStream->Read((char *)&dwLoad, sizeof(DWORD));
			CItemType *pType = FindItemType(dwLoad);
			if (pType == NULL)
				{
				*retsError = strPatternSubst(CONSTLIT("Unable to find type: %x (missing mod?)"), dwLoad);
				return ERR_FAIL;
				}

			pType->ReadFromStream(Ctx);
			}
		}

	//	Load topology data

	m_Topology.ReadFromStream(Ctx);

	//	Load type data

	if (Ctx.dwVersion >= 3)
		{
		//	Load all design types

		pStream->Read((char *)&dwCount, sizeof(DWORD));
		for (i = 0; i < (int)dwCount; i++)
			{
			pStream->Read((char *)&dwLoad, sizeof(DWORD));
			CDesignType *pType = FindDesignType(dwLoad);
			if (pType == NULL)
				{
				if (Ctx.dwSystemVersion < 168 && dwLoad == 0x9101d5)
					{
					i++;
					if (i < (int)dwCount)
						{
						do
							{
							pStream->Read((char *)&dwLoad, sizeof(DWORD));
							pType = FindDesignType(dwLoad);
							}
						while (pType == NULL);
						}

					if (pType == NULL)
						{
						*retsError = strPatternSubst(CONSTLIT("Unable to find type: %x (missing mod?)"), dwLoad);
						return ERR_FAIL;
						}

					//	fall through
					}
				else
					{
					*retsError = strPatternSubst(CONSTLIT("Unable to find type: %x (missing mod?)"), dwLoad);
					return ERR_FAIL;
					}
				}

			try
				{
				pType->ReadFromStream(Ctx);
				}
			catch (...)
				{
				*retsError = strPatternSubst(CONSTLIT("Unable to read design type: %x"), pType->GetUNID());
				return ERR_FAIL;
				}
			}
		}

	//	Objects

	if (Ctx.dwVersion >= 18)
		m_Objects.ReadFromStream(Ctx);

	if (Ctx.dwVersion >= 20)
		m_ObjStats.ReadFromStream(Ctx);

	//	Load previous versions' type data

	if (Ctx.dwVersion < 3)
		{
		pStream->Read((char *)&dwCount, sizeof(DWORD));
		for (i = 0; i < (int)dwCount; i++)
			{
			pStream->Read((char *)&dwLoad, sizeof(DWORD));
			CStationType *pType = FindStationType(dwLoad);
			if (pType == NULL)
				{
				*retsError = strPatternSubst(CONSTLIT("Unable to find type: %x (missing mod?)"), dwLoad);
				return ERR_FAIL;
				}

			pType->ReadFromStream(Ctx);
			}

		//	If we've still got data left, then this is a newer version and we
		//	can read the ship class data

		if (Ctx.dwVersion < 3)
			{
			if (pStream->Read((char *)&dwCount, sizeof(DWORD)) == NOERROR)
				{
				for (i = 0; i < (int)dwCount; i++)
					{
					pStream->Read((char *)&dwLoad, sizeof(DWORD));
					CShipClass *pClass = FindShipClass(dwLoad);
					if (pClass == NULL)
						{
						*retsError = strPatternSubst(CONSTLIT("Unable to find type: %x (missing mod?)"), dwLoad);
						return ERR_FAIL;
						}

					pClass->ReadFromStream(Ctx);
					}
				}
			}

		//	If we've still got data left, then read the sovereign data

		if (Ctx.dwVersion < 3)
			{
			if (pStream->Read((char *)&dwCount, sizeof(DWORD)) == NOERROR)
				{
				for (i = 0; i < (int)dwCount; i++)
					{
					pStream->Read((char *)&dwLoad, sizeof(DWORD));
					CSovereign *pSovereign = FindSovereign(dwLoad);
					if (pSovereign == NULL)
						{
						*retsError = strPatternSubst(CONSTLIT("Unable to find type: %x (missing mod?)"), dwLoad);
						return ERR_FAIL;
						}

					pSovereign->ReadFromStream(Ctx);
					}
				}
			}
		}

	//	Make sure we initialize adventure encounter overrides

	CAdventureDesc *pAdventure = GetCurrentAdventureDesc();
	if (pAdventure)
		pAdventure->InitEncounterOverrides();

	return NOERROR;
	}

void CUniverse::PaintObject (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pObj)

//	PaintObject
//
//	Paints this object only

	{
	if (m_pCurrentSystem && m_pPOV)
		m_pCurrentSystem->PaintViewportObject(Dest, rcView, m_pPOV, pObj);
	}

void CUniverse::PaintObjectMap (CG32bitImage &Dest, const RECT &rcView, CSpaceObject *pObj)

//	PaintObjectMap
//
//	Paints this object only

	{
	if (m_pCurrentSystem && m_pPOV)
		m_pCurrentSystem->PaintViewportMapObject(Dest, rcView, m_pPOV, pObj);
	}

void CUniverse::PaintPOV (CG32bitImage &Dest, const RECT &rcView, DWORD dwFlags)

//	PaintPOV
//
//	Paint the current point of view

	{
	if (m_pCurrentSystem && m_pPOV)
		{
		m_pCurrentSystem->PaintViewport(Dest, rcView, m_pPOV, dwFlags, &m_ViewportAnnotations);

		//	Reset annotations until the next update

		m_ViewportAnnotations.Init();
		}

	m_iPaintTick++;
	}

void CUniverse::PaintPOVLRS (CG32bitImage &Dest, const RECT &rcView, Metric rScale, DWORD dwFlags, bool *retbNewEnemies)

//	PaintPOVLRS
//
//	Paint the LRS from the current POV

	{
	if (m_pCurrentSystem && m_pPOV)
		m_pCurrentSystem->PaintViewportLRS(Dest, rcView, m_pPOV, rScale, dwFlags, retbNewEnemies);
	}

void CUniverse::PaintPOVMap (CG32bitImage &Dest, const RECT &rcView, Metric rMapScale)

//	PaintPOVMap
//
//	Paint the system map

	{
	if (m_pCurrentSystem && m_pPOV)
		m_pCurrentSystem->PaintViewportMap(Dest, rcView, m_pPOV, rMapScale);

	m_iPaintTick++;
	}

void CUniverse::PlaySound (CSpaceObject *pSource, int iChannel)

//	PlaySound
//
//	Plays a sound from the given source

	{
	if (!m_bNoSound 
			&& m_pSoundMgr 
			&& iChannel != -1)
		{
		//	Default to full volume

		int iVolume = 0;
		int iPan = 0;

		//	Figure out how close the source is to the POV. The sound fades as we get
		//	further away.

		if (pSource && m_pPOV)
			{
			CVector vDist = pSource->GetPos() - m_pPOV->GetPos();
			Metric rDist2 = vDist.Length2();
			iVolume = -(int)(10000.0 * rDist2 / (MAX_SOUND_DISTANCE * MAX_SOUND_DISTANCE));

			//	If below a certain level, then it is silent anyway

			if (iVolume <= -10000)
				return;

			//	Adjust left/right volume based on direction

			iPan = (int)(10000.0 * (vDist.GetX() / MAX_SOUND_DISTANCE));
			}

		m_pSoundMgr->Play(iChannel, iVolume, iPan);
		}
	}

void CUniverse::PutPlayerInSystem (CShip *pPlayerShip, const CVector &vPos, CSystemEventList &SavedEvents)

//	PutPlayerInSystem
//
//	Player is placed in the current system.

	{
	ASSERT(m_pCurrentSystem);
	ASSERT(pPlayerShip->GetSystem() == NULL);

	//	Clear the time-stop flag if necessary

	if (pPlayerShip->IsTimeStopped())
		pPlayerShip->RestartTime();

	//	Place the player in the new system

	pPlayerShip->Place(vPos);
	pPlayerShip->AddToSystem(m_pCurrentSystem);
	pPlayerShip->OnNewSystem(m_pCurrentSystem);

	//	Restore timer events

	m_pCurrentSystem->TransferObjEventsIn(pPlayerShip, SavedEvents);

	//	Set globals

	m_CC.DefineGlobalInteger(STR_G_PLAYER_SHIP, (int)pPlayerShip);

	//	POV

	SetPOV(pPlayerShip);

	//	Fire global event

	FireOnGlobalPlayerEnteredSystem();

	//	Tell all objects that the player has entered the system. This calls
	//	OnPlayerObj for all objects in the system.

	m_pCurrentSystem->PlayerEntered(pPlayerShip);

	//	Tell all missions that the player has entered the system

	m_AllMissions.NotifyOnPlayerEnteredSystem(pPlayerShip);
	}

void CUniverse::RefreshCurrentMission (void)

//	RefreshCurrentMission
//
//	Picks the most recently accepted player mission and refresh the target.

	{
	CMission *pMission = GetCurrentMission();
	if (pMission == NULL)
		return;

	pMission->SetPlayerTarget();
	}

ALERROR CUniverse::Reinit (void)

//	Reinit
//	
//	Reinitializes the universe

	{
    int i;

	DEBUG_TRY

	//	We start at tick 1 because sometimes we need to start with some things
	//	in the past.

	m_iTick = 1;
	m_iPaintTick = 1;

	//	Clear some basic variables

	m_bRegistered = false;
	m_bResurrectMode = false;
	m_Time.DeleteAll();
	m_pPOV = NULL;
	SetCurrentSystem(NULL);
    for (i = 0; i < m_StarSystems.GetCount(); i++)
        delete m_StarSystems[i];
	m_StarSystems.DeleteAll();
	m_dwNextID = 1;
	m_Objects.DeleteAll();

	//	NOTE: We don't reinitialize m_bDebugMode or m_bRegistered because those
	//	are set before Reinit (and thus we would overwrite them).

	//	Reinitialize some global classes

	CCompositeImageModifiers::Reinit();

	//	Clean up global missions

	m_AllMissions.DeleteAll();
	m_AscendedObjects.DeleteAll();

	//	Delete all events

	m_Events.DeleteAll();

	//	Reinitialize types

	m_Design.Reinit();
	g_bDefaultEffectsInit = false;

	//	Clear the topology nodes

	m_Topology.DeleteAll();

	//	Clear out player variables

	SetPlayer(NULL);
	SetPlayerShip(NULL);

	//	Reinitialize noise randomness

	NoiseReinit();

	return NOERROR;

	DEBUG_CATCH
	}

ALERROR CUniverse::SaveToStream (IWriteStream *pStream)

//	SaveToStream
//
//	Saves the universe to a stream.
//
//	DWORD		UNIVERSE_VERSION_MARKER
//	DWORD		universe version
//	DWORD		system version
//	DWORD		m_iTick
//	DWORD		flags
//	DWORD		m_dwNextID
//	CGameTimeKeeper m_Time
//
//	DWORD		No of enabled extensions
//	SExtensionSaveDesc for each
//
//	SExtensionSaveDesc for adventure
//
//	CDynamicDesignTable
//
//	DWORD		No of types used
//	DWORD		type: UNID
//
//	DWORD		ID of POV system (0xffffffff if none)
//	DWORD		index of POV (0xffffffff if none)
//	DWORD		milliseconds that we've spent playing the game
//
//	IPlayerController m_pPlayer
//	CMissionList	m_AllMissions
//	CAscendedObjectList	m_AscendedObjects
//	CSystemEventList	m_Events
//
//	DWORD		No of topology nodes
//	CString		node: Node ID
//	DWORD		node: ID of system instance (0xffffffff if not yet created)
//
//	DWORD		No of types
//	DWORD		type: UNID
//	CDesignType
//	CObjectTracker
//	CObjStats

	{
	int i;
	DWORD dwSave;
	CString sError;

	//	Write out version

	dwSave = UNIVERSE_VERSION_MARKER;
	pStream->Write(dwSave);
	dwSave = UNIVERSE_SAVE_VERSION;
	pStream->Write(dwSave);
	dwSave = SYSTEM_SAVE_VERSION;
	pStream->Write(dwSave);

	//	Write basic data

	pStream->Write(m_iTick);

	dwSave = 0;
	dwSave |= (m_bRegistered ? 0x00000001 : 0);
	dwSave |= (m_Design.FindExtension(DEFAULT_COMPATIBILITY_LIBRARY_UNID) ? 0x00000002 : 0);
	pStream->Write(dwSave);

	pStream->Write(m_dwNextID);
	m_Time.WriteToStream(pStream);

	//	Extensions
	//
	//	NOTE: We don't save libraries because those will be automatically loaded
	//	based on adventure and extensions.

	TArray<CExtension *> ExtensionList;
	m_Design.GetEnabledExtensions(&ExtensionList);
	dwSave = ExtensionList.GetCount();
	pStream->Write(dwSave);

	for (i = 0; i < ExtensionList.GetCount(); i++)
		{
		SExtensionSaveDesc Desc;

		Desc.dwUNID = ExtensionList[i]->GetUNID();
		Desc.dwRelease = ExtensionList[i]->GetRelease();

		pStream->Write((char *)&Desc, sizeof(SExtensionSaveDesc));
		}

	//	Adventure UNID

	SExtensionSaveDesc Desc;
	Desc.dwUNID = m_pAdventure->GetExtension()->GetUNID();
	Desc.dwRelease = m_pAdventure->GetExtension()->GetRelease();
	pStream->Write((char *)&Desc, sizeof(SExtensionSaveDesc));

	//	CDynamicDesignTable

	m_Design.WriteDynamicTypes(pStream);

	//	Types used.

	DWORD dwCount = m_Design.GetCount();
	pStream->Write(dwCount);
	for (i = 0; i < (int)dwCount; i++)
		pStream->Write(m_Design.GetEntry(i)->GetUNID());

	//	Write the ID of POV system

	dwSave = 0xffffffff;
	if (m_pPOV && m_pPOV->GetSystem())
		dwSave = m_pPOV->GetSystem()->GetID();
	pStream->Write(dwSave);

	//	Write the ID of the POV

	dwSave = OBJID_NULL;
	if (m_pPOV && m_pPOV->GetSystem())
		{
		m_pPOV->GetSystem()->WriteObjRefToStream(m_pPOV, pStream);

		if (!m_pPOV->IsPlayer())
			kernelDebugLogPattern("ERROR: Saving without player ship.");
		}

	//	Calculate the amount of time that we've been playing the game

	CTimeSpan GameLength = StopGameTime();
	GameLength.WriteToStream(pStream);

	//	Save out the player data (we start by writing out the number of players
	//	and then write out the only player).

	dwSave = 1;
	pStream->Write(dwSave);
	m_pPlayer->WriteToStream(pStream);

	//	Save out the global mission data

	if (m_AllMissions.WriteToStream(pStream, &sError) != NOERROR)
		return ERR_FAIL;

	m_AscendedObjects.WriteToStream(pStream);

	//	Save out event data. Note that we save this after the missions because
	//	events may refer to missions.

	m_Events.WriteToStream(m_pCurrentSystem, pStream);

	//	Save out topology node data

	dwCount = GetTopologyNodeCount();
	pStream->Write(dwCount);
	for (i = 0; i < (int)dwCount; i++)
		{
		CTopologyNode *pNode = GetTopologyNode(i);
		pNode->WriteToStream(pStream);
		}

	//	Save out type data

	dwCount = m_Design.GetCount();
	pStream->Write(dwCount);
	for (i = 0; i < (int)dwCount; i++)
		{
		CDesignType *pType = m_Design.GetEntry(i);

		dwSave = pType->GetUNID();
		pStream->Write(dwSave);

		pType->WriteToStream(pStream);
		}

	//	Save out universal object data. Must be written out after the
	//	topology data, because we need that.

	m_Objects.WriteToStream(pStream);
	m_ObjStats.WriteToStream(pStream);

	return NOERROR;
	}

void CUniverse::SetCurrentSystem (CSystem *pSystem)

//	SetCurrentSystem
//
//	Sets the current system

	{
	if (m_pCurrentSystem)
		m_pCurrentSystem->FlushEnemyObjectCache();

	CSystem *pOldSystem = m_pCurrentSystem;
	m_pCurrentSystem = pSystem;

	if (pSystem)
		{
		g_KlicksPerPixel = pSystem->GetSpaceScale();
		g_TimeScale = pSystem->GetTimeScale();
		g_SecondsPerUpdate = g_TimeScale / g_TicksPerSecond;
		}
	else
		{
		g_KlicksPerPixel = KLICKS_PER_PIXEL;
		g_TimeScale = TIME_SCALE;
		g_SecondsPerUpdate = g_TimeScale / g_TicksPerSecond;
		}

	//	Initialize mission cache

	if (pOldSystem != m_pCurrentSystem)
		m_AllMissions.NotifyOnNewSystem(m_pCurrentSystem);
	}

bool CUniverse::SetExtensionData (EStorageScopes iScope, DWORD dwExtension, const CString &sAttrib, const CString &sData)

//	SetExtensionData
//
//	Sets data for the extension

	{
	switch (iScope)
		{
		case storeDevice:
			return m_DeviceStorage.SetData(dwExtension, sAttrib, sData);

		default:
			return false;
		}
	}

void CUniverse::SetHost (IHost *pHost)

//	SetHost
//
//	Sets the host for the universe
	
	{
	if (pHost == NULL)
		m_pHost = &g_DefaultHost;
	else
		m_pHost = pHost;
	}

void CUniverse::SetNewSystem (CSystem *pSystem, CSpaceObject *pPOV)

//	SetNewSystem
//
//	Shows the new system before the player appears.

	{
	int i;

	//	Before we set the POV, delete any old missions

	for (i = 0; i < m_AllMissions.GetCount(); i++)
		{
		CMission *pMission = m_AllMissions.GetMission(i);

		//	If this is a completed non-player mission, then we delete it.

		if ((pMission->IsCompletedNonPlayer() && pMission->CleanNonPlayer()) || pMission->IsDestroyed())
			{
			m_AllMissions.Delete(i);
			i--;
			}
		}

	//	Set the new system

	SetPOV(pPOV);

	//	Replay any commands that might have happened while the player was in a
	//	different system.

	m_Objects.ReplayCommands(pSystem);

	//	Add a discontinuity to reflect the amount of time spent
	//	in the stargate

	AddTimeDiscontinuity(CTimeSpan(0, mathRandom(0, (SECONDS_PER_DAY - 1) * 1000)));

	//	Time passes

	SetProgramState(psStargateUpdateExtended);
	UpdateExtended();

	//	Clear the POVLRS flag for all objects (so that we don't get the
	//	"Enemy Ships Detected" message when entering a system

	pSystem->SetPOVLRS(pPOV);
	}

void CUniverse::SetPlayer (IPlayerController *pPlayer)

//	SetPlayer
//
//	Sets the player

	{
	CCodeChain &CC = GetCC();

	if (m_pPlayer)
		delete m_pPlayer;

	m_pPlayer = pPlayer;

	CC.DefineGlobal(STR_G_PLAYER, (m_pPlayer ? m_pPlayer->CreateGlobalRef(CC) : CC.CreateNil()));
	}

void CUniverse::SetPlayerShip (CSpaceObject *pPlayer)

//	SetPlayerShip
//
//	Sets the current player ship

	{
	CCodeChain &CC = GetCC();

	m_pPlayerShip = pPlayer;
	CC.DefineGlobal(STR_G_PLAYER_SHIP, (m_pPlayerShip ? CC.CreateInteger((int)m_pPlayerShip) : CC.CreateNil()));
	}

bool CUniverse::SetPOV (CSpaceObject *pPOV)

//	SetPOV
//
//	Sets the current point of view. The POV is used for both painting
//	and to determine what should be updated.

	{
	//	Only valid if POV is part of a system.

	if (pPOV && (pPOV->GetSystem() == NULL || pPOV->IsDestroyed()))
		{
		//	We try to leave the universe in a valid state. I.e., we cannot leave
		//	the old POV because it might have been destroyed.

		SetPOV(NULL);
		return false;
		}

	//	Remove old POV

	if (m_pPOV)
		{
		CSpaceObject *pOldPOV = m_pPOV;
		m_pPOV = NULL;
		pOldPOV->OnLosePOV();
		}

	m_pPOV = pPOV;

	if (m_pPOV)
		SetCurrentSystem(m_pPOV->GetSystem());
	else
		SetCurrentSystem(NULL);

	return true;
	}

void CUniverse::StartGame (bool bNewGame)

//	StartGame
//
//	Starts a game (either new or continued)

	{
	CCodeChain &CC = GetCC();

	//	At this point we can define the player variables

	CC.DefineGlobal(STR_G_PLAYER, (m_pPlayer ? m_pPlayer->CreateGlobalRef(CC) : CC.CreateNil()));
	CC.DefineGlobal(STR_G_PLAYER_SHIP, (m_pPlayerShip ? CC.CreateInteger((int)m_pPlayerShip) : CC.CreateNil()));

	//	Load images necessary for the system

	MarkLibraryBitmaps();

	//	Fire some events

	if (bNewGame)
		{
		//	Tell all types that the universe have been created.
		//	This must be after m_Universe.SetPlayerShip, which sets up
		//	gPlayerShip.

		FireOnGlobalUniverseCreated();

		//	The current system has started (0 time has elased since we last
		//	updated this system).

		m_Design.FireOnGlobalSystemStarted(0);

		//	If we have a player then tell objects that the player has entered
		//	the system.

		if (m_pPlayerShip)
			{
            //  Player entered ship

            m_pPlayerShip->FireOnPlayerEnteredShip(NULL);

            //  Player entered system

			FireOnGlobalPlayerEnteredSystem();

			//	Tell all objects that the player has entered the system

			GetCurrentSystem()->PlayerEntered(m_pPlayerShip);
			}
		}

	//	Otherwise, if this is an old game being restored, fire OnGlobalUniverseLoad

	else
		{
		FireOnGlobalUniverseLoad();
		}
	}

void CUniverse::StartGameTime (void)

//	StartGameTime
//
//	Starts timing the game

	{
	m_StartTime = CTimeDate(CTimeDate::Now);
	}

CTimeSpan CUniverse::StopGameTime (void)

//	StopGameTime
//
//	Returns the time elapsed between m_StartTime and now.

	{
	CTimeDate StopTime(CTimeDate::Now);
	return timeSpan(m_StartTime, StopTime);
	}

void CUniverse::Update (SSystemUpdateCtx &Ctx)

//	Update
//
//	Update the system of the current point of view

	{
	DEBUG_TRY

	if (m_pCurrentSystem == NULL)
		return;

	//	Update system

	m_pCurrentSystem->Update(Ctx, &m_ViewportAnnotations);

	//	Fire timed events

	m_Events.Update(m_iTick, *m_pCurrentSystem);

	//	Update missions

	UpdateMissions(m_iTick, m_pCurrentSystem);

	//	Update sovereigns

	UpdateSovereigns(m_iTick, m_pCurrentSystem);

	//	Update types

	m_Design.FireOnGlobalUpdate(m_iTick);

	//	Next

	m_iTick++;

	DEBUG_CATCH
	}

void CUniverse::UpdateExtended (void)

//	UpdateExtended
//
//	This is called when the player re-enters a system after having been
//	away. We update the system to reflect the amount of time that has passed.

	{
	DEBUG_TRY

	CSystem *pSystem = GetCurrentSystem();
	ASSERT(pSystem);
	if (pSystem == NULL)
		return;

	//	Calculate the amount of time that has elapsed from the last time the
	//	system was updated.

	CTimeSpan TotalTime(0);
	int iLastUpdated = pSystem->GetLastUpdated();
	if (iLastUpdated != -1)
		TotalTime = m_Time.GetElapsedTimeAt(m_iTick) - m_Time.GetElapsedTimeAt(iLastUpdated);

	//	Update the system 

	pSystem->UpdateExtended(TotalTime);

	DEBUG_CATCH
	}

void CUniverse::UpdateMissions (int iTick, CSystem *pSystem)

//	UpdateMissions
//
//	Update missions in the system.

	{
	DEBUG_TRY

	int i;

	SUpdateCtx Ctx;
	Ctx.pSystem = pSystem;

	//	Loop over all active missions (i.e., missions that the player has 
	//	accepted and not yet completed).

	for (i = 0; i < m_AllMissions.GetCount(); i++)
		{
		CMission *pMission = m_AllMissions.GetMission(i);

		//	If this is an active mission, update it.

		if (!pMission->IsDestroyed() && pMission->IsActive())
			pMission->Update(Ctx);
		}

	//	Every 11 ticks we check for mission expirations (since it is not that 
	//	important if we expire them right away)

	if ((iTick % 11) == 0)
		{
		for (i = 0; i < m_AllMissions.GetCount(); i++)
			{
			CMission *pMission = m_AllMissions.GetMission(i);

			if (!pMission->IsDestroyed() && pMission->IsOpen())
				pMission->UpdateExpiration(iTick);
			}
		}

	DEBUG_CATCH
	}

void CUniverse::UpdateSovereigns (int iTick, CSystem *pSystem)

//	UpdateSovereigns
//
//	Update each sovereign

	{
	DEBUG_TRY

	for (int i = 0; i < GetSovereignCount(); i++)
		{
		CSovereign *pSovereign = GetSovereign(i);
		pSovereign->Update(iTick, pSystem);
		}

	DEBUG_CATCH
	}

CString CUniverse::ValidatePlayerName (const CString &sName)

//	ValidatePlayerName
//
//	Generates a valid name from the given user input

	{
	//	Trim leading and trailing whitespace

	CString sNewName = strTrimWhitespace(sName);

	//	Remove escape characters and symbols

	char *pPos = sNewName.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		switch (*pPos)
			{
			case '/':
			case '%':
				*pPos = '_';
				break;

			default:
				if (strIsASCIIControl(pPos))
					*pPos = '_';
				break;
			}

		pPos++;
		}

	//	Make sure it is not blank

	if (sNewName.IsBlank())
		return CONSTLIT("Shofixti");

	//	Done

	return sNewName;
	}

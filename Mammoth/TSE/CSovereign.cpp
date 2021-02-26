//	CSovereign.cpp
//
//	CSovereign class

#include "PreComp.h"

#define LANGUAGE_TAG							CONSTLIT("Language")
#define RELATIONSHIPS_TAG						CONSTLIT("Relationships")
#define RELATIONSHIP_TAG						CONSTLIT("Relationship")

#define ADJECTIVE_ATTRIB						CONSTLIT("adjective")
#define ALIGNMENT_ATTRIB						CONSTLIT("alignment")
#define DEMONYM_ATTRIB							CONSTLIT("demonym")
#define DISPOSITION_ATTRIB						CONSTLIT("disposition")
#define ID_ATTRIB								CONSTLIT("id")
#define MUTUAL_ATTRIB							CONSTLIT("mutual")
#define NAME_ATTRIB								CONSTLIT("name")
#define PLURAL_ATTRIB							CONSTLIT("plural")
#define TEXT_ATTRIB								CONSTLIT("text")
#define SHORT_NAME_ATTRIB						CONSTLIT("shortName")
#define SOVEREIGN_ATTRIB						CONSTLIT("sovereign")
#define VIRTUAL_ATTRIB							CONSTLIT("virtual")

#define CONSTRUCTIVE_CHAOS_ALIGN				CONSTLIT("constructive chaos")
#define CONSTRUCTIVE_ORDER_ALIGN				CONSTLIT("constructive order")
#define NEUTRAL_ALIGN							CONSTLIT("neutral")
#define DESTRUCTIVE_ORDER_ALIGN					CONSTLIT("destructive order")
#define DESTRUCTIVE_CHAOS_ALIGN					CONSTLIT("destructive chaos")

#define DISP_FRIEND								CONSTLIT("friend")
#define DISP_NEUTRAL							CONSTLIT("neutral")
#define DISP_ENEMY								CONSTLIT("enemy")

#define EVENT_ON_UPDATE							CONSTLIT("OnUpdate")

#define FIELD_NAME								CONSTLIT("name")

static const char *g_DefaultText[] =
	{
	//	0
	"",

	//	msgAttack
	"Attack target",

	//	msgDestroyBroadcast
	"",

	//	msgHitByFriendlyFire
	"",

	//	msgQueryEscortStatus
	"",

	//	msgQueryFleetStatus
	"",

	//	msgEscortAttacked
	"",

	//	msgEscortReportingIn
	"",

	//	msgWatchTargets
	"Watch your targets!",

	//	msgNiceShooting
	"Nice shooting!",

	//	msgFormUp
	"Form up!",

	//	msgBreakAndAttack
	"Break & attack",

	//	msgQueryCommunications
	"",

	//	msgAbortAttack
	"Cancel attack"
	};

#define DEFAULT_TEXT_COUNT					(sizeof(g_DefaultText) / sizeof(g_DefaultText[0]))

static CString *g_pDefaultText = NULL;

static char g_UNIDAttrib[] = "UNID";
static char g_NameAttrib[] = "name";

CSovereign::Disposition g_DispositionTable[5][5] =
	{
		//	WE are constructive chaos...
		{
			CSovereign::dispFriend,			//	THEY are constructive chaos
			CSovereign::dispNeutral,		//	THEY are constructive order
			CSovereign::dispNeutral,		//	THEY are neutral
			CSovereign::dispEnemy,			//	THEY are destructive order
			CSovereign::dispEnemy,			//	THEY are destructive chaos
		},

		//	WE are constructive order...
		{
			CSovereign::dispNeutral,		//	THEY are constructive chaos
			CSovereign::dispFriend,			//	THEY are constructive order
			CSovereign::dispNeutral,		//	THEY are neutral
			CSovereign::dispEnemy,			//	THEY are destructive order
			CSovereign::dispEnemy,			//	THEY are destructive chaos
		},

		//	WE are neutral...
		{
			CSovereign::dispNeutral,		//	THEY are constructive chaos
			CSovereign::dispNeutral,		//	THEY are constructive order
			CSovereign::dispNeutral,		//	THEY are neutral
			CSovereign::dispNeutral,		//	THEY are destructive order
			CSovereign::dispEnemy,			//	THEY are destructive chaos
		},

		//	WE are destructive order...
		{
			CSovereign::dispEnemy,			//	THEY are constructive chaos
			CSovereign::dispEnemy,			//	THEY are constructive order
			CSovereign::dispNeutral,		//	THEY are neutral
			CSovereign::dispNeutral,		//	THEY are destructive order
			CSovereign::dispEnemy,			//	THEY are destructive chaos
		},

		//	WE are destructive chaos...
		{
			CSovereign::dispEnemy,			//	THEY are constructive chaos
			CSovereign::dispEnemy,			//	THEY are constructive order
			CSovereign::dispEnemy,			//	THEY are neutral
			CSovereign::dispEnemy,			//	THEY are destructive order
			CSovereign::dispEnemy,			//	THEY are destructive chaos
		},
	};

const DWORD ALIGN_FLAG_CONSTRUCTIVE =		0x00000001;
const DWORD ALIGN_FLAG_DESTRUCTIVE =		0x00000002;
const DWORD ALIGN_FLAG_COMMUNITY =			0x00000004;
const DWORD ALIGN_FLAG_INDEPENDENCE =		0x00000008;
const DWORD ALIGN_FLAG_KNOWLEDGE =			0x00000010;
const DWORD ALIGN_FLAG_SPIRITUALITY =		0x00000020;
const DWORD ALIGN_FLAG_EVOLUTION =			0x00000040;
const DWORD ALIGN_FLAG_TRADITION =			0x00000080;

static TStaticStringTable<TStaticStringEntry<DWORD>, 8> ALIGN_FLAG_TABLE = {
	"community",			ALIGN_FLAG_COMMUNITY,
	"constructive",			ALIGN_FLAG_CONSTRUCTIVE,
	"destructive",			ALIGN_FLAG_DESTRUCTIVE,
	"evolution",			ALIGN_FLAG_EVOLUTION,
	"independence",			ALIGN_FLAG_INDEPENDENCE,
	"knowledge",			ALIGN_FLAG_KNOWLEDGE,
	"spirituality",			ALIGN_FLAG_SPIRITUALITY,
	"tradition",			ALIGN_FLAG_TRADITION,
	};

struct SAlignData
	{
	int iNameLen;
	char pszName[32];
	DispositionClasses iDispClass;
	DWORD dwFlags;
	};

static SAlignData ALIGN_DATA[CSovereign::alignCount] =
	{
		{	CONSTDEF("republic"),		alignConstructiveChaos,		ALIGN_FLAG_CONSTRUCTIVE | ALIGN_FLAG_COMMUNITY | ALIGN_FLAG_KNOWLEDGE | ALIGN_FLAG_EVOLUTION },
		{	CONSTDEF("federation"),		alignConstructiveOrder,		ALIGN_FLAG_CONSTRUCTIVE | ALIGN_FLAG_COMMUNITY | ALIGN_FLAG_KNOWLEDGE | ALIGN_FLAG_TRADITION },
		{	CONSTDEF("uplifter"),		alignConstructiveChaos,		ALIGN_FLAG_CONSTRUCTIVE | ALIGN_FLAG_COMMUNITY | ALIGN_FLAG_SPIRITUALITY | ALIGN_FLAG_EVOLUTION },
		{	CONSTDEF("foundation"),		alignConstructiveOrder,		ALIGN_FLAG_CONSTRUCTIVE | ALIGN_FLAG_COMMUNITY | ALIGN_FLAG_SPIRITUALITY | ALIGN_FLAG_TRADITION },
		{	CONSTDEF("competitor"),		alignNeutral,				ALIGN_FLAG_CONSTRUCTIVE | ALIGN_FLAG_INDEPENDENCE | ALIGN_FLAG_KNOWLEDGE | ALIGN_FLAG_EVOLUTION },
		{	CONSTDEF("archivist"),		alignConstructiveOrder,		ALIGN_FLAG_CONSTRUCTIVE | ALIGN_FLAG_INDEPENDENCE | ALIGN_FLAG_KNOWLEDGE | ALIGN_FLAG_TRADITION },
		{	CONSTDEF("seeker"),			alignConstructiveChaos,		ALIGN_FLAG_CONSTRUCTIVE | ALIGN_FLAG_INDEPENDENCE | ALIGN_FLAG_SPIRITUALITY | ALIGN_FLAG_EVOLUTION },
		{	CONSTDEF("hermit"),			alignConstructiveOrder,		ALIGN_FLAG_CONSTRUCTIVE | ALIGN_FLAG_INDEPENDENCE | ALIGN_FLAG_SPIRITUALITY | ALIGN_FLAG_TRADITION },

		{	CONSTDEF("collective"),		alignDestructiveChaos,		ALIGN_FLAG_DESTRUCTIVE | ALIGN_FLAG_COMMUNITY | ALIGN_FLAG_KNOWLEDGE | ALIGN_FLAG_EVOLUTION },
		{	CONSTDEF("empire"),			alignDestructiveOrder,		ALIGN_FLAG_DESTRUCTIVE | ALIGN_FLAG_COMMUNITY | ALIGN_FLAG_KNOWLEDGE | ALIGN_FLAG_TRADITION },
		{	CONSTDEF("sterilizer"),		alignDestructiveChaos,		ALIGN_FLAG_DESTRUCTIVE | ALIGN_FLAG_COMMUNITY | ALIGN_FLAG_SPIRITUALITY | ALIGN_FLAG_EVOLUTION },
		{	CONSTDEF("corrector"),		alignDestructiveOrder,		ALIGN_FLAG_DESTRUCTIVE | ALIGN_FLAG_COMMUNITY | ALIGN_FLAG_SPIRITUALITY | ALIGN_FLAG_TRADITION },
		{	CONSTDEF("megalomaniac"),	alignDestructiveChaos,		ALIGN_FLAG_DESTRUCTIVE | ALIGN_FLAG_INDEPENDENCE | ALIGN_FLAG_KNOWLEDGE | ALIGN_FLAG_EVOLUTION },
		{	CONSTDEF("cryptologue"),	alignDestructiveOrder,		ALIGN_FLAG_DESTRUCTIVE | ALIGN_FLAG_INDEPENDENCE | ALIGN_FLAG_KNOWLEDGE | ALIGN_FLAG_TRADITION },
		{	CONSTDEF("perversion"),		alignDestructiveChaos,		ALIGN_FLAG_DESTRUCTIVE | ALIGN_FLAG_INDEPENDENCE | ALIGN_FLAG_SPIRITUALITY | ALIGN_FLAG_EVOLUTION },
		{	CONSTDEF("solipsist"),		alignDestructiveOrder,		ALIGN_FLAG_DESTRUCTIVE | ALIGN_FLAG_INDEPENDENCE | ALIGN_FLAG_SPIRITUALITY | ALIGN_FLAG_TRADITION },

		{	CONSTDEF("unorganized"),	alignNeutral,				0 },
		{	CONSTDEF("subsapient"),		alignNeutral,				0 },
		{	CONSTDEF("predator"),		alignDestructiveChaos,		ALIGN_FLAG_DESTRUCTIVE },
	};

const char *CSovereign::m_CACHED_EVENTS[CACHED_EVENT_COUNT] = 
	{
	"OnShipDestroyedByPlayer",
	"OnStationDestroyedByPlayer"
	};

CSovereign::CSovereign (void)

//	CSovereign constructor

	{
	}

CSovereign::~CSovereign (void)

//	CSovereign destructor

	{
	DeleteRelationships();
	}

bool CSovereign::CalcSelfRel (void)

//	CalcSelfRel
//
//	Returns TRUE if there is a relationship that mentions ourselves

	{
	SRelationship *pRel = m_pFirstRelationship;
	while (pRel && pRel->pSovereign != this)
		pRel = pRel->pNext;

	return (pRel != NULL);
	}

#ifdef DEBUG_ENEMY_CACHE_BUG
void CSovereign::DebugObjDeleted (CSpaceObject *pObj) const

//	DebugObjDeleted
//
//	Make sure this object is not in our cache.

	{
	for (int i = 0; i < m_EnemyObjects.GetCount(); i++)
		if (pObj == m_EnemyObjects.GetObj(i))
			{
			ASSERT(false);
			}
	}
#endif

void CSovereign::DeleteRelationships (void)

//	DeleteRelationships
//
//	Delete all relationships

	{
	DEBUG_TRY

	SRelationship *pRel = m_pFirstRelationship;
	while (pRel)
		{
		SRelationship *pDelete = pRel;
		pRel = pRel->pNext;
		delete pDelete;
		}

	m_pFirstRelationship = NULL;
	m_bSelfRel = false;

	DEBUG_CATCH
	}

bool CSovereign::FindDataField (const CString &sField, CString *retsValue) const

//	FindDataField
//
//	Returns meta-data

	{
	if (strEquals(sField, FIELD_NAME))
		*retsValue = GetNounPhrase();
	else
		return CDesignType::FindDataField(sField, retsValue);

	return true;
	}

const CSovereign::SRelationship *CSovereign::FindRelationship (const CSovereign *pSovereign, bool bCheckParent) const

//	FindRelationship
//
//	Finds a specific relationship with the given sovereign (or NULL
//	if none is found)

	{
	return const_cast<CSovereign *>(this)->FindRelationship(pSovereign, bCheckParent);
	}

CSovereign::SRelationship *CSovereign::FindRelationship (const CSovereign *pSovereign, bool bCheckParent)

//	FindRelationship
//
//	Finds a specific relationship with the given sovereign (or NULL
//	if none is found)

	{
	SRelationship *pRel = m_pFirstRelationship;
	while (pRel && pRel->pSovereign != pSovereign)
		pRel = pRel->pNext;

	//	If we didn't find a relationship and we want to check our parent, then
	//	we see if they have a relationship.

	CSovereign *pInheritFrom;
	if (pRel == NULL 
			&& bCheckParent
			&& (pInheritFrom = CSovereign::AsType(GetInheritFrom())))
		pRel = pInheritFrom->FindRelationship(pSovereign, true);

	//	Next look to see if we have a relationship to an ancestor of the 
	//	sovereign.

	if (pRel == NULL 
			&& bCheckParent
			&& (pInheritFrom = CSovereign::AsType(pSovereign->GetInheritFrom())))
		pRel = FindRelationship(pInheritFrom, true);

	//	Done

	return pRel;
	}

CString CSovereign::GetAlignmentID (Alignments iAlignment)

//	GetAlignmentID
//
//	Returns the ID.

	{
	if (iAlignment < 0 || iAlignment >= alignCount)
		return NULL_STR;


	return CString(ALIGN_DATA[iAlignment].pszName, ALIGN_DATA[iAlignment].iNameLen, true);
	}

IPlayerController *CSovereign::GetController (void)

//	GetController
//
//	Returns the controller for this sovereign

	{
	if (GetUNID() == g_PlayerSovereignUNID)
		return GetUniverse().GetPlayer();
	else
		return NULL;
	}

CSovereign::Disposition CSovereign::GetDispositionTowards (const CSovereign *pSovereign, bool bCheckParent) const

//	GetDispositionTowards
//
//	Returns how this sovereign feels about the given sovereign

	{
	//	Optimize checks against ourselves

	if (!m_bSelfRel && pSovereign == this)
		return dispFriend;

	//	See if we have a specific relationship. If so, then return the
	//	disposition there.

	const SRelationship *pRel = FindRelationship(pSovereign, bCheckParent);
	if (pRel)
		return pRel->iDisp;

	//	Get the alignment of the other. We treat NULL as Neutral

	DispositionClasses iOurDispClass = ALIGN_DATA[GetAlignment()].iDispClass;
	DispositionClasses iTheirDispClass = (pSovereign ? ALIGN_DATA[pSovereign->GetAlignment()].iDispClass : alignNeutral);

	//	Consult the table

	return g_DispositionTable[iOurDispClass][iTheirDispClass];
	}

CString CSovereign::GetNamePattern (DWORD dwNounFormFlags, DWORD *retdwFlags) const

//	GetNamePattern
//
//	Returns the name pattern

	{
	//	All flags are encoded in the name

	if (retdwFlags)
		*retdwFlags = 0;

	if (dwNounFormFlags & nounShort)
		return m_sShortName;
	else if (dwNounFormFlags & nounAdjective)
		return m_sAdjective;
	else if (dwNounFormFlags & nounDemonym)
		return m_sDemonym;
	else
		return m_sName;
	}

CSovereign::EThreatLevels CSovereign::GetPlayerThreatLevel (void) const

//	GetPlayerThreatLevel
//
//	Computes the threat the player poses to us, based on what they've destroyed.

	{
	//	If she's destroyed more than 10 stations, the player is an existential
	//	threat.

	if (m_iStationsDestroyedByPlayer > 10)
		return threatExistential;

	//	If she's destroyed more than 5 stations or more than 15 ships, then she
	//	is considered a major threat.

	else if (m_iStationsDestroyedByPlayer > 5 || m_iShipsDestroyedByPlayer > 15)
		return threatMajor;

	//	If she's destroyed at least one station, then the player is a minor 
	//	raiding threat.

	else if (m_iStationsDestroyedByPlayer > 0)
		return threatMinorRaiding;

	//	If she's destroyed more than 5 ships, then the player is a piracy
	//	threat.

	else if (m_iShipsDestroyedByPlayer > 5)
		return threatMinorPiracy;

	//	Otherwise, she is no threat.

	else
		return threatNone;
	}

bool CSovereign::GetPropertyInteger (const CString &sProperty, int *retiValue)

//	GetPropertyInteger
//
//	Returns an integer property

	{
	IPlayerController *pController;

	if (pController = GetController())
		return pController->GetPropertyInteger(sProperty, retiValue);
	else
		return false;
	}

bool CSovereign::GetPropertyItemList (const CString &sProperty, CItemList *retItemList)

//	GetPropertyItemList
//
//	Returns an item list property

	{
	IPlayerController *pController;

	if (pController = GetController())
		return pController->GetPropertyItemList(sProperty, retItemList);
	else
		return false;
	}

bool CSovereign::GetPropertyString (const CString &sProperty, CString *retsValue)

//	GetPropertyString
//
//	Returns a string property

	{
	IPlayerController *pController;

	if (pController = GetController())
		return pController->GetPropertyString(sProperty, retsValue);
	else
		return false;
	}

CString CSovereign::GetText (MessageTypes iMsg)

//	GetText
//
//	Get the text associated with the given message

	{
	//	Look up the message in our data table

	CString sString;
	if (TranslateText(strFromInt(iMsg), NULL, &sString))
		return sString;

	//	If we don't already have it, load the default string array

	if (g_pDefaultText == NULL)
		{
		g_pDefaultText = new CString [DEFAULT_TEXT_COUNT];
		for (int i = 0; i < DEFAULT_TEXT_COUNT; i++)
			g_pDefaultText[i] = CString(g_DefaultText[i], lstrlen(g_DefaultText[i]), true);
		}

	//	Get default text

	if ((int)iMsg > 0 && (int)iMsg < DEFAULT_TEXT_COUNT)
		return g_pDefaultText[iMsg];
	else
		return g_pDefaultText[0];
	}

bool CSovereign::IsPlayerOwned (void)

//  IsPlayerOwned
//
//  Determine if this sovereign inherits from svPlayer, but is NOT actually the
//	player sovereign.

	{
	CSovereign *pParent = CSovereign::AsType(GetInheritFrom());
	if (pParent == NULL)
		return false;
	else if (pParent->GetUNID() == g_PlayerSovereignUNID)
		return true;
	else
		return pParent->IsPlayerOwned();
	}

void CSovereign::InitEnemyObjectList (const CSystem *pSystem) const

//	InitEnemyObjectList
//
//	Compiles and caches a list of enemy objects in the system

	{
	if (m_pEnemyObjectsSystem == pSystem
			|| IsVirtual())
		return;

	//	Initialize

	m_EnemyObjects.SetAllocSize(pSystem->GetObjectCount());

	for (int i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj 
				&& pObj->ClassCanAttack()
				&& !pObj->IsDestroyed()
				&& IsEnemy(pObj->GetSovereign()))
			m_EnemyObjects.FastAdd(pObj);
		}

	m_pEnemyObjectsSystem = pSystem;
	}

void CSovereign::InitRelationships (void)

//	InitRelationships
//
//	Initialize relationships from XML element

	{
	DEBUG_TRY

	int i;

	if (m_pInitialRelationships)
		{
		for (i = 0; i < m_pInitialRelationships->GetContentElementCount(); i++)
			{
			CXMLElement *pRelDesc = m_pInitialRelationships->GetContentElement(i);
			CString sTarget;

			//	Disposition

			Disposition iDisp = ParseDisposition(pRelDesc->GetAttribute(DISPOSITION_ATTRIB));
			bool bMutual = pRelDesc->GetAttributeBool(MUTUAL_ATTRIB);

			//	If this relationship is towards an alignment, then handle it

			if (pRelDesc->FindAttribute(ALIGNMENT_ATTRIB, &sTarget))
				{
				//	See if this is a flag

				DWORD dwAlignmentFlag = ParseAlignmentFlag(sTarget);
				if (dwAlignmentFlag)
					{
					SetDispositionTowardsFlag(dwAlignmentFlag, iDisp, bMutual);
					}

				//	Otherwise, we expect a named alignment

				else
					{
					Alignments iAlignment = ParseAlignment(sTarget);
					if (iAlignment == alignUnknown)
						{
						::kernelDebugLogPattern("[%08x]: Unknown alignment: %s.", GetUNID(), sTarget);
						continue;
						}

					//	Set the disposition for all sovereigns of the given 
					//	alignment.

					SetDispositionTowards(iAlignment, iDisp, bMutual);
					}
				}

			//	Otherwise, see if this is towards a sovereign

			else if (pRelDesc->FindAttribute(SOVEREIGN_ATTRIB, &sTarget))
				{
				CSovereign *pTarget = GetUniverse().FindSovereignUnbound(strToInt(sTarget, 0));
				if (pTarget == NULL)
					{
					::kernelDebugLogPattern("[%08x]: Unknown sovereign: %s.", GetUNID(), sTarget);
					continue;
					}

				SetDispositionTowards(pTarget, iDisp, bMutual);
				}

			//	Otherwise, this is an error

			else
				{
				::kernelDebugLogPattern("[%08x]: <Relationship> must have a target.", GetUNID());
				}
			}
		}

	DEBUG_CATCH
	}

void CSovereign::MessageFromObj (CSpaceObject *pSender, const CString &sText)

//	MessageFromObj
//
//	Receive a message from the given (optional) object

	{
	//	If this is the player, then we let the player handle it.

	if (GetUNID() == g_PlayerSovereignUNID)
		{
		IPlayerController *pPlayer = GetUniverse().GetPlayer();
		if (pPlayer == NULL)
			return;

		pPlayer->OnMessageFromObj(pSender, sText);
		}

	//	Otherwise...

	else
		{
		//	LATER: We should pass this to the sovereign and let it 
		//	handle it (possibly inside an event).
		}
	}

void CSovereign::OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const

//	OnAccumulateXMLMergeFlags
//
//	Returns flags for merging XML.

	{
	//	We know how to handle these tags through the inheritance hierarchy.

	MergeFlags.SetAt(CXMLElement::GetKeywordID(RELATIONSHIPS_TAG), CXMLElement::MERGE_OVERRIDE);
	}

void CSovereign::OnAddTypesUsed (TSortMap<DWORD, bool> *retTypesUsed)

//	OnAddTypesUsed
//
//	Add types used by the sovereign.

	{
	int i;

	//	We only add explicit relationships

	if (m_pInitialRelationships)
		{
		for (i = 0; i < m_pInitialRelationships->GetContentElementCount(); i++)
			{
			CXMLElement *pRelDesc = m_pInitialRelationships->GetContentElement(i);
			CSovereign *pTarget = GetUniverse().FindSovereign(pRelDesc->GetAttributeInteger(SOVEREIGN_ATTRIB));
			if (pTarget)
				retTypesUsed->SetAt(pTarget->GetUNID(), true);
			}
		}
	}

ALERROR CSovereign::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design

	{
	InitRelationships();

	InitCachedEvents(CACHED_EVENT_COUNT, m_CACHED_EVENTS, m_CachedEvents);

	return NOERROR;
	}

ALERROR CSovereign::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML

	{
	//	Initialize

	m_sName = pDesc->GetAttribute(NAME_ATTRIB);
	m_bVirtual = pDesc->GetAttributeBool(VIRTUAL_ATTRIB);

	if (!pDesc->FindAttribute(SHORT_NAME_ATTRIB, &m_sShortName))
		m_sShortName = m_sName;

	if (!pDesc->FindAttribute(ADJECTIVE_ATTRIB, &m_sAdjective))
		m_sAdjective = m_sShortName;

	if (!pDesc->FindAttribute(DEMONYM_ATTRIB, &m_sDemonym))
		m_sDemonym = m_sAdjective;

	m_bPluralForm = pDesc->GetAttributeBool(PLURAL_ATTRIB);

	//	Alignment

	CString sAlignment = pDesc->GetAttribute(ALIGNMENT_ATTRIB);
	m_iAlignment = ParseAlignment(sAlignment);

	//	For backwards compatibility see if we have the old alignments

	if (m_iAlignment == alignUnknown)
		{
		if (strEquals(sAlignment, CONSTRUCTIVE_CHAOS_ALIGN))
			m_iAlignment = alignRepublic;
		else if (strEquals(sAlignment, CONSTRUCTIVE_ORDER_ALIGN))
			m_iAlignment = alignFederation;
		else if (strEquals(sAlignment, NEUTRAL_ALIGN))
			m_iAlignment = alignCompetitor;
		else if (strEquals(sAlignment, DESTRUCTIVE_ORDER_ALIGN))
			m_iAlignment = alignEmpire;
		else if (strEquals(sAlignment, DESTRUCTIVE_CHAOS_ALIGN))
			m_iAlignment = alignMegalomaniac;
		else if (m_bVirtual)
			m_iAlignment = alignUnorganized;
		else
			return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unknown alignment: %s"), sAlignment));
		}

	//	Load relationships

	m_pInitialRelationships = pDesc->GetContentElementByTag(RELATIONSHIPS_TAG);

	//	Done

	return NOERROR;
	}

void CSovereign::OnObjDestroyedByPlayer (const SDestroyCtx &Ctx)

//	OnObjDestroyedByPlayer
//
//	One of our objects (ship/station) was destroyed by the player.

	{
	if (Ctx.Obj.GetCategory() == CSpaceObject::catStation)
		{
		m_iStationsDestroyedByPlayer++;

		SEventHandlerDesc Event;
		if (FindEventHandlerSovereign(eventOnStationDestroyedByPlayer, &Event))
			{
			CCodeChainCtx CCX(GetUniverse());

			//	Setup

			CCX.DefineContainingType(this);
			CCX.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.Obj);
			CCX.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
			CCX.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
			CCX.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
			CCX.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
			CCX.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

			//	Execute

			ICCItemPtr pResult = CCX.RunCode(Event);

			//	Done

			if (pResult->IsError())
				ReportEventError(CONSTLIT("OnStationDestroyedByPlayer"), pResult);
			}
		}
	else
		{
		m_iShipsDestroyedByPlayer++;

		SEventHandlerDesc Event;
		if (FindEventHandlerSovereign(eventOnShipDestroyedByPlayer, &Event))
			{
			CCodeChainCtx CCX(GetUniverse());

			//	Setup

			CCX.DefineContainingType(this);
			CCX.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.Obj);
			CCX.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
			CCX.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
			CCX.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
			CCX.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
			CCX.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

			//	Execute

			ICCItemPtr pResult = CCX.RunCode(Event);

			//	Done

			if (pResult->IsError())
				ReportEventError(CONSTLIT("OnShipDestroyedByPlayer"), pResult);
			}
		}
	}

ALERROR CSovereign::OnPrepareBindDesign (SDesignLoadCtx &Ctx)

//	OnPrepareBindDesign
//
//	Delete all our relationships.

	{
	DeleteRelationships();

	return NOERROR;
	}

void CSovereign::OnPrepareReinit (void)

//	OnPrepareReinit
//
//	Delete all our relationships. We need to do this as a first pass
//	because sometimes we add relationships from a different sovereign's init
//	(because of "mutual")

	{
	DeleteRelationships();
	}

void CSovereign::OnReadFromStream (SUniverseLoadCtx &Ctx)

//	ReadFromStream
//
//	Read from stream
//
//	For each relationship:
//	DWORD		Unid of target relationship (or NULL if no more)
//	DWORD		Disposition
//	DWORD		Flags
//
//	DWORD		m_iStationsDestroyedByPlayer
//	DWORD		m_iShipsDestroyedByPlayer

	{
	DWORD dwUNID;
	DWORD dwLoad;

	DeleteRelationships();
	SRelationship **pNext = &m_pFirstRelationship;

	//	Keep reading until there are no more

	Ctx.pStream->Read((char *)&dwUNID, sizeof(DWORD));
	while (dwUNID)
		{
		SRelationship *pRel = new SRelationship;
		pRel->pSovereign = GetUniverse().FindSovereign(dwUNID);

		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		pRel->iDisp = (Disposition)dwLoad;

		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

		//	Add to list

		if (pRel->pSovereign)
			{
			(*pNext) = pRel;
			pNext = &pRel->pNext;
			}
		else
			delete pRel;

		//	Next

		Ctx.pStream->Read((char *)&dwUNID, sizeof(DWORD));
		}

	//	Terminate

	(*pNext) = NULL;

	m_bSelfRel = CalcSelfRel();

	//	Read some other data

	if (Ctx.dwVersion >= 28)
		{
		Ctx.pStream->Read((char *)&m_iStationsDestroyedByPlayer, sizeof(DWORD));
		Ctx.pStream->Read((char *)&m_iShipsDestroyedByPlayer, sizeof(DWORD));
		}
	}

void CSovereign::OnReinit (void)

//	OnReinit
//
//	Reinitialize

	{
	InitRelationships();

	m_iStationsDestroyedByPlayer = 0;
	m_iShipsDestroyedByPlayer = 0;
	}

void CSovereign::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Writes out the variable portions of the item type
//
//	For each relationship:
//	DWORD		Unid of target relationship (or NULL if no more)
//	DWORD		Disposition
//	DWORD		Flags
//
//	DWORD		m_iStationsDestroyedByPlayer
//	DWORD		m_iShipsDestroyedByPlayer

	{
	DWORD dwSave;

	SRelationship *pRel = m_pFirstRelationship;
	while (pRel)
		{
		dwSave = pRel->pSovereign->GetUNID();
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		dwSave = pRel->iDisp;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		//	Flags
		dwSave = 0;
		pStream->Write((char *)&dwSave, sizeof(DWORD));

		pRel = pRel->pNext;
		}

	//	Write out end

	dwSave = 0;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	//	Write some stats

	pStream->Write((char *)&m_iStationsDestroyedByPlayer, sizeof(DWORD));
	pStream->Write((char *)&m_iShipsDestroyedByPlayer, sizeof(DWORD));
	}

CSovereign::Alignments CSovereign::ParseAlignment (const CString &sAlign)

//	ParseAlignment
//
//	Parse the string into an alignment

	{
	int i;

	for (i = 0; i < alignCount; i++)
		if (strEquals(sAlign, CString(ALIGN_DATA[i].pszName, ALIGN_DATA[i].iNameLen, true)))
			return (Alignments)i;

	return alignUnknown;
	}

DWORD CSovereign::ParseAlignmentFlag (const CString &sValue)

//	ParseAlignmentFlag
//
//	Parse the string into an alignment flag

	{
	const TStaticStringEntry<DWORD> *pEntry = ALIGN_FLAG_TABLE.GetAt(sValue);
	if (pEntry == NULL)
		return 0;

	return pEntry->Value;
	}

CSovereign::Disposition CSovereign::ParseDisposition (const CString &sValue)

//	ParseDisposition
//
//	Parses a disposition.

	{
	if (strEquals(sValue, DISP_FRIEND))
		return dispFriend;
	else if (strEquals(sValue, DISP_NEUTRAL))
		return dispNeutral;
	else if (strEquals(sValue, DISP_ENEMY))
		return dispEnemy;
	else
		//	LATER: We should return an error code
		return dispNeutral;
	}

void CSovereign::SetDispositionTowards (CSovereign *pSovereign, Disposition iDisp, bool bMutual)

//	SetDispositionTowards
//
//	Sets the disposition towards the given sovereign

	{
	SRelationship *pRel = FindRelationship(pSovereign);
	if (pRel == NULL)
		{
		pRel = new SRelationship;
		pRel->pSovereign = pSovereign;
		pRel->pNext = m_pFirstRelationship;
		m_pFirstRelationship = pRel;
		}

	pRel->iDisp = iDisp;

	if (pSovereign == this)
		m_bSelfRel = true;

	//	Flush cache of enemy objects

	FlushEnemyObjectCache();

	//	If this is a mutual relationship, try to set it.

	if (bMutual && pSovereign != this)
		{
		Disposition iReverseDisp = pSovereign->GetDispositionTowards(this, false);
		if (iReverseDisp != iDisp)
			{
			//	Check to see if we already have an explicit relationship
			//	If we don't, then set the reverse relationship;
			//	If the reverse disposition is neutral, then we override
			//	If the reverse disposition is friendly, then we override if we're enemies

			SRelationship *pRel = pSovereign->FindRelationship(this);
			if (pRel == NULL
					|| iReverseDisp == dispNeutral
					|| (iReverseDisp == dispFriend && iDisp == dispEnemy))
				{
				pSovereign->SetDispositionTowards(this, iDisp, false);
				}
			}
		}
	}

void CSovereign::SetDispositionTowards (Alignments iAlignment, Disposition iDisp, bool bMutual)

//	SetDispositionTowards
//
//	Sets disposition towards all sovereigns of the given alignment

	{
	int i;

	for (i = 0; i < GetUniverse().GetSovereignCount(); i++)
		{
		CSovereign *pTarget = GetUniverse().GetSovereign(i);
		if (pTarget->GetAlignment() != iAlignment
				|| pTarget == this)
			continue;

		//	If we're already at this disposition then skip

		if (GetDispositionTowards(pTarget) == iDisp
				&& (!bMutual || pTarget->GetDispositionTowards(this) == iDisp))
			continue;

		//	Set disposition

		SetDispositionTowards(pTarget, iDisp, bMutual);
		}
	}

void CSovereign::SetDispositionTowardsFlag (DWORD dwAlignmentFlag, Disposition iDisp, bool bMutual)

//	SetDispositionTowardsFlag
//
//	Sets disposition towards all sovereigns that have the given flag.

	{
	for (int i = 0; i < GetUniverse().GetSovereignCount(); i++)
		{
		CSovereign *pTarget = GetUniverse().GetSovereign(i);
		Alignments iAlignment = pTarget->GetAlignment();
		if (iAlignment == alignUnknown
				|| pTarget == this
				|| !(ALIGN_DATA[iAlignment].dwFlags & dwAlignmentFlag))
			continue;

		//	If we're already at this disposition then skip

		if (GetDispositionTowards(pTarget) == iDisp
				&& (!bMutual || pTarget->GetDispositionTowards(this) == iDisp))
			continue;

		//	Set disposition

		SetDispositionTowards(pTarget, iDisp, bMutual);
		}
	}

bool CSovereign::SetPropertyInteger (const CString &sProperty, int iValue)

//	SetPropertyInteger
//
//	Sets an integer property

	{
	IPlayerController *pController;

	if (pController = GetController())
		return pController->SetPropertyInteger(sProperty, iValue);
	else
		return false;
	}

bool CSovereign::SetPropertyItemList (const CString &sProperty, const CItemList &ItemList)

//	SetPropertyItemList
//
//	Sets an item list property

	{
	IPlayerController *pController;

	if (pController = GetController())
		return pController->SetPropertyItemList(sProperty, ItemList);
	else
		return false;
	}

bool CSovereign::SetPropertyString (const CString &sProperty, const CString &sValue)

//	SetPropertyString
//
//	Sets a string property

	{
	IPlayerController *pController;

	if (pController = GetController())
		return pController->SetPropertyString(sProperty, sValue);
	else
		return false;
	}

void CSovereign::Update (int iTick, CSystem *pSystem)

//	Update
//
//	Updates the sovereign

	{
	//	Time to call OnUpdate?

	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnUpdate, &Event)
			&& (((DWORD)iTick + GetUNID()) % OBJECT_ON_UPDATE_CYCLE) == 0)
		{
		CCodeChainCtx Ctx(GetUniverse());

		//	Setup 

		Ctx.SaveAndDefineSovereignVar(this);

		//	Execute

		ICCItem *pResult = Ctx.Run(Event);

		//	Done

		if (pResult->IsError())
			ReportEventError(EVENT_ON_UPDATE, pResult);

		Ctx.Discard(pResult);
		}
	}

//	CSovereignRef --------------------------------------------------------------

ALERROR CSovereignRef::Bind (SDesignLoadCtx &Ctx)

//	Bind
//
//	Bind design

	{
	if (m_dwUNID)
		{
		if (ALERROR error = BindType(Ctx, m_dwUNID, m_pType))
			return error;

		//	Cannot reference virtual sovereigns

		if (m_pType->IsVirtual())
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Cannot refer to virtual sovereign: %08x"), m_dwUNID);
			return ERR_FAIL;
			}
		}

	return NOERROR;
	}

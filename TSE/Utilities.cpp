//	Utilities.cpp
//
//	Utility classes

#include "PreComp.h"
#include <math.h>

#define CLASS_ATTRIB						CONSTLIT("class")
#define DAMAGE_ADJ_ATTRIB					CONSTLIT("damageAdj")
#define HP_BONUS_ATTRIB						CONSTLIT("hpBonus")
#define SCALE_ATTRIB						CONSTLIT("scale")
#define SOVEREIGN_ATTRIB					CONSTLIT("sovereign")

#define FREQUENCY_COMMON					CONSTLIT("common")
#define FREQUENCY_UNCOMMON					CONSTLIT("uncommon")
#define FREQUENCY_RARE						CONSTLIT("rare")
#define FREQUENCY_VERYRARE					CONSTLIT("veryrare")
#define FREQUENCY_NOTRANDOM					CONSTLIT("notrandom")

#define ARMOR_COMPOSITION_METALLIC			CONSTLIT("metallic")
#define ARMOR_COMPOSITION_CERAMIC			CONSTLIT("ceramic")
#define ARMOR_COMPOSITION_CARBIDE			CONSTLIT("carbide")
#define ARMOR_COMPOSITION_NANOSCALE			CONSTLIT("nanoscale")
#define ARMOR_COMPOSITION_QUANTUM			CONSTLIT("quantum")
#define ARMOR_COMPOSITION_GRAVITIC			CONSTLIT("gravitic")
#define ARMOR_COMPOSITION_DARKMATTER		CONSTLIT("darkmatter")

#define VERSION_095							(CONSTLIT("0.95"))
#define VERSION_095A						(CONSTLIT("0.95a"))
#define VERSION_095B						(CONSTLIT("0.95b"))
#define VERSION_096							(CONSTLIT("0.96"))
#define VERSION_096A						(CONSTLIT("0.96a"))
#define VERSION_097							(CONSTLIT("0.97"))
#define VERSION_097A						(CONSTLIT("0.97a"))
#define VERSION_098							(CONSTLIT("0.98"))
#define VERSION_098A						(CONSTLIT("0.98a"))
#define VERSION_098B						(CONSTLIT("0.98b"))
#define VERSION_098C						(CONSTLIT("0.98c"))
#define VERSION_098D						(CONSTLIT("0.98d"))
#define VERSION_099							(CONSTLIT("0.99"))
#define VERSION_099A						(CONSTLIT("0.99a"))
#define VERSION_099B						(CONSTLIT("0.99b"))
#define VERSION_099C						(CONSTLIT("0.99c"))
#define VERSION_100							(CONSTLIT("1.0"))
#define VERSION_110							(CONSTLIT("1.1"))

#define INTER_SYSTEM_FOLLOW_PLAYER			CONSTLIT("followPlayer")
#define INTER_SYSTEM_WAIT_FOR_PLAYER		CONSTLIT("waitForPlayer")

#define SCALE_AU							CONSTLIT("AU")
#define SCALE_LIGHT_MINUTE					CONSTLIT("light-minute")
#define SCALE_LIGHT_SECOND					CONSTLIT("light-second")
#define SCALE_PIXEL							CONSTLIT("pixel")

#define STORAGE_DEVICE						CONSTLIT("local")
#define STORAGE_SERVICE_EXTENSION			CONSTLIT("serviceExtension")
#define STORAGE_SERVICE_USER				CONSTLIT("serviceUser")

struct SOrderTypeData
	{
	char *szName;
	char *szTarget;
	//	-		no target
	//	*		optional target
	//	o		required target

	char *szData;
	//	-		no data
	//	i		integer (may be optional)
	//	2		two integers (encoded in a DWORD)
	//  s		string data
	//	v		vector data

	DWORD dwFlags;
	};

static const SOrderTypeData g_OrderTypes[] =
	{
		//	name						target	data
		{	"",							"-",	"-",	0	 },

		{	"guard",					"o",	"-",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED },
		{	"dock",						"o",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"attack",					"o",	"i",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP | ORDER_FLAG_DELETE_ON_OLD_SHIP_WAITS },
		{	"wait",						"-",	"i",	0 },
		{	"gate",						"*",	"-",	0 },

		{	"gateOnThreat",				"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"gateOnStationDestroyed",	"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"patrol",					"o",	"i",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP	},
		{	"escort",					"o",	"2",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP	},
		{	"scavenge",					"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },

		{	"followPlayerThroughGate",	"o",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },
		{	"attackNearestEnemy",		"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"tradeRoute",				"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"wander",					"-",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"loot",						"o",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },

		{	"hold",						"-",	"i",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"mine",						"o",	"-",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED | ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"waitForPlayer",			"-",	"-",	0 },
		{	"attackPlayerOnReturn",		"-",	"-",	0 },
		{	"follow",					"o",	"-",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },

		{	"navPath",					"-",	"i",	0	},
		{	"goto",						"o",	"-",	0 },
		{	"waitForTarget",			"o",	"2",	ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },
		{	"waitForEnemy",				"-",	"i",	0 },
		{	"bombard",					"o",	"i",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED },

		{	"approach",					"o",	"i",	ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },
		{	"aim",						"o",	"-",	ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },
		{	"orbit",					"o",	"2",	ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP },
		{	"holdCourse",				"-",	"2",	0 },
		{	"turnTo",					"-",	"i",	0 },

		{	"attackHold",				"o",	"i",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP | ORDER_FLAG_DELETE_ON_OLD_SHIP_WAITS },
		{	"attackStation",			"o",	"i",	ORDER_FLAG_DELETE_ON_STATION_DESTROYED },
		{	"fireEvent",				"o",	"s",	0 },
		{	"waitForUndock",			"o",	"i",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"sendMessage",				"o",	"s",	0 },

		{	"attackArea",				"o",	"2",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
		{	"holdAndAttack",			"o",	"i",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED | ORDER_FLAG_UPDATE_ON_NEW_PLAYER_SHIP | ORDER_FLAG_DELETE_ON_OLD_SHIP_WAITS },
		{	"gotoPos",					"-",	"v",	0 },
		{	"waitForThreat",			"-",	"i",	0 },
		{	"sentry",					"*",	"i",	ORDER_FLAG_NOTIFY_ON_STATION_DESTROYED },
	};

#define ORDER_TYPES_COUNT		(sizeof(g_OrderTypes) / sizeof(g_OrderTypes[0]))

static char *g_pszDestructionCauses[killedCount] =
	{
	"removedFromSystem",

	"damage",
	"runningOutOfFuel",
	"radiationPoisoning",
	"self",
	"disintegration",

	"weaponMalfunction",
	"ejecta",
	"explosion",
	"shatter",
	"playerCreatedExplosion",

	"enteredStargate",
	"custom",
	"gravity",
	"powerFailure",
	"ascended",
	};

#define DESTRUCTION_CAUSES_COUNT	(sizeof(g_pszDestructionCauses) / sizeof(g_pszDestructionCauses[0]))

static char *g_pszDamageResults[damageResultCount] =
	{
	"noDamage",
	"absorbedByShields",
	"armorHit",
	"structuralHit",
	"destroyed",
	"passthrough",
	"passthroughDestroyed",
	"destroyedAbandoned",
	"noDamageNoPassthrough",
	"disintegrated",
	"shattered",
	};

static char *g_pszMessageID[] =
	{
	"",							//	0
	"AttackTarget",				//	msgAttack
	"AttackTargetBroadcast",	//	msgDestroyBroadcast
	"HitByFriendlyFire",		//	msgHitByFriendlyFire
	"QueryEscortStatus",		//	msgQueryEscortStatus
	"QueryFleetStatus",			//	msgQueryFleetStatus
	"EscortAttacked",			//	msgEscortAttacked
	"EscortReportingIn",		//	msgEscortReportingIn
	"WatchYourTargets",			//	msgWatchTargets
	"NiceShooting",				//	msgNiceShooting
	"FormUp",					//	msgFormUp
	"BreakAndAttack",			//	msgBreakAndAttack
	"QueryComms",				//	msgQueryCommunications
	"AbortAttack",				//	msgAbortAttack
	"Wait",						//	msgWait
	"QueryWaitStatus",			//	msgQueryWaitStatus
	"AttackInFormation",		//	msgAttackInFormation
	"DeterTarget",				//	msgAttackDeter
	"QueryAttackStatus",		//	msgQueryAttackStatus
	"DockingSequenceEngaged",	//	msgDockingSequenceEngaged
	"HitByHostileFire",			//	msgHitByHostileFire
	"DestroyedByFriendlyFire",	//	msgDestroyedByFriendlyFire
	"DestroyedByHostileFire",	//	msgDestroyedByHostileFire
	"BaseDestroyedByTarget",	//	msgBaseDestroyedByTarget
	};

#define MESSAGE_ID_COUNT			(sizeof(g_pszMessageID) / sizeof(g_pszMessageID[0]))

struct SGenomeData
	{
	char *pszID;
	char *pszName;
	GenomeTypes iType;
	};

static SGenomeData g_Genome[] =
	{
		{	"",						"",				genomeUnknown		},	//	genomeUnknown
		{	"humanMale",			"human male",	genomeHumanMale		},	//	genomeMale
		{	"humanFemale",			"human female",	genomeHumanFemale	},	//	genomeFemale
		{	"genderNeuter",			"neuter",		genderNeuter		},	//	genderNeuter

		{	"genderMale",			"male",			genomeHumanMale		},	//	genomeMale
		{	"genderFemale",			"female",		genomeHumanFemale	},	//	genomeFemale
		{	"genderUnspecified",	"unspecified",	genomeUnknown		},	//	genomeUnknown
	};

#define GENOME_COUNT				(sizeof(g_Genome) / sizeof(g_Genome[0]))

static char *LOAD_STATE_STRINGS[] =
	{
	"Unknown",
	"Loading object",
	"Loading object data",
	"Loading object effects",
	"Loading object subclass",
	"Loading effect",
	};

static Metric g_pDestinyToBellCurve[g_DestinyRange];
static bool g_bDestinyToBellCurveInit = false;

//	CDiceRange ----------------------------------------------------------------

DiceRange::DiceRange (int iFaces, int iCount, int iBonus) :
		m_iFaces(iFaces),
		m_iCount(iCount),
		m_iBonus(iBonus)

//	CDiceRange constructor

	{
	}

ALERROR DiceRange::LoadFromXML (const CString &sAttrib, int iDefault, CString *retsSuffix)

//	LoadFromXML
//
//	Loads from an XML attribute

	{
	bool bNullValue;
	char *pPos = sAttrib.GetASCIIZPointer();

	//	If empty, then default to 0

	if (*pPos == '\0')
		{
		m_iCount = 0;
		m_iFaces = 0;
		m_iBonus = iDefault;
		if (retsSuffix)
			*retsSuffix = NULL_STR;
		return NOERROR;
		}

	//	First is the number of dice

	int iCount = strParseInt(pPos, 0, &pPos, &bNullValue);
	if (bNullValue)
		return ERR_FAIL;

	//	If we've got a 'd' then we have a dice pattern

	if (*pPos == 'd')
		{
		pPos++;
		m_iCount = iCount;
		if (iCount < 0)
			return ERR_FAIL;

		//	Now parse the sides

		m_iFaces = strParseInt(pPos, -1, &pPos, NULL);
		if (m_iFaces == -1)
			return ERR_FAIL;

		//	Finally, add any bonus

		if (*pPos != '\0')
			m_iBonus = strParseInt(pPos, 0, &pPos, NULL);
		else
			m_iBonus = 0;
		}

	//	If we've got a '-' then we have a range pattern

	else if (*pPos == '-')
		{
		pPos++;
		int iEnd = strParseInt(pPos, 0, &pPos, &bNullValue);
		if (bNullValue)
			return ERR_FAIL;

		if (iEnd < iCount)
			Swap(iEnd, iCount);

		m_iCount = 1;
		m_iFaces = (iEnd - iCount) + 1;
		m_iBonus = iCount - 1;
		}

	//	Otherwise, we've got a constant number

	else
		{
		m_iCount = 0;
		m_iFaces = 0;
		m_iBonus = iCount;
		}

	//	Check trailing suffix

	if (retsSuffix)
		{
		//	Skip whitespace

		while (::strIsWhitespace(pPos))
			pPos++;

		char *pStart = pPos;

		//	Skip whitespace at the end

		while (*pPos != '\0' && !::strIsWhitespace(pPos))
			pPos++;

		//	Done

		*retsSuffix = CString(pStart, pPos - pStart);
		}

	return NOERROR;
	}

bool DiceRange::LoadIfValid (const CString &sAttrib, DiceRange *retValue)

//	LoadIfValid
//
//	Tries to parse sAttrib as a dice range. If successful, we return TRUE.

	{
	bool bNullValue;
	char *pPos = sAttrib.GetASCIIZPointer();

	//	If empty, then not valid

	if (*pPos == '\0'
			|| (*pPos != '-' && (*pPos < '0' || *pPos > '9')))
		return false;

	//	First is the number of dice

	int iCount = strParseInt(pPos, 0, &pPos, &bNullValue);
	int iFaces;
	int iBonus;
	if (bNullValue)
		return false;

	//	If we've got a 'd' then we have a dice pattern

	if (*pPos == 'd')
		{
		pPos++;
		if (iCount < 0)
			return false;

		//	Now parse the sides

		iFaces = strParseInt(pPos, -1, &pPos, NULL);
		if (iFaces == -1)
			return false;

		//	Finally, add any bonus

		if (*pPos != '\0')
			iBonus = strParseInt(pPos, 0, &pPos, NULL);
		else
			iBonus = 0;
		}

	//	If we've got a '-' then we have a range pattern

	else if (*pPos == '-')
		{
		pPos++;
		int iEnd = strParseInt(pPos, 0, &pPos, &bNullValue);
		if (bNullValue)
			return false;

		if (iEnd < iCount)
			Swap(iEnd, iCount);

		iFaces = (iEnd - iCount) + 1;
		iBonus = iCount - 1;
		iCount = 1;
		}

	//	If we're at the end, then this is a constant number

	else if (*pPos == '\0')
		{
		iBonus = iCount;
		iCount = 0;
		iFaces = 0;
		}

	//	Otherwise, not a dice range

	else
		return false;

	if (retValue)
		*retValue = DiceRange(iFaces, iCount, iBonus);

	return true;
	}

void DiceRange::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Read the range

	{
	if (Ctx.dwVersion >= 27)
		{
		Ctx.pStream->Read((char *)&m_iFaces, sizeof(DWORD));
		Ctx.pStream->Read((char *)&m_iCount, sizeof(DWORD));
		Ctx.pStream->Read((char *)&m_iBonus, sizeof(DWORD));
		}
	else
		{
		DWORD dwLoad;
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));

		m_iFaces = HIBYTE(LOWORD(dwLoad));
		m_iCount = LOBYTE(LOWORD(dwLoad));
		m_iBonus = (int)(short)HIWORD(dwLoad);
		}
	}

int DiceRange::Roll (void) const

//	Roll
//
//	Generate a random number

	{
	int iRoll = 0;

	for (int i = 0; i < m_iCount; i++)
		iRoll += mathRandom(1, m_iFaces);

	return iRoll + m_iBonus;
	}

int DiceRange::RollSeeded (int iSeed) const

//	RollSeeded
//
//	Generate a random number

	{
	int iRoll = 0;

	for (int i = 0; i < m_iCount; i++)
		iRoll += mathSeededRandom(iSeed, 1, m_iFaces);

	return iRoll + m_iBonus;
	}

CString DiceRange::SaveToXML (void) const

//	SaveToXML
//
//	Represents range as an attribute value

	{
	if (m_iCount > 0 && m_iFaces > 0)
		{
		if (m_iBonus > 0)
			return strPatternSubst("%dd%d+%d", m_iCount, m_iFaces, m_iBonus);
		else if (m_iBonus < 0)
			return strPatternSubst("%dd%d-%d", m_iCount, m_iFaces, -m_iBonus);
		else
			return strPatternSubst("%dd%d", m_iCount, m_iFaces);
		}
	else
		return strFromInt(m_iBonus);
	}

void DiceRange::Scale (Metric rScale)

//	Scale
//
//	Adjust values by given factor

	{
	if (m_iCount == 0)
		m_iBonus = (int)(m_iBonus * rScale + 0.5);
	else if (m_iCount == 1)
		{
		Metric rNewMid = (m_iBonus + (m_iFaces + 1) / 2.0) * rScale;

		m_iFaces = (int)(m_iFaces * rScale + 0.5);
		m_iBonus = (int)((rNewMid - (m_iFaces + 1) / 2.0) + 0.5);
		}
	else
		{
		m_iCount = (int)(m_iCount * rScale + 0.5);
		m_iBonus = (int)(m_iBonus * rScale + 0.5);
		}
	}

void DiceRange::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Writes the range

	{
	pStream->Write((char *)&m_iFaces, sizeof(DWORD));
	pStream->Write((char *)&m_iCount, sizeof(DWORD));
	pStream->Write((char *)&m_iBonus, sizeof(DWORD));
	}

//	Miscellaneous functions

CString AppendModifiers (const CString &sModifierList1, const CString &sModifierList2)

//	AppendModifiers
//
//	Returns a concatenation of the two modifier lists

	{
	if (sModifierList1.IsBlank())
		return sModifierList2;
	else if (sModifierList2.IsBlank())
		return sModifierList1;
	else
		{
		CString sList1 = strTrimWhitespace(sModifierList1);
		if (sList1.IsBlank())
			return sModifierList2;

		CString sList2 = strTrimWhitespace(sModifierList2);
		if (sList2.IsBlank())
			return sModifierList1;

		char *pLastChar = (sList1.GetASCIIZPointer() + sList1.GetLength() - 1);
		if (*pLastChar != ',' && *pLastChar != ';')
			sList1.Append(CONSTLIT(", "));

		sList1.Append(sList2);

		return sList1;
		}
	}

void AppendReferenceString (CString *iosReference, const CString &sString)
	{
	if (sString.IsBlank())
		NULL;
	else if (iosReference->IsBlank())
		*iosReference = sString;
	else
		{
		iosReference->Append(CONSTLIT(" — "));
		iosReference->Append(sString);
		}
	}

int CalcEffectiveHP (int iLevel, int iHP, int *iHPbyDamageType)

//	CalcEffectiveHP
//
//	Calculates the effective HP for the armor/shields, taking into account damage type adj

	{
	int i;
	const int WEIGHT_COUNT = 5;
	const Metric WEIGHT_DIST[WEIGHT_COUNT] = { 0.84, 0.65, 0.42, 0.21, 0.08 };

	//	Add all the values together, adjusting for how common a given
	//	damage type is at a particular level

	Metric rTotalHP = 0.0;
	Metric rTotalWeight = 0.0;
	for (i = 0; i < damageCount; i++)
		{
		int iMidLevel = GetDamageTypeLevel((DamageTypes)i) + 1;
		int iDiff = Absolute(iMidLevel - iLevel);
		int iHPatLevel = iHPbyDamageType[i];
		if (iHPatLevel == -1)
			iHPatLevel = iHP * 10;

		if (iDiff < WEIGHT_COUNT)
			{
			rTotalHP += iHPatLevel * WEIGHT_DIST[iDiff];
			rTotalWeight += WEIGHT_DIST[iDiff];
			}
		}

	//	Done

	if (rTotalWeight > 0.0)
		return (int)((rTotalHP / rTotalWeight) + 0.5);
	else
		return 0;
	}

Metric CalcLevelDiffStrength (int iDiff)

//	CalcLevelDiffStrength
//
//	Returns the relative strength based on a level difference.

	{
	switch (iDiff)
		{
		case -2:
			return 0.3125;

		case -1:
			return 0.625;

		case 0:
			return 1.0;

		case +1:
			return 1.667;

		case +2:
			return 2.5;

		default:
			return (iDiff > 0 ? 4.0 : 0.25);
		}
	}

EManeuverTypes CalcTurnManeuver (int iDesired, int iCurrent, int iRotationAngle)

//	CalcTurnManeuver
//
//	Calculates the turn maneuver required to face the given direction

	{
	int iTurn = (iDesired + 360 - iCurrent) % 360;

	if ((iTurn >= (360 - (iRotationAngle / 2)))
			|| (iTurn <= (iRotationAngle / 2)))
		return NoRotation;
	else
		{
		if (iTurn >= 180)
			return RotateRight;
		else
			return RotateLeft;
		}
	}

CString ComposeDamageAdjReference (int *AdjRow, int *StdRow)

//	ComposeDamageAdjReference
//
//	Compose a string that describes the damage adjustments

	{
	CString sResult;
	CString sVulnerable;
	CString sResistant;
	CString sImmune;
	int iVulnerableCount = 0;
	int iResistantCount = 0;
	int iImmuneCount = 0;
	int iDamage;
	bool bInvertVulnerable = false;
	bool bInvertResistant = false;
	bool bInvertImmune = false;

	//	Compute immunities and vulnerabilities

	for (iDamage = 0; iDamage < damageCount; iDamage++)
		{
		int iAdj = AdjRow[iDamage];
		int iStd = StdRow[iDamage];

		if (iAdj != iStd)
			{
			//	Append

			if (iAdj > iStd)
				{
				if (sVulnerable.IsBlank())
					sVulnerable = strPatternSubst(CONSTLIT("%s"), GetDamageShortName((DamageTypes)iDamage));
				else
					sVulnerable.Append(strPatternSubst(CONSTLIT(", %s"), GetDamageShortName((DamageTypes)iDamage)));

				iVulnerableCount++;
				}
			else if (iAdj < 20)
				{
				if (sImmune.IsBlank())
					sImmune = strPatternSubst(CONSTLIT("%s"), GetDamageShortName((DamageTypes)iDamage));
				else
					sImmune.Append(strPatternSubst(CONSTLIT(", %s"), GetDamageShortName((DamageTypes)iDamage)));

				iImmuneCount++;
				}
			else
				{
				if (sResistant.IsBlank())
					sResistant = strPatternSubst(CONSTLIT("%s"), GetDamageShortName((DamageTypes)iDamage));
				else
					sResistant.Append(strPatternSubst(CONSTLIT(", %s"), GetDamageShortName((DamageTypes)iDamage)));

				iResistantCount++;
				}
			}
		}

	//	If we've got a lot of immunities, then reverse the sense 
	//	"immune to all except:..."

	if (iImmuneCount > (damageCount - 3) && iVulnerableCount == 0 && iResistantCount == 0)
		{
		sImmune = NULL_STR;
		bInvertImmune = true;

		for (iDamage = 0; iDamage < damageCount; iDamage++)
			{
			int iAdj = AdjRow[iDamage];
			int iStd = StdRow[iDamage];

			if (iAdj == iStd)
				{
				if (sImmune.IsBlank())
					sImmune = strPatternSubst(CONSTLIT("%s"), GetDamageShortName((DamageTypes)iDamage));
				else
					sImmune.Append(strPatternSubst(CONSTLIT(", %s"), GetDamageShortName((DamageTypes)iDamage)));
				}
			}
		}

	//	If we've got a lot of resistance, then reverse the sense 
	//	"resistant to all except:..."

	if (iResistantCount > (damageCount - 3) && iImmuneCount == 0 && iVulnerableCount == 0)
		{
		sResistant = NULL_STR;
		bInvertResistant = true;

		for (iDamage = 0; iDamage < damageCount; iDamage++)
			{
			int iAdj = AdjRow[iDamage];
			int iStd = StdRow[iDamage];

			if (iAdj == iStd)
				{
				if (sResistant.IsBlank())
					sResistant = strPatternSubst(CONSTLIT("%s"), GetDamageShortName((DamageTypes)iDamage));
				else
					sResistant.Append(strPatternSubst(CONSTLIT(", %s"), GetDamageShortName((DamageTypes)iDamage)));
				}
			}
		}

	//	If we've got a lot of vulnerabilities, then reverse the sense 
	//	"vulnerable to all except:..."

	if (iVulnerableCount > (damageCount - 3) && iImmuneCount == 0 && iResistantCount == 0)
		{
		sVulnerable = NULL_STR;
		bInvertVulnerable = true;

		for (iDamage = 0; iDamage < damageCount; iDamage++)
			{
			int iAdj = AdjRow[iDamage];
			int iStd = StdRow[iDamage];

			if (iAdj == iStd)
				{
				if (sVulnerable.IsBlank())
					sVulnerable = strPatternSubst(CONSTLIT("%s"), GetDamageShortName((DamageTypes)iDamage));
				else
					sVulnerable.Append(strPatternSubst(CONSTLIT(", %s"), GetDamageShortName((DamageTypes)iDamage)));
				}
			}
		}

	//	Normal immunities

	if (!sImmune.IsBlank())
		{
		if (iImmuneCount == damageCount)
			sResult.Append(CONSTLIT("; immune to all damage"));
		else if (iImmuneCount == 1)
			sResult.Append(strPatternSubst(CONSTLIT("; %s-immune"), sImmune));
		else if (bInvertImmune)
			sResult.Append(strPatternSubst(CONSTLIT("; immune to all except: %s"), sImmune));
		else
			sResult.Append(strPatternSubst(CONSTLIT("; immune to: %s"), sImmune));
		}

	if (!sResistant.IsBlank())
		{
		if (iResistantCount == damageCount)
			sResult.Append(CONSTLIT("; resistant to all damage"));
		else if (iResistantCount == 1)
			sResult.Append(strPatternSubst(CONSTLIT("; %s-resistant"), sResistant));
		else if (iResistantCount + iImmuneCount == damageCount)
			sResult.Append(CONSTLIT("; resistant to all other damage"));
		else if (bInvertResistant)
			sResult.Append(strPatternSubst(CONSTLIT("; resistant to all except: %s"), sResistant));
		else
			sResult.Append(strPatternSubst(CONSTLIT("; resistant to: %s"), sResistant));
		}

	if (!sVulnerable.IsBlank())
		{
		if (iVulnerableCount == damageCount)
			sResult.Append(CONSTLIT("; vulnerable to all damage"));
		else if (iVulnerableCount == 1)
			sResult.Append(strPatternSubst(CONSTLIT("; %s-vulnerable"), sVulnerable));
		else if (iVulnerableCount + iResistantCount + iImmuneCount == damageCount)
			sResult.Append(CONSTLIT("; vulnerable to all other damage"));
		else if (bInvertVulnerable)
			sResult.Append(strPatternSubst(CONSTLIT("; vulnerable to all except: %s"), sVulnerable));
		else
			sResult.Append(strPatternSubst(CONSTLIT("; vulnerable to: %s"), sVulnerable));
		}

	return sResult;
	}

DWORD ExtensionVersionToInteger (DWORD dwVersion)

//	ExtensionVersionToInteger
//
//	NOTE: Prior to version 12 we tied version numbers to releases.
//	This function is no longer used, since we return api version number 
//	directly.

	{
	switch (dwVersion)
		{
		//	Version 1.1
		case 3:
			return 11;

		//	Version 1.0
		case 2:
			return 10;

		//	Prior to 1.0
		case 1:
		case 0:
			return 9;

		//	Must handle all cases
		default:
			return 0;
		}
	}

CString GenerateLevelFrequency (const CString &sLevelFrequency, int iCenterLevel)

//	GenerateLevelFrequency
//
//	Parses a special level frequency value of the form:
//
//	systemLevel:bbb|c|aaa
//
//	Where c is the frequency at iCenterLevel; bbb is the frequency before that 
//	level, and aaa is the frequency after that level.
//
//	For example, if iCenterLevel == 5 then:
//
//	"systemLevel:u|c|r"		->		"---uc r"

	{
	char *pPos = sLevelFrequency.GetASCIIZPointer();

	//	Skip to the colon

	while (*pPos != '\0' && *pPos != ':')
		pPos++;

	if (*pPos != ':')
		return NULL_STR;

	//	systemLevel: u | c | r
	//	           ^

	pPos++;
	while (*pPos == ' ')
		pPos++;

	//	systemLevel: u | c | r
	//	             ^

	char *pBelow = pPos;

	//	Count until the separator

	while (*pPos != '\0' && *pPos != '|' && *pPos != ' ')
		pPos++;

	//	systemLevel: u | c | r
	//	              ^

	int iBelowCount = (int)(pPos - pBelow);

	//	Find the separator

	while (*pPos == ' ')
		pPos++;

	if (*pPos != '|')
		return NULL_STR;

	//	systemLevel: u | c | r
	//	               ^

	pPos++;
	while (*pPos == ' ')
		pPos++;

	//	systemLevel: u | c | r
	//	                 ^

	char *pCenter = pPos;

	//	Count until the separator

	while (*pPos != '\0' && *pPos != '|' && *pPos != ' ')
		pPos++;

	//	systemLevel: u | c | r
	//	                  ^

	int iCenterCount = (int)(pPos - pCenter);
	if (iCenterCount == 0)
		return NULL_STR;

	//	Find the separator

	while (*pPos == ' ')
		pPos++;

	if (*pPos != '|')
		return NULL_STR;

	//	systemLevel: u | c | r
	//	                   ^

	pPos++;
	while (*pPos == ' ')
		pPos++;

	//	systemLevel: u | c | r
	//	                     ^

	char *pAbove = pPos;

	//	Count until the separator

	while (*pPos != '\0' && *pPos != '|' && *pPos != ' ')
		pPos++;

	//	systemLevel: u | c | r
	//	                      ^

	int iAboveCount = (int)(pPos - pAbove);

	//	Figure out the maximum level that we have

	int iMaxLevel = iCenterLevel + iAboveCount;
	int iStringSize = iMaxLevel + ((iMaxLevel - 1) / 5);
	CString sResult;
	char *pDest = sResult.GetWritePointer(iStringSize);

	//	Skip levels below 1

	while (iCenterLevel - iBelowCount < 1)
		{
		pBelow++;
		iBelowCount--;
		}

	//	Write out all the levels

	int iCurLevel = 1;
	while (iCurLevel <= iMaxLevel)
		{
		if (iCurLevel < iCenterLevel)
			{
			if (iCurLevel < iCenterLevel - iBelowCount)
				*pDest++ = '-';
			else
				*pDest++ = *pBelow++;
			}
		else if (iCurLevel == iCenterLevel)
			*pDest++ = *pCenter;
		else
			*pDest++ = *pAbove++;

		//	Add a spacing

		if ((iCurLevel % 5) == 0)
			*pDest++ = ' ';

		//	Next

		iCurLevel++;
		}

	//	Done

	return sResult;
	}

CString GenerateRandomName (const CString &sList, const CString &sSubst)

//	GenerateRandomName
//
//	Generates a random name based on a list

	{
	int iCount = strDelimitCount(sList, ';', DELIMIT_TRIM_WHITESPACE);
	CString sName = strDelimitGet(sList, ';', DELIMIT_TRIM_WHITESPACE, mathRandom(0, iCount-1));
	return GenerateRandomNameFromTemplate(sName, sSubst);
	}

CString GenerateRandomNameFromTemplate (const CString &sName, const CString &sSubst)

//	GenerateRandomNameFromTemplate
//
//	Generates a random name by appropriate substitution

	{
	//	Substitute string

	char szResult[1024];
	char *pDest = szResult;
	char *pPos = sName.GetASCIIZPointer();

	while (*pPos)
		{
		switch (*pPos)
			{
			case '%':
				{
				pPos++;
				switch (*pPos)
					{
					case '%':
						*pDest++ = *pPos++;
						break;

					case 's':
						{
						char *pCopy = sSubst.GetASCIIZPointer();
						while (*pCopy)
							*pDest++ = *pCopy++;

						pPos++;
						break;
						}

					case '0':
						*pDest++ = '0' + mathRandom(0, 9);
						pPos++;
						break;

					case '1':
						*pDest++ = '1' + mathRandom(0, 8);
						pPos++;
						break;

					case 'A':
						*pDest++ = 'A' + mathRandom(0, 25);
						pPos++;
						break;

					case 'a':
						*pDest++ = 'a' + mathRandom(0, 25);
						pPos++;
						break;
					}

				break;
				}

			default:
				*pDest++ = *pPos++;
			}
		}

	//	Done

	*pDest = '\0';
	return CString(szResult);
	}

int GetDamageTypeLevel (DamageTypes iDamage)

//	GetDamageTypeLevel
//
//	Returns the level for the given damage type
//
//	laser/particle =		1
//	particle/blast =		4
//	ion/thermo =			7
//	etc.

	{
	return 1 + 3 * ((int)iDamage / 2);
	}

CString GetDamageResultsName (EDamageResults iResult)

//	GetDamageResultsName
//
//	Returns the name of the damage result

	{
	if (iResult < damageResultCount)
		return CString(g_pszDamageResults[iResult]);
	else
		{
		ASSERT(false);
		return NULL_STR;
		}
	}

Metric *GetDestinyToBellCurveArray (void)

//	GetDestinyToBellCurveArray
//
//	Returns an array that maps a destiny value (0-359) to a random
//	number from -1.0 to 1.0 (with most of the values clustered around 0.0
//
//	We use an approximation of a cumulative distribution function
//	http://en.wikipedia.org/wiki/Error_function

	{
	int i;

	if (!g_bDestinyToBellCurveInit)
		{
		//	Some constants that make things work out to the right range

		Metric rMagicConstant = 0.147f;		//	From the approximation formula
		Metric rScale1 = 360.0f;
		Metric rScale2 = 3.7f;
		Metric r4OverPi = 4.0f / PI;

		for (i = 0; i < g_DestinyRange; i++)
			{
			Metric rX = ((Metric)i - ((Metric)(g_DestinyRange - 1) / 2.0f)) / rScale1;
			Metric rX2 = rX * rX;
			Metric raX2 = rMagicConstant * rX2;
			Metric rSign = (rX > 0.0f ? 1.0f : -1.0f);

			g_pDestinyToBellCurve[i] = (rSign * (1.0f - exp(-rX2 * (r4OverPi + raX2) / (1.0f + raX2)))) * rScale2;
			}

		g_bDestinyToBellCurveInit = true;
		}

	return g_pDestinyToBellCurve;
	}

DestructionTypes GetDestructionCause (const CString &sString)

//	GetDestructionCause
//
//	Loads a destruction cause

	{
	int i;

	for (i = 0; i < DESTRUCTION_CAUSES_COUNT; i++)
		if (strEquals(sString, CString(g_pszDestructionCauses[i])))
			return (DestructionTypes)i;

	return killedNone;
	}

CString GetDestructionName (DestructionTypes iCause)

//	GetDestructionName
//
//	Returns the name of the destruction cause

	{
	if (iCause < DESTRUCTION_CAUSES_COUNT)
		return CString(g_pszDestructionCauses[iCause]);
	else
		{
		ASSERT(false);
		return NULL_STR;
		}
	}

int GetDiceCountFromAttribute(const CString &sValue)

//	GetDiceCountFromAttribute
//
//	If sValue is blank, return 1.
//	If sValue is a number, returns the number.
//	Otherwise, assumes that sValue is a dice range and
//	returns the random value.

	{
	if (sValue.IsBlank())
		return 1;
	else
		{
		DiceRange Count;
		if (Count.LoadFromXML(sValue) != NOERROR)
			return 0;

		return Count.Roll();
		}
	}

int GetFrequency (const CString &sValue)

//	GetFrequency
//
//	Returns the frequency from an attribute value

	{
	if (strEquals(sValue, FREQUENCY_COMMON))
		return ftCommon;
	else if (strEquals(sValue, FREQUENCY_UNCOMMON))
		return ftUncommon;
	else if (strEquals(sValue, FREQUENCY_RARE))
		return ftRare;
	else if (strEquals(sValue, FREQUENCY_VERYRARE))
		return ftVeryRare;
	else
		return ftNotRandom;
	}

CString GetFrequencyName (FrequencyTypes iFrequency)

//	GetFrequencyName
//
//	Returns the frequency

	{
	switch (iFrequency)
		{
		case ftCommon:
			return FREQUENCY_COMMON;

		case ftUncommon:
			return FREQUENCY_UNCOMMON;

		case ftRare:
			return FREQUENCY_RARE;

		case ftVeryRare:
			return FREQUENCY_VERYRARE;

		default:
			return FREQUENCY_NOTRANDOM;
		}
	}

int GetFrequencyByLevel (const CString &sLevelFrequency, int iLevel)

//	GetFrequencyByLevel
//
//	Returns the frequency for a given level. The frequency is encoded in
//	a string as follows:
//
//	--cur v---- ----- ----- -----
//
//	Where:
//
//	-	Not encountered
//	c	common
//	u	uncommon
//	r	rare
//	v	very rare
//
//	The string is arranged in five sections of five characters each (for
//	human legibility).

	{
	char *pTable = sLevelFrequency.GetPointer();
	if (pTable && pTable[0] == '*')
		return ftCommon;

	ASSERT(iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL);
	int iChar = iLevel + ((iLevel - 1) / 5) - 1;
	if (iChar >= sLevelFrequency.GetLength())
		return ftNotRandom;

	switch (pTable[iChar])
		{
		case 'c':
		case 'C':
			return ftCommon;

		case 'u':
		case 'U':
			return ftUncommon;

		case 'r':
		case 'R':
			return ftRare;

		case 'v':
		case 'V':
			return ftVeryRare;

		default:
			return ftNotRandom;
		}
	}

CString GetGenomeID (GenomeTypes iGenome)

//	GetGenomeID
//
//	Returns the genome id

	{
	if ((int)iGenome < 0 || (int)iGenome >= GENOME_COUNT)
		return NULL_STR;

	return CString(g_Genome[iGenome].pszID);
	}

CString GetGenomeName (GenomeTypes iGenome)

//	GetGenomeName
//
//	Returns the genome name

	{
	if ((int)iGenome < 0 || (int)iGenome >= GENOME_COUNT)
		return NULL_STR;

	return CString(g_Genome[iGenome].pszName);
	}

Metric GetScale (CXMLElement *pObj)

//	GetScale
//
//	Returns the scale for this element

	{
	CString sScale = pObj->GetAttribute(SCALE_ATTRIB);
	if (sScale.IsBlank())
		return LIGHT_SECOND;
	else if (strEquals(sScale, SCALE_AU))
		return g_AU;
	else if (strEquals(sScale, SCALE_LIGHT_MINUTE))
		return LIGHT_MINUTE;
	else if (strEquals(sScale, SCALE_PIXEL))
		return g_KlicksPerPixel;
	else
		return LIGHT_SECOND;
	}

CSpaceObject::InterSystemResults GetInterSystemResult (const CString &sString)

//	GetInterSystemResult
//
//	Returns an InterSystemResult from a string

	{
	if (strEquals(sString, INTER_SYSTEM_FOLLOW_PLAYER))
		return CSpaceObject::interFollowPlayer;
	else if (strEquals(sString, INTER_SYSTEM_WAIT_FOR_PLAYER))
		return CSpaceObject::interWaitForPlayer;
	else
		return CSpaceObject::interNoAction;
	}

CString GetItemCategoryID (ItemCategories iCategory)

//	GetItemCategoryID
//
//	Returns the category name

	{
	switch (iCategory)
		{
		case itemcatMisc:
			return CONSTLIT("misc");

		case itemcatArmor:
			return CONSTLIT("armor");

		case itemcatWeapon:
			return CONSTLIT("weapon");

		case itemcatMiscDevice:
			return CONSTLIT("device");

		case itemcatLauncher:
			return CONSTLIT("launcher");

		case itemcatNano:
			return CONSTLIT("(unused)");

		case itemcatReactor:
			return CONSTLIT("reactor");

		case itemcatShields:
			return CONSTLIT("shields");

		case itemcatCargoHold:
			return CONSTLIT("cargoHold");

		case itemcatFuel:
			return CONSTLIT("fuel");

		case itemcatMissile:
			return CONSTLIT("ammo");

		case itemcatDrive:
			return CONSTLIT("drive");

		case itemcatUseful:
			return CONSTLIT("useful");

		default:
			return CONSTLIT("unknown");
		}
	}

CString GetItemCategoryName (ItemCategories iCategory)

//	GetItemCategoryName
//
//	Returns the category name

	{
	switch (iCategory)
		{
		case itemcatMisc:
			return CONSTLIT("miscellaneous");

		case itemcatArmor:
			return CONSTLIT("armor");

		case itemcatWeapon:
			return CONSTLIT("weapon");

		case itemcatMiscDevice:
			return CONSTLIT("device");

		case itemcatLauncher:
			return CONSTLIT("launcher");

		case itemcatNano:
			return CONSTLIT("(unused)");

		case itemcatReactor:
			return CONSTLIT("reactor");

		case itemcatShields:
			return CONSTLIT("shield generator");

		case itemcatCargoHold:
			return CONSTLIT("cargo hold");

		case itemcatFuel:
			return CONSTLIT("fuel");

		case itemcatMissile:
			return CONSTLIT("missile or ammo");

		case itemcatDrive:
			return CONSTLIT("drive");

		case itemcatUseful:
			return CONSTLIT("usable");

		default:
			return CONSTLIT("unknown type");
		}
	}

CString GetLoadStateString (ELoadStates iState)

//	GetLoadStateString
//
//	Returns the load state string.

	{
	return CString(LOAD_STATE_STRINGS[iState]);
	}

CString GetMessageID (MessageTypes iMessage)

//	GetMessageID
//
//	Returns string message ID

	{
	return CString(g_pszMessageID[iMessage]);
	}

MessageTypes GetMessageFromID (const CString &sID)

//	GetMessageFromID
//
//	Returns a message from a message ID

	{
	int i;

	for (i = 0; i < MESSAGE_ID_COUNT; i++)
		if (strEquals(sID, CString(g_pszMessageID[i])))
			return (MessageTypes)i;

	return msgNone;
	}

DWORD GetOrderFlags (IShipController::OrderTypes iOrder)

//	GetOrderFlags
//
//	Returns flags

	{
	return g_OrderTypes[iOrder].dwFlags;
	}

CString GetOrderName (IShipController::OrderTypes iOrder)

//	GetOrderName
//
//	Returns the name of the order

	{
	return CString(g_OrderTypes[iOrder].szName);
	}

CString GetPaintLayerID (CSystem::LayerEnum iPaintLayer)

//	GetPaintLayerID
//
//	Returns the paint layer ID

	{
	switch (iPaintLayer)
		{
		case CSystem::layerBackground:
			return CONSTLIT("background");

		case CSystem::layerSpace:
			return CONSTLIT("space");

		case CSystem::layerStations:
			return CONSTLIT("stations");

		case CSystem::layerShips:
			return CONSTLIT("ships");

		case CSystem::layerEffects:
			return CONSTLIT("effects");

		case CSystem::layerOverhang:
			return CONSTLIT("overhang");

		default:
			return NULL_STR;
		}
	}

CString GetRGBColor (CG32bitPixel rgbColor)

//	GetRGBColor
//
//	Returns a color string of the form "#rrggbb"

	{
	return strPatternSubst(CONSTLIT("#%02x%02x%02x"), rgbColor.GetRed(), rgbColor.GetGreen(), rgbColor.GetBlue());
	}

int OrderGetDataCount (IShipController::OrderTypes iOrder)

//	OrderGetDataCount
//
//	Returns the number of data args

	{
	switch (g_OrderTypes[iOrder].szData[0])
		{
		case '-':
			return 0;

		case 'i':
		case 's':
			return 1;
		
		case '2':
			return 2;

		default:
			return 0;
		}
	}

bool OrderHasDataString (IShipController::OrderTypes iOrder)

//	OrderHasDataString
//
//	Returns TRUE if the given order requires string data

	{
	return (*g_OrderTypes[iOrder].szData == 's');
	}

bool OrderHasDataVector (IShipController::OrderTypes iOrder)

//	OrderHasDataVector
//
//	Returns TRUE if the given order requires string data

	{
	return (*g_OrderTypes[iOrder].szData == 'v');
	}

bool OrderHasTarget (IShipController::OrderTypes iOrder, bool *retbRequired)

//	OrderHasTarget
//
//	Returns TRUE if the given order has a target

	{
	if (retbRequired)
		*retbRequired = (*g_OrderTypes[iOrder].szTarget == 'o');

	return (*g_OrderTypes[iOrder].szTarget != '-');
	}

IShipController::OrderTypes GetOrderType (const CString &sString)

//	GetOrderType
//
//	Loads an order type

	{
	int iType;

	for (iType = 0; iType < ORDER_TYPES_COUNT; iType++)
		if (strEquals(sString, CString(g_OrderTypes[iType].szName)))
			return (IShipController::OrderTypes)iType;

	return IShipController::orderNone;
	}

bool HasModifier (const CString &sModifierList, const CString &sModifier)

//	HadModifier
//
//	Returns TRUE if the item has the given modifier. sModifierList is a list
//	of semicolon (or comma) separated strings. sModifier is a string.

	{
#ifdef DEBUG_HASMODIFIER
	char szBuffer[1024];
	wsprintf(szBuffer, "Looking for: %s; Compare: %s\n", sModifier.GetPointer(), sModifierList.GetPointer());
	::OutputDebugString(szBuffer);
#endif

	//	Blank modifiers always win

	if (sModifier.IsBlank())
		return true;

	//	Loop over modifiers

	char *pPos = sModifierList.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		//	Trim spaces

		while (*pPos == ' ')
			pPos++;

		//	Do we match the modifier?

		char *pFind = sModifier.GetASCIIZPointer();
		while (*pFind != '\0' && strLowerCaseAbsolute(*pFind) == strLowerCaseAbsolute(*pPos))
			{
			pFind++;
			pPos++;
			}

		//	If we matched, then we've got this modifier

		if (*pFind == '\0' && (*pPos == '\0' || *pPos == ';' || *pPos == ',' || *pPos == ' '))
			{
#ifdef DEBUG_HASMODIFIER
			char szBuffer[1024];
			wsprintf(szBuffer, "Looking for: %s; Found: %s\n", sModifier.GetPointer(), sModifierList.GetPointer());
			::OutputDebugString(szBuffer);
#endif
			return true;
			}

		//	Otherwise, skip to the next modifier

		while (*pPos != '\0' && *pPos != ';' && *pPos != ',' && *pPos != ' ')
			pPos++;

		if (*pPos == ';' || *pPos == ',')
			pPos++;
		}

	return false;
	}

bool IsConstantName (const CString &sList)

//	IsConstantName
//
//	Returns TRUE if there is only a single name in the list and
//	if the name has no substitution codes

	{
	char *pPos = sList.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		if (*pPos == ';' || *pPos == '%')
			return false;

		pPos++;
		}

	return true;
	}

bool IsEnergyDamage (DamageTypes iType)
	{
	switch (iType)
		{
		case damageLaser:
		case damageParticle:
		case damageIonRadiation:
		case damagePositron:
		case damageAntiMatter:
		case damageGravitonBeam:
		case damageDarkAcid:
		case damageDarkLightning:
			return true;

		default:
			return false;
		}
	}

bool IsMatterDamage (DamageTypes iType)
	{
	switch (iType)
		{
		case damageKinetic:
		case damageBlast:
		case damageThermonuclear:
		case damagePlasma:
		case damageNano:
		case damageSingularity:
		case damageDarkSteel:
		case damageDarkFire:
			return true;

		default:
			return false;
		}
	}

ALERROR LoadCodeBlock (const CString &sCode, ICCItem **retpCode, CString *retsError)

//	LoadCodeBlock
//
//	Loads a block of code

	{
	//	Null case

	if (sCode.IsBlank())
		{
		*retpCode = NULL;
		return NOERROR;
		}

	//	Compile the code

	ICCItem *pCode = g_pUniverse->GetCC().Link(sCode);
	if (pCode->IsError())
		{
		if (retsError)
			*retsError = pCode->GetStringValue();

		pCode->Discard(&g_pUniverse->GetCC());
		return ERR_FAIL;
		}

	//	Done

	*retpCode = pCode;
	return NOERROR;
	}

ALERROR LoadDamageAdj (CXMLElement *pItem, const CString &sAttrib, int *retiAdj, int *retiCount)

//	LoadDamageAdj
//
//	Loads an attribute of the form "x1,x2,x3,..."
//	into an array of damage adjustments.

	{
	ALERROR error;
	int i;
	TArray<int> Adj;

	if (error = pItem->GetAttributeIntegerList(sAttrib, &Adj))
		return error;

	for (i = 0; i < damageCount; i++)
		retiAdj[i] = (i < Adj.GetCount() ? Adj[i] : 0);

	if (retiCount)
		*retiCount = Adj.GetCount();

	return NOERROR;
	}

DWORD LoadExtensionVersion (const CString &sVersion)

//	LoadExtensionVersion
//
//	Returns the extension version (or 0 if this is an unrecognized extension)
//	NOTE: This is only used for old-style extension versions.

	{
	if (strEquals(sVersion, VERSION_110))
		return 3;
	else if (strEquals(sVersion, VERSION_097)
			|| strEquals(sVersion, VERSION_097A)
			|| strEquals(sVersion, VERSION_098)
			|| strEquals(sVersion, VERSION_098A)
			|| strEquals(sVersion, VERSION_098B)
			|| strEquals(sVersion, VERSION_098C)
			|| strEquals(sVersion, VERSION_098D)
			|| strEquals(sVersion, VERSION_099)
			|| strEquals(sVersion, VERSION_099A)
			|| strEquals(sVersion, VERSION_099B)
			|| strEquals(sVersion, VERSION_099C)
			|| strEquals(sVersion, VERSION_100))
		return 2;
	else if (strEquals(sVersion, VERSION_095)
			|| strEquals(sVersion, VERSION_095A)
			|| strEquals(sVersion, VERSION_095B)
			|| strEquals(sVersion, VERSION_096)
			|| strEquals(sVersion, VERSION_096A))
		return 1;
	else
		return 0;
	}

CG32bitPixel LoadRGBColor (const CString &sString, CG32bitPixel rgbDefault)

//	LoadRGBColor
//
//	Returns a 16-bit color from an RGB triplet

	{
	char *pPos = sString.GetASCIIZPointer();

	//	Null

	if (*pPos == '\0')
		return rgbDefault;

	//	If it starts with a # we expect an RGB DWORD

	else if (*pPos == '#')
		{
		pPos++;
		DWORD dwColor = strParseIntOfBase(pPos, 16, 0);
		return CG32bitPixel((dwColor >> 16) & 0xFF, (dwColor >> 8) & 0xFF, dwColor & 0xFF);
		}

	//	Otherwise, we expect three comma-separated values

	else
		{
		int iRed = strParseInt(pPos, 0, &pPos, NULL); if (*pPos) pPos++;
		int iGreen = strParseInt(pPos, 0, &pPos, NULL); if (*pPos) pPos++;
		int iBlue = strParseInt(pPos, 0, &pPos, NULL);

		return CG32bitPixel(iRed, iGreen, iBlue);
		}
	}

ALERROR LoadUNID (SDesignLoadCtx &Ctx, const CString &sString, DWORD *retdwUNID)

//	LoadUNID
//
//	Loads the given UNID

	{
	char *pPos = sString.GetASCIIZPointer();

	//	A blank string just means NULL.

	if (*pPos == '\0')
		{
		*retdwUNID = 0;
		return NOERROR;
		}

	//	An @ prefix means that we are relative to the extension

	else if (*pPos == '@')
		{
		//	Must have an extension in this case

		if (Ctx.pExtension == NULL)
			{
			Ctx.sError = CONSTLIT("Cannot have relative UNID outside of an extension.");
			return ERR_FAIL;
			}

		WORD wLow = LOWORD(Ctx.pExtension->GetUNID()) + (WORD)strParseIntOfBase(pPos+1, 16, 0, NULL, NULL);
		*retdwUNID = MAKELONG(wLow, HIWORD(Ctx.pExtension->GetUNID()));
		return NOERROR;
		}

	//	Otherwise we expect a number

	else
		{
		*retdwUNID = strToInt(sString, 0);

		//	Must be a valid non-zero number

		if (*retdwUNID == 0)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid UNID: %s."), sString);
			return ERR_FAIL;
			}

		return NOERROR;
		}
	}

int NLCompare (TArray<CString> &Input, TArray<CString> &Pattern)

//	NLCompare
//
//	Compares the two word collection.
//
//	For each word in Input that matches a word in Pattern
//	exactly, we return +10
//
//	For each word in Input that matches a word in Pattern
//	partially, we return +2
//
//	For each word in Pattern in excess of the number of words
//	in Input, we return -1

	{
	int i, j;
	int iResult = 0;

	for (i = 0; i < Input.GetCount(); i++)
		{
		//	If this word in the input exactly matches one of the words
		//	in the pattern, then +10

		bool bMatched = false;
		for (j = 0; j < Pattern.GetCount(); j++)
			if (strEquals(Input[i], Pattern[j]))
				{
				bMatched = true;
				iResult += 10;
				break;
				}

		if (bMatched)
			continue;

		//	If this word in the input is a number, and if the number as
		//	a roman numeral matches one of the words in the pattern
		//	exactly, then +5

		int iNumber = strToInt(Input[i], -1, NULL);
		if (iNumber > 0)
			{
			CString sRomanNumeral = strRomanNumeral(iNumber);
			for (j = 0; j < Pattern.GetCount(); j++)
				if (strEquals(sRomanNumeral, Pattern[j]))
					{
					bMatched = true;
					iResult += 5;
					break;
					}
			}

		if (bMatched)
			continue;

		//	If this word in the input partially matches one of the words
		//	in the pattern, then +2

		for (j = 0; j < Pattern.GetCount(); j++)
			if (strFind(Pattern[j], Input[i]) != -1)
				{
				bMatched = true;
				iResult += 2;
				break;
				}

		if (bMatched)
			continue;
		}

	//	Adjust for length

	if (Input.GetCount() < Pattern.GetCount())
		iResult -= 1;

	return iResult;
	}

void NLSplit (const CString &sPhrase, TArray<CString> *retArray)

//	NLSplit
//
//	Splits a phrase into words for comparison

	{
	//	Generate a list of strings that we will search for

	char *pPos = sPhrase.GetASCIIZPointer();
	CString sToken;
	CString sPrevAppend;
	while (true)
		{
		if ((*pPos >= 'a' && *pPos <= 'z')
				|| (*pPos >= 'A' && *pPos <= 'Z')
				|| (*pPos >= '0' && *pPos <= '9'))
			{
			sToken.Append(CString(pPos, 1));
			}
		else
			{
			if (!sToken.IsBlank())
				{
				//	Add this word to the list

				retArray->Insert(sToken);

				//	If we had a hyphen previously, then we append the
				//	concatenation

				if (!sPrevAppend.IsBlank())
					{
					sPrevAppend.Append(sToken);
					retArray->Insert(sPrevAppend);
					sPrevAppend = NULL_STR;
					}

				//	If this is a hyphen, then remember this word for
				//	future appending

				if (*pPos == '-')
					sPrevAppend = sToken;

				//	Reset token

				sToken = NULL_STR;
				}

			//	If this is the end, then we're done

			if (*pPos == '\0')
				break;
			}

		pPos++;
		}
	}

CString ParseCriteriaParam (char **ioPos, bool bExpectColon, bool *retbBinaryParam)

//	ParseCriteriaParam
//
//	Parses a criteria parameter. *ioPos starts by pointing at the
//	criteria character right before the start of the parameter.
//	We leave it set to the last character of the parameter
//	(ready to be incremented).
//
//	If bExpectColon is set, then we parse a param of the form:
//
//		:param;
//
//	In which we strip out the leading colon; In this mode, if we don't see
//	a leading colon, we assume a NULL_STR param (this syntax supports
//	optional params)
//
//	If the param has an embedded colon, we return retbBinaryParam set to TRUE.

	{
	char *pPos = *ioPos;

	//	Skip criteria character

	pPos++;

	//	Skip whitespace

	while (*pPos == ' ' || *pPos == '\t')
		pPos++;

	//	If we expect a colon, process it now

	if (bExpectColon)
		{
		//	If we don't have a colon, then we return
		//	NULL_STR and leave the pointer where it was. This takes
		//	care of the case where we have no parameter.

		if (*pPos != ':')
			{
			if (retbBinaryParam)
				*retbBinaryParam = false;
			return NULL_STR;
			}

		//	Otherwise, consume colon

		pPos++;
		}

	//	Parse the parameter

	bool bBinaryParam = false;
	bool bAllowSpaces = false;
	char *pStart = pPos;
	while (*pPos != ';' && (bAllowSpaces || *pPos != ' ') && *pPos != '\t' && *pPos != '\0')
		{
		if (*pPos == ':')
			{
			bAllowSpaces = true;
			bBinaryParam = true;
			}
		pPos++;
		}

	//	If we hit the end, we backup one character because our caller
	//	will advance the position by one

	*ioPos = (*pPos == '\0' ? (pPos - 1) : pPos);

	//	Done

	if (retbBinaryParam)
		*retbBinaryParam = bBinaryParam;

	return CString(pStart, pPos - pStart);
	}

bool ParseCriteriaParamLevelRange (char **ioPos, int *retiLow, int *retiHigh)

//	ParseCriteriaParamLevelRange
//
//	Returns levels

	{
	CString sParam = ParseCriteriaParam(ioPos);
	char *pParamPos = sParam.GetASCIIZPointer();

	//	Parse the first number

	int iLow = strParseInt(pParamPos, -1, &pParamPos);
	int iHigh;

	//	If we don't have a second number, then we just want items
	//	of the given level.

	if (*pParamPos != '-')
		{
		iHigh = -1;
		}

	//	Otherwise, we parse a second number

	else
		{
		pParamPos++;
		iHigh = strParseInt(pParamPos, -1, &pParamPos);

		if (iLow == -1)
			iLow = 1;
		if (iHigh == -1)
			iHigh = MAX_ITEM_LEVEL;
		}

	//	Done

	if (retiLow)
		*retiLow = iLow;

	if (retiHigh)
		*retiHigh = iHigh;

	return (iLow != -1);
	}

ALERROR ParseDamageTypeList (const CString &sList, TArray<CString> *retList)

//	ParseDamageTypeList
//
//	Parses a list of strings, each string corresponding to a damageType.
//	The list is either positional (each string in the list represents a 
//	damageType by position) or explicit (each string is a key/value pair with
//	the key being a damageType).
//
//	On return we guarantee the retList has all damageTypes. Any NULL_STR
//	values means that the string for that damageType was not specified.

	{
	int i;

	//	Initialize

	retList->DeleteAll();
	retList->InsertEmpty(damageCount);

	//	Parse

	TArray<CString> DamageAdj;
	ParseStringList(sList, PSL_FLAG_ALLOW_WHITESPACE, &DamageAdj);

	//	Now parse each element

	int iCount = Min(DamageAdj.GetCount(), (int)damageCount);
	for (i = 0; i < iCount; i++)
		{
		CString sDamageType;
		CString sAdj;

		ParseKeyValuePair(DamageAdj[i], 0, &sDamageType, &sAdj);

		//	If we have an explicitly specified damage type, then use that.
		//	Otherwise we rely on the position of the adjustment.
		//	NOTE: We should not mix and match the two methods.

		if (!sDamageType.IsBlank())
			{
			DamageTypes iType = LoadDamageTypeFromXML(sDamageType);
			if (iType == damageError || iType == damageGeneric)
				return ERR_FAIL;

			retList->GetAt(iType) = sAdj;
			}
		else
			retList->GetAt(i) = sAdj;
		}

	return NOERROR;
	}

Metric ParseDistance (const CString &sValue, Metric rDefaultScale)

//	ParseDistance
//
//	Parses a number with optional units:
//
//	"123"	-> Multiplied by default scale
//	"123 px" -> 123 pixels converted to klicks
//	"123 ls" -> 123 light-seconds
//	"123 lm" -> 123 light-minutes
//	"123 au" -> 123 AUs

	{
	//	First parse the number

	char *pPos = sValue.GetASCIIZPointer();
	int iValue = strParseInt(pPos, 0, &pPos);
	if (iValue <= 0)
		return 0.0;

	//	See if we have units

	while (strIsWhitespace(pPos))
		pPos++;

	if (*pPos == '\0')
		return iValue * rDefaultScale;

	char *pStart = pPos;
	while (*pPos != '\0' && !strIsWhitespace(pPos))
		pPos++;

	CString sUnits(pStart, (int)(pPos - pStart));
	if (strEquals(sUnits, CONSTLIT("px")))
		return iValue * g_KlicksPerPixel;
	else if (strEquals(sUnits, CONSTLIT("ls"))
			|| strEquals(sUnits, CONSTLIT("light-second"))
			|| strEquals(sUnits, CONSTLIT("light-seconds")))
		return iValue * LIGHT_SECOND;
	else if (strEquals(sUnits, CONSTLIT("lm"))
			|| strEquals(sUnits, CONSTLIT("light-minute"))
			|| strEquals(sUnits, CONSTLIT("light-minutes")))
		return iValue * LIGHT_MINUTE;
	else if (strEquals(sUnits, CONSTLIT("au")))
		return iValue * g_AU;
	else
		return 0.0;
	}

GenomeTypes ParseGenomeID (const CString &sText)

//	ParseGenomeID
//
//	Returns the genome from an ID

	{
	int i;

	for (i = 0; i < GENOME_COUNT; i++)
		if (strEquals(sText, CString(g_Genome[i].pszID)))
			return g_Genome[i].iType;

	return genomeUnknown;
	}

void ParseIntegerList (const CString &sList, DWORD dwFlags, TArray<int> *retList)

//	ParseIntegerList
//
//	Splits a string into a list of integers, using ParseStringList.

	{
	int i;

	TArray<CString> List;
	ParseStringList(sList, dwFlags, &List);

	retList->DeleteAll();
	retList->InsertEmpty(List.GetCount());
	for (i = 0; i < List.GetCount(); i++)
		retList->GetAt(i) = strToInt(List[i], 0);
	}

void ParseKeyValuePair (const CString &sString, DWORD dwFlags, CString *retsKey, CString *retsValue)

//	ParseKeyValuePair
//
//	Parses a string of the form:
//
//	{key} : {value}
//
//	And returns the two parts. We trim leading and trailing whitespace from both
//	the key and the value.
//
//	NOTE: If we don't find a colon separator, we return what we found as the value.

	{
	bool bFoundSeparator = false;
	char *pPos = sString.GetASCIIZPointer();

	//	Skip whitespace

	while (strIsWhitespace(pPos))
		pPos++;

	//	Key

	char *pStart = pPos;
	while (*pPos != '\0' && *pPos != ':' && !strIsWhitespace(pPos))
		pPos++;

	//	Get the key

	CString sKey = CString(pStart, (int)(pPos - pStart));

	//	Skip whitespace

	while (strIsWhitespace(pPos))
		pPos++;

	//	Skip colon

	if (*pPos == ':')
		{
		pPos++;

		while (strIsWhitespace(pPos))
			pPos++;

		bFoundSeparator = true;
		}

	//	Parse value

	pStart = pPos;
	while (*pPos != '\0' && !strIsWhitespace(pPos))
		pPos++;

	CString sValue = CString(pStart, (int)(pPos - pStart));

	//	If we never found a separator and the value is empty, then return the
	//	string as the value.

	if (!bFoundSeparator && sValue.GetLength() == 0)
		{
		sValue = sKey;
		sKey = NULL_STR;
		}

	//	Return what we found

	if (retsKey)
		*retsKey = sKey;

	if (retsValue)
		*retsValue = sValue;
	}

bool ParseOrderString (const CString &sValue, IShipController::OrderTypes *retiOrder, IShipController::SData *retData)

//	ParseOrderString
//
//	Parses an order string of the form:
//
//	{order}:{d1}:{d2}

	{
	char *pPos = sValue.GetASCIIZPointer();

	//	Parse the order name

	char *pStart = pPos;
	while (*pPos != '\0' && *pPos != ':')
		pPos++;

	CString sOrder(pStart, (int)(pPos - pStart));

	//	For backwards compatibility we handle some special names.

	IShipController::OrderTypes iOrder;
	if (strEquals(sOrder, CONSTLIT("trade route")))
		iOrder = IShipController::orderTradeRoute;
	else
		{
		iOrder = GetOrderType(sOrder);

		//	Check for error

		if (iOrder == IShipController::orderNone && !sOrder.IsBlank())
			return false;
		}

	//	Get additional data

	if (retData)
		{
		DWORD dwData1 = 0;
		DWORD dwData2 = 0;

		if (*pPos != ':')
			*retData = IShipController::SData();
		else
			{
			pPos++;
			pStart = pPos;
			while (*pPos != '\0' && *pPos != ':')
				pPos++;

			CString sData(pStart, (int)(pPos - pStart));
			dwData1 = strToInt(sData, 0);

			if (*pPos != ':')
				*retData = IShipController::SData(dwData1);
			else
				{
				pPos++;
				pStart = pPos;
				while (*pPos != '\0')
					pPos++;

				CString sData(pStart, (int)(pPos - pStart));
				dwData2 = strToInt(sData, 0);

				*retData = IShipController::SData(dwData1, dwData2);
				}
			}
		}

	//	Done

	*retiOrder = iOrder;
	return true;
	}

CSystem::LayerEnum ParsePaintLayerID (const CString &sID)

//	ParsePaintLayerID
//
//	Parses a paint layer ID

	{
	if (strEquals(sID, CONSTLIT("background")))
		return CSystem::layerBackground;
	else if (strEquals(sID, CONSTLIT("space")))
		return CSystem::layerSpace;
	else if (strEquals(sID, CONSTLIT("stations")))
		return CSystem::layerStations;
	else if (strEquals(sID, CONSTLIT("ships")))
		return CSystem::layerShips;
	else if (strEquals(sID, CONSTLIT("effects")))
		return CSystem::layerSpace;
	else if (strEquals(sID, CONSTLIT("overhang")))
		return CSystem::layerOverhang;
	else
		return CSystem::layerBackground;
	}

EStorageScopes ParseStorageScopeID (const CString &sID)

//	ParseStorageScopeID
//
//	Parses a storage scope identifier

	{
	if (strEquals(sID, STORAGE_DEVICE))
		return storeDevice;
	else if (strEquals(sID, STORAGE_SERVICE_EXTENSION))
		return storeServiceExtension;
	else if (strEquals(sID, STORAGE_SERVICE_USER))
		return storeServiceUser;
	else
		return storeUnknown;
	}

void ParseStringList (const CString &sList, DWORD dwFlags, TArray<CString> *retList)

//	ParseStringList
//
//	Splits a string into a list of strings.
//
//	By default we accept a list of strings separated by either:
//
//	;
//	,
//	(whitespace)
//
//	If PSL_FLAG_ALLOW_WHITESPACE is set, then we allow strings to have embedded
//	whitespace (we do not use it as a terminator) but we still trim leading
//	and trailing whitespace from each string.

	{
	bool bWhitespaceDelimiter = ((dwFlags & PSL_FLAG_ALLOW_WHITESPACE) ? false : true);

	char *pPos = sList.GetASCIIZPointer();
	while (*pPos != '\0')
		{
		//	Trim spaces

		while (strIsWhitespace(pPos))
			pPos++;

		//	Skip until delimiter

		char *pStart = pPos;
		while (*pPos != '\0' && *pPos != ';' && *pPos != ',' && (!bWhitespaceDelimiter || !strIsWhitespace(pPos)))
			pPos++;

		//	Insert modifier

		if (pPos != pStart)
			{
			char *pEnd = pPos;

			//	If whitespace is not a delimiter, then trim whitespace from the
			//	end of the element.

			if (!bWhitespaceDelimiter)
				{
				while (pEnd - 1 > pStart && strIsWhitespace(pEnd - 1))
					pEnd--;
				}

			retList->Insert(CString(pStart, pEnd - pStart));
			}

		//	Next

		if (*pPos == ';' || *pPos == ',')
			pPos++;
		}
	}

void ParseUNIDList (const CString &sList, DWORD dwFlags, TArray<DWORD> *retList)

//	ParseUNIDList
//
//	Returns a list of UNIDs from a string.

	{
	int i;

	TArray<CString> List;
	ParseStringList(sList, dwFlags, &List);

	for (i = 0; i < List.GetCount(); i++)
		{
		DWORD dwUNID;
		if (dwFlags & PUL_FLAG_HEX)
			dwUNID = (DWORD)strParseIntOfBase(List[i].GetASCIIZPointer(), 16, 0);
		else
			dwUNID = (DWORD)strToInt(List[i], 0);

		if (dwUNID != 0)
			retList->Insert(dwUNID);
		}
	}

CString ReactorPower2String (int iPower)

//	ReactorPower2String
//
//	Generates a string for reactor power

	{
	int iReactorPower = (iPower + 5) / 10;

	if (iReactorPower < 1000)
		return strPatternSubst(CONSTLIT("%dMW"), iReactorPower);
	else
		{
		int iGW = iReactorPower / 1000;
		int iMW = iReactorPower % 1000;

		if (iMW == 0)
			return strPatternSubst(CONSTLIT("%dGW"), iGW);
		else
			return strPatternSubst(CONSTLIT("%d.%dGW"),	iGW, (iMW + 50) / 100);
		}
	}

void ReportCrashObj (CString *retsMessage, CSpaceObject *pCrashObj)

//	ReportCrashObj
//
//	Include information about the given object

	{
	CString sName = CONSTLIT("unknown");
	CString sType = CONSTLIT("unknown");
	bool bDestroyed = false;
	CString sData;

	if (pCrashObj == NULL)
		pCrashObj = g_pProgramObj;

	if (pCrashObj == NULL)
		{
		retsMessage->Append(CONSTLIT("obj NULL\r\n"));
		return;
		}

	if (pCrashObj)
		{
		try
			{
			sName = pCrashObj->GetNounPhrase();
			sType = pCrashObj->GetObjClassName();
			bDestroyed = pCrashObj->IsDestroyed();
			}
		catch (...)
			{
			}

		try
			{
			sData = pCrashObj->DebugCrashInfo();
			}
		catch (...)
			{
			sData = CONSTLIT("error obtaining crash info\r\n");
			}
		}

	if (bDestroyed)
		retsMessage->Append(CONSTLIT("obj destroyed\r\n"));
	retsMessage->Append(strPatternSubst(CONSTLIT("obj class: %s\r\n"), sType));
	retsMessage->Append(strPatternSubst(CONSTLIT("obj name: %s\r\n"), sName));
	retsMessage->Append(strPatternSubst(CONSTLIT("obj pointer: %x\r\n"), pCrashObj));
	retsMessage->Append(sData);
	}

bool SetFrequencyByLevel (CString &sLevelFrequency, int iLevel, int iFreq)

//	SetFrequencyByLevel
//
//	Sets the frequency on the given string. We assume that the string has places
//	for the appropriate level.

	{
	ASSERT(iLevel >= 1 && iLevel <= MAX_ITEM_LEVEL);
	int iChar = iLevel + ((iLevel - 1) / 5) - 1;
	if (iChar < 0 || iChar >= sLevelFrequency.GetLength())
		return false;

	char *pPos = sLevelFrequency.GetASCIIZPointer() + iChar;

	if (iFreq >= ftCommon)
		*pPos = 'c';
	else if (iFreq >= ftUncommon)
		*pPos = 'u';
	else if (iFreq >= ftRare)
		*pPos = 'r';
	else if (iFreq >= ftVeryRare)
		*pPos = 'v';
	else
		*pPos = '-';

	return true;
	}

CSpaceObject *SDestroyCtx::GetOrderGiver (void) const { return (Attacker.GetObj() ? Attacker.GetObj()->GetOrderGiver(iCause) : NULL); }

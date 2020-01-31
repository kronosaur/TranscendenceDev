//	Damage.cpp
//
//	Damage routines and classes

#include "PreComp.h"

#define OVERLAY_TAG								CONSTLIT("Overlay")

#define DAMAGE_ATTRIB							CONSTLIT("damage")
#define ATTACK_FORM_ATTRIB						CONSTLIT("attackMode")
#define TYPE_ATTRIB								CONSTLIT("type")
#define CHANCE_ATTRIB							CONSTLIT("chance")
#define LIFETIME_ATTRIB							CONSTLIT("lifetime")

#define SPECIAL_DAMAGE_ARMOR					CONSTLIT("armor")
#define SPECIAL_DAMAGE_BLINDING					CONSTLIT("blinding")
#define SPECIAL_DAMAGE_DEVICE					CONSTLIT("device")
#define SPECIAL_DAMAGE_DEVICE_DISRUPT			CONSTLIT("deviceDisrupt")
#define SPECIAL_DAMAGE_DISINTEGRATION			CONSTLIT("disintegration")
#define SPECIAL_DAMAGE_EMP						CONSTLIT("EMP")
#define SPECIAL_DAMAGE_FUEL						CONSTLIT("fuel")
#define SPECIAL_DAMAGE_MINING					CONSTLIT("mining")
#define SPECIAL_DAMAGE_MOMENTUM					CONSTLIT("momentum")
#define SPECIAL_DAMAGE_RADIATION				CONSTLIT("radiation")
#define SPECIAL_DAMAGE_REFLECT					CONSTLIT("reflect")
#define SPECIAL_DAMAGE_SENSOR					CONSTLIT("sensor")
#define SPECIAL_DAMAGE_SHATTER					CONSTLIT("shatter")
#define SPECIAL_DAMAGE_SHIELD					CONSTLIT("shield")
#define SPECIAL_DAMAGE_WMD						CONSTLIT("WMD")
#define SPECIAL_DAMAGE_WORMHOLE					CONSTLIT("wormhole")

#define OVERLAY_TYPE_HULL						CONSTLIT("hull")

const int DEFAULT_BLINDING_TIME =				(5 * g_TicksPerSecond);
const int DEFAULT_EMP_TIME =					(7 * g_TicksPerSecond);
const int DEFAULT_DEVICE_DISRUPT_TIME =			(30 * g_TicksPerSecond);

const int MAX_INTENSITY =						7;
const int MAX_BINARY =							1;

const Metric SHOCKWAVE_DAMAGE_FACTOR =			4.0;

struct SDamageTypeData
    {
    char *pszID;
    int iTier;
    int iLevel;
    char *pszName;
    char *pszShortName;
    };

SDamageTypeData DAMAGE_TYPE_DATA[damageCount] =
    {
        { "laser",          1,  1,  "laser",            "laser" },
        { "kinetic",        1,  1,  "kinetic",          "kinetic" },
        { "particle",       2,  4,  "particle beam",    "particle" },
        { "blast",          2,  4,  "blast",            "blast" },
        { "ion",            3,  7,  "ion",              "ion" },
        { "thermo",         3,  7,  "thermonuclear",    "thermo" },
        { "positron",       4,  10, "positron beam",    "positron" },
        { "plasma",         4,  10, "plasma",           "plasma" },
        { "antimatter",     5,  13, "antimatter",       "antimatter" },
        { "nano",           5,  13, "nanotech",         "nano" },
        { "graviton",       6,  16, "graviton",         "graviton" },
        { "singularity",    6,  16, "singularity",      "singularity" },
        { "darkAcid",       7,  19, "dark acid",        "dark acid" },
        { "darkSteel",      7,  19, "dark steel",       "dark steel" },
        { "darkLightning",  8,  22, "dark lightning",   "dark lightning" },
        { "darkFire",       8,  22, "dark fire",        "dark fire" },
    };

struct SSpecialDamageData
	{
	char *pszSpecial;						//	Name as specified in a damage desc line
	char *pszProperty;						//	Property for this special damage
	};

//	These must match the order in the SpecialDamageTypes enum.

SSpecialDamageData SPECIAL_DAMAGE_DATA[] =
	{
		{	"radiation",		"damageRadiation" },
		{	"blinding",			"damageBlinding" },
		{	"EMP",				"damageEMP" },
		{	"device",			"damageDeviceDestroy" },
		{	"disintegration",	"damageDisintegration" },
		{	"momentum",			"damageMomentum" },
		{	"shield",			"damageShieldLevel" },
		{	"WMD",				"damageWMD" },

		{	"mining",			"damageMining" },
		{	"deviceDisrupt",	"damageDeviceDisrupt" },
		{	"wormhole",			"damageWormhole" },
		{	"fuel",				"damageFuel" },
		{	"shatter",			"damageShatter" },
		{	"armor",			"damageArmorLevel" },
		{	"sensor",			"damageSensor" },
		{	"shieldPenetrate",	"damageShieldPenetrate" },

		{	"timeStop",			"damageTimeStop" },

		{	NULL,				NULL }
	};

struct SWMDData
    {
    int iAdj;                               //  Adjustments to damage at this WMD value (0-100)
    int iLevel;                             //  Display level (0-10)
    };
    
SWMDData WMD_DATA[] =
    {
        {   10,     0   },
        {   25,     2   },
        {   32,     3   },
        {   40,     4   },
        {   50,     5   },
        {   63,     6   },
        {   80,     8   },
        {   100,    10   },
    };

//	Damage Types

CString GetDamageName (DamageTypes iType)

//	GetDamageName
//
//	Returns the name of this type of damage

	{
	if (iType == damageGeneric)
		return CONSTLIT("generic");
	else
		return CString(DAMAGE_TYPE_DATA[iType].pszName);
	}

CString GetDamageShortName (DamageTypes iType)

//	GetDamageShortName
//
//	Returns the short name of this type of damage

	{
	if (iType == damageGeneric)
		return CONSTLIT("generic");
	else
		return CString(DAMAGE_TYPE_DATA[iType].pszShortName);
	}

CString GetDamageType (DamageTypes iType)

//	GetDamageType
//
//	Returns the damage type string

	{
	if (iType == damageGeneric)
		return CONSTLIT("generic");
	else
		return CString(DAMAGE_TYPE_DATA[iType].pszID);
	}

DamageTypes LoadDamageTypeFromXML (const CString &sAttrib)

//	LoadDamageTypeFromXML
//
//	Converts from string to DamageType

	{
	int iType;

	for (iType = 0; iType < damageCount; iType++)
		if (strEquals(sAttrib, CString(DAMAGE_TYPE_DATA[iType].pszID)))
			return (DamageTypes)iType;

	//	Generic

	if (strEquals(sAttrib, CONSTLIT("generic")))
		return damageGeneric;

	//	Backwards compatibility

	if (strEquals(sAttrib, CONSTLIT("dark acid")))
		return damageDarkAcid;
	else if (strEquals(sAttrib, CONSTLIT("dark steel")))
		return damageDarkSteel;
	else if (strEquals(sAttrib, CONSTLIT("dark lightning")))
		return damageDarkLightning;
	else if (strEquals(sAttrib, CONSTLIT("dark fire")))
		return damageDarkFire;

	return damageError;
	}

//	DamageDesc -----------------------------------------------------------------

void DamageDesc::AddEnhancements (const CItemEnhancementStack *pEnhancements)

//	AddEnhancements
//
//	Applies any enhancements in the given stack to the damage descriptor
//	(including bonus and special damage).

	{
	if (pEnhancements == NULL)
		return;

	//	Add bonus

	AddBonus(pEnhancements->GetBonus());

	//	Add special damage

	pEnhancements->ApplySpecialDamage(this);
	}

SpecialDamageTypes DamageDesc::ConvertToSpecialDamageTypes (const CString &sValue)

//	ConvertToSpecialDamageTypes
//
//	Converts

	{
	int iSpecial = 0;
	SSpecialDamageData *pData = SPECIAL_DAMAGE_DATA;
	while (pData->pszSpecial)
		{
		if (strEquals(CString(pData->pszSpecial, -1, true), sValue))
			return (SpecialDamageTypes)iSpecial;

		pData++;
		iSpecial++;
		}

	//	If we get this far, then we did not find it.

	return specialNone;
	}

SpecialDamageTypes DamageDesc::ConvertPropertyToSpecialDamageTypes (const CString &sValue)

//	ConvertPropertyToSpecialDamageTypes
//
//	Converts

	{
	int iSpecial = 0;
	SSpecialDamageData *pData = SPECIAL_DAMAGE_DATA;
	while (pData->pszSpecial)
		{
		if (strEquals(CString(pData->pszProperty, -1, true), sValue))
			return (SpecialDamageTypes)iSpecial;

		pData++;
		iSpecial++;
		}

	//	If we get this far, then we did not find it.

	return specialNone;
	}

ICCItem *DamageDesc::FindProperty (const CString &sName) const

//	FindProperty
//
//	Returns the given damage property (or NULL for unknown properties).

	{
	//	See if this is one of the special damage properties

	SpecialDamageTypes iSpecial;
	if ((iSpecial = ConvertPropertyToSpecialDamageTypes(sName)) != specialNone)
		return CCodeChain::CreateInteger(GetSpecialDamage(iSpecial));

	//	Otherwise, not found

	else
		return NULL;
	}

int DamageDesc::GetDamageLevel (DamageTypes iType)

//  GetDamageTier
//
//  Returns the damage tier based on damage type.

    {
    if (iType == damageGeneric)
        return 1;
    else
        return DAMAGE_TYPE_DATA[iType].iLevel;
    }

int DamageDesc::GetDamageTier (DamageTypes iType)

//  GetDamageTier
//
//  Returns the damage tier based on damage type.

    {
    if (iType == damageGeneric)
        return 1;
    else
        return DAMAGE_TYPE_DATA[iType].iTier;
    }

Metric DamageDesc::GetDamageValue (DWORD dwFlags) const

//	GetDamageValue
//
//	Returns the damage in HP

	{
	//	Get the damage value based on what we're looking for

	Metric rDamage;
	if (dwFlags & flagMinDamage)
		rDamage = m_Damage.GetMinValue();
	else if (dwFlags & flagMaxDamage)
		rDamage = m_Damage.GetMaxValue();
	else
		rDamage = m_Damage.GetAveValueFloat();

	//	Adjust for bonus

	if (dwFlags & flagIncludeBonus)
		rDamage += rDamage * m_iBonus / 100.0;

	//	Adjust for WMD

	if (dwFlags & flagWMDAdj)
		rDamage = rDamage * GetMassDestructionAdj() / 100.0;

	return rDamage;
	}

int DamageDesc::GetMassDestructionAdj (void) const

//  GetMassDestructionAdj
//
//  Returns the adjustment to damage given our level of mass destruction.

    {
    return WMD_DATA[m_MassDestructionAdj].iAdj;
    }
    
int DamageDesc::GetMassDestructionAdjFromValue (int iValue) 
    
//  GetMassDestructionAdjFromValue
//
//  Returns the damage adj of WMD.

    {
    return WMD_DATA[Max(0, Min(iValue, MAX_INTENSITY))].iAdj;
    }

int DamageDesc::GetMassDestructionLevel (void) const

//  GetMassDestructionLevel
//
//  Returns the display level of WMD. This is what we show in weapon stats.

    {
    return WMD_DATA[m_MassDestructionAdj].iLevel;
    }

int DamageDesc::GetMassDestructionLevelFromValue (int iValue) 
    
//  GetMassDestructionLevel
//
//  Returns the display level of WMD. This is what we show in weapon stats.

    {
    return WMD_DATA[Max(0, Min(iValue, MAX_INTENSITY))].iLevel;
    }

int DamageDesc::GetMiningWMDAdj (void)

//	GetMiningWMDAdj
//
//	Returns the adjustment to damage if we treat mining as WMD.

	{
    return WMD_DATA[m_MiningAdj].iAdj;
	}

int DamageDesc::GetSpecialDamage (SpecialDamageTypes iSpecial, DWORD dwFlags) const

//	GetSpecialDamage
//
//	Returns special damage level

	{
	switch (iSpecial)
		{
		case specialArmor:
			return m_ArmorDamage;

		case specialBlinding:
			return m_BlindingDamage;

		case specialDeviceDamage:
			return ((dwFlags & flagSpecialAdj) ? 4 * m_DeviceDamage * m_DeviceDamage : m_DeviceDamage);

		case specialDeviceDisrupt:
			return ((dwFlags & flagSpecialAdj) ? 4 * m_DeviceDisruptDamage * m_DeviceDisruptDamage : m_DeviceDisruptDamage);

		case specialDisintegration:
			return m_DisintegrationDamage;

		case specialEMP:
			return m_EMPDamage;

		case specialFuel:
			return m_FuelDamage;

		case specialMining:
			return ((dwFlags & flagSpecialAdj) ? GetMiningAdj() : m_MiningAdj);

		case specialMomentum:
			return m_MomentumDamage;

		case specialRadiation:
			return m_RadiationDamage;

		case specialShatter:
			return m_ShatterDamage;

		case specialShieldDisrupt:
			return m_ShieldDamage;

		case specialShieldPenetrator:
			return ((dwFlags & flagSpecialAdj) ? GetShieldPenetratorAdj() : m_ShieldPenetratorAdj);

		case specialTimeStop:
			return m_TimeStopDamage;

		case specialWMD:
            if (dwFlags & flagSpecialAdj)
                return GetMassDestructionAdj();
            else if (dwFlags & flagSpecialLevel)
                return GetMassDestructionLevel();
            else
                return m_MassDestructionAdj;

		default:
			return 0;
		}
	}

CString DamageDesc::GetSpecialDamageName (SpecialDamageTypes iSpecial)

//	GetSpecialDamageName
//
//	Returns the name

	{
	return CString(SPECIAL_DAMAGE_DATA[iSpecial].pszSpecial, -1, TRUE);
	}

void DamageDesc::SetSpecialDamage (SpecialDamageTypes iSpecial, int iLevel)

//	SetSpecialDamage
//
//	Sets special damage

	{
	switch (iSpecial)
		{
		case specialArmor:
			m_ArmorDamage = Max(1, Min(iLevel, MAX_ITEM_LEVEL));
			break;

		case specialBlinding:
			m_BlindingDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialDeviceDamage:
			m_DeviceDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialDeviceDisrupt:
			m_DeviceDisruptDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialDisintegration:
			m_DisintegrationDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialEMP:
			m_EMPDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialFuel:
			m_FuelDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialMining:
			m_MiningAdj = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialMomentum:
			m_MomentumDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialRadiation:
			m_RadiationDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialShatter:
			m_ShatterDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialShieldDisrupt:
			m_ShieldDamage = Max(1, Min(iLevel, MAX_ITEM_LEVEL));
			break;

		case specialShieldPenetrator:
			m_ShieldPenetratorAdj = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialTimeStop:
			m_TimeStopDamage = Max(1, Min(iLevel, MAX_ITEM_LEVEL));
			break;

		case specialWMD:
			m_MassDestructionAdj = Max(0, Min(iLevel, MAX_INTENSITY));
			break;
		}
	}

void DamageDesc::SetDamage (int iDamage)

//	SetDamage
//
//	Sets a constant damage

	{
	m_Damage = DiceRange(0, 0, iDamage);
	}

CString DamageDesc::GetDesc (DWORD dwFlags)

//	GetDesc
//
//	Returns a description of the damage:
//
//	laser 1-4 (+50%)

	{
	CString sDamageType;
	if (!(dwFlags & flagNoDamageType))
		sDamageType = strPatternSubst(CONSTLIT("%s "), GetDamageShortName(m_iType));

	if (dwFlags & flagAverageDamage)
		{
		Metric rDamage = GetDamageValue(DamageDesc::flagIncludeBonus);

		//	For shockwaves, we adjust for the fact that we might get hit multiple
		//	times.

		if (dwFlags & flagShockwaveDamage)
			rDamage *= SHOCKWAVE_DAMAGE_FACTOR;

		//	Compute result

		int iDamage10 = mathRound(rDamage * 10.0);
		int iDamage = iDamage10 / 10;
		int iDamageTenth = iDamage10 % 10;

		if (iDamageTenth == 0)
			return strPatternSubst(CONSTLIT("%s%d hp"), sDamageType, iDamage);
		else
			return strPatternSubst(CONSTLIT("%s%d.%d hp"), sDamageType, iDamage, iDamageTenth);
		}
	else
		{
		int iMin = m_Damage.GetCount() + m_Damage.GetBonus();
		int iMax = m_Damage.GetCount() * m_Damage.GetFaces() + m_Damage.GetBonus();

		char szBuffer[1024];
		if (m_iBonus == 0)
			{
			int iLen;
			if (iMin == iMax)
				iLen = wsprintf(szBuffer, "%s%d", sDamageType, iMax);
			else
				iLen = wsprintf(szBuffer, "%s%d-%d", sDamageType, iMin, iMax);

			return CString(szBuffer, iLen);
			}
		else if (m_iBonus > 0)
			{
			int iLen;
			if (iMin == iMax)
				iLen = wsprintf(szBuffer, "%s%d (+%d%%)", sDamageType, iMax, m_iBonus);
			else
				iLen = wsprintf(szBuffer, "%s%d-%d (+%d%%)", sDamageType, iMin, iMax, m_iBonus);

			return CString(szBuffer, iLen);
			}
		else
			{
			int iLen;
			if (iMin == iMax)
				iLen = wsprintf(szBuffer, "%s%d (-%d%%)", sDamageType, iMax, -m_iBonus);
			else
				iLen = wsprintf(szBuffer, "%s%d-%d (-%d%%)", sDamageType, iMin, iMax, -m_iBonus);

			return CString(szBuffer, iLen);
			}
		}
	}

int DamageDesc::GetMinDamage (void) const
	{
	return m_Damage.GetCount() + m_Damage.GetBonus();
	}

int DamageDesc::GetMaxDamage (void) const
	{
	return m_Damage.GetCount() * m_Damage.GetFaces() + m_Damage.GetBonus();
	}

int DamageDesc::GetTimeStopResistChance (int iTargetLevel) const

//	GetTimeStopResistChance
//
//	Returns the probability that an item at the given target level can resist 
//	time stop.

	{
	int iDiff = iTargetLevel - GetTimeStopDamageLevel();
	switch (iDiff)
		{
		case -3:
			return 10;

		case -2:
			return 25;

		case -1:
			return 40;

		case 0:
			return 50;

		case 1:
			return 60;

		case 2:
			return 75;

		case 3:
			return 90;

		default:
			return (iDiff < -3 ? 0 : 100);
		}
	}

ALERROR DamageDesc::LoadFromXML (SDesignLoadCtx &Ctx, const CString &sAttrib)

//	LoadFromXML
//
//	Loads damage of the form:
//
//	damagetype:1d1+1; ion6; momentum5; radiation4; deviceDisrupt6

	{
	ALERROR error;

	//	Initialize

	m_iType = damageGeneric;
	m_iBonus = 0;
	m_iCause = killedByDamage;
	m_EMPDamage = 0;
	m_MomentumDamage = 0;
	m_RadiationDamage = 0;
	m_DisintegrationDamage = 0;
	m_DeviceDisruptDamage = 0;
	m_BlindingDamage = 0;
	m_SensorDamage = 0;
	m_ShieldDamage = 0;
	m_ArmorDamage = 0;
	m_TimeStopDamage = 0;
	m_WormholeDamage = 0;
	m_FuelDamage = 0;
	m_ShieldPenetratorAdj = 0;
	m_fNoSRSFlash = false;
	m_fAutomatedWeapon = false;
	m_DeviceDamage = 0;
	m_MassDestructionAdj = 0;
	m_MiningAdj = 0;
	m_ShatterDamage = 0;

	//	Loop over all segments separated by semi-colons

	char *pPos = sAttrib.GetASCIIZPointer();
	while (true)
		{
		CString sKeyword;
		CString sValue;

		if (error = ParseTerm(Ctx, pPos, &sKeyword, &sValue, &pPos))
			{
			if (error == ERR_NOTFOUND)
				break;
			else
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Unable to parse damage: %s"), sAttrib);
				return error;
				}
			}

		if (error = LoadTermFromXML(Ctx, sKeyword, sValue))
			return error;
		}

	return NOERROR;
	}

ALERROR DamageDesc::LoadTermFromXML (SDesignLoadCtx &Ctx, const CString &sType, const CString &sArg)

//	LoadTermFromXML
//
//	Loads a damage desc term of one of the following forms:
//
//	laser:1d4
//	radiation1
//	shield:7

	{
	ALERROR error;

	//	Now figure out what to do based on the word. If this is a damage type
	//	then we load it as such.

	DamageTypes iType = LoadDamageTypeFromXML(sType);
	if (iType != damageError)
		{
		m_iType = iType;
		if (error = m_Damage.LoadFromXML(sArg))
			return error;
		}

	//	Otherwise, we expect the arg to be an integer

	else
		{
		int iCount;
		if (sArg.IsBlank())
			//	If there is no arg, then we assume 1. This handles cases where
			//	the modifier is binary, such as "disintegration".
			iCount = 1;
		else
			{
			iCount = strToInt(sArg, -1);
			if (iCount < 0)
				return ERR_FAIL;
			}

		if (strEquals(sType, GetSpecialDamageName(specialEMP)))
			m_EMPDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialRadiation)))
			m_RadiationDamage = (DWORD)Min(iCount, MAX_BINARY);
		else if (strEquals(sType, GetSpecialDamageName(specialMomentum)))
			m_MomentumDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialDisintegration)))
			m_DisintegrationDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialDeviceDisrupt)))
			m_DeviceDisruptDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialShieldPenetrator)))
			m_ShieldPenetratorAdj = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialBlinding)))
			m_BlindingDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialSensor)))
			m_SensorDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialShieldDisrupt)))
			{
			m_ShieldDamage = (BYTE)Min(iCount, MAX_ITEM_LEVEL);

			//	For versions prior to 1.1 we used a different calculation for
			//	shield level (because we only allocated 3 bits for this value

			if (Ctx.GetAPIVersion() < 3)
				m_ShieldDamage = 1 + ((m_ShieldDamage * m_ShieldDamage) / 2);
			}
		else if (strEquals(sType, GetSpecialDamageName(specialArmor)))
			m_ArmorDamage = (BYTE)Max(1, Min(iCount, MAX_ITEM_LEVEL));
		else if (strEquals(sType, GetSpecialDamageName(specialTimeStop)))
			m_TimeStopDamage = (BYTE)Max(1, Min(iCount, MAX_ITEM_LEVEL));
		else if (strEquals(sType, GetSpecialDamageName(specialWormhole)))
			m_WormholeDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialFuel)))
			m_FuelDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialWMD)))
			m_MassDestructionAdj = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialDeviceDamage)))
			m_DeviceDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialMining)))
			m_MiningAdj = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialShatter)))
			m_ShatterDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else
			return ERR_FAIL;
		}

	return NOERROR;
	}

ALERROR DamageDesc::ParseTerm (SDesignLoadCtx &Ctx, char *pPos, CString *retsKeyword, CString *retsValue, char **retpPos)

//	ParseTerm
//
//	Parses a damage term of the form:
//
//	{keyword} : {value} [;,]
//	{keyword} {numberValue} [;,]
//	{keyword} [;,]

	{
	enum States
		{
		stateStart,
		stateKeyword,
		stateLookingForSeparator,
		stateFoundSeparator,
		stateValue,
		stateDone,
		};

	CString sKeyword;
	CString sValue;
	int iState = stateStart;
	char *pStart;

	while (iState != stateDone)
		{
		switch (iState)
			{
			case stateStart:
				if (*pPos == '\0')
					return ERR_NOTFOUND;
				else if (!strIsWhitespace(pPos) && *pPos != ',' && *pPos != ';')
					{
					pStart = pPos;
					iState = stateKeyword;
					}
				break;

			case stateKeyword:
				if (*pPos == '\0' || *pPos == ';' || *pPos == ',')
					{
					sKeyword = CString(pStart, (int)(pPos - pStart));
					iState = stateDone;
					}
				else if (strIsWhitespace(pPos))
					{
					sKeyword = CString(pStart, (int)(pPos - pStart));

					//	For backwards compatibility we accept a space in 
					//	"dark acid", etc.

					if (!strEquals(sKeyword, CONSTLIT("dark")))
						iState = stateLookingForSeparator;
					}
				else if (*pPos == ':')
					{
					sKeyword = CString(pStart, (int)(pPos - pStart));
					iState = stateFoundSeparator;
					}
				else if (*pPos >= '0' && *pPos <= '9')
					{
					sKeyword = CString(pStart, (int)(pPos - pStart));
					pStart = pPos;
					iState = stateValue;
					}
				break;

			case stateLookingForSeparator:
				if (*pPos == '\0' || *pPos == ';' || *pPos == ',')
					iState = stateDone;
				else if (*pPos == ':')
					iState = stateFoundSeparator;
				else if (!strIsWhitespace(pPos))
					{
					pStart = pPos;
					iState = stateValue;
					}

				break;

			case stateFoundSeparator:
				if (*pPos == '\0')
					return ERR_FAIL;
				else if (!strIsWhitespace(pPos))
					{
					pStart = pPos;
					iState = stateValue;
					}
				break;

			case stateValue:
				if (strIsWhitespace(pPos) || *pPos == ';' || *pPos == ',' || *pPos == '\0')
					{
					sValue = CString(pStart, (int)(pPos - pStart));
					iState = stateDone;
					}
				break;
			}

		if (*pPos != '\0')
			pPos++;
		}

	//	Done

	if (retsKeyword)
		*retsKeyword = sKeyword;

	if (retsValue)
		*retsValue = sValue;

	if (retpPos)
		*retpPos = pPos;

	return NOERROR;
	}

void DamageDesc::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	Reads from a stream
//
//	DWORD		m_iType
//	DWORD		DiceRange
//	DWORD		m_iBonus
//	DWORD		m_iCause
//	DWORD		Extra damage
//	DWORD		Extra damage 2

	{
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&m_iType, sizeof(DWORD));
	m_Damage.ReadFromStream(Ctx);
	Ctx.pStream->Read((char *)&m_iBonus, sizeof(DWORD));

	if (Ctx.dwVersion >= 18)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_iCause = (DestructionTypes)dwLoad;
		}
	else
		m_iCause = killedByDamage;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_EMPDamage = dwLoad & 0x07;
	m_MomentumDamage = (dwLoad >> 3) & 0x07;
	m_RadiationDamage = (dwLoad >> 6) & 0x07;
	m_DeviceDisruptDamage = (dwLoad >> 9) & 0x07;
	m_BlindingDamage = (dwLoad >> 12) & 0x07;
	m_SensorDamage = (dwLoad >> 15) & 0x07;
	if (Ctx.dwVersion < 73)
		{
		m_ShieldDamage = (BYTE)((dwLoad >> 18) & 0x07);
		m_ShieldPenetratorAdj = 0;
		}
	else
		m_ShieldPenetratorAdj = (dwLoad >> 18) & 0x07;
	m_WormholeDamage = (dwLoad >> 21) & 0x07;
	m_FuelDamage = (dwLoad >> 24) & 0x07;
	m_DisintegrationDamage = (dwLoad >> 27) & 0x07;
	m_fNoSRSFlash = (dwLoad >> 30) & 0x01;
	m_fAutomatedWeapon = (dwLoad >> 31) & 0x01;

	//	In previous versions we had a bit that determined whether this damage
	//	came from weapons malfunction

	if (Ctx.dwVersion < 18 && ((dwLoad >> 30) & 0x01))
		m_iCause = killedByWeaponMalfunction;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_DeviceDamage = dwLoad & 0x07;
	m_MassDestructionAdj = (dwLoad >> 3) & 0x07;
	m_MiningAdj = (dwLoad >> 6) & 0x07;
	m_ShatterDamage = (dwLoad >> 9) & 0x07;
	m_dwSpare2 = 0;

	if (Ctx.dwVersion >= 73)
		{
		Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
		m_ShieldDamage = (BYTE)(dwLoad & 0xff);
		m_ArmorDamage = (BYTE)((dwLoad & 0xff00) >> 8);
		m_TimeStopDamage = (BYTE)((dwLoad & 0xff0000) >> 16);
		}
	}

int DamageDesc::RollDamage (void) const

//	RollDamage
//
//	Computes hp damage

	{
	int iDamage = m_Damage.Roll();
	if (m_iBonus > 0)
		{
		int iNum = iDamage * m_iBonus;
		int iWhole = iNum / 100;
		int iPartial = iNum % 100;
		if (mathRandom(1, 100) <= iPartial)
			iWhole++;

		return Max(0, iDamage + iWhole);
		}
	else
		return Max(0, iDamage);
	}

void DamageDesc::WriteToStream (IWriteStream *pStream) const

//	WriteToStream
//
//	Writes out to a stream
//
//	DWORD		m_iType
//	DWORD		DiceRange
//	DWORD		m_iBonus
//	DWORD		m_iCause
//	DWORD		Extra damage
//	DWORD		Extra damage 2
//	DWORD		Extra damage 3

	{
	DWORD dwSave;

	pStream->Write((char *)&m_iType, sizeof(DWORD));
	m_Damage.WriteToStream(pStream);
	pStream->Write((char *)&m_iBonus, sizeof(DWORD));

	dwSave = m_iCause;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = m_EMPDamage;
	dwSave |= m_MomentumDamage << 3;
	dwSave |= m_RadiationDamage << 6;
	dwSave |= m_DeviceDisruptDamage << 9;
	dwSave |= m_BlindingDamage << 12;
	dwSave |= m_SensorDamage << 15;
	dwSave |= m_ShieldPenetratorAdj << 18;
	dwSave |= m_WormholeDamage << 21;
	dwSave |= m_FuelDamage << 24;
	dwSave |= m_DisintegrationDamage << 27;
	dwSave |= m_fNoSRSFlash << 30;
	dwSave |= m_fAutomatedWeapon << 31;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = m_DeviceDamage;
	dwSave |= m_MassDestructionAdj << 3;
	dwSave |= m_MiningAdj << 6;
	dwSave |= m_ShatterDamage << 9;
	pStream->Write((char *)&dwSave, sizeof(DWORD));

	dwSave = (m_TimeStopDamage << 16) | (m_ArmorDamage << 8) | m_ShieldDamage;
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	}

//	DamageTypeSet --------------------------------------------------------------

ALERROR DamageTypeSet::InitFromXML (const CString &sAttrib)

//	InitFromXML
//
//	Initialize set from semi-comma separated list

	{
	int i;

	//	Blank means empty

	if (sAttrib.IsBlank())
		return NOERROR;

	//	"*" means all damage

	if (strEquals(sAttrib, CONSTLIT("*")))
		{
		for (i = 0; i < damageCount; i++)
			Add(i);
		return NOERROR;
		}

	//	Load each damage type

	TArray<CString> ArraySet;
	ParseStringList(sAttrib, 0, &ArraySet);

	for (i = 0; i < ArraySet.GetCount(); i++)
		{
		int iType = LoadDamageTypeFromXML(ArraySet[i]);
		if (iType == damageError || iType == damageGeneric)
			return ERR_FAIL;

		Add(iType);
		}

	return NOERROR;
	}

//	Damage Utilities -----------------------------------------------------------

static void AddEffectItem (CCodeChain &CC, CCLinkedList *pList, const CString &sEffect, int iTime = -1)
	{
	CCLinkedList *pNewItem = (CCLinkedList *)CC.CreateLinkedList();
	if (pNewItem->IsError())
		{
		pNewItem->Discard();
		return;
		}

	ICCItem *pField = CC.CreateString(sEffect);
	pNewItem->Append(pField);
	pField->Discard();

	if (iTime != -1)
		{
		pField = CC.CreateInteger(iTime);
		pNewItem->Append(pField);
		pField->Discard();
		}

	pList->Append(pNewItem);
	pNewItem->Discard();
	}

ICCItem *CreateItemFromDamageEffects (CCodeChain &CC, SDamageCtx &Ctx)

//	CreateItemFromDamageEffects
//
//	Packages up the damage effects from Ctx into a single item.
//
//	The item is a list of damage effects, each damage effects 
//	is one of the following:
//
//	([damage type] [damage])
//	('blinding [time in ticks])
//	('device)
//	('deviceDisrupt)
//	('disintegrate)
//	('EMP [time in ticks])
//	('radiation)
//	('reflect)
//	('shatter)

	{
	//	Short-circuit if we have no effects

	if (!Ctx.IsBlinded() && !Ctx.IsDeviceDamaged() && !Ctx.IsDeviceDisrupted() && !Ctx.IsDisintegrated()
			&& !Ctx.IsParalyzed() && !Ctx.IsRadioactive() && !Ctx.IsShotReflected()
			&& !Ctx.IsShattered())
		return CC.CreateNil();

	//	Create a list to hold the result

	CCLinkedList *pList = (CCLinkedList *)CC.CreateLinkedList();

	//	Add the damage

	if (Ctx.iDamage)
		AddEffectItem(CC, pList, CString(DAMAGE_TYPE_DATA[Ctx.Damage.GetDamageType()].pszID), Ctx.iDamage);

	//	Add effects

	if (Ctx.IsBlinded())
		AddEffectItem(CC, pList, SPECIAL_DAMAGE_BLINDING, Ctx.GetBlindTime());

	if (Ctx.IsDeviceDamaged())
		AddEffectItem(CC, pList, SPECIAL_DAMAGE_DEVICE);

	if (Ctx.IsDeviceDisrupted())
		AddEffectItem(CC, pList, SPECIAL_DAMAGE_DEVICE_DISRUPT, Ctx.GetDeviceDisruptTime());

	if (Ctx.IsDisintegrated())
		AddEffectItem(CC, pList, SPECIAL_DAMAGE_DISINTEGRATION);

	if (Ctx.IsParalyzed())
		AddEffectItem(CC, pList, SPECIAL_DAMAGE_EMP, Ctx.GetParalyzedTime());

	if (Ctx.IsRadioactive())
		AddEffectItem(CC, pList, SPECIAL_DAMAGE_RADIATION);

	if (Ctx.IsShotReflected())
		AddEffectItem(CC, pList, SPECIAL_DAMAGE_REFLECT);

	if (Ctx.IsShattered())
		AddEffectItem(CC, pList, SPECIAL_DAMAGE_SHATTER);

	return pList;
	}

void LoadDamageEffectsFromItem (ICCItem *pItem, SDamageCtx &Ctx)

//	LoadDamageEffectsFromItem
//
//	Modifies Ctx to load damage effects from the given item.
//	The item is a list of damage effects, each damage effects 
//	is one of the following:
//
//	([damage type] [damage])
//	('blinding [time in ticks])
//	('device)
//	('disintegrate)
//	('EMP [time in ticks])
//	('radiation)
//	('reflect)
//	('shatter)

	{
	int i;

	//	Initialize Ctx to defaults

	Ctx.iDamage = 0;
	Ctx.SetBlinded(false);
	Ctx.SetDeviceDamaged(false);
	Ctx.SetDeviceDisrupted(false);
	Ctx.SetDisintegrated(false);
	Ctx.SetParalyzed(false);
	Ctx.SetRadioactive(false);
	Ctx.SetShotReflected(false);
	Ctx.SetShattered(false);

	//	Keep track of whether we found normal damage or not
	//	so that we don't keep looking (to save time)

	bool bNormalDamageFound = false;

	//	Now load all the effects from the item

	for (i = 0; i < pItem->GetCount(); i++)
		{
		ICCItem *pEffect = pItem->GetElement(i);
		if (pEffect->GetCount() == 0)
			continue;

		CString sDamage = pEffect->GetElement(0)->GetStringValue();
		DamageTypes iDamageType;
		if (!bNormalDamageFound && (iDamageType = LoadDamageTypeFromXML(sDamage)) != damageError)
			{
			Ctx.iDamage = Max(0, pEffect->GetElement(1)->GetIntegerValue());
			if (iDamageType != Ctx.Damage.GetDamageType())
				{
				Ctx.Damage.SetDamageType(iDamageType);
				Ctx.Damage.SetDamage(Ctx.iDamage);
				}

			bNormalDamageFound = true;
			}
		else if (strEquals(sDamage, SPECIAL_DAMAGE_BLINDING))
			{
			Ctx.SetBlinded(true);
			if (pEffect->GetCount() >= 2)
				Ctx.SetBlindedTime(Max(0, pEffect->GetElement(1)->GetIntegerValue()));
			else
				Ctx.SetBlindedTime(DEFAULT_BLINDING_TIME);
			}
		else if (strEquals(sDamage, SPECIAL_DAMAGE_DEVICE))
			Ctx.SetDeviceDamaged(true);
		else if (strEquals(sDamage, SPECIAL_DAMAGE_DEVICE_DISRUPT))
			{
			Ctx.SetDeviceDisrupted(true);
			if (pEffect->GetCount() >= 2)
				Ctx.SetDeviceDisruptedTime(Max(0, pEffect->GetElement(1)->GetIntegerValue()));
			else
				Ctx.SetDeviceDisruptedTime(DEFAULT_DEVICE_DISRUPT_TIME);
			}
		else if (strEquals(sDamage, SPECIAL_DAMAGE_DISINTEGRATION))
			Ctx.SetDisintegrated(true);
		else if (strEquals(sDamage, SPECIAL_DAMAGE_EMP))
			{
			Ctx.SetParalyzed(true);
			if (pEffect->GetCount() >= 2)
				Ctx.SetParalyzedTime(Max(0, pEffect->GetElement(1)->GetIntegerValue()));
			else
				Ctx.SetParalyzedTime(DEFAULT_EMP_TIME);
			}
		else if (strEquals(sDamage, SPECIAL_DAMAGE_RADIATION))
			Ctx.SetRadioactive(true);
		else if (strEquals(sDamage, SPECIAL_DAMAGE_REFLECT))
			Ctx.SetShotReflected(true);
		else if (strEquals(sDamage, SPECIAL_DAMAGE_SHATTER))
			Ctx.SetShattered(true);
		}
	}

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
#define SPECIAL_DAMAGE_ATTRACT					CONSTLIT("attract")
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
#define SPECIAL_DAMAGE_REPEL					CONSTLIT("repel")
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
constexpr int MAX_STRENGTH =					100;

const Metric SHOCKWAVE_DAMAGE_FACTOR =			4.0;

struct SDamageTypeData
	{
	const char *pszID;
	int iTier;
	int iLevel;
	const char *pszName;
	const char *pszShortName;
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
	const char *pszSpecial;						//	Name as specified in a damage desc line
	const char *pszProperty;						//	Property for this special damage
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
		{	"attract",			"damageAttract" },
		{	"repel",			"damageRepel" },
		{	"miningScan",		"damageMiningScan" },

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
	else if (iType == damageNull)
		return CONSTLIT("null");
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
	else if (iType == damageNull)
		return CONSTLIT("null");
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
	else if (iType == damageNull)
		return CONSTLIT("null");
	else
		return CString(DAMAGE_TYPE_DATA[iType].pszID);
	}

//	LoadDamageTypeFromXML
//
//	Converts from string to DamageType
//
DamageTypes LoadDamageTypeFromXML (const CString &sAttrib)

	{
	for (int iType = 0; iType < damageCount; iType++)
		if (strEquals(sAttrib, CString(DAMAGE_TYPE_DATA[iType].pszID)))
			return (DamageTypes)iType;

	//	Null

	if (strEquals(sAttrib, CONSTLIT("null")))
		return damageNull;

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

//	AddEnhancements
//
//	Applies any enhancements in the given stack to the damage descriptor
//	(including bonus and special damage).
//
void DamageDesc::AddEnhancements (const CItemEnhancementStack *pEnhancements)


	{
	if (pEnhancements == NULL)
		return;

	//	Add bonus

	AddBonus(pEnhancements->GetBonus());

	//	Add special damage

	pEnhancements->ApplySpecialDamage(this);
	}

//	AsString
//
//	Represent as a string.
//
CString DamageDesc::AsString (void) const

	{
	CMemoryWriteStream Output(3000);
	if (Output.Create() != NOERROR)
		return CONSTLIT("[Out of memory]");

	Output.Write(::GetDamageType(m_iType));
	Output.Write(CONSTLIT(":"));
	Output.Write(m_Damage.SaveToXML());
	Output.Write(CONSTLIT(";"));

	if (m_Extra.MomentumDamage < 0)
		WriteValue(Output, CONSTLIT("attract"), -m_Extra.MomentumDamage);
	WriteValue(Output, CONSTLIT("armor"), m_Extra.ArmorDamage);
	WriteValue(Output, CONSTLIT("blinding"), m_Extra.BlindingDamage);
	WriteValue(Output, CONSTLIT("deviceDamage"), m_Extra.DeviceDamage);
	WriteValue(Output, CONSTLIT("deviceDisrupt"), m_Extra.DeviceDisruptDamage);
	WriteValue(Output, CONSTLIT("disintegration"), m_Extra.DisintegrationDamage);
	WriteValue(Output, CONSTLIT("EMP"), m_Extra.EMPDamage);
	WriteValue(Output, CONSTLIT("mining"), m_Extra.MiningAdj);
	WriteValue(Output, CONSTLIT("radiation"), m_Extra.RadiationDamage);
	if (m_Extra.MomentumDamage > 0)
		WriteValue(Output, CONSTLIT("repel"), m_Extra.MomentumDamage);
	WriteValue(Output, CONSTLIT("shatter"), m_Extra.ShatterDamage);
	WriteValue(Output, CONSTLIT("shield"), m_Extra.ShieldDamage);
	WriteValue(Output, CONSTLIT("shieldPenetrate"), m_Extra.ShieldPenetratorAdj);
	WriteValue(Output, CONSTLIT("timeStop"), m_Extra.TimeStopDamage);
	WriteValue(Output, CONSTLIT("WMD"), m_Extra.MassDestructionAdj);
	WriteValue(Output, CONSTLIT("miningScan"), m_Extra.fMiningScan);

	return CString(Output.GetPointer(), Output.GetLength());
	}

int DamageDesc::ConvertOldMomentum (int iValue)

//	ConvertOldMomentum
//
//	Converts from 0-7 to 0-100.

	{
	if (iValue == 0)
		return 0;

	Metric rImpulse = 250 * iValue * iValue;
	return mathRound(mathLog((rImpulse / IMPULSE_FACTOR) + 1, IMPULSE_BASE) / IMPULSE_SCALE);
	}

int DamageDesc::ConvertToOldMomentum (int iValue)

//	ConvertToOldMomentum
//
//	Converts from 0-100 to 0-7.

	{
	Metric rImpulse = IMPULSE_FACTOR * pow(IMPULSE_BASE, Absolute(iValue) * IMPULSE_SCALE) - 1.0;
	return mathRound(sqrt(rImpulse / 250.0));
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

//	IsHostile
//
//	Checks if this damage desc confers any hostile intent via status
//	effects or actual damage
//
bool DamageDesc::IsHostile () const
	{
	//	null damage is never considered hostile
	return m_iType != damageNull
		//	otherwise if we can potentially do any amount of damage
		//	then we are considered hostile
		&& (
			m_Damage.GetMaxValue()
			//	similarly any hostile status/special effects also count
			//	however, movement alone does not count as hostile
			|| m_Extra.EMPDamage
			|| m_Extra.RadiationDamage
			|| m_Extra.DeviceDamage
			|| m_Extra.DeviceDisruptDamage
			|| m_Extra.BlindingDamage
			|| m_Extra.SensorDamage
			|| m_Extra.FuelDamage
			|| m_Extra.DisintegrationDamage
			|| m_Extra.ShatterDamage
			|| m_Extra.TimeStopDamage
			);
	}

int DamageDesc::GetDamageLevel (DamageTypes iType)

//  GetDamageTier
//
//  Returns the damage tier based on damage type.

	{
	if (iType == damageGeneric || iType == damageNull)
		return 1;
	else
		return DAMAGE_TYPE_DATA[iType].iLevel;
	}

int DamageDesc::GetDamageTier (DamageTypes iType)

//  GetDamageTier
//
//  Returns the damage tier based on damage type.

	{
	if (iType == damageGeneric || iType == damageNull)
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

//  GetMassDestructionAdj
//
//  Returns the adjustment to damage given our level of mass destruction.
//
int DamageDesc::GetMassDestructionAdj (void) const


	{
	return WMD_DATA[m_Extra.MassDestructionAdj].iAdj;
	}

//  GetMassDestructionAdjFromValue
//
//  Returns the damage adj of WMD.
//
int DamageDesc::GetMassDestructionAdjFromValue (int iValue) 

	{
	return WMD_DATA[Max(0, Min(iValue, MAX_INTENSITY))].iAdj;
	}

//  GetMassDestructionLevel
//
//  Returns the display level of WMD. This is what we show in weapon stats.
//
int DamageDesc::GetMassDestructionLevel (void) const

	{
	return WMD_DATA[m_Extra.MassDestructionAdj].iLevel;
	}

//  GetMassDestructionLevel
//
//  Returns the display level of WMD. This is what we show in weapon stats.
//
int DamageDesc::GetMassDestructionLevelFromValue (int iValue) 

	{
	return WMD_DATA[Max(0, Min(iValue, MAX_INTENSITY))].iLevel;
	}

//	GetMiningWMDAdj
//
//	Returns the adjustment to damage if we treat mining as WMD.
//
int DamageDesc::GetMiningWMDAdj (void)

	{
	return WMD_DATA[m_Extra.MiningAdj].iAdj;
	}

//	GetSpecialDamage
//
//	Returns special damage level
//
int DamageDesc::GetSpecialDamage (SpecialDamageTypes iSpecial, DWORD dwFlags) const

	{
	switch (iSpecial)
		{
		case specialArmor:
			return m_Extra.ArmorDamage;

		case specialAttract:
			return (m_Extra.MomentumDamage < 0 ? -m_Extra.MomentumDamage : 0);

		case specialBlinding:
			return m_Extra.BlindingDamage;

		case specialDeviceDamage:
			return ((dwFlags & flagSpecialAdj) ? 4 * m_Extra.DeviceDamage * m_Extra.DeviceDamage : m_Extra.DeviceDamage);

		case specialDeviceDisrupt:
			return ((dwFlags & flagSpecialAdj) ? 4 * m_Extra.DeviceDisruptDamage * m_Extra.DeviceDisruptDamage : m_Extra.DeviceDisruptDamage);

		case specialDisintegration:
			return m_Extra.DisintegrationDamage;

		case specialEMP:
			return m_Extra.EMPDamage;

		case specialFuel:
			return m_Extra.FuelDamage;

		case specialMining:
			return ((dwFlags & flagSpecialAdj) ? GetMiningAdj() : m_Extra.MiningAdj);

		case specialMiningScan:
			return m_Extra.fMiningScan;

		case specialMomentum:
			return (m_Extra.MomentumDamage > 0 ? ConvertToOldMomentum(m_Extra.MomentumDamage) : 0);

		case specialRadiation:
			return m_Extra.RadiationDamage;

		case specialRepel:
			return (m_Extra.MomentumDamage > 0 ? m_Extra.MomentumDamage : 0);

		case specialShatter:
			return m_Extra.ShatterDamage;

		case specialShieldDisrupt:
			return m_Extra.ShieldDamage;

		case specialShieldPenetrator:
			return ((dwFlags & flagSpecialAdj) ? GetShieldPenetratorAdj() : m_Extra.ShieldPenetratorAdj);

		case specialTimeStop:
			return m_Extra.TimeStopDamage;

		case specialWMD:
			if (dwFlags & flagSpecialAdj)
				return GetMassDestructionAdj();
			else if (dwFlags & flagSpecialLevel)
				return GetMassDestructionLevel();
			else
				return m_Extra.MassDestructionAdj;

		default:
			return 0;
		}
	}

//	GetSpecialDamageFromCondition
//
//	Returns the special damage that causes the given condition.
//
SpecialDamageTypes DamageDesc::GetSpecialDamageFromCondition (ECondition iCondition)

	{
	switch (iCondition)
		{
		case ECondition::blind:
			return specialBlinding;

		case ECondition::paralyzed:
			return specialEMP;

		case ECondition::radioactive:
			return specialRadiation;

		case ECondition::timeStopped:
			return specialTimeStop;

		default:
			return specialNone;
		}
	}

//	GetSpecialDamageName
//
//	Returns the name
//
CString DamageDesc::GetSpecialDamageName (SpecialDamageTypes iSpecial)

	{
	return CString(SPECIAL_DAMAGE_DATA[iSpecial].pszSpecial, -1, TRUE);
	}

//	HasMomentumDamage
//
//	Returns TRUE if we have momuntum damage and optionally returns the size of
//	the force/second (impulse, positive is a push away from source; negative is 
//	a pull).
//
bool DamageDesc::HasImpulseDamage (Metric *retrImpulse) const

	{
	if ( m_Extra.MomentumDamage)
		{
		if (retrImpulse)
			{
			Metric rStrength = IMPULSE_FACTOR * (pow(IMPULSE_BASE, IMPULSE_SCALE * Absolute(m_Extra.MomentumDamage)) - 1.0);
			*retrImpulse = (m_Extra.MomentumDamage > 0 ? rStrength : -rStrength);
			}

		return true;
		}

	return false;
	}

//	InterpolateTo
//
//	Alters the descriptor to be between this descriptor and End, where 0.0 is
//	this descriptor and 1.0 is End.
//
void DamageDesc::InterpolateTo (const DamageDesc &End, Metric rSlider)

	{
	//	Figure out how much the average damage value changes.

	const Metric rFromDamage = m_Damage.GetAveValueFloat();
	if (rFromDamage > 0.0)
		{
		const Metric rToDamage = End.m_Damage.GetAveValueFloat();
		const Metric rRange = rToDamage - rFromDamage;
		const Metric rInterDamage = rFromDamage + (rRange * rSlider);
		m_Damage.Scale(rInterDamage / rFromDamage);
		}

	m_Extra.EMPDamage = InterpolateValue(m_Extra.EMPDamage, End.m_Extra.EMPDamage, rSlider);
	m_Extra.RadiationDamage = InterpolateValue(m_Extra.RadiationDamage, End.m_Extra.RadiationDamage, rSlider);
	m_Extra.DeviceDisruptDamage = InterpolateValue(m_Extra.DeviceDisruptDamage, End.m_Extra.DeviceDisruptDamage, rSlider);
	m_Extra.BlindingDamage = InterpolateValue(m_Extra.BlindingDamage, End.m_Extra.BlindingDamage, rSlider);
	m_Extra.SensorDamage = InterpolateValue(m_Extra.SensorDamage, End.m_Extra.SensorDamage, rSlider);
	m_Extra.WormholeDamage = InterpolateValue(m_Extra.WormholeDamage, End.m_Extra.WormholeDamage, rSlider);
	m_Extra.FuelDamage = InterpolateValue(m_Extra.FuelDamage, End.m_Extra.FuelDamage, rSlider);
	m_Extra.DisintegrationDamage = InterpolateValue(m_Extra.DisintegrationDamage, End.m_Extra.DisintegrationDamage, rSlider);
	m_Extra.ShieldPenetratorAdj = InterpolateValue(m_Extra.ShieldPenetratorAdj, End.m_Extra.ShieldPenetratorAdj, rSlider);
	m_Extra.MassDestructionAdj = InterpolateValue(m_Extra.MassDestructionAdj, End.m_Extra.MassDestructionAdj, rSlider);
	m_Extra.DeviceDamage = InterpolateValue(m_Extra.DeviceDamage, End.m_Extra.DeviceDamage, rSlider);
	m_Extra.MiningAdj = InterpolateValue(m_Extra.MiningAdj, End.m_Extra.MiningAdj, rSlider);
	m_Extra.ShatterDamage = InterpolateValue(m_Extra.ShatterDamage, End.m_Extra.ShatterDamage, rSlider);
	m_Extra.ShieldDamage = (BYTE)InterpolateValue(m_Extra.ShieldDamage, End.m_Extra.ShieldDamage, rSlider);
	m_Extra.ArmorDamage = (BYTE)InterpolateValue(m_Extra.ArmorDamage, End.m_Extra.ArmorDamage, rSlider);
	m_Extra.TimeStopDamage = (BYTE)InterpolateValue(m_Extra.TimeStopDamage, End.m_Extra.TimeStopDamage, rSlider);
	m_Extra.MomentumDamage = (INT8)InterpolateValue(m_Extra.MomentumDamage, End.m_Extra.MomentumDamage, rSlider);
	}

//	IsDamaging
// 
//	Returns true if the damage the potential to do HP Damage or has
//	harmful secondary effects
//	
//	Optionally takes an arg iDamage. If this is supplied, it is used
//  in place of the max value check.
//
bool DamageDesc::IsDamaging() const
	{
	return (m_Damage.GetMaxValue() && m_iType != damageNull)
		//	similarly any hostile status/special effects also count
		//	however, movement alone does not count as hostile
		|| m_sExtra.EMPDamage
		|| m_sExtra.RadiationDamage
		|| m_sExtra.DeviceDamage
		|| m_sExtra.DeviceDisruptDamage
		|| m_sExtra.BlindingDamage
		|| m_sExtra.SensorDamage
		|| m_sExtra.FuelDamage
		|| m_sExtra.DisintegrationDamage
		|| m_sExtra.ShatterDamage
		|| m_sExtra.TimeStopDamage
		;
	}

//	InterpolateValue
//
//	Interpolates from one value to another.
//
int DamageDesc::InterpolateValue (int iFrom, int iTo, Metric rSlider)

	{
	if (iFrom == iTo)
		return iFrom;
	else
		{
		int iRange = iTo - iFrom;
		return iFrom + mathRound(rSlider * iRange);
		}
	}

//	SetSpecialDamage
//
//	Sets special damage
//
void DamageDesc::SetSpecialDamage (SpecialDamageTypes iSpecial, int iLevel)

	{
	switch (iSpecial)
		{
		case specialAttract:
			m_Extra.MomentumDamage = -Max(0, Min(iLevel, MAX_STRENGTH));
			break;

		case specialArmor:
			m_Extra.ArmorDamage = Max(1, Min(iLevel, MAX_ITEM_LEVEL));
			break;

		case specialBlinding:
			m_Extra.BlindingDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialDeviceDamage:
			m_Extra.DeviceDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialDeviceDisrupt:
			m_Extra.DeviceDisruptDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialDisintegration:
			m_Extra.DisintegrationDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialEMP:
			m_Extra.EMPDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialFuel:
			m_Extra.FuelDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialMining:
			m_Extra.MiningAdj = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialMomentum:
			m_Extra.MomentumDamage = ConvertOldMomentum(Max(0, Min(iLevel, MAX_INTENSITY)));
			break;

		case specialRadiation:
			m_Extra.RadiationDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialRepel:
			m_Extra.MomentumDamage = Max(0, Min(iLevel, MAX_STRENGTH));
			break;

		case specialShatter:
			m_Extra.ShatterDamage = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialShieldDisrupt:
			m_Extra.ShieldDamage = Max(1, Min(iLevel, MAX_ITEM_LEVEL));
			break;

		case specialShieldPenetrator:
			m_Extra.ShieldPenetratorAdj = Max(0, Min(iLevel, MAX_INTENSITY));
			break;

		case specialTimeStop:
			m_Extra.TimeStopDamage = Max(1, Min(iLevel, MAX_ITEM_LEVEL));
			break;

		case specialWMD:
			m_Extra.MassDestructionAdj = Max(0, Min(iLevel, MAX_INTENSITY));
			break;
		}
	}

//	SetDamage
//
//	Sets a constant damage
//
void DamageDesc::SetDamage (int iDamage)

	{
	m_Damage = DiceRange(0, 0, iDamage);
	}

//	GetDesc
//
//	Returns a description of the damage:
//
//	laser 1-4 (+50%)
//
CString DamageDesc::GetDesc (DWORD dwFlags)

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
				iLen = wsprintf(szBuffer, "%s%d", (LPSTR)sDamageType, iMax);
			else
				iLen = wsprintf(szBuffer, "%s%d-%d", (LPSTR)sDamageType, iMin, iMax);

			return CString(szBuffer, iLen);
			}
		else if (m_iBonus > 0)
			{
			int iLen;
			if (iMin == iMax)
				iLen = wsprintf(szBuffer, "%s%d (+%d%%)", (LPSTR)sDamageType, iMax, m_iBonus);
			else
				iLen = wsprintf(szBuffer, "%s%d-%d (+%d%%)", (LPSTR)sDamageType, iMin, iMax, m_iBonus);

			return CString(szBuffer, iLen);
			}
		else
			{
			int iLen;
			if (iMin == iMax)
				iLen = wsprintf(szBuffer, "%s%d (-%d%%)", (LPSTR)sDamageType, iMax, -m_iBonus);
			else
				iLen = wsprintf(szBuffer, "%s%d-%d (-%d%%)", (LPSTR)sDamageType, iMin, iMax, -m_iBonus);

			return CString(szBuffer, iLen);
			}
		}
	}

CString DamageDesc::GetDPSDesc (Metric rFireRate, Metric rMultiplier, DWORD dwFlags)

//	GetDesc
//
//	Returns a description of the damage:
//
//	laser 15 hp/sec (+50%)

	{
	CString sDamageType;
	if (!(dwFlags & flagNoDamageType))
		sDamageType = strPatternSubst(CONSTLIT("%s "), GetDamageShortName(m_iType));

	//	We always use average damage computation for DPS

	Metric rDamage = GetDamageValue(DamageDesc::flagIncludeBonus) * rFireRate * rMultiplier;

	//	For shockwaves, we adjust for the fact that we might get hit multiple
	//	times.

	if (dwFlags & flagShockwaveDamage)
		rDamage *= SHOCKWAVE_DAMAGE_FACTOR;

	//	Compute result

	int iDamage10 = mathRound(rDamage * 10.0);
	int iDamage = iDamage10 / 10;
	int iDamageTenth = iDamage10 % 10;

	if (iDamageTenth == 0)
		return strPatternSubst(CONSTLIT("%s%d hp/sec"), sDamageType, iDamage);
	else
		return strPatternSubst(CONSTLIT("%s%d.%d hp/sec"), sDamageType, iDamage, iDamageTenth);
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

	m_iType = damageNull;
	m_iBonus = 0;
	m_iCause = killedByDamage;

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
	
	//	Handle backwards compatibility

	DWORD dwAPIVersion = Ctx.GetAPIVersion();

	//	For APIs 48-56, genericDamage mining weapons scanned instead of mined
	//	Prior to API 48, the concept of mining scanning didnt exist

	if (dwAPIVersion >= 48 && dwAPIVersion < 57 && m_Extra.MiningAdj && m_iType == damageGeneric)
		m_Extra.fMiningScan = 1;

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
			m_Extra.EMPDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialRadiation)))
			m_Extra.RadiationDamage = (DWORD)Min(iCount, MAX_BINARY);
		else if (strEquals(sType, GetSpecialDamageName(specialDisintegration)))
			m_Extra.DisintegrationDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialDeviceDisrupt)))
			m_Extra.DeviceDisruptDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialShieldPenetrator)))
			m_Extra.ShieldPenetratorAdj = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialBlinding)))
			m_Extra.BlindingDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialSensor)))
			m_Extra.SensorDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialShieldDisrupt)))
			{
			m_Extra.ShieldDamage = (BYTE)Min(iCount, MAX_ITEM_LEVEL);

			//	For versions prior to 1.1 we used a different calculation for
			//	shield level (because we only allocated 3 bits for this value

			if (Ctx.GetAPIVersion() < 3)
				m_Extra.ShieldDamage = 1 + ((m_Extra.ShieldDamage * m_Extra.ShieldDamage) / 2);
			}
		else if (strEquals(sType, GetSpecialDamageName(specialArmor)))
			m_Extra.ArmorDamage = (BYTE)Max(1, Min(iCount, MAX_ITEM_LEVEL));
		else if (strEquals(sType, GetSpecialDamageName(specialTimeStop)))
			m_Extra.TimeStopDamage = (BYTE)Max(1, Min(iCount, MAX_ITEM_LEVEL));
		else if (strEquals(sType, GetSpecialDamageName(specialWormhole)))
			m_Extra.WormholeDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialFuel)))
			m_Extra.FuelDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialWMD)))
			m_Extra.MassDestructionAdj = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialDeviceDamage)))
			m_Extra.DeviceDamage = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialMining)))
			m_Extra.MiningAdj = (DWORD)Min(iCount, MAX_INTENSITY);
		else if (strEquals(sType, GetSpecialDamageName(specialShatter)))
			m_Extra.ShatterDamage = (DWORD)Min(iCount, MAX_INTENSITY);

		else if (strEquals(sType, GetSpecialDamageName(specialMiningScan)))
			{
			m_Extra.fMiningScan = (DWORD)Min(iCount, MAX_BINARY);

			//	MiningScan:# automatically enables m_MiningAdj if m_MiningAdj is 0
			//	We dont care if miningScan: was processed before mining:, because
			//	mining will override the value that m_fMiningScan sets.

			if (!m_Extra.MiningAdj)
				m_Extra.MiningAdj = (DWORD)Min(iCount, MAX_INTENSITY);
			}

		//	These special damage types translate to momentum

		else if (strEquals(sType, GetSpecialDamageName(specialMomentum)))
			m_Extra.MomentumDamage = ConvertOldMomentum(Min(iCount, MAX_INTENSITY));
		else if (strEquals(sType, GetSpecialDamageName(specialAttract)))
			m_Extra.MomentumDamage = -Min(iCount, MAX_STRENGTH);
		else if (strEquals(sType, GetSpecialDamageName(specialRepel)))
			m_Extra.MomentumDamage = Min(iCount, MAX_STRENGTH);
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
//	DWORD			m_iType
//	DWORD			DiceRange
//	DWORD			m_iBonus
//	DWORD			m_iCause
//	SExtraDamage	m_Extra

	{
	DWORD dwLoad;

	Ctx.pStream->Read(dwLoad);
	m_iType = (DamageTypes)dwLoad;

	m_Damage.ReadFromStream(Ctx);
	Ctx.pStream->Read(m_iBonus);

	if (Ctx.dwVersion >= 18)
		{
		Ctx.pStream->Read(dwLoad);
		m_iCause = (DestructionTypes)dwLoad;
		}
	else
		m_iCause = killedByDamage;

	//	Extra damage struct
	//	
	//	starting in save ver 216+, we read this struct to/from memory

	if (Ctx.dwVersion >= 216)
		{
		Ctx.pStream->Read((char *)&m_Extra, sizeof(SExtraDamage));
		}

	//	legacy loading logic for versions below 216
	// 
	//	DWORD		Extra damage
	//	DWORD		Extra damage 2
	//	DWORD		Extra damage 3

	else
		{
		//	Extra damage 1

		Ctx.pStream->Read(dwLoad);
		m_Extra.EMPDamage = dwLoad & 0x07;
		BYTE byOldMomentumDamage = (Ctx.dwVersion < 190 ? ((dwLoad >> 3) & 0x07) : 0);
		m_Extra.RadiationDamage = (dwLoad >> 6) & 0x07;
		m_Extra.DeviceDisruptDamage = (dwLoad >> 9) & 0x07;
		m_Extra.BlindingDamage = (dwLoad >> 12) & 0x07;
		m_Extra.SensorDamage = (dwLoad >> 15) & 0x07;
		if (Ctx.dwVersion < 73)
			{
			m_Extra.ShieldDamage = (BYTE)((dwLoad >> 18) & 0x07);
			m_Extra.ShieldPenetratorAdj = 0;
			}
		else
			m_Extra.ShieldPenetratorAdj = (dwLoad >> 18) & 0x07;
		m_Extra.WormholeDamage = (dwLoad >> 21) & 0x07;
		m_Extra.FuelDamage = (dwLoad >> 24) & 0x07;
		m_Extra.DisintegrationDamage = (dwLoad >> 27) & 0x07;
		m_Extra.fNoSRSFlash = (dwLoad >> 30) & 0x01;
		m_Extra.fAutomatedWeapon = (dwLoad >> 31) & 0x01;

		//	In previous versions we had a bit that determined whether this damage
		//	came from weapons malfunction

		if (Ctx.dwVersion < 18 && ((dwLoad >> 30) & 0x01))
			m_iCause = killedByWeaponMalfunction;

		//	Extra Damage 2

		Ctx.pStream->Read(dwLoad);
		m_Extra.DeviceDamage = dwLoad & 0x07;
		m_Extra.MassDestructionAdj = (dwLoad >> 3) & 0x07;
		m_Extra.MiningAdj = (dwLoad >> 6) & 0x07;
		m_Extra.ShatterDamage = (dwLoad >> 9) & 0x07;

		if (Ctx.dwVersion >= 215)
			m_Extra.fMiningScan = (dwLoad >> 10) & 0x01;

		//	In API 48, mining scan was done by using miningAdj + damageGeneric

		else if (Ctx.dwVersion >= 177)
			{
			//	177 corresponds to 1.9a4 (API48)
			m_Extra.fMiningScan = (m_iType == damageGeneric && m_Extra.MiningAdj) ? 1 : 0;
			}

		//	Mining scan was not a feature prior to 1.9a4

		else
			m_Extra.fMiningScan = 0;

		m_Extra.dwSpare2 = 0;

		//	Extra Damage 3

		if (Ctx.dwVersion >= 73)
			{
			Ctx.pStream->Read(dwLoad);
			m_Extra.ShieldDamage = (BYTE)(dwLoad & 0xff);
			m_Extra.ArmorDamage = (BYTE)((dwLoad & 0xff00) >> 8);
			m_Extra.TimeStopDamage = (BYTE)((dwLoad & 0xff0000) >> 16);
			}

		if (Ctx.dwVersion >= 190)
			m_Extra.MomentumDamage = (INT8)(BYTE)((dwLoad & 0xff000000) >> 24);
		else
			m_Extra.MomentumDamage = ConvertOldMomentum(byOldMomentumDamage);
		}
	}


//	RollDamage
//
//	Computes hp damage
//
int DamageDesc::RollDamage (void) const

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

//	WriteToStream
//
//	Writes out to a stream
//
//	DWORD			m_iType
//	DWORD			DiceRange
//	DWORD			m_iBonus
//	DWORD			m_iCause
//	SExtraDamage	Extra damage
//
void DamageDesc::WriteToStream (IWriteStream *pStream) const

	{
	DWORD dwSave;

	pStream->Write(m_iType);
	m_Damage.WriteToStream(pStream);
	pStream->Write(m_iBonus);

	dwSave = m_iCause;
	pStream->Write(dwSave);

	//	Extra Damage
	pStream->Write((char *)&m_Extra, sizeof(SExtraDamage));
	}

//	WriteValue
//
//	Writes the given value as a string.
//
void DamageDesc::WriteValue (CMemoryWriteStream &Stream, const CString &sField, int iValue)

	{
	if (iValue)
		Stream.Write(strPatternSubst(CONSTLIT(" %s:%d;"), sField, iValue));
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
		if (iType < damageMinListed)
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

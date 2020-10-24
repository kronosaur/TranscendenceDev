//	CAsteroidDesc.cpp
//
//	CAsteroidDesc class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ASTEROID_TAG							CONSTLIT("Asteroid")

#define ASTEROID_TYPE_ATTRIB					CONSTLIT("asteroidType")
#define TYPE_ATTRIB								CONSTLIT("type")

TStaticStringTable<TStaticStringEntry<EAsteroidType>, 5> CAsteroidDesc::COMPOSITION_INDEX = {
	"icy",					EAsteroidType::icy,
	"metallic",				EAsteroidType::metallic,
	"primordial",			EAsteroidType::primordial,
	"rocky",				EAsteroidType::rocky,
	"volcanic",				EAsteroidType::volcanic,
	};

CAsteroidDesc::SCompositionDesc CAsteroidDesc::COMPOSITION_TABLE[EAsteroidTypeCount] = 
	{	
		//	ID			Mining Difficulty
		{	"none",			0,

			//	ablation	drill		explosion	shockwave
			{	1.0,		1.0,		1.0,		1.0		},	//	Success Adj
			{	1.0,		1.0,		1.0,		1.0		},	//	Yield Adj
		},

		{	"icy",			25,

			//	ablation	drill		explosion	shockwave
			{	2.0,		1.0,		1.0,		0.5		},	//	Success Adj
			{	1.25,		1.5,		1.0,		0.5		},	//	Yield Adj
		},

		{	"metallic",		100,

			//	ablation	drill		explosion	shockwave
			{	1.0,		1.0,		0.5,		2.0		},	//	Success Adj
			{	1.5,		1.0,		0.5,		1.25	},	//	Yield Adj
		},
		{	"primordial",	50,

			//	ablation	drill		explosion	shockwave
			{	0.5,		2.0,		1.0,		1.0		},	//	Success Adj
			{	0.5,		1.25,		1.5,		1.0		},	//	Yield Adj
		},
		{	"rocky",		50,

			//	ablation	drill		explosion	shockwave
			{	1.0,		1.0,		1.0,		1.0		},	//	Success Adj
			{	1.0,		1.0,		1.0,		1.0		},	//	Yield Adj
		},
		{	"volcanic",		75,

			//	ablation	drill		explosion	shockwave
			{	1.0,		0.5,		2.0,		1.0		},	//	Success Adj
			{	1.0,		0.5,		1.25,		1.5		},	//	Yield Adj
		},
	};

const CAsteroidDesc CAsteroidDesc::m_Null;

Metric CAsteroidDesc::CalcBaseMiningSuccess (int iMiningLevel, int iMiningDifficulty)

//	CalcBaseMiningSuccess
//
//	Computes the probability of mining success (0-1.0) based on mining level and
//	mining difficulty.

	{
	if (iMiningDifficulty <= 0)
		return Max(0.0, Min(iMiningLevel / 100.0, 1.0));
	else if (iMiningLevel <= 0)
		return 0.0;
	else
		return Max(0.0, Min(2.0 * (iMiningLevel * iMiningLevel) / (100.0 * (iMiningLevel + iMiningDifficulty)), 1.0));
	}

int CAsteroidDesc::CalcMaxOreLevel (DamageTypes iDamageType)

//	CalcMaxOreLevel
//
//	Returns the maximum ore level that can be extracted by the given damage 
//	type.

	{
	switch (iDamageType)
		{
		case damageLaser:
		case damageKinetic:
			return 4;

		case damageParticle:
		case damageBlast:
			return 7;

		case damageIonRadiation:
		case damageThermonuclear:
			return 10;

		case damagePositron:
		case damagePlasma:
			return 13;

		case damageAntiMatter:
		case damageNano:
			return 16;

		case damageGravitonBeam:
		case damageSingularity:
			return 19;

		case damageDarkAcid:
		case damageDarkSteel:
			return 22;

		case damageDarkLightning:
		case damageDarkFire:
			return 25;

		default:
			return 0;
		}
	}

void CAsteroidDesc::CalcMining (int iMiningLevel, int iMiningDifficulty, EAsteroidType iType, const SDamageCtx &DamageCtx, SMiningStats &retMining)

//	CalcMining
//
//	Calculate mining stats.

	{
	//	Chance of success is based on mining level, mining difficulty, mining
	//	method (type of weapon) and asteroid type.

	const SCompositionDesc &CompositionDesc = GetCompositionDesc(iType);
	const EMiningMethod iMiningMethod = CalcMiningMethod(DamageCtx.GetDesc());
	const Metric rBaseSuccess = CalcBaseMiningSuccess(iMiningLevel, iMiningDifficulty);

	retMining.iSuccessChance = CMath::Round(100.0 * CMath::AdjustChance(rBaseSuccess, CompositionDesc.SuccessAdj[static_cast<int>(iMiningMethod)]));

	//	Yield adjustment is based on asteroid type and mining method.

	retMining.rYieldAdj = CompositionDesc.YieldAdj[static_cast<int>(iMiningMethod)];

	//	Max ore level is the maximum level of ore that we can extract with the 
	//	given shot.

	retMining.iMaxOreLevel = CalcMaxOreLevel(DamageCtx.Damage.GetDamageType());
	}

EMiningMethod CAsteroidDesc::CalcMiningMethod (const CWeaponFireDesc &Desc)

//	CalcMiningMethod
//
//	Compute the mining method based on shot descriptor

	{
	switch (Desc.GetFireType())
		{
		case CWeaponFireDesc::ftArea:
			return EMiningMethod::shockwave;

		case CWeaponFireDesc::ftParticles:
		case CWeaponFireDesc::ftRadius:
			return EMiningMethod::ablation;

		case CWeaponFireDesc::ftBeam:
		case CWeaponFireDesc::ftContinuousBeam:
			return EMiningMethod::drill;

		default:
			{
			constexpr Metric DRILL_SPEED_THRESHOLD = 0.85 * LIGHT_SPEED;

			if (::IsEnergyDamage(Desc.GetDamageType()) 
					&& Desc.GetAveSpeed() >= DRILL_SPEED_THRESHOLD)
				return EMiningMethod::drill;
			else
				return EMiningMethod::explosion;
			}
		}
	}

CString CAsteroidDesc::CompositionID (EAsteroidType iType)

//	CompositionID
//
//	Returns an ID string for the asteroid type.

	{
	if (iType == EAsteroidType::unknown)
		return CONSTLIT("unknown");

	return CString(GetCompositionDesc(iType).pszID, -1, TRUE);
	}

int CAsteroidDesc::GetDefaultMiningDifficulty (EAsteroidType iType)

//	GetDefaultMiningDifficulty
//
//	Returns the default mining difficulty for the given asteroid type.

	{
	if (iType == EAsteroidType::unknown)
		return 0;

	return GetCompositionDesc(iType).iMiningDifficulty;
	}

CString CAsteroidDesc::GetTypeLabel (CUniverse &Universe) const

//	GetTypeLabel
//
//	Returns the human readable name for the asteroid type.

	{
	return Universe.GetEngineLanguage().GetAsteroidTypeLabel(m_iType);
	}

ALERROR CAsteroidDesc::InitFromStationTypeXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromStationTypeXML
//
//	Initializes from XML.

	{
	if (const CXMLElement *pChild = Desc.GetContentElementByTag(ASTEROID_TAG))
		{
		return InitFromXML(Ctx, *pChild);
		}
	else
		{
		CString sValue;
		if (Desc.FindAttribute(ASTEROID_TYPE_ATTRIB, &sValue) 
				&& !sValue.IsBlank())
			{
			EAsteroidType iType = ParseComposition(sValue);
			if (iType == EAsteroidType::unknown)
				{
				Ctx.sError = strPatternSubst(CONSTLIT("Unknown asteroidType: %s"), sValue);
				return ERR_FAIL;
				}

			m_iType = iType;
			}
		else
			{
			m_iType = EAsteroidType::none;
			m_bDefault = true;
			}
		}

	return NOERROR;
	}

ALERROR CAsteroidDesc::InitFromXML (SDesignLoadCtx &Ctx, const CXMLElement &Desc)

//	InitFromXML
//
//	Initializes from an <Asteroid> element.

	{
	CString sValue;
	if (Desc.FindAttribute(TYPE_ATTRIB, &sValue) 
			&& !sValue.IsBlank())
		{
		EAsteroidType iType = ParseComposition(sValue);
		if (iType == EAsteroidType::unknown)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Unknown asteroidType: %s"), sValue);
			return ERR_FAIL;
			}

		m_iType = iType;
		}
	else
		{
		m_iType = EAsteroidType::none;
		m_bDefault = true;
		}

	return NOERROR;
	}

EAsteroidType CAsteroidDesc::ParseComposition (const CString &sValue)

//	ParseComposition
//
//	Parses a composition value.

	{
	auto pEntry = COMPOSITION_INDEX.GetAt(sValue);
	if (pEntry == NULL)
		return EAsteroidType::unknown;
	else
		return pEntry->Value;
	}

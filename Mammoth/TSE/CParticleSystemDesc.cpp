//	CParticleSystemDesc.cpp
//
//	CParticleSystemDesc object
//	Copyright (c) 2015 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PARTICLE_EFFECT_TAG						CONSTLIT("ParticleEffect")

#define EMIT_LIFETIME_ATTRIB					CONSTLIT("emitLifetime")
#define EMIT_RATE_ATTRIB						CONSTLIT("emitRate")
#define EMIT_SPEED_ATTRIB						CONSTLIT("emitSpeed")
#define EMIT_WIDTH_ATTRIB						CONSTLIT("emitWidth")
#define MISS_CHANCE_ATTRIB						CONSTLIT("missChance")
#define PARTICLE_EFFECT_ATTRIB					CONSTLIT("particleEffect")
#define PARTICLE_LIFETIME_ATTRIB				CONSTLIT("particleLifetime")
#define SPLASH_CHANCE_ATTRIB					CONSTLIT("splashChance")
#define SPREAD_ANGLE_ATTRIB						CONSTLIT("spreadAngle")
#define STYLE_ATTRIB							CONSTLIT("style")
#define XFORM_ROTATION_ATTRIB					CONSTLIT("xformRotation")
#define XFORM_TIME_ATTRIB						CONSTLIT("xformTime")

#define PARTICLE_COUNT_ATTRIB					CONSTLIT("particleCount")
#define PARTICLE_EMIT_TIME_ATTRIB				CONSTLIT("particleEmitTime")
#define PARTICLE_MISS_CHANCE_ATTRIB				CONSTLIT("particleMissChance")
#define PARTICLE_SPLASH_CHANCE_ATTRIB			CONSTLIT("particleSplashChance")
#define PARTICLE_SPREAD_ANGLE_ATTRIB			CONSTLIT("particleSpreadAngle")
#define PARTICLE_SPREAD_WIDTH_ATTRIB			CONSTLIT("particleSpreadWidth")

static LPSTR STYLE_TABLE[] =
	{
	//	Must be same order as EStyles
		"",

		"amorphous",
		"comet",
		"exhaust",
		"jet",
		"radiate",
		"spray",
		"writhe",
		"brownian",

		NULL,
	};

CParticleSystemDesc::CParticleSystemDesc (void) :
		m_iStyle(styleJet),
		m_iEmitChance(100),
		m_EmitSpeed(6, 1, 0),
		m_EmitRate(6, 1, 0),
		m_EmitLifetime(6, 1, 0),
		m_EmitWidth(0, 0, 0),
		m_ParticleLifetime(6, 1, 0),
		m_Radius(0, 0, 64),
		m_SpreadAngle(0, 0, 0),
		m_iXformRotation(0),
		m_rXformTime(1.0),
		m_rCohesionFactor(0.0),
		m_rWakeFactor(0.0),
		m_iSplashChance(0),
		m_iMissChance(0),
		m_bHasWake(false),
		m_bSprayCompatible(false),
		m_bFixedPos(false),
		m_bTrackingObject(false)

//	CParticleSystemDesc constructor

	{
	}

ALERROR CParticleSystemDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	InitFromXML
//
//	Initializes from XML element.

	{
	ALERROR error;

	DWORD dwID;
	CString sAttrib = pDesc->GetAttribute(STYLE_ATTRIB);
	if (CEffectParamDesc::FindIdentifier(sAttrib, STYLE_TABLE, &dwID))
		m_iStyle = (EStyles)dwID;
	else if (!sAttrib.IsBlank())
		{
		Ctx.sError = strPatternSubst(CONSTLIT("Invalid style: %s"), sAttrib);
		return ERR_FAIL;
		}

	if (error = m_EmitRate.LoadFromXML(pDesc->GetAttribute(EMIT_RATE_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Invalid particleCount count.");
		return error;
		}

	if (error = m_EmitSpeed.LoadFromXML(pDesc->GetAttribute(EMIT_SPEED_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Invalid emitSpeed value.");
		return error;
		}

	if (error = m_EmitLifetime.LoadFromXML(pDesc->GetAttribute(EMIT_LIFETIME_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Invalid emitTime.");
		return error;
		}

	if (error = m_EmitWidth.LoadFromXML(pDesc->GetAttribute(EMIT_WIDTH_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Invalid emitWidth.");
		return error;
		}

	if (error = m_ParticleLifetime.LoadFromXML(pDesc->GetAttribute(PARTICLE_LIFETIME_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Invalid particleLifetime.");
		return error;
		}

	if (error = m_SpreadAngle.LoadFromXML(pDesc->GetAttribute(SPREAD_ANGLE_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Invalid spreadAngle.");
		return error;
		}

	m_iXformRotation = AngleMod(pDesc->GetAttributeInteger(XFORM_ROTATION_ATTRIB));
	m_rXformTime = pDesc->GetAttributeIntegerBounded(XFORM_TIME_ATTRIB, 0, -1, 100) / 100.0;

	m_iMissChance = pDesc->GetAttributeIntegerBounded(MISS_CHANCE_ATTRIB, 0, 100, 0);
	m_iSplashChance = pDesc->GetAttributeIntegerBounded(SPLASH_CHANCE_ATTRIB, 0, 100, 0);

	//	Load the effect to use

	if (error = m_pParticleEffect.LoadEffect(Ctx, 
			strPatternSubst("%s:p", sUNID),
			pDesc->GetContentElementByTag(PARTICLE_EFFECT_TAG),
			pDesc->GetAttribute(PARTICLE_EFFECT_ATTRIB)))
		return error;

	return NOERROR;
	}

ALERROR CParticleSystemDesc::InitFromWeaponDescXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	InitFromWeaponDescXML
//
//	For backwards compatibility, we allow loading from a weapon descriptor

	{
	ALERROR error;

	if (error = InitFromXML(Ctx, pDesc, sUNID))
		return error;

	//	For weapon descriptors we cannot use styleJet, so we convert to 
	//	styleSpray.

	if (m_iStyle == styleJet)
		{
		if (g_pUniverse->InDebugMode())
			::kernelDebugLogPattern("%s: style='jet' is not valid for weapons; using style='spray' instead.", sUNID);

		m_iStyle = styleSpray;
		}

	//	Done

	return NOERROR;
	}

ALERROR CParticleSystemDesc::InitFromWeaponDescXMLCompatible (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromWeaponDescXMLCompatible
//
//	For backwards compatibility, we allow loading from a weapon descriptor

	{
	ALERROR error;

	m_iStyle = styleSpray;

	if (error = m_EmitRate.LoadFromXML(pDesc->GetAttribute(PARTICLE_COUNT_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Invalid particleCount count.");
		return error;
		}

	if (error = m_EmitLifetime.LoadFromXML(pDesc->GetAttribute(PARTICLE_EMIT_TIME_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Invalid particleEmitTime.");
		return error;
		}

	if (error = m_EmitWidth.LoadFromXML(pDesc->GetAttribute(PARTICLE_SPREAD_WIDTH_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Invalid particleSpreadWidth.");
		return error;
		}

	if (error = m_SpreadAngle.LoadFromXML(pDesc->GetAttribute(PARTICLE_SPREAD_ANGLE_ATTRIB)))
		{
		Ctx.sError = CONSTLIT("Invalid particleSpreadAngle.");
		return error;
		}

	m_iSplashChance = pDesc->GetAttributeIntegerBounded(PARTICLE_SPLASH_CHANCE_ATTRIB, 0, 100, 0);
	m_iMissChance = pDesc->GetAttributeIntegerBounded(PARTICLE_MISS_CHANCE_ATTRIB, 0, 100, 0);

	return NOERROR;
	}

void CParticleSystemDesc::MarkImages (void)

//	MarkImages
//
//	Make sure we've loaded our images.
	
	{
	if (m_pParticleEffect) 
		m_pParticleEffect->MarkImages(); 
	}

CParticleSystemDesc::EStyles CParticleSystemDesc::ParseStyle (const CString &sValue)

//	ParseStyle
//
//	Returns the style

	{
	DWORD dwID;
	if (!CEffectParamDesc::FindIdentifier(sValue, STYLE_TABLE, &dwID))
		return styleUnknown;

	return (EStyles)dwID;
	}

void CParticleSystemDesc::SetStyle (const CEffectParamDesc &Value)

//	SetStyle
//
//	Sets style based on a parameter descriptor

	{
	SetStyle((EStyles)Value.EvalIdentifier(STYLE_TABLE, styleMax, styleJet));
	}

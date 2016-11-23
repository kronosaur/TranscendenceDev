//	SFXParticleSystem.cpp
//
//	CParticleSystemEffectCreator class
//	Copyright (c) 2014 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define PARTICLE_EFFECT_TAG						CONSTLIT("ParticleEffect")

#define CUR_DIRECTION_ATTRIB					CONSTLIT("curDirection")
#define EMIT_RATE_ATTRIB						CONSTLIT("emitRate")
#define EMIT_SPEED_ATTRIB						CONSTLIT("emitSpeed")
#define EMIT_STOP_ATTRIB						CONSTLIT("emitLifetime")
#define FIXED_POS_ATTRIB						CONSTLIT("fixedPos")
#define IS_TRACKING_OBJECT_ATTRIB				CONSTLIT("isTrackingObject")
#define PARTICLE_LIFETIME_ATTRIB				CONSTLIT("particleLifetime")
#define SPREAD_ANGLE_ATTRIB						CONSTLIT("spreadAngle")
#define LAST_DIRECTION_ATTRIB					CONSTLIT("lastDirection")
#define LAST_EMIT_POS_ATTRIB					CONSTLIT("lastEmitPos")
#define LIFETIME_ATTRIB							CONSTLIT("lifetime")
#define STYLE_ATTRIB							CONSTLIT("style")
#define TANGENT_SPEED_ATTRIB					CONSTLIT("tangentSpeed")
#define USE_OBJECT_CENTER_ATTRIB				CONSTLIT("useObjectCenter")
#define WAKE_POTENTIAL_ATTRIB					CONSTLIT("wakePotential")
#define XFORM_ROTATION_ATTRIB					CONSTLIT("xformRotation")
#define XFORM_TIME_ATTRIB						CONSTLIT("xformTime")

class CParticleSystemEffectPainter : public IEffectPainter
	{
	public:
		CParticleSystemEffectPainter (CCreatePainterCtx &Ctx, CParticleSystemEffectCreator *pCreator);
		~CParticleSystemEffectPainter (void);

		//	IEffectPainter virtuals
		virtual CEffectCreator *GetCreator (void) override { return m_pCreator; }
		virtual int GetFadeLifetime (void)  override{ return m_Desc.GetParticleLifetime().GetMaxValue(); }
		virtual int GetLifetime (void) override;
		virtual void GetParam (const CString &sParam, CEffectParamDesc *retValue) override;
		virtual bool GetParamList (TArray<CString> *retList) const override;
		virtual int GetParticleCount (void)  override{ return m_Particles.GetCount(); }
		virtual void GetRect (RECT *retRect) const override;
		virtual void OnMove (SEffectMoveCtx &Ctx, bool *retbBoundsChanged = NULL) override;
		virtual void OnUpdate (SEffectUpdateCtx &Ctx) override;
		virtual void Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) override;
		virtual void PaintFade (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)  override { bool bOldFade = Ctx.bFade; Ctx.bFade = true; Paint(Dest, x, y, Ctx); Ctx.bFade = bOldFade; }
		virtual void PaintHit (CG32bitImage &Dest, int x, int y, const CVector &vHitPos, SViewportPaintCtx &Ctx)  override{ Paint(Dest, x, y, Ctx); }
		virtual void SetPos (const CVector &vPos) override { m_Particles.SetOrigin(vPos); }

	protected:
		virtual void OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value) override;

	private:
		//	NOTE: These values are saved to disk; do not re-order.

		enum EOldStyles
			{
			oldstyleUnknown =			0,

			oldstyleJet =				1,
			oldstyleRadiate =			2,
			oldstyleWrithe =			3,

			oldstyleMax =				3,
			};

		CVector CalcInitialVel (CSpaceObject *pObj);
		void CreateFixedParticles (CSpaceObject *pObj, int iCount, const CVector &vInitialPos, const CVector &vInitialVel, int iTick);
		void CreateInterpolatedParticles (CSpaceObject *pObj, int iCount, const CVector &vInitialPos, const CVector &vInitialVel, int iTick);
		void CreateLinearParticles (CSpaceObject *pObj, int iCount, const CVector &vInitialPos, const CVector &vInitialVel, int iTick);
		void CreateNewParticles (CSpaceObject *pObj, int iCount, const CVector &vInitialPos, const CVector &vInitialVel, int iTick);
		void CreateRadiateParticles (CSpaceObject *pObj, int iCount, const CVector &vInitialPos, const CVector &vInitialVel, int iTick);
		void InitParticles (const CVector &vInitialPos);

		CParticleSystemEffectCreator *m_pCreator;

		CParticleSystemDesc m_Desc;

		DiceRange m_TangentSpeed;
		
		int m_iEmitLifetime;
		int m_iLifetime;
		bool m_bUseObjectCenter;
		bool m_bUseObjectMotion;
		bool m_bTrackingObject;

		int m_iCurDirection;
		CVector m_vLastEmitPos;
		int m_iLastDirection;

		CParticleArray m_Particles;
		IEffectPainter *m_pParticlePainter;
	};

static LPSTR OLD_STYLE_TABLE[] =
	{
	//	Must be same order as EStyles
		"",

		"jet",
		"radiate",
		"writhe",

		NULL,
	};

CParticleSystemEffectCreator::CParticleSystemEffectCreator (void) :
			m_pParticleEffect(NULL),
			m_pSingleton(NULL)

//	CRayEffectCreator constructor

	{
	}

CParticleSystemEffectCreator::~CParticleSystemEffectCreator (void)

//	CRayEffectCreator destructor

	{
	if (m_pSingleton)
		delete m_pSingleton;
	}

IEffectPainter *CParticleSystemEffectCreator::OnCreatePainter (CCreatePainterCtx &Ctx)

//	CreatePainter
//
//	Creates a new painter
	
	{
	//	If we have a singleton, return that

	if (m_pSingleton)
		return m_pSingleton;

	//	Otherwise we need to create a painter with the actual
	//	parameters.

	IEffectPainter *pPainter = new CParticleSystemEffectPainter(Ctx, this);

	//	Initialize the painter parameters

	pPainter->SetParam(Ctx, STYLE_ATTRIB, m_Style);
	pPainter->SetParam(Ctx, EMIT_RATE_ATTRIB, m_EmitRate);

	pPainter->SetParam(Ctx, FIXED_POS_ATTRIB, m_FixedPos);
	pPainter->SetParam(Ctx, PARTICLE_LIFETIME_ATTRIB, m_ParticleLifetime);
	pPainter->SetParam(Ctx, LIFETIME_ATTRIB, m_Lifetime);
	pPainter->SetParam(Ctx, WAKE_POTENTIAL_ATTRIB, m_WakePotential);
	pPainter->SetParam(Ctx, XFORM_ROTATION_ATTRIB, m_XformRotation);
	pPainter->SetParam(Ctx, XFORM_TIME_ATTRIB, m_XformTime);
	pPainter->SetParam(Ctx, EMIT_STOP_ATTRIB, m_EmitLifetime);

	if (!m_FixedPos.EvalBool() || m_EmitSpeed.GetType() != CEffectParamDesc::typeNull)
		pPainter->SetParam(Ctx, EMIT_SPEED_ATTRIB, m_EmitSpeed);

	if (m_SpreadAngle.GetType() != CEffectParamDesc::typeNull)
		pPainter->SetParam(Ctx, SPREAD_ANGLE_ATTRIB, m_SpreadAngle);
	else
		pPainter->SetParam(Ctx, TANGENT_SPEED_ATTRIB, m_TangentSpeed);

	//	Initialize via GetParameters, if necessary

	InitPainterParameters(Ctx, pPainter);

	//	If we are a singleton, then we only need to create this once.

	if (GetInstance() == instGame)
		{
		pPainter->SetSingleton(true);
		m_pSingleton = pPainter;
		}

	return pPainter;
	}

ALERROR CParticleSystemEffectCreator::OnEffectCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, const CString &sUNID)

//	OnEffectCreateFromXML
//
//	Initialize type-specific data

	{
	ALERROR error;

	m_FixedPos.InitBool(pDesc->GetAttributeBool(FIXED_POS_ATTRIB));

	CString sStyle = pDesc->GetAttribute(STYLE_ATTRIB);
	if (sStyle.IsBlank())
		m_Style.InitInteger(CParticleSystemDesc::styleJet);
	else
		{
		CParticleSystemDesc::EStyles iStyle = CParticleSystemDesc::ParseStyle(sStyle);
		if (iStyle == CParticleSystemDesc::styleUnknown)
			{
			Ctx.sError = strPatternSubst(CONSTLIT("Invalid effect param identifier: %s"), pDesc->GetAttribute(STYLE_ATTRIB));
			return ERR_FAIL;
			}

		m_Style.InitInteger(iStyle);
		}

	if (error = m_EmitRate.InitIntegerFromXML(Ctx, pDesc->GetAttribute(EMIT_RATE_ATTRIB)))
		return error;

	if (error = m_EmitSpeed.InitIntegerFromXML(Ctx, pDesc->GetAttribute(EMIT_SPEED_ATTRIB)))
		return error;

	if (error = m_EmitLifetime.InitIntegerFromXML(Ctx, pDesc->GetAttribute(EMIT_STOP_ATTRIB)))
		return error;

	if (error = m_ParticleLifetime.InitIntegerFromXML(Ctx, pDesc->GetAttribute(PARTICLE_LIFETIME_ATTRIB)))
		return error;

	if (error = m_SpreadAngle.InitIntegerFromXML(Ctx, pDesc->GetAttribute(SPREAD_ANGLE_ATTRIB)))
		return error;

	if (error = m_Lifetime.InitIntegerFromXML(Ctx, pDesc->GetAttribute(LIFETIME_ATTRIB)))
		return error;

	if (error = m_TangentSpeed.InitIntegerFromXML(Ctx, pDesc->GetAttribute(TANGENT_SPEED_ATTRIB)))
		return error;

	if (error = m_WakePotential.InitIntegerFromXML(Ctx, pDesc->GetAttribute(WAKE_POTENTIAL_ATTRIB)))
		return error;

	if (error = m_XformRotation.InitIntegerFromXML(Ctx, pDesc->GetAttribute(XFORM_ROTATION_ATTRIB)))
		return error;

	if (error = m_XformTime.InitIntegerFromXML(Ctx, pDesc->GetAttribute(XFORM_TIME_ATTRIB)))
		return error;

	//	Load the effect to use for particles

	CXMLElement *pEffect = pDesc->GetContentElementByTag(PARTICLE_EFFECT_TAG);
	if (pEffect)
		{
		if (error = CEffectCreator::CreateFromXML(Ctx, pEffect, NULL_STR, &m_pParticleEffect))
			return error;
		}
	else
		m_pParticleEffect = NULL;

	return NOERROR;
	}

ALERROR CParticleSystemEffectCreator::OnEffectBindDesign (SDesignLoadCtx &Ctx)

//	OnEffectBindDesign
//
//	Resolve loading

	{
	ALERROR error;

	//	Clean up, because we might want to recompute for next time.

	if (m_pSingleton)
		{
		delete m_pSingleton;
		m_pSingleton = NULL;
		}

	if (m_pParticleEffect)
		{
		if (error = m_pParticleEffect->BindDesign(Ctx))
			return error;
		}

	return NOERROR;
	}

//	CParticleSystemEffectPainter --------------------------------------------------

CParticleSystemEffectPainter::CParticleSystemEffectPainter (CCreatePainterCtx &Ctx, CParticleSystemEffectCreator *pCreator) : 
		m_pCreator(pCreator),
		m_iLifetime(0),
		m_iCurDirection(-1),
		m_iLastDirection(-1),
		m_bUseObjectCenter(Ctx.UseObjectCenter()),
		m_bUseObjectMotion(false),
		m_bTrackingObject(Ctx.IsTracking())

//	CParticleSystemEffectPainter constructor

	{
	//	Initialize the single particle painter

	CEffectCreator *pEffect = m_pCreator->GetParticleEffect();
	if (pEffect)
		m_pParticlePainter = pEffect->CreatePainter(Ctx);
	else
		m_pParticlePainter = NULL;
	}

CParticleSystemEffectPainter::~CParticleSystemEffectPainter (void)

//	CParticleSystemEffectPainter destructor

	{
	if (m_pParticlePainter)
		m_pParticlePainter->Delete();
	}

CVector CParticleSystemEffectPainter::CalcInitialVel (CSpaceObject *pObj)

//	CalcInitialVel
//
//	Calculates the initial particle velocity

	{
#if 0
	return (pObj ? m_pCreator->GetDrag() * pObj->GetVel() : CVector());
#else
	return CVector();
#endif
	}

void CParticleSystemEffectPainter::CreateFixedParticles (CSpaceObject *pObj, int iCount, const CVector &vInitialPos, const CVector &vInitialVel, int iTick)

//	CreateFixedParticles
//
//	Creates particles along the objects path (e.g., missile exhaust).

	{
	int i;
	
	ASSERT(m_iCurDirection != -1);

	//	Calculate a vector to our previous position
	//
	//	NOTE: In this mode m_vLastEmitPos is the last position of the object.

	CVector vCurPos = (pObj ? pObj->GetPos() : CVector());
	CVector vToOldPos;
	if (m_bTrackingObject)
		{
		Metric rAveSpeed = m_Desc.GetXformTime() * m_Desc.GetEmitSpeed().GetAveValue() * LIGHT_SPEED / 100.0;
		vToOldPos = m_vLastEmitPos - (vCurPos + vInitialPos) + ::PolarToVector(180 + m_iLastDirection, rAveSpeed * g_SecondsPerUpdate);
		}
	else
		{
		Metric rSpeed = (pObj ? pObj->GetVel().Length() : 0.0);
		vToOldPos = ::PolarToVector(180 + m_iLastDirection, rSpeed * g_SecondsPerUpdate);
		}

	//	Compute two orthogonal coordinates

	CVector vAxis = ::PolarToVector(m_iCurDirection + 180, 1.0);
	CVector vTangent = ::PolarToVector(m_iCurDirection + 90, 1.0);

	//	Create particles

	for (i = 0; i < iCount; i++)
		{
		Metric rSlide = mathRandom(0, 9999) / 10000.0;

		//	Compute a position randomly along the line between the current and
		//	last emit positions.

		CVector vPos = vInitialPos + rSlide * vToOldPos;

		//	Generate a random velocity along the tangent

		Metric rTangentSlide = mathRandom(-9999, 9999) / 10000.0;
		Metric rAxisJitter = mathRandom(-50, 50) / 100.0;
		CVector vVel = (vTangent * rTangentSlide * m_Desc.GetXformTime() * m_TangentSpeed.Roll() * LIGHT_SPEED / 100.0)
				+ (vAxis * (m_Desc.GetEmitSpeed().Roll() + rAxisJitter) * LIGHT_SPEED / 100.0);

		//	Lifetime

		int iLifeLeft = m_Desc.GetParticleLifetime().Roll();

		//	Add the particle

		m_Particles.AddParticle(vPos, vVel, iLifeLeft, m_iCurDirection, -1, iTick);
		}

	//	Remember the last position

	m_iLastDirection = m_iCurDirection;
	m_vLastEmitPos = vCurPos + vInitialPos;
	}

void CParticleSystemEffectPainter::CreateInterpolatedParticles (CSpaceObject *pObj, int iCount, const CVector &vInitialPos, const CVector &vInitialVel, int iTick)

//	CreateInterpolatedParticles
//
//	Creates particles interpolated between two directions.

	{
	int i;

	ASSERT(m_iCurDirection != -1);

	//	Compute some basic stuff

	const Metric rJitterFactor = LIGHT_SPEED / 100000.0;
	Metric rLastRotation = mathDegreesToRadians(180 + m_Desc.GetXformRotation() + m_iLastDirection);
	Metric rCurRotation = mathDegreesToRadians(180 + m_Desc.GetXformRotation() + m_iCurDirection);

	//	Compute the spread angle, in radians

	Metric rSpread = mathDegreesToRadians(Max(0, m_Desc.GetSpreadAngle().Roll()));
	Metric rHalfSpread = 0.5 * rSpread;

	//	Calculate where last tick's particles would be based on the last rotation.

	Metric rAveSpeed = m_Desc.GetEmitSpeed().GetAveValue() * LIGHT_SPEED / 100.0;
	CVector vLastStart = (m_vLastEmitPos + (m_Desc.GetXformTime() * ::PolarToVectorRadians(rLastRotation, rAveSpeed * g_SecondsPerUpdate))) - vInitialPos;

	//	Calculate where last tick's particles would be IF we have used the current
	//	rotation. This allows us to interpolate a turn.

	CVector vCurStart = (m_vLastEmitPos + (m_Desc.GetXformTime() * ::PolarToVectorRadians(rCurRotation, rAveSpeed * g_SecondsPerUpdate))) - vInitialPos;

	//	Create particles

	for (i = 0; i < iCount; i++)
		{
		Metric rSlide = mathRandom(0, 9999) / 10000.0;

		//	Compute two points along the two slide vectors (last and current)

		CVector vSlide1 = rSlide * vLastStart;
		CVector vSlide2 = rSlide * vCurStart;
		CVector vAdj = (rSlide * vSlide1) + ((1.0 - rSlide) * vSlide2);

		//	We place the particle along the line betwen the current
		//	and last emit positions

		CVector vPos = vInitialPos + vAdj;

		//	We blend the rotation as well

		if (Absolute(rCurRotation - rLastRotation) > PI)
			{
			if (rLastRotation < rCurRotation)
				rLastRotation += PI * 2.0;
			else
				rCurRotation += PI * 2.0;
			}

		Metric rSlideRotation = (rSlide * rLastRotation) + ((1.0 - rSlide) * rCurRotation);

		//	Generate a random velocity backwards

		Metric rRotation = rSlideRotation + (rHalfSpread * mathRandom(-1000, 1000) / 1000.0);
		Metric rSpeed = m_Desc.GetEmitSpeed().Roll() * LIGHT_SPEED / 100.0;
		CVector vVel = m_Desc.GetXformTime() * (vInitialVel + ::PolarToVectorRadians(rRotation, rSpeed + rJitterFactor * mathRandom(-500, 500)));

		//	Lifetime

		int iLifeLeft = m_Desc.GetParticleLifetime().Roll();

		//	Add the particle

		m_Particles.AddParticle(vPos, vVel, iLifeLeft, AngleToDegrees(rRotation), -1, iTick);
		}

	//	Remember the last position

	m_iLastDirection = m_iCurDirection;
	m_vLastEmitPos = vInitialPos;
	}

void CParticleSystemEffectPainter::CreateLinearParticles (CSpaceObject *pObj, int iCount, const CVector &vInitialPos, const CVector &vInitialVel, int iTick)

//	CreateLinearParticles
//
//	Creates new particles on a straight line

	{
	int i;

	ASSERT(m_iCurDirection != -1);

	//	Compute some basic stuff

	const Metric rJitterFactor = LIGHT_SPEED / 100000.0;
	Metric rCurRotation = mathDegreesToRadians(180 + m_Desc.GetXformRotation() + m_iCurDirection);

	//	Compute the spread angle, in radians

	Metric rSpread = mathDegreesToRadians(Max(0, m_Desc.GetSpreadAngle().Roll()));
	Metric rHalfSpread = 0.5 * rSpread;

	//	Calculate where last tick's particles would be based on the last rotation.

	Metric rAveSpeed = m_Desc.GetEmitSpeed().GetAveValue() * LIGHT_SPEED / 100.0;
	CVector vCurStart = (m_vLastEmitPos + (m_Desc.GetXformTime() * ::PolarToVectorRadians(rCurRotation, rAveSpeed * g_SecondsPerUpdate))) - vInitialPos;

	//	Create particles

	for (i = 0; i < iCount; i++)
		{
		Metric rSlide = mathRandom(0, 9999) / 10000.0;

		//	We place the particle along the line betwen the current
		//	and last emit positions

		CVector vPos = vInitialPos + rSlide * vCurStart;

		//	Generate a random velocity backwards

		Metric rRotation = rCurRotation + (rHalfSpread * mathRandom(-1000, 1000) / 1000.0);
		Metric rSpeed = m_Desc.GetEmitSpeed().Roll() * LIGHT_SPEED / 100.0;
		CVector vVel = m_Desc.GetXformTime() * (vInitialVel + ::PolarToVectorRadians(rRotation, rSpeed + rJitterFactor * mathRandom(-500, 500)));

		//	Lifetime

		int iLifeLeft = m_Desc.GetParticleLifetime().Roll();

		//	Add the particle

		m_Particles.AddParticle(vPos, vVel, iLifeLeft, AngleToDegrees(rRotation), -1, iTick);
		}

	//	Remember the last position

	m_iLastDirection = m_iCurDirection;
	m_vLastEmitPos = vInitialPos;
	}

void CParticleSystemEffectPainter::CreateNewParticles (CSpaceObject *pObj, int iCount, const CVector &vInitialPos, const CVector &vInitialVel, int iTick)

//	CreateNewParticles
//
//	Create the new particles for a tick

	{
	//	Make sure we're initialized

	InitParticles(vInitialPos);

	//	Create based on style

	switch (m_Desc.GetStyle())
		{
		case CParticleSystemDesc::styleJet:
			{
			//	If we haven't yet figured out which direction to emit, then we wait.
			//	(This doesn't happen until paint time).

			if (m_iCurDirection == -1)
				return;

			//	If we using object motion (e.g., for missile exhaust, then have a 
			//	completely different algorithm.

			if (m_bUseObjectMotion)
				CreateFixedParticles(pObj, iCount, vInitialPos, vInitialVel, iTick);

			//	If our emit direction has changed then we need to interpolate between the two

			else if (m_iCurDirection != m_iLastDirection)
				CreateInterpolatedParticles(pObj, iCount, vInitialPos, vInitialVel, iTick);

			//	Otherwise, just linear creation

			else
				CreateLinearParticles(pObj, iCount, vInitialPos, vInitialVel, iTick);

			break;
			}

		case CParticleSystemDesc::styleRadiate:
		case CParticleSystemDesc::styleWrithe:
			CreateRadiateParticles(pObj, iCount, vInitialPos, vInitialVel, iTick);
			break;
		}
	}

void CParticleSystemEffectPainter::CreateRadiateParticles (CSpaceObject *pObj, int iCount, const CVector &vInitialPos, const CVector &vInitialVel, int iTick)

//	CreateRadiateParticles
//
//	Creates new particles shooting out in all directions

	{
	int i;

	//	Compute some basic stuff

	const Metric rJitterFactor = LIGHT_SPEED / 100000.0;

	//	Calculate where last tick's particles would be based on the last rotation.

	Metric rAveSpeed = m_Desc.GetEmitSpeed().GetAveValue() * LIGHT_SPEED / 100.0;

	//	Create particles

	for (i = 0; i < iCount; i++)
		{
		//	Choose a random angle and velocity

		Metric rAngle = 2.0 * PI * (mathRandom(0, 9999) / 10000.0);
		Metric rSpeed = (m_Desc.GetEmitSpeed().Roll() * LIGHT_SPEED / 100.0) + rJitterFactor * mathRandom(-500, 500);
		CVector vVel = m_Desc.GetXformTime() * (vInitialVel + ::PolarToVectorRadians(rAngle, rSpeed));

		//	Lifetime

		int iLifeLeft = m_Desc.GetParticleLifetime().Roll();

		//	Add the particle

		m_Particles.AddParticle(vInitialPos, vVel, iLifeLeft, AngleToDegrees(rAngle), -1, iTick);
		}

	//	Remember the last position

	m_iLastDirection = m_iCurDirection;
	m_vLastEmitPos = vInitialPos;
	}

void CParticleSystemEffectPainter::GetParam (const CString &sParam, CEffectParamDesc *retValue)

//	GetParam
//
//	Returns the parameter

	{
	if (strEquals(sParam, CUR_DIRECTION_ATTRIB))
		retValue->InitInteger(m_iCurDirection);

	else if (strEquals(sParam, EMIT_RATE_ATTRIB))
		retValue->InitDiceRange(m_Desc.GetEmitRate());

	else if (strEquals(sParam, EMIT_SPEED_ATTRIB))
		retValue->InitDiceRange(m_Desc.GetEmitSpeed());

	else if (strEquals(sParam, EMIT_STOP_ATTRIB))
		retValue->InitInteger(m_iEmitLifetime);

	else if (strEquals(sParam, FIXED_POS_ATTRIB))
		retValue->InitBool(m_bUseObjectMotion);

	else if (strEquals(sParam, IS_TRACKING_OBJECT_ATTRIB))
		retValue->InitBool(m_bTrackingObject);

	else if (strEquals(sParam, PARTICLE_LIFETIME_ATTRIB))
		retValue->InitDiceRange(m_Desc.GetParticleLifetime());

	else if (strEquals(sParam, LAST_DIRECTION_ATTRIB))
		retValue->InitInteger(m_iLastDirection);

	else if (strEquals(sParam, LAST_EMIT_POS_ATTRIB))
		retValue->InitVector(m_vLastEmitPos);

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		retValue->InitInteger(m_iLifetime);

	else if (strEquals(sParam, SPREAD_ANGLE_ATTRIB))
		retValue->InitDiceRange(m_Desc.GetSpreadAngle());

	else if (strEquals(sParam, STYLE_ATTRIB))
		retValue->InitInteger(m_Desc.GetStyle());

	else if (strEquals(sParam, TANGENT_SPEED_ATTRIB))
		retValue->InitDiceRange(m_TangentSpeed);

	else if (strEquals(sParam, USE_OBJECT_CENTER_ATTRIB))
		retValue->InitBool(m_bUseObjectCenter);

	else if (strEquals(sParam, WAKE_POTENTIAL_ATTRIB))
		retValue->InitInteger(m_Desc.GetWakePotential());

	else if (strEquals(sParam, XFORM_ROTATION_ATTRIB))
		retValue->InitInteger(m_Desc.GetXformRotation());

	else if (strEquals(sParam, XFORM_TIME_ATTRIB))
		retValue->InitInteger((int)(m_Desc.GetXformTime() * 100.0));

	else
		retValue->InitNull();
	}

bool CParticleSystemEffectPainter::GetParamList (TArray<CString> *retList) const

//	GetParamList
//
//	Returns a list of value parameter names

	{
	retList->DeleteAll();
	retList->InsertEmpty(17);
	retList->GetAt(0) = CUR_DIRECTION_ATTRIB;
	retList->GetAt(1) = EMIT_RATE_ATTRIB;
	retList->GetAt(2) = EMIT_SPEED_ATTRIB;
	retList->GetAt(3) = EMIT_STOP_ATTRIB;
	retList->GetAt(4) = FIXED_POS_ATTRIB;
	retList->GetAt(5) = IS_TRACKING_OBJECT_ATTRIB;
	retList->GetAt(6) = PARTICLE_LIFETIME_ATTRIB;
	retList->GetAt(7) = LAST_DIRECTION_ATTRIB;
	retList->GetAt(8) = LAST_EMIT_POS_ATTRIB;
	retList->GetAt(9) = LIFETIME_ATTRIB;
	retList->GetAt(10) = SPREAD_ANGLE_ATTRIB;
	retList->GetAt(11) = STYLE_ATTRIB;
	retList->GetAt(12) = TANGENT_SPEED_ATTRIB;
	retList->GetAt(13) = USE_OBJECT_CENTER_ATTRIB;
	retList->GetAt(14) = WAKE_POTENTIAL_ATTRIB;
	retList->GetAt(15) = XFORM_ROTATION_ATTRIB;
	retList->GetAt(16) = XFORM_TIME_ATTRIB;

	return true;
	}

int CParticleSystemEffectPainter::GetLifetime (void)

//	GetLifetime
//
//	Returns the lifetime
	
	{
	if (m_iLifetime > 0)
		return m_iLifetime;

	else if (m_iEmitLifetime > 0)
		return (m_iEmitLifetime + m_Desc.GetParticleLifetime().GetMaxValue());

	else
		return m_iLifetime; 
	}

void CParticleSystemEffectPainter::GetRect (RECT *retRect) const

//	GetRect
//
//	Returns the rect bounds of the image

	{
	*retRect = m_Particles.GetBounds();
	}

void CParticleSystemEffectPainter::InitParticles (const CVector &vInitialPos)

//	Init
//
//	Make sure particle array is initialized

	{
	if (m_Particles.GetCount() == 0)
		{
		//	Compute the maximum number of particles that we could ever have

		int iNewParticleRate = m_Desc.GetEmitRate().GetMaxValue();
		int iParticleLifetime = m_Desc.GetParticleLifetime().GetMaxValue();

		int iMaxParticleCount = Max(0, iParticleLifetime * iNewParticleRate);

		//	Initialize the array

		m_Particles.Init(iMaxParticleCount, vInitialPos);
		}
	}

void CParticleSystemEffectPainter::OnMove (SEffectMoveCtx &Ctx, bool *retbBoundsChanged)

//	OnMove
//
//	Move the particles

	{
	//	Update the single-particle painter

	if (m_pParticlePainter)
		m_pParticlePainter->OnMove(Ctx);

	//	Update particle motion

	m_Particles.UpdateMotionLinear();

	//	If we're using the object's motion, adjust now

	if (m_bUseObjectMotion && Ctx.pObj)
		{
		CVector vToOldPos;
		if (m_bTrackingObject)
			{
			CVector vCurPos = Ctx.pObj->GetPos();
			vToOldPos = Ctx.vOldPos - vCurPos;
			}
		else
			{
			Metric rSpeed = Ctx.pObj->GetVel().Length();
			vToOldPos = ::PolarToVector(180 + m_iLastDirection, rSpeed * g_SecondsPerUpdate);
			}

		//	Move all particles by the given amount

		m_Particles.Move(vToOldPos);
		}

	//	If we're using the object as the center, then set the origin

	if (m_bUseObjectCenter && Ctx.pObj)
		m_Particles.SetOrigin(Ctx.pObj->GetPos());

	//	Bounds are always changing

	if (retbBoundsChanged)
		*retbBoundsChanged = true;
	}

void CParticleSystemEffectPainter::OnUpdate (SEffectUpdateCtx &Ctx)

//	OnUpdate
//
//	Update the painter

	{
	//	Update the single-particle painter

	if (m_pParticlePainter)
		m_pParticlePainter->OnUpdate(Ctx);

	//	LATER: Support damage

#if 0
	//	If we have a wake potential or if the particles do damage
	//	then we need to hit test against all objects in the system.

	Ctx.pDamageDesc = m_pCreator->GetDamageDesc();
#endif

	//	Update particle behavior

	m_Particles.Update(m_Desc, Ctx);

	//	Create new particles

	if (!Ctx.bFade)
		{
		if (m_iEmitLifetime <= 0 || Ctx.iTick < m_iEmitLifetime)
			CreateNewParticles(Ctx.pObj, m_Desc.GetEmitRate().Roll(), Ctx.vEmitPos, CalcInitialVel(Ctx.pObj), Ctx.iTick);
		}
	else
		{
		if (m_bUseObjectMotion && Ctx.pObj)
			m_vLastEmitPos = Ctx.pObj->GetPos();

		//	If we're fading, reset direction (otherwise, when painting thruster 
		//	effects we'll try to interpolate between stale directions).

		m_iCurDirection = -1;
		}
	}

void CParticleSystemEffectPainter::Paint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint the effect

	{
	int iParticleLifetime = m_Desc.GetParticleLifetime().GetMaxValue();

	//	Particles move the opposite direction from the shot

	int iTrailDirection = Ctx.iRotation;

	//	If we're using the object center then we paint at the object center.
	//	Otherwise we paint at the given position.

	int xPaint;
	int yPaint;
	if (m_bUseObjectCenter)
		{
		if (Ctx.pObj)
			Ctx.XForm.Transform(Ctx.pObj->GetPos(), &xPaint, &yPaint);
		else
			{
			//	If we don't have an object then we use the viewport center. This
			//	handles the case where we paint in TransData (where there is
			//	no object).

			xPaint = Ctx.xCenter;
			yPaint = Ctx.yCenter;
			}
		}
	else
		{
		xPaint = x;
		yPaint = y;
		}

	//	Paint with the painter

	m_Particles.Paint(m_Desc, Dest, xPaint, yPaint, m_pParticlePainter, Ctx);

	//	Update last direction

	if (m_iCurDirection == -1)
		{
		m_iCurDirection = iTrailDirection;
		m_iLastDirection = iTrailDirection;

		//	Figure out the position where we create particles

		CVector vPos;

		//	If we're using the object center then it means that x,y is where
		//	we emit particles from. We need to convert from screen coordinates
		//	to object-relative coordinates.

		if (m_bUseObjectCenter)
			vPos = CVector((x - xPaint) * g_KlicksPerPixel, (yPaint - y) * g_KlicksPerPixel);

		//	Initialize last emit position

		m_vLastEmitPos = (m_bUseObjectMotion && Ctx.pObj ? Ctx.pObj->GetPos() + vPos : vPos);
		}
	else
		m_iCurDirection = iTrailDirection;
	}

void CParticleSystemEffectPainter::OnSetParam (CCreatePainterCtx &Ctx, const CString &sParam, const CEffectParamDesc &Value)

//	SetParam
//
//	Sets parameters

	{
	if (strEquals(sParam, CUR_DIRECTION_ATTRIB))
		m_iCurDirection = Value.EvalIntegerBounded(0, -1, -1);

	else if (strEquals(sParam, EMIT_RATE_ATTRIB))
		m_Desc.SetEmitRate(Value.EvalDiceRange(10));

	else if (strEquals(sParam, EMIT_SPEED_ATTRIB))
		m_Desc.SetEmitSpeed(Value.EvalDiceRange(50));

	else if (strEquals(sParam, EMIT_STOP_ATTRIB))
		m_iEmitLifetime = Value.EvalIntegerBounded(0, -1, 0);

	else if (strEquals(sParam, FIXED_POS_ATTRIB))
		m_bUseObjectMotion = Value.EvalBool();

	else if (strEquals(sParam, IS_TRACKING_OBJECT_ATTRIB))
		m_bTrackingObject = Value.EvalBool();

	else if (strEquals(sParam, PARTICLE_LIFETIME_ATTRIB))
		m_Desc.SetParticleLifetime(Value.EvalDiceRange(10));

	else if (strEquals(sParam, LAST_DIRECTION_ATTRIB))
		m_iLastDirection = Value.EvalIntegerBounded(0, -1, -1);

	else if (strEquals(sParam, LAST_EMIT_POS_ATTRIB))
		m_vLastEmitPos = Value.EvalVector();

	else if (strEquals(sParam, LIFETIME_ATTRIB))
		m_iLifetime = Value.EvalIntegerBounded(0, -1, 0);

	else if (strEquals(sParam, SPREAD_ANGLE_ATTRIB))
		m_Desc.SetSpreadAngle(Value.EvalDiceRange(5));

	else if (strEquals(sParam, STYLE_ATTRIB))
		{
		//	Backwards compatibility for older version

		if (Ctx.GetLoadVersion() < 138)
			{
			int iOldStyle = Value.EvalIdentifier(OLD_STYLE_TABLE, oldstyleMax, oldstyleJet);
			switch (iOldStyle)
				{
				case oldstyleJet:
					m_Desc.SetStyle(CParticleSystemDesc::styleJet);
					break;

				case oldstyleRadiate:
					m_Desc.SetStyle(CParticleSystemDesc::styleRadiate);
					break;

				case oldstyleWrithe:
					m_Desc.SetStyle(CParticleSystemDesc::styleWrithe);
					break;

				default:
					m_Desc.SetStyle(CParticleSystemDesc::styleJet);
					break;
				}
			}
		else
			m_Desc.SetStyle(Value);
		}

	else if (strEquals(sParam, TANGENT_SPEED_ATTRIB))
		m_TangentSpeed = Value.EvalDiceRange(0);

	else if (strEquals(sParam, USE_OBJECT_CENTER_ATTRIB))
		m_bUseObjectCenter = Value.EvalBool();

	else if (strEquals(sParam, WAKE_POTENTIAL_ATTRIB))
		m_Desc.SetWakePotential(Value.EvalIntegerBounded(0, -1, 0));

	else if (strEquals(sParam, XFORM_ROTATION_ATTRIB))
		m_Desc.SetXformRotation(Value.EvalIntegerBounded(-359, 359, 0));

	else if (strEquals(sParam, XFORM_TIME_ATTRIB))
		m_Desc.SetXformTime(Value.EvalIntegerBounded(0, -1, 100) / 100.0);
	}

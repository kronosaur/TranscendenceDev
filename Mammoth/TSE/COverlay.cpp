//	COverlay.cpp
//
//	COverlay class

#include "PreComp.h"

#define EVENT_GET_DOCK_SCREEN					CONSTLIT("GetDockScreen")
#define EVENT_ON_OBJ_DOCKED						CONSTLIT("OnObjDocked")

#define ON_CREATE_EVENT							CONSTLIT("OnCreate")
#define ON_DAMAGE_EVENT							CONSTLIT("OnDamage")
#define ON_DESTROY_EVENT						CONSTLIT("OnDestroy")
#define ON_MINING_EVENT							CONSTLIT("OnMining")
#define ON_OBJ_DESTROYED_EVENT					CONSTLIT("OnObjDestroyed")
#define ON_UPDATE_EVENT							CONSTLIT("OnUpdate")

#define OVERLAY_RADIUS							(10.0 * g_KlicksPerPixel)
#define OVERLAY_RADIUS2							(OVERLAY_RADIUS * OVERLAY_RADIUS)

#define LIFETIME_ATTRIB							CONSTLIT("lifetime")

#define PROPERTY_ACTIVE							CONSTLIT("active")
#define PROPERTY_COUNTER						CONSTLIT("counter")
#define PROPERTY_COUNTER_LABEL					CONSTLIT("counterLabel")
#define PROPERTY_HP								CONSTLIT("hp")
#define PROPERTY_MAX_HP							CONSTLIT("maxHP")
#define PROPERTY_POS							CONSTLIT("pos")
#define PROPERTY_ROTATION						CONSTLIT("rotation")
#define PROPERTY_TYPE							CONSTLIT("type")

const int ANNOTATION_INNER_SPACING_Y =			2;
const int FLAG_INNER_SPACING_X =				4;

COverlay::COverlay (void) : 
		m_fDestroyed(false),
		m_fFading(false),
		m_fInactive(false)

//	COverlay constructor

	{
	}

COverlay::~COverlay (void)

//	COverlay destructor

	{
	if (m_pPainter)
		m_pPainter->Delete();

	if (m_pHitPainter)
		m_pHitPainter->Delete();
	}

bool COverlay::AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx)

//	AbsorbDamage
//
//	Absorbs damage and returns TRUE if any damage is absorbed

	{
	//	If we're inactive, then nothing

	if (!IsActive())
		{
		return false;
		}

	//	If we absorb damage, then see how much we absorbed

	else if (int iAbsorb = m_pType->GetDamageAbsorbed(pSource, Ctx))
		{
		//	Handle custom damage (this is to give custom damage
		//	a chance to react to hitting an overlay)

		if (Ctx.pDesc->FireOnDamageOverlay(Ctx, this))
			return true;

		//	Absorb damage

		Ctx.iDamage -= iAbsorb;

		//	Hit effect

		CreateHitEffect(pSource, Ctx);

		//	Done

		return true;
		}

	//	Otherwise, see if the damage affects us

	else
		{
		bool bHit = false;

		//	If we have a position, then we only get hit if the shot was near us

		if (m_iPosRadius)
			{
			CVector vPos = GetPos(pSource);
			if ((Ctx.vHitPos - vPos).Length2() < OVERLAY_RADIUS2)
				bHit = true;
			}

		//	Otherwise, we always get hit

		else
			bHit = true;

		//	If we're hit, then fire an event

		if (bHit)
			{
			//	Handle custom damage (this is to give custom damage
			//	a chance to react to hitting an overlay)

			if (Ctx.pDesc->FireOnDamageOverlay(Ctx, this))
				return true;

			//	Handle damage (if we don't do anything on hit, then
			//	we let the damage through)

			if (FireOnDamage(pSource, Ctx))
				{
				CreateHitEffect(pSource, Ctx);
				return true;
				}

			//	If we're a device overlay then there is a chance that we
			//	damage the device.

			if (m_iDevice != -1
					&& mathRandom(1, 100) <= 40)
				pSource->DamageExternalDevice(m_iDevice, Ctx);
			}
		
		return bHit;
		}
	}

void COverlay::AccumulateBounds (CSpaceObject *pSource, int iScale, int iRotation, RECT *ioBounds)

//	AccumulateBounds
//
//	Set bounds

	{
	if (!IsActive())
		return;

	switch (m_pType->GetCounterDesc().GetType())
		{
		case COverlayCounterDesc::counterRadius:
			{
			if (m_iCounter > 0)
				{
				ioBounds->left = Min(-m_iCounter, (int)ioBounds->left);
				ioBounds->top = Min(-m_iCounter, (int)ioBounds->top);
				ioBounds->right = Max(m_iCounter, (int)ioBounds->right);
				ioBounds->bottom = Max(m_iCounter, (int)ioBounds->bottom);
				}
			break;
			}
		}

	//	Add painter bounds

	if (m_pPainter)
		{
		RECT rcRect;
		m_pPainter->GetBounds(&rcRect);

		//	Offset based on paint position (which requires iScale and iRotation)

		int xOffset, yOffset;
		CalcOffset(*pSource, iScale, iRotation, &xOffset, &yOffset);

		::OffsetRect(&rcRect, xOffset, yOffset);

		//	Adjust resulting RECT to include painter rect

		::UnionRect(ioBounds, ioBounds, &rcRect);
		}
	}

bool COverlay::AccumulateEnhancements (CSpaceObject &Source, CDeviceItem &DeviceItem, TArray<CString> &EnhancementIDs, CItemEnhancementStack &Enhancements)

//	AccumulateEnhancements
//
//	Accumulates enhancements for the given device to the stack. Returns TRUE if
//	any enhancements were added.

	{
	if (!IsActive())
		return false;
	else
		{
		bool bEnhanced = false;

		//	Accumulate our enhancement list. We don't pass the EnhancementIDs array
		//	because we allow multiple overlays to take effect. Note that some 
		//	overlays have multiple enhancements with the same type and if we 
		//	pass in EnhancementIDs then only one enhancement will work. If we
		//	want to prevent duplicate overlay enhancements then we should do the
		//	check here, not inside CEnhancementDesc::Accumulate.

		if (m_pType->GetEnhancementsConferred().Accumulate(1, DeviceItem, Enhancements))
			bEnhanced = true;

		//	Convert weapon bonus to an enhancement.

		switch (DeviceItem.GetCategory())
			{
			case itemcatLauncher:
			case itemcatWeapon:
				{
				if (int iWeaponBonus = m_pType->GetWeaponBonus(DeviceItem.GetInstalledDevice(), &Source))
					{
					Enhancements.InsertHPBonus(NULL, iWeaponBonus);
					bEnhanced = true;
					}

				break;
				}
			}

		return bEnhanced;
		}
	}

void COverlay::CalcOffset (const CSpaceObject &Source, int iScale, int iRotation, int *retxOffset, int *retyOffset, int *retiRotationOrigin) const

//	CalcOffset
//
//	Compute the offset where we should paint the overlay relative to the object
//	center (in pixels).

	{
	//	Adjust position, if necessary

	if (m_iPosRadius || m_iPosZ)
		{
		//	Do we rotate with the source?

		int iRotationOrigin = (m_pType->RotatesWithSource(Source) ? iRotation : 0);
		if (retiRotationOrigin)
			*retiRotationOrigin = iRotationOrigin;

		//	Adjust based on position

		C3DConversion::CalcCoord(iScale, iRotationOrigin + m_iPosAngle, m_iPosRadius, m_iPosZ, retxOffset, retyOffset);
		}

	//	Otherwise, we're at the center

	else
		{
		*retxOffset = 0;
		*retyOffset = 0;

		if (retiRotationOrigin)
			*retiRotationOrigin = (m_pType->RotatesWithSource(Source) ? iRotation : 0);
		}
	}

void COverlay::CreateHitEffect (CSpaceObject *pSource, SDamageCtx &Ctx)

//	CreateHitEffect
//
//	Creates the appropriate hit effect

	{
	CEffectCreator *pHitEffect = m_pType->GetHitEffectCreator();
	if (pHitEffect == NULL)
		return;

	//	If the hit effect just replaces the normal effect, create a painter
	//	and set the state

	if (m_pType->IsHitEffectAlt())
		{
		CCreatePainterCtx CreateCtx;
		CreateCtx.SetAPIVersion(m_pType->GetAPIVersion());
		CreateCtx.SetAnchor(pSource);
		CreateCtx.SetOverlay(*this);

		//	Create a painter

		if (m_pHitPainter)
			m_pHitPainter->Delete();
		m_pHitPainter = pHitEffect->CreatePainter(CreateCtx);

		//	Initialize

		m_iPaintHit = m_pHitPainter->GetLifetime();
		m_iPaintHitTick = 0;

		m_pHitPainter->PlaySound(pSource);
		}

	//	Otherwise, create an effect object

	else
		{
		pHitEffect->CreateEffect(pSource->GetSystem(),
				NULL,
				Ctx.vHitPos,
				pSource->GetVel(),
				Ctx.iDirection);
		}
	}

void COverlay::CreateFromType (COverlayType &Type, 
							   CSpaceObject &Source,
							   int iPosAngle,
							   int iPosRadius,
							   int iRotation,
							   int iPosZ,
							   int iLifeLeft, 
							   COverlay **retpField)

//	CreateFromType
//
//	Create field from type

	{
	COverlay *pField = new COverlay;

	pField->m_pType = &Type;
	pField->m_dwID = Type.GetUniverse().CreateGlobalID();
	pField->m_iDevice = -1;
	pField->m_iLifeLeft = iLifeLeft;
	pField->m_iTick = 0;
	pField->m_iPosAngle = iPosAngle;
	pField->m_iPosRadius = iPosRadius;
	pField->m_iRotation = iRotation;
	pField->m_iPosZ = iPosZ;
	pField->m_iPaintHit = 0;
	pField->m_iHitPoints = Type.GetMaxHitPoints(Source);

	//	If this is an underground vault, then we start out inactive (the player
	//	must excavate it).

	if (Type.IsUnderground() && pField->m_iHitPoints > 0)
		pField->SetActive(Source, false);

	//	Create painters

	CEffectCreator *pCreator = Type.GetEffectCreator();
	if (pCreator)
		{
		CCreatePainterCtx CreateCtx;
		CreateCtx.SetAPIVersion(Type.GetAPIVersion());
		CreateCtx.SetAnchor(&Source);
		CreateCtx.SetOverlay(*pField);
		CreateCtx.SetLifetime(pField->m_iLifeLeft);

		pField->m_pPainter = pCreator->CreatePainter(CreateCtx);
		}

	pField->m_pHitPainter = NULL;

	//	Initialize data

	Type.InitObjectData(Source, pField->m_Data);

	//	Done

	*retpField = pField;
	}

void COverlay::Destroy (CSpaceObject *pSource)

//	Destroy
//
//	Destroy the field

	{
	if (!m_fDestroyed)
		{
		FireOnDestroy(pSource);

		//	Mark the field as destroyed.
		//	It will be deleted in COverlayList::Update

		m_fDestroyed = true;

		//	If the painter needs time to fade out, then handle it

		int iPainterFade;
		if (m_pPainter 
				&& (iPainterFade = m_pPainter->GetFadeLifetime(false)) > 0)
			{
			m_pPainter->OnBeginFade();
			m_iLifeLeft = iPainterFade;
			m_fFading = true;

			//	No need for hit painter

			m_iPaintHit = 0;
			if (m_pHitPainter)
				{
				m_pHitPainter->Delete();
				m_pHitPainter = NULL;
				}
			}
		}
	}

bool COverlay::FindCustomProperty (CCodeChainCtx &CCCtx, CSpaceObject &SourceObj, const CString &sProperty, ICCItemPtr &pValue) const

//	FindCustomProperty
//
//	Returns a custom property. Returns FALSE if given property is not defined.

	{
	if (m_pType == NULL)
		return false;

	EPropertyType iType;
	ICCItemPtr pResult = m_pType->GetProperty(CCCtx, sProperty, &iType);

	if (iType == EPropertyType::propNone)
		return false;

	//	If the property is an object property, then we need to look in 
	//	object data.

	else if (iType == EPropertyType::propVariant || iType == EPropertyType::propData)
		{
		pValue = GetData(sProperty);
		return true;
		}

	//	If this is a dynamic property, we need to evaluate.

	else if (iType == EPropertyType::propDynamicData)
		{
		CCodeChainCtx Ctx(GetUniverse());
		Ctx.SaveAndDefineSourceVar(&SourceObj);
		Ctx.SaveAndDefineOverlayID(m_dwID);

		pValue = Ctx.RunCode(pResult);
		return true;
		}

	//	Otherwise we have a valid property.

	else
		{
		pValue = pResult;
		return true;
		}
	}

void COverlay::FireCustomEvent (CSpaceObject *pSource, const CString &sEvent, ICCItem *pData, ICCItem **retpResult)

//	FireCustomEvent
//
//	Fires a custom event

	{
	SEventHandlerDesc Event;
	if (m_pType->FindEventHandler(sEvent, &Event))
		{
		CCodeChainCtx Ctx(pSource->GetUniverse());
		
		//	Setup 

		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineDataVar(pData);
		Ctx.SaveAndDefineOverlayID(m_dwID);

		//	Execute

		ICCItem *pResult = Ctx.Run(Event);

		//	Done

		if (retpResult)
			*retpResult = pResult;
		else
			Ctx.Discard(pResult);
		}
	else if (retpResult)
		*retpResult = GetUniverse().GetCC().CreateNil();
	}

bool COverlay::FireGetDockScreen (const CSpaceObject *pSource, CDockScreenSys::SSelector &Selector) const

//	FireGetDockScreen
//
//	Fires GetDockScreen event.

	{
	if (!IsActive())
		return false;

	SEventHandlerDesc Event;
	if (!m_pType->FindEventHandler(EVENT_GET_DOCK_SCREEN, &Event))
		return false;

	CCodeChainCtx Ctx(pSource->GetUniverse());
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(pSource);
	Ctx.SaveAndDefineOverlayID(m_dwID);

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		{
		pSource->ReportEventError(EVENT_GET_DOCK_SCREEN, pResult);
		return false;
		}

	return CTLispConvert::AsScreenSelector(pResult, &Selector);
	}

void COverlay::FireOnCreate (CSpaceObject *pSource)

//	FireOnCreate
//
//	Fire OnCreate event

	{
	SEventHandlerDesc Event;
	if (m_pType->FindEventHandler(ON_CREATE_EVENT, &Event))
		{
		CCodeChainCtx Ctx(pSource->GetUniverse());
		
		//	Setup 

		Ctx.SetEvent(eventOverlayEvent);
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineOverlayID(m_dwID);

		//	Execute

		ICCItem *pResult = Ctx.Run(Event);

		//	Done

		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Overlay OnCreate: %s"), pResult->GetStringValue()), pResult);

		Ctx.Discard(pResult);
		}
	}

bool COverlay::FireOnDamage (CSpaceObject *pSource, SDamageCtx &Ctx)

//	FireOnDamage
//
//	Fire OnDamage event

	{
	SEventHandlerDesc Event;
	if (m_pType->FindEventHandler(ON_DAMAGE_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(pSource->GetUniverse());
		
		//	Setup 

		CCCtx.SetEvent(eventOverlayEvent);
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(pSource);
		CCCtx.DefineDamageCtx(Ctx);
		CCCtx.SaveAndDefineOverlayID(m_dwID);

		//	Execute

		ICCItem *pResult = CCCtx.Run(Event);

		//	Check for error

		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Overlay OnDamage: %s"), pResult->GetStringValue()), pResult);

		//	If we don't return Nil, then the result is the damage that passes to the source

		bool bHandled = false;
		if (!pResult->IsNil())
			{
			Ctx.iDamage = pResult->GetIntegerValue();
			bHandled = true;
			}

		//	Done

		CCCtx.Discard(pResult);
		return bHandled;
		}
	else
		return false;
	}

void COverlay::FireOnDestroy (CSpaceObject *pSource)

//	FireOnDestroy
//
//	Fire OnDestroy event

	{
	SEventHandlerDesc Event;
	if (m_pType->FindEventHandler(ON_DESTROY_EVENT, &Event))
		{
		CCodeChainCtx Ctx(pSource->GetUniverse());

		//	Setup 

		Ctx.SetEvent(eventOverlayEvent);
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineOverlayID(m_dwID);

		//	Execute

		ICCItem *pResult = Ctx.Run(Event);

		//	Done

		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Overlay OnDestroy: %s"), pResult->GetStringValue()), pResult);

		Ctx.Discard(pResult);
		}
	}

void COverlay::FireOnMining (CSpaceObject &Source, EAsteroidType iType, SDamageCtx &Ctx)

//	FireOnMining
//
//	Fire <OnMining> event on overlay

	{
	SEventHandlerDesc Event;
	if (!m_pType->FindEventHandler(ON_MINING_EVENT, &Event))
		return;

	//	Compute some mining stats

	const int iMiningLevel = Ctx.Damage.GetMiningAdj();
	const int iMiningDifficulty = Source.CalcMiningDifficulty(iType);

	CAsteroidDesc::SMiningStats MiningStats;
	CAsteroidDesc::CalcMining(iMiningLevel, iMiningDifficulty, iType, Ctx, MiningStats);

	//	Run

	CCodeChainCtx CCX(GetUniverse());
	CCX.DefineContainingType(this);
	CCX.SaveAndDefineSourceVar(&Source);
	CCX.SaveAndDefineOverlayID(m_dwID);

	CCX.DefineSpaceObject(CONSTLIT("aCause"), Ctx.pCause);
	CCX.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
	CCX.DefineSpaceObject(CONSTLIT("aMiner"), Ctx.Attacker.GetObj());
	CCX.DefineVector(CONSTLIT("aMinePos"), Ctx.vHitPos);
	CCX.DefineInteger(CONSTLIT("aMineDir"), Ctx.iDirection);
	CCX.DefineInteger(CONSTLIT("aMineProbability"), iMiningLevel);
	CCX.DefineInteger(CONSTLIT("aMineDifficulty"), iMiningDifficulty);
	CCX.DefineString(CONSTLIT("aAsteroidType"), CAsteroidDesc::CompositionID(iType));
	CCX.DefineInteger(CONSTLIT("aSuccessChance"), MiningStats.iSuccessChance);
	CCX.DefineInteger(CONSTLIT("aMaxOreLevel"), MiningStats.iMaxOreLevel);
	CCX.DefineDouble(CONSTLIT("aYieldAdj"), MiningStats.rYieldAdj);
	CCX.DefineInteger(CONSTLIT("aHP"), Ctx.iDamage);
	CCX.DefineString(CONSTLIT("aDamageType"), GetDamageShortName(Ctx.Damage.GetDamageType()));
	CCX.DefineItemType(CONSTLIT("aWeaponType"), Ctx.pDesc->GetWeaponType());

	ICCItemPtr pResult = CCX.RunCode(Event);
	if (pResult->IsError())
		Source.ReportEventError(strPatternSubst(CONSTLIT("Overlay OnMining: %s"), pResult->GetStringValue()), pResult);
	}

void COverlay::FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const

//	FireOnObjDestroyed
//
//	OnObjDestroyed event

	{
	SEventHandlerDesc Event;
	if (m_pType->FindEventHandler(ON_OBJ_DESTROYED_EVENT, &Event))
		{
		CCodeChainCtx CCCtx(pSource->GetUniverse());

		//	Setup

		CCCtx.SetEvent(eventOverlayEvent);
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(pSource);
		CCCtx.SaveAndDefineOverlayID(m_dwID);
		CCCtx.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.Obj);
		CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
		CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
		CCCtx.DefineBool(CONSTLIT("aDestroy"), Ctx.WasDestroyed());
		CCCtx.DefineString(CONSTLIT("aDestroyReason"), GetDestructionName(Ctx.iCause));

		//	Execute

		ICCItem *pResult = CCCtx.Run(Event);

		//	Done

		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Overlay OnObjDestroyed: %s"), pResult->GetStringValue()), pResult);

		CCCtx.Discard(pResult);
		}
	}

void COverlay::FireOnObjDocked (CSpaceObject *pSource, CSpaceObject *pShip) const

//	FireOnObjDocked
//
//	Calls OnObjDocked on all overlays

	{
	SEventHandlerDesc Event;
	if (m_pType->FindEventHandler(EVENT_ON_OBJ_DOCKED, &Event))
		{
		CCodeChainCtx Ctx(pSource->GetUniverse());

		Ctx.SetEvent(eventOverlayEvent);
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineOverlayID(m_dwID);
		Ctx.DefineSpaceObject(CONSTLIT("aObjDocked"), pShip);
		Ctx.DefineSpaceObject(CONSTLIT("aDockTarget"), pSource);

		ICCItem *pResult = Ctx.Run(Event);
		if (pResult->IsError())
			pSource->ReportEventError(EVENT_ON_OBJ_DOCKED, pResult);
		Ctx.Discard(pResult);
		}
	}

void COverlay::FireOnUpdate (CSpaceObject *pSource)

//	FireOnUpdate
//
//	Fire OnUpdate event (once per 15 ticks)

	{
	SEventHandlerDesc Event;
	if (m_pType->FindEventHandler(CDesignType::evtOnUpdate, &Event))
		{
		CCodeChainCtx Ctx(pSource->GetUniverse());

		//	Setup 

		Ctx.SetEvent(eventOverlayEvent);
		Ctx.DefineContainingType(this);
		Ctx.SaveAndDefineSourceVar(pSource);
		Ctx.SaveAndDefineOverlayID(m_dwID);

		//	Execute

		ICCItem *pResult = Ctx.Run(Event);

		//	Done

		if (pResult->IsError())
			pSource->ReportEventError(strPatternSubst(CONSTLIT("Overlay OnUpdate: %s"), pResult->GetStringValue()), pResult);

		Ctx.Discard(pResult);
		}
	}

CConditionSet COverlay::GetConditions (CSpaceObject *pSource) const

//	GetConditions
//
//	Returns the set of imparted conditions

	{
	CConditionSet Conditions;

	if (!IsActive())
		return Conditions;

	//	Do we disarm the source?

	if (Disarms(pSource))
		Conditions.Set(CConditionSet::cndDisarmed);

	//	Do we paralyze the source?

	if (Paralyzes(pSource))
		Conditions.Set(CConditionSet::cndParalyzed);

	//	Can't bring up ship status

	if (IsShipScreenDisabled())
		Conditions.Set(CConditionSet::cndShipScreenDisabled);

	//	Do we spin the source ?

	if (Spins(pSource))
		Conditions.Set(CConditionSet::cndSpinning);

	//	Drag

	if (GetDrag(pSource) < 1.0)
		Conditions.Set(CConditionSet::cndDragged);

	//	Time-stopped?

	if (StopsTime(pSource))
		Conditions.Set(CConditionSet::cndTimeStopped);

	//	Done

	return Conditions;
	}

bool COverlay::GetImpact (CSpaceObject *pSource, SImpactDesc &Impact) const

//	GetImpact
//
//	Returns the impact of this overlay.

	{
	if (!IsActive())
		return false;

	//	Conditions

	Impact.Conditions = GetConditions(pSource);
	bool bHasImpact = !Impact.Conditions.IsEmpty();

	//	Get appy drag

	if ((Impact.rDrag = GetDrag(pSource)) < 1.0)
		bHasImpact = true;

	//	Done

	return bHasImpact;
	}

CVector COverlay::GetPos (CSpaceObject *pSource) const

//	GetPos
//
//	Returns the current absolute position of the overlay

	{
	if (m_iPosRadius)
		{
		int iRotationOrigin = (m_pType->RotatesWithSource(*pSource) ? pSource->GetRotation() : 0);
		return pSource->GetPos() + PolarToVector(iRotationOrigin + m_iPosAngle, (Metric)m_iPosRadius * g_KlicksPerPixel);
		}
	else
		return pSource->GetPos();
	}

ICCItemPtr COverlay::GetProperty (CCodeChainCtx &CCCtx, CSpaceObject &SourceObj, const CString &sProperty) const

//	GetProperty
//
//	Returns a property

	{
	ICCItemPtr pValue;

	if (FindCustomProperty(CCCtx, SourceObj, sProperty, pValue))
		return pValue;

	else if (strEquals(sProperty, PROPERTY_ACTIVE))
		return ICCItemPtr(!m_fInactive);

	else if (strEquals(sProperty, PROPERTY_COUNTER))
		return ICCItemPtr(m_iCounter);

	else if (strEquals(sProperty, PROPERTY_COUNTER_LABEL))
		return ICCItemPtr(!m_sMessage.IsBlank() ? m_sMessage : m_pType->GetCounterDesc().GetLabel());

	else if (strEquals(sProperty, PROPERTY_HP))
		return ICCItemPtr(m_iHitPoints);

	else if (strEquals(sProperty, PROPERTY_MAX_HP))
		return ICCItemPtr(m_pType->GetMaxHitPoints(SourceObj));

	else if (strEquals(sProperty, PROPERTY_POS))
		return ICCItemPtr(CreateListFromVector(GetPos(&SourceObj)));

	else if (strEquals(sProperty, PROPERTY_ROTATION))
		return ICCItemPtr(GetRotation());

	else if (strEquals(sProperty, PROPERTY_TYPE))
		return ICCItemPtr(GetType()->GetUNID());

	else
		return ICCItemPtr(ICCItem::Nil);
	}

bool COverlay::IncCustomProperty (CSpaceObject &SourceObj, const CString &sProperty, ICCItem *pInc, ICCItemPtr &pResult)

//	IncCustomProperty
//
//	Increment a custom property.

	{
	if (m_pType == NULL)
		return false;

	ICCItemPtr pDummy;
	EPropertyType iType;
	if (!m_pType->FindCustomProperty(sProperty, pDummy, &iType))
		return false;
		
	switch (iType)
		{
		case EPropertyType::propGlobal:
			pResult = m_pType->IncGlobalData(sProperty, pInc);
			return true;

		case EPropertyType::propData:
			pResult = IncData(sProperty, pInc);
			return true;

		default:
			return false;
		}
	}

bool COverlay::IncProperty (CSpaceObject &SourceObj, const CString &sProperty, ICCItem *pInc, ICCItemPtr &pResult)

//	IncProperty
//
//	Increments a property. Returns FALSE if we could not increment.

	{
	CCodeChain &CC = GetUniverse().GetCC();

	//	See if this is a custom property

	if (IncCustomProperty(SourceObj, sProperty, pInc, pResult))
		return true;

	else if (strEquals(sProperty, PROPERTY_COUNTER))
		{
		//	If we're a radius counter, then do unit conversion.

		if (m_pType->GetCounterDesc().GetType() == COverlayCounterDesc::counterRadius)
			{
			Metric rOldRadius = m_iCounter * g_KlicksPerPixel;
			Metric rRadiusInc;
			if (pInc && !pInc->IsNil())
				rRadiusInc = ParseDistance(pInc->GetStringValue(), LIGHT_SECOND);
			else
				rRadiusInc = LIGHT_SECOND;

			m_iCounter = mathRound((rOldRadius + rRadiusInc) / g_KlicksPerPixel);
			SourceObj.RefreshBounds();
			}

		//	Otherwise, take the integer value

		else if (pInc && !pInc->IsNil())
			m_iCounter += pInc->GetIntegerValue();

		else
			m_iCounter += 1;

		pResult = ICCItemPtr(m_iCounter);
		return true;
		}

	else if (strEquals(sProperty, PROPERTY_HP))
		{
		int iInc = 1;
		if (pInc && !pInc->IsNil())
			iInc = pInc->GetIntegerValue();

		m_iHitPoints = Min(Max(0, m_iHitPoints + iInc), m_pType->GetMaxHitPoints(SourceObj));

		pResult = ICCItemPtr(m_iHitPoints);
		return true;
		}

	else
		return false;
	}

bool COverlay::OnMiningDamage (CSpaceObject &Source, EAsteroidType iType, SDamageCtx &Ctx)

//	OnMiningDamage
//
//	Handles mining damage. Returns TRUE if the overlay handles the mining
//	damage.

	{
	//	If we're an underground vault, then we handle mining

	if (m_pType->IsUnderground())
		{
		FireOnMining(Source, iType, Ctx);
		return true;
		}

	//	Otherwise, nothing

	else
		return false;
	}

void COverlay::Paint (CG32bitImage &Dest, int iScale, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint the field. x,y is always the screen coordinates of the object center.

	{
	if (Ctx.pObj == NULL || !IsActive())
		return;

	int xOffset, yOffset, iRotationOrigin;
	CalcOffset(*Ctx.pObj, iScale, Ctx.iRotation, &xOffset, &yOffset, &iRotationOrigin);

	x = x + xOffset;
	y = y + yOffset;

	//	Adjust rotation

	int iSavedTick = Ctx.iTick;
	int iSavedRotation = Ctx.iRotation;
	Ctx.iRotation = (iRotationOrigin + m_iRotation) % 360;

	//	Paint

	if (m_fFading)
		{
		Ctx.iTick = m_iTick;
		m_pPainter->PaintFade(Dest, x, y, Ctx);
		}
	else if (m_iPaintHit > 0 && m_pHitPainter)
		{
		//	Paint hit effect

		Ctx.iTick = m_iPaintHitTick;
		m_pHitPainter->Paint(Dest, x, y, Ctx);
		}
	else if (m_pPainter)
		{
		Ctx.iTick = m_iTick;
		m_pPainter->Paint(Dest, x, y, Ctx);
		}

	//	Done

	Ctx.iRotation = iSavedRotation;
	Ctx.iTick = iSavedTick;
	}

void COverlay::PaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintAnnotations
//
//	Paint field annotations.

	{
	if (Ctx.pObj == NULL)
		{ }

	//	If we're an underground vault, then we sometimes paint a flag.

	else if (m_pType->IsUnderground())
		{
		if (!IsActive() && Ctx.pObj->IsExplored())
			{
			//	Flag

			TArray<CString> Text;
			strDelimit(m_sMessage, '\n', 2, &Text);
			PaintCounterFlag(Dest, x, y, Text[0], Text[1], Ctx.pObj->GetSymbolColor(), Ctx);

			//	Damage bar

			int iMaxHP = m_pType->GetMaxHitPoints(*Ctx.pObj);
			int cyHeight;

			CPaintHelper::PaintStatusBar(Dest,
					x,
					Ctx.yAnnotations,
					GetUniverse().GetPaintTick(),
					Ctx.pObj->GetSymbolColor(),
					CONSTLIT("Excavating"),
					m_iHitPoints,
					iMaxHP,
					&cyHeight);

			Ctx.yAnnotations += cyHeight + ANNOTATION_INNER_SPACING_Y;
			}
		}

	//	If inactive, we don't paint anything

	else if (!IsActive())
		{ }

	//	Otherwise, paint counter anomations

	else
		{
		const COverlayCounterDesc &Counter = m_pType->GetCounterDesc();

		switch (Counter.GetType())
			{
			case COverlayCounterDesc::counterFlag:
				PaintCounterFlag(Dest, x, y, strFromInt(m_iCounter), m_sMessage, Counter.GetColor(), Ctx);
				break;

			case COverlayCounterDesc::counterTextFlag:
				{
				TArray<CString> Text;
				strDelimit(m_sMessage, '\n', 2, &Text);
				PaintCounterFlag(Dest, x, y, Text[0], Text[1], Counter.GetColor(), Ctx);
				break;
				}

			case COverlayCounterDesc::counterProgress:
				{
				int cyHeight;

				CG32bitPixel rgbColor = Counter.GetColor();
				if (rgbColor.IsNull() && Ctx.pObj)
					rgbColor = Ctx.pObj->GetSymbolColor();

				CPaintHelper::PaintStatusBar(Dest,
						x,
						Ctx.yAnnotations,
						GetUniverse().GetPaintTick(),
						rgbColor,
						(!m_sMessage.IsBlank() ? m_sMessage : Counter.GetLabel()),
						Min(Max(0, m_iCounter), Counter.GetMaxValue()),
						Counter.GetMaxValue(),
						&cyHeight);

				Ctx.yAnnotations += cyHeight + ANNOTATION_INNER_SPACING_Y;
				break;
				}
			}
		}
	}

void COverlay::PaintBackground (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintBackground
//
//	Paints overlay background

	{
	if (!IsActive())
		return;

	const COverlayCounterDesc &Counter = m_pType->GetCounterDesc();

	switch (Counter.GetType())
		{
		case COverlayCounterDesc::counterRadius:
			{
			CG32bitPixel rgbColor = Counter.GetColor();
			if (rgbColor.IsNull() && Ctx.pObj)
				rgbColor = Ctx.pObj->GetSymbolColor();

			if (m_iCounter > 0)
				CGDraw::Circle(Dest, x, y, m_iCounter, CG32bitPixel(rgbColor, 64));

			//	Paint the label

			if (!m_sMessage.IsBlank())
				{
				const CG16bitFont &TextFont = GetUniverse().GetFont(CONSTLIT("SubTitle"));
				CG32bitPixel rgbTextColor = CG32bitPixel::Blend(rgbColor, CG32bitPixel(255, 255, 255), (BYTE)128);

				int yText = y + (m_iCounter / 2) - TextFont.GetHeight();
				if (yText + TextFont.GetHeight() > Ctx.rcView.bottom)
					yText = y - (m_iCounter / 2);

				Dest.DrawText(x, yText, TextFont, rgbTextColor, m_sMessage, CG16bitFont::AlignCenter);
				}

			break;
			}
		}
	}

void COverlay::PaintCounterFlag (CG32bitImage &Dest, int x, int y, const CString &sCounter, const CString &sLabel, CG32bitPixel rgbColor, SViewportPaintCtx &Ctx)

//	PaintCounterFlag
//
//	Paints a counter flag.

	{
	const CG16bitFont &CounterFont = g_pUniverse->GetNamedFont(CUniverse::fontSRSMessage);
	const CG16bitFont &LabelFont = g_pUniverse->GetNamedFont(CUniverse::fontSRSObjCounter);

	if (rgbColor.IsNull() && Ctx.pObj)
		rgbColor = Ctx.pObj->GetSymbolColor();

	//	Get the size of the object we're painting on.

	int cyHalfHeight = (RectHeight(Ctx.pObj->GetImage().GetImageRect())) / 2;
	int cyMast = cyHalfHeight + LabelFont.GetHeight() + CounterFont.GetHeight();

	//	Paint the mast

	int yTop = y - cyMast;
	int xText = x + FLAG_INNER_SPACING_X;

	Dest.DrawLine(x, yTop, x, y, 1, rgbColor);

	//	Paint the counter

	if (!sCounter.IsBlank())
		{
		CounterFont.DrawText(Dest, xText, yTop, rgbColor, sCounter);
		yTop += CounterFont.GetHeight();
		}

	//	Paint the label

	LabelFont.DrawText(Dest, xText, yTop, rgbColor, sLabel);
	}

void COverlay::PaintLRSAnnotations (const ViewportTransform &Trans, CG32bitImage &Dest, int x, int y)

//	PaintLRSAnnotations
//
//	Paints annotations on the LRS

	{
	if (!IsActive())
		return;

	const COverlayCounterDesc &Counter = m_pType->GetCounterDesc();

	switch (Counter.GetType())
		{
		case COverlayCounterDesc::counterFlag:
		case COverlayCounterDesc::counterTextFlag:
			{
			const CG16bitFont &CounterFont = GetUniverse().GetNamedFont(CUniverse::fontMapLabel);

			CG32bitPixel rgbColor = Counter.GetColor();

			//	We paint the text to the right and vertically centered

			int yTop = y - (CounterFont.GetHeight() / 2);
			int xLeft = x + FLAG_INNER_SPACING_X;

			//	Paint the counter

			CounterFont.DrawText(Dest, xLeft, yTop, rgbColor, strFromInt(m_iCounter));
			break;
			}
		}
	}

void COverlay::PaintMapAnnotations (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y)

//	PaintMapAnnotations
//
//	Paints annotations on the system map

	{
	if (!IsActive())
		return;

	const COverlayCounterDesc &Counter = m_pType->GetCounterDesc();

	switch (Counter.GetType())
		{
		case COverlayCounterDesc::counterFlag:
		case COverlayCounterDesc::counterTextFlag:
			{
			const CG16bitFont &CounterFont = GetUniverse().GetNamedFont(CUniverse::fontMapLabel);

			CG32bitPixel rgbColor = Counter.GetColor();

			//	We paint the text to the right and vertically centered

			int yTop = y - (CounterFont.GetHeight() / 2);
			int xLeft = x + FLAG_INNER_SPACING_X;

			//	Paint the counter

			CounterFont.DrawText(Dest, xLeft, yTop, rgbColor, strFromInt(m_iCounter));
			break;
			}
		}
	}

void COverlay::ReadFromStream (SLoadCtx &Ctx)

//	ReadFromStream
//
//	DWORD	UNID of energy field type
//	DWORD	m_dwID
//	DWORD	m_iPosAngle
//	DWORD	m_iPosRadius
//	DWORD	m_iRotation
//	DWORD	m_iPosZ
//	DWORD	m_iDevice
//	DWORD	Life left
//	DWORD	m_iHitPoints
//	CAttributeDataBlock m_Data
//	DWORD	m_iCounter
//	CString	m_sMessage
//	IPainter	m_pPaint
//	IPainter	m_pHitPainter
//	DWORD	Flags

	{
	DWORD dwLoad;

	Ctx.pStream->Read(dwLoad);
	m_pType = Ctx.GetUniverse().FindShipEnergyFieldType(dwLoad);
	if (m_pType == NULL)
		throw CException(ERR_FAIL, strPatternSubst(CONSTLIT("Undefined overlay type: %08x"), dwLoad));

	if (Ctx.dwVersion >= 38)
		{
		Ctx.pStream->Read(m_dwID);
		Ctx.pStream->Read(m_iPosAngle);
		Ctx.pStream->Read(m_iPosRadius);
		Ctx.pStream->Read(m_iRotation);
		}
	else
		{
		m_dwID = Ctx.GetUniverse().CreateGlobalID();
		m_iPosAngle = 0;
		m_iPosRadius = 0;
		m_iRotation = 0;
		}

	if (Ctx.dwVersion >= 168)
		{
		Ctx.pStream->Read(m_iPosZ);
		}
	else
		{
		m_iPosZ = 0;
		}

	if (Ctx.dwVersion >= 63)
		Ctx.pStream->Read(m_iDevice);
	else
		m_iDevice = -1;

	if (Ctx.dwVersion >= 124)
		Ctx.pStream->Read(m_iTick);
	else
		m_iTick = 0;

	Ctx.pStream->Read(m_iLifeLeft);

	if (Ctx.dwVersion >= 184)
		Ctx.pStream->Read(m_iHitPoints);
	else
		m_iHitPoints = 0;

	if (Ctx.dwVersion >= 39)
		m_Data.ReadFromStream(Ctx);

	if (Ctx.dwVersion >= 103)
		{
		Ctx.pStream->Read(m_iCounter);
		m_sMessage.ReadFromStream(Ctx.pStream);
		}

	m_pPainter = CEffectCreator::CreatePainterFromStreamAndCreator(Ctx, m_pType->GetEffectCreator());
	m_pHitPainter = CEffectCreator::CreatePainterFromStreamAndCreator(Ctx, m_pType->GetHitEffectCreator());

	m_iPaintHit = 0;

	//	Flags

	DWORD dwFlags = 0;
	if (Ctx.dwVersion >= 101)
		Ctx.pStream->Read(dwFlags);

	//	NOTE: We need to save the destroyed flag because we defer removal of
	//	overlays. There are cases where we set the flag, save the game, and then
	//	remove the overlay on the first update after load.
	//
	//	We have to saved destroyed overlays because we need to run some code
	//	when removing an overlay (e.g., see CShip::CalcOverlayImpact).

	m_fDestroyed =	((dwFlags & 0x00000001) ? true : false);
	m_fFading =		((dwFlags & 0x00000002) ? true : false);
	m_fInactive =	((dwFlags & 0x00000004) ? true : false);
	}

void COverlay::SetActive (CSpaceObject &Source, bool bActive)

//	SetActive
//
//	Set active or inactive

	{
	if (bActive != !m_fInactive)
		{
		m_fInactive = !bActive;

		//	Activating may enable dock screens, so we need to refresh various
		//	cached flags on the object.

		Source.SetEventFlags();

		//	Refresh bounds, since the overlay may need to be shown.

		Source.RefreshBounds();
		}
	}

bool COverlay::SetCustomProperty (CSpaceObject &SourceObj, const CString &sProperty, ICCItem *pValue)

//	SetCustomProperty
//
//	Sets a property. Returns TRUE if this was a valid property.

	{
	if (m_pType == NULL)
		return false;

	ICCItemPtr pDummy;
	EPropertyType iType;
	if (!m_pType->FindCustomProperty(sProperty, pDummy, &iType))
		return false;
		
	switch (iType)
		{
		case EPropertyType::propGlobal:
			m_pType->SetGlobalData(sProperty, pValue);
			return true;

		case EPropertyType::propData:
			SetData(sProperty, pValue);
			return true;

		default:
			return false;
		}
	}

bool COverlay::SetEffectProperty (const CString &sProperty, ICCItem *pValue)

//	SetEffectProperty
//
//	Sets a property on the effect

	{
	if (m_pPainter)
		return m_pPainter->SetProperty(sProperty, pValue);

	return false;
	}

void COverlay::SetPos (CSpaceObject *pSource, const CVector &vPos)

//	SetPos
//
//	Sets the position of the overlay

	{
	Metric rRadius;
	int iDirection = VectorToPolar(vPos - pSource->GetPos(), &rRadius);
	int iRotationOrigin = (m_pType->RotatesWithSource(*pSource) ? pSource->GetRotation() : 0);
	m_iPosAngle = (iDirection + 360 - iRotationOrigin) % 360;
	m_iPosRadius = (int)(rRadius / g_KlicksPerPixel);
	}

bool COverlay::SetProperty (CSpaceObject &Source, const CString &sName, ICCItem *pValue)

//	SetProperty
//
//	Sets the property

	{
	CCodeChain &CC = GetUniverse().GetCC();

	//	See if this is a custom property

	if (SetCustomProperty(Source, sName, pValue))
		return true;

	else if (strEquals(sName, PROPERTY_ACTIVE))
		{
		SetActive(Source, !pValue->IsNil());
		}
	else if (strEquals(sName, PROPERTY_COUNTER))
		{
		//	If we're a radius counter, then do unit conversion.

		if (m_pType->GetCounterDesc().GetType() == COverlayCounterDesc::counterRadius)
			{
			Metric rRadius = ParseDistance(pValue->GetStringValue(), LIGHT_SECOND);
			m_iCounter = mathRound(rRadius / g_KlicksPerPixel);
			Source.RefreshBounds();
			}

		//	Otherwise, take the integer value

		else
			m_iCounter = pValue->GetIntegerValue();
		}

	else if (strEquals(sName, PROPERTY_COUNTER_LABEL))
		m_sMessage = pValue->GetStringValue();

	else if (strEquals(sName, PROPERTY_HP))
		m_iHitPoints = Min(Max(0, pValue->GetIntegerValue()), m_pType->GetMaxHitPoints(Source));

	else if (strEquals(sName, PROPERTY_POS))
		{
		CVector vPos = CreateVectorFromList(CC, pValue);
		SetPos(&Source, vPos);
		}

	else if (strEquals(sName, PROPERTY_ROTATION))
		SetRotation(pValue->GetIntegerValue());

	//	If nothing else, ask the painter

	else if (m_pPainter)
		return m_pPainter->SetProperty(sName, pValue);
	else
		return false;

	return true;
	}

void COverlay::Update (CSpaceObject *pSource, int iScale, int iRotation, bool *retbModified)

//	Update
//
//	Update the field

	{
	//	See if our lifetime has expired

	if (m_iLifeLeft != -1 && --m_iLifeLeft == 0)
		{
		if (m_fFading)
			m_fFading = false;
		else
			Destroy(pSource);

		//	No need to mark as modified; the destruction should take care
		//	of that.

		if (retbModified)
			*retbModified = false;

		return;
		}

	bool bModified = false;

	//	Update paint hit counter

	if (m_iPaintHit > 0)
		{
		m_iPaintHit--;
		m_iPaintHitTick++;

		if (m_iPaintHit == 0 && m_pHitPainter)
			{
			m_pHitPainter->Delete();
			m_pHitPainter = NULL;
			bModified = true;
			}
		}

	//	Update the painters

	SEffectUpdateCtx UpdateCtx;
	UpdateCtx.pSystem = pSource->GetSystem();
	UpdateCtx.pObj = pSource;
	UpdateCtx.iTick = m_iTick;
	UpdateCtx.iRotation = m_iRotation;
	UpdateCtx.bFade = m_fFading;

	SEffectMoveCtx MoveCtx;
	MoveCtx.pObj = pSource;

	//	If our painter needs an origin, then compute it and set it.

	if (m_pPainter && m_pPainter->UsesOrigin())
		{
		int xOffset, yOffset, iRotationOrigin;
		CalcOffset(*pSource, iScale, iRotation, &xOffset, &yOffset, &iRotationOrigin);

		MoveCtx.bUseOrigin = true;
		MoveCtx.vOrigin = pSource->GetPos() + CVector(xOffset * g_KlicksPerPixel, -yOffset * g_KlicksPerPixel);
		}

	//	Update
	
	if (m_pPainter)
		{
		bool bBoundsChanged;

		m_pPainter->OnUpdate(UpdateCtx);
		m_pPainter->OnMove(MoveCtx, &bBoundsChanged);

		if (bBoundsChanged)
			bModified = true;
		}

	if (m_pHitPainter)
		{
		bool bBoundsChanged;

		UpdateCtx.iTick = m_iPaintHitTick;
		m_pHitPainter->OnUpdate(UpdateCtx);
		m_pHitPainter->OnMove(MoveCtx, &bBoundsChanged);

		if (bBoundsChanged)
			bModified = true;
		}

	//	Call OnUpdate

	if (m_pType->FindEventHandler(CDesignType::evtOnUpdate)
			&& pSource->IsDestinyTime(OVERLAY_ON_UPDATE_CYCLE, OVERLAY_ON_UPDATE_OFFSET)
			&& !IsDestroyed())
		{
		FireOnUpdate(pSource);
		}

	m_iTick++;

	//	Done

	if (retbModified)
		*retbModified = bModified;
	}

void COverlay::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//
//	DWORD	UNID of energy field type
//	DWORD	m_dwID
//	DWORD	m_iPosAngle
//	DWORD	m_iPosRadius
//	DWORD	m_iRotation
//	DWORD	m_iPosZ
//	DWORD	m_iDevice
//	DWORD	m_iTick
//	DWORD	Life left
//	DWORD	m_iHitPoints
//	CAttributeDataBlock m_Data
//	DWORD	m_iCounter
//	CString	m_sMessage
//	IPainter	m_pPaint
//	IPainter	m_pHitPainter
//	DWORD	Flags

	{
	DWORD dwSave = m_pType->GetUNID();
	pStream->Write(dwSave);
	pStream->Write(m_dwID);
	pStream->Write(m_iPosAngle);
	pStream->Write(m_iPosRadius);
	pStream->Write(m_iRotation);
	pStream->Write(m_iPosZ);
	pStream->Write(m_iDevice);
	pStream->Write(m_iTick);
	pStream->Write(m_iLifeLeft);
	pStream->Write(m_iHitPoints);

	m_Data.WriteToStream(pStream);

	pStream->Write(m_iCounter);
	m_sMessage.WriteToStream(pStream);

	CEffectCreator::WritePainterToStream(pStream, m_pPainter);
	CEffectCreator::WritePainterToStream(pStream, m_pHitPainter);

	DWORD dwFlags = 0;
	dwFlags |= (m_fDestroyed ?	0x00000001 : 0);
	dwFlags |= (m_fFading ?		0x00000002 : 0);
	dwFlags |= (m_fInactive ?	0x00000004 : 0);
	pStream->Write(dwFlags);
	}

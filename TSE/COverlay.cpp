//	COverlay.cpp
//
//	COverlay class

#include "PreComp.h"

#define EVENT_GET_DOCK_SCREEN					CONSTLIT("GetDockScreen")
#define EVENT_ON_OBJ_DOCKED						CONSTLIT("OnObjDocked")

#define ON_CREATE_EVENT							CONSTLIT("OnCreate")
#define ON_DAMAGE_EVENT							CONSTLIT("OnDamage")
#define ON_DESTROY_EVENT						CONSTLIT("OnDestroy")
#define ON_OBJ_DESTROYED_EVENT					CONSTLIT("OnObjDestroyed")
#define ON_UPDATE_EVENT							CONSTLIT("OnUpdate")

#define OVERLAY_RADIUS							(10.0 * g_KlicksPerPixel)
#define OVERLAY_RADIUS2							(OVERLAY_RADIUS * OVERLAY_RADIUS)

#define LIFETIME_ATTRIB							CONSTLIT("lifetime")

#define PROPERTY_COUNTER						CONSTLIT("counter")
#define PROPERTY_COUNTER_LABEL					CONSTLIT("counterLabel")
#define PROPERTY_POS							CONSTLIT("pos")
#define PROPERTY_ROTATION						CONSTLIT("rotation")
#define PROPERTY_TYPE							CONSTLIT("type")

const int ANNOTATION_INNER_SPACING_Y =			2;
const int FLAG_INNER_SPACING_X =				4;

COverlay::COverlay (void) : 
		m_pType(NULL),
		m_iCounter(0),
		m_pPainter(NULL),
		m_pHitPainter(NULL),
		m_fDestroyed(false),
		m_fFading(false),
		m_pNext(NULL)

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
	//	If we absorb damage, then see how much we absorbed

	int iAbsorb = m_pType->GetDamageAbsorbed(pSource, Ctx);
	if (iAbsorb > 0)
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
	switch (m_pType->GetCounterStyle())
		{
		case COverlayType::counterRadius:
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
		CalcOffset(iScale, iRotation, &xOffset, &yOffset);

		::OffsetRect(&rcRect, xOffset, yOffset);

		//	Adjust resulting RECT to include painter rect

		::UnionRect(ioBounds, ioBounds, &rcRect);
		}
	}

void COverlay::CalcOffset (int iScale, int iRotation, int *retxOffset, int *retyOffset, int *retiRotationOrigin) const

//	CalcOffset
//
//	Compute the offset where we should paint the overlay relative to the object
//	center (in pixels).

	{
	//	Adjust position, if necessary

	if (m_iPosRadius)
		{
		//	Do we rotate with the source?

		int iRotationOrigin = (m_pType->RotatesWithShip() ? iRotation : 0);
		if (retiRotationOrigin)
			*retiRotationOrigin = iRotationOrigin;

		//	Adjust based on position

		C3DConversion::CalcCoord(iScale, iRotationOrigin + m_iPosAngle, m_iPosRadius, 0, retxOffset, retyOffset);
		}

	//	Otherwise, we're at the center

	else
		{
		*retxOffset = 0;
		*retyOffset = 0;

		if (retiRotationOrigin)
			*retiRotationOrigin = (m_pType->RotatesWithShip() ? iRotation : 0);
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
		//	Create a painter

		if (m_pHitPainter)
			m_pHitPainter->Delete();
		m_pHitPainter = pHitEffect->CreatePainter(CCreatePainterCtx());

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

void COverlay::CreateFromType (COverlayType *pType, 
								   int iPosAngle,
								   int iPosRadius,
								   int iRotation,
								   int iLifeLeft, 
								   COverlay **retpField)

//	CreateFromType
//
//	Create field from type

	{
	ASSERT(pType);

	COverlay *pField = new COverlay;

	pField->m_pType = pType;
	pField->m_dwID = g_pUniverse->CreateGlobalID();
	pField->m_iDevice = -1;
	pField->m_iLifeLeft = iLifeLeft;
	pField->m_iTick = 0;
	pField->m_iPosAngle = iPosAngle;
	pField->m_iPosRadius = iPosRadius;
	pField->m_iRotation = iRotation;
	pField->m_iPaintHit = 0;

	//	Create painters

	CEffectCreator *pCreator = pType->GetEffectCreator();
	if (pCreator)
		{
		CCreatePainterCtx CreateCtx;
		CreateCtx.SetAPIVersion(pType->GetAPIVersion());
		CreateCtx.SetLifetime(pField->m_iLifeLeft);

		pField->m_pPainter = pCreator->CreatePainter(CreateCtx);
		}

	pField->m_pHitPainter = NULL;

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

void COverlay::FireCustomEvent (CSpaceObject *pSource, const CString &sEvent, ICCItem *pData, ICCItem **retpResult)

//	FireCustomEvent
//
//	Fires a custom event

	{
	SEventHandlerDesc Event;
	if (m_pType->FindEventHandler(sEvent, &Event))
		{
		CCodeChainCtx Ctx;
		
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
		*retpResult = g_pUniverse->GetCC().CreateNil();
	}

bool COverlay::FireGetDockScreen (CSpaceObject *pSource, CString *retsScreen, int *retiPriority, ICCItemPtr *retpData) const

//	FireGetDockScreen
//
//	Fires GetDockScreen event.

	{
	SEventHandlerDesc Event;
	if (!m_pType->FindEventHandler(EVENT_GET_DOCK_SCREEN, &Event))
		return false;

	CCodeChainCtx Ctx;
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(pSource);
	Ctx.SaveAndDefineOverlayID(m_dwID);

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		{
		pSource->ReportEventError(EVENT_GET_DOCK_SCREEN, pResult);
		return false;
		}

	return CTLispConvert::AsScreen(pResult, retsScreen, retpData, retiPriority);
	}

void COverlay::FireOnCreate (CSpaceObject *pSource)

//	FireOnCreate
//
//	Fire OnCreate event

	{
	SEventHandlerDesc Event;
	if (m_pType->FindEventHandler(ON_CREATE_EVENT, &Event))
		{
		CCodeChainCtx Ctx;
		
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
		CCodeChainCtx CCCtx;
		
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
		CCodeChainCtx Ctx;

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

void COverlay::FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const

//	FireOnObjDestroyed
//
//	OnObjDestroyed event

	{
	SEventHandlerDesc Event;
	if (m_pType->FindEventHandler(ON_OBJ_DESTROYED_EVENT, &Event))
		{
		CCodeChainCtx CCCtx;

		//	Setup

		CCCtx.SetEvent(eventOverlayEvent);
		CCCtx.DefineContainingType(this);
		CCCtx.SaveAndDefineSourceVar(pSource);
		CCCtx.SaveAndDefineOverlayID(m_dwID);
		CCCtx.DefineSpaceObject(CONSTLIT("aObjDestroyed"), Ctx.pObj);
		CCCtx.DefineSpaceObject(CONSTLIT("aDestroyer"), Ctx.Attacker.GetObj());
		CCCtx.DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
		CCCtx.DefineSpaceObject(CONSTLIT("aWreckObj"), Ctx.pWreck);
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
		CCodeChainCtx Ctx;

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
		CCodeChainCtx Ctx;

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

CVector COverlay::GetPos (CSpaceObject *pSource)

//	GetPos
//
//	Returns the current absolute position of the overlay

	{
	if (m_iPosRadius)
		{
		int iRotationOrigin = (m_pType->RotatesWithShip() ? pSource->GetRotation() : 0);
		return pSource->GetPos() + PolarToVector(iRotationOrigin + m_iPosAngle, (Metric)m_iPosRadius * g_KlicksPerPixel);
		}
	else
		return pSource->GetPos();
	}

ICCItem *COverlay::GetProperty (CCodeChainCtx *pCCCtx, CSpaceObject *pSource, const CString &sName)

//	GetProperty
//
//	Returns a property

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sName, PROPERTY_COUNTER))
		return CC.CreateInteger(m_iCounter);

	else if (strEquals(sName, PROPERTY_COUNTER_LABEL))
		return CC.CreateString(!m_sMessage.IsBlank() ? m_sMessage : m_pType->GetCounterLabel());

	else if (strEquals(sName, PROPERTY_POS))
		return CreateListFromVector(CC, GetPos(pSource));

	else if (strEquals(sName, PROPERTY_ROTATION))
		return CC.CreateInteger(GetRotation());

	else if (strEquals(sName, PROPERTY_TYPE))
		return CC.CreateInteger(GetType()->GetUNID());

	else
		return CC.CreateNil();
	}

void COverlay::Paint (CG32bitImage &Dest, int iScale, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paint the field. x,y is always the screen coordinates of the object center.

	{
	int xOffset, yOffset, iRotationOrigin;
	CalcOffset(iScale, Ctx.iRotation, &xOffset, &yOffset, &iRotationOrigin);

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
	switch (m_pType->GetCounterStyle())
		{
		case COverlayType::counterFlag:
			{
			const CG16bitFont &CounterFont = g_pUniverse->GetNamedFont(CUniverse::fontSRSMessage);
			const CG16bitFont &LabelFont = g_pUniverse->GetNamedFont(CUniverse::fontSRSObjCounter);

			CG32bitPixel rgbColor = m_pType->GetCounterColor();
			if (rgbColor.IsNull() && Ctx.pObj)
				rgbColor = Ctx.pObj->GetSymbolColor();

			//	Get the size of the object we're painting on.

			int cyHalfHeight = (RectHeight(Ctx.pObj->GetImage().GetImageRect())) / 2;
			int cyMast = cyHalfHeight + LabelFont.GetHeight() + CounterFont.GetHeight();

			//	Paint the mast

			int yTop = y - cyMast;
			Dest.DrawLine(x, yTop, x, y, 1, rgbColor);

			//	Paint the counter

			int xText = x + FLAG_INNER_SPACING_X;
			CounterFont.DrawText(Dest, xText, yTop, rgbColor, strFromInt(m_iCounter));

			//	Paint the label

			yTop += CounterFont.GetHeight();
			LabelFont.DrawText(Dest, xText, yTop, rgbColor, m_sMessage);

			break;
			}

		case COverlayType::counterProgress:
			{
			int cyHeight;

			CG32bitPixel rgbColor = m_pType->GetCounterColor();
			if (rgbColor.IsNull() && Ctx.pObj)
				rgbColor = Ctx.pObj->GetSymbolColor();

			CPaintHelper::PaintStatusBar(Dest,
					x,
					Ctx.yAnnotations,
					g_pUniverse->GetPaintTick(),
					rgbColor,
					(!m_sMessage.IsBlank() ? m_sMessage : m_pType->GetCounterLabel()),
					Min(Max(0, m_iCounter), m_pType->GetCounterMax()),
					m_pType->GetCounterMax(),
					&cyHeight);

			Ctx.yAnnotations += cyHeight + ANNOTATION_INNER_SPACING_Y;
			break;
			}
		}
	}

void COverlay::PaintBackground (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintBackground
//
//	Paints overlay background

	{
	switch (m_pType->GetCounterStyle())
		{
		case COverlayType::counterRadius:
			{
			CG32bitPixel rgbColor = m_pType->GetCounterColor();
			if (rgbColor.IsNull() && Ctx.pObj)
				rgbColor = Ctx.pObj->GetSymbolColor();

			if (m_iCounter > 0)
				CGDraw::Circle(Dest, x, y, m_iCounter, CG32bitPixel(rgbColor, 64));

			//	Paint the label

			if (!m_sMessage.IsBlank())
				{
				const CG16bitFont &TextFont = g_pUniverse->GetFont(CONSTLIT("SubTitle"));
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

void COverlay::PaintLRSAnnotations (const ViewportTransform &Trans, CG32bitImage &Dest, int x, int y)

//	PaintLRSAnnotations
//
//	Paints annotations on the LRS

	{
	switch (m_pType->GetCounterStyle())
		{
		case COverlayType::counterFlag:
			{
			const CG16bitFont &CounterFont = g_pUniverse->GetNamedFont(CUniverse::fontMapLabel);

			CG32bitPixel rgbColor = m_pType->GetCounterColor();

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
	switch (m_pType->GetCounterStyle())
		{
		case COverlayType::counterFlag:
			{
			const CG16bitFont &CounterFont = g_pUniverse->GetNamedFont(CUniverse::fontMapLabel);

			CG32bitPixel rgbColor = m_pType->GetCounterColor();

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
//	DWORD	m_iDevice
//	DWORD	Life left
//	CAttributeDataBlock m_Data
//	DWORD	m_iCounter
//	CString	m_sMessage
//	IPainter	m_pPaint
//	IPainter	m_pHitPainter
//	DWORD	Flags

	{
	DWORD dwLoad;

	Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
	m_pType = g_pUniverse->FindShipEnergyFieldType(dwLoad);
	if (m_pType == NULL)
		{
		kernelDebugLogPattern("Unable to find overlay type: %x", dwLoad);
		throw CException(ERR_FAIL);
		}

	if (Ctx.dwVersion >= 38)
		{
		Ctx.pStream->Read((char *)&m_dwID, sizeof(DWORD));
		Ctx.pStream->Read((char *)&m_iPosAngle, sizeof(DWORD));
		Ctx.pStream->Read((char *)&m_iPosRadius, sizeof(DWORD));
		Ctx.pStream->Read((char *)&m_iRotation, sizeof(DWORD));
		}
	else
		{
		m_dwID = g_pUniverse->CreateGlobalID();
		m_iPosAngle = 0;
		m_iPosRadius = 0;
		m_iRotation = 0;
		}

	if (Ctx.dwVersion >= 63)
		Ctx.pStream->Read((char *)&m_iDevice, sizeof(DWORD));
	else
		m_iDevice = -1;

	if (Ctx.dwVersion >= 124)
		Ctx.pStream->Read((char *)&m_iTick, sizeof(DWORD));
	else
		m_iTick = 0;

	Ctx.pStream->Read((char *)&m_iLifeLeft, sizeof(DWORD));

	if (Ctx.dwVersion >= 39)
		m_Data.ReadFromStream(Ctx);

	if (Ctx.dwVersion >= 103)
		{
		Ctx.pStream->Read((char *)&m_iCounter, sizeof(DWORD));
		m_sMessage.ReadFromStream(Ctx.pStream);
		}

	m_pPainter = CEffectCreator::CreatePainterFromStreamAndCreator(Ctx, m_pType->GetEffectCreator());
	m_pHitPainter = CEffectCreator::CreatePainterFromStreamAndCreator(Ctx, m_pType->GetHitEffectCreator());

	m_iPaintHit = 0;

	//	Flags

	DWORD dwFlags = 0;
	if (Ctx.dwVersion >= 101)
		Ctx.pStream->Read((char *)&dwFlags, sizeof(DWORD));

	//	NOTE: We need to saved the destroyed flag because we defer removal of
	//	overlays. There are cases where we set the flag, save the game, and then
	//	remove the overlay on the first update after load.
	//
	//	We have to saved destroyed overlays because we need to run some code
	//	when removing an overlay (e.g., see CShip::CalcOverlayImpact).

	m_fDestroyed =	((dwFlags & 0x00000001) ? true : false);
	m_fFading =		((dwFlags & 0x00000002) ? true : false);
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
	int iRotationOrigin = (m_pType->RotatesWithShip() ? pSource->GetRotation() : 0);
	m_iPosAngle = (iDirection + 360 - iRotationOrigin) % 360;
	m_iPosRadius = (int)(rRadius / g_KlicksPerPixel);
	}

bool COverlay::SetProperty (CSpaceObject *pSource, const CString &sName, ICCItem *pValue)

//	SetProperty
//
//	Sets the property

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sName, PROPERTY_COUNTER))
		{
		//	If we're a radius counter, then do unit conversion.

		if (m_pType->GetCounterStyle() == COverlayType::counterRadius)
			{
			Metric rRadius = ParseDistance(pValue->GetStringValue(), LIGHT_SECOND);
			m_iCounter = (int)((rRadius / g_KlicksPerPixel) + 0.5);
			pSource->RefreshBounds();
			}

		//	Otherwise, take the integer value

		else
			m_iCounter = pValue->GetIntegerValue();
		}

	else if (strEquals(sName, PROPERTY_COUNTER_LABEL))
		m_sMessage = pValue->GetStringValue();

	else if (strEquals(sName, PROPERTY_POS))
		{
		CVector vPos = CreateVectorFromList(CC, pValue);
		SetPos(pSource, vPos);
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
		CalcOffset(iScale, iRotation, &xOffset, &yOffset, &iRotationOrigin);

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
//	DWORD	m_iDevice
//	DWORD	m_iTick
//	DWORD	Life left
//	CAttributeDataBlock m_Data
//	DWORD	m_iCounter
//	CString	m_sMessage
//	IPainter	m_pPaint
//	IPainter	m_pHitPainter
//	DWORD	Flags

	{
	DWORD dwSave = m_pType->GetUNID();
	pStream->Write((char *)&dwSave, sizeof(DWORD));
	pStream->Write((char *)&m_dwID, sizeof(DWORD));
	pStream->Write((char *)&m_iPosAngle, sizeof(DWORD));
	pStream->Write((char *)&m_iPosRadius, sizeof(DWORD));
	pStream->Write((char *)&m_iRotation, sizeof(DWORD));
	pStream->Write((char *)&m_iDevice, sizeof(DWORD));
	pStream->Write((char *)&m_iTick, sizeof(DWORD));
	pStream->Write((char *)&m_iLifeLeft, sizeof(DWORD));

	m_Data.WriteToStream(pStream);

	pStream->Write((char *)&m_iCounter, sizeof(DWORD));
	m_sMessage.WriteToStream(pStream);

	CEffectCreator::WritePainterToStream(pStream, m_pPainter);
	CEffectCreator::WritePainterToStream(pStream, m_pHitPainter);

	DWORD dwFlags = 0;
	dwFlags |= (m_fDestroyed ?	0x00000001 : 0);
	dwFlags |= (m_fFading ?		0x00000002 : 0);
	pStream->Write((char *)&dwFlags, sizeof(DWORD));
	}


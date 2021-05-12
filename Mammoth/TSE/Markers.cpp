//	Markers.cpp
//
//	Marker classes

#include "PreComp.h"

#define PROPERTY_STYLE  						CONSTLIT("style")

#define STYLE_SMALL_CROSS  						CONSTLIT("smallCross")

const CG32bitPixel RGB_ORBIT_LINE =		        CG32bitPixel(115, 149, 229);

//	CMarker -------------------------------------------------------------------

static TStaticStringTable<TStaticStringEntry<CMarker::EStyle>, 2> STYLE_TABLE = {
	"message",				CMarker::EStyle::Message,
	"smallCross",			CMarker::EStyle::SmallCross,
	};

CMarker::CMarker (CUniverse &Universe) : TSpaceObjectImpl(Universe)

//	CMarker constructor

	{
	}

CMarker::~CMarker (void)

//  CMarker destructor

    {
    if (m_pMapOrbit)
        delete m_pMapOrbit;
    }

ALERROR CMarker::Create (CSystem &System,
						 const CVector &vPos,
						 const CVector &vVel,
					     const SCreateOptions &Options,
						 CMarker **retpMarker)

//	Create
//
//	Create a marker

	{
	ALERROR error;
	CMarker *pMarker;

	pMarker = new CMarker(System.GetUniverse());
	if (pMarker == NULL)
		return ERR_MEMORY;

	pMarker->Place(vPos, vVel);
	pMarker->SetCannotBeHit();

	//	Note: Cannot disable destruction notification because some markers
	//	are used as destination targets for ships

	pMarker->m_sName = Options.sName;
	pMarker->m_pSovereign = Options.pSovereign;
	pMarker->m_iStyle = Options.iStyle;
	if (pMarker->m_pSovereign == NULL)
		pMarker->m_pSovereign = System.GetUniverse().FindSovereign(g_PlayerSovereignUNID);

	pMarker->m_dwCreatedOn = System.GetUniverse().GetTicks();
	if (Options.iLifetime > 0)
		pMarker->m_dwDestroyOn = System.GetUniverse().GetTicks() + Options.iLifetime;

	//	Add to system

	if (error = pMarker->AddToSystem(System))
		{
		delete pMarker;
		return error;
		}

	//	Done

	if (retpMarker)
		*retpMarker = pMarker;

	return NOERROR;
	}

ICCItem *CMarker::GetPropertyCompatible (CCodeChainCtx &Ctx, const CString &sName) const

//	GetProperty
//
//	Returns a property

	{
	CCodeChain &CC = GetUniverse().GetCC();

    if (strEquals(sName, PROPERTY_STYLE))
        {
        switch (m_iStyle)
            {
            case EStyle::SmallCross:
                return CC.CreateString(STYLE_SMALL_CROSS);

            default:
                return CC.CreateNil();
            }
        }

	else
		return CSpaceObject::GetPropertyCompatible(Ctx, sName);
	}

CSovereign *CMarker::GetSovereign (void) const
	{
	//	Return player

	return m_pSovereign;
	}

CString CMarker::GetStyleID (EStyle iStyle)
	{
	switch (iStyle)
		{
		case EStyle::None:
		case EStyle::Error:
			return NULL_STR;

		default:
			return STYLE_TABLE.FindKey(iStyle);
		}
	}

void CMarker::OnObjLeaveGate (CSpaceObject *pObj)

//	OnObjLeaveGate
//
//	Object leaves a gate

	{
	CEffectCreator *pEffect = GetUniverse().FindEffectType(g_StargateOutUNID);
	if (pEffect)
		pEffect->CreateEffect(GetSystem(),
				NULL,
				GetPos(),
				NullVector,
				0);
	}

void CMarker::OnPaint (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	OnPaint
//
//	Paint the marker

	{
    switch (m_iStyle)
        {
        case EStyle::SmallCross:
            Dest.DrawDot(x, y, CG32bitPixel(255, 255, 0), markerSmallCross);
            break;

		case EStyle::Message:
			PaintMessage(Dest, x, y, Ctx);
			break;
        }
	}

void CMarker::OnPaintMap (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y)

//  OnPaintMap
//
//  Paints on the system map

    {
	//	Draw an orbit

	if (m_pMapOrbit)
		m_pMapOrbit->Paint(Ctx, Dest, RGB_ORBIT_LINE);
    }

void CMarker::OnReadFromStream (SLoadCtx &Ctx)

//	OnReadFromStream
//
//	Read the object's data from a stream
//
//	CString		m_sName

	{
	if (Ctx.dwVersion >= 8)
		{
		m_sName.ReadFromStream(Ctx.pStream);
		CSystem::ReadSovereignRefFromStream(Ctx, &m_pSovereign);
		}

    if (Ctx.dwVersion >= 126)
        {
        DWORD dwLoad;
        Ctx.pStream->Read(dwLoad);
        m_iStyle = (EStyle)dwLoad;
        }
    else
        m_iStyle = EStyle::None;

    DWORD dwFlags = 0;
    if (Ctx.dwVersion >= 134)
        Ctx.pStream->Read(dwFlags);

    bool bHasMapOrbit = ((dwFlags & 0x00000001) ? true : false);

	//	Other fields

	if (Ctx.dwVersion >= 206)
		{
		Ctx.pStream->Read(m_dwCreatedOn);
		Ctx.pStream->Read(m_dwDestroyOn);
		}

    //  Read map orbit, if we have it.

    if (bHasMapOrbit)
        {
        m_pMapOrbit = new COrbit;
		m_pMapOrbit->ReadFromStream(Ctx);
        }
    else
        m_pMapOrbit = NULL;
	}

void CMarker::OnUpdate (SUpdateCtx &Ctx, Metric rSecondsPerTick)

//	OnUpdate
//
//	Updates

	{
	if (m_dwDestroyOn
			&& GetUniverse().GetTicks() >= m_dwDestroyOn)
		{
		Destroy(removedFromSystem, CDamageSource());
		return;
		}
	}

void CMarker::OnWriteToStream (IWriteStream *pStream)

//	OnWriteToStream
//
//	Write the object's data to a stream
//
//	CString		m_sName
//	DWORD		m_pSovereign (CSovereign ref)
//  DWORD       m_iStyle
//  DWORD       Flags
//	DWORD		m_dwCreatedOn
//	DWORD		m_dwDestroyOn
//  COrbit      (optional)

	{
	m_sName.WriteToStream(pStream);
	CSystem::WriteSovereignRefToStream(m_pSovereign, pStream);

    DWORD dwSave = (DWORD)m_iStyle;
    pStream->Write(dwSave);

    DWORD dwFlags = 0;
    dwFlags |= (m_pMapOrbit ? 0x00000001 : 0);

    pStream->Write(dwFlags);

	pStream->Write(m_dwCreatedOn);
	pStream->Write(m_dwDestroyOn);

    //  Write map orbit, if we have one

    if (m_pMapOrbit)
        m_pMapOrbit->WriteToStream(*pStream);
	}

void CMarker::PaintMessage (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx) const

//	PaintMessage
//
//	Paint a message marker

	{
	constexpr DWORD FADE_OUT_TIME = 30;
	constexpr DWORD FADE_IN_TIME = 30;
	constexpr int MESSAGE_WIDTH = 600;
	const CG16bitFont &MessageFont = GetUniverse().GetNamedFont(CUniverse::fontSRSMessage);

	DWORD dwFontFlags = CG16bitFont::AlignCenter;

	RECT rcRect;
	rcRect.left = x - MESSAGE_WIDTH / 2;
	rcRect.right = x + MESSAGE_WIDTH / 2;
	rcRect.top = y;
	rcRect.bottom = y + 1000;

	CG32bitPixel rgbColor = GetSymbolColor();
	if (m_dwDestroyOn)
		{
		DWORD dwTimeAlive = GetUniverse().GetTicks() - m_dwCreatedOn;
		DWORD dwTimeLeft = m_dwDestroyOn - GetUniverse().GetTicks();

		if (dwTimeLeft < FADE_OUT_TIME)
			{
			BYTE byAlpha = (BYTE)(255 * dwTimeLeft / FADE_OUT_TIME);
			rgbColor = CG32bitPixel(rgbColor, byAlpha);
			}
		else if (dwTimeAlive < FADE_IN_TIME)
			{
			rgbColor = CG32bitPixel::Blend(rgbColor, 0xffff, (BYTE)(255 * (FADE_IN_TIME - dwTimeAlive) / FADE_IN_TIME));
			}
		}

	MessageFont.DrawText(Dest,
			rcRect,
			rgbColor,
			m_sName,
			0,
			dwFontFlags);
	}

CMarker::EStyle CMarker::ParseStyle (const CString &sValue)
	{
	if (sValue.IsBlank())
		return EStyle::None;

	auto pEntry = STYLE_TABLE.GetAt(sValue);
	if (!pEntry)
		return EStyle::Error;

	return pEntry->Value;
	}

void CMarker::SetOrbit (const COrbit &Orbit)

//  SetOrbit
//
//  Sets the orbit to paint on the map

    {
    if (m_pMapOrbit)
        delete m_pMapOrbit;

    m_pMapOrbit = new COrbit(Orbit);
    }

bool CMarker::SetProperty (const CString &sName, ICCItem *pValue, CString *retsError)

//	SetProperty
//
//	Sets an object property

	{
	CCodeChain &CC = GetUniverse().GetCC();

	if (strEquals(sName, PROPERTY_STYLE))
		{
        CString sStyle = pValue->GetStringValue();
        if (sStyle.IsBlank())
            m_iStyle = EStyle::None;
        else if (strEquals(sStyle, STYLE_SMALL_CROSS))
            m_iStyle = EStyle::SmallCross;
        else
            {
			*retsError = strPatternSubst(CONSTLIT("Invalid style: %s"), sStyle);
			return false;
            }

		return true;
		}
	else
		return CSpaceObject::SetProperty(sName, pValue, retsError);
	}

//	CPOVMarker ----------------------------------------------------------------

CPOVMarker::CPOVMarker (CUniverse &Universe) : TSpaceObjectImpl(Universe)

//	CPOVMarker constructor

	{
	}

ALERROR CPOVMarker::Create (CSystem &System,
							const CVector &vPos,
							const CVector &vVel,
							CPOVMarker **retpMarker)

//	Create
//
//	Create a marker

	{
	ALERROR error;
	CPOVMarker *pMarker;

	pMarker = new CPOVMarker(System.GetUniverse());
	if (pMarker == NULL)
		return ERR_MEMORY;

	pMarker->Place(vPos, vVel);
	pMarker->DisableObjectDestructionNotify();
	pMarker->SetCannotBeHit();

	//	Add to system

	if (error = pMarker->AddToSystem(System))
		{
		delete pMarker;
		return error;
		}

	//	Done

	if (retpMarker)
		*retpMarker = pMarker;

	return NOERROR;
	}

CSovereign *CPOVMarker::GetSovereign (void) const
	{
	//	Return player

	return GetUniverse().FindSovereign(g_PlayerSovereignUNID);
	}

void CPOVMarker::OnLosePOV (void)

//	OnLosePOV
//
//	Marker is no longer part of POV

	{
	Destroy(removedFromSystem, CDamageSource());
	}

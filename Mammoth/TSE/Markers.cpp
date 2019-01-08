//	Markers.cpp
//
//	Marker classes

#include "PreComp.h"

#define PROPERTY_STYLE  						CONSTLIT("style")

#define STYLE_SMALL_CROSS  						CONSTLIT("smallCross")

const CG32bitPixel RGB_ORBIT_LINE =		        CG32bitPixel(115, 149, 229);

//	CMarker -------------------------------------------------------------------

static CObjectClass<CMarker>g_MarkerClass(OBJID_CMARKER, NULL);

CMarker::CMarker (void) : CSpaceObject(&g_MarkerClass),
        m_pMapOrbit(NULL)

//	CMarker constructor

	{
	}

CMarker::~CMarker (void)

//  CMarker destructor

    {
    if (m_pMapOrbit)
        delete m_pMapOrbit;
    }

ALERROR CMarker::Create (CSystem *pSystem,
						 CSovereign *pSovereign,
						 const CVector &vPos,
						 const CVector &vVel,
						 const CString &sName,
						 CMarker **retpMarker)

//	Create
//
//	Create a marker

	{
	ALERROR error;
	CMarker *pMarker;

	pMarker = new CMarker;
	if (pMarker == NULL)
		return ERR_MEMORY;

	pMarker->Place(vPos, vVel);
	pMarker->SetCannotBeHit();

	//	Note: Cannot disable destruction notification because some markers
	//	are used as destination targets for ships

	pMarker->m_sName = sName;
	pMarker->m_pSovereign = pSovereign;
	if (pMarker->m_pSovereign == NULL)
		pMarker->m_pSovereign = g_pUniverse->FindSovereign(g_PlayerSovereignUNID);

    //  Basic properties

    pMarker->m_iStyle = styleNone;

	//	Add to system

	if (error = pMarker->AddToSystem(pSystem))
		{
		delete pMarker;
		return error;
		}

	//	Done

	if (retpMarker)
		*retpMarker = pMarker;

	return NOERROR;
	}

ICCItem *CMarker::GetProperty (CCodeChainCtx &Ctx, const CString &sName)

//	GetProperty
//
//	Returns a property

	{
	CCodeChain &CC = g_pUniverse->GetCC();

    if (strEquals(sName, PROPERTY_STYLE))
        {
        switch (m_iStyle)
            {
            case styleSmallCross:
                return CC.CreateString(STYLE_SMALL_CROSS);

            default:
                return CC.CreateNil();
            }
        }

	else
		return CSpaceObject::GetProperty(Ctx, sName);
	}

CSovereign *CMarker::GetSovereign (void) const
	{
	//	Return player

	return m_pSovereign;
	}

void CMarker::OnObjLeaveGate (CSpaceObject *pObj)

//	OnObjLeaveGate
//
//	Object leaves a gate

	{
	CEffectCreator *pEffect = g_pUniverse->FindEffectType(g_StargateOutUNID);
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
        case styleSmallCross:
            Dest.DrawDot(x, y, CG32bitPixel(255, 255, 0), markerSmallCross);
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
        Ctx.pStream->Read((char *)&dwLoad, sizeof(DWORD));
        m_iStyle = (EStyles)dwLoad;
        }
    else
        m_iStyle = styleNone;

    DWORD dwFlags = 0;
    if (Ctx.dwVersion >= 134)
        Ctx.pStream->Read((char *)&dwFlags, sizeof(DWORD));

    bool bHasMapOrbit = ((dwFlags & 0x00000001) ? true : false);

    //  Read map orbit, if we have it.

    if (bHasMapOrbit)
        {
        m_pMapOrbit = new COrbit;
        Ctx.pStream->Read((char *)m_pMapOrbit, sizeof(COrbit));
        }
    else
        m_pMapOrbit = NULL;
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
//  COrbit      (optional)

	{
	m_sName.WriteToStream(pStream);
	GetSystem()->WriteSovereignRefToStream(m_pSovereign, pStream);

    DWORD dwSave = (DWORD)m_iStyle;
    pStream->Write((char *)&dwSave, sizeof(DWORD));

    DWORD dwFlags = 0;
    dwFlags |= (m_pMapOrbit ? 0x00000001 : 0);

    pStream->Write((char *)&dwFlags, sizeof(DWORD));

    //  Write map orbit, if we have one

    if (m_pMapOrbit)
        pStream->Write((char *)m_pMapOrbit, sizeof(COrbit));
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
	CCodeChain &CC = g_pUniverse->GetCC();

	if (strEquals(sName, PROPERTY_STYLE))
		{
        CString sStyle = pValue->GetStringValue();
        if (sStyle.IsBlank())
            m_iStyle = styleNone;
        else if (strEquals(sStyle, STYLE_SMALL_CROSS))
            m_iStyle = styleSmallCross;
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

static CObjectClass<CPOVMarker>g_POVMarkerClass(OBJID_CPOVMARKER, NULL);

CPOVMarker::CPOVMarker (void) : CSpaceObject(&g_POVMarkerClass)

//	CPOVMarker constructor

	{
	}

ALERROR CPOVMarker::Create (CSystem *pSystem,
							const CVector &vPos,
							const CVector &vVel,
							CPOVMarker **retpMarker)

//	Create
//
//	Create a marker

	{
	ALERROR error;
	CPOVMarker *pMarker;

	pMarker = new CPOVMarker;
	if (pMarker == NULL)
		return ERR_MEMORY;

	pMarker->Place(vPos, vVel);
	pMarker->DisableObjectDestructionNotify();
	pMarker->SetCannotBeHit();

	//	Add to system

	if (error = pMarker->AddToSystem(pSystem))
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

	return g_pUniverse->FindSovereign(g_PlayerSovereignUNID);
	}

void CPOVMarker::OnLosePOV (void)

//	OnLosePOV
//
//	Marker is no longer part of POV

	{
	Destroy(removedFromSystem, CDamageSource());
	}

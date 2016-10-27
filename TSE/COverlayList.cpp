//	COverlayList.cpp
//
//	COverlayList class

#include "PreComp.h"

COverlayList::COverlayList (void) :
		m_pFirst(NULL)

//	COverlayList constructor

	{
	}

COverlayList::~COverlayList (void)

//	COverlayList destructor

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		COverlay *pDelete = pField;
		pField = pField->GetNext();
		delete pDelete;
		}
	}

bool COverlayList::AbsorbDamage (CSpaceObject *pSource, SDamageCtx &Ctx)

//	AbsorbDamage
//
//	Absorbs damage (returns TRUE if damage was absorbed)

	{
	DEBUG_TRY

	bool bDamageAbsorbed = false;

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed()
				&& pField->IsShieldOverlay())
			{
			if (pField->AbsorbDamage(pSource, Ctx))
				bDamageAbsorbed = true;
			}

		pField = pField->GetNext();
		}

	return bDamageAbsorbed;

	DEBUG_CATCH
	}

bool COverlayList::AbsorbsWeaponFire (CInstalledDevice *pDevice)

//	AbsorbWeaponFire
//
//	Returns TRUE if the field prevents the ship from firing the given weapon

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			{
			if (pField->GetType()->AbsorbsWeaponFire(pDevice))
				return true;
			}

		pField = pField->GetNext();
		}

	return false;
	}

void COverlayList::AccumulateBounds (CSpaceObject *pSource, RECT *ioBounds)

//	AccumulateBounds
//
//	Adds to bounds

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			pField->AccumulateBounds(pSource, ioBounds);

		pField = pField->GetNext();
		}
	}

bool COverlayList::Damage (CSpaceObject *pSource, SDamageCtx &Ctx)

//	Damage
//
//	Takes damage (returns TRUE if damage was absorbed)

	{
	DEBUG_TRY

	bool bDamageAbsorbed = false;

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed()
				&& !pField->IsShieldOverlay())
			{
			if (pField->AbsorbDamage(pSource, Ctx))
				bDamageAbsorbed = true;
			}

		pField = pField->GetNext();
		}

	return bDamageAbsorbed;

	DEBUG_CATCH
	}

CString COverlayList::DebugCrashInfo (void) const

//	DebugCrashInfo
//
//	Outputs overlay info

	{
	try
		{
		CString sResult;

		COverlay *pField = m_pFirst;
		while (pField)
			{
			sResult.Append(strPatternSubst(CONSTLIT("overlay [%d]: type = %08x\n"), pField->GetID(), pField->GetType()->GetUNID()));

			pField = pField->GetNext();
			}

		return sResult;
		}
	catch (...)
		{
		return CONSTLIT("Crash obtaining overlay information.\n");
		}
	}

int COverlayList::GetCountOfType (COverlayType *pType)

//	GetCountOfType
//
//	Returns the number of existing overlays of the given type.

	{
	int iCount = 0;
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed()
				&& pField->GetType() == pType)
			iCount++;

		pField = pField->GetNext();
		}

	return iCount;
	}

void COverlayList::GetImpact (CSpaceObject *pSource, SImpactDesc *retImpact) const

//	GetImpact
//
//	Returns the impact of this set of overlays on the source.

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			{
			//	Do we disarm the source?

			if (pField->Disarms(pSource))
				retImpact->bDisarm = true;

			//	Do we paralyze the source?

			if (pField->Paralyzes(pSource))
				retImpact->bParalyze = true;

			//	Can't bring up ship status

			if (pField->IsShipScreenDisabled())
				retImpact->bShipScreenDisabled = true;

			//	Do we spin the source ?

			if (pField->Spins(pSource))
				retImpact->bSpin = true;

			//	Get appy drag

			Metric rDrag;
			if ((rDrag = pField->GetDrag(pSource)) < 1.0)
				retImpact->rDrag *= rDrag;
			}

		//	Next

		pField = pField->GetNext();
		}
	}

void COverlayList::AddField (CSpaceObject *pSource, 
								 COverlayType *pType,
								 int iPosAngle,
								 int iPosRadius,
								 int iRotation,
								 int iLifeLeft, 
								 DWORD *retdwID)

//	AddField
//
//	Adds a field of the given type to the head of the list

	{
	COverlay *pField;
	COverlay::CreateFromType(pType, 
			iPosAngle, 
			iPosRadius, 
			iRotation, 
			iLifeLeft, 
			&pField);

	//	Add the field to the head of the list

	pField->SetNext(m_pFirst);
	m_pFirst = pField;

	//	Call OnCreate

	pField->FireOnCreate(pSource);

	//	If we deleted, then we're done

	if (pField->IsDestroyed())
		{
		COverlay *pNext = pField->GetNext();
		m_pFirst = pNext;
		delete pField;

		if (retdwID)
			*retdwID = 0;
		return;
		}

	//	Done

	if (retdwID)
		*retdwID = pField->GetID();
	}

void COverlayList::FireOnObjDestroyed (CSpaceObject *pSource, const SDestroyCtx &Ctx) const

//	FireOnObjDestroyed
//
//	Calls OnObjDestroyed on all overlays

	{
	DEBUG_TRY

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			pField->FireOnObjDestroyed(pSource, Ctx);

		pField = pField->GetNext();
		}

	DEBUG_CATCH
	}

const CString &COverlayList::GetData (DWORD dwID, const CString &sAttrib)

//	GetData
//
//	Returns opaque data for the given energy field

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			{
			if (pField->GetID() == dwID)
				return pField->GetData(sAttrib);
			}

		pField = pField->GetNext();
		}

	return NULL_STR;
	}

void COverlayList::GetList (TArray<COverlay *> *retList)

//	GetList
//
//	Returns all the fields in an array

	{
	retList->DeleteAll();

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			retList->Insert(pField);

		pField = pField->GetNext();
		}
	}

void COverlayList::GetListOfCommandPaneCounters (TArray<COverlay *> *retList)

//	GetListOfCommandPaneCounters
//
//	Returns the list of overlays that should be displayed as counters on the
//	command pane.

	{
	retList->DeleteAll();

	COverlay *pField = m_pFirst;
	while (pField)
		{
		COverlayType *pType;

		if (!pField->IsDestroyed()
				&& (pType = pField->GetType())
				&& pType->GetCounterStyle() == COverlayType::counterCommandBarProgress)
			retList->Insert(pField);

		pField = pField->GetNext();
		}
	}

COverlay *COverlayList::GetOverlay (DWORD dwID) const

//	GetOverlay
//
//	Returns the overlay with the given ID (or NULL)

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField;

		pField = pField->GetNext();
		}

	return NULL;
	}

CVector COverlayList::GetPos (CSpaceObject *pSource, DWORD dwID)

//	GetPos
//
//	Returns the current (absolute) position of the overlay

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{

		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->GetPos(pSource);

		pField = pField->GetNext();
		}

	return CVector();
	}

ICCItem *COverlayList::GetProperty (CCodeChainCtx *pCCCtx, CSpaceObject *pSource, DWORD dwID, const CString &sName)

//	GetProperty
//
//	Returns the property

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{

		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->GetProperty(pCCCtx, pSource, sName);

		pField = pField->GetNext();
		}

	return g_pUniverse->GetCC().CreateNil();
	}

int COverlayList::GetRotation (DWORD dwID)

//	GetRotation
//
//	Returns the rotation of the given overlay

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->GetRotation();

		pField = pField->GetNext();
		}

	return -1;
	}

COverlayType *COverlayList::GetType (DWORD dwID)

//	GetType
//
//	Returns the type of overlay

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->GetType();

		pField = pField->GetNext();
		}

	return NULL;
	}

int COverlayList::GetWeaponBonus (CInstalledDevice *pDevice, CSpaceObject *pSource)

//	GetWeaponBonus
//
//	Returns the weapon bonus confered by the fields

	{
	int iBonus = 0;

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			iBonus += pField->GetType()->GetWeaponBonus(pDevice, pSource);

		pField = pField->GetNext();
		}

	return iBonus;
	}

void COverlayList::IncData (DWORD dwID, const CString &sAttrib, ICCItem *pValue, ICCItem **retpNewValue)

//  IncData
//
//  Increment data

    {
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->IncData(sAttrib, pValue, retpNewValue);

		pField = pField->GetNext();
		}

    //  If we get this far, then we couldn't find it, so we just return nil

    if (retpNewValue)
        *retpNewValue = g_pUniverse->GetCC().CreateNil();
    }

void COverlayList::Paint (CG32bitImage &Dest, int iScale, int x, int y, SViewportPaintCtx &Ctx)

//	Paint
//
//	Paints all fields

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed() || pField->IsFading())
			pField->Paint(Dest, iScale, x, y, Ctx);

		pField = pField->GetNext();
		}
	}

void COverlayList::PaintAnnotations (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintAnnotations
//
//	Paints all field annotations

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			pField->PaintAnnotations(Dest, x, y, Ctx);

		pField = pField->GetNext();
		}
	}

void COverlayList::PaintBackground (CG32bitImage &Dest, int x, int y, SViewportPaintCtx &Ctx)

//	PaintBackground
//
//	Paints all field backgrounds

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			pField->PaintBackground(Dest, x, y, Ctx);

		pField = pField->GetNext();
		}
	}

void COverlayList::PaintLRSAnnotations (const ViewportTransform &Trans, CG32bitImage &Dest, int x, int y)

//	PaintLRSAnnotations
//
//	Paints annotations on the LRS

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed() && pField->GetType()->IsShownOnMap())
			pField->PaintLRSAnnotations(Trans, Dest, x, y);

		pField = pField->GetNext();
		}
	}

void COverlayList::PaintMapAnnotations (CMapViewportCtx &Ctx, CG32bitImage &Dest, int x, int y)

//	PaintMapAnnotations
//
//	Paints annotations on the system map

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed() && pField->GetType()->IsShownOnMap())
			pField->PaintMapAnnotations(Ctx, Dest, x, y);

		pField = pField->GetNext();
		}
	}

void COverlayList::ReadFromStream (SLoadCtx &Ctx, CSpaceObject *pSource)

//	ReadFromStream
//
//	DWORD		no of fields
//	COverlay array

	{
	DWORD dwCount;

	Ctx.pStream->Read((char *)&dwCount, sizeof(DWORD));
	COverlay *pPrevField = NULL;
	while (dwCount)
		{
		//	Allocate the overlay and connect it to
		//	the linked list.

		COverlay *pField = new COverlay;
		if (pPrevField)
			pPrevField->SetNext(pField);
		else
			m_pFirst = pField;

		//	Read

		pField->ReadFromStream(Ctx);

		//	If this field is associated with a device, make
		//	the association now (we assume that in all objects
		//	the devices are loaded first).

		int iDevice;
		if ((iDevice = pField->GetDevice()) != -1)
			{
			if (iDevice < pSource->GetDeviceCount())
				pSource->GetDevice(iDevice)->SetOverlay(pField);
			else
				pField->SetDevice(-1);
			}

		//	Next

		pPrevField = pField;
		dwCount--;
		}
	}

void COverlayList::RemoveField (CSpaceObject *pSource, DWORD dwID)

//	RemoveField
//
//	Removes the field by ID

	{
	//	Mark the field as destroyed. The field will be deleted in Update

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID)
			{
			pField->Destroy(pSource);
			return;
			}

		pField = pField->GetNext();
		}
	}

void COverlayList::SetData (DWORD dwID, const CString &sAttrib, const CString &sData)

//	SetData
//
//	Sets opaque data

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->SetData(sAttrib, sData);

		pField = pField->GetNext();
		}
	}

bool COverlayList::SetEffectProperty (DWORD dwID, const CString &sProperty, ICCItem *pValue)

//	SetEffectProperty
//
//	Sets a property on the effect

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->SetEffectProperty(sProperty, pValue);

		pField = pField->GetNext();
		}

	return false;
	}

void COverlayList::SetPos (CSpaceObject *pSource, DWORD dwID, const CVector &vPos)

//	SetPosition
//
//	Sets the position of the overlay

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->SetPos(pSource, vPos);

		pField = pField->GetNext();
		}
	}

bool COverlayList::SetProperty (CSpaceObject *pSource, DWORD dwID, const CString &sName, ICCItem *pValue)

//	SetProperty
//
//	Sets a property.

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->SetProperty(pSource, sName, pValue);

		pField = pField->GetNext();
		}

	return false;
	}

void COverlayList::SetRotation (DWORD dwID, int iRotation)

//	SetRotation
//
//	Sets the rotation of an overlay

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->SetRotation(iRotation);

		pField = pField->GetNext();
		}
	}

void COverlayList::Update (CSpaceObject *pSource, bool *retbModified)

//	Update
//
//	Update fields. Returns bModified = TRUE if any field changed such that the object
//	need to be recalculated.

	{
	DEBUG_TRY

	//	First update all fields

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed()
				|| pField->IsFading())
			{
			pField->Update(pSource);

			//	If the source got destroyed, then we're done

			if (CSpaceObject::IsDestroyedInUpdate())
				return;
			}

		pField = pField->GetNext();
		}

	//	Do a second pass in which we destroy fields marked for deletion

	bool bModified = false;

	pField = m_pFirst;
	COverlay *pPrevField = NULL;
	while (pField)
		{
		COverlay *pNext = pField->GetNext();

		//	If this overlay was destroyed, handle that now

		if (pField->IsDestroyed()
				&& !pField->IsFading())
			{
			delete pField;

			if (pPrevField)
				pPrevField->SetNext(pNext);
			else
				m_pFirst = pNext;

			bModified = true;
			}
		else
			pPrevField = pField;

		//	Next energy field

		pField = pNext;
		}

	*retbModified = bModified;

	DEBUG_CATCH
	}

void COverlayList::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//	ReadFromStream
//
//	DWORD		no of fields
//	COverlay array

	{
	//	NOTE: We have to saved destroyed overlays because we need to run some 
	//	code when removing an overlay (e.g., see CShip::CalcOverlayImpact).

	DWORD dwSave = 0;
	COverlay *pField = m_pFirst;
	while (pField)
		{
		dwSave++;
		pField = pField->GetNext();
		}

	pStream->Write((char *)&dwSave, sizeof(DWORD));
	pField = m_pFirst;
	while (pField)
		{
		pField->WriteToStream(pStream);
		pField = pField->GetNext();
		}
	}

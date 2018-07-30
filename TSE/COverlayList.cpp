//	COverlayList.cpp
//
//	COverlayList class

#include "PreComp.h"

#define EVENT_ON_REMOVE_AT_DOCK_SERVICES		CONSTLIT("OnRemoveAtDockServices")

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

void COverlayList::AccumulateBounds (CSpaceObject *pSource, int iScale, int iRotation, RECT *ioBounds)

//	AccumulateBounds
//
//	Adds to bounds

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			pField->AccumulateBounds(pSource, iScale, iRotation, ioBounds);

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

	DWORD dwID = pField->GetID();

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
		pField = NULL;

		dwID = 0;
		}

	//	NOTE: After this point we cannot guarantee that pField is valid, since
	//	it may have been destroyed.

	//	Either way, we need to recalculate impact

	OnConditionsChanged(pSource);

	//	Done

	if (retdwID)
		*retdwID = dwID;
	}

CConditionSet COverlayList::CalcConditions (CSpaceObject *pSource) const

//	CalcConditions
//
//	Computes the conditions that we impart.

	{
	CConditionSet AllConditions;

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			{
			AllConditions.Set(pField->GetConditions(pSource));
			}

		//	Next

		pField = pField->GetNext();
		}

	return AllConditions;
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

bool COverlayList::DestroyDeleted (void)

//	DestroyDeleted
//
//	Destroy any deleted/expired overlays. Returns TRUE if any where deleted.

	{
	bool bModified = false;

	COverlay *pField = m_pFirst;
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

	return bModified;
	}

bool COverlayList::FireGetDockScreen (CSpaceObject *pSource, CString *retsScreen, int *retiPriority, ICCItemPtr *retpData) const

//	FireGetDockScreen
//
//	Fires <GetDockScreen> event for overlay. If we return TRUE, the caller must
//	discard retpData.

	{
	CCodeChain &CC = g_pUniverse->GetCC();

	CString sBestScreen;
	int iBestPriority = -1;
	ICCItemPtr pBestData;

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			{
			CString sScreen;
			int iPriority;
			ICCItemPtr pData;

			if (pField->FireGetDockScreen(pSource, &sScreen, &iPriority, &pData))
				{
				if (iPriority > iBestPriority)
					{
					sBestScreen = sScreen;
					iBestPriority = iPriority;
					pBestData = pData;
					}
				}
			}

		pField = pField->GetNext();
		}

	if (iBestPriority == -1)
		return false;

	if (retsScreen)
		*retsScreen = sBestScreen;

	if (retiPriority)
		*retiPriority = iBestPriority;

	if (retpData)
		*retpData = pBestData;

	return true;
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

bool COverlayList::GetImpact (CSpaceObject *pSource, COverlay::SImpactDesc &Impact) const

//	GetImpact
//
//	Returns the impact of this set of overlays on the source.

	{
	DEBUG_TRY

	bool bHasImpact = false;
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			{
			COverlay::SImpactDesc FieldImpact;
			if (pField->GetImpact(pSource, FieldImpact))
				{
				Impact.Conditions.Set(FieldImpact.Conditions);
				Impact.rDrag *= FieldImpact.rDrag;

				bHasImpact = true;
				}
			}

		//	Next

		pField = pField->GetNext();
		}

	return bHasImpact;
	DEBUG_CATCH
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

void COverlayList::FireOnObjDocked (CSpaceObject *pSource, CSpaceObject *pShip) const

//	FireOnObjDocked
//
//	Calls OnObjDocked on all overlays

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			pField->FireOnObjDocked(pSource, pShip);

		pField = pField->GetNext();
		}
	}

ICCItemPtr COverlayList::GetData (DWORD dwID, const CString &sAttrib) const

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

	return ICCItemPtr(g_pUniverse->GetCC().CreateNil());
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

ICCItemPtr COverlayList::IncData (DWORD dwID, const CString &sAttrib, ICCItem *pValue)

//  IncData
//
//  Increment data

    {
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->IncData(sAttrib, pValue);

		pField = pField->GetNext();
		}

    //  If we get this far, then we couldn't find it, so we just return nil

	return ICCItemPtr(g_pUniverse->GetCC().CreateNil());
    }

void COverlayList::OnConditionsChanged (CSpaceObject *pSource)

//	OnConditionsChanged
//
//	Imparted conditions may have changed, so notify source.

	{
	int i;

	CConditionSet OldConditions = m_Conditions;
	m_Conditions = CalcConditions(pSource);

	//	See what changed.

	TArray<CConditionSet::ETypes> Added;
	TArray<CConditionSet::ETypes> Removed;

	if (m_Conditions.Diff(OldConditions, Added, Removed))
		{
		for (i = 0; i < Added.GetCount(); i++)
			pSource->OnOverlayConditionChanged(Added[i], CConditionSet::cndAdded);

		for (i = 0; i < Removed.GetCount(); i++)
			pSource->OnOverlayConditionChanged(Removed[i], CConditionSet::cndRemoved);
		}
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

	//	Recalc conditions

	if (m_pFirst)
		m_Conditions = CalcConditions(pSource);
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

			//	Recalc conditions

			OnConditionsChanged(pSource);
			return;
			}

		pField = pField->GetNext();
		}
	}

void COverlayList::ScrapeHarmfulOverlays (CSpaceObject *pSource, int iMaxRemoved)

//	ScrapeHarmfulOverlays
//
//	Remove any harmful overlays

	{
	CCodeChain &CC = g_pUniverse->GetCC();
	int iRemoved = 0;

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed()
				&& pField->GetType()->FindEventHandler(EVENT_ON_REMOVE_AT_DOCK_SERVICES))
			{
			//	Invoke the event to remove it.

			pField->FireCustomEvent(pSource, EVENT_ON_REMOVE_AT_DOCK_SERVICES, NULL, NULL);

			//	We assume the event removes the object.

			iRemoved++;
			if (iRemoved >= iMaxRemoved)
				return;
			}

		pField = pField->GetNext();
		}
	}

void COverlayList::SetData (DWORD dwID, const CString &sAttrib, ICCItem *pData)

//	SetData
//
//	Sets opaque data

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			return pField->SetData(sAttrib, pData);

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

void COverlayList::Update (CSpaceObject *pSource, int iScale, int iRotation, bool *retbModified)

//	Update
//
//	Update fields. Returns bModified = TRUE if any field changed such that the object
//	need to be recalculated.

	{
	DEBUG_TRY

	bool bModified = false;
	bool bDestroyed = false;

	//	First update all fields

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed()
				|| pField->IsFading())
			{
			bool bIsDestroyed = pField->IsDestroyed();
			bool bOverlayModified;

			pField->Update(pSource, iScale, iRotation, &bOverlayModified);
			if (!bIsDestroyed && pField->IsDestroyed())
				{
				bDestroyed = true;
				bModified = true;
				}
			else if (bOverlayModified)
				bModified = true;

			//	If the source got destroyed, then we're done

			if (CSpaceObject::IsDestroyedInUpdate())
				return;
			}

		pField = pField->GetNext();
		}

	//	Recalc conditions

	if (bDestroyed)
		OnConditionsChanged(pSource);

	//	Do a second pass in which we destroy fields marked for deletion

	if (DestroyDeleted())
		bModified = true;

	if (retbModified) *retbModified = bModified;

	DEBUG_CATCH
	}

void COverlayList::UpdateTimeStopped (CSpaceObject *pSource, int iScale, int iRotation, bool *retbModified)

//	UpdateTimeStopped
//
//	Update fields. Returns bModified = TRUE if any field changed such that the object
//	need to be recalculated.

	{
	DEBUG_TRY

	bool bModified = false;
	bool bDestroyed = false;

	//	First update all fields

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetType()->StopsTime()
				&& (!pField->IsDestroyed()
					|| pField->IsFading()))
			{
			bool bIsDestroyed = pField->IsDestroyed();
			bool bOverlayModified;

			pField->Update(pSource, iScale, iRotation, &bOverlayModified);
			if (!bIsDestroyed && pField->IsDestroyed())
				{
				bDestroyed = true;
				bModified = true;
				}
			else if (bOverlayModified)
				bModified = true;

			//	If the source got destroyed, then we're done

			if (CSpaceObject::IsDestroyedInUpdate())
				return;
			}

		pField = pField->GetNext();
		}

	//	Recalc conditions

	if (bDestroyed)
		OnConditionsChanged(pSource);

	//	Do a second pass in which we destroy fields marked for deletion

	if (DestroyDeleted())
		bModified = true;

	if (retbModified) *retbModified = bModified;

	DEBUG_CATCH
	}

void COverlayList::WriteToStream (IWriteStream *pStream)

//	WriteToStream
//	ReadFromStream
//
//	DWORD		no of fields
//	COverlay array

	{
	//	NOTE: We have to save destroyed overlays because we need to run some 
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

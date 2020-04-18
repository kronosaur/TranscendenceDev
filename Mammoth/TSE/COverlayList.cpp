//	COverlayList.cpp
//
//	COverlayList class

#include "PreComp.h"

#define EVENT_ON_REMOVE_AT_DOCK_SERVICES		CONSTLIT("OnRemoveAtDockServices")

COverlayList::COverlayList (void)

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

bool COverlayList::AccumulateEnhancements (CSpaceObject &Source, CDeviceItem &Device, TArray<CString> &EnhancementIDs, CItemEnhancementStack &Enhancements)

//	AccumulateEnhancements
//
//	Adds enhancements to the given device to the enhancement stack. Returns TRUE
//	if any enhancements were added.

	{
	bool bAdded = false;

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			{
			if (pField->AccumulateEnhancements(Source, Device, EnhancementIDs, Enhancements))
				bAdded = true;
			}

		pField = pField->GetNext();
		}

	return bAdded;
	}

void COverlayList::AddField (CSpaceObject &Source, 
							 COverlayType &Type,
							 int iPosAngle,
							 int iPosRadius,
							 int iRotation,
							 int iPosZ,
							 int iLifeLeft, 
							 DWORD *retdwID)

//	AddField
//
//	Adds a field of the given type to the head of the list

	{
	COverlay *pField;
	COverlay::CreateFromType(Type, 
			Source,
			iPosAngle, 
			iPosRadius, 
			iRotation,
			iPosZ,
			iLifeLeft, 
			&pField);

	DWORD dwID = pField->GetID();

	//	Add the field to the head of the list

	pField->SetNext(m_pFirst);
	m_pFirst = pField;

	//	Call OnCreate

	pField->FireOnCreate(&Source);

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

	OnConditionsChanged(&Source);

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

COverlay *COverlayList::FindField (DWORD dwID)

//	FindField
//
//	Returns a field with the given ID (or NULL).

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID)
			{
			if (pField->IsDestroyed())
				return NULL;
			else
				return pField;
			}

		pField = pField->GetNext();
		}

	return NULL;
	}

bool COverlayList::FireGetDockScreen (const CSpaceObject *pSource, CDockScreenSys::SSelector *retSelector, CDesignType **retpLocalScreens) const

//	FireGetDockScreen
//
//	Fires <GetDockScreen> event for overlay. If we return TRUE, the caller must
//	discard retpData.

	{
	CDesignType *pBestLocalScreens = NULL;
	CDockScreenSys::SSelector BestScreen;
	BestScreen.iPriority = -1;

	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			{
			CDockScreenSys::SSelector OverlayScreen;
			if (pField->FireGetDockScreen(pSource, OverlayScreen))
				{
				//	Short-circuit if all we care about is whether we have at 
				//	least one screen.

				if (retSelector == NULL)
					return true;

				//	See if this is better than previous.

				else if (OverlayScreen.iPriority > BestScreen.iPriority)
					{
					BestScreen = OverlayScreen;
					pBestLocalScreens = pField->GetType();
					}
				}
			}

		pField = pField->GetNext();
		}

	if (BestScreen.iPriority == -1)
		return false;

	if (retSelector)
		*retSelector = BestScreen;

	if (retpLocalScreens)
		*retpLocalScreens = pBestLocalScreens;

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

	return ICCItemPtr(ICCItem::Nil);
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
				&& pType->GetCounterDesc().GetType() == COverlayCounterDesc::counterCommandBarProgress)
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

ICCItem *COverlayList::GetProperty (CCodeChainCtx *pCCCtx, CSpaceObject *pSource, DWORD dwID, const CString &sName) const

//	GetProperty
//
//	Returns the property

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (pField->GetID() == dwID && !pField->IsDestroyed())
			{
			ICCItemPtr pResult = pField->GetProperty(*pCCCtx, *pSource, sName);
			return pResult->Reference();
			}

		pField = pField->GetNext();
		}

	return CCodeChain::CreateNil();
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

bool COverlayList::HasMinableItem (void) const

//	HasMinableItem
//
//	Returns TRUE if we have an overlay that can be mined. This is generally
//	used to determine mining targets.

	{
	const COverlay *pOverlay = m_pFirst;
	while (pOverlay)
		{
		//	Not active means that it is still buried and that mining will help
		//	to uncover it.

		if (!pOverlay->IsActive() 
				&& pOverlay->GetType()->IsUnderground())
			return true;

		pOverlay = pOverlay->GetNext();
		}

	return false;
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

	return ICCItemPtr(ICCItem::Nil);
	}

bool COverlayList::IncProperty (CSpaceObject &SourceObj, DWORD dwID, const CString &sProperty, ICCItem *pInc, ICCItemPtr &pResult)

//	IncProperty
//
//	Increments a property.

	{
	COverlay *pField = FindField(dwID);
	if (pField == NULL)
		return false;

	return pField->IncProperty(SourceObj, sProperty, pInc, pResult);
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

bool COverlayList::OnMiningDamage (CSpaceObject &Source, EAsteroidType iType, SDamageCtx &Ctx)

//	OnMiningDamage
//
//	Handle mining damage. Returns TRUE if the overlays handled it.

	{
	COverlay *pField = m_pFirst;
	while (pField)
		{
		if (!pField->IsDestroyed())
			{
			if (pField->OnMiningDamage(Source, iType, Ctx))
				//	We only handle one underground vault
				return true;
			}

		pField = pField->GetNext();
		}

	return false;
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

	Ctx.pStream->Read(dwCount);
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
			return pField->SetProperty(*pSource, sName, pValue);

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

			if (pSource->IsDestroyed())
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

			if (pSource->IsDestroyed())
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

	pStream->Write(dwSave);
	pField = m_pFirst;
	while (pField)
		{
		pField->WriteToStream(pStream);
		pField = pField->GetNext();
		}
	}

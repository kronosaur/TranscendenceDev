//	CCodeChainCtx.cpp
//
//	CCodeChainCtx object

#include "PreComp.h"

#define FIELD_ANGLE								CONSTLIT("angle")
#define FIELD_MAX								CONSTLIT("max")
#define FIELD_MIN								CONSTLIT("min")
#define FIELD_OMNIDIRECTIONAL					CONSTLIT("omnidirectional")
#define FIELD_RADIUS							CONSTLIT("radius")
#define FIELD_Z									CONSTLIT("z")

#define STR_A_OVERLAY_ID						CONSTLIT("aOverlayID")
#define STR_G_DATA								CONSTLIT("gData")
#define STR_G_ITEM								CONSTLIT("gItem")
#define STR_G_SOURCE							CONSTLIT("gSource")
#define STR_G_TYPE								CONSTLIT("gType")

TArray<CCodeChainCtx::SInvokeFrame> CCodeChainCtx::g_Invocations;

CCodeChainCtx::CCodeChainCtx (void) :
		m_CC(g_pUniverse->GetCC()),
		m_iEvent(eventNone),
		m_pScreen(NULL),
		m_pCanvas(NULL),
		m_pItemType(NULL),
		m_pScreensRoot(NULL),
		m_pSysCreateCtx(NULL),
		m_pExtension(NULL),
		m_pListData(NULL),
		m_pOldData(NULL),
		m_pOldSource(NULL),
		m_pOldItem (NULL),
		m_pOldOverlayID(NULL),
		m_bRestoreGlobalDefineHook(false),
		m_pOldGlobalDefineHook(NULL)

//	CCodeChainCtx constructor

	{
	}

CCodeChainCtx::~CCodeChainCtx (void)

//	CCodeChainCtx destructor

	{
	RestoreVars();

	if (m_bRestoreGlobalDefineHook)
		{
		IItemTransform *pHook = m_CC.GetGlobalDefineHook();
		m_CC.SetGlobalDefineHook(m_pOldGlobalDefineHook);
		delete pHook;
		}
	}

void CCodeChainCtx::AddFrame (void)

//	AddFrame
//
//	Adds an invocation frame

	{
	SInvokeFrame *pFrame = g_Invocations.Insert();
	pFrame->iEvent = m_iEvent;
	pFrame->pListData = m_pListData;
	}

C3DObjectPos CCodeChainCtx::As3DObjectPos (CSpaceObject *pObj, ICCItem *pItem, bool bAbsoluteRotation)

//	As3DObjectPos
//
//	Converts to C3DObjectPos

	{
	if (pItem == NULL || pItem->IsNil())
		return C3DObjectPos();
	else if (pItem->IsSymbolTable())
		{
		int iAngle = AngleMod(pItem->GetIntegerAt(FIELD_ANGLE));
		int iRadius = Max(0, pItem->GetIntegerAt(FIELD_RADIUS));
		int iZ = pItem->GetIntegerAt(FIELD_Z);

		return C3DObjectPos(iAngle, iRadius, iZ);
		}
	else
		{
		CVector vPos = AsVector(pItem);

		Metric rRadius;
		int iDirection = ::VectorToPolar(vPos - pObj->GetPos(), &rRadius);
		int iRotationOrigin = (!bAbsoluteRotation ? pObj->GetRotation() : 0);
		int iAngle = AngleMod(iDirection - iRotationOrigin);
		int iRadius = mathRound(rRadius / g_KlicksPerPixel);

		return C3DObjectPos(iAngle, iRadius);
		}
	}

bool CCodeChainCtx::AsArc (ICCItem *pItem, int *retiMinArc, int *retiMaxArc, bool *retbOmnidirectional)

//	AsArc
//
//	Converts to a fire arc

	{
	//	A value of nil means default value.

	if (pItem == NULL || pItem->IsNil())
		{
		if (retbOmnidirectional) *retbOmnidirectional = false;
		if (retiMinArc) *retiMinArc = -1;
		if (retiMaxArc) *retiMaxArc = -1;
		}

	//	A struct has fields

	else if (pItem->IsSymbolTable())
		{
		if (retbOmnidirectional) *retbOmnidirectional = false;
		if (retiMinArc) *retiMinArc = AngleMod(pItem->GetIntegerAt(FIELD_MIN));
		if (retiMaxArc) *retiMaxArc = AngleMod(pItem->GetIntegerAt(FIELD_MAX));
		}

	//	A value of "omnidirectional" counts

	else if (strEquals(pItem->GetStringValue(), FIELD_OMNIDIRECTIONAL))
		{
		if (retbOmnidirectional) *retbOmnidirectional = true;
		if (retiMinArc) *retiMinArc = 0;
		if (retiMaxArc) *retiMaxArc = 0;
		}

	//	A single value means that we just point in a direction

	else if (pItem->GetCount() == 1)
		{
		int iMinFireArc = AngleMod(pItem->GetElement(0)->GetIntegerValue());

		if (retbOmnidirectional) *retbOmnidirectional = false;
		if (retiMinArc) *retiMinArc = iMinFireArc;
		if (retiMaxArc) *retiMaxArc = iMinFireArc;
		}

	//	Otherwise we expect a list with two elements

	else if (pItem->GetCount() >= 2)
		{
		int iMinFireArc = AngleMod(pItem->GetElement(0)->GetIntegerValue());
		int iMaxFireArc = AngleMod(pItem->GetElement(1)->GetIntegerValue());

		if (retbOmnidirectional) *retbOmnidirectional = false;
		if (retiMinArc) *retiMinArc = iMinFireArc;
		if (retiMaxArc) *retiMaxArc = iMaxFireArc;
		}

	//	Invalid

	else
		return false;

	return true;
	}

DWORD CCodeChainCtx::AsNameFlags (ICCItem *pItem)

//	AsNameFlags
//
//	Parse name flags

	{
	int i;

	if (pItem->IsInteger())
		return pItem->GetIntegerValue();
	else if (pItem->IsIdentifier())
		return CLanguage::ParseNounFlags(pItem->GetStringValue());
	else
		{
		DWORD dwFlags = 0;

		for (i = 0; i < pItem->GetCount(); i++)
			dwFlags |= CLanguage::ParseNounFlags(pItem->GetElement(i)->GetStringValue());

		return dwFlags;
		}
	}

CSpaceObject *CCodeChainCtx::AsSpaceObject (ICCItem *pItem)

//	AsSpaceObject
//
//	Convert item to CSpaceObject

	{
	CSpaceObject *pObj;
	try
		{
		pObj = dynamic_cast<CSpaceObject *>((CObject *)pItem->GetIntegerValue());
		}
	catch (...)
		{
		pObj = NULL;
		}
	return pObj;
	}

CVector CCodeChainCtx::AsVector (ICCItem *pItem)

//	AsVector
//
//	Convert item to vector

	{
	return CreateVectorFromList(m_CC, pItem);
	}

void CCodeChainCtx::DefineDamageCtx (const SDamageCtx &Ctx, int iDamage)

//	DefineDamageCtx
//
//	Sets the standard variables that represent damage ctx.

	{
	DefineInteger(CONSTLIT("aArmorSeg"), Ctx.iSectHit);
	DefineSpaceObject(CONSTLIT("aAttacker"), Ctx.Attacker.GetObj());
	DefineSpaceObject(CONSTLIT("aCause"), Ctx.pCause);
	DefineInteger(CONSTLIT("aDamageHP"), (iDamage == -1 ? Ctx.iDamage : iDamage));
	DefineString(CONSTLIT("aDamageType"), GetDamageShortName(Ctx.Damage.GetDamageType()));
	DefineInteger(CONSTLIT("aHitDir"), Ctx.iDirection);
	DefineVector(CONSTLIT("aHitPos"), Ctx.vHitPos);
	DefineSpaceObject(CONSTLIT("aOrderGiver"), Ctx.GetOrderGiver());
	DefineInteger(CONSTLIT("aWeaponLevel"), Ctx.pDesc->GetLevel());
	DefineItemType(CONSTLIT("aWeaponType"), Ctx.pDesc->GetWeaponType());
	}

void CCodeChainCtx::DefineDamageEffects (const CString &sVar, SDamageCtx &Ctx)

//	DefineDamageEffects
//
//	Sets a variable to damage effects

	{
	ICCItem *pItem = CreateItemFromDamageEffects(m_CC, Ctx);
	m_CC.DefineGlobal(sVar, pItem);
	pItem->Discard(&m_CC);
	}

void CCodeChainCtx::DefineContainingType (CDesignType *pType)

//	DefineContainingType
//
//	Defines an containing type

{
	DefineInteger(CONSTLIT(STR_G_TYPE), pType->GetUNID());
}

void CCodeChainCtx::DefineContainingType (const CItem &Item)

//	DefineContainingType
//
//	Defines an containing type

	{
	DefineInteger(CONSTLIT(STR_G_TYPE), Item.GetType()->GetUNID());
	}

void CCodeChainCtx::DefineContainingType (const COverlay *pOverlay)

//	DefineContainingType
//
//	Defines an containing type

	{
	DefineInteger(CONSTLIT(STR_G_TYPE), pOverlay->GetType()->GetUNID());
	}

void CCodeChainCtx::DefineContainingType (CSpaceObject *pObj)

//	DefineContainingType
//
//	Defines an containing type

	{
	DefineContainingType(pObj->GetType());
	}

void CCodeChainCtx::DefineItem (const CString &sVar, CItemCtx &ItemCtx)

//	DefineItem
//
//	Defines a CItem variable

	{
	ICCItem *pItem = ItemCtx.CreateItemVariable(m_CC);
	m_CC.DefineGlobal(sVar, pItem);
	pItem->Discard(&m_CC);
	}

void CCodeChainCtx::DefineItem (const CItem &Item)

//	DefineItem
//
//	Sets gItem

	{
	DefineItem(STR_G_ITEM, Item);
	}

void CCodeChainCtx::DefineItem (const CString &sVar, const CItem &Item)

//	DefineItem
//
//	Defines a CItem variable

	{
	if (Item.GetType())
		{
		ICCItem *pItem = CreateListFromItem(m_CC, Item);
		m_CC.DefineGlobal(sVar, pItem);
		pItem->Discard(&m_CC);
		}
	else
		m_CC.DefineGlobal(sVar, m_CC.CreateNil());
	}

void CCodeChainCtx::DefineItemType (const CString &sVar, CItemType *pType)

//	DefineItemType
//
//	Defines a CItemType variable

	{
	if (pType)
		m_CC.DefineGlobalInteger(sVar, pType->GetUNID());
	else
		m_CC.DefineGlobal(sVar, m_CC.CreateNil());
	}

void CCodeChainCtx::DefineOrbit (const CString &sVar, const COrbit &OrbitDesc)

//	DefineOrbit
//
//	Defines a global COrbit variable

	{
	ICCItem *pValue = CreateListFromOrbit(m_CC, OrbitDesc);
	m_CC.DefineGlobal(sVar, pValue);
	pValue->Discard(&m_CC);
	}

void CCodeChainCtx::DefineSource (CSpaceObject *pSource)

//	DefineSource
//
//	Sets gSource

	{
	DefineGlobalSpaceObject(m_CC, STR_G_SOURCE, pSource);
	}

void CCodeChainCtx::DefineSpaceObject (const CString &sVar, CSpaceObject *pObj)

//	DefineSpaceObject
//
//	Defines a global SpaceObject variable

	{
	if (pObj)
		m_CC.DefineGlobalInteger(sVar, (int)pObj);
	else
		{
		ICCItem *pValue = m_CC.CreateNil();
		m_CC.DefineGlobal(sVar, pValue);
		pValue->Discard(&m_CC);
		}
	}

void CCodeChainCtx::DefineVector (const CString &sVar, const CVector &vVector)

//	DefineVector
//
//	Defines a global CVector variable

	{
	ICCItem *pValue = CreateListFromVector(m_CC, vVector);
	m_CC.DefineGlobal(sVar, pValue);
	pValue->Discard(&m_CC);
	}

DWORD CCodeChainCtx::GetAPIVersion (void) const

//	GetAPIVersion
//
//	Returns the API version of the extension
	
	{
	return (m_pExtension ? m_pExtension->GetAPIVersion() : API_VERSION);
	}

bool CCodeChainCtx::InEvent (ECodeChainEvents iEvent)

//	InEvent
//
//	Returns TRUE if we are inside this event (by looking at the invocation 
//	stack).

	{
	int i;

	for (i = 0; i < g_Invocations.GetCount(); i++)
		if (g_Invocations[i].iEvent == iEvent)
			return true;

	return false;
	}

void CCodeChainCtx::RemoveFrame (void)

//	RemoveFrame
//
//	Removes a frame

	{
	ASSERT(g_Invocations.GetCount() > 0);
	g_Invocations.Delete(g_Invocations.GetCount() - 1);
	}

void CCodeChainCtx::RestoreVars (void)

//	RestoreVars
//
//	Restores all saved variables

	{
	if (m_pOldItem)
		{
		m_CC.DefineGlobal(STR_G_ITEM, m_pOldItem);
		m_pOldItem->Discard(&m_CC);
		m_pOldItem = NULL;
		}

	if (m_pOldSource)
		{
		m_CC.DefineGlobal(STR_G_SOURCE, m_pOldSource);
		m_pOldSource->Discard(&m_CC);
		m_pOldSource = NULL;
		}

	if (m_pOldData)
		{
		m_CC.DefineGlobal(STR_G_DATA, m_pOldData);
		m_pOldData->Discard(&m_CC);
		m_pOldData = NULL;
		}

	if (m_pOldOverlayID)
		{
		m_CC.DefineGlobal(STR_A_OVERLAY_ID, m_pOldOverlayID);
		m_pOldOverlayID->Discard(&m_CC);
		m_pOldOverlayID = NULL;
		}
	}

ICCItem *CCodeChainCtx::Run (ICCItem *pCode)

//	Run
//
//	Runs the given piece of code and returns a result
//	(which must be discarded by the caller)

	{
	DEBUG_TRY

	AddFrame();
	ICCItem *pResult = m_CC.TopLevel(pCode, this);
	RemoveFrame();

	return pResult;

	DEBUG_CATCH
	}

ICCItem *CCodeChainCtx::Run (const SEventHandlerDesc &Event)

//	Run
//
//	Runs the given event and returns a result. (Which must be discarded by the
//	caller).

	{
	DEBUG_TRY

	CExtension *pOldExtension = m_pExtension;
	m_pExtension = Event.pExtension;

	ICCItem *pResult = Run(Event.pCode);

	m_pExtension = pOldExtension;
	return pResult;

	DEBUG_CATCH
	}

ICCItemPtr CCodeChainCtx::RunCode (const SEventHandlerDesc &Event)

//	RunCode
//
//	Runs the given event and returns a result. (Which must be discarded by the
//	caller).

	{
	DEBUG_TRY

	CExtension *pOldExtension = m_pExtension;
	m_pExtension = Event.pExtension;

	ICCItemPtr pResult = ICCItemPtr(Run(Event.pCode));

	m_pExtension = pOldExtension;
	return pResult;

	DEBUG_CATCH
	}

ICCItem *CCodeChainCtx::RunLambda (ICCItem *pCode)

//	RunLambda
//
//	Runs a piece of code or a lambda expression
//	and returns a result (which must be discarded by the caller)

	{
	DEBUG_TRY

	AddFrame();

	//	If this is a lambda expression, then eval as if
	//	it were an expression with no arguments

	ICCItem *pResult;
	if (pCode->IsFunction())
		pResult = m_CC.Apply(pCode, m_CC.CreateNil(), this);
	else
		pResult = m_CC.TopLevel(pCode, this);

	//	Done

	RemoveFrame();

	return pResult;

	DEBUG_CATCH
	}

bool CCodeChainCtx::RunEvalString (const CString &sString, bool bPlain, CString *retsResult)

//	RunString
//
//	If sString starts with '=' or if bPlain is TRUE, then we evaluate sString as an
//	expression. If success (no error) we return TRUE. Otherwise, we return FALSE and
//	the error is in retsResult.

	{
	char *pPos = sString.GetPointer();

	if (bPlain || *pPos == '=')
		{
		ICCItem *pExp = Link(sString, (bPlain ? 0 : 1), NULL);

		ICCItem *pResult = Run(pExp);	//	LATER:Event
		Discard(pExp);

		if (pResult->IsError())
			{
			*retsResult = pResult->GetStringValue();
			Discard(pResult);
			return false;
			}

		//	Note: We use GetStringValue instead of Unlink because we don't
		//	want to preserve CC semantics (e.g., we don't need strings to
		//	be quoted).

		*retsResult = pResult->GetStringValue();
		Discard(pResult);
		return true;
		}
	else
		{
		*retsResult = strCEscapeCodes(sString);
		return true;
		}
	}

void CCodeChainCtx::SaveAndDefineDataVar (ICCItem *pData)

//	SaveAndDefineDataVar
//
//	Saves and defines gData

	{
	if (m_pOldData == NULL)
		m_pOldData = m_CC.LookupGlobal(STR_G_DATA, this);

	if (pData)
		DefineVar(STR_G_DATA, pData);
	else
		DefineNil(STR_G_DATA);
	}

void CCodeChainCtx::SaveAndDefineItemVar (const CItem &Item)

//	SaveAndDefineItemVar
//
//	Saves and defines gItem

	{
	if (m_pOldItem == NULL)
		m_pOldItem = m_CC.LookupGlobal(STR_G_ITEM, this);

	DefineItem(STR_G_ITEM, Item);
	}

void CCodeChainCtx::SaveAndDefineItemVar (CItemCtx &ItemCtx)

//	SaveAndDefineItemVar
//
//	Saves and defines gItem

	{
	if (m_pOldItem == NULL)
		m_pOldItem = m_CC.LookupGlobal(STR_G_ITEM, this);

	DefineItem(STR_G_ITEM, ItemCtx);
	}

void CCodeChainCtx::SaveAndDefineOverlayID (DWORD dwID)

//	SaveAndDefineOverlayID
//
//	Save the overlay ID

	{
	//	NOTE: We save the previous overlayID in SetEvent. Callers must call
	//	SetEvent(eventOverlayEvent) before this method.

	DefineInteger(STR_A_OVERLAY_ID, (int)dwID);
	}

void CCodeChainCtx::SaveAndDefineSourceVar (CSpaceObject *pSource)

//	SaveAndDefineSourceVar
//
//	Saves and sets gSource

	{
	if (m_pOldSource == NULL)
		m_pOldSource = m_CC.LookupGlobal(STR_G_SOURCE, this);

	DefineGlobalSpaceObject(m_CC, STR_G_SOURCE, pSource);
	}

void CCodeChainCtx::SaveAndDefineSovereignVar (CSovereign *pSource)

//	SaveAndDefineSovereignVar
//
//	Saves and sets gSource

	{
	if (m_pOldSource == NULL)
		m_pOldSource = m_CC.LookupGlobal(STR_G_SOURCE, this);

	DefineInteger(STR_G_SOURCE, pSource->GetUNID());
	}

void CCodeChainCtx::SaveItemVar (void)

//	SaveItemVar
//
//	Saves gItem if not already saved

	{
	if (m_pOldItem == NULL)
		m_pOldItem = m_CC.LookupGlobal(STR_G_ITEM, this);
	}

void CCodeChainCtx::SaveSourceVar (void)

//	SaveSourceVar
//
//	Saves gSource if not already saved

	{
	if (m_pOldSource == NULL)
		m_pOldSource = m_CC.LookupGlobal(STR_G_SOURCE, this);
	}

void CCodeChainCtx::SetEvent (ECodeChainEvents iEvent)

//	SetEvent
//
//	Sets the event that we're about to invoke.

	{
	//	If we're about to invoke an overlay event, and we're currently inside an
	//	overlay event, then save aOverlayID.

	if (iEvent == eventOverlayEvent && InEvent(eventOverlayEvent))
		{
		if (m_pOldOverlayID == NULL)
			m_pOldOverlayID = m_CC.LookupGlobal(STR_A_OVERLAY_ID, this);
		}

	m_iEvent = iEvent;
	}

void CCodeChainCtx::SetGlobalDefineWrapper (CExtension *pExtension)

//	SetGlobalDefineWrapper
//
//	Sets a global define hook so that every global function that is defined gets
//	wrapper with an item that provides context.

	{
	if (!m_bRestoreGlobalDefineHook)
		{
		m_pOldGlobalDefineHook = m_CC.GetGlobalDefineHook();

		CAddFunctionContextWrapper *pHook = new CAddFunctionContextWrapper;
		pHook->SetExtension(pExtension);

		m_CC.SetGlobalDefineHook(pHook);

		m_bRestoreGlobalDefineHook = true;
		}
	}

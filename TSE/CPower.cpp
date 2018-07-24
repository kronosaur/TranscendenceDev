//	CPower.cpp
//
//	CPower class

#include "PreComp.h"

#define INVOKE_TAG								CONSTLIT("Invoke")
#define ON_SHOW_TAG								CONSTLIT("OnShow")
#define ON_INVOKED_BY_PLAYER_TAG				CONSTLIT("OnInvokedByPlayer")
#define ON_INVOKED_BY_NON_PLAYER_TAG			CONSTLIT("OnInvokedByNonPlayer")
#define ON_DESTROY_CHECK_TAG					CONSTLIT("OnDestroyCheck")

#define UNID_ATTRIB								CONSTLIT("UNID")
#define COST_ATTRIB								CONSTLIT("cost")
#define KEY_ATTRIB								CONSTLIT("key")
#define NAME_ATTRIB								CONSTLIT("name")

#define STR_A_CAUSE								CONSTLIT("aCause")
#define STR_A_DESTROYER							CONSTLIT("aDestroyer")

static char *CACHED_EVENTS[CPower::evtCount] =
	{
	"OnInvoke",
	"OnShow",
	"OnInvokedByPlayer",
	"OnInvokedByNonPlayer",
	"OnDestroyCheck",
	};

CPower::CPower (void)

//	CPower constructor

	{
	}

CPower::~CPower (void)

//	CPower destructor

	{
	}

void CPower::InitOldStyleEvent (ECachedHandlers iEvent, ICCItem *pCode)

//	InitOldStyleEvent
//
//	Initialize the event, if necessary.

	{
	if (m_CachedEvents[iEvent].pCode == NULL && pCode)
		{
		m_CachedEvents[iEvent].pCode = pCode;
		m_CachedEvents[iEvent].pExtension = GetExtension();
		}
	}

void CPower::Invoke (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError)

//	Invoke
//
//	Invoke the power

	{
	SEventHandlerDesc Event;
	if (!FindEventHandler(evtCode, &Event))
		return;

	CCodeChainCtx Ctx;
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(pSource);
	Ctx.DefineSpaceObject(CONSTLIT("gTarget"), pTarget);

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (retsError)
		{
		if (pResult->IsError())
			*retsError = pResult->GetStringValue();
		else
			*retsError = NULL_STR;
		}
	}

void CPower::InvokeByPlayer (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError)

//	Invoke
//
//	Invoke the power for a player

	{
	if (retsError) *retsError = NULL_STR;

	CCodeChainCtx Ctx;
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(pSource);
	Ctx.DefineSpaceObject(CONSTLIT("gTarget"), pTarget);

	//	First handle the OnInvokedByPlayer check. If it is not defined, then skip it.

	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnInvokedByPlayer, &Event))
		{
		ICCItemPtr pResult = Ctx.RunCode(Event);

		if (pResult->IsError())
			{
			if (retsError)
				*retsError = pResult->GetStringValue();

			//	Can't invoke

			return;
			}

		//	If OnInvokedByPlayer returns Nil, then we do not invoke

		else if (pResult->IsNil())
			return;
		}

	//	Invoke

	if (FindEventHandler(evtCode, &Event))
		{
		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			{
			if (retsError)
				*retsError = pResult->GetStringValue();
			}
		}
	}

void CPower::InvokeByNonPlayer(CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError)

//	Invoke
//
//	Invoke the power for a non-player

	{
	if (retsError) *retsError = NULL_STR;

	CCodeChainCtx Ctx;
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(pSource);
	Ctx.DefineSpaceObject(CONSTLIT("gTarget"), pTarget);

	//	First handle the OnInvokedByNonPlayer check. If it is not defined, then skip it.

	SEventHandlerDesc Event;
	if (FindEventHandler(evtOnInvokedByNonPlayer, &Event))
		{
		ICCItemPtr pResult = Ctx.RunCode(Event);

		if (pResult->IsError())
			{
			if (retsError)
				*retsError = pResult->GetStringValue();

			//	Can't invoke

			return;
			}

		//	If OnInvokedByNonPlayer returns Nil, then we do not invoke

		else if (pResult->IsNil())
			return;
		}

	//	Invoke

	if (FindEventHandler(evtCode, &Event))
		{
		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			{
			if (retsError)
				*retsError = pResult->GetStringValue();
			}
		}
	}

ALERROR CPower::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML

	{
	int i;

	//	Load basic stuff

	m_sName = pDesc->GetAttribute(NAME_ATTRIB);
	m_iInvokeCost = pDesc->GetAttributeInteger(COST_ATTRIB);
	m_sInvokeKey = pDesc->GetAttribute(KEY_ATTRIB);

	//	Load various code blocks

	m_pCode = NULL;
	m_pOnShow = NULL;
	m_pOnInvokedByPlayer = NULL;
	m_pOnInvokedByNonPlayer = NULL;
	m_pOnDestroyCheck = NULL;
	for (i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pBlock = pDesc->GetContentElement(i);

		if (strEquals(pBlock->GetTag(), INVOKE_TAG))
			{
			if (!m_pCode.Load(pBlock->GetContentText(0), &Ctx.sError))
				return ERR_FAIL;
			}
		else if (strEquals(pBlock->GetTag(), ON_SHOW_TAG))
			{
			if (!m_pOnShow.Load(pBlock->GetContentText(0), &Ctx.sError))
				return ERR_FAIL;
			}
		else if (strEquals(pBlock->GetTag(), ON_INVOKED_BY_PLAYER_TAG))
			{
			if (!m_pOnInvokedByPlayer.Load(pBlock->GetContentText(0), &Ctx.sError))
				return ERR_FAIL;
			}
		else if (strEquals(pBlock->GetTag(), ON_INVOKED_BY_NON_PLAYER_TAG))
			{
			if (!m_pOnInvokedByNonPlayer.Load(pBlock->GetContentText(0), &Ctx.sError))
				return ERR_FAIL;
			}
		else if (strEquals(pBlock->GetTag(), ON_DESTROY_CHECK_TAG))
			{
			if (!m_pOnDestroyCheck.Load(pBlock->GetContentText(0), &Ctx.sError))
				return ERR_FAIL;
			}
		else if (IsValidLoadXML(pBlock->GetTag()))
			;
		else
			{
			return ComposeLoadError(Ctx, strPatternSubst(CONSTLIT("Unknown element: <%s>"), pBlock->GetTag()));
			}
		}

	//	Done

	return NOERROR;
	}

ALERROR CPower::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design.

	{
	//	Init from standard events

	InitCachedEvents(evtCount, CACHED_EVENTS, m_CachedEvents);

	//	See if we have old-style events.

	InitOldStyleEvent(evtCode, m_pCode);
	InitOldStyleEvent(evtOnShow, m_pOnShow);
	InitOldStyleEvent(evtOnInvokedByPlayer, m_pOnInvokedByPlayer);
	InitOldStyleEvent(evtOnInvokedByNonPlayer, m_pOnInvokedByNonPlayer);
	InitOldStyleEvent(evtOnDestroyCheck, m_pOnDestroyCheck);

	//	Done

	return NOERROR;
	}

bool CPower::OnDestroyCheck (CSpaceObject *pSource, DestructionTypes iCause, const CDamageSource &Attacker)

//	OnDestroyCheck
//
//	Returns TRUE if ship can be destroyed; otherwise, FALSE

	{
	SEventHandlerDesc Event;
	if (!FindEventHandler(evtOnDestroyCheck, &Event))
		return true;

	//	Set up parameters

	CCodeChainCtx Ctx;
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(pSource);
	Ctx.DefineSpaceObject(STR_A_DESTROYER, Attacker.GetObj());
	Ctx.DefineInteger(STR_A_CAUSE, iCause);

	//	Invoke

	ICCItemPtr pResult = Ctx.RunCode(Event);
	return (pResult->IsNil() ? false : true);
	}

bool CPower::OnShow (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError)

//	OnShow
//
//	Returns TRUE if we should show this power on the menu

	{
	SEventHandlerDesc Event;
	if (!FindEventHandler(evtOnShow, &Event))
		return true;

	//	Set up parameters

	CCodeChainCtx Ctx;
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(pSource);
	Ctx.DefineSpaceObject(CONSTLIT("gTarget"), pTarget);

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (retsError)
		{
		if (pResult->IsError())
			*retsError = pResult->GetStringValue();
		else
			*retsError = NULL_STR;
		}

	return !pResult->IsNil();
	}

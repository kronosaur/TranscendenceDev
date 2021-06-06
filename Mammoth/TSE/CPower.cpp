//	CPower.cpp
//
//	CPower class
//	Copyright (c) 2020 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define IMAGE_TAG								CONSTLIT("Image")
#define INVOKE_TAG								CONSTLIT("Invoke")
#define ON_SHOW_TAG								CONSTLIT("OnShow")
#define ON_INVOKED_BY_PLAYER_TAG				CONSTLIT("OnInvokedByPlayer")
#define ON_INVOKED_BY_NON_PLAYER_TAG			CONSTLIT("OnInvokedByNonPlayer")
#define ON_DESTROY_CHECK_TAG					CONSTLIT("OnDestroyCheck")

#define UNID_ATTRIB								CONSTLIT("UNID")
#define COST_ATTRIB								CONSTLIT("cost")
#define KEY_ATTRIB								CONSTLIT("key")
#define LEVEL_ATTRIB							CONSTLIT("level")
#define NAME_ATTRIB								CONSTLIT("name")

#define PROPERTY_NAME							CONSTLIT("name")

#define STR_A_CAUSE								CONSTLIT("aCause")
#define STR_A_DESTROYER							CONSTLIT("aDestroyer")

static const char *CACHED_EVENTS[(int)CPower::EEvent::count] =
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

const CObjectImageArray &CPower::GetImage (void) const

//	GetImage
//
//	Returns an image for the power.

	{
	if (!m_Image.IsEmpty())
		return m_Image;
	else if (GetInheritFrom())
		return GetInheritFrom()->GetTypeSimpleImage();
	else
		return CObjectImageArray::Null();
	}

void CPower::InitOldStyleEvent (EEvent iEvent, ICCItem *pCode)

//	InitOldStyleEvent
//
//	Initialize the event, if necessary.

	{
	if (m_CachedEvents[(int)iEvent].pCode == NULL && pCode)
		{
		m_CachedEvents[(int)iEvent].pCode = pCode;
		m_CachedEvents[(int)iEvent].pExtension = GetExtension();
		}
	}

void CPower::Invoke (CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError)

//	Invoke
//
//	Invoke the power

	{
	SEventHandlerDesc Event;
	if (!FindEventHandler(EEvent::Code, &Event))
		return;

	CCodeChainCtx Ctx(GetUniverse());
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
	if (!pSource)
		throw CException(ERR_FAIL);

	if (retsError) *retsError = NULL_STR;

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(pSource);
	Ctx.DefineSpaceObject(CONSTLIT("gTarget"), pTarget);

	//	First handle the OnInvokedByPlayer check. If it is not defined, then skip it.

	SEventHandlerDesc Event;
	if (FindEventHandler(EEvent::OnInvokedByPlayer, &Event))
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

	if (FindEventHandler(EEvent::Code, &Event))
		{
		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			{
			if (retsError)
				*retsError = pResult->GetStringValue();
			}
		}

	//	Notify player that we were invoked

	if (pSource->IsPlayer())
		GetUniverse().GetPlayer().OnPowerInvoked(*this);
	}

void CPower::InvokeByNonPlayer(CSpaceObject *pSource, CSpaceObject *pTarget, CString *retsError)

//	Invoke
//
//	Invoke the power for a non-player

	{
	if (retsError) *retsError = NULL_STR;

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(pSource);
	Ctx.DefineSpaceObject(CONSTLIT("gTarget"), pTarget);

	//	First handle the OnInvokedByNonPlayer check. If it is not defined, then skip it.

	SEventHandlerDesc Event;
	if (FindEventHandler(EEvent::OnInvokedByNonPlayer, &Event))
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

	if (FindEventHandler(EEvent::Code, &Event))
		{
		ICCItemPtr pResult = Ctx.RunCode(Event);
		if (pResult->IsError())
			{
			if (retsError)
				*retsError = pResult->GetStringValue();
			}
		}
	}

void CPower::OnAccumulateXMLMergeFlags (TSortMap<DWORD, DWORD> &MergeFlags) const

//	OnAccumulateXMLMergeFlags
//
//	Returns flags to determine how we merge from inherited types.

	{
	//	We know how to handle these tags through the inheritance hierarchy.

	MergeFlags.SetAt(CXMLElement::GetKeywordID(IMAGE_TAG), CXMLElement::MERGE_OVERRIDE);
	}

ALERROR CPower::OnBindDesign (SDesignLoadCtx &Ctx)

//	OnBindDesign
//
//	Bind design.

	{
	//	Images

	if (ALERROR error = m_Image.OnDesignLoadComplete(Ctx))
		return error;

	//	Init from standard events

	InitCachedEvents((int)EEvent::count, CACHED_EVENTS, m_CachedEvents);

	//	See if we have old-style events.

	InitOldStyleEvent(EEvent::Code, m_pCode);
	InitOldStyleEvent(EEvent::OnShow, m_pOnShow);
	InitOldStyleEvent(EEvent::OnInvokedByPlayer, m_pOnInvokedByPlayer);
	InitOldStyleEvent(EEvent::OnInvokedByNonPlayer, m_pOnInvokedByNonPlayer);
	InitOldStyleEvent(EEvent::OnDestroyCheck, m_pOnDestroyCheck);

	//	Done

	return NOERROR;
	}

ALERROR CPower::OnCreateFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	OnCreateFromXML
//
//	Create from XML

	{
	//	Load basic stuff

	m_sName = pDesc->GetAttribute(NAME_ATTRIB);
	m_iLevel = pDesc->GetAttributeIntegerBounded(LEVEL_ATTRIB, 1, MAX_SYSTEM_LEVEL);
	m_iInvokeCost = pDesc->GetAttributeInteger(COST_ATTRIB);
	m_sInvokeKey = pDesc->GetAttribute(KEY_ATTRIB);

	//	Load various code blocks

	m_pCode = NULL;
	m_pOnShow = NULL;
	m_pOnInvokedByPlayer = NULL;
	m_pOnInvokedByNonPlayer = NULL;
	m_pOnDestroyCheck = NULL;
	for (int i = 0; i < pDesc->GetContentElementCount(); i++)
		{
		CXMLElement *pBlock = pDesc->GetContentElement(i);

		if (strEquals(pBlock->GetTag(), IMAGE_TAG))
			{
			if (ALERROR error = m_Image.InitFromXML(Ctx, *pBlock))
				return ComposeLoadError(Ctx, CONSTLIT("Unable to load image"));
			}
		else if (strEquals(pBlock->GetTag(), INVOKE_TAG))
			{
			if (!m_pCode.Load(pBlock->GetContentText(0), &Ctx.sError))
				return ComposeLoadError(Ctx, Ctx.sError);
			}
		else if (strEquals(pBlock->GetTag(), ON_SHOW_TAG))
			{
			if (!m_pOnShow.Load(pBlock->GetContentText(0), &Ctx.sError))
				return ComposeLoadError(Ctx, Ctx.sError);
			}
		else if (strEquals(pBlock->GetTag(), ON_INVOKED_BY_PLAYER_TAG))
			{
			if (!m_pOnInvokedByPlayer.Load(pBlock->GetContentText(0), &Ctx.sError))
				return ComposeLoadError(Ctx, Ctx.sError);
			}
		else if (strEquals(pBlock->GetTag(), ON_INVOKED_BY_NON_PLAYER_TAG))
			{
			if (!m_pOnInvokedByNonPlayer.Load(pBlock->GetContentText(0), &Ctx.sError))
				return ComposeLoadError(Ctx, Ctx.sError);
			}
		else if (strEquals(pBlock->GetTag(), ON_DESTROY_CHECK_TAG))
			{
			if (!m_pOnDestroyCheck.Load(pBlock->GetContentText(0), &Ctx.sError))
				return ComposeLoadError(Ctx, Ctx.sError);
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

bool CPower::OnDestroyCheck (CSpaceObject *pSource, DestructionTypes iCause, const CDamageSource &Attacker)

//	OnDestroyCheck
//
//	Returns TRUE if ship can be destroyed; otherwise, FALSE

	{
	if (!pSource)
		throw CException(ERR_FAIL);

	SEventHandlerDesc Event;
	if (!FindEventHandler(EEvent::OnDestroyCheck, &Event))
		return true;

	//	Set up parameters

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(pSource);
	Ctx.DefineSpaceObject(STR_A_DESTROYER, Attacker.GetObj());
	Ctx.DefineInteger(STR_A_CAUSE, iCause);

	//	Invoke

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (!pResult->IsNil())
		return true;

	//	This counts as an invocation.

	if (pSource->IsPlayer())
		GetUniverse().GetPlayer().OnPowerInvoked(*this);

	//	Cancel destruction

	return false;
	}

void CPower::OnMarkImages (void)

//	OnMarkImages
//
//	Mark images in use.

	{
	m_Image.MarkImage();
	}

bool CPower::OnShow (CSpaceObject &SourceObj, CSpaceObject *pTarget, DWORD &retdwCooldownStart, DWORD &retdwCooldownEnd, CString *retsError)

//	OnShow
//
//	Returns TRUE if we should show this power on the menu

	{
	if (retsError) *retsError = NULL_STR;
	retdwCooldownStart = 0;
	retdwCooldownEnd = 0;

	SEventHandlerDesc Event;
	if (!FindEventHandler(EEvent::OnShow, &Event))
		return true;

	//	Set up parameters

	CCodeChainCtx Ctx(GetUniverse());
	Ctx.DefineContainingType(this);
	Ctx.SaveAndDefineSourceVar(&SourceObj);
	Ctx.DefineSpaceObject(CONSTLIT("gTarget"), pTarget);

	ICCItemPtr pResult = Ctx.RunCode(Event);
	if (pResult->IsError())
		{
		if (retsError)
			*retsError = pResult->GetStringValue();

		return true;
		}
	else if (pResult->IsSymbolTable())
		{
		retdwCooldownStart = pResult->GetIntegerAt(CONSTLIT("cooldownStart"));
		retdwCooldownEnd = pResult->GetIntegerAt(CONSTLIT("cooldownEnd"));
		return true;
		}
	else
		return !pResult->IsNil();
	}

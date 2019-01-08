//	CCommunicationsStack.cpp
//
//	CCommunicationsStack class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

CCommunicationsStack::CCommunicationsStack (void) :
		m_bInitialized(false)

//	CCommunicationsStack constructor

	{
	}

CCommunicationsHandler *CCommunicationsStack::GetCommsHandler (CDesignType *pParent)

//	GetCommsHandler
//
//	Returns an integrated communications handler, including any inherited 
//	values. Or NULL if we have none.

	{
	CCommunicationsHandler *pParentHandler;
	if (pParent && (pParentHandler = pParent->GetCommsHandler()))
		{
		if (!m_bInitialized)
			{
			m_Composite.Merge(m_CommsHandler);
			m_Composite.Merge(*pParentHandler);

			m_bInitialized = true;
			}

		return (m_Composite.GetCount() ? &m_Composite : NULL);
		}
	else
		return (m_CommsHandler.GetCount() ? &m_CommsHandler : NULL);
	}

ALERROR CCommunicationsStack::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc)

//	InitFromXML
//
//	Initialize from XML

	{
	if (pDesc == NULL)
		return NOERROR;

	if (m_CommsHandler.InitFromXML(pDesc, &Ctx.sError) != NOERROR)
		return ERR_FAIL;

	m_bInitialized = false;
	return NOERROR;
	}

void CCommunicationsStack::Merge (CCommunicationsStack &Src)

//	Merge
//
//	Merges from a source

	{
	if (Src.m_CommsHandler.GetCount() > 0)
		m_CommsHandler.Merge(Src.m_CommsHandler);
	}

void CCommunicationsStack::Unbind (void)

//	Unbind
//
//	Unbind in case the inheritance hierarchy changed.

	{
	m_bInitialized = false;
	m_Composite.DeleteAll();
	}

//	CCommunicationsHandler.cpp
//
//	CCommunicationsHandler class
//	Copyright (c) 2017 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define ON_SHOW_TAG									CONSTLIT("OnShow")
#define CODE_TAG									CONSTLIT("Code")
#define INVOKE_TAG									CONSTLIT("Invoke")

#define ID_ATTRIB									CONSTLIT("id")
#define KEY_ATTRIB									CONSTLIT("key")
#define NAME_ATTRIB									CONSTLIT("name")

CCommunicationsHandler::CCommunicationsHandler (void)

//	CCommunicationsHandler constructor

	{
	}

CCommunicationsHandler::~CCommunicationsHandler (void)

//	CCommunicationsHandler destructor

	{
	DeleteAll();
	}

CCommunicationsHandler &CCommunicationsHandler::operator= (const CCommunicationsHandler &Src)

//	CCommunicationsHandler operator =

	{
	int i;

	DeleteAll();

	m_Messages = Src.m_Messages;

	for (i = 0; i < GetCount(); i++)
		{
		if (m_Messages[i].InvokeEvent.pCode)
			m_Messages[i].InvokeEvent.pCode = m_Messages[i].InvokeEvent.pCode->Reference();

		if (m_Messages[i].OnShowEvent.pCode)
			m_Messages[i].OnShowEvent.pCode = m_Messages[i].OnShowEvent.pCode->Reference();
		}

	return *this;
	}

void CCommunicationsHandler::DeleteAll (void)

//	DeleteAll
//
//	Delete all messages
	
	{
	for (int i = 0; i < GetCount(); i++)
		{
		if (m_Messages[i].InvokeEvent.pCode)
			m_Messages[i].InvokeEvent.pCode->Discard(&g_pUniverse->GetCC());

		if (m_Messages[i].OnShowEvent.pCode)
			m_Messages[i].OnShowEvent.pCode->Discard(&g_pUniverse->GetCC());
		}

	m_Messages.DeleteAll(); 
	}

bool CCommunicationsHandler::FindMergePos (const SMessage &Msg, int *retiPos)

//	FindMergePos
//
//	If the given message does not already exist (by ID) then we return
//	the position at which it should be inserted.

	{
	int i;

	//	If we already have this message, then we don't need to insert it.

	for (i = 0; i < GetCount(); i++)
		if (strEquals(m_Messages[i].sID, Msg.sID))
			return false;

	//	Figure out where to insert it (by shortcut order)

	if (retiPos)
		{
		i = 0;
		while (i < GetCount() && strCompareAbsolute(Msg.sShortcut, m_Messages[i].sShortcut) != 1)
			i++;

		*retiPos = i;
		}

	return true;
	}

bool CCommunicationsHandler::FindMessage (const CString &sID, const SMessage **retpMessage) const

//	FindMessage
//
//	Finds the message by ID

	{
	int i;

	for (i = 0; i < GetCount(); i++)
		if (strEquals(m_Messages[i].sID, sID))
			{
			if (retpMessage)
				*retpMessage = &m_Messages[i];
			return true;
			}

	return false;
	}

int CCommunicationsHandler::FindMessageByName (const CString &sMessage) const

//	FindMessageByName
//
//	Finds the message by name

	{
	for (int i = 0; i < GetCount(); i++)
		if (strEquals(m_Messages[i].sMessage, sMessage))
			return i;

	return -1;
	}

void CCommunicationsHandler::FireInvoke (const CString &sID, CSpaceObject *pObj, CSovereign *pSender, ICCItem *pData)

//	FireInvoke
//
//	Invoke the message

	{
	//	Get the code

	const SMessage *pMsg;
	if (!FindMessage(sID, &pMsg))
		return;

	if (pMsg->InvokeEvent.pCode == NULL)
		return;

	//	Run

	CCodeChainCtx Ctx;

	//	Define parameters

	Ctx.SaveAndDefineSourceVar(pObj);
	Ctx.SaveAndDefineDataVar(pData);
	Ctx.DefineInteger(CONSTLIT("aPlayer"), pSender->GetUNID());

	//	Execute

	ICCItem *pResult = Ctx.Run(pMsg->InvokeEvent);
	if (pResult->IsError())
		pSender->MessageFromObj(pObj, pResult->GetStringValue());

	Ctx.Discard(pResult);
	}

ALERROR CCommunicationsHandler::InitFromXML (CXMLElement *pDesc, CString *retsError)

//	InitFromXML
//
//	Load from an XML element

	{
	int i, j;
	CString sError;

	//	Allocate the structure

	int iCount = pDesc->GetContentElementCount();
	if (iCount == 0)
		return NOERROR;

	m_Messages.InsertEmpty(iCount);

	for (i = 0; i < iCount; i++)
		{
		CXMLElement *pMessage = pDesc->GetContentElement(i);

		//	ID

		m_Messages[i].sID = pMessage->GetAttribute(ID_ATTRIB);

		//	Get the name

		m_Messages[i].sMessage = pMessage->GetAttribute(NAME_ATTRIB);
		m_Messages[i].sShortcut = pMessage->GetAttribute(KEY_ATTRIB);

		//	If the ID is blank, then use the shortcut; if that's missing, use the ordinal value.

		if (m_Messages[i].sID.IsBlank())
			{
			if (!m_Messages[i].sShortcut.IsBlank())
				m_Messages[i].sID = m_Messages[i].sShortcut;
			else
				m_Messages[i].sID = strFromInt(i);
			}

		//	If no sub elements, just get the code from the content

		if (pMessage->GetContentElementCount() == 0)
			{
			m_Messages[i].InvokeEvent.pExtension = NULL;
			m_Messages[i].InvokeEvent.pCode = g_pUniverse->GetCC().Link(pMessage->GetContentText(0), 0, NULL);

			m_Messages[i].OnShowEvent.pExtension = NULL;
			m_Messages[i].OnShowEvent.pCode = NULL;
			}

		//	If we've got sub elements, then load the different code blocks

		else
			{
			m_Messages[i].InvokeEvent.pExtension = NULL;
			m_Messages[i].InvokeEvent.pCode = NULL;

			m_Messages[i].OnShowEvent.pExtension = NULL;
			m_Messages[i].OnShowEvent.pCode = NULL;

			for (j = 0; j < pMessage->GetContentElementCount(); j++)
				{
				CXMLElement *pItem = pMessage->GetContentElement(j);

				//	OnShow

				if (strEquals(pItem->GetTag(), ON_SHOW_TAG))
					{
					m_Messages[i].OnShowEvent.pExtension = NULL;
					m_Messages[i].OnShowEvent.pCode = g_pUniverse->GetCC().Link(pItem->GetContentText(0), 0, NULL);
					}
				else if (strEquals(pItem->GetTag(), INVOKE_TAG) || strEquals(pItem->GetTag(), CODE_TAG))
					{
					m_Messages[i].InvokeEvent.pExtension = NULL;
					m_Messages[i].InvokeEvent.pCode = g_pUniverse->GetCC().Link(pItem->GetContentText(0), 0, NULL);
					}
				else
					{
					*retsError = strPatternSubst(CONSTLIT("Unknown element: <%s>"), pItem->GetTag());
					return ERR_FAIL;
					}
				}
			}

		//	Deal with error

		if (m_Messages[i].InvokeEvent.pCode && m_Messages[i].InvokeEvent.pCode->IsError())
			sError = strPatternSubst(CONSTLIT("Communications: %s <Invoke>: %s"),
					m_Messages[i].sMessage,
					m_Messages[i].InvokeEvent.pCode->GetStringValue());

		else if (m_Messages[i].OnShowEvent.pCode && m_Messages[i].OnShowEvent.pCode->IsError())
			sError = strPatternSubst(CONSTLIT("Communications: %s <OnShow>: %s"),
					m_Messages[i].sMessage,
					m_Messages[i].OnShowEvent.pCode->GetStringValue());
		}

	//	Done

	if (retsError)
		*retsError = sError;

	return (sError.IsBlank() ? NOERROR : ERR_FAIL);
	}

void CCommunicationsHandler::Merge (CCommunicationsHandler &New)

//	Merge
//
//	Merges messages from the new handler into this handler

	{
	int i;

	for (i = 0; i < New.GetCount(); i++)
		{
		int iInsert;
		if (FindMergePos(New.m_Messages[i], &iInsert))
			{
			SMessage *pMsg = m_Messages.InsertAt(iInsert);

			pMsg->sID = New.m_Messages[i].sID;
			pMsg->sMessage = New.m_Messages[i].sMessage;
			pMsg->sShortcut = New.m_Messages[i].sShortcut;

			pMsg->InvokeEvent = New.m_Messages[i].InvokeEvent;
			if (pMsg->InvokeEvent.pCode)
				pMsg->InvokeEvent.pCode->Reference();

			pMsg->OnShowEvent = New.m_Messages[i].OnShowEvent;
			if (pMsg->OnShowEvent.pCode)
				pMsg->OnShowEvent.pCode->Reference();
			}
		}
	}

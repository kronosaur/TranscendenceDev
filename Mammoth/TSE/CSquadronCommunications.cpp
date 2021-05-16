//	CSquadronCommunications.cpp
//
//	CSquadronCommunications class
//	Copyright (c) 2021 Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

TStaticStringTable<TStaticStringEntry<CSquadronCommunications::SSquadronMsgDesc>, 8> CSquadronCommunications::m_SquadronMsgTable = 
	{
		"msgAttackInFormation",	{	ESquadronMsg::AttackInFormation,	"msgAttackInFormation",	"Attack in formation",	msgAttackInFormation,	resCanAttackInFormation	},
		"msgAttackTarget",		{	ESquadronMsg::Attack,				"msgAttackTarget",		"Attack target",		msgAttack,				resCanAttack		},
		"msgBreakAndAttack",	{	ESquadronMsg::BreakAndAttack,		"msgBreakAndAttack",	"Break & attack",		msgBreakAndAttack,		resCanBreakAndAttack	},
		"msgFormationAlpha",	{	ESquadronMsg::FormationAlpha,		"msgFormationAlpha",	"Alpha formation",		msgFormUp,				resCanBeInFormation	},
		"msgFormationBeta",		{	ESquadronMsg::FormationBeta,		"msgFormationBeta",		"Beta formation",		msgFormUp,				resCanBeInFormation	},
		"msgFormationGamma",	{	ESquadronMsg::FormationGamma,		"msgFormationGamma",	"Gamma formation",		msgFormUp,				resCanBeInFormation	},
		"msgFormUp",			{	ESquadronMsg::FormUp,				"msgFormUp",			"Form up",				msgFormUp,				resCanFormUp	},
		"msgWait",				{	ESquadronMsg::Wait,					"msgWait",				"Wait",					msgWait,				resCanWait	},
	};

CSquadronCommunications::CSquadronCommunications (CSpaceObject &LeaderObj) :
		m_LeaderObj(LeaderObj)

//	CSquadronCommunications constructor

	{
	//	Initialize with all active squadron members

	CSystem *pSystem = m_LeaderObj.GetSystem();
	if (!pSystem)
		return;

	for (int i = 0; i < pSystem->GetObjectCount(); i++)
		{
		CSpaceObject *pObj = pSystem->GetObject(i);

		if (pObj && m_LeaderObj.IsInOurSquadron(*pObj))
			m_Squadron.Insert(pObj);
		}
	}

CSquadronCommunications::CSquadronCommunications (CSpaceObject &LeaderObj, const TArray<CSpaceObject *> &List) :
		m_LeaderObj(LeaderObj),
		m_Squadron(List)

//	CSquadronCommunications constructor

	{
	}

void CSquadronCommunications::AccumulateMessageList (CSpaceObject &Obj, TSortMap<CString, int> &retList) const

//	AccumulateMessageList
//
//	Add the list of messages that we handle to the list.

	{
	//	If we have a communications stack then we use it to fill the message list.

	if (const CCommunicationsHandler *pHandler = Obj.GetCommsHandler())
		{
		for (int i = 0; i < pHandler->GetCount(); i++)
			{
			const auto &Msg = pHandler->GetMessage(i);
			if (!Msg.sID.IsBlank()
					&& Obj.IsCommsMessageValidFrom(m_LeaderObj, i))
				{
				bool bNew;
				int *pCount = retList.SetAt(Msg.sID, &bNew);
				if (bNew)
					*pCount = 1;
				else
					(*pCount) += 1;
				}
			}
		}

	//	Otherwise, we use the old-style comms infrastructure

	else
		{
		DWORD dwStatus = m_LeaderObj.Communicate(&Obj, msgQueryCommunications);

		for (int i = 0; i < m_SquadronMsgTable.GetCount(); i++)
			{
			const auto &Entry = m_SquadronMsgTable[i].Value;
			if (dwStatus & (DWORD)Entry.iResponse)
				{
				bool bNew;
				int *pCount = retList.SetAt(CString(Entry.pszID), &bNew);
				if (bNew)
					*pCount = 1;
				else
					(*pCount) += 1;
				}
			}
		}
	}

TArray<CString> CSquadronCommunications::GetMessageList () const

//	GetMessageList
//
//	Returns the list of messages that ALL objects in the squadron handle.

	{
	TSortMap<CString, int> AllMessages;

	for (int i = 0; i < m_Squadron.GetCount(); i++)
		{
		AccumulateMessageList(*m_Squadron[i], AllMessages);
		}

	TArray<CString> Result;
	for (int i = 0; i < AllMessages.GetCount(); i++)
		if (AllMessages[i] == m_Squadron.GetCount())
			Result.Insert(AllMessages.GetKey(i));

	return Result;
	}

DWORD CSquadronCommunications::GetSquadronCommsStatus ()

//	GetSquandronCommsStatus
//
//	Returns the set of messages that the squadron will respond to.

	{
	DWORD dwStatus = 0;

	for (int i = 0; i < m_Squadron.GetCount(); i++)
		{
		CSpaceObject &Obj = *m_Squadron[i];

		//	First add messages from the old-style "fleet" controller.

		dwStatus |= m_LeaderObj.Communicate(&Obj, msgQueryCommunications);

		//	Next add in the well-known messages handled through the 
		//	communications structure.

		for (int j = 0; j < m_SquadronMsgTable.GetCount(); i++)
			{
			const auto &Entry = m_SquadronMsgTable[j].Value;
			if (!(dwStatus & (DWORD)Entry.iResponse)
					&& IsMessageValidFor(Obj, Entry))
				{
				dwStatus |= (DWORD)Entry.iResponse;
				}
			}
		}

	return dwStatus;
	}

CUniverse &CSquadronCommunications::GetUniverse () const

//	GetUniverse
//
//	Returns the universe.

	{
	return m_LeaderObj.GetUniverse();
	}

bool CSquadronCommunications::IsMessageValidFor (CSpaceObject &Obj, const SSquadronMsgDesc &MsgDesc, int *retiIndex) const

//	IsMessageValidFor
//
//	Returns TRUE if the given message is valid for the object.

	{
	const CCommunicationsHandler *pHandler = Obj.GetCommsHandler();
	if (pHandler == NULL)
		return false;

	int iIndex = pHandler->FindMessageByID(CString(MsgDesc.pszID));
	if (iIndex == -1)
		{
		iIndex = pHandler->FindMessageByName(CString(MsgDesc.pszName));
		if (iIndex == -1)
			return false;
		}

	//	Make sure the message is valid.

	if (!Obj.IsCommsMessageValidFrom(m_LeaderObj, iIndex))
		return false;

	//	Valid!

	if (retiIndex)
		*retiIndex = iIndex;

	return true;
	}

void CSquadronCommunications::Send (const CString &sMsg) const

//	Send
//
//	Sends the given message to all members.

	{
	//	See if this is a stadard message

	int iPos;
	if (m_SquadronMsgTable.FindPos(sMsg, &iPos))
		{
		Send(m_SquadronMsgTable[iPos].Value);
		}
	else
		{
		for (int i = 0; i < m_Squadron.GetCount(); i++)
			Send(*m_Squadron[i], sMsg);
		}
	}

void CSquadronCommunications::Send (const SSquadronMsgDesc &MsgDesc) const

//	Send
//
//	Sends a message to the squadron.

	{
	DWORD dwFormationPos = 0;

	for (int i = 0; i < m_Squadron.GetCount(); i++)
		{
		if (Send(*m_Squadron[i], MsgDesc, dwFormationPos))
			dwFormationPos++;
		}
	}

void CSquadronCommunications::Send (CSpaceObject &ReceiverObj, const CString &sMsg) const

//	Send
//
//	Sends the message to the given object.

	{
	const CCommunicationsHandler *pHandler = ReceiverObj.GetCommsHandler();
	if (!pHandler)
		return;

	int iIndex;
	if ((iIndex = pHandler->FindMessageByID(sMsg)) != -1
			&& ReceiverObj.IsCommsMessageValidFrom(m_LeaderObj, iIndex))
		ReceiverObj.CommsMessageFrom(&m_LeaderObj, iIndex);
	}

bool CSquadronCommunications::Send (CSpaceObject &ReceiverObj, const SSquadronMsgDesc &MsgDesc, DWORD dwFormationPos) const

//	Send
//
//	Sends the message

	{
	static constexpr DWORD DEFAULT_FORMATION_PARAM = 0xffffffff;
	static constexpr DWORD FORMATION_ALPHA_INDEX = 0;
	static constexpr DWORD FORMATION_BETA_INDEX = 1;
	static constexpr DWORD FORMATION_GAMMA_INDEX = 2;

	int iIndex;
	if (IsMessageValidFor(ReceiverObj, MsgDesc, &iIndex))
		{
		ReceiverObj.CommsMessageFrom(&m_LeaderObj, iIndex);
		return true;
		}
	else
		{
		switch (MsgDesc.iMsg)
			{
			case ESquadronMsg::Attack:
				{
				CSpaceObject *pTarget = m_LeaderObj.GetTarget(IShipController::FLAG_ACTUAL_TARGET);
				if (pTarget)
					{
					DWORD dwRes = m_LeaderObj.Communicate(&ReceiverObj, MsgDesc.iSend, pTarget);
					return (dwRes == resAck);
					}
				else if (m_LeaderObj.IsPlayer())
					{
					CString sText = GetUniverse().TranslateEngineText(CONSTLIT("msgNoTargetForFleet"));
					m_LeaderObj.SendMessage(&ReceiverObj, sText);
					return false;
					}
				else
					{
					return false;
					}
				}

			case ESquadronMsg::FormUp:
				{
				DWORD dwRes = m_LeaderObj.Communicate(&ReceiverObj, MsgDesc.iSend, &m_LeaderObj, DEFAULT_FORMATION_PARAM);
				return (dwRes == resAck);
				}

			case ESquadronMsg::FormationAlpha:
				{
				DWORD dwFormation = MAKELONG(dwFormationPos, FORMATION_ALPHA_INDEX);
				DWORD dwRes = m_LeaderObj.Communicate(&ReceiverObj, MsgDesc.iSend, &m_LeaderObj, dwFormation);
				return (dwRes == resAck);
				}
				
			case ESquadronMsg::FormationBeta:
				{
				DWORD dwFormation = MAKELONG(dwFormationPos, FORMATION_BETA_INDEX);
				DWORD dwRes = m_LeaderObj.Communicate(&ReceiverObj, MsgDesc.iSend, &m_LeaderObj, dwFormation);
				return (dwRes == resAck);
				}
				
			case ESquadronMsg::FormationGamma:
				{
				DWORD dwFormation = MAKELONG(dwFormationPos, FORMATION_GAMMA_INDEX);
				DWORD dwRes = m_LeaderObj.Communicate(&ReceiverObj, MsgDesc.iSend, &m_LeaderObj, dwFormation);
				return (dwRes == resAck);
				}
				
			default:
				{
				DWORD dwRes = m_LeaderObj.Communicate(&ReceiverObj, MsgDesc.iSend);
				return (dwRes == resAck);
				}
			}
		}
	}

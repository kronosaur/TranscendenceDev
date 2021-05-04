//	CReactionImpl.cpp
//
//	CReactionImpl class
//	Copyright (c) 2021 Kronosaur Productions. All Rights Reserved.

#include "PreComp.h"

void CReactionImpl::Init (const COrderDesc &OrderDesc)

//	Init
//
//	Initializes from order desc.

	{
	m_iReactToAttack = InitFromOrderDesc(OrderDesc, CONSTLIT("reactToAttack"), m_iReactToAttack);
	m_iReactToBaseDestroyed = InitFromOrderDesc(OrderDesc, CONSTLIT("reactToBaseDestroyed"), m_iReactToBaseDestroyed);
	m_iReactToThreat = InitFromOrderDesc(OrderDesc, CONSTLIT("reactToThreat"), m_iReactToThreat);
	}

AIReaction CReactionImpl::InitFromOrderDesc (const COrderDesc &OrderDesc, const CString &sField, AIReaction iDefault)

//	InitFromOrderDesc
//
//	Returns the reaction from order desc (or default).

	{
	CString sValue = OrderDesc.GetDataString(sField);
	if (sValue.IsBlank())
		return iDefault;

	AIReaction iReaction = CAISettings::ConvertToAIReaction(sValue);
	if (iReaction == AIReaction::Default)
		return iDefault;
	else
		return iReaction;
	}

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

	m_rThreatRange = OrderDesc.GetDataDouble(CONSTLIT("threatRange"), m_rThreatRange);
	m_rThreatStopRange = OrderDesc.GetDataDouble(CONSTLIT("threatStopRange"), m_rThreatStopRange);
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

void CReactionImpl::SetOptions (ICCItem &Data) const

//	SetOptions
//
//	Stores are reactions in a data block.

	{
	Data.SetStringAt(CONSTLIT("reactToAttack"), CAISettings::ConvertToID(m_iReactToAttack));
	Data.SetStringAt(CONSTLIT("reactToBaseDestroyed"), CAISettings::ConvertToID(m_iReactToBaseDestroyed));
	Data.SetStringAt(CONSTLIT("reactToThreat"), CAISettings::ConvertToID(m_iReactToThreat));

	Data.SetDoubleAt(CONSTLIT("threatRange"), m_rThreatRange);
	Data.SetDoubleAt(CONSTLIT("threatStopRange"), m_rThreatStopRange);
	}
